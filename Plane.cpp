#include "Plane.h"
#include "MathUtils.h"

Plane::Plane (Vector& normal, Vector& point)
{
	vNormal = normal.Normalize();
	vPoint = point;
}

float Plane::SignedDistance (Vector& point) 
{ 
	return (  (point - vPoint) * vNormal );
}

bool Plane::HalfSpaceTest (Vector& point)
{
	return SignedDistance (point) >= 0.0f;
}

bool Plane::CollidesWith (Line& line, Vector* vIntersection)
{
	//compute the intersection of the plane with the infinite line containing the line segment
	if (vIntersection)
	{
		Vector lineSegment = line.vEnd - line.vBegin;		//represents the direction + magnitude of the line segment
		float distance = SignedDistance (line.vBegin);
		float ratio = -distance / (lineSegment * vNormal);
		*vIntersection = line.vBegin + lineSegment * ratio;
	}

	//failure if both points are on the same side of the plane
	if (HalfSpaceTest (line.vBegin) == HalfSpaceTest (line.vEnd))
		return false;

	//success otherwise
	return true;
}

bool Plane::CollidesWith (Vector& point)
{
	return ( ((point - vPoint) * vNormal) == 0);
}