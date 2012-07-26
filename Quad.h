//////////////////////////////
// Quad.h
//
// Defines a quad, which is represented by 4 vertices
// A quad is any convex, planar polygon with 4 vertices v1 through v4
//
// As opposed to a triangle, the order of the vertices matters:
// the vertices must be in either clockwise or counter-clockwise order.
// In other words, there is no edge from v1 to v3, or from v2 to v4
//////////////////////////////

#ifndef QUAD_H
#define QUAD_H

//#include "BoundingShape.h"
#include "Vertex.h"
#include "Vector.h"
#include "Triangle.h"

class Quad// : public BoundingShape
{
public:
	Vector v1;
	Vector v2;
	Vector v3;
	Vector v4;

	// Manage memory
	#if _DEBUG
		ManageMemory(Quad, SHAPE_QUAD);
	#endif

	// CONSTRUCTORS
	Quad(){}
	Quad (Vector& V1, Vector& V2, Vector& V3, Vector& V4);

	// COLLISION DETECTION
	
	////////////////////////////////////
	// CollidesWith
	//
	// Computes the intersection of a quad with a plane
	// Returns true if the quad intersects the plane, false otherwise
	// Outputs the point of intersection in the optional parameter vIntersection
	////////////////////////////////////
	bool CollidesWith (Plane& plane, Vector* vIntersection = 0);

	//same as above, but with other types
	bool CollidesWith (Line& line, Vector* vIntersection = 0);

	//same as above, BUT:
	//if the quad intersects the plane containing the other shape, but not the shape itself, the point of intersection is that of the plane
	bool CollidesWith (Triangle& other, Vector* vIntersection = 0);
	bool CollidesWith (Quad& other, Vector* vIntersection = 0);

	////////////////////////////////////////
	// CollidesWith (Vector& point)
	//
	// checks if a given point is inside the bounds of the quad by running a half space test on each edge
	////////////////////////////////////////
	bool CollidesWith (Vector& point);

	//draw function for debugging purposes
	void Draw();
};

#endif