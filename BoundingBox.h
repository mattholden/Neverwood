//////////////////////////////////
// BoundingBox.h
//
// implements an axis-aligned bounding box
// an axis-aligned bounding box is represented by the following:
//		Center: the global position of the center of the bounding box.
//				this is inherited as the position column of the base class's matrix
//		Dimensions:
//				with, height, and depth are inherited from the base class
////////////////////////////////////

#include "BoundingShape.h"

// TO DO: remove this after the renderer is working
#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>

class BoundingBox: public BoundingShape
{
public:
	// CONSTRUCTORS
	BoundingBox();
	BoundingBox (float width, float height, float depth);
	virtual ~BoundingBox(){}

	// CONVENIENCE METHODS
	float GetLeft() { return mGlobal.fMatrix[12] - fWidth*0.5f; }
	float GetRight() { return mGlobal.fMatrix[12] + fWidth*0.5f; }
	float GetBottom() { return mGlobal.fMatrix[13] - fHeight*0.5f; }
	float GetTop() { return mGlobal.fMatrix[13] + fHeight*0.5f; }
	float GetBack() { return mGlobal.fMatrix[14] + fDepth*0.5f; }
	float GetFront() { return mGlobal.fMatrix[14] - fDepth*0.5f; }

	Quad GetLeftFace();
	Quad GetRightFace();
	Quad GetTopFace();
	Quad GetBottomFace();
	Quad GetFrontFace();
	Quad GetBackFace();

	// Manage memory
	#if _DEBUG
		ManageMemory(BoundingBox, SHAPE_BOX);
	#endif

	// COLLISION DETECTION
	virtual bool CollidesWith (BoundingShape* other, Vector* vIntersection = NULL);
	virtual bool CollidesWith (Vector& vPoint);
	virtual bool CollidesWith (Line& line, Vector* vIntersection = 0);
	virtual bool CollidesWith (Triangle& triangle, Vector* vIntersection = 0);
	virtual bool CollidesWith (Quad& quad, Vector* vIntersection = 0);

	// override movement. This is necessary, since an axis aligned bounding box must always face the z axis
	virtual void RotateGlobalX (float) {}
	virtual void RotateGlobalY (float) {}
	virtual void RotateGlobalZ (float) {}
	virtual void RotateGlobal (Vector& vAxis, float fDegrees){}

	virtual void RotateX (float) {}
	virtual void RotateY (float) {}
	virtual void RotateZ (float) {}
	
	virtual void LookAt (Vector& vPoint){}
	virtual void FaceDirection (Vector& vPoint){}
	virtual void TurnTo (Vector& vPoint){}
	
	// need a draw function for debugging
	virtual void Draw();

};