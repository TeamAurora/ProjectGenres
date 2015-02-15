#include "game_object.h"
#include "maths\math_utils.h"
#include <system/platform.h>
#include "box2d_helpers.h"

GameObject::GameObject() :
	visibility_(false),
	velocity_(0.0f, 0.0f),
	world_(NULL),
	body_(NULL),
	physicsengine_(DEFAULT),
	magnitude_(150)
{
}

GameObject::~GameObject()
{
}

void GameObject::MoveTo(const float x, const float y)
{
	switch(physicsengine_)
	{
	case DEFAULT:
		position_.x = x;
		position_.y = y;
		break;
	case BOX2D:
		position_.x = x;
		position_.y = y;
		body_->SetTransform(b2Vec2(GFX_BOX2D_POS_X(position_.x), GFX_BOX2D_POS_Y(position_.y)), body_->GetAngle()); // sets body location after calculating new location
		break;
	}
}

void GameObject::MoveBy(const float x, const float y)
{
	switch(physicsengine_)
	{
	case DEFAULT:
		position_.x += x;
		position_.y += y;
		break;
	case BOX2D:
		position_.x += x;
		position_.y += y;
		body_->SetTransform(b2Vec2(GFX_BOX2D_POS_X(position_.x), GFX_BOX2D_POS_Y(position_.y)), body_->GetAngle()); // sets body location after calculating new location
		break;
	}
}

void GameObject::UpdatePosition()
{
	switch(physicsengine_)
	{
	case DEFAULT:
		position_.x += velocity_.x;
		position_.y += velocity_.y;
		break;
	case BOX2D:
		position_.x = BOX2D_GFX_POS_X(body_->GetPosition().x);
		position_.y = BOX2D_GFX_POS_Y(body_->GetPosition().y);
		break;
	}
}

bool GameObject::CollisionTest(const GameObject& Object)
{ // Checks x axis first, then y axis, if we can't rule out both then a collision has definitely occurred
	if((position().x + width()/2 < Object.position().x - Object.width()/2) || (position().x - width()/2 > Object.position().x + Object.width()/2))
	{ // If ObjectAs right edge is less than ObjectBs left edge or ObjectAs left edge is more than ObjectBs right edge, there is no collision.
		return false;
	}
	else if((position().y + height()/2 < Object.position().y - Object.height()/2) || (position().y - height()/2 > Object.position().y + Object.height()/2))
	{ // If ObjectAs top edge is less than ObjectBs bottom edge or ObjectAs bottom edge is more than ObjectBs top edge, there is no collision.
		return false;
	}
	else
	{ // if previous 2 tests are passed then there is a collision
		return true;
	}
}

void GameObject::VerifyPosition(const abfw::Platform& platform_)
{
	if((position().x > platform_.width()) || (position().x < 0) || (position().y > platform_.height()) || (position().y < 0)) // If out of screen bounds
	{
		set_visibility(false); // sets object to not visible, essentially removing it from scene
	}
}

void GameObject::AccelerateTo(const abfw::Vector2& velocity)
{
	switch(physicsengine_)
	{
	case DEFAULT:
		velocity_.x = velocity.x;
		velocity_.y = velocity.y;
		break;
	case BOX2D:
		body_->SetLinearVelocity(b2Vec2(velocity.x, velocity.y)); // manually sets body velocity to the body specified
		break;
	}
}

void GameObject::AccelerateBy(const abfw::Vector2& deltavelocity)
{
	switch(physicsengine_)
	{
	case DEFAULT:
		velocity_.x += deltavelocity.x;
		velocity_.y += deltavelocity.y;
		break;
	case BOX2D:
		velocity_.x = body_->GetLinearVelocity().x + deltavelocity.x;
		velocity_.y = body_->GetLinearVelocity().y + deltavelocity.y;
		body_->SetLinearVelocity(b2Vec2(velocity_.x, velocity_.y)); // updates body velocity with new velocity
		break;
	}
}

void GameObject::RotateTo(const float degrees)
{
	switch(physicsengine_)
	{
	case DEFAULT:
		rotation_ = abfw::DegToRad(degrees);
		break;
	case BOX2D:
		rotation_ = abfw::DegToRad(degrees);
		body_->SetTransform(body_->GetPosition(), rotation_); // rotates the box2d body also
		break;
	}
}

void GameObject::RotateBy(const float degrees)
{
	switch(physicsengine_)
	{
	case DEFAULT:
		rotation_ += abfw::DegToRad(degrees);
		break;
	case BOX2D:
		rotation_ += abfw::DegToRad(degrees);
		body_->SetTransform(body_->GetPosition(), rotation_); // rotates the box2d body also
		break;
	}
}

void GameObject::ScaleBy(const float scalefactor)
{
	switch(physicsengine_)
	{
	case DEFAULT:
		width_ *= scalefactor;
		height_ *= scalefactor;
		break;
	case BOX2D:
		width_ *= scalefactor;
		height_ *= scalefactor;
		
		// TO WRITE (change all of the bodies fixture sizes by scalefactor) Not important as I don't use this function at all right now
		break;
	}
}

void GameObject::AddBody(b2World* world, const b2BodyDef body_def)
{
	world_ = world;					// sets world pointer for box2d
	physicsengine_ = BOX2D;			// changes to box2d physics for this object
	body_ = world_->CreateBody(&body_def);
	body_->SetUserData(this);
}

void GameObject::DestroyBody()
{
	world_->DestroyBody(body_);	// destroys the box2d body of this object
	physicsengine_ = BOX2D;		// changes back to default physics engine for this object
	world_ = NULL;			// sets world pointer to null
}

void GameObject::AddFixture(const b2FixtureDef fixture_def)
{
	body_->CreateFixture(&fixture_def);
}

void GameObject::Force(const b2Vec2& force, const b2Vec2& point)
{
	body_->ApplyForce(force, point);
}

void GameObject::ForceToCentre(const b2Vec2& force)
{
	body_->ApplyForceToCenter(force);
}

void GameObject::LinearImpulse(const b2Vec2& impulse, const b2Vec2& point)
{
	body_->ApplyLinearImpulse(impulse, point);
}

//John//////////////////////////
void GameObject::CreateStaticBody(b2World* world_,float x , float y, float width, float height)
{
	dead = false;
	destroyed = false;

	body_half_width = width;
	body_half_height = height;

	//set position
	bodyInitialPosition.x = x;
	bodyInitialPosition.y = y;

	// setup the ground definition
	b2BodyDef body_def;
	body_def.position.x = bodyInitialPosition.x;
	body_def.position.y = bodyInitialPosition.y;

	// create the ground
	body_ = world_->CreateBody(&body_def);

	// set the shape for the object
	b2PolygonShape shape;
	shape.SetAsBox(body_half_width, body_half_height);

	// bind the shape to the body
	body_ -> CreateFixture(&shape, 0.0f);

	//set ground sprite size
	set_width(BOX2D_GFX_SIZE(2*body_half_width));
	set_height(BOX2D_GFX_SIZE(2*body_half_height));

	//set sprite position
	float spriteX = BOX2D_GFX_POS_X(body_->GetPosition().x);
	float spriteY = BOX2D_GFX_POS_Y(body_->GetPosition().y);

	body_->SetUserData(this);

	set_position(abfw::Vector3(spriteX,spriteY,1));
}

void GameObject::Knockback(b2Vec2 pos1, b2Vec2 pos2)
{
	if(pos1.x - pos2.x <= 0)//push to left
	{	
		knockbackForce_.Set(magnitude_,0.0f);
	}
	else if(pos1.x - pos2.x > 0)//push to right
	{	
		knockbackForce_.Set(-magnitude_,0.0f);
	}

	if(pos1.y - pos2.y <= 0)//push down
	{
		knockbackForce_.Set(0.0f,-magnitude_);
	}
	else if(pos1.y - pos2.y > 0)//push up
	{
		knockbackForce_.Set(0.0f,magnitude_);
	}

	body_->ApplyForceToCenter(knockbackForce_);
}