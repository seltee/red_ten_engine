// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "actor/actor.h"
#include "stage/layerActors.h"

Actor::Actor()
{
    registerClassName("Actor");
}

Actor::~Actor()
{
    removeComponents();
    if (physicsBody){
        physicsBody->clearOwner();
    }
}

void Actor::setActorName(std::string name)
{
    this->name = name;
}

const std::string Actor::getActorName()
{
    return this->name;
}

void Actor::setPhysicsMotionType(MotionType motionType)
{
    if (this->motionType != motionType)
    {
        this->motionType = motionType;
        bPhysicsNeedsToBeRebuild = true;
    }
}

void Actor::markPhysicsUpdateNeeded()
{
    bPhysicsNeedsToBeRebuild = true;
}

std::list<Actor *> *Actor::getLayerActorsList()
{
    if (layer)
        return layer->getActorsList();
    return nullptr;
}

void Actor::removeComponents()
{
    for (auto it = components.begin(); it != components.end(); it++)
    {
        delete (*it);
    }
    components.clear();
    bPhysicsNeedsToBeRebuild = true;
}

void Actor::removeComponent(Component *component)
{
    for (auto it = components.begin(); it != components.end(); it++)
    {
        if (*it == component)
        {
            delete *it;
        }
        return;
    }
}

void Actor::prepareNewComponent(Component *component)
{
    this->components.push_back(component);
    component->prepare(this);
    bPhysicsNeedsToBeRebuild = true;
}

void Actor::setPhysicsWorld(PhysicsWorld *physicsWorld)
{
    this->physicsWorld = physicsWorld;
    bPhysicsNeedsToBeRebuild = true;
}

PhysicsWorld *Actor::getPhysicsWorld()
{
    return physicsWorld;
}

void Actor::setZAxisLocked(bool state)
{
    bIsZAxisLocked = state;
    bPhysicsNeedsToBeRebuild = true;
}

void Actor::setZAxisRotationLocked(bool state)
{
    bIsZRotationLocked = state;
    bPhysicsNeedsToBeRebuild = true;
}

void Actor::process(float delta)
{
    onProcess(delta);
    if (components.size() > 0)
    {
        auto it = components.begin();
        while (it != components.end())
        {
            (*it)->process(delta);
            if ((*it)->isDestroyed())
            {
                delete (*it);
                it = components.erase(it);
                bPhysicsNeedsToBeRebuild = true;
            }
            else
                ++it;
        }
    }
    if (bPhysicsNeedsToBeRebuild)
        updatePhysics();
}

void Actor::setRestitution(float newValue)
{
    this->restitution = newValue;
    bPhysicsNeedsToBeRebuild = true;
}

float Actor::getRestitution()
{
    return restitution;
}

void Actor::setFriction(float newValue)
{
    this->friction = newValue;
    bPhysicsNeedsToBeRebuild = true;
}

float Actor::getFriction()
{
    return friction;
}

void Actor::setFrictionAndRestitution(float newFrictionValue, float newRestitutionValue)
{
    this->friction = newFrictionValue;
    this->restitution = newRestitutionValue;
    bPhysicsNeedsToBeRebuild = true;
}

PhysicsBody *Actor::getPhysicsBody()
{
    if (bPhysicsNeedsToBeRebuild)
        updatePhysics();
    return physicsBody;
}

void Actor::onSpawned()
{
    updatePhysics();
}

void Actor::onProcess(float delta)
{
}

void Actor::onRender(Matrix4 &vpMatrix, std::vector<Component *> *lights)
{
    bHasBlended = false;
    if (components.size() > 0)
        for (auto it = components.begin(); it != components.end(); it++)
            if ((*it)->isVisible())
            {
                if ((*it)->isUsingBlendingPhase())
                    bHasBlended = true;
                else
                    (*it)->onRender(vpMatrix, &transform);

                if ((*it)->isUsingLightPhase())
                {
                    lights->push_back((*it));
                }
            }
}

void Actor::onRenderShadowed(Matrix4 &vpMatrix)
{
    if (components.size() > 0)
        for (auto it = components.begin(); it != components.end(); it++)
            if ((*it)->isVisible() && !(*it)->isUsingBlendingPhase())
                (*it)->onRenderShadow(vpMatrix, &transform);
}

void Actor::onRenderBlended(Matrix4 &vpMatrix)
{
    if (components.size() > 0)
        for (auto it = components.begin(); it != components.end(); it++)
            if ((*it)->isVisible() && (*it)->isUsingBlendingPhase())
                (*it)->onRender(vpMatrix, &transform);
}

void Actor::onDestroy()
{
    Destroyable::onDestroy();

    if (components.size() > 0)
        for (auto it = components.begin(); it != components.end(); it++)
            (*it)->destroy();

    if (physicsBody)
        physicsBody->destroy();
}

void Actor::onCollide(Actor *hitWith, Vector3 v)
{
}

void Actor::onCollidePersisted(Actor *hitWith, Vector3 v)
{
}

void Actor::onCollideStopped(Actor *hitWith)
{
}

void Actor::childUpdated()
{
    bPhysicsNeedsToBeRebuild = true;
}

void Actor::updatePhysics()
{
    bPhysicsNeedsToBeRebuild = false;

    if (physicsWorld && components.size() > 0)
    {
        int collisionsAmount = 0;
        for (auto component = components.begin(); component != components.end(); component++)
            collisionsAmount += (*component)->shapes.size();

        if (collisionsAmount > 0)
        {
            if (physicsBody)
            {
                physicsBody->destroy();
                physicsBody = nullptr;
            }

            std::vector<Shape *> shapesList;
            for (auto component = components.begin(); component != components.end(); component++)
            {
                auto shapes = &(*component)->shapes;
                if (shapes->size() > 0)
                    for (auto shape = shapes->begin(); shape != shapes->end(); shape++)
                        shapesList.push_back(*shape);
            }

            if (shapesList.size() > 0)
            {
                physicsBody = physicsWorld->createPhysicsBody(shapesList.at(0), this);
                physicsBody->setRelation(&transform, this);
                physicsBody->setRestitution(restitution);
                physicsBody->setFriction(friction);
                physicsBody->getShape()->setDebugName(this->getActorName());

                if (motionType == MotionType::Dynamic)
                {
                    physicsBody->setDynamicMotionType();

                    if (bIsZAxisLocked || bIsZRotationLocked)
                    {
                        physicsBody->addConstraint6DOF({bIsZAxisLocked,
                                                        bIsZAxisLocked,
                                                        bIsZRotationLocked,
                                                        false,
                                                        false,
                                                        bIsZAxisLocked});
                    }
                }

                if (motionType == MotionType::Static)
                    physicsBody->setStaticMotionType();
            }
        }
    }
}
