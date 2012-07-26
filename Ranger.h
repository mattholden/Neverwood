
// Defines Tempest.

#ifndef StormForge_Shared_Ranger
#define StormForge_Shared_Ranger

#include "Humanoid.h"
#include "MemoryMgr.h"

class Ranger : public Humanoid
{
public:
	Ranger(int iPlayerFlag = CHARACTER_PLAYER1) : Humanoid(BASETYPE_RANGER, iPlayerFlag) 
	{
		iHitPoints = 100;

		ModelLoad("models\\arch.x");
		ModelScale(0.1f);
		float fRadius = fWidth * 0.3f;
		pBoundingShape = new VerticalCylinder (fHeight - fRadius, fRadius);
		//pBoundingShape = new Sphere (0.5f);
		ResetBoundingShapeColor();	

		fJumpStrength = 12;
		fAttackDuration = 0.35f;
		fAttackRange = 50;

		RTF.Set (60, 0.1f, fAttackRange, 0.1f);
	}
	
	#if _DEBUG
		ManageMemory(Ranger, BASETYPE_RANGER);
	#endif

	void ResetBoundingShapeColor(void)
	{
		pBoundingShape->SetColor (1,.75f, .75f);
	}

	void Draw()
	{
		if (RContext->bDraw && pModel)
		{
			Direct3D::GetMatrixStack()->Push();
			Direct3D::GetMatrixStack()->LoadMatrixClass(mGlobal);
		
			// when we load the ranger, its center is on the ground. However, we want the object to be centered around the middle.
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
