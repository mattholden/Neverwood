// Defines creates that fly

#ifndef StormForge_Shared_Bird
#define StormForge_Shared_Bird

#include "Character.h"
#include "MemoryMgr.h"
#include "sphere.h"

class Bird : public Character
{
public:
	Bird(int iType = BASETYPE_BIRD, int iPlayerFlag = CHARACTER_AI_ENEMY) : Character(iType, iPlayerFlag) 
	{
		bGroundClamped = false;
		pBoundingShape = new Sphere(0.25f);
		pBoundingShape->SetColor(1,1,1);
	}


	#if _DEBUG
		ManageMemory(Bird, BASETYPE_BIRD);
	#endif

		void HandleInput() { }

	void ResetBoundingShapeColor(void)	{}
};

#endif
