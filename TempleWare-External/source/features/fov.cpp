#include "fov.h"
#include "../offsets/globals.h"
#include "../offsets/offsets.h"

namespace features {

    // Helper function to safely read a pointer
    template <typename T>
    T SafeReadPointer(const Memory& memory, std::uintptr_t address) noexcept {
        return (address != 0) ? memory.Read<T>(address) : T{};
    }

    void FOVManager::AdjustFOV(const Memory& memory) noexcept {
        // Constants for clarity
        constexpr std::uintptr_t NULLPTR = 0;

        // Read the local player pointer
        const std::uintptr_t localPlayer = SafeReadPointer<std::uintptr_t>(memory, globals::client + offsets::dwLocalPlayerPawn);
        if (localPlayer == NULLPTR) {
            return; // Exit if local player is null
        }

        // Read the camera services pointer
        const std::uintptr_t cameraServices = SafeReadPointer<std::uintptr_t>(memory, localPlayer + offsets::m_pCameraServices);
        if (cameraServices == NULLPTR) {
            return; // Exit if camera services is null
        }

        // Read the current FOV value
        const std::uint16_t currentFov = SafeReadPointer<std::uint16_t>(memory, cameraServices + offsets::m_iFOV);

        // Check if the player is scoped
        const bool isScoped = SafeReadPointer<bool>(memory, localPlayer + offsets::m_bIsScoped);

        // Define the desired FOV
        const std::uint16_t desiredFov = static_cast<std::uint16_t>(globals::FOV);

        // Adjust FOV only if the player is not scoped and the current FOV is different from the desired FOV
        if (!isScoped && currentFov != desiredFov) {
            memory.Write<std::uint16_t>(cameraServices + offsets::m_iFOV, desiredFov);
        }
    }

} // namespace features
