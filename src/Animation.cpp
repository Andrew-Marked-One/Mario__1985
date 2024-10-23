#include "Animation.h"
#include <cmath>

Animation::Animation() {}

Animation::Animation(const std::string& name, const sf::Texture& t, size_t frameCount, size_t speed)
		: m_name(name)
		, m_sprite(t)
		, m_frameCount(frameCount)
		, m_currentFrame(0)
		, m_speed(speed) {
	m_size = Vec2(static_cast<float>(t.getSize().x / m_frameCount), static_cast<float>(t.getSize().y));
	m_initSize = m_size;
	m_sprite.setOrigin(m_size.x / 2.f, m_size.y / 2.f);
	m_sprite.setTextureRect(sf::IntRect(m_currentFrame * m_initSize.x, 0, m_initSize.x, m_initSize.y));
}
void Animation::update() {
	if (m_speed > 0) {
		m_sprite.setTextureRect(sf::IntRect((static_cast<int>(floor(m_currentFrame / m_speed)) % m_frameCount) * m_initSize.x, 0, m_initSize.x, m_initSize.y));
	}
	m_currentFrame++;
}
const Vec2& Animation::getSize() const{
	return m_size;
}
const std::string& Animation::getName() const {
	return m_name;
}
sf::Sprite& Animation::getSprite() {
	return m_sprite;
}
bool Animation::hasEnded() const {
	return floor(m_currentFrame / m_speed) >= m_frameCount;
}
void Animation::changeAnimSize(float scaleX, float scaleY) {
	m_sprite.setScale(scaleX, scaleY);
	m_size = {m_size.x * scaleX, m_size.y * scaleY};
}
