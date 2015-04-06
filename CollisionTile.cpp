#include "CollisionTile.h"
#include <iostream>

CollisionTile::CollisionTile(SHAPETYPE shapetype, int flag) :
	shapetype_(shapetype_),
	harmful_(false)
{
	switch (shapetype_)
	{
	case BOX:
		if ((flag < 0) || (flag >= BOXEDGES::OUTOFRANGE))
		{
			std::cout << "CollisionTile box flag out of range" << std::endl;
		}
		edges_ = (BOXEDGES)flag;
		break;
	case DIAGONAL:
		if ((flag < 0) || (flag >= DIAGONALS::OUTOFRANGE))
		{
			std::cout << "CollisionTile diagonal flag out of range" << std::endl;
		}
		diagonal_ = (DIAGONALS)flag;
		break;
	}
}


CollisionTile::~CollisionTile(void)
{
}
