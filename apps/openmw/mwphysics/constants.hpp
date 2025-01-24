#ifndef OPENMW_MWPHYSICS_CONSTANTS_H
#define OPENMW_MWPHYSICS_CONSTANTS_H

namespace MWPhysics
{
    static constexpr float sStepSizeUp = 34.0f;
    static constexpr float sStepSizeDown = 62.0f;

    static constexpr float sMinStep = 10.0f; // hack to skip over tiny unwalkable slopes
    static constexpr float sMinStep2 = 20.0f; // hack to skip over shorter but longer/wider/further unwalkable slopes
    // whether to do the above stairstepping logic hacks to work around bad morrowind assets - disabling causes problems but improves performance
    static constexpr bool sDoExtraStairHacks = true;

    static constexpr float sGroundOffset = 1.0f;
    static constexpr float sMaxSlope = 44.0f; //49 is default
    static constexpr float sSurfThreshold = 34.0f; 
    static constexpr float sSlideSpeed = 0.01f;

    // Arbitrary number. To prevent infinite loops. They shouldn't happen but it's good to be prepared.
    static constexpr int sMaxIterations = 8;
    // Allows for more precise movement solving without getting stuck or snagging too easily.
    static constexpr float sCollisionMargin = 0.1f;
    // Allow for a small amount of penetration to prevent numerical precision issues from causing the "unstuck"ing code to run unnecessarily
    // Currently set to 0 because having the "unstuck"ing code run whenever possible prevents some glitchy snagging issues
    static constexpr float sAllowedPenetration = 0.0f;
    
    // Surf mechanic constants
    // static constexpr float sSurfThreshold = 30.0f;   // Minimum angle for a surfable slope in degrees
    // static constexpr float sMaxSurfAngle = 60.0f;    // Maximum angle for a surfable slope in degrees
    // static constexpr float sSlideSpeed = 5.0f;       // Sliding speed on surfable slopes
    static constexpr float AirStrafeAcceleration = 3000.0f; // Adjust based on desired responsiveness
    // Define the base air control factor (e.g., 0.5 means the player has half the control in the air as on the ground)
    static constexpr float baseAirControl = 1.0f;
    static constexpr float maxAirSpeed = 1.0f;

}

#endif
