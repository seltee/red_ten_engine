// SPDX-FileCopyrightText: 2023 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "physicsBody.h"
#include "actor/actor.h"

PhysicsBody::PhysicsBody(Shape *shape, float simScale)
{
    this->shape = shape;
    this->simScale = simScale;
}

PhysicsBody::~PhysicsBody()
{
    for (auto &body : bodyCollisionData)
    {
        body.foreignBody->notifyBodyRemoved(this);
    }
}

void PhysicsBody::prepareSteps()
{
    if (transformation)
    {
        Vector3 newPosition = transformation->getPosition() * simScale;
        Quat newOrientation = transformation->getRotation();

        if (newPosition.x != position.x || newPosition.y != position.y || newPosition.z != position.z ||
            newOrientation.x != orientation.x || newOrientation.y != orientation.y || newOrientation.z != orientation.z || newOrientation.w != orientation.w)
        {
            bIsSleeping = false;
            position = newPosition;
            orientation = newOrientation;

            localTransform = glm::translate(Matrix4(1.0f), Vector3(position.x, position.y, position.z));
            localTransform *= glm::toMat4(orientation);
            this->shape->provideTransformation(&localTransform);
        }
    }
}

void PhysicsBody::finishStep(float delta)
{
    if (glm::length2(translationAccumulator) > 0.0000000001f)
    {
        if (!constraints.empty())
            for (auto constraint = constraints.begin(); constraint != constraints.end(); constraint++)
                translationAccumulator = (*constraint)->processTranslation(translationAccumulator);

        position += translationAccumulator;
        translationAccumulator = Vector3(0.0f);
        bIsSleeping = false;
    }

    if (motion && !bIsSleeping && bIsEnabled)
    {
        motion->checkLimits();

        if (!constraints.empty())
            for (auto constraint = constraints.begin(); constraint != constraints.end(); constraint++)
                (*constraint)->processMotion(motion);

        position += motion->linearVelocity * delta;

        Vector3 angularVelocityDelta = motion->angularVelocity * delta;
        float len = glm::length(angularVelocityDelta);
        if (len > 1.0e-6f)
        {
            orientation = glm::normalize(glm::angleAxis(len, angularVelocityDelta / len) * orientation);
        }

        if (transformation)
        {
            transformation->setPosition(position / simScale);
            transformation->setRotation(orientation);
        }

        localTransform = glm::translate(Matrix4(1.0f), Vector3(position.x, position.y, position.z));
        localTransform *= glm::toMat4(orientation);
        this->shape->provideTransformation(&localTransform);

        if (glm::length2(motion->linearVelocity) < delta * 0.2f && glm::length2(motion->angularVelocity) < delta * 0.2f)
        {
            sleepAccumulator += delta;
            if (sleepAccumulator > 0.8f)
                setAsleep();
        }
        else
            sleepAccumulator = 0.0f;
    }

    for (auto &body : bodyCollisionData)
    {
        body.reaccuredTimer += delta;
    }
}

void PhysicsBody::triggerPostCollisionEvent(PhysicsBody *foreignBody, Vector3 &point)
{
    for (auto it = bodyCollisionData.begin(); it != bodyCollisionData.end(); it++)
    {
        if ((*it).foreignBody == foreignBody)
        {
            it->point = point;
            it->reaccuredTimer = 0.0f;
            if (owner)
                owner->onCollidePersisted(foreignBody->getOwner(), point);
            return;
        }
    }
    bodyCollisionData.push_back(BodyCollisionData({foreignBody, point, 0.0f}));
    if (owner)
        owner->onCollide(foreignBody->getOwner(), point);
}

void PhysicsBody::removeNotPersistedCollisions()
{
    auto it = bodyCollisionData.begin();
    while (it != bodyCollisionData.end())
    {
        if (it->reaccuredTimer > 0.015f)
        {
            it = bodyCollisionData.erase(it);
            if (owner)
                owner->onCollideStopped((*it).foreignBody->getOwner());
        }
        else
            ++it;
    }
}

void PhysicsBody::notifyBodyRemoved(PhysicsBody *body)
{
    for (auto it = bodyCollisionData.begin(); it != bodyCollisionData.end(); it++)
    {
        if (it->foreignBody == body)
        {
            if (owner)
                owner->onCollideStopped(it->foreignBody->getOwner());
            bodyCollisionData.erase(it);
            break;
        }
    }
}

void PhysicsBody::setRelation(Transformation *transformation, Actor *owner)
{
    this->owner = owner;
    this->transformation = transformation;
    position = transformation->getPosition() * simScale;
    orientation = transformation->getRotation();
    localTransform = glm::translate(Matrix4(1.0f), Vector3(position.x, position.y, position.z));
    localTransform *= glm::toMat4(orientation);
    this->shape->provideTransformation(&localTransform);
}

void PhysicsBody::setStaticMotionType()
{
    this->motionType = MotionType::Static;
    if (motion)
    {
        delete motion;
        motion = nullptr;
    }
}

void PhysicsBody::setDynamicMotionType(float linearDamping, float angularDamping, float gravityFactor)
{
    this->motionType = MotionType::Dynamic;
    if (!motion)
        motion = new Motion();

    motion->setProperties(linearDamping, angularDamping, gravityFactor);
    auto inertia = shape->getInertiaTensor();
    motion->setInertia(inertia);
    motion->setMass(shape->getMass());
}

MotionType PhysicsBody::getMotionType()
{
    return motionType;
}

void PhysicsBody::setFriction(float friction)
{
    this->friction = friction;
}

float PhysicsBody::getFriction()
{
    return friction;
}

void PhysicsBody::setRestitution(float restitution)
{
    this->restitution = restitution;
}

float PhysicsBody::getRestitution()
{
    return restitution;
}

float PhysicsBody::getMass()
{
    if (motionType != MotionType::Static)
        return shape->getMass();
    else
        return 99999.9f;
}

float PhysicsBody::getInvMass()
{
    if (motion)
        return motion->getInvMass();
    else
        return 0.000001f;
}

Vector3 PhysicsBody::getLinearVelocity()
{
    if (motion)
        return motion->linearVelocity;
    return Vector3(0.0f, 0.0f, 0.0f);
}

void PhysicsBody::setLinearVelocity(Vector3 velocity)
{
    lock.lock();
    bIsSleeping = false;
    if (motion)
        motion->linearVelocity = velocity;
    lock.unlock();
}

void PhysicsBody::addLinearVelocity(Vector3 velocity)
{
    lock.lock();
    bIsSleeping = false;
    sleepAccumulator = 0.0f;
    if (motion)
        motion->linearVelocity += velocity;
    lock.unlock();
}

Vector3 PhysicsBody::getAngularVelocity()
{
    if (motion)
        return motion->angularVelocity;
    return Vector3(0.0f, 0.0f, 0.0f);
}

void PhysicsBody::setAngularVelocity(Vector3 velocity)
{
    lock.lock();
    bIsSleeping = false;
    sleepAccumulator = 0.0f;
    if (motion)
        motion->angularVelocity = velocity;
    lock.unlock();
}

void PhysicsBody::addAngularVelocity(Vector3 velocity)
{
    lock.lock();
    bIsSleeping = false;
    sleepAccumulator = 0.0f;
    if (motion)
        motion->angularVelocity += velocity;
    lock.unlock();
}

void PhysicsBody::translate(Vector3 v)
{
    lock.lock();
    translationAccumulator += v;
    bIsSleeping = false;
    sleepAccumulator = 0.0f;
    lock.unlock();
}

Matrix3 PhysicsBody::getInvertedInertia()
{
    if (motion)
        return motion->getInvertedInteria();

    Matrix3 inertia;
    inertia[0] = Vector3(1.0f, 0.0f, 0.0f);
    inertia[1] = Vector3(0.0f, 1.0f, 0.0f);
    inertia[2] = Vector3(0.0f, 0.0f, 1.0f);
    return glm::inverse(inertia);
}

Vector3 PhysicsBody::getPointVelocity(const Vector3 &localPoint)
{
    if (motion)
        return motion->getPointVelocity(localPoint);
    return Vector3(0.0f, 0.0f, 0.0f);
}

void PhysicsBody::process(float delta, const Vector3 &gravity)
{
    if (bIsSleeping)
        return;
    if (motionType == MotionType::Static)
    {
        setAsleep();
        return;
    }

    if (motion)
    {
        motion->process(delta, gravity, orientation);
    }
}

void PhysicsBody::castRay(const Segment &ray, std::vector<PhysicsBodyPoint> *points)
{
    std::vector<RayCollisionPoint> localPoints;
    if (this->shape->testRay(ray, &localPoints))
    {
        for (auto point = localPoints.begin(); point != localPoints.end(); point++)
        {
            points->push_back({actor, point->point / simScale, point->distance});
        }
    }
}

void PhysicsBody::setAsleep()
{
    bIsSleeping = true;
    localTransform = glm::translate(Matrix4(1.0f), Vector3(position.x, position.y, position.z));
    localTransform *= glm::toMat4(orientation);
    this->shape->provideTransformation(&localTransform);
}