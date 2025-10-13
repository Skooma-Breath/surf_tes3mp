#ifndef OPENMW_SETTINGSAPI_HPP
#define OPENMW_SETTINGSAPI_HPP

#include "../Types.hpp"

#define SETTINGSAPI \
    {"SetDifficulty",               SettingFunctions::SetDifficulty},\
    {"SetEnforcedLogLevel",         SettingFunctions::SetEnforcedLogLevel},\
    {"SetPhysicsFramerate",         SettingFunctions::SetPhysicsFramerate},\
    \
    {"SetConsoleAllowed",           SettingFunctions::SetConsoleAllowed},\
    {"SetBedRestAllowed",           SettingFunctions::SetBedRestAllowed},\
    {"SetWildernessRestAllowed",    SettingFunctions::SetWildernessRestAllowed},\
    {"SetWaitAllowed",              SettingFunctions::SetWaitAllowed},\
    \
    {"SetGameSettingValue",         SettingFunctions::SetGameSettingValue},\
    {"ClearGameSettingValues",      SettingFunctions::ClearGameSettingValues},\
    \
    {"SetVRSettingValue",           SettingFunctions::SetVRSettingValue},\
    {"ClearVRSettingValues",        SettingFunctions::ClearVRSettingValues},\
    \
    {"GetAirAccel",                 SettingFunctions::GetAirAccel},\
    {"GetMaxAirSpeed",              SettingFunctions::GetMaxAirSpeed},\
    {"GetFriction",                 SettingFunctions::GetFriction},\
    {"GetGroundAccel",              SettingFunctions::GetGroundAccel},\
    {"GetJumpSpeed",                SettingFunctions::GetJumpSpeed},\
    {"GetGravityMult",              SettingFunctions::GetGravityMult},\
    {"GetOverbounce",               SettingFunctions::GetOverbounce},\
    {"GetRampAngle",                SettingFunctions::GetRampAngle},\
    \
    {"SetAirAccel",                 SettingFunctions::SetAirAccel},\
    {"SetMaxAirSpeed",              SettingFunctions::SetMaxAirSpeed},\
    {"SetFriction",                 SettingFunctions::SetFriction},\
    {"SetGroundAccel",              SettingFunctions::SetGroundAccel},\
    {"SetJumpSpeed",                SettingFunctions::SetJumpSpeed},\
    {"SetGravityMult",              SettingFunctions::SetGravityMult},\
    {"SetOverbounce",               SettingFunctions::SetOverbounce},\
    {"SetRampAngle",                SettingFunctions::SetRampAngle},\
    {"GetSurfPhysicsEnabled",       SettingFunctions::GetSurfPhysicsEnabled},\
    {"SetSurfPhysicsEnabled",       SettingFunctions::SetSurfPhysicsEnabled},\
    \
    {"SendSettings",                SettingFunctions::SendSettings}

class SettingFunctions
{
public:

    /**
    * \brief Set the difficulty for a player.
    *
    * This changes the difficulty for that player in the server memory, but does not by itself
    * send a packet.
    *
    * \param pid The player ID.
    * \param difficulty The difficulty.
    * \return void
    */
    static void SetDifficulty(unsigned short pid, int difficulty);

    /**
    * \brief Set the client log level enforced for a player.
    *
    * This changes the enforced log level for that player in the server memory, but does not by itself
    * send a packet.
    *
    * Enforcing a certain log level is necessary to prevent players from learning information from
    * their console window that they are otherwise unable to obtain, such as the locations of
    * other players.
    *
    * If you do not wish to enforce a log level, simply set enforcedLogLevel to -1
    *
    * \param pid The player ID.
    * \param enforcedLogLevel The enforced log level.
    * \return void
    */
    static void SetEnforcedLogLevel(unsigned short pid, int enforcedLogLevel);

    /**
    * \brief Set the physics framerate for a player.
    *
    * This changes the physics framerate for that player in the server memory, but does not by itself
    * send a packet.
    *
    * \param pid The player ID.
    * \param physicsFramerate The physics framerate.
    * \return void
    */
    static void SetPhysicsFramerate(unsigned short pid, double physicsFramerate);

    /**
    * \brief Set whether the console is allowed for a player.
    *
    * This changes the console permission for that player in the server memory, but does not
    * by itself send a packet.
    *
    * \param pid The player ID.
    * \param state The console permission state.
    * \return void
    */
    static void SetConsoleAllowed(unsigned short pid, bool state);

    /**
    * \brief Set whether resting in beds is allowed for a player.
    *
    * This changes the resting permission for that player in the server memory, but does not
    * by itself send a packet.
    *
    * \param pid The player ID.
    * \param state The resting permission state.
    * \return void
    */
    static void SetBedRestAllowed(unsigned short pid, bool state);

    /**
    * \brief Set whether resting in the wilderness is allowed for a player.
    *
    * This changes the resting permission for that player in the server memory, but does not
    * by itself send a packet.
    *
    * \param pid The player ID.
    * \param state The resting permission state.
    * \return void
    */
    static void SetWildernessRestAllowed(unsigned short pid, bool state);

    /**
    * \brief Set whether waiting is allowed for a player.
    *
    * This changes the waiting permission for that player in the server memory, but does not
    * by itself send a packet.
    *
    * \param pid The player ID.
    * \param state The waiting permission state.
    * \return void
    */
    static void SetWaitAllowed(unsigned short pid, bool state);

    /**
    * \brief Set value for a game setting.
    *
    * This overrides the setting value set in OpenMW Launcher. Only applies to the Game category.
    *
    * \param pid The player ID.
    * \param setting Name of a setting in the Game category
    * \param value Value of the setting (as a string)
    * \return void
    */
    static void SetGameSettingValue(unsigned short pid, const char* setting, const char* value);

    /**
    * \brief Clear the Game setting values stored for a player.
    *
    * Clear any changes done by SetGameSettingValue()
    *
    * \param pid The player ID.
    * \return void
    */
    static void ClearGameSettingValues(unsigned short pid);

    /**
    * \brief Set value for a VR setting.
    *
    * This overrides the setting value set in OpenMW Launcher. Only applies to the VR category.
    *
    * \param pid The player ID.
    * \param setting Name of a setting in the VR category
    * \param value Value of the setting (as a string)
    * \return void
    */
    static void SetVRSettingValue(unsigned short pid, const char* setting, const char* value);

    /**
    * \brief Clear the VR setting values stored for a player.
    *
    * Clear any changes done by SetVRSettingValue()
    *
    * \param pid The player ID.
    * \return void
    */
    static void ClearVRSettingValues(unsigned short pid);

    /**
    * \brief Get the air acceleration value for a player.
    *
    * \param pid The player ID.
    * \return The air acceleration value.
    */
    static double GetAirAccel(unsigned short pid) noexcept;

    /**
    * \brief Get the maximum air speed value for a player.
    *
    * \param pid The player ID.
    * \return The maximum air speed value.
    */
    static double GetMaxAirSpeed(unsigned short pid) noexcept;

    /**
    * \brief Get the ground friction value for a player.
    *
    * \param pid The player ID.
    * \return The ground friction value.
    */
    static double GetFriction(unsigned short pid) noexcept;

    /**
    * \brief Get the ground acceleration value for a player.
    *
    * \param pid The player ID.
    * \return The ground acceleration value.
    */
    static double GetGroundAccel(unsigned short pid) noexcept;

    /**
    * \brief Get the jump speed value for a player.
    *
    * \param pid The player ID.
    * \return The jump speed value.
    */
    static double GetJumpSpeed(unsigned short pid) noexcept;

    /**
    * \brief Get the gravity multiplier value for a player.
    *
    * \param pid The player ID.
    * \return The gravity multiplier value.
    */
    static double GetGravityMult(unsigned short pid) noexcept;

    /**
    * \brief Get the overbounce value for a player.
    *
    * \param pid The player ID.
    * \return The overbounce value.
    */
    static double GetOverbounce(unsigned short pid) noexcept;

    /**
    * \brief Get the rampAngle value for a player.
    *
    * \param pid The player ID.
    * \return The rampAngle value.
    */
    static double GetRampAngle(unsigned short pid) noexcept;

    /**
    * \brief Get whether surf physics are enabled for a player.
    *
    * \param pid The player ID.
    * \return True if surf physics are enabled, false otherwise.
    */
    static bool GetSurfPhysicsEnabled(unsigned short pid) noexcept;

    /**
    * \brief Set the air acceleration value for a player.
    *
    * This changes the value for that player in the server memory, but does not by itself send a packet.
    *
    * \param pid The player ID.
    * \param value The new air acceleration value.
    * \return void
    */
    static void SetAirAccel(unsigned short pid, double value) noexcept;

    /**
    * \brief Set the maximum air speed value for a player.
    *
    * This changes the value for that player in the server memory, but does not by itself send a packet.
    *
    * \param pid The player ID.
    * \param value The new maximum air speed value.
    * \return void
    */
    static void SetMaxAirSpeed(unsigned short pid, double value) noexcept;

    /**
    * \brief Set the ground friction value for a player.
    *
    * This changes the value for that player in the server memory, but does not by itself send a packet.
    *
    * \param pid The player ID.
    * \param value The new ground friction value.
    * \return void
    */
    static void SetFriction(unsigned short pid, double value) noexcept;

    /**
    * \brief Set the ground acceleration value for a player.
    *
    * This changes the value for that player in the server memory, but does not by itself send a packet.
    *
    * \param pid The player ID.
    * \param value The new ground acceleration value.
    * \return void
    */
    static void SetGroundAccel(unsigned short pid, double value) noexcept;

    /**
    * \brief Set the jump speed value for a player.
    *
    * This changes the value for that player in the server memory, but does not by itself send a packet.
    *
    * \param pid The player ID.
    * \param value The new jump speed value.
    * \return void
    */
    static void SetJumpSpeed(unsigned short pid, double value) noexcept;

    /**
    * \brief Send a PlayerSettings packet to the player affected by it.
    *
    * \param pid The player ID to send it to.
    * \return void
    */

    /**
    * \brief Set the gravity multiplier value for a player.
    *
    * This changes the value for that player in the server memory, but does not by itself send a packet.
    *
    * \param pid The player ID.
    * \param value The new gravity multiplier value (1.0 = normal, 0.5 = half gravity, 2.0 = double).
    * \return void
    */
    static void SetGravityMult(unsigned short pid, double value) noexcept;

    /**
    * \brief Set the overbounce value for a player.
    *
    * This changes the value for that player in the server memory, but does not by itself send a packet.
    *
    * \param pid The player ID.
    * \param value The new overbounce value.
    * \return void
    */
    static void SetOverbounce(unsigned short pid, double value) noexcept;

    /**
    * \brief Set the rampangle value for a player.
    *
    * This changes the value for that player in the server memory, but does not by itself send a packet.
    *
    * \param pid The player ID.
    * \param value The new rampAngle value.
    * \return void
    */
    static void SetRampAngle(unsigned short pid, double value) noexcept;

    /**
    * \brief Set whether surf physics are enabled for a player.
    *
    * This changes the value for that player in the server memory, but does not by itself send a packet.
    *
    * \param pid The player ID.
    * \param state The new state for surf physics (true for enabled, false for disabled).
    * \return void
    */
    static void SetSurfPhysicsEnabled(unsigned short pid, bool state) noexcept;

    static void SendSettings(unsigned short pid, bool sendToOtherPlayers, bool skipAttachedPlayer) noexcept;
};

#endif //OPENMW_SETTINGSAPI_HPP
