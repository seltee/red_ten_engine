// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "stage/layerActors.h"
#include "math/math.h"
#include "actor/actorGUIElement.h"
#include "opengl/glew.h"
#include "common/commonShaders.h"
#include "component/componentLight.h"
#include <math.h>
#include <algorithm>

LayerActors::LayerActors(std::string name, int index) : Layer(name, index)
{
    renderingTrackerId = profiler->addTracker("layer actors \"" + name + "\" rendering");
    processingTrackerId = profiler->addTracker("layer actors \"" + name + "\" processing");
    physicsTrackerId = profiler->addTracker("layer actors \"" + name + "\" physics");
    profiler->setInactive(physicsTrackerId);
}

bool compareBodyPoints(PhysicsBodyPoint a, PhysicsBodyPoint b)
{
    return (a.distance < b.distance);
}

void LayerActors::process(float delta)
{
    if (!bProcessingEnabled)
        return;

    profiler->startTracking(processingTrackerId);

    for (auto actor = actors.begin(); actor != actors.end(); ++actor)
        (*actor)->process(delta);

    profiler->startTracking(physicsTrackerId);
    if (physicsWorld)
        physicsWorld->process(delta);
    profiler->stopTracking(physicsTrackerId);

    auto actor = actors.begin();
    while (actor != actors.end())
        if ((*actor)->isDestroyed())
        {
            delete (*actor);
            actor = actors.erase(actor);
        }
        else
            ++actor;

    profiler->startTracking(physicsTrackerId);
    if (physicsWorld)
        physicsWorld->removeDestroyed();
    profiler->stopTracking(physicsTrackerId);

    if (bUseSorting)
    {
        actors.sort([](const Actor *player1, const Actor *player2)
                    { return player1->zDepth < player2->zDepth; });
    }

    profiler->stopTracking(processingTrackerId);
}

void LayerActors::render(RenderTarget *renderTarget)
{
    profiler->startTracking(renderingTrackerId);

    Matrix4 m1, m2;
    // Clear light renderer
    renderTarget->setupLightning();

    // Clear deffered buffer and set this as primary
    renderTarget->setupNewFrame();

    std::vector<Component *> sceneLights;
    std::vector<Actor *> blends;
    std::vector<Actor *> shadowCasters;
    std::vector<Actor *> debugView;

    if (!activeCamera || !bIsVisible)
        return;

    Matrix4 mView = *activeCamera->getViewMatrix();
    Matrix4 mProjectionView = *activeCamera->getProjectionMatrix() * mView;

    // G Buffer phase
    activeCamera->prepareToRender(renderTarget);

    for (auto &actor : actors)
    {
        if (actor->isVisible())
        {
            actor->onRender(mProjectionView, &sceneLights);
            if (actor->hasBlended())
                blends.push_back(actor);
            shadowCasters.push_back(actor);
        }
        if (actor->hasDebugView())
        {
            debugView.push_back(actor);
        }
    }

    activeCamera->finishRender();

    // light phase
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_ALPHA_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);

    auto initialLightningShader = CommonShaders::getInitialLightningShader();
    renderTarget->setupLightning(false);
    initialLightningShader->use(m1, m2);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, renderTarget->getAlbedoTexture());
    glUniform3fv(initialLightningShader->locV3AmbientColor, 1, ambientColor);
    CommonShaders::getScreenMesh()->useVertexArray();

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Lightning phase
    if (sceneLights.size() > 0)
    {
        bool setupLightningFrame = true;
        for (auto light = sceneLights.begin(); light != sceneLights.end(); ++light)
        {
            // shadow preparation phase
            if ((*light)->isUsingShadowPhase())
            {
                glEnable(GL_DEPTH_TEST);
                glEnable(GL_CULL_FACE);
                if (renderTarget->getShadowMapSize() < 2048)
                    glFrontFace(GL_CW);
                glDisable(GL_BLEND);

                // prepare shadowed render
                Matrix4 mLightViewProjection = (*light)->preparePreShadowPhase(activeCamera->getOwnerTransform()->getPosition());
                renderTarget->setupShadowHQ();

                for (auto actor = shadowCasters.begin(); actor != shadowCasters.end(); ++actor)
                {
                    (*actor)->onRenderShadowed(mLightViewProjection);
                }
                setupLightningFrame = true;

                if (renderTarget->getShadowMapSize() < 2048)
                    glFrontFace(GL_CCW);
            }
            if (setupLightningFrame)
            {
                setupLightningFrame = false;
                renderTarget->setupLightning(false);

                glDisable(GL_DEPTH_TEST);
                glDisable(GL_CULL_FACE);
                glEnable(GL_BLEND);

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, renderTarget->getAlbedoTexture());

                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, renderTarget->getNormalTexture());

                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, renderTarget->getPositionTexture());
            }

            (*light)->renderLightPhase(mProjectionView, renderTarget->getShadowTexture(), activeCamera);
        }
    }

    // Blending phase
    renderTarget->setupLightning(false);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);

    if (blends.size() > 0)
    {
        for (auto &actor : blends)
        {
            actor->onRenderBlended(mProjectionView);
        }
    }

    // Debug render
    if (debugView.size() > 0 && activeCamera)
    {
        for (auto &actor : debugView)
        {
            if (physicsWorld && actor->isBoundingBoxShown())
            {
                auto body = actor->getPhysicsBody();
                if (body)
                {
                    debug->renderBoundingBox(body->getAABB(), &mProjectionView, physicsWorld->getSimScale(), activeCamera->getLineThickness(), Vector3(0.2f, 0.2f, 0.2f));
                    Shape *shape = body->getShape();
                    if (shape)
                    {
                        shape->renderDebug(&mProjectionView, actor->transform.getModelMatrix(), 1.0f / physicsWorld->getSimScale(), activeCamera->getLineThickness());
                    }
                }
            }

            // TOFO: move normal rendering to components
            if (actor->isNormalsShown())
            {
                for (auto &component : *actor->getComponents())
                {
                    auto mesh = component->getStaticMesh();
                    if (mesh)
                    {
                        auto vAmount = mesh->getVertexAmount();
                        auto floatsPerVertex = mesh->getFloatsPerVertex();
                        auto data = mesh->getVertexData();

                        for (int i = 0; i < vAmount; i++)
                        {
                            int shift = i * floatsPerVertex;
                            Vector4 v = Vector4(data[shift + 0], data[shift + 1], data[shift + 2], 1.0f);
                            Vector3 n = Vector3(data[shift + 3], data[shift + 4], data[shift + 5]);

                            Vector4 vts4 = *actor->transform.getModelMatrix() * *component->transform.getModelMatrix() * v;

                            Vector3 vts3 = Vector3(vts4.x, vts4.y, vts4.z);

                            Vector3 nr = glm::rotate(actor->transform.getRotation() * component->transform.getRotation(), n);

                            debug->renderLine(vts3, vts3 + nr * 0.1f, &mProjectionView, 0.01f, Vector3(0.2f, 0.9f, 0.2f));
                        }
                    }
                }
            }
        }
    }
    debug->renderAll(&mProjectionView);

    // Final phase
    renderTarget->useResultBuffer();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, renderTarget->getLightningTexture());
    CommonShaders::getScreenShader()->use(m1, m2);
    CommonShaders::getScreenMesh()->useVertexArray();

    glDrawArrays(GL_TRIANGLES, 0, 6);

    profiler->stopTracking(renderingTrackerId);
}

void LayerActors::prepareNewActor(Actor *actor)
{
    if (physicsWorld)
        actor->setPhysicsWorld(physicsWorld);

    actors.push_back(actor);
    actor->setCurrentLayer(this);
    actor->onSpawned();
    actor->getPhysicsBody();
}

void LayerActors::enablePhisics(const Vector3 &gravity, float simScale, int stepsPerSecond)
{
    if (!physicsWorld)
    {
        physicsWorld = new PhysicsWorld(gravity, simScale, stepsPerSecond);
        for (auto actor = actors.begin(); actor != actors.end(); ++actor)
            (*actor)->setPhysicsWorld(physicsWorld);
    }
    else
    {
        physicsWorld->setBasicParameters(gravity, simScale, stepsPerSecond);
    }

    profiler->setActive(physicsTrackerId);
}

void LayerActors::enableSorting()
{
    bUseSorting = true;
}

void LayerActors::disableSorting()
{
    bUseSorting = false;
}

bool LayerActors::castRaySingleCollision(const Segment &ray, PhysicsBodyPoint &resultPoint, bool viewDebugLine, float showTime)
{
    if (viewDebugLine)
    {
        showDebugLine(ray, Vector3({0.0f, 1.0f, 0.0f}), showTime);
    }

    if (physicsWorld)
    {
        auto result = physicsWorld->castRay(ray);
        resultPoint.distance = 99999.9f;

        for (auto point = result.begin(); point != result.end(); ++point)
        {
            if (viewDebugLine)
                showDebugBox(point->point, 4.0f, Vector3({1.0f, 0.0f, 0.0f}), showTime);
            if (point->distance < resultPoint.distance)
                resultPoint = *point;
        }
        return result.size() > 0;
    }

    return false;
}

std::vector<PhysicsBodyPoint> LayerActors::castRayCollision(const Segment &ray, bool viewDebugLine, float showTime)
{
    if (viewDebugLine)
    {
        showDebugLine(ray, Vector3({0.0f, 1.0f, 0.0f}), showTime);
    }

    if (physicsWorld)
    {
        auto result = physicsWorld->castRay(ray);
        if (result.size() > 0)
        {
            std::sort(result.begin(), result.end(), compareBodyPoints);

            if (viewDebugLine)
                for (auto point = result.begin(); point != result.end(); ++point)
                    showDebugBox(point->point, 4.0f, Vector3({1.0f, 0.0f, 0.0f}), showTime);
        }

        return result;
    }

    std::vector<PhysicsBodyPoint> list;
    return list;
}

std::list<PhysicsBodyPoint> LayerActors::castSphereCollision(const Vector3 &p, float radius)
{
    std::list<PhysicsBodyPoint> list;
    return list;
}

std::list<PhysicsBodyPoint> LayerActors::castPointCollision(const Vector3 &p)
{
    std::list<PhysicsBodyPoint> list;
    return list;
}

std::list<Actor *> *LayerActors::getActorsList()
{
    return &actors;
}

std::list<Actor *> LayerActors::getActorsByName(std::string name)
{
    std::list<Actor *> list;

    if (actors.size() > 0)
    {
        for (auto actor = actors.begin(); actor != actors.end(); ++actor)
        {
            if ((*actor)->getActorName() == name)
            {
                list.push_back(*actor);
            }
        }
    }

    return list;
}

std::list<Actor *> LayerActors::getActorsByPartName(std::string partOfName)
{
    std::list<Actor *> list;

    if (actors.size() > 0)
    {
        for (auto actor = actors.begin(); actor != actors.end(); ++actor)
        {
            if ((*actor)->getActorName().find(name) != std::string::npos)
            {
                list.push_back(*actor);
            }
        }
    }

    return list;
}

LayerActors *LayerActors::getActorsLayer(Actor *actor)
{
    return (LayerActors *)actor->getCurrentLayer();
}

void LayerActors::clear()
{
    if (actors.size() > 0)
        for (auto actor = actors.begin(); actor != actors.end(); ++actor)
            (*actor)->destroy();
}

void LayerActors::setVisible(bool state)
{
    bIsVisible = state;
}

bool LayerActors::isVisible()
{
    return bIsVisible;
}

void LayerActors::setProcessingEnabled(bool state)
{
    bProcessingEnabled = state;
}
bool LayerActors::isProcessingEnabled()
{
    return bProcessingEnabled;
}

Camera *LayerActors::getActiveCamera()
{
    return activeCamera;
}

void LayerActors::setActiveCamera(Camera *activeCamera)
{
    this->activeCamera = activeCamera;
}

void LayerActors::setAmbientColor(float r, float g, float b)
{
    ambientColor[0] = r;
    ambientColor[1] = g;
    ambientColor[2] = b;
}

PhysicsWorld *LayerActors::getPhysicsWorld()
{
    return physicsWorld;
}

void LayerActors::showDebugLine(const Segment &ray, const Vector3 &color, float showTime)
{
    float thickness = activeCamera->getLineThickness();
    debug->addDebugLine(ray.a, ray.b, showTime, thickness, color);
}

void LayerActors::showDebugBox(const Vector3 &p, float size, const Vector3 &color, float showTime)
{
    size *= activeCamera->getLineThickness();
    debug->addDebugBox(p, size, showTime, color);
}