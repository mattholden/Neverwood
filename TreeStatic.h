#ifndef TREE_H
#define TREE_H

#include "Base.h"
#include "VerticalCylinder.h"
// trees and bushes share static models
// for the sake of variety, trees and bushes have more than one static model associated with them
#define TREEMODELS 3

// TREE
// essentially, a tree is a static model whose mesh can be one of several statically stored models
// by default, a tree has a random model associated with it
// alternately, you can create a tree using a specific model by passing a model number into the constructor, or by calling the SetModel function
class Tree : public Base
{

private:
	static DXModel pTreeModels[TREEMODELS];
	static int iTreeModels;	//stores the total number of different trees we're using. Basically a copy of NUMTREES

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

	Tree::Tree(void) : Base(BASETYPE_TREE) 
	{ 
		iModel = rand() % TREEMODELS;
		pModel = &pTreeModels[iModel];
		fWidth = ModelGetWidth();
		fHeight = ModelGetHeight();
		fDepth = ModelGetDepth();

		pBoundingShape = new VerticalCylinder(fHeight, fWidth);
		pBoundingShape->SetColor(0, 0.7f, 0);
	}

	Tree::Tree (int iModelIdx)
	{
		iModel = iModelIdx;
		pModel = &pTreeModels[iModel];
		fWidth = ModelGetWidth();
		fHeight = ModelGetHeight();
		fDepth = ModelGetDepth();

		pBoundingShape = new VerticalCylinder(fHeight, fWidth);
		pBoundingShape->SetColor(0, 0.7f, 0);
	}

	static void LoadModel (int iModelNum, char* strFile);
	static void ScaleModel (int iModelNum, float fScale);
	static void DeleteModel (int iModelNum);
	static int GetNumModels () { return iTreeModels; }

	void MoveTo (Vector& vPoint);
	void MoveTo (float x, float y, float z);

	void SetModel (int iModelIdx);
	
	void ResetBoundingShapeColor(void);
	
	virtual void Draw();
	
	#if _DEBUG
		ManageMemory(Tree, BASETYPE_TREE);
	#endif
};

#endif