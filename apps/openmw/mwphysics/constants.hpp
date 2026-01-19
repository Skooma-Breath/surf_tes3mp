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
    static constexpr float sMaxSlope = 49.0f; //49 is default    44 for surfing???

    // Arbitrary number. To prevent infinite loops. They shouldn't happen but it's good to be prepared.
    static constexpr int sMaxIterations = 8;
    // Allows for more precise movement solving without getting stuck or snagging too easily.
    static constexpr float sCollisionMargin = 0.1f; // experiment with this and see how surfing feels
    // Allow for a small amount of penetration to prevent numerical precision issues from causing the "unstuck"ing code to run unnecessarily
    // Currently set to 0 because having the "unstuck"ing code run whenever possible prevents some glitchy snagging issues
    static constexpr float sAllowedPenetration = 0.0f;

    //TODO: maybe find a better place for these?....
    // Surf mechanic vars - made non-const so they can be modified via Lua
    inline float AIR_ACCEL = 70.0f;
    inline float MAX_AIR_SPEED = 2000.0f;
    inline float FRICTION = 5.0f;
    inline float GROUND_ACCEL = 10.0f;
    inline float JUMP_SPEED = 268.0f;
    inline float GRAVITY_MULT = 1.0f;        // Multiplier for gravity (1.0 = normal, 0.5 = half gravity, 2.0 = double)
    inline float OVERBOUNCE = 1.6f;          // Bounce multiplier for collision response
    inline float RAMP_ANGLE = 0.8f;          // Check if a slope is walkable (Source Engine uses normal.z >= 0.7) (0.8 = ~36 degrees)

    inline bool SURF_PHYSICS_ENABLED = false; // surf physics boolean toggle

    inline float OVERBOUNCE_IMPACT = 1.1f;
    //inline float OVERBOUNCE_SURF = 1.6f;
    //inline float OVERBOUNCE_WALL = 1.1f;
    inline float IMPACT_VELOCITY_THRESHOLD = 200.0f;

}

#endif
