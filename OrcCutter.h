
// Defines an Orcish Woodcutter

#ifndef StormForge_Shared_OrcCutter
#define StormForge_Shared_OrcCutter

#include "Humanoid.h"
#include "MemoryMgr.h"

class OrcCutter : public Humanoid
{
public:
	OrcCutter(void) : Humanoid(BASETYPE_ORCCUTTER) 
	{
		ModelLoad ("models\\orc_static.x");
		ModelScale (0.75f);
		float fBoundingShapeRadius = fWidth * 0.3f;
		pBoundingShape = new VerticalCylinder (fHeight - fBoundingShapeRadius*1.5f, fBoundingShapeRadius); 
		ResetBoundingShapeColor();

		viewFrustum.Set (10.0f, 0.1f, 20.0f, 1.0f);

		bWeaponList[WEAPON_SWORD]=true;
		iSelectedWeapon=WEAPON_SWORD;

		fMaxSpd = 4.0f;

		fAttackProb = 0.35f;
		fAttackDuration = 0.6f;
		fAttackRange = 3;

		RTF.Set (2, 0.1f, fAttackRange, 10);
	}

	#if _DEBUG
		ManageMemory(OrcCutter, BASETYPE_ORCCUTTER);
	#endif

	void ResetBoundingShapeColor(void)
	{
		pBoundingShape->SetColor(0,1,1);
	}

	virtual void Draw()
	{
		if (RContext->bDraw && pModel)
		{
			Direct3D::GetMatrixStack()->Push();
			Direct3D::GetMatrixStack()->LoadMatrixClass(mGlobal);
		
			// when we load the model, its center is on the ground. However, we want the object to be centered around the middle.
			Direct3D::GetMatrixStack()->TranslateLocal (0, -fHeight*0.5f, 0);		
			Direct3D::GetMatrixStack()->ApplyToWorld();
				pModel->Draw();
			Direct3D::GetMatrixStack()->Pop();
		}	
	}

};

#endif
