// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "stage/layerActors.h"
#include "math/math.h"
#include "math/glm/gtc/type_ptr.hpp"
#include "actor/actorGUIElement.h"
#include "component/componentLight.h"
#include "common/commonTextures.h"
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

void LayerActors::render(Renderer *renderer, RenderTarget *renderTarget)
{
    profiler->startTracking(renderingTrackerId);

    if (!activeCamera || !bIsVisible)
        return;

    // G Buffer phase
    activeCamera->prepareToRender(renderTarget);

    Matrix4 mView = *activeCamera->getViewMatrix();
    Matrix4 mProjectionView = *activeCamera->getProjectionMatrix() * mView;
    Vector4 cm4position = activeCamera->getWorldModelMatrix() * Vector4(0.0f, 0.0f, 0.0f, 1.0f);
    Vector3 cmPosition = Vector3(cm4position.x, cm4position.y, cm4position.z);

    RenderQueue *renderQueue = renderer->getRenderQueue();

    renderQueue->reset();
    renderQueue->setViewProjectionMatrix(mProjectionView);
    renderQueue->setAmbientLight(ambientColor);
    renderQueue->setCameraPosition(cmPosition);
    renderQueue->setHDRTexture(HDRTexture);
    renderQueue->setHDRRadianceTexture(HDRRadianceTexture);
    renderQueue->setGamma(gamma);

    if (bUseSorting)
        renderQueue->enableSorting();
    else
        renderQueue->disableSorting();

    renderQueue->setShowEnvHDR(HDRTexture && HDREnvVisibility);
    renderQueue->setEnvHDRRotation(HDRRotation);

    // Put elements to queues
    if (actors.size() > 0)
    {
        auto actorsList = this->actors;
        renderQueue->bDone = false;
        core->queueJob([actorsList, renderQueue]
                       {
                            for (auto &actor : actorsList)
                            {
                                if (actor->isVisible())
                                {
                                    actor->onRenderQueue(renderQueue);
                                }
                            }
                            renderQueue->bDone = true; });
    }

    // Render queue
    renderer->render(renderTarget);

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

void LayerActors::setHDRTextures(Texture *HDRTexture, Texture *HDRRadianceTexture)
{
    this->HDRTexture = HDRTexture;
    this->HDRRadianceTexture = HDRRadianceTexture;
}

void LayerActors::setHDRRotation(float r)
{
    this->HDRRotation = r;
}

void LayerActors::setHDREnvVisibility(bool state)
{
    HDREnvVisibility = state;
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
