#include "platforms.h"


Platform::Platform()
{
	//set object type
	type_ = PLATFORM;
}

void Platform::Create_platform(b2World* world_,float x , float y, float width, float height)
{
	body_half_width = width;
	body_half_height = height;

	//set position
	bodyInitialPosition.x = x;
	bodyInitialPosition.y = y;

	// setup the ground definition
	b2BodyDef ground_body_def;
	ground_body_def.position.x = bodyInitialPosition.x;
	ground_body_def.position.y = bodyInitialPosition.y;

	// create the ground
	body_ = world_->CreateBody(&ground_body_def);

	// set the shape for the ground
	b2PolygonShape ground_shape;
	ground_shape.SetAsBox(body_half_width, body_half_height);

	// bind the shape to the body
	body_ -> CreateFixture(&ground_shape, 0.0f);

	//set ground sprite size
	set_width(BOX2D_GFX_SIZE(2*body_half_width));
	set_height(BOX2D_GFX_SIZE(2*body_half_height));

	//set ground sprite position
	float ground_x = BOX2D_GFX_POS_X(body_->GetPosition().x);
	float ground_y = BOX2D_GFX_POS_Y(body_->GetPosition().y);

	body_->SetUserData(this);

	set_position(abfw::Vector3(ground_x,ground_y,1));

}