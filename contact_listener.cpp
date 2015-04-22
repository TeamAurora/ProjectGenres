/////John//////////////
#include "Contact_listener.h"
#include "game_object.h"
#include "player.h"
#include "enemy.h"
#include "pickup.h"
#include "blade.h"
#include "bullet.h"

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
////////Combat//////////////
		//health decrease 
		if (game_object->getType() == GameObject::PLAYER && game_object_b->getType() == GameObject::ENEMY && ((Player *)game_object)->attacking == false)
		{
			((Player *)game_object)->hurting = true;
			((Player *)game_object)->Knockback(((Player *)game_object)->body_->GetPosition(),((Enemy *)game_object_b)->body_->GetPosition());
			//((Enemy* )game_object_b)->meleeState_ = Enemy::ATTACKING;
			((Enemy* )game_object_b)->attack_ = true;
		}
		else if (game_object->getType() == GameObject::ENEMY && game_object_b->getType() == GameObject::PLAYER && ((Player *)game_object_b)->attacking == false)
		{
			((Player *)game_object_b)->hurting = true;			
			((Player *)game_object_b)->Knockback(((Player *)game_object_b)->body_->GetPosition(), ((Enemy *)game_object)->body_->GetPosition());
			((Enemy* )game_object)->attack_ = true;
			//((Enemy* )game_object)->meleeState_ = Enemy::ATTACKING;
		}

		//attack enemies
		if (game_object->getType() == GameObject::BLADE && game_object_b->getType() == GameObject::ENEMY)
		{
			((Enemy* )game_object_b)->dead = true;
		}
		else if (game_object->getType() == GameObject::ENEMY && game_object_b->getType() == GameObject::BLADE)
		{
			((Enemy* )game_object)->dead = true;
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

//////////Pick Ups/////////////////////////
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

///////////Platform check///// Craig
		if (game_object->getType() == GameObject::PLAYER && game_object_b->getType() == GameObject::COLLISIONTILE)
		{
			static_cast<Player*>(game_object)->ResolveCollisionTile(static_cast<CollisionTile*>(game_object_b));
		}

		if (game_object->getType() == GameObject::COLLISIONTILE && game_object_b->getType() == GameObject::PLAYER)
		{
			static_cast<Player*>(game_object_b)->ResolveCollisionTile(static_cast<CollisionTile*>(game_object));
		}
	
//////////Bullets///////////////////////
		//destroy bullets if it hits platfrom
		if (game_object->getType() == GameObject::BULLET && game_object_b->getType() == GameObject::COLLISIONTILE)
		{
			((Bullet* )game_object)->dead = true;
		}
		else if (game_object->getType() == GameObject::COLLISIONTILE && game_object_b->getType() == GameObject::BULLET)
		{
			((Bullet* )game_object_b)->dead = true;
		}

		//destroy bullets if it hits collectible
		if (game_object->getType() == GameObject::BULLET && game_object_b->getType() == GameObject::PICKUP)
		{
			((Bullet* )game_object)->dead = true;
		}
		else if (game_object->getType() == GameObject::PICKUP && game_object_b->getType() == GameObject::BULLET)
		{
			((Bullet* )game_object_b)->dead = true;
		}

		//destroy bullets if it hits spikes
		if (game_object->getType() == GameObject::BULLET && game_object_b->getType() == GameObject::SPIKE)
		{
			((Bullet* )game_object)->dead = true;
		}
		else if (game_object->getType() == GameObject::SPIKE && game_object_b->getType() == GameObject::BULLET)
		{
			((Bullet* )game_object_b)->dead = true;
		}

		//kill player and destroy bullet
		if (game_object->getType() == GameObject::BULLET && game_object_b->getType() == GameObject::PLAYER)
		{
			((Bullet* )game_object)->dead = true;
			((Player* )game_object_b)->dead = true;
		}
		else if (game_object->getType() == GameObject::PLAYER && game_object_b->getType() == GameObject::BULLET)
		{
			((Bullet* )game_object_b)->dead = true;
			((Player* )game_object)->dead = true;
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
	}
}

