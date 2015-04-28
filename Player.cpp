#include "Player.h"
#include <input/sony_controller_input_manager.h>
#include "box2d_helpers.h"
#include <iostream>

/////John//////
Player::Player()
{
	// set gameobject type
	type_ = PLAYER;

	// start idle
	state_ = IDLE;

	//set values for variables
	move_v = 25;
	damage = 12.5;

	//texture coords
	uv_x = 0.0f;
	uv_y = 0.0f;
	uv_width = 0.03125f;
	uv_height = 0.5f;

	//player states
	attacking = false;
	hurting = false;
	move_right = false;
	horizontal = true;
	moveUp = true;
	deadAnim = false;//has death animation played
	stickPushed = false;
	touchingPlatform = false;

	//on stick axis
	jumpCutOff = 0.8;
	moveCutOff = 0.6;

	//jump angle in radians
	currentRayAngle = 0; //5.235987756 ;//300 radians
	jumpStrength = 25.0f;
	setGravity(DOWN);
}

Player::~Player()
{

}

void Player::Create_Player(b2World* world_, float x, float y)
{
	//varaibles that need reset if player has been killed
	dead = false;
	setMaxHealth(100.0f);
	setHealth(max_health());	
	state_ = IDLE;
	deadAnim = false;
	setFlyingWidth = false;

	// sets initial gravity to be down
	gravity = b2Vec2(0.0f, -10.0f);
	gDir = DOWN;

	float width = 128.0f;
	float height = 128.0f;

	//sprite set up
	//set sprite size to match body
	set_width(width);
	set_height(height);

	//create body
	bodyInitialPosition.x = x;
	bodyInitialPosition.y = y;

	body_half_width = GFX_BOX2D_SIZE(width/2.0f);
	body_half_height = GFX_BOX2D_SIZE(height/2.0f);

	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.x = bodyInitialPosition.x;
	bodyDef.position.y = bodyInitialPosition.y;
	bodyDef.linearDamping = 0.05f;
	bodyDef.fixedRotation = true;//stops body rotating 
	this->AddBody(world_, bodyDef);

	b2CircleShape shape;
	shape.m_p.Set(0, 0);
	shape.m_radius = body_half_width;

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &shape;
	fixtureDef.density = 1.0f;//set mass
	body_->ResetMassData();//it only sets new value after this is called
	fixtureDef.friction = 0.95f;
	fixtureDef.restitution = 0.0f; // not bouncy
	this->AddFixture(fixtureDef);

	UpdatePosition();

	//uv height, width and position
	set_uv_height(uv_height);
	set_uv_width(uv_width);
	set_uv_position(abfw::Vector2(uv_x, uv_y));
}

void Player::Update(const float& ticks, bool gameOver, bool flying)
{
	bool result;
	
	//update sprite position to match body
	UpdatePosition();

	flying_ = flying;//check what animation should be done

	if(setFlyingWidth == false && flying_ == true)//set inital sprite width
	{
		set_uv_width(0.0625f);
		set_uv_height(1);
		setFlyingWidth = true;
	}

	OBJECTSTATE prevState = state_;

	//damage player
	if(hurting && health() > 0)
	{
		updateHealth(-damage);	
		MoveBy(knockbackForce_.x,knockbackForce_.y);
	}

	//die
	if(health() <= 0)
	{
		dead = true;
	}

	changeState();//check variables and change player's state
	
	//set player's axis
	if((gDir == UP) || (gDir == DOWN))
	{
		horizontal = true;
	}
	else
	{
		horizontal = false;
	}

	if(flying_ == false)
	{
		//flip facing back the right way after jump has landed
		if(prevState == INAIR &&  state_ != INAIR)
		{
			if(move_right)
			{
				move_right = false;
			}
			else
			{
				move_right = true;
			}

			if(moveUp)
			{
				moveUp = false;
			}
			else
			{
				moveUp = true;
			}
		}
	}

	//set up animations for each state
	if(state_ != prevState)
	{
		switch(state_)
		{
			case IDLE:
				idleAnimation();
				break;
			case RUNNING:
				runningAnimation();
				break;			 
			case ATTACKING:
				attackAnimation();
				break;
			case DEAD:
				deadAnimation();
				break;
			case JUMPING:
				break;
			case FLYING:
				break;
		}
	}
		
	// play the animation
	if(horizontal == true)
	{
		result = Animate(ticks, move_right);
	}
	else
	{
		result = Animate(ticks, moveUp);
	}

	//check for animations that play once
	if(state_ == DEAD)
	{
		deadAnim = result;//death animation has played
	}

	//jump animation has played
	if (state_ == JUMPING && result == true)
	{
		state_ = INAIR;
	}

	//attack animation has played
	if(state_ == ATTACKING && result == true)
	{
		attacking = false;
		if(flying_)
		{
			//reset uv
			set_uv_width(0.0625f);
			set_uv_position(abfw::Vector2 (0.0f,0.0f));
		}
		//reset sprite size
		set_width(BOX2D_GFX_SIZE(2 * body_half_width));
		set_height(BOX2D_GFX_SIZE(2 * body_half_height));
	}
	
	if(!flying_)
	{
		if(state_ != JUMPING && state_ != INAIR)//no change in movement during jump
		{
			//apply gravity
			body_->ApplyForceToCenter(gravity);
		}
	}
	else
	{		
		//apply gravity
		body_->ApplyForceToCenter(gravity);
	}
}

void Player::Player_Input(const abfw::SonyController* controller)
{
	// check we have a valid controller object (one that isn't NULL)
	if (controller)
	{
////Rebecca//////////
		// Sets current gravity manipulation via player input
		//jetpack flight
		if(flying_ == true)
		{
			gDir = DOWN;
			if (controller->right_stick_y_axis() < -jumpCutOff)//up
			{
				//MoveBy(0.0f, -3.0f);
				gravity = b2Vec2(0.0f, 10.0f);
				state_ = FLYING;
				flyAnimation();
			}
			else if (controller->right_stick_x_axis() > jumpCutOff)//right
			{
				//MoveBy(3.0f, 0.0f);
				move_right = true;
				gravity = b2Vec2(10.0f, 00.0f);
				state_ = FLYING;
				flyAnimation();
			}
			else if (controller->right_stick_x_axis() < -jumpCutOff)//left
			{
				//MoveBy(-3.0f, 0.0f);
				move_right = false;
				gravity = b2Vec2(-10.0f, 0.0f);
				state_ = FLYING;
				flyAnimation();
			}
			else if (controller->right_stick_y_axis() > jumpCutOff)//down
			{
				//MoveBy(0.0f, 3.0f);
				gravity = b2Vec2(0.0f, -10.0f);
				state_ = FLYING;
				flyAnimation();
			}
		}
		else//wall jump
		{
			// change angle of projected jump
			float xAxisVal = controller->right_stick_x_axis();
			float yAxisVal = controller->right_stick_y_axis();
			currentRayAngle = atan2(xAxisVal, -yAxisVal); //* FRAMEWORK_RAD_TO_DEG;
			if(xAxisVal != 0 || yAxisVal != 0)
			{
				stickPushed = true;
			}
			else
			{
				stickPushed = false;
			}

			// jump mechanic
			if ((controller->buttons_pressed() & ABFW_SONY_CTRL_R2) && (state_ != INAIR))
			{
				b2Vec2 angle = b2Vec2(sinf(currentRayAngle), cosf(currentRayAngle));

				//Craig
				bool jump_possible = true;
				switch(gDir)
				{
				case DOWN:
					if (angle.y < 0)
					{
						jump_possible = false;
					}
					break;
				case UP:
					if (angle.y > 0)
					{
						jump_possible = false;
					}
					break;
				case LEFT:
					if (angle.x < 0)
					{
						jump_possible = false;
					}
					break;
				case RIGHT:
					if (angle.x > 0)
					{
						jump_possible = false;
					}
					break;
				}

				if(jump_possible)
				{
					b2Vec2 impulse = b2Vec2(jumpStrength * angle);
					//calls to set up animtaion for the jump
					state_ = JUMPING;
					jumpAnimation(xAxisVal, yAxisVal);
					body_->ApplyLinearImpulse(impulse, body_->GetWorldCenter());
				}
			}
		}

////////John/////////
		// checks what gravity is currently applied and dictates what movement
		// can be used ie. x axis movement on ground/ceiling, y axis movement on
		// walls
		if(!flying_)
		{
			if((state_ != INAIR) && (state_ != JUMPING))
			{
				if (gDir == DOWN || gDir == UP)
				{
					if (controller->left_stick_x_axis() > moveCutOff)//right
					{
						move = true;
						move_right = true;
						force.Set(move_v, 0.0f);
					}
					else if (controller->left_stick_x_axis() < -moveCutOff)//left
					{
						move = true;
						move_right = false;
						force.Set(-move_v, 0.0f);
					}
					else
					{
						move = false;
						force.SetZero();
						AccelerateTo(abfw::Vector2(0.0f,0.0f));
					}
				}
				else if (gDir == RIGHT || gDir == LEFT)
				{
					if (controller->left_stick_y_axis() > moveCutOff) // down
					{
						move = true;
						moveUp = false;
						force.Set(0.0f, -move_v);
					}
					else if (controller->left_stick_y_axis() < -moveCutOff) // up
					{
						move = true;
						moveUp = true;
						force.Set(0.0f, move_v);
					}
					else
					{
						move = false;
						force.SetZero();		
						AccelerateTo(abfw::Vector2(0.0f,0.0f));
					}
				}
			}
		}

///////John///////////////////
		//attack
		if ((controller->buttons_pressed() & ABFW_SONY_CTRL_L2) && (attacking == false))
		{
			attacking = true;
		}
	}
	body_->ApplyForceToCenter(force);
}

//// Craig
void Player::ResolveCollisionTile(CollisionTile* collisiontile)
{
	if(state_ == INAIR || state_ == JUMPING)
	{
		state_ = GROUNDED;
	}

	if(collisiontile->harmful_ == true)
	{
		this->dead = true;
	}
	else
	{
		switch(collisiontile->shapetype_)
		{
		case CollisionTile::BOX:
			{
				// Using collisiontile hints to determine which edge of tile is being collided with:
				//		Checking each edge to see if the centre of player is beyond that edge
				//		If true, then that edge is being collided with
				//		NOTE: it's possible to be colliding with multiple edges
				//		The else-if structure creates an edge-precedence order
				bool result = false;
				Direction direction(DOWN);
				//while(!result)
				//{
					if((collisiontile->edges_.DOWN == true) && (this->position().y > collisiontile->position().y + (collisiontile->height() / 2.0f)))
					{
						result = true;
						direction = UP;
					}
					if((collisiontile->edges_.UP == true) && (this->position().y < collisiontile->position().y - (collisiontile->height() / 2.0f)))
					{
						result = true;
						direction = DOWN;
					}
					if((collisiontile->edges_.LEFT == true) && (this->position().x < collisiontile->position().x - (collisiontile->width() / 2.0f)))
					{
						result = true;
						direction = RIGHT;
					}
					if((collisiontile->edges_.RIGHT == true) && (this->position().x > collisiontile->position().x + (collisiontile->width() / 2.0f)))
					{
						result = true;
						direction = LEFT;
					}
					if(!result)
					{
						// colliding with a tile edge we shouldn't be, so just kill player to restart
						this->dead = true;
						//std::cout << "Collision tile orientation error." << std::endl;
					}
				//}
				setGravity(direction);
				break;
			}
		case CollisionTile::DIAGONAL:
			// all diagonals are currently harmful, will never reach here
			break;
		}
	}
}

void Player::setGravity(Direction direction)
{
	switch (direction)
	{
		case DOWN:
			gravity = b2Vec2(0.0f, -10.0f);
			break;
		case RIGHT:
			gravity = b2Vec2(10.0f, 0.0f);
			break;
		case UP:
			gravity = b2Vec2(0.0f, 10.0f);
			break;
		case LEFT:
			gravity = b2Vec2(-10.0f, 0.0f);
			break;
	}
	gDir = direction;
}

//John
void Player::changeState()
{
	if (flying_)
	{
		if (attacking == true)
		{
			state_ = ATTACKING;
		}
		else if (dead == true && attacking == false)
		{
			state_ = DEAD;
			AccelerateTo(abfw::Vector2(0.0f,0.0f));
		}
		else
		{
			state_ = FLYING;
		}
	}
	else
	{
		if (dead == true)
		{
			state_ = DEAD;
			AccelerateTo(abfw::Vector2(0.0f,0.0f));
		}
		else if (attacking == true && state_ != INAIR && state_ != JUMPING)//no mid air attack
		{
			state_ = ATTACKING;
		}
		else if (move == true && dead == false && state_ != INAIR && state_ != JUMPING && state_ != ATTACKING)//only run when on a surface
		{
			state_ = RUNNING;
		}
		else if (state_ != INAIR && state_ != JUMPING && attacking != true)//state stays as INAIR until a surface is touched
		{
			state_ = IDLE;
		}
	}
}

//John
void Player::runningAnimation()
{
	//flip for gravity
	if (gDir == UP)
	{
		set_uv_height(-0.25f);
	}
	else if (gDir == DOWN)
	{
		set_uv_height(0.25f);
	}
	else if (gDir == RIGHT)
	{
		set_uv_width(0.25f);
	}
	else if (gDir == LEFT)
	{
		set_uv_width(-0.25f);
	}
	
	if(horizontal == true)//horizontal animation
	{
		if(gDir == DOWN)
		{
			if(move_right == true)
			{
				set_uv_width(0.0625);
				set_uv_position(abfw::Vector2(0.0f,0.5f));
			}

			if(move_right == false)
			{
				set_uv_width(-0.0625);
				set_uv_position(abfw::Vector2(0.0625f,0.5f));
			}
		}
		else if(gDir == UP)
		{
			if(move_right == true)
			{
				set_uv_width(0.0625);
				set_uv_position(abfw::Vector2(0.0f,0.75f));
			}

			if(move_right == false)
			{
				set_uv_width(-0.0625);
				set_uv_position(abfw::Vector2(0.0625f,0.75f));
			}
		}

				
		//set up animation
		InitSpriteAnimation(0.0125,16,true,SCROLL_X,16,2);
	}
	else if (horizontal != true)//vertical animation
	{
		if(gDir == LEFT)
		{
			if(moveUp == true)
			{
				set_uv_height(0.0625);	
				set_uv_position(abfw::Vector2(0.75f,0.9375f));
			}

			if(moveUp == false)
			{
				set_uv_height(-0.0625);	
				set_uv_position(abfw::Vector2(0.75f,0.9375f));
			}
		}
		else if(gDir == RIGHT)
		{
			if(moveUp == true)
			{
				set_uv_height(0.0625);		
				set_uv_position(abfw::Vector2(0.5f,0.9375f));
			}

			if(moveUp == false)
			{
				set_uv_height(-0.0625);	
				set_uv_position(abfw::Vector2(0.5f,0.9375f));
			}
		}
				
		//set up animation
		InitSpriteAnimation(0.0125,16,true,SCROLL_Y,16,2);
	}	
}

//John
void Player::idleAnimation()
{
	//flip for gravity
	if (gDir == UP)
	{
		set_uv_height(-0.5f);
	}
	else if (gDir == DOWN)
	{
		set_uv_height(0.5f);
	}
	else if (gDir == RIGHT)
	{
		set_uv_width(0.5f);
	}
	else if (gDir == LEFT)
	{
		set_uv_width(-0.5f);
	}

	//orientate sprite with direction
	if (horizontal == true)
	{
		if(gDir == DOWN)
		{
			if (move_right == true)
			{
				set_uv_width(0.03125f);
				set_uv_position(abfw::Vector2 (0.0f,0.0f));
			}
			else
			{
				set_uv_width(-0.03125f);
				set_uv_position(abfw::Vector2 (0.03125f,0.0f));
			}
		}
		else if (gDir == UP)
		{
			if (move_right == true)
			{
				set_uv_width(0.03125f);
				set_uv_position(abfw::Vector2 (0.0f,0.5f));
			}
			else
			{
				set_uv_width(-0.03125f);
				set_uv_position(abfw::Vector2 (0.03125f,0.5f));
			}
		}

		//set up animation
		InitSpriteAnimation(0.02,64,true,SCROLL_XY,32,2);
				
	}
	else
	{
		if(gDir == LEFT)
		{
			if (moveUp == true)
			{
				set_uv_height(0.03125);
				set_uv_position(abfw::Vector2(0.0f,0.96875f));
			}
			else
			{
				set_uv_height(-0.03125f);
				set_uv_position(abfw::Vector2(0.0f,1.0f));
			}
		}
		else if (gDir == RIGHT)
		{
			if (moveUp == true)
			{
				set_uv_height(0.03125f);
				set_uv_position(abfw::Vector2(0.0f,0.96875f));
			}
			else
			{
				set_uv_height(-0.03125f);
				set_uv_position(abfw::Vector2(0.0f,1.0f));
			}
		}

		//set up animation
		InitSpriteAnimation(0.02,64,true,SCROLL_YX,2,32);
	}
}

//John
void Player::deadAnimation()
{
	//flip for gravity
	if (gDir == UP)
	{
		set_uv_height(-1);
	}
	else if (gDir == DOWN)
	{
		set_uv_height(1);
	}
	else if (gDir == RIGHT)
	{
		set_uv_width(1);
	}
	else if (gDir == LEFT)
	{
		set_uv_width(-1);
	}
		
	//orientate sprite with direction
	if (horizontal == true)
	{
		if(gDir == DOWN)
		{
			if (move_right == true)
			{
				set_uv_width(0.0625);
				set_uv_position(abfw::Vector2 (0.0f,0.0f));
			}
			else
			{
				set_uv_width(-0.0625);
				set_uv_position(abfw::Vector2 (0.0625,0.0f));
			}
		}
		else if (gDir == UP)
		{
			if (move_right == true)
			{
				set_uv_width(0.0625);
				set_uv_position(abfw::Vector2 (0.0f,1));
			}
			else
			{
				set_uv_width(-0.0625);
				set_uv_position(abfw::Vector2 (0.0625,1));
			}
		}				

		//set up animation
		InitSpriteAnimation(0.07,16,false,SCROLL_X,0,0);
				
	}
	else
	{
		if(gDir == LEFT)
		{
			if (moveUp == true)
			{
				set_uv_height(0.0625);
				set_uv_position(abfw::Vector2(1.0f,1.0f));
			}
			else
			{
				set_uv_height(-0.0625);
				set_uv_position(abfw::Vector2(1.0f,1.0f));
			}
		}
		else if (gDir == RIGHT)
		{
			if (moveUp == true)
			{
				set_uv_height(0.0625);
				set_uv_position(abfw::Vector2(0.0f,0.9375f));
			}
			else
			{
				set_uv_height(-0.0625);
				set_uv_position(abfw::Vector2(0.0f,1.0f));
			}
		}

		//set up animation
		InitSpriteAnimation(0.07,16,false,SCROLL_Y,0,0);
	}
}

//John
void Player::jumpAnimation(float xAxis, float yAxis)
{
	//flip for gravity
	if (gDir == UP)
	{
		set_uv_height(-1);
	}
	else if (gDir == DOWN)
	{
		set_uv_height(1);
	}
	else if (gDir == RIGHT)
	{
		set_uv_width(1);
	}
	else if (gDir == LEFT)
	{
		set_uv_width(-1);
	}
		
	//orientate sprite with direction
	if (horizontal == true)
	{
		//needs to be set from stick 	
		if(xAxis > 0)
		{
			move_right = true;
		}
		else if(xAxis < 0)
		{
			move_right = false;
		}

		if(gDir == DOWN)
		{
			if (move_right == true)
			{
				set_uv_width(0.0625);
				set_uv_position(abfw::Vector2 (0.0f,0.0f));
			}
			else
			{
				set_uv_width(-0.0625);
				set_uv_position(abfw::Vector2 (0.0625,0.0f));
			}
		}
		else if (gDir == UP)
		{
			if (move_right == true)
			{
				set_uv_width(0.0625);
				set_uv_position(abfw::Vector2 (0.0f,1));
			}
			else
			{
				set_uv_width(-0.0625);
				set_uv_position(abfw::Vector2 (0.0625,1));
			}
		}

		//set up animation
		InitSpriteAnimation(0.005,16,false,SCROLL_X,0,0);
				
	}
	else
	{
		//needs to be set from stick 	
		if(yAxis > 0)
		{
			moveUp = false;
		}
		else if(yAxis < 0)
		{
			moveUp = true;
		}

		if(gDir == LEFT)
		{
			if (moveUp == true)
			{
				set_uv_height(0.0625);
				set_uv_position(abfw::Vector2(0.0f,0.0625));
			}
			else
			{
				set_uv_height(-0.0625);
				set_uv_position(abfw::Vector2(0.0f,1.0f));
			}
		}
		else if (gDir == RIGHT)
		{
			if (moveUp == true)
			{
				set_uv_height(0.0625);
				set_uv_position(abfw::Vector2(0.0f,0.0625));
			}
			else
			{
				set_uv_height(-0.0625);
				set_uv_position(abfw::Vector2(0.0f,1.0f));
			}
		}

		//set up animation
		InitSpriteAnimation(0.005,16,false,SCROLL_Y,0,0);
	}
}

//John
void Player::attackAnimation()
{
	//attack sprite is bigger so must be resized to match other sprites
	set_width(BOX2D_GFX_SIZE(2.5 * body_half_width));
	set_height(BOX2D_GFX_SIZE(2.15 * body_half_height));
	
	if(flying_ == false)
	{
		//flip for gravity
		if (gDir == UP)
		{
			set_uv_height(-1);
		}
		else if (gDir == DOWN)
		{
			set_uv_height(1);
		}
		else if (gDir == RIGHT)
		{
			set_uv_width(-1);
		}
		else if (gDir == LEFT)
		{
			set_uv_width(1);
		}
	}
		
	//orientate sprite with direction

	if (horizontal == true)
	{
		if(gDir == DOWN)
		{
			if (move_right == true)
			{
				set_uv_width(0.0833);
				set_uv_position(abfw::Vector2 (0.0f,0.0f));
			}
			else
			{
				set_uv_width(-0.0833);
				set_uv_position(abfw::Vector2 (0.0833,0.0f));
			}
		}
		else if (gDir == UP)
		{
			if (move_right == true)
			{
				set_uv_width(0.0833);
				set_uv_position(abfw::Vector2 (0.0f,1));
			}
			else
			{
				set_uv_width(-0.0833);
				set_uv_position(abfw::Vector2 (0.0833,1));				
			}
		}				

		//set up animation
		InitSpriteAnimation(0.07,12,false,SCROLL_X,0,0);
				
	}
	else
	{
		if(gDir == LEFT)
		{
			if (moveUp == true)
			{
				set_uv_height(0.0833);
				set_uv_position(abfw::Vector2(1.0f,0.9277f));
			}
			else
			{
				set_uv_height(-0.0833);
				set_uv_position(abfw::Vector2(1.0f,1.0f));
			}
		}
		else if (gDir == RIGHT)
		{
			if (moveUp == true)
			{
				set_uv_height(0.0833);
				set_uv_position(abfw::Vector2(0.0f,0.9277f));
			}
			else
			{
				set_uv_height(-0.0833);
				set_uv_position(abfw::Vector2(0.0f,1.0f));
			}
		}

		//set up animation
		InitSpriteAnimation(0.07,12,false,SCROLL_Y,0,0);
	}
}

//John
void Player::flyAnimation()
{
	if (move_right == true)
	{
		set_uv_width(0.0625f);
		set_uv_position(abfw::Vector2 (0.0f,0.0f));
	}
	else
	{
		set_uv_width(-0.0625f);
		set_uv_position(abfw::Vector2 (0.0625f,0.0f));
	}

	//set up animation
	InitSpriteAnimation(0.0125,16,true,SCROLL_X,16,2);
}
