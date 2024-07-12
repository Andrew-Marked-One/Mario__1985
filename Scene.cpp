#pragma once
#include "Scene.h"
#include "GameEngine.h"
void Scene::setPaused(bool paused) {

}
Scene::Scene() {}
Scene::Scene(GameEngine* gameEngine) {
	m_game = gameEngine;
}
void Scene::simulate(const size_t frames) {

}
void Scene::registerAction(int inputKey, const std::string& actionName) {
	m_actionMap[inputKey] = actionName;
}
size_t Scene::width() const {
	return m_game->window().getSize().x;
}
size_t Scene::height() const {
	return m_game->window().getSize().y;
}
size_t Scene::currentFrame() const {
	return m_currentFrame;
}
bool Scene::hasEnded() const {
	return m_hasEnded;
}
const ActionMap& Scene::getActionMap() const {
	return m_actionMap;
}
void Scene::drawLine(const Vec2& p1, const Vec2& p2) {
	sf::Vertex line[] =
	{
		sf::Vertex(sf::Vector2f(p1.x, p1.y), sf::Color::White),
		sf::Vertex(sf::Vector2f(p2.x, p2.y), sf::Color::White)
	};
	m_game->window().draw(line, 2, sf::Lines);
}