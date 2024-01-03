// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#pragma once
#include "actor/watchable.h"
#include "common/utils.h"
#include "common/destroyable.h"
#include "common/entity.h"
#include "connector/withDebug.h"
#include "connector/withLogger.h"
#include "math/math.h"
#include "math/transformation.h"
#include "component/component.h"
#include "physics/shapes/shape.h"
#include "physics/physicsWorld.h"
#include "renderer/renderQueue.h"
#include "core/safePointer.h"
#include <string>
#include <list>
#include <vector>

class Actor : public Entity, public Watchable<Actor>, public Destroyable, public WithDebug, public WithLogger
{
public:
    EXPORT Actor();
    EXPORT virtual ~Actor();

    EXPORT void setActorName(std::string name);
    EXPORT const std::string getActorName();

    EXPORT void setPhysicsMotionType(MotionType motionType);
    EXPORT void markPhysicsUpdateNeeded();
    EXPORT std::list<Actor *> *getLayerActorsList();

    template <class T, typename std::enable_if<std::is_base_of<Component, T>::value>::type * = nullptr>
    EXPORT T *createComponent()
    {
        auto newComonent = new T();
        prepareNewComponent(newComonent);
        return newComonent;
    }
    EXPORT void removeComponents();
    EXPORT void removeComponent(Component *component);
    EXPORT void prepareNewComponent(Component *component);
    EXPORT void setPhysicsWorld(PhysicsWorld *physicsWorld);
    EXPORT PhysicsWorld *getPhysicsWorld();
    EXPORT void setZAxisLocked(bool state);
    EXPORT void setZAxisRotationLocked(bool state);

    EXPORT void process(float delta);

    EXPORT void setRestitution(float newValue);
    EXPORT float getRestitution();
    EXPORT void setFriction(float newValue);
    EXPORT float getFriction();
    EXPORT void setFrictionAndRestitution(float newFrictionValue, float newRestitutionValue);

    EXPORT PhysicsBody *getPhysicsBody();

    EXPORT virtual void onSpawned();
    EXPORT virtual void onProcess(float delta);
    EXPORT virtual void onRenderQueue(RenderQueue *renderQueue);

    EXPORT virtual void onDestroy();
    EXPORT virtual void onCollide(Actor *hitWith, Vector3 v);
    EXPORT virtual void onCollidePersisted(Actor *hitWith, Vector3 v);
    EXPORT virtual void onCollideStopped(Actor *hitWith);

    EXPORT inline bool hasDebugView() { return bShowBoundingBox || bShowNormals; }
    EXPORT inline void showBoundingBox(bool state) { bShowBoundingBox = state; }
    EXPORT inline void showNormals(bool state) { bShowNormals = state; }

    inline bool isBoundingBoxShown() { return bShowBoundingBox; }
    inline bool isNormalsShown() { return bShowNormals; }

    EXPORT void childUpdated();

    inline const std::list<Component *> *getComponents() { return &components; }

protected:
    void updatePhysics();

    bool bIsZAxisLocked = false;
    bool bIsZRotationLocked = false;
    bool bIsVisible = true;
    bool bPhysicsNeedsToBeRebuild = false;
    bool bShowBoundingBox = false;
    bool bShowNormals = false;

    MotionType motionType = MotionType::Static;
    PhysicsWorld *physicsWorld = nullptr;
    PhysicsBody *physicsBody = nullptr;

    float restitution = 0.5f;
    float friction = 1.0f;
    float zLockedPosition = 0.0f;

    std::list<Component *> components;
    std::string name = "actor";
};
