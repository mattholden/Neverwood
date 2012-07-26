// Defines a buzzard

#ifndef StormForge_Shared_Buzzard
#define StormForge_Shared_Buzzard

#include "Bird.h"
#include "MemoryMgr.h"

class Buzzard : public Bird
{
public:
	Buzzard(void) : Bird(BASETYPE_BUZZARD) 
	{
		ResetBoundingShapeColor();		
	}

	#if _DEBUG
		ManageMemory(Buzzard, BASETYPE_BUZZARD);
	#endif

	void ResetBoundingShapeColor(void)
	{
		pBoundingShape->SetColor(0.6f,0.6f,0.0f);
	}

};

#endif
