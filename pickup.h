#ifndef PICKUP_H
#define PICKUP_H
#include "game_object.h"

class PickUp : public GameObject
{
public:

	PickUp();

	void RecreateBody(b2World* _world);

	enum PICKUPTYPE { RED, BLUE, YELLOW, GREEN };
	PICKUPTYPE pickup_type_;

	bool collided;
};

#endif