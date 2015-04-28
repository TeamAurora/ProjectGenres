#ifndef PLANT_H
#define PLANT_H
#include "game_object.h"
//John and Craig
class Plant : public GameObject
{
public:

	Plant();

	void CreateBody(b2World* _world);

	enum PLANTTYPE { WALL, BLOCK };
	PLANTTYPE plant_type_;

	bool collided;

	abfw::Vector2 spawn_position;
};

#endif