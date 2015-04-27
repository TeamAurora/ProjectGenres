#include "plant.h"

Plant::Plant()
{
	type_ = PLANT;
}

void Plant::CreateBody(b2World* _world)
{
	//reset all variable for removing object
	dead = false;
	collided = false;
	
	body_half_width = width() / 2.0f;
	body_half_height = height() / 2.0f;

	//set position
	spawn_position.x = position().x;
	spawn_position.y = position().y;

	// setup the ground definition
	b2BodyDef body_def;
	body_def.type = b2_staticBody;
	body_def.position.x = GFX_BOX2D_POS_X(spawn_position.x);
	body_def.position.y = GFX_BOX2D_POS_Y(spawn_position.y);
	AddBody(_world, body_def);

	// set the shape for the object
	b2PolygonShape shape;
	shape.SetAsBox(GFX_BOX2D_SIZE(body_half_width), GFX_BOX2D_SIZE(body_half_height));

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &shape;
	fixtureDef.density = 0.1f;
	fixtureDef.friction = 0.1f;
	// bind the shape to the body
	AddFixture(fixtureDef);

	//set sprite position
	UpdatePosition();
}