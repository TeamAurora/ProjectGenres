#pragma once
#include "game_object.h"

class CollisionTile : public GameObject
{
public:

	enum SHAPETYPE
	{
		BOX = 0,
		DIAGONAL
	};

	CollisionTile(SHAPETYPE shapetype, int flag);
	virtual ~CollisionTile(void);

	// Bitshifted values since shapes can have multiple collidable edges
	enum BOXEDGES
	{
		LEFT = 1 << 0,	// Binary 0001 - 0 or 1 bit
		DOWN = 1 << 1,	// Binary 0010 - 0 or 2 bit
		RIGHT = 1 << 2,	// Binary 0100 - 0 or 4 bit
		UP = 1 << 3,	// Binary 1000 - 0 or 8 bit
		OUTOFRANGE = 1 << 4
	};

	enum DIAGONALS
	{
		TOPLEFT = 0,
		BOTTOMLEFT,
		BOTTOMRIGHT,
		TOPRIGHT,
		OUTOFRANGE
	};

	SHAPETYPE shapetype_;
	BOXEDGES edges_;
	DIAGONALS diagonal_;


	bool harmful_; // if true this tile will damage
};

