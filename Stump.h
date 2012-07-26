#ifndef STUMP_H
#define STUMP_H

#include "base.h"

class Stump : public Base
{
public:

	Stump() : Base (BASETYPE_STUMP){}

	void ResetBoundingShapeColor()
	{
		pBoundingShape->SetColor (0,1,0);
	}

#if _DEBUG
	ManageMemory (Tree, BASETYPE_STUMP);
#endif
};

#endif