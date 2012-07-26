
#ifndef BUSH_H
#define BUSH_H

#include "Base.h"
#include "VerticalCylinder.h"

// trees and bushes share static models
// for the sake of variety, trees and bushes have more than one static model associated with them
#define BUSHMODELS 3

class Bush : public Base
{
private:
	static DXModel pBushModels[BUSHMODELS];
	static int iBushModels;

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

	Bush (int iModelIdx) : Base (BASETYPE_BUSH)
	{
		iModel = iModelIdx;
		pModel = &pBushModels[iModel];
		fWidth = ModelGetWidth();
		fHeight = ModelGetHeight();
		fDepth = ModelGetDepth();

		pBoundingShape = new VerticalCylinder(fHeight, fWidth);
		pBoundingShape->SetColor(0, 0.7f, 0);
	}

	static void LoadModel (int iModelNum, char* strFilename);
	static void DeleteModel (int iModelNum);
	static int GetNumModels () { return iBushModels; }

	virtual void MoveTo (Vector& vPoint);
	virtual void MoveTo (float x, float y, float z);

	void SetModel (int iModelIdx);
	void ResetBoundingShapeColor(void);
	
	void Draw();

	#if _DEBUG
		ManageMemory(Bush, BASETYPE_BUSH);
	#endif


};

#endif