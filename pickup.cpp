#include "pickup.h"

PickUp::PickUp()
{
	type_ = PICKUP;
}

void PickUp::RecreateBody(b2World* world_)
{
	//reset all variable for removing object
	dead = false;
	collided = false;
	
	body_half_width = width();
	body_half_height = height();

	//set position
	bodyInitialPosition.x = GFX_BOX2D_POS_X(position().x);
	bodyInitialPosition.y = GFX_BOX2D_POS_Y(position().y);

	// setup the ground definition
	b2BodyDef body_def;
	body_def.position.x = bodyInitialPosition.x;
	body_def.position.y = bodyInitialPosition.y;
	AddBody(world_, body_def);

	// set the shape for the object
	b2PolygonShape shape;
	shape.SetAsBox(GFX_BOX2D_SIZE(body_half_width), GFX_BOX2D_SIZE(body_half_height));

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &shape;
	fixtureDef.friction = 0.1f;
	//fixtureDef.isSensor = true;
	// bind the shape to the body
	AddFixture(fixtureDef);

	//set ground sprite size
	set_width(2*body_half_width);
	set_height(2*body_half_height);

	//set sprite position
	UpdatePosition();
}