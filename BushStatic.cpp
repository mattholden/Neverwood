#include "Bush.h"

DXModel Bush::pBushModels[BUSHMODELS];
int Bush::iBushModels = BUSHMODELS;

	void Bush::MoveTo (Vector& vPoint)
	{
		mGlobal.SetPos (vPoint);
		mDraw.SetPos (vPoint + Vector (0, fHeight * 0.5f, 0));
	}
	void Bush::MoveTo (float x, float y, float z) { MoveTo (Vector (x,y,z)); }

	void Bush::SetModel (int iModelIdx)
	{
		iModel = iModelIdx;
		pModel = &pBushModels[iModel];
		fWidth = ModelGetWidth();
		fHeight = ModelGetHeight();
		fDepth = ModelGetDepth();
	}

	void Bush::ResetBoundingShapeColor(void)
	{
		pBoundingShape->SetColor(0, 1, 0);
	}
	
	void Bush::Draw()
	{
		Direct3D::GetMatrixStack()->Push();
		Direct3D::GetMatrixStack()->LoadMatrixClass (mDraw);
		Bush::pBushModels[iModel].Draw();
		Direct3D::GetMatrixStack()->Pop();
	}

	void Bush::LoadModel (int iModelNum, char* strFilename)
	{
		pBushModels[iModelNum].Load (strFilename);
	}

	void Bush::DeleteModel (int iModelNum)
	{
		pBushModels[iModelNum].Delete();
	}
