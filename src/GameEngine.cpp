#pragma once
#include "GameEngine.h"
#include "Assets.h"
#include "Scene_Play.h"
#include "Scene_Menu.h"
#include <fstream>
#include <iostream>
#include <sstream>

GameEngine::GameEngine(const std::string& path) {
	init(path);
}
void GameEngine::init(const std::string& path) {
	std::ifstream file(path);
	if (!file.is_open()) {
		std::cerr << "Couldn't open config file\n";
	}
	std::string line;
	while (std::getline(file, line)) {
		std::stringstream iss(line);
		int wWidth, wHeight, frameLimit;
		std::string type, assetsPath;
		iss >> type;
		if (type == "Window") {
			iss >> wWidth >> wHeight >> frameLimit;
			m_window.create(sf::VideoMode(wWidth, wHeight), "Mario");
			m_window.setFramerateLimit(frameLimit);
		}
		else if (type == "Assets") {
			iss >> assetsPath;
			m_assets.loadAssets(assetsPath);
		}
	}
	changeScene("MENU", std::make_shared<Scene_Menu>(this));
}
void GameEngine::update() {
	sUserInput();
	if (currentScene()->hasEnded()) {
		changeScene("MENU", std::make_shared<Scene_Menu>(this), true);
	}
	currentScene()->update();
}
void GameEngine::sUserInput() {
	sf::Event event;
	while (m_window.pollEvent(event)) {
		if (event.type == sf::Event::Closed) {
			quit();
		}
		if (event.type == sf::Event::KeyPressed) {
			if (event.key.code == sf::Keyboard::X) {
				sf::Texture texture;
				texture.create(m_window.getSize().x, m_window.getSize().y);
				texture.update(m_window);
				if (texture.copyToImage().saveToFile("test.png")) {
					std::cout << "screenshot saved to " << "text.png" << std::endl;
				}
			}
		}
		if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased) {
			//if the current scene does not have an action associated with this key, skip the event
			if (currentScene()->getActionMap().find(event.key.code) == currentScene()->getActionMap().end()) {
				continue;
			}
			//determine start or end action by whether it was key pres or release
			const std::string actionType = (event.type == sf::Event::KeyPressed) ? "START" : "END";
			//look up the action and send the action to the scene
			currentScene()->sDoAction(Action(currentScene()->getActionMap().at(event.key.code), actionType));
		}
	}
}
std::shared_ptr<Scene>& GameEngine::currentScene() {
	return m_sceneMap[m_currentScene];
}
void GameEngine::changeScene(const std::string& sceneName, std::shared_ptr<Scene> scene, bool endCurrentScene) {
	if (endCurrentScene) {
		auto it = m_sceneMap.find(m_currentScene);
		if (it != m_sceneMap.end()) {
			m_sceneMap.erase(it);
		}
	}
	m_sceneMap[sceneName] = scene;
	m_currentScene = sceneName;
	
}
void GameEngine::quit() {
	m_running = false;
}
void GameEngine::run() {
	while (isRunning()) {
		update();
	}
}
sf::RenderWindow& GameEngine::window() {
	return m_window;
}
Assets& GameEngine::assets() {
	return m_assets;
}
bool GameEngine::isRunning() {
	return m_running && m_window.isOpen();
}
