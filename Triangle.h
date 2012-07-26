//////////////////////////////
// Triangle.h
//
// Defines a triangle, which is represented by 3 vertices
//////////////////////////////

#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "Vector.h"
#include "Plane.h"
#include "MemoryMgr.h"

class Triangle
{
public:
	Vector v1;
	Vector v2;
	Vector v3;
	Vector vNormal;

	// CONSTRUCTORS
	Triangle(){}
	Triangle (Vector& V1, Vector& V2, Vector& V3);
	Triangle (Vector& V1, Vector& V2, Vector& V3, Vector& normal);

		// Manage memory
	#if _DEBUG
		ManageMemory(Triangle, SHAPE_TRIANGLE);
	#endif

	// COLLISION DETECTION
	
	////////////////////////////////////
	// CollidesWith
	//
	// Computes the intersection of a triangle with a plane
	// Returns true if the triangle intersects the plane, false otherwise
	// Outputs the point of intersection in the optional parameter vIntersection
	//
	// Note that collision detection is slightly inaccurate:
	//		if more than one edge of the triangle intersects the plane, the point of intersection is that of the first edge that intersects the plane
	//		this is a deliberate imperfection in order to save computation, and it should not be noticable since triangles are assumed to be small
	////////////////////////////////////
	bool CollidesWith (Plane& plane, Vector* vIntersection = 0);

	//same as above, but with other types
	bool CollidesWith (Line& line, Vector* vIntersection = 0);
	
	//same as above, BUT:
	//if the triangle intersects the plane containing the other triangle, but not the triangle itself, the point of intersection is that of the plane
	bool CollidesWith (Triangle& other, Vector* vIntersection = 0);

	////////////////////////////////////////
	// CollidesWith (Vector& point)
	//
	// checks if a given point is inside the bounds of the triangle by running a half space test on each edge
	////////////////////////////////////////
	bool CollidesWith (Vector& point);
};

#endif