#ifndef OPENMW_MWPHYSICS_MOVEMENTSOLVER_H
#define OPENMW_MWPHYSICS_MOVEMENTSOLVER_H

#include <osg/Vec3f>

#include "constants.hpp"
#include "../mwworld/ptr.hpp"

#include <iostream>

class btCollisionWorld;

namespace MWWorld
{
    class Ptr;
}

namespace MWPhysics
{
    /// Vector projection
    static inline osg::Vec3f project(const osg::Vec3f& u, const osg::Vec3f &v)
    {
        return v * (u * v);
    }

    /// Vector rejection
    static inline osg::Vec3f reject(const osg::Vec3f& direction, const osg::Vec3f &planeNormal)
    {
        return direction - project(direction, planeNormal);
    }

    template <class Vec3>
    static bool isWalkableSlope(const Vec3 &normal)
    {
        static const float sMaxSlopeCos = std::cos(osg::DegreesToRadians(MWPhysics::sMaxSlope));
            bool result = (normal.z() > sMaxSlopeCos);
            /*std::cout << "isWalkableSlope - normal.z(): " << normal.z() 
                      << ", sMaxSlopeCos: " << sMaxSlopeCos 
                      << ", result: " << result << std::endl;*/
            //std::cout << "i think i sharted." << std::endl;
            return result;
    }

    class Actor;
    struct ActorFrameData;
    struct WorldFrameData;

    class MovementSolver
    {
    public:
        static osg::Vec3f traceDown(const MWWorld::Ptr &ptr, const osg::Vec3f& position, Actor* actor, btCollisionWorld* collisionWorld, float maxHeight);
        static void move(ActorFrameData& actor, float time, const btCollisionWorld* collisionWorld, WorldFrameData& worldData);
        static void unstuck(ActorFrameData& actor, const btCollisionWorld* collisionWorld);
        
        // SURF ADDITION
        static bool isSurfableSlope(const osg::Vec3f& normal);
        static osg::Vec3f calculateSlideDirection(const osg::Vec3f& normal);
    };
}

#endif
