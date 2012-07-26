/////////////////////////////////////////////
// Cylinder.h
//
//	implements a cylinder bounding shape. A cylinder is defined by the following:
//		height (distance between the two circular surfaces):
//					inherited from the base class
//		radius: the radius of the circular faces. Inherited as fWidth from the base class
//		direction (orientation of the Y axis): inherited from the base class
//		base point: inherited as the position of the base class's matrix
//
//	a cylinder's y-axis always points in the direction of its height
/////////////////////////////////////////////

#ifndef CYLINDER_H
#define CYLINDER_H

#include "BoundingShape.h"

class Cylinder: public BoundingShape
{
public:
	// CONSTRUCTORS
	Cylinder();

	// Manage memory
	#if _DEBUG
		ManageMemory(Cylinder, SHAPE_CYLINDER);
	#endif

	// Cylinder
	// Use this function to construct a cylinder with a given height and radius, positioned at (0,0,0)
	Cylinder (float height, float radius);

	// Cylinder
	// Use this function to construct a cylinder that initially faces upwards
	// Input:
	//		base-> the base point of the cylinder (the middle of the bottom face)
	//		height-> the height of the cylinder (along the y axis)
	Cylinder (Vector& base, float height, float radius);

	// Cylinder
	// Use this function to construct an arbitrarily oriented cylinder
	// Input:
	//		base-> the base point of the cylinder (in the middle of the bottom face)
	//		top-> top point of the cylinder (in the middle of the top face)
	Cylinder (Vector& base, Vector& top, float radius);

	virtual ~Cylinder(){}

	//////////////////////////////////
	// CONVENIENCE METHODS
	//////////////////////////////////
	Vector GetBottom();			//calculates the point at the bottom of the cylinder
	Vector GetTop();			//calculates the point at the top of the cylinder
	Vector GetCenter();			//calculates the cylinder's center point

	//////////////////////////////////
	// COLLISION DETECTION
	//////////////////////////////////
	virtual bool CollidesWith (BoundingShape* other, Vector* vIntersect = NULL);
	virtual bool CollidesWith (Vector& vPoint);
	virtual bool CollidesWith (Line& line, Vector* vIntersection = 0);
	virtual bool CollidesWith (Plane& plane, Vector* vIntersection = 0);
	virtual bool CollidesWith (Triangle& triangle, Vector* vIntersection = 0);
	virtual bool CollidesWith (Quad& quad, Vector* vIntersection = 0);
	
	virtual bool CollisionClamp (BoundingShape* other);

	//need a draw function for debugging
	virtual void Draw();
};


#endif