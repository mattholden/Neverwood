#ifndef HUT_H
#define HUT_H

#include "character.h"

class Hut : public Base
{
public:

	int iHitPoints, iMaxHitPoints;

	// data variables
	// a hut is basically a static object with HP
	Hut() : Base(BASETYPE_HUT)
	{
		ModelLoad ("models\\hut.x");
		ModelScale (0.25f);
		float fRadius = fWidth * 0.55f;
		pBoundingShape = new VerticalCylinder (fHeight, fRadius);

		iHitPoints = iMaxHitPoints = 50;

		bTakesDamage = true;
	}

	virtual void Update()
	{
		float fGray = float (iHitPoints) / iMaxHitPoints;
		pModel->SetMaterialColor (fGray, fGray, fGray);
	}

	void ResetBoundingShapeColor()
	{
		pBoundingShape->SetColor (0,1,0);
	}

	void TakeDamage (int iDamage)
	{
		iHitPoints -= iDamage;
		if (iHitPoints < 0)
		{
			RContext->bDraw = false;
			bCollidable = false;
		}
	}


	// just to be on the safe side, make sure we don't burn any CPU cycle

#if _DEBUG
	ManageMemory (Hut, BASETYPE_HUT);
#endif
};

#endif