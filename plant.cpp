#include "plant.h"

Plant::Plant()
{
	type_ = PLANT;
}

void Plant::CreatePlant(b2World* world_,float x , float y, float width, float height)
{
	dead = false;
	destroyed = false;

	body_half_width = width;
	body_half_height = height;

	//set position
	bodyInitialPosition.x = x;
	bodyInitialPosition.y = y;

	// setup the plant definition
	b2BodyDef plant_body_def;
	plant_body_def.position.x = bodyInitialPosition.x;
	plant_body_def.position.y = bodyInitialPosition.y;

	// create the plant
	body_ = world_->CreateBody(&plant_body_def);

	// set the shape for the plant
	b2PolygonShape plant_shape;
	plant_shape.SetAsBox(body_half_width, body_half_height);

	// bind the shape to the body
	body_ -> CreateFixture(&plant_shape, 0.0f);

	//set plant sprite size
	set_width(BOX2D_GFX_SIZE(2*body_half_width));
	set_height(BOX2D_GFX_SIZE(2*body_half_height));

	//set plant sprite position
	float plant_x = BOX2D_GFX_POS_X(body_->GetPosition().x);
	float plant_y = BOX2D_GFX_POS_Y(body_->GetPosition().y);

	body_->SetUserData(this);
	
	set_position(abfw::Vector3(plant_x,plant_y,1));
}