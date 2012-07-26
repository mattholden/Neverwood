#ifndef TREE_H
#define TREE_H

#include "base.h"

class Tree : public Base
{
public:

	Tree() : Base (BASETYPE_TREE){}

	void ResetBoundingShapeColor()
	{
		pBoundingShape->SetColor (0,1,0);
	}

#if _DEBUG
	ManageMemory (Tree, BASETYPE_TREE);
#endif
};

#endif