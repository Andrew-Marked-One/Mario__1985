#pragma once

#include "Scene_Play.h"
#include "Assets.h"
#include "GameEngine.h"
#include "Components.h"
#include "Action.h"
#include "Physics.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

Scene_Play::Scene_Play(GameEngine* gameEngine, const std::string& levelPath)
	: Scene(gameEngine)
	, m_levelPath(levelPath)
	, m_endScript(10, 0) {
	init(m_levelPath);
}
void Scene_Play::init(const std::string& levelPath) {
	registerAction(sf::Keyboard::P, "PAUSE");
	registerAction(sf::Keyboard::Escape, "QUIT");
	registerAction(sf::Keyboard::T, "TOGGLE_TEXTURE");
	registerAction(sf::Keyboard::C, "TOGGLE_COLLISION");
	registerAction(sf::Keyboard::G, "TOGGLE_GRID");
	registerAction(sf::Keyboard::F, "TOGGLE_FPS");

	registerAction(sf::Keyboard::W, "UP");
	registerAction(sf::Keyboard::Space, "UP");
	registerAction(sf::Keyboard::A, "LEFT");
	registerAction(sf::Keyboard::D, "RIGHT");

	m_gridText.setCharacterSize(12);
	m_gridText.setFont(m_game->assets().getFont("Roboto"));
	m_gridText.setFillColor(sf::Color(186, 186, 186));
	loadLevel(levelPath);
}

Vec2 Scene_Play::gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity>& entity) {
	float sizeX = entity->getComponent<CAnimation>().animation.getSize().x;
	float sizeY = entity->getComponent<CAnimation>().animation.getSize().y;
	return Vec2(sizeX / 2.f + m_gridSize.x * gridX, height() - sizeY / 2.f - m_gridSize.y * gridY);
}

void Scene_Play::loadLevel(const std::string& filename) {
	m_entityManager = EntityManager();
	std::ifstream file(filename);
	if (!file.is_open()) {
		std::cerr << "Could not load level file\n";
	}
	std::string line;
	while (std::getline(file, line)) {
		std::stringstream iss(line);
		std::string type, name;
		float posX, posY;
		iss >> type;
		if (type == "Tile") {
			iss >> name >> posX >> posY;
			auto tile = m_entityManager.addEntity(type);
			tile->addComponent<CAnimation>(m_game->assets().getAnimation(name), true);
			tile->getComponent<CAnimation>().animation.changeAnimSize(4, 4);
			tile->addComponent<CTransform>(gridToMidPixel(posX, posY, tile));
			tile->getComponent<CTransform>().prevPos = tile->getComponent<CTransform>().pos;
			tile->addComponent<CBoundingBox>(tile->getComponent<CAnimation>().animation.getSize());
			tile->addComponent<CState>();
		}
		else if (type == "Dec") {
			iss >> name >> posX >> posY;
			auto dec = m_entityManager.addEntity(type);
			dec->addComponent<CAnimation>(m_game->assets().getAnimation(name), true);
			dec->getComponent<CAnimation>().animation.changeAnimSize(4, 4);
			dec->addComponent<CTransform>(gridToMidPixel(posX, posY, dec));
			dec->getComponent<CTransform>().prevPos = dec->getComponent<CTransform>().pos;
		}
		else if (type == "Enemy") {
			bool goLeft;
			iss >> name >> posX >> posY >> goLeft;
			m_spawnScripts.push_back(std::make_shared<SpawnScript>(posX, posY, name, goLeft));
		}
		else if (type == "Edge") {
			iss >> name >> posX >> posY;
			auto edge = m_entityManager.addEntity(type);
			edge->addComponent<CAnimation>(m_game->assets().getAnimation(name), true);
			edge->getComponent<CAnimation>().animation.changeAnimSize(4, 4);
			edge->addComponent<CTransform>(gridToMidPixel(posX, posY, edge));
			edge->addComponent<CBoundingBox>(edge->getComponent<CAnimation>().animation.getSize());
			edge->getComponent<CTransform>().prevPos = edge->getComponent<CTransform>().pos;
		}
		else if (type == "EnemyConfig") {
			iss >> m_enemyConfig.SPEED >> m_enemyConfig.GRAVITY;
		}
		else if (type == "Player") {
			iss >> m_playerConfig.X >> m_playerConfig.Y >> m_playerConfig.CX >> m_playerConfig.CY >> m_playerConfig.SPEED >>
			m_playerConfig.JUMP >> m_playerConfig.MAXJUMP >> m_playerConfig.MAXSPEED >> m_playerConfig.GRAVITY >> m_playerConfig.WEAPON;
			spawnPlayer();
		}
	}
}

void Scene_Play::spawnPlayer() {
	m_player = m_entityManager.addEntity("Player");
	m_player->addComponent<CAnimation>(m_game->assets().getAnimation("Stand"), true);
	m_player->getComponent<CAnimation>().animation.changeAnimSize(4, 4);
	m_player->addComponent<CTransform>(gridToMidPixel(m_playerConfig.X, m_playerConfig.Y, m_player));
	m_player->addComponent<CBoundingBox>(Vec2(m_playerConfig.CX, m_playerConfig.CY));
	m_player->addComponent<CGravity>(m_playerConfig.GRAVITY);
	m_player->addComponent<CState>("standing");
	ViewX = 0;
}
void Scene_Play::spawnBullet(std::shared_ptr<Entity> entity) {
	//this should spawn a bullet at the given entity, going in the direction the entity is facing
}
void Scene_Play::update() {
	auto& pT = m_player->getComponent<CTransform>();
	float halfWidth = width() / 2.f;
	for (auto& script : m_spawnScripts) {
		if (!script->spawned) {
			float scriptX = script->x * m_gridSize.x;
			if (pT.pos.x > scriptX - m_gridSize.x - halfWidth || (scriptX > ViewX && scriptX < ViewX + width())) {
				spawnEnemy(script->name, script->x, script->y, script->goLeft);
				script->spawned = true;
			}
		}
	}
	m_entityManager.update();
	//implement pause functionality
	sMovement();
	sLifespan();
	sCollision();
	sAnimation();
	sRender();
	m_currentFrame++;
}
void Scene_Play::sMovement() {
	auto& pT = m_player->getComponent<CTransform>();
	auto& pS = m_player->getComponent<CState>();
	auto& pI = m_player->getComponent<CInput>();
	Vec2 playerVelocity = pT.velocity;
	if (m_player->getComponent<CInput>().up && pS.canJump) {
		playerVelocity.y += m_playerConfig.JUMP;
		if (playerVelocity.y < m_playerConfig.MAXJUMP) {
			playerVelocity.y = m_playerConfig.MAXJUMP;
			pS.canJump = false;
		}
		pS.inAir = true;
		pS.state = "jumping";
	}
	if (m_player->getComponent<CInput>().left) {
		playerVelocity.x += -m_playerConfig.SPEED;
		if (playerVelocity.x < -m_playerConfig.MAXSPEED) {
			playerVelocity.x = -m_playerConfig.MAXSPEED;
		}
		pS.lookingRight = false;
		pS.state = pS.state == "jumping" ? pS.state : pS.state = "running";
	}
	if (m_player->getComponent<CInput>().right) {
		playerVelocity.x += m_playerConfig.SPEED;
		if (playerVelocity.x > m_playerConfig.MAXSPEED) {
			playerVelocity.x = m_playerConfig.MAXSPEED;
		}
		pS.lookingRight = true;
		pS.state = pS.state == "jumping" ? pS.state : pS.state = "running";
	}
	
	m_player->getComponent<CTransform>().velocity = playerVelocity;
	// gravity
	for (auto& e : m_entityManager.getEntities()) {
		if (e->hasComponent<CGravity>()) {
			e->getComponent<CTransform>().velocity.y += e->getComponent<CGravity>().gravity;
			if (m_playerConfig.MAXSPEED < e->getComponent<CTransform>().velocity.y) {
				e->getComponent<CTransform>().velocity.y = m_playerConfig.MAXSPEED;
			}
		}
		e->getComponent<CTransform>().pos += e->getComponent<CTransform>().velocity;
	}

	if (playerVelocity.y > 0) {
		pS.canJump = false;
	}
}
void Scene_Play::sLifespan() {
	//Check lifespan of entityes that have them, and destroy them if they go over
}
void Scene_Play::sCollision() {
	auto& pT = m_player->getComponent<CTransform>();
	auto& pB = m_player->getComponent<CBoundingBox>().halfSize;
	auto& pS = m_player->getComponent<CState>();
	bool pHasB = m_player->hasComponent<CBoundingBox>();
	//player to tile
	for (auto& t : m_entityManager.getEntities("Tile")) {
		auto& tT = t->getComponent<CTransform>();
		auto& tB = t->getComponent<CBoundingBox>().halfSize;
		auto& tA = t->getComponent<CAnimation>().animation;
		auto& tS = t->getComponent<CState>();
		if ((pT.pos.x - pB.x < tT.pos.x + tB.x) && (tT.pos.x - tB.x < pT.pos.x + pB.x) && (pT.pos.y - pB.y < tT.pos.y + tB.y) && (tT.pos.y - tB.y < pT.pos.y + pB.y) && pHasB) {
			Vec2 overlap = Physics::getOverlap(m_player, t);
			Vec2 prevOverlap = Physics::getPreviousOverlap(m_player, t);
			if (prevOverlap.x > 0 && pT.pos.y < tT.pos.y) {
				pT.pos.y -= overlap.y;
				pT.velocity.y = 0;
				pS.canJump = true;
				pS.inAir = false;
			}
			else if (prevOverlap.x > 0 && pT.pos.y > tT.pos.y) {
				pT.pos.y += overlap.y;
				if (pT.velocity.y < 0) {
					if (tA.getName() == "Brick") {
						tT.velocity.y = -7.5;
						t->addComponent<CGravity>(m_enemyConfig.GRAVITY);
						tS.inAir = true;
					}
					else if (tA.getName() == "QuesAnim") {
						t->addComponent<CAnimation>(m_game->assets().getAnimation("QuesEmpt"), true);
						tA.changeAnimSize(4, 4);
						auto coin = m_entityManager.addEntity("Dec");
						coin->addComponent<CAnimation>(m_game->assets().getAnimation("CoinEarn"), true);
						coin->getComponent<CAnimation>().animation.changeAnimSize(4, 4);
						coin->addComponent<CTransform>(tT);
						coin->getComponent<CTransform>().pos.y -= (tB.y + coin->getComponent<CAnimation>().animation.getSize().y);
						coin->getComponent<CTransform>().prevPos = coin->getComponent<CTransform>().pos;
						coin->getComponent<CTransform>().velocity.y = -8.5;
						coin->addComponent<CGravity>(m_enemyConfig.GRAVITY);
					}
				}
				pT.velocity.y = 0;
			}
			else if (prevOverlap.y > 0 && pT.pos.x < tT.pos.x) {
				pT.pos.x -= overlap.x;
			}
			else if (prevOverlap.y > 0 && pT.pos.x > tT.pos.x) {
				pT.pos.x += overlap.x;
			}
		}
		//gravity for bricks
		if (tA.getName() == "Brick" && tS.inAir) {
			if (tT.pos.y > tT.prevPos.y) {
				tT.pos.y = tT.prevPos.y;
				tT.velocity.y = 0;
				t->removeComponent<CGravity>();
				tS.inAir = false;
			}
		}
	}
	//enemy to tile
	for (auto& e : m_entityManager.getEntities("Enemy")) {
		if (e->hasComponent<CBoundingBox>()) {
			auto& eT = e->getComponent<CTransform>();
			auto& eB = e->getComponent<CBoundingBox>().halfSize;
			for (auto& t : m_entityManager.getEntities("Tile")) {
				auto& tT = t->getComponent<CTransform>().pos;
				auto& tB = t->getComponent<CBoundingBox>().halfSize;
				if ((eT.pos.x - eB.x < tT.x + tB.x) && (tT.x - tB.x < eT.pos.x + eB.x) && (eT.pos.y - eB.y < tT.y + tB.y) && (tT.y - tB.y < eT.pos.y + eB.y)) {
					Vec2 overlap = Physics::getOverlap(e, t);
					Vec2 prevOverlap = Physics::getPreviousOverlap(e, t);
					if (prevOverlap.x > 0 && eT.pos.y < tT.y) {
						eT.pos.y -= overlap.y;
						eT.velocity.y = 0;
					}
					else if (prevOverlap.x > 0 && eT.pos.y > tT.y) {
						eT.pos.y += overlap.y;
						eT.velocity.y = 0;
					}
					else if (prevOverlap.y > 0 && eT.pos.x < tT.x) {
						eT.pos.x -= overlap.x;
						eT.velocity.x = -eT.velocity.x;
						e->getComponent<CAnimation>().animation.changeAnimSize(-4, 4);
					}
					else if (prevOverlap.y > 0 && eT.pos.x > tT.x) {
						eT.pos.x += overlap.x;
						eT.velocity.x = -eT.velocity.x;
						e->getComponent<CAnimation>().animation.changeAnimSize(4, 4);
					}
				}
			}
			//enemy to edge
			for (auto& edge : m_entityManager.getEntities("Edge")) {
				auto& edgeT = edge->getComponent<CTransform>().pos;
				auto& edgeB = edge->getComponent<CBoundingBox>().halfSize;
				if ((eT.pos.x - eB.x < edgeT.x + edgeB.x) && (edgeT.x - edgeB.x < eT.pos.x + eB.x) && (eT.pos.y - eB.y < edgeT.y + edgeB.y) && (edgeT.y - edgeB.y < eT.pos.y + eB.y)) {
					Vec2 overlap = Physics::getOverlap(e, edge);
					Vec2 prevOverlap = Physics::getPreviousOverlap(e, edge);
					if (prevOverlap.x > 0 && eT.pos.y < edgeT.y) {
						eT.pos.y -= overlap.y;
						eT.velocity.y = 0;
					}
					else if (prevOverlap.x > 0 && eT.pos.y > edgeT.y) {
						eT.pos.y += overlap.y;
						eT.velocity.y = 0;
					}
					else if (prevOverlap.y > 0 && eT.pos.x < edgeT.x) {
						eT.pos.x -= overlap.x;
						eT.velocity.x = -eT.velocity.x;
						e->getComponent<CAnimation>().animation.changeAnimSize(-4, 4);
					}
					else if (prevOverlap.y > 0 && eT.pos.x > edgeT.x) {
						eT.pos.x += overlap.x;
						eT.velocity.x = -eT.velocity.x;
						e->getComponent<CAnimation>().animation.changeAnimSize(4, 4);
					}
				}
			}
			// player to enemy
			if ((pT.pos.x - pB.x < eT.pos.x + eB.x) && (eT.pos.x - eB.x < pT.pos.x + pB.x) && (pT.pos.y - pB.y < eT.pos.y + eB.y) && (eT.pos.y - eB.y < pT.pos.y + pB.y)) {
				Vec2 overlap = Physics::getOverlap(m_player, e);
				Vec2 prevOverlap = Physics::getPreviousOverlap(m_player, e);
				if (pHasB) {
					if (prevOverlap.x > 0 && pT.pos.y < eT.pos.y) {
						pT.velocity.y = -m_playerConfig.MAXSPEED;
						e->getComponent<CState>().state = "dying";
						eT.velocity.x = 0;
					}
					else if (prevOverlap.x > 0 && pT.pos.y > eT.pos.y) {
						pT.velocity.y = -m_playerConfig.MAXSPEED;
						pS.state = "dying";
					}
					else if (prevOverlap.y > 0 && pT.pos.x < eT.pos.x) {
						pT.velocity.y = -m_playerConfig.MAXSPEED;
						pS.state = "dying";
					}
					else if (prevOverlap.y > 0 && pT.pos.x > eT.pos.x) {
						pT.velocity.y = -m_playerConfig.MAXSPEED;
						pS.state = "dying";
					}
				}
			}
			//update prev enemy pos
			eT.prevPos = eT.pos;
		}
	}
	//gravity for coins earned
	//pole collision
	for (auto& dec : m_entityManager.getEntities("Dec")) {
		auto& decA = dec->getComponent<CAnimation>();
		auto& decT = dec->getComponent<CTransform>();
		if (decA.animation.getName() == "CoinEarn") {
			if (decT.pos.y > decT.prevPos.y) {
				decT.pos.y = decT.prevPos.y;
				decT.velocity.y = 0;
				dec->removeComponent<CGravity>();
				decA.repeat = false;
			}
		}
		else if (decA.animation.getName() == "CastleFP") {
			if (pT.pos.x + pB.x >= decT.pos.x && m_endScript[0] == 0) {
				pS.state = "endScene";
				m_endScript[0] = 1;
				pT.velocity = { 0, 4 };
				m_player->getComponent<CInput>().up = false;
				m_player->getComponent<CInput>().left = false;
				m_player->getComponent<CInput>().right = false;
				m_player->removeComponent<CGravity>();
			}
		}
		else if (decA.animation.getName() == "CastleF") {
			if (m_endScript[0] == 1 && m_endScript[1] == 0) {
				decT.velocity.y = 4;
				if (decT.pos.y + decA.animation.getSize().y / 2.f > pT.pos.y + pB.y) {
					decT.pos.y = pT.pos.y + pB.y - decA.animation.getSize().y / 2.f;
					pT.pos.x = decT.pos.x + decA.animation.getSize().x / 2.f + pB.x - 5;
					decT.velocity.y = 0;
					m_endScript[1] = 1;
					pT.velocity = { 0, 0 };
					m_player->addComponent<CGravity>(m_playerConfig.GRAVITY);
				}
			}
		}
		else if (decA.animation.getName() == "Castle") {
			if (m_endScript[2] == 1 && m_endScript[3] == 0) {
				if (decT.pos.x < pT.pos.x) {
					onEnd();
				}
			}
		}
	}
	//left boundery
	if (pT.pos.x - pB.x < ViewX) {
		pT.pos.x = ViewX + pB.x;
	}
	//bottom boundery
	if (pT.pos.y > height() && pHasB) {
		pT.velocity.y = -20;
		pS.state = "dying";
		pT.velocity.x = 0;
		m_player->getComponent<CInput>().up = false;
		m_player->getComponent<CInput>().left = false;
		m_player->getComponent<CInput>().right = false;
	}
	//update prev player pos
	pT.prevPos = pT.pos;
}
void Scene_Play::sDoAction(const Action& action) {
	auto& pI = m_player->getComponent<CInput>();
	auto& pT = m_player->getComponent<CTransform>();
	if (action.type() == "START") {
		if (action.name() == "TOGGLE_TEXTURE") {
			m_drawTextures = !m_drawTextures;
		}
		else if (action.name() == "TOGGLE_GRID") {
			m_drawGrid = !m_drawGrid;
		}
		else if (action.name() == "TOGGLE_COLLISION") {
			m_drawCollision = !m_drawCollision;
		}
		else if (action.name() == "TOGGLE_FPS") {
			m_FPS.m_showFPS = !m_FPS.m_showFPS;
		}
		else if (action.name() == "PAUSE") {
			setPaused(!m_paused);
		}
		else if (action.name() == "QUIT") {
			onEnd();
		}
		if (m_player->getComponent<CState>().canMove) {
			if (action.name() == "UP") {
				pI.up = true;
			}
			else if (action.name() == "LEFT") {
				pI.left = true;
				if (pT.velocity.x > 0) {
					pT.velocity.x = 0;
				}
			}
			else if (action.name() == "RIGHT") {
				pI.right = true;
				if (pT.velocity.x < 0) {
					pT.velocity.x = 0;
				}
			}
		}
	}
	else if (action.type() == "END") {
		if (action.name() == "UP") {
			pI.up = false;
			m_player->getComponent<CState>().canJump = false;
		}
		if (action.name() == "LEFT") {
			pI.left = false;
			if (pI.right == false) {
				m_player->getComponent<CTransform>().velocity.x = 0;
			}
		}
		if (action.name() == "RIGHT") {
			pI.right = false;
			if (pI.left == false) {
				m_player->getComponent<CTransform>().velocity.x = 0;
			}
		}
	}
}
void Scene_Play::sAnimation() {
	auto& pS = m_player->getComponent<CState>();
	auto& pA = m_player->getComponent<CAnimation>().animation;
	auto& pT = m_player->getComponent<CTransform>();
	if (!pS.inAir && m_endScript[0] == 0 && pS.state != "dying") {
		if (pT.velocity.y != 0) {
			pS.state = "jumping";
		}
		else if (pT.velocity.x != 0) {
			pS.state = "running";
		}
		else if (pT.velocity.x == 0 && pT.velocity.y == 0) {
			pS.state = "standing";
		}
	}
	if (pS.state == "dying") {
		if (pA.getName() != "Die"){
			m_player->addComponent<CAnimation>(m_game->assets().getAnimation("Die"), true);
			m_player->removeComponent<CBoundingBox>();
			pS.canMove = false;
			pT.velocity.x = 0;
			m_player->getComponent<CInput>().up = false;
			m_player->getComponent<CInput>().left = false;
			m_player->getComponent<CInput>().right = false;
		}
		if (pA.hasEnded()) {
			onEnd();
		}
	}
	else if (pS.state == "jumping" && pA.getName() != "Jump") {
		m_player->addComponent<CAnimation>(m_game->assets().getAnimation("Jump"), true);
	}
	else if (pS.state == "standing" && pA.getName() != "Stand") {
		m_player->addComponent<CAnimation>(m_game->assets().getAnimation("Stand"), true);
	}
	else if (pS.state == "running" && pA.getName() != "Run") {
		m_player->addComponent<CAnimation>(m_game->assets().getAnimation("Run"), true);
	}
	else if (pS.state == "endScene") {
		pS.canMove = false;
		if (m_endScript[0] == 1 && m_endScript[1] == 0 && pA.getName() != "Climb") {
			m_player->addComponent<CAnimation>(m_game->assets().getAnimation("Climb"), true);
		}
		else if (m_endScript[1] == 1 && m_endScript[2] == 0) {
			if (pA.getName() != "ClimbStatic") {
				pS.lookingRight = false;
				m_player->addComponent<CAnimation>(m_game->assets().getAnimation("ClimbStatic"), true);
			}
			else if(pA.hasEnded()) {
				m_endScript[2] = 1;
			}
		}
		else if (m_endScript[2] == 1 && m_endScript[3] == 0) {
			if (pA.getName() != "Run") {
				pS.lookingRight = true;
				m_player->addComponent<CAnimation>(m_game->assets().getAnimation("Run"), true);
			}
			pT.velocity.x = 3;
			pT.pos += pT.velocity;
		}
	}
	if (pS.lookingRight) {
		m_player->getComponent<CAnimation>().animation.changeAnimSize(4, 4);
	}
	else {
		m_player->getComponent<CAnimation>().animation.changeAnimSize(-4, 4);
	}

	for (auto& e : m_entityManager.getEntities("Enemy")) {
		if (e->getComponent<CState>().state == "dying") {
			auto& eA = e->getComponent<CAnimation>().animation;
			if (eA.getName() == "GoombaW" && eA.getName() != "GoombaD") {
				e->addComponent<CAnimation>(m_game->assets().getAnimation("GoombaD"), false);
				eA.changeAnimSize(4, 4);
				e->removeComponent<CBoundingBox>();
				e->removeComponent<CGravity>();
			}
			else if (eA.getName() == "KoopaW" && eA.getName() != "KoopaD") {
				e->addComponent<CAnimation>(m_game->assets().getAnimation("KoopaD"), false);
				eA.changeAnimSize(4, 4);
				e->getComponent<CTransform>().velocity.y = -5;
				e->removeComponent<CBoundingBox>();
			}
		}
	}

	for (auto& e : m_entityManager.getEntities()) {
		auto& eA = e->getComponent<CAnimation>();
		if (!eA.repeat && eA.animation.hasEnded()) {
			e->destroy();
		}
		eA.animation.update();
	}
}

void Scene_Play::sRender() {
	m_game->window().clear(sf::Color(100, 99, 254));
	float temp_xPos = floor(m_player->getComponent<CTransform>().pos.x - (static_cast<float>(width()) / 2.f) * 0.75);
	if (ViewX < temp_xPos && m_endScript[0] == 0) {
		ViewX = temp_xPos;
	}
	sf::View view(sf::FloatRect(ViewX, 0, width(), height()));
	m_game->window().setView(view);

	if (m_drawTextures) {
		for (auto& e : m_entityManager.getEntities()) {
			e->getComponent<CAnimation>().animation.getSprite().setPosition(e->getComponent<CTransform>().pos.x, e->getComponent<CTransform>().pos.y);
			m_game->window().draw(e->getComponent<CAnimation>().animation.getSprite());
		}
	}
	if (m_drawCollision) {
		for (auto& e : m_entityManager.getEntities()) {
			if (e->hasComponent<CBoundingBox>()) {
				sf::RectangleShape boundingBoxShape(sf::Vector2f(e->getComponent<CBoundingBox>().size.x, e->getComponent<CBoundingBox>().size.y));
				boundingBoxShape.setPosition(e->getComponent<CTransform>().pos.x - e->getComponent<CBoundingBox>().size.x / 2.f, e->getComponent<CTransform>().pos.y - e->getComponent<CBoundingBox>().size.y / 2.f);
				boundingBoxShape.setFillColor(sf::Color(0, 0, 0, 0));
				boundingBoxShape.setOutlineColor(sf::Color::White);
				boundingBoxShape.setOutlineThickness(-2.0f);
				m_game->window().draw(boundingBoxShape);
			}
		}
	}
	if (m_drawGrid) {
		for (size_t i = 1; i < height() / m_gridSize.y; i++) {
			drawLine(Vec2(0, height() - m_gridSize.y * i), Vec2(ViewX + width(), height() - m_gridSize.y * i));
		}
		for (size_t i = 1; i < (ViewX + width()) / m_gridSize.x; i++) {
			drawLine(Vec2(m_gridSize.x * i, 0), Vec2(m_gridSize.x * i, height()));
		}
		for (size_t i = 0; i < (ViewX + width()) / m_gridSize.x; i++) {
			for (size_t j = 0; j < height() / m_gridSize.y; j++) {
				m_gridText.setString("(" + std::to_string(i) + ", " + std::to_string(j) + ")");
				m_gridText.setPosition(m_gridSize.x * i, height() - m_gridSize.y * j - m_gridSize.y);
				m_game->window().draw(m_gridText);
			}
		}
	}
	if (m_FPS.m_showFPS) {
		m_gridText.setCharacterSize(20);
		m_FPS.elapsedTime += m_FPS.clock.restart();
		m_FPS.frameCount++;

		if (m_FPS.elapsedTime >= m_FPS.fpsUpdateTime) {
			m_FPS.fps = static_cast<unsigned int>(m_FPS.frameCount / m_FPS.elapsedTime.asSeconds());
			m_FPS.elapsedTime = sf::Time::Zero;
			m_FPS.frameCount = 0;
		}
		m_gridText.setPosition(ViewX, 0);
		m_gridText.setString(std::to_string(m_FPS.fps));
		m_game->window().draw(m_gridText);
		m_gridText.setCharacterSize(12);
	}
	m_game->window().display();
}

void Scene_Play::onEnd() {
	m_hasEnded = true;
}
void Scene_Play::spawnEnemy(std::string name, float X, float Y, bool goLeft) {
	if (name == "Goomba") {
		auto enemy = m_entityManager.addEntity("Enemy");
		enemy->addComponent<CAnimation>(m_game->assets().getAnimation("GoombaW"), true);
		enemy->getComponent<CAnimation>().animation.changeAnimSize(4, 4);
		enemy->addComponent<CTransform>(gridToMidPixel(X, Y, enemy));
		goLeft ? enemy->getComponent<CTransform>().velocity.x = -m_enemyConfig.SPEED : enemy->getComponent<CTransform>().velocity.x = m_enemyConfig.SPEED;
		enemy->addComponent<CBoundingBox>(Vec2(enemy->getComponent<CAnimation>().animation.getSize()));
		enemy->addComponent<CGravity>(m_enemyConfig.GRAVITY);
		enemy->addComponent<CState>("walking");
	}
	else if (name == "Koopa") {
		auto enemy = m_entityManager.addEntity("Enemy");
		enemy->addComponent<CAnimation>(m_game->assets().getAnimation("KoopaW"), true);
		enemy->getComponent<CAnimation>().animation.changeAnimSize(4, 4);
		enemy->addComponent<CTransform>(gridToMidPixel(X, Y, enemy));
		goLeft ? enemy->getComponent<CTransform>().velocity.x = -m_enemyConfig.SPEED : enemy->getComponent<CTransform>().velocity.x = m_enemyConfig.SPEED;
		enemy->addComponent<CBoundingBox>(Vec2(enemy->getComponent<CAnimation>().animation.getSize()));
		enemy->addComponent<CGravity>(m_enemyConfig.GRAVITY);
		enemy->addComponent<CState>("walking");
		enemy->getComponent<CAnimation>().animation.changeAnimSize(-4, 4);
	}
}
