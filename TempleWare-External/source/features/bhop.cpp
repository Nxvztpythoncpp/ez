#include "bhop.h"
#include "../offsets/globals.h"
#include "../offsets/offsets.h"

namespace features
{
    void Bhop::Run(const Memory& memory) noexcept
    {
        // Get the local player pointer
        std::uintptr_t localPlayer = memory.Read<std::uintptr_t>(globals::client + offsets::dwLocalPlayerPawn);
        if (localPlayer == 0)
            return;

        // Find the CS2 window
        HWND hwnd_cs2 = FindWindowA(NULL, "Counter-Strike 2");
        if (!hwnd_cs2)
            return;

        // Check if SPACE is pressed
        bool isSpacePressed = GetAsyncKeyState(VK_SPACE) & 0x8000;
        int flags = memory.Read<int>(localPlayer + offsets::m_fFlags);

        // Check if the player is in the air
        bool isInAir = (flags & 1) == 0;

        // Handle Bunny Hop logic
        if (isSpacePressed)
        {
            if (isInAir)
            {
                // Player is in the air, simulate a quick release and press of SPACE
                SendMessage(hwnd_cs2, WM_KEYUP, VK_SPACE, 0);
                SendMessage(hwnd_cs2, WM_KEYDOWN, VK_SPACE, 0);
            }
        }
        else
        {
            // Ensure SPACE is released
            SendMessage(hwnd_cs2, WM_KEYUP, VK_SPACE, 0);
        }
    }
}
