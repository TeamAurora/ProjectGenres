#include "collectible.h"

PickUp::PickUp()
{
	//set object type
	type_ = PICKUP;
	spawned = false;
}

void PickUp::Create_pickup(b2World* world_, float x, float y)
{
	//for non destroyed objects
	dead = false;
	destroyed = false;

	//set size
	body_half_width = 0.2;
	body_half_height = 0.2;

	//set position
	bodyInitialPosition.x = x;
	bodyInitialPosition.y = y;

	// setup the body definition
	b2BodyDef body_def;
	body_def.type = b2_dynamicBody;
	body_def.position.x = bodyInitialPosition.x;
	body_def.position.y = bodyInitialPosition.y;
	body_def.fixedRotation = true;//stops body rotating 
	body_ = world_->CreateBody(&body_def);// create the body

	// set the shape for the ground
	b2PolygonShape pickUp_shape;
	pickUp_shape.SetAsBox(body_half_width, body_half_height);

	// bind the shape to the body
	b2FixtureDef fixtureDef;
	fixtureDef.shape = &pickUp_shape;
	fixtureDef.density = 0.1f;//set mass
	body_->ResetMassData();//it only sets new value after this is called
	fixtureDef.friction = 0.75f;
	fixtureDef.restitution = 0.1f; // not bouncy
	body_->CreateFixture(&fixtureDef);
//	body -> CreateFixture(&pickUp_shape, 0.0f);

	//set  sprite variables
	set_width(BOX2D_GFX_SIZE(2*body_half_width));
	set_height(BOX2D_GFX_SIZE(2*body_half_height));
	float sprite_x = BOX2D_GFX_POS_X(body_->GetPosition().x);
	float sprite_y = BOX2D_GFX_POS_Y(body_->GetPosition().y);
	
	body_->SetUserData(this);

	set_position(abfw::Vector3(sprite_x,sprite_y,1));
}


void PickUp::Update(float ticks)
{
	//update sprite position to match body
	float new_x = BOX2D_GFX_POS_X(body_->GetPosition().x);
	float new_y = BOX2D_GFX_POS_Y(body_->GetPosition().y);

	set_position(abfw::Vector3(new_x,new_y,0.f));
	set_rotation(-body_->GetAngle());
}