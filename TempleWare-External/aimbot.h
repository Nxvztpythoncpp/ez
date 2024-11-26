#pragma once
#include <vector>
#include "vector.hpp"

class Aimbot
{
public:
	void doAimbot();
private:
	Vector findClosest(const std::vector<Vector> playerPositions);
	void MoveMouseToPlayer(Vector position);
};

inline Aimbot aimbot;