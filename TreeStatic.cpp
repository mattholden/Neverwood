#include "Tree.h"

DXModel Tree::pTreeModels[TREEMODELS];
int Tree::iTreeModels = TREEMODELS;

	void Tree::MoveTo (Vector& vPoint)
	{
		mGlobal.SetPos (vPoint);
		mDraw.SetPos (vPoint + Vector (0, fHeight * 0.5f, 0));
	}
	void Tree::MoveTo (float x, float y, float z) { MoveTo (Vector (x,y,z)); }

	void Tree::SetModel (int iModelIdx)
	{
		iModel = iModelIdx;
		pModel = &pTreeModels[iModel];
		fWidth = ModelGetWidth();
		fHeight = ModelGetHeight();
		fDepth = ModelGetDepth();
	}

	void Tree::ResetBoundingShapeColor(void)
	{
		pBoundingShape->SetColor(0, 1, 0);
	}
	
	void Tree::Draw()
	{
		Direct3D::GetMatrixStack()->Push();
		Direct3D::GetMatrixStack()->LoadMatrixClass (mGlobal);
		Direct3D::GetMatrixStack()->ApplyToWorld();
		Tree::pTreeModels[iModel].Draw();
		Direct3D::GetMatrixStack()->Pop();
		//Direct3D::Wireframe (false);
		//TextureMgr::Set (0);
		//pBoundingShape->Draw();
	}

	void Tree::LoadModel (int iModelNum, char* strFile)
	{
		pTreeModels [iModelNum].Load (strFile);
		pTreeModels [iModelNum].Scale (0.1f);
	}

	void Tree::ScaleModel (int iModelNum, float fScale)
	{
		pTreeModels [iModelNum].Scale (fScale);
	}

	void Tree::DeleteModel (int iModelNum)
	{
		pTreeModels[iModelNum].Delete();
	}