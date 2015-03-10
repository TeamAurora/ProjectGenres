#include "bullet.h"

Bullet::Bullet()
{
	type_ = BULLET;
	move_v = 1;
	created = false;
	offsetAmount = 30;
	slowAmount = 100;
}


void Bullet::CreateBullet(b2World* world_, float x, float y, b2Vec2 gravity, b2Vec2 &playerPos)
{
	//start bools as false
	dead = false;
	destroyed = false;

	//place starting position outside enemy
	if(gravity.x > 0)
	{
		offset.x = -offsetAmount;
	}
	else if(gravity.x < 0)
	{
		offset.x = offsetAmount;
	}
	else
	{
		offset.x = 0;
	}
	
	if(gravity.y < 0)
	{
		offset.y = -offsetAmount;
	}
	else if(gravity.y > 0)
	{
		offset.y = offsetAmount;
	}
	else
	{
		offset.y = 0;
	}

	//set player as target
	target_ = playerPos;

	//starting position
	bodyInitialPosition.x = GFX_BOX2D_POS_X(x + offset.x);
	bodyInitialPosition.y = GFX_BOX2D_POS_Y(y + offset.y);
	
	//size
	body_half_width = 0.075f;
	body_half_height = 0.075f;

	//assigning values to body
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.x = bodyInitialPosition.x;
	bodyDef.position.y = bodyInitialPosition.y;
	bodyDef.bullet = true;
	body_ = world_->CreateBody(&bodyDef);

	b2PolygonShape box;
	box.SetAsBox(body_half_width, body_half_height);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &box;
	fixtureDef.density = 0.1f;
	fixtureDef.friction = 0.5f;
	fixtureDef.restitution = 0.1f; // bouncieness
	body_->CreateFixture(&fixtureDef);

	//set sprite size to match body
	set_width(BOX2D_GFX_SIZE(2*body_half_width));
	set_height(BOX2D_GFX_SIZE(2*body_half_height));

	body_->SetUserData(this);

	//set colour black
	set_colour(0xff00ff00);

	created = true;
}

void Bullet::Update(float ticks)
{
	//update sprite position to match body
	float new_x = BOX2D_GFX_POS_X(body_->GetPosition().x);
	float new_y = BOX2D_GFX_POS_Y(body_->GetPosition().y);

	set_position(abfw::Vector3(new_x,new_y,0.f));
	set_rotation(-body_->GetAngle());

	//get vector between player's current and bullet start position
	b2Vec2 aimVector;
	aimVector.x = (target_.x - new_x)/slowAmount;
	aimVector.y = -(target_.y - new_y)/slowAmount;//negative due to screen's y-axis
	
	//assign as force
	force = aimVector;

	body_->ApplyForceToCenter(force);
}