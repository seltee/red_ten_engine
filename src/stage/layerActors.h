// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "common/utils.h"
#include "stage/layer.h"
#include "actor/actor.h"
#include "os/view.h"
#include "camera/camera.h"
#include "math/math.h"
#include "physics/physicsWorld.h"
#include "connector/withProfiler.h"
#include "renderer/renderer.h"
#include <list>

class LayerActors : public Layer,
                    public WithDebug,
                    public WithProfiler
{
public:
    LayerActors(std::string name, int index);

    template <class T, typename std::enable_if<std::is_base_of<Actor, T>::value>::type * = nullptr>
    EXPORT T *createActor()
    {
        auto newActor = new T();
        prepareNewActor(newActor);
        return newActor;
    }

    EXPORT void process(float delta) override;
    EXPORT void render(Renderer* renderer, RenderTarget *renderTarget) override;
    EXPORT void prepareNewActor(Actor *actor);
    EXPORT void enablePhisics(const Vector3 &gravity, float simScale = 1.0f, int stepsPerSecond = 100);
    EXPORT void enableSorting();
    EXPORT void disableSorting();

    EXPORT bool castRaySingleCollision(const Segment &ray, PhysicsBodyPoint &resultPoint, bool viewDebugLine = false, float showTime = 1.6f);
    EXPORT std::vector<PhysicsBodyPoint> castRayCollision(const Segment &ray, bool viewDebugLine = false, float showTime = 1.6f);
    EXPORT std::list<PhysicsBodyPoint> castSphereCollision(const Vector3 &p, float radius);
    EXPORT std::list<PhysicsBodyPoint> castPointCollision(const Vector3 &p);

    EXPORT std::list<Actor *> *getActorsList();
    EXPORT std::list<Actor *> getActorsByName(std::string name);
    EXPORT std::list<Actor *> getActorsByPartName(std::string partOfName);
    EXPORT static LayerActors *getActorsLayer(Actor *actor);
    EXPORT void clear();

    EXPORT void setVisible(bool state);
    EXPORT bool isVisible();

    EXPORT void setProcessingEnabled(bool state);
    EXPORT bool isProcessingEnabled();

    EXPORT Camera *getActiveCamera();
    EXPORT void setActiveCamera(Camera *activeCamera);

    EXPORT void setAmbientColor(float r, float g, float b);

    EXPORT void setHDRTextures(Texture *HDRTexture, Texture *HDRRadianceTexture = nullptr);
    EXPORT void setHDRRotation(float r);
    EXPORT void setHDREnvVisibility(bool state);

    EXPORT PhysicsWorld *getPhysicsWorld();

    EXPORT void showDebugLine(const Segment &ray, const Vector3 &color, float showTime);
    EXPORT void showDebugBox(const Vector3 &p, float size, const Vector3 &color, float showTime);

    void inline setGamma(float gamma) { this->gamma = gamma; }
    float inline getGamma() { return gamma; }

protected:
    bool bIsVisible = true;
    bool bProcessingEnabled = true;
    Vector3 ambientColor = Vector3(1.0f);

    std::list<Actor *> actors;
    PhysicsWorld *physicsWorld = nullptr;
    bool bUseSorting = false;
    Camera *activeCamera = nullptr;

    int renderingTrackerId = 0;
    int processingTrackerId = 0;
    int physicsTrackerId = 0;

    Texture *HDRTexture = nullptr;
    Texture *HDRRadianceTexture = nullptr;

    float HDRRotation = 0.0f;
    bool HDREnvVisibility = true;

    float gamma = 1.0f;
};
