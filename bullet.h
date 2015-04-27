#ifndef BULLET_H
#define BULLET_H

#include "game_object.h"

class Bullet : public GameObject
{
	public:
		Bullet();
		void CreateBullet(b2World* world_, float x, float y, b2Vec2 gravity, b2Vec2 playerPos);
		void Update(float ticks);

		bool created;

	private:
		b2Vec2 offset;
		float offsetAmount;
		b2Vec2 target_;
		float slowAmount;
};
#endif