//////////////////////////
// StaticObject.h
// defines a basic class that can be used to draw and collide with models that do not move,
// such as trees, tree stumps, bushes, huts, and bridges
////////////////////////

#ifndef STATIC_OBJECT_H
#define STATIC_OBJECT_H

#include "base.h"

class StaticObject : public Base
{
public:

	StaticObject() : Base (BASETYPE_STATIC_OBJECT){}

	void ResetBoundingShapeColor()
	{
		pBoundingShape->SetColor (0,1,0);
	}

#if _DEBUG
	ManageMemory (Tree, BASETYPE_STATIC_OBJECT);
#endif
};

#endif