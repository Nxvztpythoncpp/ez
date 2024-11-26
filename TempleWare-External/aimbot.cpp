#include "entity.hpp"
#include "aimbot.h"
#include <limits>

void Aimbot::doAimbot()
{
    static const std::chrono::milliseconds sleepDuration(1);
    std::this_thread::sleep_for(sleepDuration);

    reader.ThreadLoop();

    auto view_matrix = mem.Read<view_matrix_t>(reader.client + offset::dwViewMatrix);

    std::vector<Vector> playerPositions;
    playerPositions.clear();

    for (const auto& player : reader.playerList)
    {
        Vector playerPosition = mem.Read<Vector>(player.pCSPlayerPawn + offset::m_vOldOrigin);
        Vector headPos{ playerPosition.x, playerPosition.y, playerPosition.z + 65.0f };

        Vector f, h;
        if (Vector::world_to_screen(view_matrix, playerPosition, f) &&
            Vector::world_to_screen(view_matrix, headPos, h))
        {
            playerPositions.push_back(h);
        }
    }

    if (GetAsyncKeyState(VK_RBUTTON))
    {
        if (!playerPositions.empty())
        {
            auto closest_player = Aimbot::findClosest(playerPositions);
            MoveMouseToPlayer(closest_player);
        }
    }
}

Vector Aimbot::findClosest(const std::vector<Vector> playerPositions)
{
    if (playerPositions.empty())
    {
        return { 0.0f, 0.0f, 0.0f };
    }

    static const float screenWidth = static_cast<float>(GetSystemMetrics(0));
    static const float screenHeight = static_cast<float>(GetSystemMetrics(1));
    static const Vector center_of_screen{ screenWidth / 2.0f, screenHeight / 2.0f, 0.0f };

    float lowestDistance = 10000.f;
    size_t index = 0;

    for (size_t i = 0; i < playerPositions.size(); ++i)
    {
        float distance = std::sqrt(
            std::pow(playerPositions[i].x - center_of_screen.x, 2) +
            std::pow(playerPositions[i].y - center_of_screen.y, 2)
        );

        if (distance < lowestDistance) {
            lowestDistance = distance;
            index = i;
        }
    }

    return { playerPositions[index].x, playerPositions[index].y, 0.0f };
}

void Aimbot::MoveMouseToPlayer(Vector position)
{
    if (position.IsZero())
        return;

    static const float screenWidth = static_cast<float>(GetSystemMetrics(0));
    static const float screenHeight = static_cast<float>(GetSystemMetrics(1));
    static const Vector center_of_screen{ screenWidth / 2.0f, screenHeight / 2.0f, 0.0f };

    auto new_x = static_cast<long>(position.x - center_of_screen.x);
    auto new_y = static_cast<long>(position.y - center_of_screen.y);

    mouse_event(MOUSEEVENTF_MOVE, new_x, new_y, 0, 0);
}