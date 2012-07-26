
// Define all the "objects" in our world that are to be treated as static geometry; i.e. not go anywhere.

#ifndef StormForge_Matt_StaticObjects
#define StormForge_Matt_StaticObjects

#include "base.h"
#include "memorymgr.h"
//#include "AllShapes.h"
#include "Sphere.h"
#include "Cylinder.h"
#include "Dxmodel.h"

// trees and bushes share static models
// for the sake of variety, trees and bushes have more than one static model associated with them
#define TREEMODELS 3
#define BUSHMODELS 3


// TREE
// essentially, a tree is a static model whose mesh can be one of several statically stored models
// by default, a tree has a random model associated with it
// alternately, you can create a tree using a specific model by passing a model number into the constructor, or by calling the SetModel function
class Tree : public Base
{

private:
	static DXModel pTreeModels[TREEMODELS];
	int iModel;
	Matrix mDraw;		//trees get translated before they are drawn
						//this is so that the tree's position will represent its base instead of its center
						//as an optimization, we store the translated position here
	virtual void MoveByLocal (Vector& vOffset){}
	virtual void MoveByLocal (float x, float y, float z) { MoveByLocal (Vector (x,y,z)); }

	virtual void MoveForward (float amount){}
	virtual void MoveRight (float amount){}
	virtual void MoveUp (float amount){}

	virtual void RotateX (float fDegrees){}
	virtual void RotateY (float fDegrees){}
	virtual void RotateZ (float fDegrees){}
	virtual void Rotate (Vector& vAxis, float fDegrees){}

	virtual void RotateLocalX (float fDegrees){}
	virtual void RotateLocalY (float fDegrees){}
	virtual void RotateLocalZ (float fDegrees){}

	virtual void LookAt (Vector& vPoint){}
	virtual void FaceDirection (Vector& vDirection){}
	virtual void TurnTo (Vector& vPoint, float fAngle){}
	
public:

	Tree(void) : Base(BASETYPE_TREE) 
	{ 
		iModel = rand() % TREEMODELS;
		pModel = &pTreeModels[iModel];
		fWidth = ModelGetWidth();
		fHeight = ModelGetHeight();
		fDepth = ModelGetDepth();

		pBoundingShape = new VerticalCylinder(fHeight, fWidth);
		pBoundingShape->SetColor(0, 0.7f, 0);
	}

	Tree (int iModelIdx)
	{
		iModel = iModelIdx;
		pModel = &pTreeModels[iModel];
		fWidth = ModelGetWidth();
		fHeight = ModelGetHeight();
		fDepth = ModelGetDepth();

		pBoundingShape = new VerticalCylinder(fHeight, fWidth);
		pBoundingShape->SetColor(0, 0.7f, 0);
	}

	virtual void MoveTo (Vector& vPoint)
	{
		mGlobal.SetPos (vPoint);
		mDraw.SetPos (vPoint + Vector (0, fHeight * 0.5f, 0));
	}
	virtual void MoveTo (float x, float y, float z) { MoveTo (Vector (x,y,z)); }

	void SetModel (int iModelIdx)
	{
		iModel = iModelIdx;
		pModel = &pTreeModels[iModel];
	}

	void ResetBoundingShapeColor(void)
	{
		pBoundingShape->SetColor(0, 1, 0);
	}
	
	virtual void Draw()
	{
		Direct3D::GetMatrixStack()->Push();
		Direct3D::GetMatrixStack()->LoadMatrixClass (mDraw);
		Tree::pTreeModels[iModel].Draw();
		Direct3D::GetMatrixStack()->Pop();
		//Direct3D::Wireframe (false);
		//TextureMgr::Set (0);
		//pBoundingShape->Draw();
	}
	
	#if _DEBUG
		ManageMemory(Tree, BASETYPE_TREE);
	#endif
};



class Bush : public Base
{
private:
	static DXModel pBushModels[TREEMODELS];
	int iModel;
	Matrix mDraw;		//trees get translated before they are drawn
						//this is so that the tree's position will represent its base instead of its center
						//as an optimization, we store the translated position here
	virtual void MoveByLocal (Vector& vOffset){}
	virtual void MoveByLocal (float x, float y, float z) { MoveByLocal (Vector (x,y,z)); }

	virtual void MoveForward (float amount){}
	virtual void MoveRight (float amount){}
	virtual void MoveUp (float amount){}

	virtual void RotateX (float fDegrees){}
	virtual void RotateY (float fDegrees){}
	virtual void RotateZ (float fDegrees){}
	virtual void Rotate (Vector& vAxis, float fDegrees){}

	virtual void RotateLocalX (float fDegrees){}
	virtual void RotateLocalY (float fDegrees){}
	virtual void RotateLocalZ (float fDegrees){}

	virtual void LookAt (Vector& vPoint){}
	virtual void FaceDirection (Vector& vDirection){}
	virtual void TurnTo (Vector& vPoint, float fAngle){}

public:
	Bush(void) : Base(BASETYPE_BUSH) 
	{ 
		iModel = rand() % BUSHMODELS;
		pModel = &pBushModels[iModel];
		fWidth = ModelGetWidth();
		fHeight = ModelGetHeight();
		fDepth = ModelGetDepth();

		pBoundingShape = new VerticalCylinder(fHeight, fWidth);
		pBoundingShape->SetColor(0, 0.7f, 0);
	}

	Bush (int iModelIdx)
	{
		iModel = iModelIdx;
		pModel = &pBushModels[iModel];
		fWidth = ModelGetWidth();
		fHeight = ModelGetHeight();
		fDepth = ModelGetDepth();

		pBoundingShape = new VerticalCylinder(fHeight, fWidth);
		pBoundingShape->SetColor(0, 0.7f, 0);
	}

	virtual void MoveTo (Vector& vPoint)
	{
		mGlobal.SetPos (vPoint);
		mDraw.SetPos (vPoint + Vector (0, fHeight * 0.5f, 0));
	}
	virtual void MoveTo (float x, float y, float z) { MoveTo (Vector (x,y,z)); }

	void SetModel (int iModelIdx)
	{
		iModel = iModelIdx;
		pModel = &pBushModels[iModel];
	}

	void ResetBoundingShapeColor(void)
	{
		pBoundingShape->SetColor(0, 1, 0);
	}
	
	virtual void Draw()
	{
		Direct3D::GetMatrixStack()->Push();
		Direct3D::GetMatrixStack()->LoadMatrixClass (mDraw);
		Bush::pBushModels[iModel].Draw();
		Direct3D::GetMatrixStack()->Pop();
		//Direct3D::Wireframe (false);
		//TextureMgr::Set (0);
		//pBoundingShape->Draw();
	}

	#if _DEBUG
		ManageMemory(Bush, BASETYPE_BUSH);
	#endif


};

#endif
