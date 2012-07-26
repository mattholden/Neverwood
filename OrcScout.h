
// Defines an Orc Scout.

#ifndef StormForge_Shared_OrcScout
#define StormForge_Shared_OrcScout

#include "Humanoid.h"
#include "MemoryMgr.h"

class OrcScout : public Humanoid
{
public:
	OrcScout(void) : Humanoid(BASETYPE_ORCSCOUT) 
	{
		ModelLoad ("models\\orc_static.x");
		ModelScale (0.5f, 0.75f, 0.5f);
		fRed = 0;
		fGreen = 0.7f;
		fBlue = 0;
		pModel->SetMaterialColor(fRed, fGreen, fBlue);

		float fBoundingShapeRadius = fWidth * 0.3f;
		pBoundingShape = new VerticalCylinder (fHeight - fBoundingShapeRadius*1.5f, fBoundingShapeRadius); 
		ResetBoundingShapeColor();

		viewFrustum.Set (15.0f, 0.1f, 30.0f, 1.0f);

		bWeaponList[WEAPON_BOW]=true;
		iSelectedWeapon=WEAPON_BOW;

		fMaxSpd = 3.0f;

		fAttackProb = 0.25f;
		fAttackDuration = 0.9f;
		fAttackRange = 20;

		RTF.Set (60, 0.1f, fAttackRange, 0.1f);
	}

	#if _DEBUG
		ManageMemory(OrcScout, BASETYPE_ORCSCOUT);
	#endif

	void ResetBoundingShapeColor(void)
	{
		pBoundingShape->SetColor(0.2f, 0.6f, 0.2f);
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
