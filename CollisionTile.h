#pragma once
#include "game_object.h"
//Craig
class CollisionTile : public GameObject
{
public:

	enum SHAPETYPE
	{
		BOX = 0,
		DIAGONAL
	};

	CollisionTile(SHAPETYPE shapetype);
	virtual ~CollisionTile(void);

	// flags to hint collidable edges
	struct EDGES
	{
		bool LEFT;
		bool DOWN;
		bool RIGHT;
		bool UP;
	};

	// enum to check if the shape has a diagonal edge (only 1 per tile)
	enum DIAGONALS
	{
		NONE = 0,
		TOPLEFT,
		BOTTOMLEFT,
		BOTTOMRIGHT,
		TOPRIGHT
	};

	SHAPETYPE shapetype_;
	EDGES edges_;
	DIAGONALS diagonal_;


	bool harmful_; // if true this tile will damage
};

