#include "glow.h"
#include "../offsets/globals.h"
#include "../offsets/offsets.h"
#include <thread>

namespace features {
    void Glow::Run(const Memory& memory) noexcept {
        // Check if Glow is enabled
        if (!globals::Glow)
            return;

        // Read the local player controller pointer
        const uintptr_t localPlayerController = memory.Read<uintptr_t>(globals::client + offsets::dwLocalPlayerController);
        if (!localPlayerController)
            return;

        // Read the local player's team number
        int localTeam = memory.Read<int>(localPlayerController + offsets::m_iTeamNum);

        // Base address of the entity list
        uintptr_t entityList = memory.Read<uintptr_t>(globals::client + offsets::dwEntityList);
        if (!entityList)
            return;

        // Iterate over potential entities
        for (int i = 1; i < 64; i++) {
            // Calculate the entity entry
            uintptr_t listEntry = memory.Read<uintptr_t>(entityList + (8 * ((i & 0x7FFF) >> 9)) + 16);
            if (!listEntry)
                continue;

            // Read the player object
            uintptr_t player = memory.Read<uintptr_t>(listEntry + 120 * (i & 0x1FF));
            if (!player)
                continue;

            // Check if the player is on the same team
            int playerTeam = memory.Read<int>(player + offsets::m_iTeamNum);
            if (playerTeam == localTeam)
                continue;

            // Read the player pawn handle
            uint32_t playerPawnHandle = memory.Read<uint32_t>(player + offsets::m_hPlayerPawn);

            // Calculate the player's pawn entry
            uintptr_t listEntry2 = memory.Read<uintptr_t>(entityList + (8 * ((playerPawnHandle & 0x7FFF) >> 9)) + 16);
            if (!listEntry2)
                continue;

            // Read the CS2 Pawn object
            uintptr_t playerCsPawn = memory.Read<uintptr_t>(listEntry2 + 120 * (playerPawnHandle & 0x1FF));
            if (!playerCsPawn)
                continue;

            // Convert ImVec4 color to ARGB format
            ImVec4 color = globals::GlowColor;
            DWORD colorArgb = ((DWORD)(color.w * 255) << 24) | ((DWORD)(color.z * 255) << 16) | ((DWORD)(color.y * 255) << 8) | (DWORD)(color.x * 255);

            // Write glow data
            memory.Write<DWORD64>(playerCsPawn + offsets::m_Glow + offsets::m_glowColorOverride, colorArgb);
            memory.Write<DWORD64>(playerCsPawn + offsets::m_Glow + offsets::m_bGlowing, 1);
        }
    }
}
