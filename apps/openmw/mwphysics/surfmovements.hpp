#ifndef OPENMW_MWPHYSICS_SURFMOVEMENTS_H
#define OPENMW_MWPHYSICS_SURFMOVEMENTS_H

#include <BulletCollision/CollisionDispatch/btCollisionWorld.h>
#include "../mwworld/ptr.hpp"
#include "actor.hpp"
#include "physicssystem.hpp"

class btVector3;

namespace MWPhysics
{
    struct ActorFrameData;

    namespace SurfMovements
    {
        // Constants for surf mechanics
        extern float SURF_ANGLE_MIN;
        extern float SURF_ANGLE_MAX;
        extern float SURF_FRICTION;
        extern float SURF_GRAVITY;
        extern float AIR_ACCELERATE;
        extern float MAX_AIR_SPEED;
        extern float SURF_STICK_FORCE;
        
        // Add setter functions
        void setSurfAngleMin(float value);
        void setSurfAngleMax(float value);
        void setSurfFriction(float value);
        void setSurfGravity(float value);
        void setAirAccelerate(float value);
        void setMaxAirSpeed(float value);
        void setSurfStickForce(float value);
        
        // Add getter functions
        float getSurfAngleMin();
        float getSurfAngleMax();
        float getSurfFriction();
        float getSurfGravity();
        float getAirAccelerate();
        float getMaxAirSpeed();
        float getSurfStickForce();

        bool isSurfableSurface(const btVector3& normal);
        void handleSurfMovement(ActorFrameData& actorData, const btVector3& normal, float dt);
        void applySurfingEffects(const MWWorld::Ptr& ptr);
        btVector3 calculateSurfVelocity(const btVector3& currentVel, const btVector3& normal, 
                                      const btVector3& inputDir, float dt);
        void updateSurfingState(Actor* actor, const btVector3& normal);
        float getSurfingSpeed(const btVector3& velocity);
        bool shouldPreserveMomentum(const btVector3& normal, const btVector3& velocity);
    }
}

#endif