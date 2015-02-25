#include "Contact_listener.h"
#include "game_object.h"
#include "player.h"
#include "enemy.h"
#include "collectible.h"
#include "blade.h"

void Contact_Listener::BeginContact(b2Contact* contact)
{
	// get the colliding bodies
	b2Body* bodyA = contact->GetFixtureA()->GetBody();
	b2Body* bodyB = contact->GetFixtureB()->GetBody();

	// DO COLLISION RESPONSE HERE
	GameObject* game_object = (GameObject*)bodyA->GetUserData();
	GameObject* game_object_b = (GameObject*)bodyB->GetUserData();
		   
	if(game_object && game_object_b)//check user data is not null
	{
		//health decrease 
		if (game_object->getType() == GameObject::PLAYER && game_object_b->getType() == GameObject::ENEMY && ((Player *)game_object)->attacking == false)
		{
			((Player *)game_object)->hurting = true;
			((Player *)game_object)->Knockback(((Player *)game_object)->body_->GetPosition(),((Enemy *)game_object_b)->body_->GetPosition());
		}
		else if (game_object->getType() == GameObject::ENEMY && game_object_b->getType() == GameObject::PLAYER&& ((Player *)game_object_b)->attacking == false)
		{
			((Player *)game_object_b)->hurting = true;
			((Player *)game_object_b)->Knockback(((Player *)game_object_b)->body_->GetPosition(), ((Enemy *)game_object)->body_->GetPosition());
		}

		//attack enemies
		if (game_object->getType() == GameObject::BLADE && game_object_b->getType() == GameObject::ENEMY)
		{
			//((Enemy *)game_object_b)->Knockback(((Enemy *)game_object)->body_->GetPosition(), ((Blade *)game_object_b)->body_->GetPosition());
			((Enemy* )game_object_b)->dead = true;
		}
		else if (game_object->getType() == GameObject::ENEMY && game_object_b->getType() == GameObject::BLADE)
		{
			//((Enemy *)game_object_b)->Knockback(((Enemy *)game_object_b)->body_->GetPosition(), ((Blade *)game_object)->body_->GetPosition());
			((Enemy* )game_object)->dead = true;
		}

		//pickup points
		if (game_object->getType() == GameObject::PLAYER && game_object_b->getType() == GameObject::PICKUP)
		{
			((PickUp *)game_object_b)->dead = true;
		}

		if (game_object->getType() == GameObject::PICKUP && game_object_b->getType() == GameObject::PLAYER)
		{
			((PickUp *)game_object)->dead = true;
		}

		//stops the blade pushing the pickup away 
		if (game_object->getType() == GameObject::BLADE && game_object_b->getType() == GameObject::PICKUP)
		{
			((PickUp *)game_object_b)->dead = true;
		}

		if (game_object->getType() == GameObject::PICKUP && game_object_b->getType() == GameObject::BLADE)
		{
			((PickUp *)game_object)->dead = true;
		}


		//kill plants
		if (game_object->getType() == GameObject::BLADE && game_object_b->getType() == GameObject::PLANT)
		{
			((GameObject* )game_object_b)->dead = true;
		}
		else if (game_object->getType() == GameObject::PLANT && game_object_b->getType() == GameObject::BLADE)
		{
			((GameObject* )game_object)->dead = true;
		}

		//player death on spikes
		if (game_object->getType() == GameObject::PLAYER && game_object_b->getType() == GameObject::SPIKE)
		{
			((Player* )game_object)->dead = true;
		}
		else if (game_object->getType() == GameObject::SPIKE && game_object_b->getType() == GameObject::PLAYER)
		{
			((Player* )game_object_b)->dead = true;
		}

		//check player is touching platform
		if (game_object->getType() == GameObject::PLAYER && game_object_b->getType() == GameObject::PLATFORM)
		{

			if (game_object->position().y - game_object_b->position().y > 0)
			{
				((Player *)game_object)->Player_Gravity(2);
			}
			if (game_object->position().y - game_object_b->position().y < 0)
			{
				((Player *)game_object)->Player_Gravity(0);
			}
			((Player *)game_object)->grounded = true;
		}

			
		if (game_object->getType() == GameObject::PLATFORM && game_object_b->getType() == GameObject::PLAYER)
		{
			if (game_object_b->position().y - game_object->position().y > 0)
			{
				((Player *)game_object_b)->Player_Gravity(2);
			}

			if (game_object_b->position().y - game_object->position().y < 0)
			{
				((Player *)game_object_b)->Player_Gravity(0);
			}

			((Player *)game_object_b)->grounded = true;
		}
		
		if (game_object->getType() == GameObject::PLAYER && game_object_b->getType() == GameObject::PLATFORM)
		{
			if (game_object->position().x - game_object_b->position().x > 0)
			{ 
				((Player *)game_object)->Player_Gravity(3);
			}
			if (game_object->position().x - game_object_b->position().x < 0)
			{
				((Player *)game_object)->Player_Gravity(1);
			}

			((Player *)game_object)->grounded = true;

		}
		
		if (game_object->getType() == GameObject::PLATFORM && game_object_b->getType() == GameObject::PLAYER)
		{
			
			if (game_object_b->position().x - game_object->position().x > 0)
			{
				((Player *)game_object_b)->Player_Gravity(3);
			}

			if (game_object_b->position().x - game_object->position().x < 0)
			{
				((Player *)game_object_b)->Player_Gravity(1);
			}	

			((Player *)game_object)->grounded = true;
		}
	}
}

void Contact_Listener::EndContact(b2Contact* contact)
{
	// get the colliding bodies
	b2Body* bodyA = contact->GetFixtureA()->GetBody();
	b2Body* bodyB = contact->GetFixtureB()->GetBody();

	// DO COLLISION RESPONSE HERE
	GameObject* game_object = (GameObject*)bodyA->GetUserData();
	GameObject* game_object_b = (GameObject*)bodyB->GetUserData();
		   
	if(game_object && game_object_b)//check user data is not null
	{
		//health decrease 
		if (game_object->getType() == GameObject::PLAYER && game_object_b->getType() == GameObject::ENEMY)
		{
			((Player *)game_object)->hurting = false;
		}
		else if (game_object->getType() == GameObject::ENEMY && game_object_b->getType() == GameObject::PLAYER)
		{
			((Player *)game_object_b)->hurting = false;
		}

		//touching platform
		if (game_object->getType() == GameObject::PLAYER && game_object_b->getType() == GameObject::PLATFORM)
		{
			((Player *)game_object)->grounded = false;
		}
		else if (game_object->getType() == GameObject::PLATFORM && game_object_b->getType() == GameObject::PLAYER)
		{
			((Player *)game_object_b)->grounded = false;
		}
	}
}