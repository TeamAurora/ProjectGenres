#pragma once
#include "game_object.h"

class PickUp : public GameObject
{
public:
	enum PICKUPTYPE { RED, BLUE, YELLOW, GREEN };
	PICKUPTYPE pickup_type_;
}