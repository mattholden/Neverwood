#ifndef Bridge_H
#define Bridge_H

#include "base.h"

class Bridge : public Base
{
public:

	Bridge() : Base (BASETYPE_BRIDGE)
	{
		ModelLoad ("models\\bridge.x");
		ModelScale (0.25f);
		//pBoundingShape = new Sphere (fDepth * 0.5f);
	}

	void ResetBoundingShapeColor()
	{
		pBoundingShape->SetColor (0,1,0);
	}

#if _DEBUG
	ManageMemory (Tree, BASETYPE_BRIDGE);
#endif
};

#endif