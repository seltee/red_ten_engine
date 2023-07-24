#pragma once
#include "math/math.h"
#include "math/transformation.h"
#include "common/destroyable.h"
#include "physics/shapes/shape.h"
#include "physics/motion.h"
#include "physics/constraint6DOF.h"
#include <vector>
#include <thread>

class PhysicsBody;
class Actor;

enum class MotionType : unsigned char
{
    Static,    ///< Non movable
//    Kinematic, ///< Movable using velocities only, does not respond to forces
    Dynamic,   ///< Responds to forces as a normal physics object
};

struct PhysicsBodyPoint
{
    Actor *actor;
    Vector3 point;
    float distance;
};

class PhysicsBody : public Destroyable
{
public:
    PhysicsBody(Shape *shape, float simScale);
    virtual ~PhysicsBody();
    EXPORT void prepareSteps();

    EXPORT void setRelation(Transformation *transformation);
    EXPORT void setStaticMotionType();
    EXPORT void setDynamicMotionType(float linearDamping = 0.15f, float angularDamping = 0.05f, float gravityFactor = 1.0f);

    EXPORT MotionType getMotionType();

    EXPORT void setFriction(float friction);
    EXPORT float getFriction();
    EXPORT void setRestitution(float restitution);
    EXPORT float getRestitution();
    EXPORT float getMass();
    EXPORT float getInvMass();

    EXPORT inline Vector3 getCenterOfMass() { return position; }
    EXPORT inline Quat getOrientation() { return orientation; }
    EXPORT inline Matrix4 *getTransformation() { return transformation->getModelMatrix(); }

    EXPORT Vector3 getLinearVelocity();
    EXPORT void setLinearVelocity(Vector3 velocity);
    EXPORT void addLinearVelocity(Vector3 velocity);

    EXPORT Vector3 getAngularVelocity();
    EXPORT void setAngularVelocity(Vector3 velocity);
    EXPORT void addAngularVelocity(Vector3 velocity);

    EXPORT void translate(Vector3 v);

    EXPORT void addConstraint6DOF(Constraint6DOFDescriptor descriptor){
        auto constraint = new Constraint6DOF(descriptor);
        constraints.push_back(constraint);
    }

    EXPORT Matrix3 getInvertedInertia();

    EXPORT Vector3 getPointVelocity(Vector3 localPoint);

    EXPORT void process(float delta, Vector3 gravity);

    EXPORT void castRay(Line ray, std::vector<PhysicsBodyPoint> *points);

    inline ShapeCollisionType getType()
    {
        return shape->getType();
    }

    inline Shape *getShape()
    {
        return shape;
    }

    inline void setActor(Actor *actor)
    {
        this->actor = actor;
    }
    inline Actor *getActor()
    {
        return actor;
    }
    inline bool checkAABB(AABB aabb)
    {
        return getAABB().test(aabb);
    }
    inline bool checkAABB(Line line)
    {
        return getAABB().test(line);
    }
    inline AABB getAABB()
    {
        return this->shape->getAABB();
    }

protected:
    std::vector<Constraint*> constraints;
    Vector3 translationAccumulator = Vector3(0.0f);
    float translationDivision = 0.0f;

    Transformation *transformation = nullptr;

    Matrix4 localTransform;
    MotionType motionType = MotionType::Static;
    Vector3 position = Vector3({0.0f, 0.0f, 0.0f});
    Quat orientation = Quat(1.0f, 0.0f, 0.0f, 0.0f);

    Motion *motion = nullptr;

    float friction = 0.5f;
    float restitution = 0.5f;

    float simScale = 0.1f;

    Actor *actor = nullptr;
    Shape *shape = nullptr;

    std::mutex lock;
};