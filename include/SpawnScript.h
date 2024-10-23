#pragma once
#include <string>

struct SpawnScript {
	float x;
	float y;
	std::string name;
	bool goLeft;
	bool spawned = false;
	SpawnScript(float posX, float posY, std::string& enemyName, bool GoLeft);
};
