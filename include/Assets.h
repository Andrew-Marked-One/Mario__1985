#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <map>
#include <memory>
#include "Animation.h"



class Assets {
	std::map<std::string, sf::Texture> textures;
	std::map<std::string, sf::Font> fonts;
	std::map<std::string, std::shared_ptr<Animation>>  animation;
public:
	Assets();
	void loadAssets(const std::string& path);
	void loadTextures(const std::string& name, const std::string& path);
	void loadAnimation(const std::string& name, const std::string& textName, const size_t frames, const size_t duration);
	void loadFont(const std::string& name, const std::string& path);
	Animation& getAnimation(const std::string& s);
	sf::Font& getFont(const std::string& name);
};
