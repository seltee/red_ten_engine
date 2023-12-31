#pragma once
#include "math/math.h"
#include "math/transformation.h"
#include "common/destroyable.h"
#include "physics/shapes/shape.h"
#include "physics/motion.h"
#include "physics/constraint6DOF.h"
#include <vector>
#include <thread>
#include <mutex>

class PhysicsBody;
class Actor;

enum class MotionType : unsigned char
{
    Static,  ///< Non movable
    Dynamic, ///< Responds to forces as a normal physics object
};

struct PhysicsBodyPoint
{
    Actor *actor;
    Vector3 point;
    Vector3 normal;
    float distance;
};

struct BodyCollisionData
{
    PhysicsBody *foreignBody;
    Vector3 point;
    float reaccuredTimer;
};

class PhysicsBody : public Destroyable
{
public:
    PhysicsBody(Shape *shape, float simScale);
    virtual ~PhysicsBody();
    EXPORT void prepareSteps();
    EXPORT void finishStep(float delta);

    EXPORT void triggerPostCollisionEvent(PhysicsBody *foreignBody, Vector3 &point);
    EXPORT void removeNotPersistedCollisions();
    EXPORT void notifyBodyRemoved(PhysicsBody *body);

    EXPORT void setRelation(Transformation *transformation, Actor *owner);
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

    inline std::vector<BodyCollisionData> *getCollisionBodies() { return &bodyCollisionData; }

    EXPORT void translate(Vector3 v);

    EXPORT void addConstraint6DOF(const Constraint6DOFDescriptor &descriptor)
    {
        auto constraint = new Constraint6DOF(descriptor);
        constraints.push_back(constraint);
    }

    EXPORT Matrix3 getInvertedInertia();

    EXPORT Vector3 getPointVelocity(const Vector3 &localPoint);

    EXPORT void process(float delta, const Vector3 &gravity);

    EXPORT void castRay(const Segment &ray, std::vector<PhysicsBodyPoint> *points);

    inline ShapeCollisionType getType()
    {
        return shape->getType();
    }

    inline Shape *getShape()
    {
        return shape;
    }

    inline Actor *getOwner() { return owner; }
    inline void clearOwner() { owner = nullptr; }

    inline void setActor(Actor *actor)
    {
        this->actor = actor;
    }
    inline Actor *getActor()
    {
        return actor;
    }
    inline bool checkAABB(const AABB &aabb)
    {
        return getAABB().test(aabb);
    }
    inline bool checkAABB(const Segment &line)
    {
        return getAABB().test(line);
    }
    inline AABB getAABB()
    {
        return this->shape->getAABB();
    }
    inline bool isSleeping()
    {
        return bIsSleeping;
    }
    inline void forceWake()
    {
        bIsSleeping = false;
    }
    inline bool isEnabled()
    {
        return this->bIsEnabled;
    }
    inline void setEnabled(bool bState)
    {
        this->bIsEnabled = bState;
    }

    void setAsleep();

protected:
    std::vector<Constraint *> constraints;
    std::vector<BodyCollisionData> bodyCollisionData;

    Vector3 translationAccumulator = Vector3(0.0f);

    Transformation *transformation = nullptr;
    Actor *owner = nullptr;

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

    bool bIsSleeping = false;
    float sleepAccumulator = 0.0f;

    bool bIsEnabled = true;
};