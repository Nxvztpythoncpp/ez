#include "TriggerBot.h"
#include "../offsets/globals.h"
#include "../offsets/offsets.h"
#include <thread>
#include <Windows.h>

namespace features {

    namespace {
        // Constants for readability
        constexpr int MAX_ATTACK_FLAG = 65537;
        constexpr int RELEASE_ATTACK_FLAG = 256;
        constexpr int KEY_PRESSED_FLAG = 0x8000;
        constexpr int DEBOUNCE_DELAY_MS = 200;

        // Debug logging macro
#ifdef DEBUG_TRIGGERBOT
#include <iostream>
#define DEBUG_LOG(msg) std::cout << "[TriggerBot] " << msg << std::endl;
#else
#define DEBUG_LOG(msg)
#endif

        // Helper function: Sleep for a specific duration
        inline void SleepForMs(int ms) {
            std::this_thread::sleep_for(std::chrono::milliseconds(ms));
        }

        // Helper function: Safely read memory
        template<typename T>
        inline T SafeRead(const Memory& memory, std::uintptr_t address) {
            return address ? memory.Read<T>(address) : T{};
        }

        // Helper function: Validate an entity pointer
        inline bool IsValidEntity(std::uintptr_t entity, const Memory& memory) {
            return entity && SafeRead<int>(memory, entity + offsets::m_iHealth) > 0;
        }

        // Helper function: Check if flash duration affects the player
        inline bool IsFlashed(std::uintptr_t localPlayer, const Memory& memory) {
            return SafeRead<float>(memory, localPlayer + offsets::flFlashDuration) > 0.0f;
        }

        // Helper function: Perform an attack action
        inline void PerformAttack(const Memory& memory) {
            memory.Write<int>(globals::client + offsets::attack, MAX_ATTACK_FLAG);
            SleepForMs(globals::TriggerBotDelay);
            memory.Write<int>(globals::client + offsets::attack, RELEASE_ATTACK_FLAG);
        }

        // Helper function: Check if a key is pressed
        inline bool IsKeyPressed(int key) {
            return GetAsyncKeyState(key) & KEY_PRESSED_FLAG;
        }

        // Helper function: Toggle TriggerBot mode
        inline void HandleToggleMode() {
            if (globals::TriggerBotMode == 1 && IsKeyPressed(globals::TriggerBotKey)) {
                globals::TriggerBotToggled = !globals::TriggerBotToggled;
                SleepForMs(DEBOUNCE_DELAY_MS);
            }
        }
    }

    void TriggerBot::Run(const Memory& memory) noexcept {
        while (globals::isRunning) {
            if (!globals::TriggerBot) {
                SleepForMs(10); // Reduced idle delay
                continue;
            }

            // Handle TriggerBot toggle mode
            HandleToggleMode();
            if (globals::TriggerBotMode == 1 && !globals::TriggerBotToggled) {
                SleepForMs(5); // Further reduced delay
                continue;
            }

            if (globals::TriggerBotMode == 0 && !IsKeyPressed(globals::TriggerBotKey)) {
                SleepForMs(5);
                continue;
            }

            // Retrieve local player
            const std::uintptr_t localPlayer = SafeRead<std::uintptr_t>(memory, globals::client + offsets::dwLocalPlayerPawn);
            if (!localPlayer) {
                DEBUG_LOG("Local player not found.");
                SleepForMs(5);
                continue;
            }

            // Check if player is flashed (optional)
            if (!globals::TriggerBotIgnoreFlash && IsFlashed(localPlayer, memory)) {
                DEBUG_LOG("Player is flashed.");
                SleepForMs(5);
                continue;
            }

            // Get entity under the crosshair
            const int crosshairEntityIndex = SafeRead<int>(memory, localPlayer + offsets::m_iIDEntIndex);
            if (crosshairEntityIndex <= 0) { // Simplified invalid check
                SleepForMs(5);
                continue;
            }

            // Retrieve entity list and resolve the entity
            const std::uintptr_t entityList = SafeRead<std::uintptr_t>(memory, globals::client + offsets::dwEntityList);
            if (!entityList) {
                DEBUG_LOG("Entity list not found.");
                SleepForMs(5);
                continue;
            }

            const std::uintptr_t listEntry = SafeRead<std::uintptr_t>(
                memory, entityList + 0x8 * (crosshairEntityIndex >> 9) + 0x10);
            const std::uintptr_t entity = SafeRead<std::uintptr_t>(
                memory, listEntry + 120 * (crosshairEntityIndex & 0x1FF));

            if (!IsValidEntity(entity, memory)) {
                DEBUG_LOG("Invalid entity.");
                SleepForMs(5);
                continue;
            }

            // Perform team check if enabled
            if (globals::TriggerBotTeamCheck) {
                const BYTE localTeam = SafeRead<BYTE>(memory, localPlayer + offsets::m_iTeamNum);
                const BYTE entityTeam = SafeRead<BYTE>(memory, entity + offsets::m_iTeamNum);
                if (entityTeam == localTeam) {
                    DEBUG_LOG("Entity is on the same team.");
                    SleepForMs(5);
                    continue;
                }
            }

            // Attack the target
            DEBUG_LOG("Attacking target.");
            PerformAttack(memory);
        }
    }
}
     