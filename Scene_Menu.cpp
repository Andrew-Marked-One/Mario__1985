#pragma once
#include "Scene_Menu.h"
#include "GameEngine.h"
#include "Scene_Play.h"

Scene_Menu::Scene_Menu(GameEngine* gameEngine)
	: Scene(gameEngine) {
	init();
}
void Scene_Menu::init() {
	registerAction(sf::Keyboard::W, "UP");
	registerAction(sf::Keyboard::Up, "UP");
	registerAction(sf::Keyboard::S, "DOWN");
	registerAction(sf::Keyboard::Down, "DOWN");
	registerAction(sf::Keyboard::Enter, "PLAY");
	registerAction(sf::Keyboard::Escape, "QUIT");
	m_title = "MEGA MARIO";
	m_menuStrings = { "Level 1", "Level 2", "Level 3" };
	m_menuText.setFont(m_game->assets().getFont("Pixelify"));
	m_levelPaths.push_back("bin/level1.txt");
	m_levelPaths.push_back("bin/level2.txt");
	m_levelPaths.push_back("bin/level3.txt");
	m_game->window().setView(sf::View(sf::FloatRect(0, 0, width(), height())));
}
void Scene_Menu::sRender(){
	m_game->window().clear(sf::Color(100, 99, 254));

	m_menuText.setString(m_title);
	m_menuText.setCharacterSize(80);
	m_menuText.setPosition(0, 0);
	m_menuText.setFillColor(sf::Color(0, 0, 0));
	m_game->window().draw(m_menuText);

	m_menuText.setPosition(10, 100);
	m_menuText.setCharacterSize(70);
	for (size_t i = 0; i < m_menuStrings.size(); i++) {
		if (m_selectedMenuIndex == i) {
			m_menuText.setFillColor(sf::Color(255, 255, 255));
		}
		else {
			m_menuText.setFillColor(sf::Color(0, 0, 0));
		}
		m_menuText.setPosition(10, m_menuText.getPosition().y + 5 + m_menuText.getCharacterSize());
		m_menuText.setString(m_menuStrings[i]);
		m_game->window().draw(m_menuText);
	}

	m_menuText.setFillColor(sf::Color(0, 0, 0));
	m_menuText.setCharacterSize(40);
	m_menuText.setPosition(0, height() - m_menuText.getCharacterSize() - 10);
	m_menuText.setString("W: UP   S: DOWN   Enter: PLAY   Escape: QUIT");
	m_game->window().draw(m_menuText);

	m_game->window().display();
}
void Scene_Menu::sDoAction(const Action& action){
	if (action.type() == "START") {
		if (action.name() == "UP") {
			if (m_selectedMenuIndex > 0) {
				m_selectedMenuIndex--;
			}
			else{
				m_selectedMenuIndex = m_menuStrings.size() - 1;
			}
		}
		else if (action.name() == "DOWN") {
			m_selectedMenuIndex = (m_selectedMenuIndex + 1) % m_menuStrings.size();
		}
		else if (action.name() == "PLAY") {
			m_game->changeScene("PLAY", std::make_shared<Scene_Play>(m_game, m_levelPaths[m_selectedMenuIndex]), true);
		}
		else if (action.name() == "QUIT") {
			onEnd();
		}
	}
}
void Scene_Menu::update(){
	sRender();
	m_currentFrame++;
}
void Scene_Menu::onEnd(){
	m_game->quit();
}
