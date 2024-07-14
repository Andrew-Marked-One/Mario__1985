#include "SpawnScript.h"

SpawnScript::SpawnScript(float posX, float posY, std::string& enemyName, bool GoLeft)
	: x(posX)
	, y(posY)
	, name(enemyName)
	, goLeft(GoLeft) {}
