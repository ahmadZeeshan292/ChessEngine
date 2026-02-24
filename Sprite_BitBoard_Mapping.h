#pragma once

#include "BitBoard.h"
#include <SFML/Graphics.hpp>
#include <unordered_map>


/* mapping of bit position to sprite hash_mapp needed */

struct Sprite_Table {

	sf::Sprite sprite;
	sf::Texture texture;

	Sprite_Table(string filepath){
		if (filepath == "") return;

		texture.loadFromFile(filepath);
		sprite.setTexture(texture);

		sprite.setScale(
			float(800 / 8) / texture.getSize().x,
			float(800 / 8) / texture.getSize().y
		);
	}
};

class Sprite_BitBoard_Mapping {
public:
	unordered_map<uint64_t, Sprite_Table*> sprite_map;

	Sprite_BitBoard_Mapping();
	Sprite_BitBoard_Mapping(string filepath, uint64_t bitBoard);
};

