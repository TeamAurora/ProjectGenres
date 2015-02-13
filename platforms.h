#ifndef _PLATFORM_H
#define _PLATFROM_H

#include "game_object.h"

class Platform: public GameObject
{
	public:
		//functions
		Platform();//constructor
		void Create_platform(b2World* world_, float x , float y, float width, float height);//create platform at x,y position with this width and height
};
#endif