// Event Trigger
// an event trigger is an area of the world that calls a function if something collides with it
// an example of an event trigger is the endpoint of a level

#ifndef EVENTTRIGGERS_H
#define EVENTTRIGGERS_H

#include "Base.h"
#include "Identifiers.h"
#include "VerticalCylinder.h"

class EventTrigger: public Base
{
public:

	EventTrigger() : Base (BASETYPE_EVENT_TRIGGER)
	{
		//pBoundingShape->SetColor (1,0,1);
		OnActivate = NULL;
	}
	// OnActivate
	// when something activates the event trigger by colliding with it, the event trigger does its thing by calling this function
	void (*OnActivate) (void);
	
#if _DEBUG
	ManageMemory (EventTrigger, BASETYPE_EVENT_TRIGGER);
#endif
};

#endif