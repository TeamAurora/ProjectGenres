#include "blade.h"

Blade::Blade()
{
	type_ = BLADE;
	created = false;
	armLength = 50;
}

void Blade::Create(b2World* world_,const Player &player)
{
	//varaibles that need reset if player has been killed
	dead = false;
	destroyed = false;

	alignFace(player);

	//create body
	bodyInitialPosition.x = GFX_BOX2D_POS_X(player.currentPos.x + xOffset);
	bodyInitialPosition.y = GFX_BOX2D_POS_Y(player.currentPos.y + yOffset);

	body_half_width = 0.05f;
	body_half_height = 0.1f;

	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.x = bodyInitialPosition.x;
	bodyDef.position.y = bodyInitialPosition.y;
	bodyDef.fixedRotation = true;//stops body rotating 
	body_ = world_->CreateBody(&bodyDef);

	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(body_half_width, body_half_height);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &dynamicBox;
	fixtureDef.density = 2.0f;//set mass
	body_->ResetMassData();//it only sets new value after this is called
	fixtureDef.friction = 0.5f;
	fixtureDef.restitution = 0.1f; // not bouncy
	body_->CreateFixture(&fixtureDef);

	body_->SetUserData(this);

	//set sprite size to match body
	set_width(BOX2D_GFX_SIZE(2*body_half_width));
	set_height(BOX2D_GFX_SIZE(2*body_half_height));

	set_colour(0xffff0000);
	
	created = true;

	//uv height, width and position
	//set_uv_height(uv_height);
	//set_uv_width(uv_width);
	//set_uv_position(abfw::Vector2(uv_x,uv_y));
}

void Blade::Update(float ticks,const Player &player)
{
	alignFace(player);

	//update sprite position to match body
	float new_x = player.currentPos.x + xOffset;
	float new_y = player.currentPos.y + yOffset;

	set_position(abfw::Vector3(new_x,new_y,0.f));
	set_rotation(-body_->GetAngle());
}

void Blade::alignFace(const Player &player)
{
	
	if(player.horizontal == true)
	{
		yOffset = 0;

		if(player.move_right == true)
		{
			xOffset = armLength;
		
		}
		else 
		{
			xOffset = -armLength;
			
		}
	}
	else
	{
		xOffset = 0;

		if (player.moveUp == true)
		{
			yOffset = -armLength;
		}
		else
		{
			yOffset = armLength;
		}
	}
}
	

