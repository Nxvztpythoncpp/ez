#pragma once

#include <cstddef>
#include <locale>

#include "../../imgui/imgui.h"

#define VK_LSHIFT 0xA0

namespace globals {

    // Aimbot
    inline bool Aimbot = false;
    
    // TriggerBot
    inline bool TriggerBot = false;                    // TriggerBot activation state
    inline int TriggerBotKey = VK_LSHIFT;              // Key to activate TriggerBot
    inline char TriggerBotKeyName[64] = "L-Shift";     // Name of the key for display
    inline int TriggerBotMode = 0;                     // TriggerBot mode (Hold/Toggle)
    inline int TriggerBotDelay = 20;
    inline int TriggerHitChance = 10;                   // TriggerBot delay in milliseconds
    inline bool TriggerBotToggled = false;             // Toggle state for TriggerBot
    inline bool TriggerBotTeamCheck = true;            // Whether to shoot teammates or not
    inline bool TriggerBotIgnoreFlash = false;         // Whether TriggerBot works while flashed

    // Fov
    inline int FOV = 90;                               // Field of View setting

    // Glow
    inline bool Glow = false;                          // Glow activation state
    inline ImVec4 GlowColor = ImVec4(0.0f, 0.0f, 1.0f, 1.0f); // Glow color setting
    
    // NoFlash
    inline bool NoFlashEnabled = false;                // No Flash activation state

    // Menu
    inline ImVec4 MenuAccentColor = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);  // GUI color setting
    inline bool Rainbow = false;                       // Rainbow color mode activation
    inline bool MenuVisible = true;                    // Menu visibility state

    // Application state
    inline bool isRunning = true;                      // Whether the hack is running
    inline std::uintptr_t client = 0;                  // Client base address

    // Misc
    inline bool BunnyHopEnabled = false;               // Bhop activation state
}