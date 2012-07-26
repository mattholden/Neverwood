#include "OrcScout.h"

DXModel* OrcScout::psModel = 0;

// STATIC FUNCTIONS
	void OrcScout::LoadModel (char* strFile)
	{
		DeleteModel();
		psModel = new DXModel;
		psModel->Load (strFile);
	}

	void OrcScout::ScaleModel (float fScale)
	{
		psModel->Scale (fScale);
	}

	void OrcScout::DeleteModel()
	{
		if (psModel)
		{
			psModel->Release();
			psModel=0;
		}
	}

// NON-STATIC
	void OrcScout::ResetBoundingShapeColor(void)
	{
		pBoundingShape->SetColor(0.2f, 0.6f, 0.2f);
	}

	/*
	void OrcScout::Draw()
	{
		if (RContext->bDraw && pModel)
		{
			Direct3D::GetMatrixStack()->Push();
			Direct3D::GetMatrixStack()->LoadMatrixClass(mGlobal);
		
			// when we load the ranger, its center is on the ground. However, we want the object to be centered around the middle.
			Direct3D::GetMatrixStack()->TranslateLocal (0, -fHeight*0.5f, 0);
		
			Direct3D::GetMatrixStack()->ApplyToWorld();
			OrcScout::psModel->Draw();
			Direct3D::GetMatrixStack()->Pop();
		}
	}
	*/

	void OrcScout::Update() 
	{ 
		if (GetAsyncKeyState ('X'))
			int blah=0;
		Humanoid::Update(); 
	}