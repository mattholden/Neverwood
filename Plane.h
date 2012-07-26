////////////////////////////////////////////////
// Plane.h
//
// Defines functionality for an infinite plane.
// A plane is represented by two vectors:
//
// the plane's normal vector
// the position of a point on the plane
//////////////////////////////////////////////////

#ifndef PLANE_H
#define PLANE_H

#include "Line.h"
#include "MathUtils.h"
 #include "MemoryMgr.h"

class Plane
{
public:
	Vector vNormal;
	Vector vPoint;

	Plane (){}
	Plane (Vector& normal, Vector& point);

	// Manage memory
	#if _DEBUG
		ManageMemory(Plane, SHAPE_PLANE);
	#endif

	/////////////////////////////
	// SignedDistance
	//
	// Computes the distance of a point to the plane
	// Note that the distance is negative if the plane's normal vector points away from the point
	/////////////////////////////
	float SignedDistance (Vector& vPoint);

	/////////////////////////////
	// HalfSpaceTest
	//
	// Returns bool if the plane's normal vector points toward a given point, false otherwise
	///////////////////////////////
	bool HalfSpaceTest (Vector& vPoint);

	////////////////////////////////////
	// CollidesWith
	//
	// Computes the intersection of a line segment with the plane
	// Returns true if the line segment intersects the plane, false otherwise
	// Outputs the point of intersection in the optional parameter vIntersection
	////////////////////////////////////
	bool CollidesWith (Line& line, Vector* vIntersection = 0);

	// checks if a point is on the plane
	bool CollidesWith (Vector& vPoint);
};


#endif