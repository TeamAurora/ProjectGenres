#include "enemy.h"


Enemy::Enemy()
{
	//set object type
	type_ = ENEMY;
	
	//initalise values
	move_v = 5;
	threshold_ = 20;
	range_ = 150;
	patrolTime_ = 20;

	//texture coords
	uv_x = 0.0f;
	uv_y = 0.0f;
	uv_width = 0.0625f;
	uv_height = 1.0f;
	meleeState_ = IDLE;
	shooterState_ = IDLE;

	shotFired = false;
}

void Enemy::Create_Enemy(b2World* world_,float x , float y)
{
	//values that need set each time an enemy is spawned
	dead = false;
	destroyed = false;
	moveTimer_ = 0;
	move = true;
	deadAnim = false;

	//starting position
	bodyInitialPosition.x = x;
	bodyInitialPosition.y = y;

	//size
	body_half_width = 0.4f;
	body_half_height = 0.4f;

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

	body_->SetUserData(this);

	//Sprite set up
	//set size to match body
	set_width(BOX2D_GFX_SIZE(2*body_half_width));
	set_height(BOX2D_GFX_SIZE(2*body_half_height));

	//uv height, width and position
	set_uv_height(uv_height);
	set_uv_width(uv_width);
	set_uv_position(abfw::Vector2(uv_x, uv_y));

	//set_colour(0xff0000ff);//red
}

void Enemy::Update_Enemy(float ticks, b2Vec2 playerPos, bool meleeEnemy)
{
	//update sprite position to match body
	float enemy_new_x = BOX2D_GFX_POS_X(body_->GetPosition().x);
	float enemy_new_y = BOX2D_GFX_POS_Y(body_->GetPosition().y);

	set_position(abfw::Vector3(enemy_new_x,enemy_new_y,0.f));
	set_rotation(-body_->GetAngle());

	//gives position - used for spawning bullets
	x = enemy_new_x;
	y = enemy_new_y;

	if(meleeEnemy)//update for melee enemies
	{
		MeleeUpdate(ticks, playerPos);
	}
	else
	{
		ShooterUpdate(ticks);
	}

	//apply gravity
	body_->ApplyForceToCenter(gravity);
}

void Enemy::MeleeUpdate(float ticks, b2Vec2 playerPos)
{
	bool result;

	//change state
	OBJECTSTATE prevState = meleeState_;

	if(dead == false)
	{
		Patrol(ticks);
		meleeState_ = MOVING;
	}	
	else
	{
		meleeState_ = DEAD;
	}

	
	Attack(playerPos, x, y);//move enemy to hurt player

	//set up animations for each state
	if(meleeState_ != prevState)
	{
		switch(meleeState_)
		{
			case MOVING:
					moveAnimation();
					break;			 
			case ATTACKING:

					break;
			case DEAD:
					deathAnimation(true);
					break;
				
		};
	}

	//move = true when moving left but Animate takes true for moving right
	//has to be switched to play animations
	bool facing;
	if(move)
	{
		facing = false;
	}
	else
	{
		facing = true;
	}
	
	//do animation
	result = Animate(ticks, facing);

	if(meleeState_ == DEAD)
	{
		deadAnim = result;
	}
}

void Enemy::ShooterUpdate(float ticks)
{
	bool result = false;

	if(shooterState_ == IDLE)
	{
		uv_width = 0.03125f;
		set_uv_width(uv_width);
		set_uv_height(-uv_height);	
		set_uv_position(abfw::Vector2 (0.0f,1.0f));
	}

	//change state
	OBJECTSTATE prevState = shooterState_;

	if(shooting == true && dead != true)
	{
		shooterState_ = SHOOTING;
	}
	else if(dead == true)
	{
		shooterState_ = DEAD;
	}
	else
	{
		shooterState_ = IDLE;
	}

	//set up animations for each state
	if(shooterState_ != prevState)
	{
		switch(shooterState_)
		{	 
			case SHOOTING:
					shootAnimation();
					break;
			case DEAD:
					deathAnimation(false);
					break;		
		};
	}

	//do animation
	result = Animate(ticks, true);

	//check for single play animations
	if(shooterState_ == SHOOTING)
	{
		shotFired = result;
	}

	if(shooterState_ == DEAD)
	{
		deadAnim = result;
	}

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
		force.Set(-move_v,0.0f);//left
		moveAnimation();
	}

	if(moveTimer_ >= patrolTime_  && moveTimer_ <= (patrolTime_ + 1))
	{
		move = false;
		force.Set(move_v,0.0f);//right
		moveAnimation();
	}

	if(moveTimer_ >= (patrolTime_*2) &&  moveTimer_ <= (patrolTime_*2 + 1) && move == false)
	{
		move = true;
		force.Set(-move_v,0.0f);//left again
		moveTimer_ = 0;		
		moveAnimation();
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

void Enemy::moveAnimation()
{
	if(moveTimer_ == 0 && move == true)
	{
		set_uv_width(-uv_width);// face left
	}

	if(moveTimer_ >= patrolTime_)
	{
		set_uv_width(uv_width);//face right
	}

	if(moveTimer_ >= (patrolTime_*2) && move == false)
	{
		set_uv_width(-uv_width);//face left
	}

	InitSpriteAnimation(0.025,16,true,SCROLL_X,0,0);
}

void Enemy::shootAnimation()
{
	set_uv_width(uv_width);
	set_uv_height(-uv_height);

	//make sure sprites starts from first frame
	set_uv_position(abfw::Vector2 (0.0f,1.0f));

	InitSpriteAnimation(0.025,32,false,SCROLL_X,0,0);
}

void Enemy::deathAnimation(bool melee)
{ 
	if(melee)
	{
		//set facing
		if(move)
		{
			set_uv_width(-uv_width);// face left
		}
		else
		{
			set_uv_width(uv_width);// face right
		}

		//make sure sprites starts from first frame
		set_uv_position(abfw::Vector2 (0.0f,0.0f));
	}
	else
	{
		set_uv_width(0.0625);
		//make sure sprites starts from first frame
		set_uv_position(abfw::Vector2 (0.0f,1.0f));
	}

	
	
	InitSpriteAnimation(0.07,16,false,SCROLL_X,0,0);
}