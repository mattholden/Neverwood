
/////////////////////////////////////////////
// VerticalCylinder.h
//
//	implements a vertical cylinder bounding shape. A vertical cylinder is defined by the following:
//		height (distance between the two circular surfaces):
//					inherited from the base class
//		radius: the radius of the circular faces. Inherited as fWidth from the base class
//
//  NOTE:	when a vertical cylinder computes collision detection, it acts like a capsule, with at sphere coming out of the top and bottom of the cylinder
//	NOTE:	a vertical cylinder's axis always points up.
//			this makes vertical cylinders MUCH faster than regular cylinders.
/////////////////////////////////////////////

#ifndef VerticalCylinder_H
#define VerticalCylinder_H

#include "BoundingShape.h"

class VerticalCylinder: public BoundingShape
{
public:

	// Manage memory
	#if _DEBUG
		ManageMemory(VerticalCylinder, SHAPE_VERTICAL_CYLINDER);
	#endif

	// VerticalCylinder
	// Use this function to construct a VerticalCylinder with a given height and radius, positioned at (0,0,0)
	VerticalCylinder (float height = 1.0f, float radius = 1.0f);
	
	// SetHeight
	// use to change the height of a cylinder
	void SetHeight (float newHeight);
	
	// SetRadius
	void SetRadius (float newRadius);

	virtual ~VerticalCylinder(){}

	//////////////////////////////////
	// CONVENIENCE METHODS
	//////////////////////////////////
	float GetBottom();			//calculates the y coordinate of the bottom of the VerticalCylinder
	float GetTop();				//calculates the y coordinate of the top of the VerticalCylinder
	Vector GetTopPos();
	Vector GetBottomPos();

	//////////////////////////////////
	// COLLISION DETECTION
	//////////////////////////////////
	virtual bool CollidesWith (BoundingShape* other, Vector* vIntersect = NULL);
	virtual bool CollidesWith (Vector& vPoint);
	virtual bool CollidesWith (Line& line, Vector* vIntersection = 0);
	virtual bool CollidesWith (Plane& plane, Vector* vIntersection = 0);
	virtual bool CollidesWith (Triangle& triangle, Vector* vIntersection = 0);

	virtual bool CollisionClamp (BoundingShape* other);
	virtual bool CollisionClamp (Vector& point);

	//need a draw function for debugging
	virtual void Draw();


	// ENSURE THAT THE Y AXIS REMAINS VERTICAL
	virtual void RotateX(float){}
	virtual void RotateLocalX(float){}
	virtual void RotateZ(float){}
	virtual void RotateLocalZ(float){}
	virtual void LookAt (Vector& vPoint) 
	{ 
		LookAt (Vector (vPoint.fComp[0], GetYPos(), vPoint.fComp[2])); 
	}
	virtual void FaceDirection (Vector& vDirection) 
	{ 
		FaceDirection (Vector (vDirection.fComp[0], 0, vDirection.fComp[2])); 
	}
	virtual void TurnTo (Vector& vPoint, float fAngle) 
	{ 
		TurnTo (Vector (vPoint.fComp[0], GetYPos(), vPoint.fComp[2]), fAngle);
	}
};

#endif