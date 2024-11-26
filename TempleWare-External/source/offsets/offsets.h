#pragma once

#include <locale>

namespace offsets {
    // Game offsets
    constexpr std::ptrdiff_t dwLocalPlayerPawn = 0x1854CF8;
    constexpr std::ptrdiff_t dwLocalPlayerController = 0x1A40F90;
    constexpr std::ptrdiff_t dwEntityList = 0x19F1468;
    constexpr std::ptrdiff_t dwViewMatrix = 0x1A53510;
    constexpr std::ptrdiff_t dwViewAngles = 0x1A5D610;
    constexpr std::ptrdiff_t m_vecOrigin = 0x568;

    // Player details
    constexpr std::ptrdiff_t m_pCameraServices = 0x11E0;
    constexpr std::ptrdiff_t m_glowColorOverride = 0x40;
    constexpr std::ptrdiff_t m_iFOV = 0x210;
    constexpr std::ptrdiff_t m_bGlowing = 0x51;
    constexpr std::ptrdiff_t m_bIsScoped = 0x23E8;
    constexpr std::ptrdiff_t attack = 0x184D4E0;
    constexpr std::ptrdiff_t m_iIDEntIndex = 0x1458;
    constexpr std::ptrdiff_t flFlashDuration = 0x140C;

    // Entity details
    constexpr std::ptrdiff_t m_hPawn = 0x62C;
    constexpr std::ptrdiff_t m_hPlayerPawn = 0x80C;
    constexpr std::ptrdiff_t m_Glow = 0xC00;
    constexpr std::ptrdiff_t m_iHealth = 0x344;
    constexpr std::ptrdiff_t m_iTeamNum = 0x3E3;
    constexpr std::ptrdiff_t m_vOldOrigin = 0x1324;
    constexpr std::ptrdiff_t m_entitySpottedState = 0x11A8;
    constexpr std::ptrdiff_t m_vecViewOffset = 0xCB0;
    constexpr std::ptrdiff_t m_fFlags = 0x63;
}
