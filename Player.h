#pragma once
#include "living_object.h"
class Player :
	public LivingObject
{

public:

	Player();
	virtual ~Player();

	void Create_Player(b2World* world_, float width, float height); // make player body and match it to sprite
	void Update(const float& ticks_, bool gameOver);						// player update loop - called from UpdateGameObjects in gamestate
	void Player_Input(const class abfw::SonyController* controller_manager_);	// player input loop - called from InputLoop in gamestate

	//////// to knockback player use gameobject Force or ForceToCentre function
	//////// inherits health and related functions from being a livingobject - no need to implement here

	b2Vec2 gravity;//personal gravity

	float jump_v;//jump force
	float jump_gap;//time between being able to jump

	bool attacking;//attacking
	bool hurting;//is player taking damage
	bool gameOver;//has game stopped
	bool horizontal;
	bool move_right, moveUp;//flag for telling player direction used to change animation/place blade
	bool grounded;//player is touching a platform
	b2Vec2 currentPos;

private:

	/////// Enumerated states player can be in - use these to do logic
	enum OBJECTSTATE { IDLE, INAIR, GROUNDED, RUNNING, ATTACKING };
	OBJECTSTATE state_;

	float damage;//amount of damage taken
	b2Vec2 knockbackForce;//push player back when hit

	enum gravityDirection { UP, DOWN, LEFT, RIGHT };
	gravityDirection gDir;

	float jumpCutOff, moveCutOff;//how far the sticks have to be pushed to trigger action

};

