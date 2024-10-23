#include <SFML/Graphics.hpp>
#include <vector>
#include <map>
#include <memory>
#include <fstream>
#include <iostream>
#include <sstream>
#include "Animation.h"
#include "Assets.h"

Assets::Assets() {}
void Assets::loadAssets(const std::string& path) {
	std::ifstream file(path);
	if (!file.is_open()) {
		std::cerr << "Could not load assets file\n";
	}
	std::string line;
	while (std::getline(file, line)) {
		std::stringstream iss(line);
		std::string type, name, path, textName;
		size_t frames, duration;
		iss >> type;
		if (type == "Texture") {
			iss >> name >> path;
			loadTextures(name, path);
		}
		else if (type == "Animation") {
			iss >> name >> textName >> frames >> duration;
			loadAnimation(name, textName, frames, duration);
		}
		else if (type == "Font") {
			iss >> name >> path;
			loadFont(name, path);
		}
	}
}
void Assets::loadTextures(const std::string& name, const std::string& path) {
	sf::Texture texture;
	if (texture.loadFromFile(path)) {
		textures[name] = texture;
	}
	else {
		std::cerr << "Could not load texture file (" << name << ")\n";
	}
}
void Assets::loadAnimation(const std::string& name, const std::string& textName, const size_t frames, const size_t duration) {
		animation[name] = std::make_shared<Animation>(name, textures[textName], frames, duration);
}
void Assets::loadFont(const std::string& name, const std::string& path) {
	sf::Font font;
	if (font.loadFromFile(path)) {
		fonts[name] = font;
	}
	else {
		std::cerr << "Could not load font file (" << name << ")\n";
	}
}
Animation& Assets::getAnimation(const std::string& s) {
	return *animation[s];
}
sf::Font& Assets::getFont(const std::string& name) {
	return fonts[name];
}

