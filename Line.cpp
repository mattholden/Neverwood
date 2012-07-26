#include "Line.h"
#include "CollisionUtils.h"
#include "Direct3D.h"

Line::Line (Vector& begin, Vector& end)
{
	vBegin = begin;
	vEnd = end;
}

float Line::GetLength() { return ~(vEnd - vBegin); }

float Line::DistanceTo (Line& other)
{
	Vector vNormal = (vEnd - vBegin) & (other.vEnd - other.vBegin);		//the normal vector of a fictitions plane 
	// special case: Parallel lines
	if (vNormal == Vector (0.0f, 0.0f, 0.0f))
		return DistanceTo (other.vBegin);
	// general case
	float fDot = (other.vBegin - vBegin) * (vNormal.Normalize());
	return ABS (fDot);
}

float Line::DistanceTo (Vector& point)
{
	return ~(point - ClosestPoint(point));
}

Vector Line::ClosestPoint (Vector& point)
{
	return vBegin + ((point - vBegin) % (vEnd - vBegin));
}

Vector Line::ClosestPoint (Line& other)
{
	// special case: if the lines are parallel, just return the center point
	Vector vThisDirection = vEnd - vBegin;
	Vector vOtherDirection = other.vEnd - other.vBegin;
	Vector vTemp = vThisDirection & vOtherDirection;
	if ( vTemp == Vector(0,0,0))
		return ( (vEnd + vBegin) * 0.5f);

	// general case

	//first, find the plane that contains the other line and is orthogonal to the plane containing both lines
	Vector vNormal = (vTemp & vOtherDirection).Normalize();	//normal vector of the plane
												//can't get access to plane.cpp from here, so this will have to suffice

	//now find the intersection of the orthogonal plane with
	// the infinite line containing this line segment

	// CHANGED
	// now doing other.vBegin - vBegin instead of the other way around
	//float fSignedDist = (other.vBegin-vBegin) * vNormal;		//the signed distance of the line's beginning from the plane
	float fSignedDist = (vBegin - other.vBegin) * vNormal;
	float fLineDist = vThisDirection * vNormal;	//the distance of the other line, projected onto the plane's normal
	float fRatio = -fSignedDist / fLineDist;
	return vBegin + vThisDirection * fRatio;
}

bool Line::SameSide (Vector& v1, Vector& v2)
{
	Vector vToV1 = v1 - vBegin;
	Vector vLineDirection = vEnd - vBegin;
	Vector vTemp = vToV1 & vLineDirection;	//the normal vector to the plane containing the line and v1
	Vector vNormal = (vLineDirection & vTemp).Normalize();		//the normal vector to the plane representing the half space test

	float fDot1 = vToV1 * vNormal;
	float fDot2 = (v2 - vBegin) * vNormal;
	return fDot1 * fDot2 > -0.0001f;	//give the line a little thickness to account for floating point errors
										//after all, it is important that a point on the line always returns true!
}

bool Line::ContainsPoint (Vector& point)
{
	Vector vLineDirection = vEnd - vBegin;
	float fDot1 = (point - vBegin) * vLineDirection;
	float fDot2 = (vEnd - point) * vLineDirection;
	return fDot1 * fDot2 > -FLOAT_EPSILON;	//return whether the two dot products have the same sign
}

void Line::Draw (float red, float green, float blue)
{
	Direct3D::SetVertexFormat (D3DFVF_XYZ, 12);
	Direct3D::SetColor (red, green, blue);
	float fComponents[6];		//the coordinates of the line's begin and end points, concatenated	
	memcpy (fComponents, vBegin.fComp, 12);
	memcpy (fComponents+3, vEnd.fComp, 12);
	Direct3D::DrawLines (fComponents, 2);
}