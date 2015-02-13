#ifndef COLLECTIBLE_H
#define COLLECTIBLE_H

#include "game_object.h"

class PickUp : public GameObject
{
	public:
		//functions
		PickUp();//constructor
		void Create_pickup(b2World* world_, float, float);// create at x/y coordiantes
		void Update(float);

		bool spawned;
};
#endif