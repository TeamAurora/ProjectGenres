#include "enemy.h"


Enemy::Enemy()
{
	//set object type
	type_ = ENEMY;
	
	//initalise values
	move_v = 4;
	threshold = 20;
	range = 150;
}

void Enemy::Create_Enemy(b2World* world_,float x , float y)
{
	//start bools as false
	dead = false;
	destroyed = false;

	//starting position
	bodyInitialPosition.x = x;
	bodyInitialPosition.y = y;

	//size
	body_half_width = 0.25f;
	body_half_height = 0.25f;

	//assigning values to body
	b2BodyDef enemy_bodyDef;
	enemy_bodyDef.type = b2_dynamicBody;
	enemy_bodyDef.position.x = bodyInitialPosition.x;
	enemy_bodyDef.position.y = bodyInitialPosition.y;
	body_ = world_->CreateBody(&enemy_bodyDef);

	b2PolygonShape enemy_Box;
	enemy_Box.SetAsBox(body_half_width, body_half_height);

	b2FixtureDef enemy_fixtureDef;
	enemy_fixtureDef.shape = &enemy_Box;
	enemy_fixtureDef.density = 1.0f;
	enemy_fixtureDef.friction = 0.5f;
	enemy_fixtureDef.restitution = 0.1f; // bouncieness
	body_->CreateFixture(&enemy_fixtureDef);


	//set sprite size to match body
	set_width(BOX2D_GFX_SIZE(2*body_half_width));
	set_height(BOX2D_GFX_SIZE(2*body_half_height));

	body_->SetUserData(this);

	//set colour red
	set_colour(0xff0000ff);

}

void Enemy::Update_Enemy(float ticks, b2Vec2 playerPos)
{
	//update sprite position to match body
	float enemy_new_x = BOX2D_GFX_POS_X(body_->GetPosition().x);
	float enemy_new_y = BOX2D_GFX_POS_Y(body_->GetPosition().y);

	set_position(abfw::Vector3(enemy_new_x,enemy_new_y,0.f));
	set_rotation(-body_->GetAngle());

	Attack(playerPos, enemy_new_x, enemy_new_y);//move enemy to hurt player

	//apply gravity
	body_->ApplyForceToCenter(gravity);
}

void Enemy::Attack(b2Vec2 playerPos, float enemyX, float enemyY)
{
	//check vertically if enemy is on a wall
	if(gravity.y == 0)
	{
		if(playerPos.x > (enemyX - threshold) && playerPos.x < (enemyX + threshold))//enemy  and player close to the same x cooridinate level
		{
			if(playerPos.y > (enemyY - range) && playerPos.y < (enemyY + range))//player is in attack range
			{
				if(playerPos.y > enemyY)//if below move down
				{
					force.Set(0,-move_v);
				}
				else if(playerPos.y < enemyY)//if above move up
				{
					force.Set(0,move_v);
				}

				body_->ApplyForceToCenter(force);
			}

		}
	}
	else if (gravity.x == 0)//check horizontally if enemy is on ground or roof
	{
		if(playerPos.y > (enemyY - threshold) && playerPos.y < (enemyY + threshold))//enemy  and player close to the same y cooridinate level
		{
			if(playerPos.x > (enemyX - range) && playerPos.x < (enemyX + range))//player is in attack range
			{
				if(playerPos.x > enemyX)//if to the right move right
				{
					force.Set(move_v,0);
				}
				else if(playerPos.x < enemyX)//if to the left move left
				{
					force.Set(-move_v,0);
				}

				body_->ApplyForceToCenter(force);
			}
		}
	}
}