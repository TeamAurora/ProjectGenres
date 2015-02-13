#ifndef SPIKE_H
#define SPIKE_H

#include "game_object.h"

class Spike : public GameObject
{
	public:
		Spike();
		void CreateSpike(b2World* world_, float x, float y, float width, float height);
};
#endif