
// Defines Aurexis

#ifndef StormForge_Shared_Falcon
#define StormForge_Shared_Falcon

#include "Bird.h"
#include "MemoryMgr.h"

class Falcon : public Bird
{
public:

	Falcon (int iPlayerFlag = CHARACTER_AI_PLAYER2) : Bird(BASETYPE_FALCON, iPlayerFlag) 
	{
		ResetBoundingShapeColor(); 
		ModelLoad("models\\falcon.x");
		ModelScale(0.15f);

		pBoundingShape = new Sphere(.3f);
		pBoundingShape->SetColor (0,.0, 1);
	}

	#if _DEBUG
		ManageMemory(Falcon, BASETYPE_FALCON);
	#endif

	void ResetBoundingShapeColor(void)
	{
		pBoundingShape->SetColor(0.6f,0.0f,0.2f);
	}

	
	void Draw(void)
	{
		if (RContext->bDraw && pModel)
		{

		Direct3D::GetMatrixStack()->Push();
		Direct3D::GetMatrixStack()->LoadMatrixClass(mGlobal);
		Direct3D::GetMatrixStack()->TranslateLocal (0, -fHeight*0.5f, 0);
		Direct3D::GetMatrixStack()->RotateAxisLocal(&D3DXVECTOR3(0, 1, 0), 180.0f * MATH_TRIG_DEGREES_TO_RADIANS);
		Direct3D::GetMatrixStack()->ApplyToWorld();

		pModel->Draw();

		Direct3D::GetMatrixStack()->Pop();
		}

	}
	
};

#endif
