#include "CollisionTile.h"
#include <iostream>

CollisionTile::CollisionTile(SHAPETYPE shapetype) :
	shapetype_(shapetype),
	harmful_(false)
{
	type_ = COLLISIONTILE;
	edges_.DOWN = false;
	edges_.UP = false;
	edges_.LEFT = false;
	edges_.RIGHT = false;
	diagonal_ = NONE;
}


CollisionTile::~CollisionTile(void)
{
}
