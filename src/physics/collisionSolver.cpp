#include "collisionSolver.h"
#include "actor/actor.h"

CollisionSolver::CollisionSolver(float simScale)
{
    this->simScale = simScale;
}

void CollisionSolver::solve(PhysicsBody *a, PhysicsBody *b, CollisionManifold &manifold, float delta)
{
    Vector3 normal = manifold.normal;
    float depth = manifold.depth;
    Vector3 pointA = manifold.pointsOnA[0];
    Vector3 pointB = manifold.pointsOnB[0];
    Vector3 localPointA = pointA - a->getCenterOfMass();
    Vector3 localPointB = pointB - b->getCenterOfMass();

    // Calculate velocity of collision points
    Vector3 pointVelocityBodyA = a->getPointVelocity(localPointA);
    Vector3 pointVelocityBodyB = b->getPointVelocity(localPointB);
    Vector3 relativeVelocity = pointVelocityBodyB - pointVelocityBodyA;

    float friction = sqrtf(a->getFriction() * b->getFriction());

    float normalVelocity = glm::dot(relativeVelocity, normal);
    if (normalVelocity > 0.0f)
        return;

    // Calculate velocity bias due to restitution
    float combinedRestitution = fmaxf(a->getRestitution(), b->getRestitution());
    float normalVelocityBias = 0.0f;
    if (combinedRestitution > 0.0f && normalVelocity < -0.08f)
        normalVelocityBias = combinedRestitution * normalVelocity;

    // printf("normalVelocityBias %f, %f\n", normalVelocityBias, normalVelocity);

    Vector3 translate = normal * depth;
    if (a->getMotionType() != MotionType::Static)
        a->translate(-translate);
    if (b->getMotionType() != MotionType::Static)
        b->translate(translate);

    float lambda = solveAxis(
        a,
        b,
        localPointA,
        localPointB,
        pointVelocityBodyA,
        pointVelocityBodyB,
        a->getAngularVelocity(),
        b->getAngularVelocity(),
        normal,
        -3.4f,
        3.4f,
        normalVelocityBias);

    if (friction > 0.0f)
    {
        // Apply friction
        Vector3 tangent1 = getNormalizedPerpendicular(normal);
        Vector3 tangent2 = glm::cross(normal, tangent1);

        float maxLambda = friction * lambda;

        solveAxis(
            a,
            b,
            localPointA,
            localPointB,
            a->getLinearVelocity(),
            b->getLinearVelocity(),
            a->getAngularVelocity(),
            b->getAngularVelocity(),
            tangent1,
            -maxLambda,
            maxLambda,
            0.0f);

        solveAxis(
            a,
            b,
            localPointA,
            localPointB,
            a->getLinearVelocity(),
            b->getLinearVelocity(),
            a->getAngularVelocity(),
            b->getAngularVelocity(),
            tangent2,
            -maxLambda,
            maxLambda,
            0.0f);
    }
}

float CollisionSolver::solveAxis(
    PhysicsBody *a,
    PhysicsBody *b,
    Vector3 pointA,
    Vector3 pointB,
    Vector3 velocityBodyA,
    Vector3 velocityBodyB,
    Vector3 angularVelocityBodyA,
    Vector3 angularVelocityBodyB,
    Vector3 axis,
    float fMin,
    float fMax,
    float bias)
{
    Vector3 r1Axis = Vector3(0.0f);
    if (a->getMotionType() != MotionType::Static)
        r1Axis = glm::cross(pointA, axis);

    Vector3 r2Axis = Vector3(0.0f);
    if (b->getMotionType() != MotionType::Static)
        r2Axis = glm::cross(pointB, axis);

    // Inverse effective mass: K = J M^-1 J^T
    float invEffectiveMass = 0.0f;

    Vector3 mInvI1_R1Axis(0.0f);
    if (a->getMotionType() == MotionType::Dynamic)
    {
        mInvI1_R1Axis = a->getInvertedInertia() * r1Axis;
        invEffectiveMass = a->getInvMass() + glm::dot(mInvI1_R1Axis, r1Axis);
    }
    else
        invEffectiveMass = 0.0f;

    Vector3 mInvI2_R2Axis(0.0f);
    if (b->getMotionType() == MotionType::Dynamic)
    {
        mInvI2_R2Axis = b->getInvertedInertia() * r2Axis;
        invEffectiveMass += b->getInvMass() + glm::dot(mInvI2_R2Axis, r2Axis);
    }

    float jv = 0.0f;
    if (a->getMotionType() != MotionType::Static && b->getMotionType() != MotionType::Static)
        jv = glm::dot(axis, velocityBodyA - velocityBodyB);
    else if (a->getMotionType() != MotionType::Static)
        jv = glm::dot(axis, velocityBodyA);
    else if (b->getMotionType() != MotionType::Static)
        jv = glm::dot(axis, -velocityBodyB);

    // Calculate jacobian multiplied by angular velocity
    if (a->getMotionType() != MotionType::Static)
    {
        jv += glm::dot(r1Axis, angularVelocityBodyA);
    }
    if (b->getMotionType() != MotionType::Static)
    {
        jv -= glm::dot(r2Axis, angularVelocityBodyB);
    }

    float mEffectiveMass = 1.0f / invEffectiveMass;
    float lambda = fminf(fmaxf(fMin, mEffectiveMass * (jv - bias)), fMax);

    if (lambda != 0.0f)
    {
        a->addLinearVelocity(-(lambda * a->getInvMass()) * axis);
        a->addAngularVelocity(-lambda * mInvI1_R1Axis);

        b->addLinearVelocity((lambda * b->getInvMass()) * axis);
        b->addAngularVelocity(lambda * mInvI2_R2Axis);
    }

    return lambda;
}
