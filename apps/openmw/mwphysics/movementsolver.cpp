#include "movementsolver.hpp"

#include <BulletCollision/CollisionDispatch/btCollisionObject.h>
#include <BulletCollision/CollisionDispatch/btCollisionWorld.h>
#include <BulletCollision/CollisionShapes/btCollisionShape.h>

#include <components/esm/loadgmst.hpp>
#include <components/misc/convert.hpp>

#include "../mwbase/world.hpp"
#include "../mwbase/environment.hpp"

#include "../mwworld/class.hpp"
#include "../mwworld/esmstore.hpp"
#include "../mwworld/refdata.hpp"

#include "actor.hpp"
#include "collisiontype.hpp"
#include "constants.hpp"
#include "contacttestwrapper.h"
#include "physicssystem.hpp"
#include "stepper.hpp"
#include "trace.h"

#include <cmath>

namespace MWPhysics
{
    // Source Engine-inspired physics constants
    //const float GRAVITY = 800.0f;       // Gravity in units/s�
    //const float AIR_ACCEL = 10.0f;      // Air acceleration (sv_airaccelerate)
    //const float GROUND_ACCEL = 10.0f;   // Ground acceleration (sv_accelerate)
    //const float FRICTION = 4.0f;        // Ground friction
    //const float MAX_AIR_SPEED = 1000.0f; // Maximum air speed cap

    static bool isActor(const btCollisionObject* obj)
    {
        assert(obj);
        return obj->getBroadphaseHandle()->m_collisionFilterGroup == CollisionType_Actor;
    }

    class ContactCollectionCallback : public btCollisionWorld::ContactResultCallback
    {
    public:
        ContactCollectionCallback(const btCollisionObject* me, osg::Vec3f velocity) : mMe(me)
        {
            m_collisionFilterGroup = me->getBroadphaseHandle()->m_collisionFilterGroup;
            m_collisionFilterMask = me->getBroadphaseHandle()->m_collisionFilterMask & ~CollisionType_Projectile;
            mVelocity = Misc::Convert::toBullet(velocity);
        }
        btScalar addSingleResult(btManifoldPoint& contact, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1) override
        {
            if (isActor(colObj0Wrap->getCollisionObject()) && isActor(colObj1Wrap->getCollisionObject()))
                return 0.0;
            if (contact.m_normalWorldOnB.dot(mVelocity) > 0.0)
                return 0.0;
            auto delta = contact.m_normalWorldOnB * -contact.m_distance1;
            mContactSum += delta;
            mMaxX = std::max(std::abs(delta.x()), mMaxX);
            mMaxY = std::max(std::abs(delta.y()), mMaxY);
            mMaxZ = std::max(std::abs(delta.z()), mMaxZ);
            if (contact.m_distance1 < mDistance)
            {
                mDistance = contact.m_distance1;
                mNormal = contact.m_normalWorldOnB;
                mDelta = delta;
                return mDistance;
            }
            else
            {
                return 0.0;
            }
        }
        btScalar mMaxX = 0.0;
        btScalar mMaxY = 0.0;
        btScalar mMaxZ = 0.0;
        btVector3 mContactSum{ 0.0, 0.0, 0.0 };
        btVector3 mNormal{ 0.0, 0.0, 0.0 };
        btVector3 mDelta{ 0.0, 0.0, 0.0 };
        btScalar mDistance = 0.0;
    protected:
        btVector3 mVelocity;
        const btCollisionObject* mMe;
    };

    osg::Vec3f MovementSolver::traceDown(const MWWorld::Ptr& ptr, const osg::Vec3f& position, Actor* actor, btCollisionWorld* collisionWorld, float maxHeight)
    {
        osg::Vec3f offset = actor->getCollisionObjectPosition() - ptr.getRefData().getPosition().asVec3();

        ActorTracer tracer;
        tracer.findGround(actor, position + offset, position + offset - osg::Vec3f(0, 0, maxHeight), collisionWorld);
        if (tracer.mFraction >= 1.0f)
        {
            actor->setOnGround(false);
            return position;
        }

        actor->setOnGround(true);

        btVector3 from = Misc::Convert::toBullet(position);
        btVector3 to = from - btVector3(0, 0, maxHeight);

        btCollisionWorld::ClosestRayResultCallback resultCallback1(from, to);
        resultCallback1.m_collisionFilterGroup = 0xff;
        resultCallback1.m_collisionFilterMask = CollisionType_World | CollisionType_HeightMap;

        collisionWorld->rayTest(from, to, resultCallback1);

        if (resultCallback1.hasHit() && ((Misc::Convert::toOsg(resultCallback1.m_hitPointWorld) - tracer.mEndPos + offset).length2() > 35 * 35
            || !isWalkableSlope(tracer.mPlaneNormal)))
        {
            actor->setOnSlope(!isWalkableSlope(resultCallback1.m_hitNormalWorld));
            return Misc::Convert::toOsg(resultCallback1.m_hitPointWorld) + osg::Vec3f(0.f, 0.f, sGroundOffset);
        }

        actor->setOnSlope(!isWalkableSlope(tracer.mPlaneNormal));

        return tracer.mEndPos - offset + osg::Vec3f(0.f, 0.f, sGroundOffset);
    }

    // Check if a slope is walkable (Source Engine uses normal.z >= 0.7)
    bool isWalkableSlope(const osg::Vec3f& normal)
    {
        float zComponent = normal.z();
        bool walkable = zComponent >= 0.8f;
        //std::cout << "Slope normal: (" << normal.x() << ", " << normal.y() << ", " << normal.z()
            //<< "), z-component: " << zComponent << ", walkable: " << (walkable ? "true" : "false") << std::endl;
        return walkable;
    }

    // Clip velocity against a surface normal for sliding (Source-like collision)
    osg::Vec3f ClipVelocity(const osg::Vec3f& in, const osg::Vec3f& normal, float overbounce = 1.1f)
    {
        float backoff = in * normal;
        if (backoff < 0)
            backoff *= overbounce;
        else
            backoff /= overbounce;
        return in - normal * backoff;
    }

    // Calculate desired velocity based on input and movement state
    osg::Vec3f calculateWishVelocity(const ESM::Position& refpos, const osg::Vec3f& movement, bool isInAir)
    {
        if (isInAir)
            return (osg::Quat(refpos.rot[0], osg::Vec3f(-1, 0, 0)) *
                osg::Quat(refpos.rot[2], osg::Vec3f(0, 0, -1))) * movement;
        else
            return (osg::Quat(refpos.rot[2], osg::Vec3f(0, 0, -1))) * movement;
    }

    //void handleSlopeSliding(btCollisionObject* actor, const btVector3& slopeNormal, float timeStep)
    //{
    //    // Get the current velocity
    //    btVector3 velocity = actor->getLinearVelocity();

    //    // Project velocity onto the slope plane (remove component pushing into the slope)
    //    btVector3 projectedVelocity = velocity - slopeNormal * velocity.dot(slopeNormal);

    //    // Apply gravity along the slope
    //    btVector3 gravity = btVector3(0, 0, -9.8f) * timeStep;  // Adjust gravity value as needed
    //    btVector3 gravityAlongSlope = gravity - slopeNormal * gravity.dot(slopeNormal);

    //    // Update velocity with projected velocity and gravity along the slope
    //    velocity = projectedVelocity + gravityAlongSlope;

    //    // Set the new velocity
    //    actor->setLinearVelocity(velocity);

    //    // Reduce restitution to prevent bouncing
    //    actor->setRestitution(0.0f);
    //}

    void MovementSolver::move(ActorFrameData& actor, float time, const btCollisionWorld* collisionWorld,
        WorldFrameData& worldData)
    {
        auto* physicActor = actor.mActorRaw;
        const ESM::Position& refpos = actor.mRefpos;

        // Early-out for totally static creatures
        {
            const auto ptr = physicActor->getPtr();
            if (!ptr.getClass().isMobile(ptr))
                return;
        }

        // Reset per-frame data
        physicActor->setWalkingOnWater(false);

        // Skip collision if disabled or requested
        if (!physicActor->getCollisionMode() || actor.mSkipCollisionDetection)
        {
            actor.mPosition += calculateWishVelocity(refpos, actor.mMovement, true) * time;
            return;
        }

        const btCollisionObject* colobj = physicActor->getCollisionObject();
        osg::Vec3f halfExtents = physicActor->getHalfExtents();
        actor.mPosition.z() += halfExtents.z();  // Adjust for collision mesh offset

        // Swim level calculation
        static const float fSwimHeightScale = MWBase::Environment::get().getWorld()->getStore().get<ESM::GameSetting>().find("fSwimHeightScale")->mValue.getFloat();
        float swimlevel = actor.mWaterlevel + halfExtents.z() - (physicActor->getRenderingHalfExtents().z() * 2 * fSwimHeightScale);

        // Physics constants (adjustable based on testing)
        const float FRICTION = 5.0f;        // Ground friction (increase to 6.0f if too slippery)
        const float GROUND_ACCEL = 10.0f;   // Ground acceleration (Source default)
        //const float AIR_ACCEL = 10.0f;      // Air acceleration (Source default)
        const float GRAVITY = Constants::GravityConst * Constants::UnitsPerMeter;  // ~627.2 units/s�
        const float JUMP_SPEED = 268.0f;    // Jump velocity (adjust for unit scale)

        // Get current inertial force (persistent velocity)
        osg::Vec3f velocity = physicActor->getInertialForce();

        // Calculate desired movement direction and speed
        bool isInAir = !physicActor->getOnGround() || physicActor->getOnSlope();
        osg::Vec3f wishvel = calculateWishVelocity(refpos, actor.mMovement, isInAir);
        float wishspeed = wishvel.length();
        osg::Vec3f wishdir = wishvel;
        if (wishspeed > 0) wishdir.normalize();

        // Handle jumping
        if (actor.mWantJump && physicActor->getOnGround() && !physicActor->getOnSlope())
        {
            velocity.z() = JUMP_SPEED;
            actor.mDidJump = true;
            physicActor->setOnGround(false);  // Leave ground immediately
        }

        // Determine movement state
        bool isOnGround = physicActor->getOnGround();
        bool isOnSlope = physicActor->getOnSlope();

        // Movement logic
        if (isOnGround && !isOnSlope)
        {
            // Ground movement: Apply friction to horizontal components
            float speed = sqrt(velocity.x() * velocity.x() + velocity.y() * velocity.y());
            if (speed > 0)
            {
                float drop = speed * FRICTION * time;
                float newSpeed = std::max(speed - drop, 0.0f);
                float scale = newSpeed / speed;
                velocity.x() *= scale;
                velocity.y() *= scale;
            }

            // Ground acceleration
            osg::Vec3f horizontalWishdir(wishdir.x(), wishdir.y(), 0);
            float currentspeed = velocity.x() * horizontalWishdir.x() + velocity.y() * horizontalWishdir.y();
            float addspeed = wishspeed - currentspeed;
            if (addspeed > 0)
            {
                float accelspeed = GROUND_ACCEL * wishspeed * time;
                if (accelspeed > addspeed) accelspeed = addspeed;
                velocity.x() += accelspeed * horizontalWishdir.x();
                velocity.y() += accelspeed * horizontalWishdir.y();
            }

            velocity.z() = 0;  // Stay on ground
        }
        else
        {
            // Air or slope movement (surfing)
            float currentspeed = velocity * wishdir;
            float addspeed = wishspeed - currentspeed;
            if (addspeed > 0)
            {
                float accelspeed = AIR_ACCEL * wishspeed * time;
                if (accelspeed > addspeed) accelspeed = addspeed;
                velocity += wishdir * accelspeed;
            }

            // Apply gravity
            velocity.z() -= GRAVITY * time;
        }

        // Collision and movement loop
        Stepper stepper(collisionWorld, colobj);
        ActorTracer tracer;
        osg::Vec3f newPosition = actor.mPosition;
        float remainingTime = time;
        bool seenGround = physicActor->getOnGround() && !physicActor->getOnSlope() && !actor.mFlying;
        bool forceGroundTest = false;

        for (int iterations = 0; iterations < sMaxIterations && remainingTime > 0.0001f; ++iterations)
        {
            osg::Vec3f nextpos = newPosition + velocity * remainingTime;

            if (!actor.mFlying && nextpos.z() > swimlevel && newPosition.z() < swimlevel)
            {
                const osg::Vec3f down(0, 0, -1);
                velocity = reject(velocity, down);
                continue;
            }

            if ((newPosition - nextpos).length2() > 0.0001)
            {
                tracer.doTrace(colobj, newPosition, nextpos, collisionWorld);

                if (tracer.mFraction >= 1.0f)
                {
                    newPosition = tracer.mEndPos;
                    break;
                }
            }
            else
            {
                break;
            }

            if (isWalkableSlope(tracer.mPlaneNormal) && !actor.mFlying && newPosition.z() >= swimlevel)
                seenGround = true;

            float hitHeight = tracer.mHitPoint.z() - tracer.mEndPos.z() + halfExtents.z();
            osg::Vec3f oldPosition = newPosition;
            bool usedStepLogic = false;
            if (hitHeight < sStepSizeUp && !isActor(tracer.mHitObject))
            {
                usedStepLogic = stepper.step(newPosition, velocity, remainingTime, seenGround, iterations == 0);
            }
            if (usedStepLogic)
            {
                const auto ptr = physicActor->getPtr();
                if (ptr.getClass().isPureWaterCreature(ptr) && newPosition.z() + halfExtents.z() > actor.mWaterlevel)
                    newPosition = oldPosition;
                else if (!actor.mFlying && actor.mPosition.z() >= swimlevel)
                    forceGroundTest = true;
            }
            else
            {
                remainingTime *= (1.0f - tracer.mFraction);
                velocity = ClipVelocity(velocity, tracer.mPlaneNormal);

                if ((newPosition - tracer.mEndPos).length2() > sCollisionMargin * sCollisionMargin)
                {
                    auto direction = velocity;
                    direction.normalize();
                    newPosition = tracer.mEndPos - direction * sCollisionMargin;
                }
            }
        }

        // Final ground check
        bool isOnGroundFinal = false;
        bool isOnSlopeFinal = false;
        if (forceGroundTest || (velocity.z() <= 0.f && newPosition.z() >= swimlevel))
        {
            osg::Vec3f from = newPosition;
            auto dropDistance = 2 * sGroundOffset + (physicActor->getOnGround() ? sStepSizeDown : 0);
            osg::Vec3f to = newPosition - osg::Vec3f(0, 0, dropDistance);
            tracer.doTrace(colobj, from, to, collisionWorld);
            if (tracer.mFraction < 1.0f)
            {
                if (!isActor(tracer.mHitObject))
                {
                    isOnGroundFinal = true;
                    isOnSlopeFinal = !isWalkableSlope(tracer.mPlaneNormal);

                    const btCollisionObject* standingOn = tracer.mHitObject;
                    PtrHolder* ptrHolder = static_cast<PtrHolder*>(standingOn->getUserPointer());
                    if (ptrHolder)
                        actor.mStandingOn = ptrHolder->getPtr();

                    if (standingOn->getBroadphaseHandle()->m_collisionFilterGroup == CollisionType_Water)
                        physicActor->setWalkingOnWater(true);
                    if (!actor.mFlying && !isOnSlopeFinal)
                    {
                        if (tracer.mFraction * dropDistance > sGroundOffset)
                            newPosition.z() = tracer.mEndPos.z() + sGroundOffset;
                        else
                        {
                            newPosition.z() = tracer.mEndPos.z();
                            tracer.doTrace(colobj, newPosition, newPosition + osg::Vec3f(0, 0, 2 * sGroundOffset), collisionWorld);
                            newPosition = (newPosition + tracer.mEndPos) / 2.0;
                        }
                    }
                }
            }
        }

        if (isOnSlopeFinal)
        {
            float zComponent = tracer.mPlaneNormal.z();
            bool walkable = zComponent >= 0.8f;
            std::cout << "Slope normal: (" << tracer.mPlaneNormal.x() << ", " << tracer.mPlaneNormal.y() << ", " << tracer.mPlaneNormal.z()
                << "), z-component: " << zComponent << ", walkable: " << (walkable ? "true" : "false") << std::endl;
        }

        // Update actor states
        if (isOnGroundFinal && !isOnSlopeFinal)
        {
            velocity.z() = 0;  // Reset vertical velocity on flat ground
        }
        physicActor->setInertialForce(velocity);
        physicActor->setOnGround(isOnGroundFinal);
        physicActor->setOnSlope(isOnSlopeFinal);

        actor.mPosition = newPosition;
        actor.mPosition.z() -= halfExtents.z();  // Undo offset


        //log position and speed
        //std::cout << "Position: (" << actor.mPosition.x() << ", " << actor.mPosition.y() << ", " << actor.mPosition.z() << ")" << std::endl;
        //std::cout << "Velocity: (" << velocity.x() << ", " << velocity.y() << ", " << velocity.z() << ")" << std::endl;
    }

    btVector3 addMarginToDelta(btVector3 delta)
    {
        if (delta.length2() == 0.0)
            return delta;
        return delta + delta.normalized() * sCollisionMargin;
    }

    void MovementSolver::unstuck(ActorFrameData& actor, const btCollisionWorld* collisionWorld)
    {
        const auto& ptr = actor.mActorRaw->getPtr();
        if (!ptr.getClass().isMobile(ptr))
            return;

        auto* physicActor = actor.mActorRaw;
        if (!physicActor->getCollisionMode() || actor.mSkipCollisionDetection)
            return;

        auto* collisionObject = physicActor->getCollisionObject();
        auto tempPosition = actor.mPosition;

        if (physicActor->getStuckFrames() >= 10)
        {
            if ((physicActor->getLastStuckPosition() - actor.mPosition).length2() < 100)
                return;
            else
            {
                physicActor->setStuckFrames(0);
                physicActor->setLastStuckPosition({ 0, 0, 0 });
            }
        }

        const auto verticalHalfExtent = osg::Vec3f(0.0, 0.0, physicActor->getHalfExtents().z());
        auto velocity = (osg::Quat(actor.mRefpos.rot[0], osg::Vec3f(-1, 0, 0)) * osg::Quat(actor.mRefpos.rot[2], osg::Vec3f(0, 0, -1))) * actor.mMovement;
        if (!physicActor->getOnGround() || physicActor->getOnSlope())
            velocity += physicActor->getInertialForce();

        osg::Vec3f refPosition = tempPosition + verticalHalfExtent;
        osg::Vec3f goodPosition = refPosition;
        const btTransform oldTransform = collisionObject->getWorldTransform();
        btTransform newTransform = oldTransform;

        auto gatherContacts = [&](btVector3 newOffset) -> ContactCollectionCallback
            {
                goodPosition = refPosition + Misc::Convert::toOsg(addMarginToDelta(newOffset));
                newTransform.setOrigin(Misc::Convert::toBullet(goodPosition));
                collisionObject->setWorldTransform(newTransform);

                ContactCollectionCallback callback{ collisionObject, velocity };
                ContactTestWrapper::contactTest(const_cast<btCollisionWorld*>(collisionWorld), collisionObject, callback);
                return callback;
            };

        auto contactCallback = gatherContacts({ 0.0, 0.0, 0.0 });
        if (contactCallback.mDistance < -sAllowedPenetration)
        {
            physicActor->setStuckFrames(physicActor->getStuckFrames() + 1);
            physicActor->setLastStuckPosition(actor.mPosition);
            auto positionDelta = contactCallback.mContactSum;
            if (std::abs(positionDelta.x()) > contactCallback.mMaxX)
                positionDelta *= contactCallback.mMaxX / std::abs(positionDelta.x());
            if (std::abs(positionDelta.y()) > contactCallback.mMaxY)
                positionDelta *= contactCallback.mMaxY / std::abs(positionDelta.y());
            if (std::abs(positionDelta.z()) > contactCallback.mMaxZ)
                positionDelta *= contactCallback.mMaxZ / std::abs(positionDelta.z());

            auto contactCallback2 = gatherContacts(positionDelta);
            if (contactCallback2.mDistance > contactCallback.mDistance)
                tempPosition = goodPosition - verticalHalfExtent;
            else
            {
                auto contactCallback3 = gatherContacts({ 0.0, 0.0, std::abs(positionDelta.z()) });
                if (contactCallback3.mDistance > contactCallback.mDistance)
                    tempPosition = goodPosition - verticalHalfExtent;
                else
                {
                    auto contactCallback4 = gatherContacts({ 0.0, 0.0, 10.0 });
                    if (contactCallback4.mDistance > contactCallback.mDistance)
                        tempPosition = goodPosition - verticalHalfExtent;
                }
            }
        }
        else
        {
            physicActor->setStuckFrames(0);
            physicActor->setLastStuckPosition({ 0, 0, 0 });
        }

        collisionObject->setWorldTransform(oldTransform);
        actor.mPosition = tempPosition;
    }
}
