#include "surfmovements.hpp"

#include <BulletCollision/CollisionDispatch/btCollisionObject.h>
#include <BulletCollision/CollisionShapes/btCollisionShape.h>
#include <LinearMath/btVector3.h>
#include <LinearMath/btTransform.h>
#include <LinearMath/btQuaternion.h>

#include <components/misc/convert.hpp>
#include <components/settings/settings.hpp>

#include "../mwbase/world.hpp"
#include "../mwbase/environment.hpp"
#include "../mwbase/soundmanager.hpp"
#include "../mwmechanics/movement.hpp"
#include "../mwworld/class.hpp"
#include "../mwworld/player.hpp"

// #include "../mwbase/mechanicsmanager.hpp"


#include "actor.hpp"
#include "movementsolver.hpp"
#include "physicssystem.hpp"
#include "trace.h"

namespace MWPhysics
{
    namespace SurfMovements
    {
        float SURF_ANGLE_MIN = -0.9f;
        float SURF_ANGLE_MAX = 0.9f;
        float SURF_FRICTION = 0.05f;
        float SURF_GRAVITY = 400.0f;
        float AIR_ACCELERATE = 100.0f;
        float MAX_AIR_SPEED = 1000.0f;
        float SURF_STICK_FORCE = 50.0f;
        
        // Implement setters
        void setSurfAngleMin(float value) { SURF_ANGLE_MIN = value; }
        void setSurfAngleMax(float value) { SURF_ANGLE_MAX = value; }
        void setSurfFriction(float value) { SURF_FRICTION = value; }
        void setSurfGravity(float value) { SURF_GRAVITY = value; }
        void setAirAccelerate(float value) { AIR_ACCELERATE = value; }
        void setMaxAirSpeed(float value) { MAX_AIR_SPEED = value; }
        void setSurfStickForce(float value) { SURF_STICK_FORCE = value; }
        
        // Implement getters
        float getSurfAngleMin() { return SURF_ANGLE_MIN; }
        float getSurfAngleMax() { return SURF_ANGLE_MAX; }
        float getSurfFriction() { return SURF_FRICTION; }
        float getSurfGravity() { return SURF_GRAVITY; }
        float getAirAccelerate() { return AIR_ACCELERATE; }
        float getMaxAirSpeed() { return MAX_AIR_SPEED; }
        float getSurfStickForce() { return SURF_STICK_FORCE; }


        bool isSurfableSurface(const btVector3& normal)
        {
            float angle = normal.angle(btVector3(0, 0, 1));
            return (angle > SURF_ANGLE_MIN && angle < SURF_ANGLE_MAX);
        }

        void handleSurfMovement(ActorFrameData& actorData, const btVector3& normal, float dt)
        {
            btVector3 velocity = Misc::Convert::toBullet(actorData.mMovement);

            if (isSurfableSurface(normal))
            {
                btVector3 surfaceNormal = normal.normalized();
                btVector3 projectedVelocity = velocity - (velocity.dot(surfaceNormal) * surfaceNormal);
                btVector3 surfaceGravity = surfaceNormal * -SURF_GRAVITY * dt;
                projectedVelocity += surfaceGravity;
                velocity = projectedVelocity * (1.0f - SURF_FRICTION);

                const float sideMove = actorData.mMovement.x();
                if (std::abs(sideMove) > 0.0f)
                {
                    btVector3 wishDir = btVector3(sideMove > 0 ? 1 : -1, 0, 0);
                    float currentSpeed = velocity.length();
                    float addSpeed = std::min(MAX_AIR_SPEED - currentSpeed, AIR_ACCELERATE * dt);
                    if (addSpeed > 0)
                        velocity += wishDir * addSpeed;
                }

                velocity += surfaceNormal * -SURF_STICK_FORCE * dt;

                float speed = velocity.length();
                if (speed > MAX_AIR_SPEED)
                {
                    float scale = MAX_AIR_SPEED / speed;
                    velocity *= scale;
                }

                actorData.mMovement = Misc::Convert::toOsg(velocity);
                actorData.mDidJump = false;
                actorData.mFlying = false;
                actorData.mSwimming = false;
            }
        }

        void applySurfingEffects(const MWWorld::Ptr& ptr)
        {
            MWBase::SoundManager *sndMgr = MWBase::Environment::get().getSoundManager();
            if (!sndMgr->getSoundPlaying(ptr, "surf"))
                sndMgr->playSound3D(ptr, "surf", 1.0f, 1.0f, MWSound::Type::Sfx, MWSound::PlayMode::Loop);
        }

        btVector3 calculateSurfVelocity(const btVector3& currentVel, const btVector3& normal, const btVector3& inputDir, float dt)
        {
            btVector3 newVelocity = currentVel;
            btVector3 surfRight = normal.cross(btVector3(0, 0, 1)).normalized();

            if (!inputDir.isZero())
            {
                float dot = surfRight.dot(inputDir);
                btVector3 strafeDir = surfRight * dot;
                float currentSpeed = newVelocity.length();
                float addSpeed = std::min(MAX_AIR_SPEED - currentSpeed, AIR_ACCELERATE * dt);
                if (addSpeed > 0)
                    newVelocity += strafeDir * addSpeed;
            }
            return newVelocity;
        }

        void updateSurfingState(Actor* actor, const btVector3& normal)
        {
            actor->setSurfing(isSurfableSurface(normal));
            btCollisionObject* collisionObject = actor->getCollisionObject();
            if (collisionObject)
                collisionObject->setFriction(SURF_FRICTION);
        }

        float getSurfingSpeed(const btVector3& velocity)
        {
            return velocity.length();
        }

        bool shouldPreserveMomentum(const btVector3& normal, const btVector3& velocity)
        {
            if (!isSurfableSurface(normal))
                return false;
            float speedSquared = velocity.length2();
            return speedSquared > 100.0f;
        }
    }
}
