
// Defines an Orcish Lieutenant

#ifndef StormForge_Shared_OrcLt
#define StormForge_Shared_OrcLt

#include "Humanoid.h"
#include "MemoryMgr.h"

class OrcLt : public Humanoid
{
public:
	OrcLt(void) : Humanoid(BASETYPE_ORCLT) 
	{
		ModelLoad ("models\\orc_static.x");
		fRed = 0.4f;
		fGreen = 0;
		fBlue = 0;
		pModel->SetMaterialColor (fRed, fGreen, fBlue);
		float fBoundingShapeRadius = fWidth * 0.3f;
		pBoundingShape = new VerticalCylinder (fHeight - fBoundingShapeRadius*1.5f, fBoundingShapeRadius); 
		ResetBoundingShapeColor();

		viewFrustum.Set (40.0f, 0.1f, 20.0f, 0.25f);

		bWeaponList[WEAPON_SWORD]=true;
		iSelectedWeapon=WEAPON_SWORD;

		fMaxSpd = 4.0f;

		fAttackProb = 0.3f;
		fAttackDuration = 1.0f;
		fAttackRange = 5;
		iHitPoints = 100;
		fBlockProb = 1.0f;
		fMinBlockDuration = 1.0f;

		RTF.Set (2, 0.1f, fAttackRange, 10);
	}

	#if _DEBUG
		ManageMemory(OrcLt, BASETYPE_ORCLT);
	#endif

	void ResetBoundingShapeColor(void)
	{
		pBoundingShape->SetColor(1,0,0);
	}

	void Draw()
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

	void Update() 
	{ 
		Humanoid::Update(); 
	}
};

#endif
