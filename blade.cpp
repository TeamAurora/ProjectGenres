#include "blade.h"

Blade::Blade()
{
	type_ = BLADE;
	armLength = 100.0f;
	disabled = true;
}

void Blade::Create(b2World* world_,const Player &player)
{
	//varaibles that need reset if player has been killed
	dead = false;

	alignFace(player);

	//create body
	bodyInitialPosition.x = GFX_BOX2D_POS_X(player.position().x + xOffset);
	bodyInitialPosition.y = GFX_BOX2D_POS_Y(player.position().y + yOffset);

	body_half_width = 0.35f;
	body_half_height = 0.5f;

	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.x = bodyInitialPosition.x;
	bodyDef.position.y = bodyInitialPosition.y;
	bodyDef.fixedRotation = true;//stops body rotating 
	AddBody(world_, bodyDef);

	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(body_half_width, body_half_height);

	b2FixtureDef fixtureDef;
	//fixtureDef.isSensor = true;
	fixtureDef.shape = &dynamicBox;
	body_->ResetMassData();//it only sets new value after this is called
	AddFixture(fixtureDef);

	//set sprite size to match body
	set_width(BOX2D_GFX_SIZE(2*body_half_width));
	set_height(BOX2D_GFX_SIZE(2*body_half_height));

	set_colour(0xffff0000);
	
	disabled = false;
}

void Blade::Update(float ticks,const Player &player)
{
	if(!disabled)
	{
		alignFace(player);

		//update sprite position to match body
		float new_x = player.position().x + xOffset;
		float new_y = player.position().y + yOffset;

		MoveTo(new_x, new_y);
	}
}

void Blade::alignFace(const Player &player)
{
	if(player.horizontal == true || player.flying_ == true)
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
	else if(player.flying_ == false)
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
	

