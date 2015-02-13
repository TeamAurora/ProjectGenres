#ifndef ENEMY_H
#define ENEMY_H

#include "game_object.h"
#include <iostream>
using namespace std;
class Enemy : public GameObject
{
	public:
		Enemy();//constructor
		void Create_Enemy(b2World* world_, float x, float y);// create enemy at inputted position
		void Update_Enemy(float, b2Vec2);//update enemy
		void Attack(b2Vec2 playerPos,float enemyX,float enemyY);//check player position and if within range move to attack

		b2Vec2 gravity;//keep the enemy on a surface

private:
		float threshold;//check player is on same level
		float range;//range on enemy attack
};
#endif