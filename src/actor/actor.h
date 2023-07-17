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
    EXPORT virtual void onRender(Matrix4 &vpMatrix, std::vector<Component *> *lights);
    EXPORT virtual void onRenderShadowed(Matrix4 &vpMatrix);
    EXPORT virtual void onRenderBlended(Matrix4 &vpMatrix);
    EXPORT virtual void onDestroy();
    EXPORT virtual void onCollide(Actor *hitWith, Vector3 v);
    EXPORT virtual void onCollidePersisted(Actor *hitWith, Vector3 v);

    EXPORT inline bool hasBlended() { return bHasBlended; };

    EXPORT inline bool hasDebugView() { return bShowBoundingBox; }
    EXPORT inline void showBoundingBox(bool state) { bShowBoundingBox = state; }

    EXPORT void childUpdated();

protected:
    void updatePhysics();

    bool bIsZAxisLocked = false;
    bool bIsZRotationLocked = false;
    bool bIsVisible = true;
    bool bHasBlended = false;
    bool bPhysicsNeedsToBeRebuild = false;
    bool bShowBoundingBox = false;

    MotionType motionType = MotionType::Static;
    PhysicsWorld *physicsWorld = nullptr;
    PhysicsBody *physicsBody = nullptr;

    float restitution = 0.5f;
    float friction = 1.0f;
    float zLockedPosition = 0.0f;

    std::list<Component *> components;
    std::string name = "actor";
};
