///////////////////////////////////////////////////////////////
// BoundingShape.h
//
//	BoundingShape is an abstract base class for shapes that are used as collision boundaries.
//	A bounding shape can be one of the following:
//
//	Sphere
//	Cylinder
//	Axis Aligned Bounding Box  (abbreviated as AABB)
//	Frustrum
//
///////////////////////////////////////////////////////////////

#ifndef BOUNDINGSHAPE_H
#define BOUNDINGSHAPE_H

#include "Identifiers.h"
#include "Matrix.h"
#include "Vector.h"
#include "CollisionUtils.h"
#include "Line.h"
#include "Triangle.h"
#include "Quad.h"

// TO DO: REMOVE THESE AFTER THE RENDERER IS DONE
#include <windows.h>
#include <gl/gl.h>
#include "MemoryMgr.h"

class BoundingShape
{
public:

	//data members
	Matrix mGlobal;		//stores the position and orientation of the bounding box
	
	float fWidth;		//if the bounding shape is a box, this is the box's width
						//for a sphere, fWidth is the sphere's radius
						//for a cylinder, fWidth is the ylinder's radius
						//for a frustum, fWidth is the width of the far clipping plane
	float fHeight;		//denotes the height of a box or cylinder.
						//fHeight is not used for spheres
						//for a frustum, fHeight is the height of the far clipping plane
	float fDepth;		//not used for spheres or cylinders
						//equivalent to the far clipping distance for a view frustum
	int iObjectType;

	Vector vOffset;

	float fRed, fGreen, fBlue, fAlpha;

	//member functions
	BoundingShape(): fRed(1), fGreen(1), fBlue(1), fAlpha(1) {} 
	virtual ~BoundingShape(){}

	// Manage memory
	#if _DEBUG
		ManageMemory(BoundingShape, SHAPE_BASE);
	#endif

	// MATRIX ACCESSORS (No modifiers needed since we're returning pointers) //
	Vector GetPos(void)	{ return mGlobal.GetPos(); }
	Vector GetZ(void)	{ return mGlobal.GetZ(); }
	Vector GetY(void)	{ return mGlobal.GetY(); }
	Vector GetX(void)	{ return mGlobal.GetX(); }
	float GetXPos(void)	{ return mGlobal.fMatrix[12]; }
	float GetYPos(void)	{ return mGlobal.fMatrix[13]; }
	float GetZPos(void)	{ return mGlobal.fMatrix[14]; }

	// MUTATORS
	void SetColor (float red, float green, float blue, float alpha=1)
	{
		fRed = red; fGreen = green; fBlue = blue; fAlpha = alpha;
	}
	void SetPos (Vector& pos) { memcpy (mGlobal.fMatrix + 12, pos.fComp, 12); }
	void SetPos (float x, float y, float z) { SetPos ( Vector (x,y,z)); }

	/////////////////////////////////////////////////
	// MOVEMENT FUNCTIONS
	//////////////////////////////////////////////////
		
	// puts the object to the given position
	void MoveTo (Vector& vPoint) { memcpy (mGlobal.fMatrix + 12, vPoint.fComp, 12); }

	// translates the object globally by a given amount
	void MoveBy	(Vector& vPoint);

	// moves the vector along its local axes
	void MoveForward (float amount);
	void MoveRight (float amount);
	void MoveUp (float amount);	

	// rotate around the world's axes
	virtual void RotateX (float fDegrees);
	virtual void RotateY (float fDegrees);
	virtual void RotateZ (float fDegrees);
	virtual void Rotate (Vector& vAxis, float fDegrees);

	// rotate around the object's local axes
	virtual void RotateLocalX (float fDegrees);
	virtual void RotateLocalY (float fDegrees);
	virtual void RotateLocalZ (float fDegrees);
	
	// more sophisticated movement algorithms
	virtual void LookAt (Vector& vPoint);
	virtual void FaceDirection (Vector& vDirection);
	virtual void TurnTo (Vector& vPoint, float fAngle);

	///////////////////////////////
	// COLLISION DETECTION
	////////////////////////////////
	
	////////////////////////////////////////////////////////
	// CollidesWith
	//
	//	determines whether two bounding shapes intersect.
	//	If they intersect, CollidesWith optionally computes the point of intersection and stores it in vIntersect
	/////////////////////////////////////////////////////////
	virtual bool CollidesWith (BoundingShape* other, Vector* vIntersect = NULL) { return false; }

	////////////////////////////////////////////////////////
	// CollidesWith
	//
	//	CollidesWith is overloaded to handle collision with simple geometry:
	//		CollidesWith (Vector) determines if a point is inside a bounding shape
	//		CollidesWith (Line) determines if a line intersects the bounding shape
	//		CollidesWith (Triangle) determines if a triangle intersets the bounding shape
	//		CollidesWith (Quad) determines if a quad is inside the bounding shape
	//////////////////////////////////////////////////////////
	virtual bool CollidesWith (Vector& vPoint) { return false; }
	virtual bool CollidesWith (Line& line, Vector* vIntersection = NULL) { return false; }
	virtual bool CollidesWith (Triangle& triangle, Vector* vIntersection = NULL);
	virtual bool CollidesWith (Quad& quad, Vector* vIntersection = NULL);
	virtual bool CollidesWith (Plane& plane, Vector* vIntersection = NULL) { return false; }

	////////////////////////////////////////////////
	// Draw
	// Draws the bounding shape
	// for the sake of efficiency and flexibility, the draw fun ction does not apply context changes
	//////////////////////////////////////////////////
	virtual void Draw(){}

	//////////////////////////////////////////////////
	// CLAMPING
	//
	// Clamping is a common collision response. It first checks if the bounding box collides with another.
	// If it does, it places the current bounding box just outside the one it collided with
	//
	//	returns: true if the object got clamped (i.e. if it collided with the other object), false otherwise.
	//
	// NOTE: clamping is only implemented for the following:
	//		sphere to sphere
	//		vertical cylinder to vertical cylinder
	//		sphere to vertical cylinder (NOT vertical cylinder to sphere!)
	//		sphere / vertical cylinder to point
	// NOTE: clamping automatically does collision detection, since it requires an intersection 
	////////////////////////////////////////////////////
	virtual bool CollisionClamp (BoundingShape* other){ return false; }
	virtual bool CollisionClamp (Vector& vPoint) { return false; }

};

#endif