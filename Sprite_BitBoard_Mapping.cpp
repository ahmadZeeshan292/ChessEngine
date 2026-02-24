#include "Sprite_BitBoard_Mapping.h"
#include <iostream>
Sprite_BitBoard_Mapping::Sprite_BitBoard_Mapping()
{
}

Sprite_BitBoard_Mapping::Sprite_BitBoard_Mapping(string filepath, uint64_t bitBoard)
{
	for (int i = 0; i < 64; i++) {
		if (bitBoard & 1ULL) {
			sprite_map[i] = new Sprite_Table(filepath);
		}

		bitBoard = bitBoard >> 1;
	}
}
