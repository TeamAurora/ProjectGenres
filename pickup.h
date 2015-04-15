#ifndef PICKUP_H
#define PICKUP_H

#include "game_object.h"

class PickUp : public GameObject
{
	public:
		enum PICKUPTYPE { RED, BLUE, YELLOW, GREEN };
		PICKUPTYPE pickup_type_;

		PickUp();
		void Create_pickup(b2World* world_, float, float);

		bool spawned;
};
#endif