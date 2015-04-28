///////////John & Rebecca//////////
#pragma once
#include "living_object.h"
#include "CollisionTile.h"
//John
//Minor editing by Craigs
class Player :
	public LivingObject
{

public:

	Player();
	virtual ~Player();

	void Create_Player(b2World* world_, float x, float y); // make player body and match it to sprite
	void Update(const float& ticks_, bool gameOver, bool flying);// player update loop - called from UpdateGameObjects in gamestate
	void Player_Input(const class abfw::SonyController* controller_);	// player input loop - called from InputLoop in gamestate
	//Craig
	void ResolveCollisionTile(CollisionTile* collisiontile); // deterimes orientation after colliding with a collisiontile

	b2Vec2 gravity;//personal gravity
	
	bool attacking;//attacking
	bool hurting;//is player taking damage
	bool gameOver;//has game stopped

	bool horizontal;
	bool move_right, moveUp;//flag for telling player direction used to change animation/place blade

	float currentRayAngle; // angle of raycast line displayed on screen
	bool stickPushed;//has the right stick been puhed in any directio
	bool touchingPlatform;

	enum OBJECTSTATE { IDLE, INAIR, GROUNDED, RUNNING, ATTACKING, DEAD, JUMPING, FLYING };
	OBJECTSTATE state_;
	bool flying_;

private:
	//set up uv coords for animations
	//void setUVCoordinates(float width, float height);//take the height and width of single frame of the sprite
	void runningAnimation();
	void idleAnimation();
	void deadAnimation();
	void attackAnimation();
	void jumpAnimation(float, float);
	void flyAnimation();

	void changeState();

	float damage;//amount of damage taken

	enum Direction { UP, DOWN, LEFT, RIGHT };
	Direction gDir;

	void setGravity(Direction direction);

	float jumpCutOff, moveCutOff;//how far the sticks have to be pushed to trigger action
	float jumpStrength;
	bool setFlyingWidth;
};

