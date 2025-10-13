#include "Settings.hpp"

#include <components/openmw-mp/NetworkMessages.hpp>
#include <components/openmw-mp/TimedLog.hpp>

#include <apps/openmw-mp/Script/ScriptFunctions.hpp>
#include <apps/openmw-mp/Networking.hpp>

#include <iostream>

void SettingFunctions::SetDifficulty(unsigned short pid, int difficulty)
{
    Player *player;
    GET_PLAYER(pid, player, );

    player->difficulty = difficulty;
}

void SettingFunctions::SetEnforcedLogLevel(unsigned short pid, int enforcedLogLevel)
{
    Player *player;
    GET_PLAYER(pid, player, );

    player->enforcedLogLevel = enforcedLogLevel;
}

void SettingFunctions::SetPhysicsFramerate(unsigned short pid, double physicsFramerate)
{
    Player *player;
    GET_PLAYER(pid, player, );

    player->physicsFramerate = physicsFramerate;
}

void SettingFunctions::SetConsoleAllowed(unsigned short pid, bool state)
{
    Player *player;
    GET_PLAYER(pid, player,);

    player->consoleAllowed = state;
}

void SettingFunctions::SetBedRestAllowed(unsigned short pid, bool state)
{
    Player *player;
    GET_PLAYER(pid, player, );

    player->bedRestAllowed = state;
}

void SettingFunctions::SetWildernessRestAllowed(unsigned short pid, bool state)
{
    Player *player;
    GET_PLAYER(pid, player, );

    player->wildernessRestAllowed = state;
}

void SettingFunctions::SetWaitAllowed(unsigned short pid, bool state)
{
    Player *player;
    GET_PLAYER(pid, player, );

    player->waitAllowed = state;
}

void SettingFunctions::SetGameSettingValue(unsigned short pid, const char* setting, const char* value) {
    Player* player;
    GET_PLAYER(pid, player, );

    player->gameSettings[setting] = value;
}

void SettingFunctions::ClearGameSettingValues(unsigned short pid) {
    Player* player;
    GET_PLAYER(pid, player, );

    player->gameSettings.clear();
}

void SettingFunctions::SetVRSettingValue(unsigned short pid, const char* setting, const char* value) {
    Player* player;
    GET_PLAYER(pid, player, );

    player->vrSettings[setting] = value;
}

void SettingFunctions::ClearVRSettingValues(unsigned short pid) {
    Player* player;
    GET_PLAYER(pid, player, );

    player->vrSettings.clear();
}

double SettingFunctions::GetAirAccel(unsigned short pid) noexcept
{
    Player *player;
    GET_PLAYER(pid, player, 0.0);

    return player->airAccel;
}

double SettingFunctions::GetMaxAirSpeed(unsigned short pid) noexcept
{
    Player *player;
    GET_PLAYER(pid, player, 0.0);

    return player->maxAirSpeed;
}

double SettingFunctions::GetFriction(unsigned short pid) noexcept
{
    Player *player;
    GET_PLAYER(pid, player, 0.0);

    return player->friction;
}

double SettingFunctions::GetGroundAccel(unsigned short pid) noexcept
{
    Player *player;
    GET_PLAYER(pid, player, 0.0);

    return player->groundAccel;
}

double SettingFunctions::GetJumpSpeed(unsigned short pid) noexcept
{
    Player *player;
    GET_PLAYER(pid, player, 0.0);

    return player->jumpSpeed;
}

double SettingFunctions::GetGravityMult(unsigned short pid) noexcept
{
    Player* player;
    GET_PLAYER(pid, player, 0.0);

    return player->gravityMult;
}

double SettingFunctions::GetOverbounce(unsigned short pid) noexcept
{
    Player* player;
    GET_PLAYER(pid, player, 0.0);

    return player->overbounce;
}

double SettingFunctions::GetRampAngle(unsigned short pid) noexcept
{
    Player* player;
    GET_PLAYER(pid, player, 0.0);

    return player->rampAngle;
}

bool SettingFunctions::GetSurfPhysicsEnabled(unsigned short pid) noexcept
{
    Player* player;
    GET_PLAYER(pid, player, false); // Default to false if player not found

    return player->surfPhysicsEnabled;
}

void SettingFunctions::SetAirAccel(unsigned short pid, double value) noexcept
{
    Player *player;
    GET_PLAYER(pid, player, );

    player->airAccel = static_cast<float>(value);
}

void SettingFunctions::SetMaxAirSpeed(unsigned short pid, double value) noexcept
{
    Player *player;
    GET_PLAYER(pid, player, );

    player->maxAirSpeed = static_cast<float>(value);
}

void SettingFunctions::SetFriction(unsigned short pid, double value) noexcept
{
    Player *player;
    GET_PLAYER(pid, player, );

    player->friction = static_cast<float>(value);
}

void SettingFunctions::SetGroundAccel(unsigned short pid, double value) noexcept
{
    Player *player;
    GET_PLAYER(pid, player, );

    player->groundAccel = static_cast<float>(value);
}

void SettingFunctions::SetJumpSpeed(unsigned short pid, double value) noexcept
{
    Player *player;
    GET_PLAYER(pid, player, );

    player->jumpSpeed = static_cast<float>(value);
}

void SettingFunctions::SetGravityMult(unsigned short pid, double value) noexcept
{
    Player* player;
    GET_PLAYER(pid, player, );

    player->gravityMult = static_cast<float>(value);
}

void SettingFunctions::SetOverbounce(unsigned short pid, double value) noexcept
{
    Player* player;
    GET_PLAYER(pid, player, );

    player->overbounce = static_cast<float>(value);
}

void SettingFunctions::SetRampAngle(unsigned short pid, double value) noexcept
{
    Player* player;
    GET_PLAYER(pid, player, );

    player->rampAngle = static_cast<float>(value);
}

void SettingFunctions::SetSurfPhysicsEnabled(unsigned short pid, bool state) noexcept
{
    Player* player;
    GET_PLAYER(pid, player, );

    player->surfPhysicsEnabled = state;
}

void SettingFunctions::SendSettings(unsigned short pid, bool sendToOtherPlayers, bool skipAttachedPlayer) noexcept
{
    Player *player;
    GET_PLAYER(pid, player,);

    mwmp::PlayerPacket *packet = mwmp::Networking::get().getPlayerPacketController()->GetPacket(ID_GAME_SETTINGS);
    packet->setPlayer(player);

    if (!skipAttachedPlayer)
        packet->Send(false);
    if (sendToOtherPlayers)
        packet->Send(true);
}
