#ifndef BULLET_H
#define BULLET_H

#include "game_object.h"
#include <iostream>

using namespace::std;

class Bullet : public GameObject
{
	public:
		Bullet();
		void CreateBullet(b2World* world_, float x, float y, b2Vec2 gravity);
		void Update(float ticks);

		bool created;
	private:
		b2Vec2 offset;
		float offsetAmount;
};
#endif