#include "Contact_listener.h"
#include "game_object.h"
#include "player.h"
#include "platforms.h"
#include "enemy.h"
#include "collectible.h"
#include "plant.h"
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
		if (game_object->type() == GameObject::PLAYER && game_object_b->type() == GameObject::ENEMY && ((Player *)game_object)->attacking == false)
		{
			((Player *)game_object)->hurting = true;
			//((Player *)game_object)->Knockback(((Player *)game_object)->body_->GetPosition(),((Enemy *)game_object_b)->body_->GetPosition());
		}
		else if (game_object->type() == GameObject::ENEMY && game_object_b->type() == GameObject::PLAYER&& ((Player *)game_object_b)->attacking == false)
		{
			((Player *)game_object_b)->hurting = true;
			//((Player *)game_object_b)->Knockback(((Player *)game_object_b)->body_->GetPosition(), ((Enemy *)game_object)->body_->GetPosition());
		}

		//attack enemies
		if (game_object->type() == GameObject::BLADE && game_object_b->type() == GameObject::ENEMY)
		{
			//((Enemy *)game_object_b)->Knockback(((Enemy *)game_object)->body->GetPosition(), ((Blade *)game_object_b)->body->GetPosition());
			((Enemy* )game_object_b)->dead = true;
		}
		else if (game_object->type() == GameObject::ENEMY && game_object_b->type() == GameObject::BLADE)
		{
			//((Enemy *)game_object_b)->Knockback(((Enemy *)game_object_b)->body->GetPosition(), ((Blade *)game_object)->body->GetPosition());
			((Enemy* )game_object)->dead = true;
		}

		//pickup points
		if (game_object->type() == GameObject::PLAYER && game_object_b->type() == GameObject::PICKUP)
		{
			((PickUp *)game_object_b)->dead = true;
		}

		if (game_object->type() == GameObject::PICKUP && game_object_b->type() == GameObject::PLAYER)
		{
			((PickUp *)game_object)->dead = true;
		}


		//kill plants
		if (game_object->type() == GameObject::BLADE && game_object_b->type() == GameObject::PLANT)
		{
			((Plant* )game_object_b)->dead = true;
		}
		else if (game_object->type() == GameObject::PLANT && game_object_b->type() == GameObject::BLADE)
		{
			((Plant* )game_object)->dead = true;
		}

		//player death on spikes
		if (game_object->type() == GameObject::PLAYER && game_object_b->type() == GameObject::SPIKE)
		{
			((Player* )game_object)->dead = true;
		}
		else if (game_object->type() == GameObject::SPIKE && game_object_b->type() == GameObject::PLAYER)
		{
			((Player* )game_object_b)->dead = true;
		}

		//check player is touching platform
		if (game_object->type() == GameObject::PLAYER && game_object_b->type() == GameObject::PLATFORM)
		{
			((Player *)game_object)->grounded = true;
		}
		else if (game_object->type() == GameObject::PLATFORM && game_object_b->type() == GameObject::PLAYER)
		{
			((Player *)game_object_b)->grounded = true;
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
		if (game_object->type() == GameObject::PLAYER && game_object_b->type() == GameObject::ENEMY)
		{
			((Player *)game_object)->hurting = false;
		}
		else if (game_object->type() == GameObject::ENEMY && game_object_b->type() == GameObject::PLAYER)
		{
			((Player *)game_object_b)->hurting = false;
		}

		//touching platform
		if (game_object->type() == GameObject::PLAYER && game_object_b->type() == GameObject::PLATFORM)
		{
			((Player *)game_object)->grounded = false;
		}
		else if (game_object->type() == GameObject::PLATFORM && game_object_b->type() == GameObject::PLAYER)
		{
			((Player *)game_object_b)->grounded = false;
		}
	}
}

