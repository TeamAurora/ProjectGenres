#include "enemy.h"


Enemy::Enemy()
{
	//set object type
	type_ = ENEMY;
	
	//initalise values
	move_v = 4;
	threshold_ = 20;
	range_ = 150;
	patrolTime_ = 20;
}

void Enemy::Create_Enemy(b2World* world_,float x , float y)
{
	//values that need set each time an enemy is spawned
	dead = false;
	destroyed = false;
	moveTimer_ = 0;
	move = true;

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

void Enemy::Update_Enemy(float ticks, b2Vec2 playerPos, bool patrol)
{
	//update sprite position to match body
	float enemy_new_x = BOX2D_GFX_POS_X(body_->GetPosition().x);
	float enemy_new_y = BOX2D_GFX_POS_Y(body_->GetPosition().y);

	set_position(abfw::Vector3(enemy_new_x,enemy_new_y,0.f));
	set_rotation(-body_->GetAngle());
	//gives position - used for spawning bullets
	x = enemy_new_x;
	y = enemy_new_y;

	patrol_ = patrol;

	if(patrol == true)
	{
		Patrol(ticks);
	}

	Attack(playerPos, enemy_new_x, enemy_new_y);//move enemy to hurt player

	//apply gravity
	body_->ApplyForceToCenter(gravity);
}

void Enemy::Attack(b2Vec2 playerPos, float enemyX, float enemyY)
{
	//check vertically if enemy is on a wall
	if(gravity.y == 0)
	{
		if(playerPos.x > (enemyX - threshold_) && playerPos.x < (enemyX + threshold_))//enemy  and player close to the same x cooridinate level
		{
			if(playerPos.y > (enemyY - range_) && playerPos.y < (enemyY + range_))//player is in attack range
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
		if(playerPos.y > (enemyY - threshold_) && playerPos.y < (enemyY + threshold_))//enemy  and player close to the same y cooridinate level
		{
			if(playerPos.x > (enemyX - range_) && playerPos.x < (enemyX + range_))//player is in attack range
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


void Enemy::Patrol(float ticks)
{
	//make enemies move back and forth
	if(moveTimer_ == 0 && move == true)
	{
		force.Set(-move_v,0.0f);
	}

	 if(moveTimer_ >= patrolTime_)
	{
		move = false;
		force.Set(move_v,0.0f);
	}

	if(moveTimer_ >= (patrolTime_*2) && move == false)
	{
		move = true;
		force.Set(-move_v,0.0f);
		moveTimer_ = 0;
	}

	//set velocity cap
	if ( force.x > move_v)
	{
		force.Set(move_v,0.0f);
	}
	else if (force.x < -move_v)
	{
		force.Set(-move_v,0.0f);
	}

	moveTimer_+= (ticks*20);
	body_->ApplyForceToCenter(force);
}

