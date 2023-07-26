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

LayerActors::LayerActors(std::string name, int index) : Layer(name, index) {}

bool compareBodyPoints(PhysicsBodyPoint a, PhysicsBodyPoint b)
{
    return (a.distance < b.distance);
}

void LayerActors::process(float delta)
{
    if (!bProcessingEnabled)
        return;

    for (auto actor = actors.begin(); actor != actors.end(); ++actor)
        (*actor)->process(delta);

    if (physicsWorld)
        physicsWorld->process(delta);

    auto actor = actors.begin();
    while (actor != actors.end())
        if ((*actor)->isDestroyed())
        {
            delete (*actor);
            actor = actors.erase(actor);
        }
        else
            ++actor;

    if (physicsWorld)
        physicsWorld->removeDestroyed();

    if (bUseSorting)
    {
        actors.sort([](const Actor *player1, const Actor *player2)
                    { return player1->zDepth < player2->zDepth; });
    }
}

void LayerActors::render(View *view)
{
    Matrix4 m1, m2;
    auto renderer = view->getRenderer();
    // Clear light renderer
    renderer->setupLightning();

    // Clear deffered buffer and set this as primary
    renderer->setupNewFrame();

    std::vector<Component *> sceneLights;
    std::vector<Actor *> blends;
    std::vector<Actor *> shadowCasters;
    std::vector<Actor *> debugView;

    if (!activeCamera || !bIsVisible)
        return;

    Matrix4 mView = *activeCamera->getViewMatrix();
    Matrix4 mProjectionView = *activeCamera->getProjectionMatrix() * mView;

    // G Buffer phase
    activeCamera->prepareToRender(view);

    for (auto actor = actors.begin(); actor != actors.end(); ++actor)
    {
        if ((*actor)->isVisible())
        {
            (*actor)->onRender(mProjectionView, &sceneLights);
            if ((*actor)->hasBlended())
                blends.push_back(*actor);
            shadowCasters.push_back(*actor);
        }
        if ((*actor)->hasDebugView())
        {
            debugView.push_back(*actor);
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
    renderer->setupLightning(false);
    initialLightningShader->use(m1, m2);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, renderer->getAlbedoTexture());
    glUniform3fv(initialLightningShader->locV3AmbientColor, 1, ambientColor);
    CommonShaders::getScreenMesh()->use();

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
                if (renderer->getShadowMapSize() < 2048)
                    glFrontFace(GL_CW);
                glDisable(GL_BLEND);

                // prepare shadowed render
                Matrix4 mLightViewProjection = (*light)->preparePreShadowPhase(activeCamera->getOwnerTransform()->getPosition());
                renderer->setupShadowHQ();

                for (auto actor = shadowCasters.begin(); actor != shadowCasters.end(); ++actor)
                {
                    (*actor)->onRenderShadowed(mLightViewProjection);
                }
                setupLightningFrame = true;

                if (renderer->getShadowMapSize() < 2048)
                    glFrontFace(GL_CCW);
            }
            if (setupLightningFrame)
            {
                setupLightningFrame = false;
                renderer->setupLightning(false);

                glDisable(GL_DEPTH_TEST);
                glDisable(GL_CULL_FACE);
                glEnable(GL_BLEND);

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, renderer->getAlbedoTexture());

                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, renderer->getNormalTexture());

                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, renderer->getPositionTexture());
            }

            (*light)->renderLightPhase(mProjectionView, renderer->getShadowTexture());
        }
    }

    // Blending phase
    renderer->setupLightning(false);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);

    if (blends.size() > 0)
    {
        for (auto actor = blends.begin(); actor != blends.end(); ++actor)
        {
            (*actor)->onRenderBlended(mProjectionView);
        }
    }

    // Debug render
    if (debugView.size() > 0 && physicsWorld && activeCamera)
    {
        for (auto actor = debugView.begin(); actor != debugView.end(); ++actor)
        {
            auto body = (*actor)->getPhysicsBody();
            if (body)
            {
                debug->renderBoundingBox(body->getAABB(), &mProjectionView, physicsWorld->getSimScale(), activeCamera->getLineThickness(), Vector3(0.2f, 0.2f, 0.2f));
                Shape *shape = body->getShape();
                if (shape)
                {
                    shape->renderDebug(&mProjectionView, (*actor)->transform.getModelMatrix(), 1.0f / physicsWorld->getSimScale(), activeCamera->getLineThickness());
                }
            }
        }
    }
    debug->renderAll(&mProjectionView);

    // Final phase
    view->useFrameBuffer();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, renderer->getLightningTexture());
    CommonShaders::getScreenShader()->use(m1, m2);
    CommonShaders::getScreenMesh()->use();

    glDrawArrays(GL_TRIANGLES, 0, 6);
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

void LayerActors::enablePhisics(Vector3 gravity, float simScale, int stepsPerSecond)
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
}

void LayerActors::enableSorting()
{
    bUseSorting = true;
}

void LayerActors::disableSorting()
{
    bUseSorting = false;
}

bool LayerActors::castRaySingleCollision(Line ray, PhysicsBodyPoint &resultPoint, bool viewDebugLine, float showTime)
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

std::vector<PhysicsBodyPoint> LayerActors::castRayCollision(Line ray, bool viewDebugLine, float showTime)
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

std::list<PhysicsBodyPoint> LayerActors::castSphereCollision(Vector3 p, float radius)
{
    std::list<PhysicsBodyPoint> list;
    return list;
}

std::list<PhysicsBodyPoint> LayerActors::castPointCollision(Vector3 p)
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

void LayerActors::clear(bool destroyCameras)
{
    if (actors.size() > 0)
    {
        for (auto actor = actors.begin(); actor != actors.end(); ++actor)
        {
            if ((!destroyCameras && !(*actor)->implements("Camera")) || (destroyCameras))
            {
                (*actor)->destroy();
            }
        }
    }
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

void LayerActors::showDebugLine(Line ray, Vector3 color, float showTime)
{
    float thickness = activeCamera->getLineThickness();
    debug->addDebugLine(ray.a, ray.b, showTime, thickness, color);
}

void LayerActors::showDebugBox(Vector3 p, float size, Vector3 color, float showTime)
{
    size *= activeCamera->getLineThickness();
    debug->addDebugBox(p, size, showTime, color);
}