#ifndef COLLECTIBLE_H
#define COLLECTIBLE_H

#include "game_object.h"

class PickUp : public GameObject
{
	public:

		PickUp();
		void Create_pickup(b2World* world_, float, float);
		void Update(float);

		bool spawned;
};
#endif