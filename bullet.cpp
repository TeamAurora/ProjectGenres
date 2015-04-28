#include "bullet.h"

Bullet::Bullet()
{
	type_ = BULLET;
	move_v = 1;
	created = false;
	offsetAmount = 30;
	slowAmount = 5.0f;
}


void Bullet::CreateBullet(b2World* world_, float x, float y, b2Vec2 gravity, b2Vec2 playerPos)
{
	//start bools as false
	dead = false;

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
	body_half_width = 0.15f;
	body_half_height = 0.15f;

	//assigning values to body
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.x = bodyInitialPosition.x;
	bodyDef.position.y = bodyInitialPosition.y;
	bodyDef.bullet = true;
	AddBody(world_, bodyDef);

	b2PolygonShape box;
	box.SetAsBox(body_half_width, body_half_height);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &box;
	fixtureDef.density = 0.1f;
	fixtureDef.friction = 0.5f;
	fixtureDef.restitution = 0.1f; // bouncieness
	AddFixture(fixtureDef);

	//set sprite size to match body
	set_width(BOX2D_GFX_SIZE(2*body_half_width));
	set_height(BOX2D_GFX_SIZE(2*body_half_height));

	created = true;
}

void Bullet::Update(float ticks)
{
	//update sprite position to match body
	UpdatePosition();

	//get vector between player's current and bullet start position
	b2Vec2 aimVector;
	aimVector.x = (target_.x - position().x);
	aimVector.y = -(target_.y - position().y);//negative due to screen's y-axis
	aimVector.Normalize();
	aimVector.x = aimVector.x*slowAmount;
	aimVector.y = aimVector.y*slowAmount;

	//assign as force
	force = aimVector;

	body_->ApplyForceToCenter(force);
}