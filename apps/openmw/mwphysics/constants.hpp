#ifndef OPENMW_MWPHYSICS_CONSTANTS_H
#define OPENMW_MWPHYSICS_CONSTANTS_H

namespace MWPhysics
{
    static constexpr float sStepSizeUp = 34.0f;
    static constexpr float sStepSizeDown = 62.0f;

    static constexpr float sMinStep = 10.0f; // hack to skip over tiny unwalkable slopes
    static constexpr float sMinStep2 = 20.0f; // hack to skip over shorter but longer/wider/further unwalkable slopes
    // whether to do the above stairstepping logic hacks to work around bad morrowind assets - disabling causes problems but improves performance
    static constexpr bool sDoExtraStairHacks = false;

    static constexpr float sGroundOffset = 1.0f;
    static constexpr float sMaxSlope = 44.0f; //49 is default

    // Arbitrary number. To prevent infinite loops. They shouldn't happen but it's good to be prepared.
    static constexpr int sMaxIterations = 8;
    // Allows for more precise movement solving without getting stuck or snagging too easily.
    static constexpr float sCollisionMargin = 0.1f;
    // Allow for a small amount of penetration to prevent numerical precision issues from causing the "unstuck"ing code to run unnecessarily
    // Currently set to 0 because having the "unstuck"ing code run whenever possible prevents some glitchy snagging issues
    static constexpr float sAllowedPenetration = 0.0f;

    // Surf mechanic vars - made non-const so they can be modified via Lua
    inline float AIR_ACCEL = 70.0f;
    inline float MAX_AIR_SPEED = 2000.0f;
    inline float FRICTION = 5.0f;
    inline float GROUND_ACCEL = 10.0f;
    inline float JUMP_SPEED = 268.0f;
    inline float GRAVITY_MULT = 1.0f;        // Multiplier for gravity (1.0 = normal, 0.5 = half gravity, 2.0 = double)
    inline float OVERBOUNCE = 1.1f;          // Bounce multiplier for collision response

}

#endif
