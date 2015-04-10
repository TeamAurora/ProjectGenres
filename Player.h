///////////John & Rebecca//////////
#pragma once
#include "living_object.h"
#include "CollisionTile.h"

class Player :
	public LivingObject
{

public:

	Player();
	virtual ~Player();

	void Create_Player(b2World* world_, float x, float y); // make player body and match it to sprite
	void Update(const float& ticks_, bool gameOver, bool flying);// player update loop - called from UpdateGameObjects in gamestate
	void Player_Input(const class abfw::SonyController* controller_);	// player input loop - called from InputLoop in gamestate
	void DetermineOrientation(CollisionTile* collisiontile); // deterimes orientation after colliding with a collisiontile

	b2Vec2 gravity;//personal gravity
	
	bool attacking;//attacking
	bool hurting;//is player taking damage
	bool gameOver;//has game stopped

	bool horizontal;
	bool move_right, moveUp;//flag for telling player direction used to change animation/place blade

	//current varaibles of player
	b2Vec2 currentPos;
	float currentRayAngle; // angle of raycast line displayed on screen

	/////// Enumerated states player can be in - use these to do logic & for sprite animation
	/*enum PLAYERSTATE { DEAD, IDLE, INAIR, RUNNING, ATTACKING };
	PLAYERSTATE state_;*/
	enum OBJECTSTATE { IDLE, INAIR, GROUNDED, RUNNING, ATTACKING, DEAD, JUMPING };
	OBJECTSTATE state_;
	
private:
	//set up uv coords for animations
	//void setUVCoordinates(float width, float height);//take the height and width of single frame of the sprite
	void runningAnimation();
	void idleAnimation();
	void deadAnimation();
	void jumpAnimation(float, float);

	void changeState();

	float damage;//amount of damage taken
	b2Vec2 knockbackForce;//push player back when hit

	enum Direction { UP, DOWN, LEFT, RIGHT };
	Direction gDir;

	void setGravity(Direction direction);

	float jumpCutOff, moveCutOff;//how far the sticks have to be pushed to trigger action
	float rayLength;
	bool mflying;
};

