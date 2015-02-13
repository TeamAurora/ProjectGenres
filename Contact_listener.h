#ifndef _CONTACT_LISTENER_H
#define _CONTACT_LISTENER_H

#include <Box2D\Box2D.h>
#include <iostream>

class Contact_Listener : public b2ContactListener
{
	public:
		 void BeginContact(b2Contact* contact);//response at start of contact
		 void EndContact(b2Contact* contact);//response at end of contact
};

#endif