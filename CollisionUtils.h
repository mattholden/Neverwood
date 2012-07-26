/////////////////////////////////////////////////////
// CollisionUtils.h
//
// Provides quick access to generally useful collision routines, 
// implemented in an old-school fashion for the sake of efficiency and simplicity.
// Each routine has the following scheme:
//
// Returns: true if collision occured, false otherwise
// Output: optionally outputs the point of intersection	
/////////////////////////////////////////////////////

#ifndef COLLISION_UTILS_H
#define COLLISION_UTILS_H

#include "Vector.h"

// PROTOTYPES
static bool SphereToSphere (Vector& center1, float radius1, Vector& center2, float radius2, Vector* vIntersection = 0);
static bool SphereToPoint (Vector& center, float radius, Vector& point, Vector* vIntersection = 0);
static bool SphereToPlane (Vector& center, float radius, Vector& normal, Vector& point, Vector* vIntersection = 0);
static bool SphereToLine (Vector& center, float radius, Vector& normal, Vector& point, Vector* vIntersection = 0);
static bool HalfSpaceTest (Vector& point, Vector& normal, Vector& planePoint);

///////////////// SPHERES ////////////////////////////

// Sphere to Sphere
bool SphereToSphere (Vector& center1, float radius1, Vector& center2, float radius2, Vector* vIntersection)
{
	if ( (center2 - center1).LengthSquared() > (radius1 + radius2) * (radius1 + radius2) )
		return false;
	
	// collision succeeded, so output intersection if appropriate
	if (vIntersection)
		*vIntersection = center1 + ((center2 - center1)* (radius1 / (radius1 + radius2)));
	return true;
}

// Sphere to Point
bool SphereToPoint (Vector& center, float radius, Vector& point, Vector* vIntersection)
{
	return ( (point - center).LengthSquared() <= radius * radius);
}

// Sphere to Plane
// computes the point of intersection of a sphere with a plane
// CAUTION: it is assumed that the plane's normal vector has unit length
bool SphereToPlane (Vector& center, float radius, Vector& normal, Vector& point, Vector* vIntersection)
{
	// check if the sphere is close enough
	Vector unitNormal = normal.Normalize();
	float fDistance = (center - point) * unitNormal;
	if (ABS(fDistance) > radius)
		return false;

	// compute intersection
	if (vIntersection)
	{
		(*vIntersection) = center - (unitNormal * fDistance);
	}
	return true;
}
// Sphere to Line
// calculates the point of intersection of a sphere with a line
// even if there is no intersection, vIntersection will store the point on the line closest to the sphere
bool SphereToLine (Vector& center, float radius, Vector& begin, Vector& end, Vector* vIntersection)
{
	Vector vToSphere = center - begin;
	Vector vLine = end - begin;
	Vector vLineNormalized = vLine.Normalize();

	float fDotProduct = vToSphere * vLineNormalized;		//the distance of the closest point from the beginning of the line
	
	// compute the closest point
	Vector vClosestPoint = begin + vLineNormalized * fDotProduct;
	if (vIntersection)
		*vIntersection = vClosestPoint;

	// determine whether collision occured
	if (fDotProduct < 0 || fDotProduct * fDotProduct > vLine.LengthSquared())
		return false;
	return SphereToPoint (center, radius, vClosestPoint);
}


// PLANES


///////////////////////////////////////////////
// HALF SPACE TEST
// Determines whether a plane's normal vector points towards a given point
//
// Input:
//		point:	the point to run the half space test on
//		normal: the normal vector of the plane
//		planePoint: any point lying on the plane
///////////////////////////////////////////////
bool HalfSpaceTest (Vector& point, Vector& normal, Vector& planePoint)
{
	return (planePoint - point) * normal >= 0;
}

#endif

