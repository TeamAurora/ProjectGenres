#ifndef PLANT_H
#define PLANT_H

#include "game_object.h"

class Plant : public GameObject
{
	public:
		Plant();
		void CreatePlant(b2World* world_, float x , float y, float width, float height);
};
#endif 