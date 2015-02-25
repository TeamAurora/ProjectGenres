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
	void Player_Input(const class abfw::SonyController* controller_);	// player input loop - called from InputLoop in gamestate
	
	void Player_Gravity(int n);


	b2Vec2 gravity;//personal gravity

	float jump_v;//jump force
	float jump_gap;//time between being able to jump

	bool attacking;//attacking
	bool hurting;//is player taking damage
	bool jumping;//jumping
	bool gameOver;//has game stopped
	bool horizontal;
	bool move_right, moveUp;//flag for telling player direction used to change animation/place blade
	bool grounded;//player is touching a platform
	b2Vec2 currentPos;
	b2Vec2 projectedPos;

	float currentRayAngle; // angle of raycast line displayed on screen

private:

	/////// Enumerated states player can be in - use these to do logic & for sprite animation
	enum OBJECTSTATE { IDLE, INAIR, GROUNDED, RUNNING, ATTACKING };
	OBJECTSTATE state_;

	float damage;//amount of damage taken
	b2Vec2 knockbackForce;//push player back when hit

	enum gravityDirection { UP, DOWN, LEFT, RIGHT };
	gravityDirection gDir;

	float jumpCutOff, moveCutOff;//how far the sticks have to be pushed to trigger action

	//float currentRayAngle; // angle of raycast line displayed on screen
	float rayLength;
};

