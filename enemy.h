///////////////John///////////////
#ifndef ENEMY_H
#define ENEMY_H

#include "game_object.h"
//John
class Enemy : public GameObject
{
	public:
		Enemy();
		void Create_Enemy(b2World* world_, float x, float y);// create enemy at inputted position
		void Update_Enemy(float ticks, b2Vec2 playerPos, bool patrol);//update enemy
		void Advance(b2Vec2 playerPos,float enemyX,float enemyY);//check player position and if within range move to attack
		void Patrol(float ticks);

		b2Vec2 gravity;//keep the enemy on a surface

		// Enumerated states enemy can be in
		enum OBJECTSTATE {IDLE, MOVING, ATTACKING, DEAD, SHOOTING };
		OBJECTSTATE meleeState_;
		OBJECTSTATE shooterState_;

		bool shotFired;
		bool shooting;
		bool attack_;
private:
		//updates for each enemy type
		void MeleeUpdate(float ticks, b2Vec2 playerPos);
		void ShooterUpdate(float ticks);

		//set up for said animations
		void moveAnimation();
		void deathAnimation(bool);
		void shootAnimation();
		void attackAnimation();

		float threshold_;//check player is on same level
		float range_;//range on enemy attack
		bool patrol_;
		float moveTimer_;//time moving in one direction
		float patrolTime_;
};
#endif