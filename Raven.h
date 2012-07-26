
// Defines Sul'talan

#ifndef StormForge_Shared_Raven
#define StormForge_Shared_Raven

#include "Bird.h"
#include "MemoryMgr.h"

class Raven : public Bird
{
public:
	Raven(void) : Bird(BASETYPE_RAVEN) 
	{
		ResetBoundingShapeColor();
	}

	#if _DEBUG
		ManageMemory(Raven, BASETYPE_RAVEN);
	#endif

	void ResetBoundingShapeColor(void)
	{
		pBoundingShape->SetColor(0.5f, 0.0f, 0.5f);
	}


};

#endif
