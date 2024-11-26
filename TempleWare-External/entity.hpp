#include <vector>
#include <thread>
#include "memify.h"
#include "vector.hpp"

inline memify mem("cs2.exe");

namespace offset
{
	constexpr std::ptrdiff_t dwEntityList = 0x19F1468;
	constexpr std::ptrdiff_t dwViewMatrix = 0x1A53510;
	constexpr std::ptrdiff_t dwLocalPlayerPawn = 0x1854CF8;

	constexpr std::ptrdiff_t m_hPlayerPawn = 0x62C;

	constexpr std::ptrdiff_t m_iHealth = 0x344;
	constexpr std::ptrdiff_t m_iTeamNum = 0x3E3;

	constexpr std::ptrdiff_t m_vOldOrigin = 0x1324;

	constexpr std::ptrdiff_t m_entitySpottedState = 0x2288; // 0x8 m_bSpotted
}

// create an entity class for our vector, since we need to be able to push_back().
class C_CSPlayerPawn
{
public:
	int health, team;

	Vector Position;

	uintptr_t pCSPlayerPawn;
};

inline C_CSPlayerPawn CCSPlayerPawn;

// create a class for filtering players, and our new thread.
class Reader
{
public:
	uintptr_t client = 0;

	std::vector<C_CSPlayerPawn> playerList;

	void ThreadLoop();
private:
	void FilterPlayers();
};

inline Reader reader;

