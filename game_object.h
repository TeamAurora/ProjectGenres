#ifndef _GAME_OBJECT_H
#define _GAME_OBJECT_H

#include "sprite.h"
#include "math.h"
#include <maths/vector2.h>
#include <maths/math_utils.h>
#include <Box2D/Box2D.h>
#include <input/vita/sony_controller_input_manager_vita.h>
#include "box2d_helpers.h"

namespace abfw
{
	class Platform;
}

class GameObject : public Sprite
{	// Physics based game object
public:

	enum TYPE { PLAYER, COLLISIONTILE, ENEMY, PICKUP, PLANT, SPIKE, BLADE, BULLET }; // used for specific collision response - objects type

	GameObject();		// Default constructor for default gameobjects
	virtual ~GameObject();

	// Common Functions - these work fine with and without box2d
	void UpdatePosition(); // updates position_ with velocity_ or with box2d body_ position
	//void VerifyPosition(const abfw::Platform& platform_); // Used to make sure the object is within the bounds of the screen region
	bool CollisionTest(const GameObject& Object); // AABB bounding box collision detection between this object (one which called the function) and parameter object
	void RotateTo(const float degrees);	// Absolute rotation function
	void RotateBy(const float degrees); // Relative rotation function
	void ScaleBy(const float scalefactor); // DOESN'T WORK WITH BOX2D

	//getters and setters
	inline const TYPE getType() const { return type_; }
	inline void setType(const TYPE type) { type_ = type; }
	inline const bool visibility() const { return visibility_; }
	inline void set_visibility(const bool visibility) { visibility_ = visibility; }
	
	// Default Physics Functions - these work with box2d but shouldn't be used as box2d replaces their functionality
	void MoveTo(const float x, const float y);					// Only real use for these with an object using box2d physics is for teleportation
	void MoveBy(const float x, const float y);
	void AccelerateTo(const abfw::Vector2& velocity);			// Modifies body velocity directly if the object is using box2d physics
	void AccelerateBy(const abfw::Vector2& deltavelocity);
	
	// Box2D Physics Functions
	void AddBody(b2World* world, const b2BodyDef body_def);			// adds a body to this gameobject and changes physicsengine_ to box2d
	void DestroyBody();												// destroys the box2d body for this gameobject
	void AddFixture(const b2FixtureDef fixture_def);				// adds a fixture to the box2d body - not used but useful to have in case
	void Force(const b2Vec2& force, const b2Vec2& point);			// applies a force to the body associated with the gameobject
	void ForceToCentre(const b2Vec2& force);						// applies a force to the centre of the body associated with the gameobject
	void LinearImpulse(const b2Vec2& impulse, const b2Vec2& point);	// applies an impulse to the body associated with the gameobject

/////John// 
	void CreateStaticBody(b2World* world_, float x , float y, float width, float height);//create Static body at (x,y) with that width & height
	void Knockback(b2Vec2, b2Vec2);//knock object back when they take damage
	void setAnimation();//set up plants for death animation

	bool dead;//flag for killing
	bool destroyed;// to check if object has removed from game

	//animation
	bool deadAnim;//has the death animation been played
	bool rotated;//are the plants rotated
//////////
protected:
	enum PHYSICSENGINE { DEFAULT, BOX2D }; // used to keep same functions being usable independent of physics engine (switch statement in each function handles switching between functionality)
	
	b2Body* body_; // pointer to a box2d body

	TYPE type_;
	PHYSICSENGINE physicsengine_;
	bool visibility_; // flags object for updates, rendering and occasionally deletion
	abfw::Vector2 velocity_; // not used for gameobjects with a box2d body (since the world_ provides positions for them)
	b2World* world_; // pointer to a box2d world

///John////

	//Initial values for making body and sprites
	abfw::Vector2 bodyInitialPosition;
	float body_half_width;
	float body_half_height;
	float uv_x,uv_y, uv_width,uv_height;//initial uv values

	//Movement
	b2Vec2 force;//force for causing movement
	float move_v;//speed at which bodies move
	bool move;//movement flag
	
	//Knockback
	b2Vec2 knockbackForce_;//push object back when hit
	float magnitude_;///size of knockback force

///////
};

#endif // _GAME_OBJECT_H