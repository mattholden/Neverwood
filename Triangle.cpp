#include "Triangle.h"

Triangle::Triangle (Vector& V1, Vector& V2, Vector& V3)
{
	v1 = V1;
	v2 = V2;
	v3 = V3;
	vNormal = ((v2-v1) & (v3-v1)).Normalize();
}

Triangle::Triangle (Vector& V1, Vector& V2, Vector& V3, Vector& normal)
{
	v1 = V1;
	v2 = V2;
	v3 = V3;
	vNormal = normal;
}



///////////////////////
// COLLISION DETECTION
///////////////////////

bool Triangle::CollidesWith (Plane& plane, Vector* vIntersection)
{
	// check if any of the triangle's edges intersect the plane
	if (plane.CollidesWith (Line (v1,v2), vIntersection))
		return true;
	if (plane.CollidesWith (Line (v2,v3), vIntersection))
		return true;
	if (plane.CollidesWith (Line (v1,v3), vIntersection))
		return true;

	// no intersection: return false
	return false;
}

bool Triangle::CollidesWith (Line& line, Vector* vIntersection)
{
	// first, check if the line even intersects the plane containing the triangle
	Vector vPlaneIntersection;
	if (Plane(vNormal, v1).CollidesWith (line, &vPlaneIntersection) == false)
		return false;

	// check if the intersection point is within the triangle's bounds
	if (!CollidesWith (vPlaneIntersection))
		return false;

	// the line collides with the triangle, so return the result
	if (vIntersection)
		*vIntersection = vPlaneIntersection;
	return true;
}

bool Triangle::CollidesWith (Triangle& other, Vector* vIntersection )
{
	// just do a line to triangle check with all of the other triangle's lines
	if (other.CollidesWith (Line (v1,v2), vIntersection))
		return true;
	if (other.CollidesWith (Line (v2,v3), vIntersection))
		return true;
	if (other.CollidesWith (Line (v1,v3), vIntersection))
		return true;

	return false;
}

bool Triangle::CollidesWith (Vector& point)
{
	if (Line (v1,v2).SameSide (v3, point) == false)
		return false;
	if (Line (v2,v3).SameSide (v1, point) == false)
		return false;
	if (Line (v3,v1).SameSide (v2, point) == false)
		return false;

	//the point passed all the half space tests, so check to see if it's co-planar
	/*
	Vector vToPoint = point - v1;
	float fDot = vToPoint * vNormal;
	if (!FloatEqual ( vToPoint * vNormal, 0.0f))
		return false;
	*/
	return true;
}

