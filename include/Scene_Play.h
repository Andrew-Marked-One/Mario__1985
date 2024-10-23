#pragma once
#include <cstdint>
#include "Scene.h"
#include "SpawnScript.h"

class Scene_Play : public Scene {
	struct PlayerConfig {
		//gridX, gridY, BBoxX, BBoxY, speed left/right, jump speed, max jump speed, max speed left/right/down, gravity, weapon animation
		float X, Y, CX, CY, SPEED, JUMP, MAXJUMP, MAXSPEED, GRAVITY;
		std::string WEAPON;
	};
	struct EnemyConfig {
		float SPEED, GRAVITY;
	};
	struct FPS {
		sf::Clock clock;
		sf::Time elapsedTime;
		sf::Time fpsUpdateTime = sf::seconds(0.5f);
		unsigned int frameCount = 0;
		unsigned int fps = 0;
		bool m_showFPS = false;
	};
protected:
	std::vector<std::shared_ptr<SpawnScript>> m_spawnScripts;
	std::vector<std::uint8_t> m_endScript;
	std::shared_ptr<Entity> m_player;
	std::string m_levelPath;
	PlayerConfig m_playerConfig;
	EnemyConfig m_enemyConfig;
	FPS m_FPS;
	bool m_drawTextures = true;
	bool m_drawCollision = false; 
	bool m_drawGrid = false;
	const Vec2 m_gridSize = { 64, 64 };
	sf::Text m_gridText;
	int m_score;
	float ViewX;

	void init(const std::string& levelPath);
	void loadLevel(const std::string& filename);
	Vec2 gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity>& entity);
	void spawnPlayer();
	void spawnBullet(std::shared_ptr<Entity> entity);
	void update() override;
	void sMovement();
	void sLifespan();
	void sCollision();
	void sDoAction(const Action& action) override;
	void sAnimation();
	void sRender() override;
	void onEnd() override;
	void spawnEnemy(std::string name, float X, float Y, bool goLeft);
public:
	Scene_Play(GameEngine* gameEngine, const std::string& levelPath);
};
