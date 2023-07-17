#include "physicsBody.h"

PhysicsBody::PhysicsBody(Shape *shape, float simScale)
{
    this->shape = shape;
    this->simScale = simScale;
}

PhysicsBody::~PhysicsBody()
{
}

void PhysicsBody::prepareSteps()
{
    if (transformation)
    {
        position = transformation->getPosition() * simScale;
        orientation = transformation->getRotation();
    }
    if (translationDivision != 0.0f)
    {
        if (constraints.size() > 0)
            for (auto constraint = constraints.begin(); constraint != constraints.end(); constraint++)
                translationAccumulator = (*constraint)->processTranslation(translationAccumulator);

        position += translationAccumulator / translationDivision;

        translationAccumulator = Vector3(0.0f);
        translationDivision = 0.0f;
    }
}

void PhysicsBody::setRelation(Transformation *transformation)
{
    this->transformation = transformation;
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
    if (motion)
        motion->linearVelocity = velocity;
}

void PhysicsBody::addLinearVelocity(Vector3 velocity)
{
    if (motion)
        motion->linearVelocity += velocity;
}

Vector3 PhysicsBody::getAngularVelocity()
{
    if (motion)
        return motion->angularVelocity;
    return Vector3(0.0f, 0.0f, 0.0f);
}

void PhysicsBody::setAngularVelocity(Vector3 velocity)
{
    if (motion)
        motion->angularVelocity = velocity;
}

void PhysicsBody::addAngularVelocity(Vector3 velocity)
{
    if (motion)
        motion->angularVelocity += velocity;
}

void PhysicsBody::translate(Vector3 v)
{
    translationAccumulator += v;
    translationDivision += 1.0f;
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

Vector3 PhysicsBody::getPointVelocity(Vector3 localPoint)
{
    if (motion)
        return motion->getPointVelocity(localPoint);
    return Vector3(0.0f, 0.0f, 0.0f);
}

void PhysicsBody::process(float delta, Vector3 gravity)
{
    if (motion)
    {
        if (constraints.size() > 0)
            for (auto constraint = constraints.begin(); constraint != constraints.end(); constraint++)
                (*constraint)->processMotion(motion);

        motion->process(delta, gravity, orientation);
        position += motion->linearVelocity * delta;

        Vector3 angularVelocityDelta = motion->angularVelocity * delta;
        float len = glm::length(angularVelocityDelta);
        if (len > 1.0e-6f)
        {
            orientation = glm::normalize(glm::angleAxis(len, angularVelocityDelta / len) * orientation);
        }
    }

    if (transformation)
    {
        transformation->setPosition(position / simScale);
        transformation->setRotation(orientation);
    }

    localTransform = glm::translate(Matrix4(1.0f), Vector3(position.x, position.y, position.z));
    localTransform *= glm::toMat4(orientation);
    this->shape->provideTransformation(&localTransform);
}

void PhysicsBody::castRay(Line ray, std::vector<PhysicsBodyPoint> *points)
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