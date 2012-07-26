//////////////////////////////////////////
// Sphere.h
//
//	defines a bounding sphere used for collision detection
//	A sphere is represented by the following:
//		* radius: represented by the base class's width.
//		* center: represented by the 4th column of the base class's matrix
///////////////////////////////////////////

#ifndef SPHERE_H
#define SPHERE_H

#include "BoundingShape.h"

class Sphere: public BoundingShape
{
public:
	Sphere (float radius = 1.0f) 
	{ 
		iObjectType = SHAPE_SPHERE;
		fWidth = fHeight = fDepth = radius;
	}
	
	Sphere (Vector& vPos, float radius)
	{
		iObjectType = SHAPE_SPHERE;
		fWidth = fHeight = fDepth = radius;
		MoveTo (vPos);
	}
	virtual ~Sphere(){}

	// Manage memory
	#if _DEBUG
		ManageMemory(Sphere, SHAPE_SPHERE);
	#endif

	virtual bool CollidesWith (BoundingShape* other, Vector* vIntersection = NULL);
	virtual bool CollidesWith (Vector& vPoint);
	virtual bool CollidesWith (Line& line, Vector* vIntersection = 0);
	virtual bool CollidesWith (Plane& plane, Vector* vIntersection = 0);
	virtual bool CollidesWith (Triangle& triangle, Vector* vIntersection = 0);
	virtual bool CollidesWith (Quad& quad, Vector* vIntersection = 0);

	virtual bool CollisionClamp (BoundingShape* other);
	virtual bool CollisionClamp (Vector& point);

	//need a draw function for debugging
	virtual void Draw();

};


#endif