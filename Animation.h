#pragma once
#include <SFML/Graphics.hpp>
#include "Vec2.h"

#include <vector>

class Animation {
	sf::Sprite m_sprite;
	size_t m_frameCount = 1; // total number of frames of animation
	size_t m_currentFrame = 0;
	size_t m_speed = 0;    //duration of a single img of animation in frames
	Vec2 m_size = { 1, 1 };
	Vec2 m_initSize = { 1, 1 };
	std::string m_name = "NONE";
public:
	Animation();
	Animation(const std::string& name, const sf::Texture& t, size_t frameCount, size_t speed);
	void update();
	bool hasEnded() const;
	const std::string& getName() const;
	const Vec2& getSize() const;
	sf::Sprite& getSprite();
	void changeAnimSize(float scaleX, float scaleY);
};