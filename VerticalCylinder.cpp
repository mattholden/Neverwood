#include "VerticalCylinder.h"
#include "Sphere.h"
#include "Direct3d.h"
#include "Shapes.h"


VerticalCylinder::VerticalCylinder (float height, float radius)
{
	iObjectType = SHAPE_VERTICAL_CYLINDER;
	fHeight = height;
	fWidth = fDepth = radius;
	mGlobal.fMatrix[12] = mGlobal.fMatrix[13] = mGlobal.fMatrix[14] = 0;
}

float VerticalCylinder::GetBottom() { return mGlobal.fMatrix[13] - 0.5f*fHeight; }
float VerticalCylinder::GetTop() { return mGlobal.fMatrix[13] + 0.5f*fHeight; }
Vector VerticalCylinder::GetTopPos() { return Vector (GetXPos(), GetTop(), GetZPos()); }
Vector VerticalCylinder::GetBottomPos() { return Vector (GetXPos(), GetBottom(), GetZPos()); }


bool VerticalCylinder::CollidesWith (BoundingShape* other, Vector* vIntersection)
{
	switch (other->iObjectType)
	{
		//VerticalCylinder to Sphere
	case SHAPE_SPHERE:
		return other->CollidesWith (this, vIntersection);
		break;
		
		//VerticalCylinder to VerticalCylinder
	case SHAPE_VERTICAL_CYLINDER:
		{
			Vector vToOther = other->GetPos() - GetPos();
			
			// test if the cylinders are close enough horizontally
			Vector vHor (vToOther.fComp[0], 0, vToOther.fComp[2]);
			if (vHor.LengthSquared() > (fWidth + other->fWidth) * (fWidth + other->fWidth))
				return false;

			// test if they're close enough vertically
			if (GetBottom() > ((VerticalCylinder*)other)->GetTop() || GetTop() < ((VerticalCylinder*)other)->GetBottom())
				return false;

			// compute point of intersection
			// let's make the point of intersection on the same height as the center of the cylinder.
			// That way, collision response won't change the cylinder's vertical position
			if (vIntersection)
			{
				// get the point on the infinite line containing the other cylinder's axis that is closes to this cylinder's center
				Vector vPos = GetPos();
				Vector vOtherPos = other->GetPos();
				Vector vClosestOther (vOtherPos.fComp[0], GetYPos(), vOtherPos.fComp[2]);

				float fRatio = fWidth / (fWidth + other->fWidth);
				*vIntersection = vPos + (vOtherPos - vPos) * fRatio;				
			}
			return true;

		}//VerticalCylinder
		break;

	case SHAPE_FRUSTUM:
		return other->CollidesWith (this, vIntersection);
		break;

	case SHAPE_BOX:
		return other->CollidesWith (this, vIntersection);
		break;

	default:
		return false;
	}
}

bool VerticalCylinder::CollidesWith (Vector& vPoint)
{
	// check if the point is close enough to the axis
	Vector vToPoint = vPoint - GetPos();
	vToPoint.fComp[1] = 0;
	if (vToPoint.LengthSquared() > fWidth * fWidth)
		return false;

	// check if the point is beyond the beginning or end point of the sphere's axis
	if (vPoint.fComp[1] < GetBottom() - fWidth)
		return SphereToPoint (GetBottomPos(), fWidth, GetPos());
	if (vPoint.fComp[1] > GetTop() + fWidth * 0.5f)
		return SphereToPoint (GetTopPos(), fWidth*0.5f, GetPos());

	return true;	
}

//////////////////////////////////////////////////
// VerticalCylinder to Line collision
// The point of intersection is slightly inaccurate:
// it returns the point on the line that is closest to the VerticalCylinder
// also, it returns this point regardless of whether the two objects intersected
///////////////////////////////////////////////////
bool VerticalCylinder::CollidesWith (Line& line, Vector* vIntersection)
{
	// check if the line is close enough to the axis
	Vector vBase = GetBottomPos();
	Vector yAxis = GetY();
	Vector vTop = GetTopPos();
	float fTop = GetTop();
	float fBottom = GetBottom();
	if (Line (vBase, vTop).DistanceTo (line) > fWidth)
		return false;

	// check if the line's endpoints are over the top or bottom of the VerticalCylinder
	float yBegin = line.vBegin.fComp[1];
	float yEnd = line.vEnd.fComp[1];
	if (yBegin < fBottom && yEnd < fBottom)
		return (SphereToLine (GetBottomPos(), fWidth, line.vBegin, line.vEnd, vIntersection));
	if (yBegin > fTop && yEnd > fTop)
		return SphereToLine (GetTopPos(), fWidth*.25f, line.vBegin, line.vEnd, vIntersection);


	// at this point, we know that the infinite line containing the line segment intersects the VerticalCylinder
	// all we need to do now is compute the intersection and check if it's on the line
	Vector vMaybeIntersection = line.ClosestPoint ( Line (vBase, vTop));
	if (line.ContainsPoint (vMaybeIntersection) && CollidesWith (vMaybeIntersection))
	{
		if (vIntersection)
			*vIntersection = vMaybeIntersection;
		return true;
	}

	return false;
}

bool VerticalCylinder::CollidesWith (Plane& plane, Vector* vIntersection)
{
	// fudge: treat the cylinder as a capsule (spheres coming out of the top and bottom)
	// collision detection will be slightly generous. This is okay, though, since we're only using this to test if a vertical cylinder is inside a frustum

	// check if both endpoints of the cyliner are too far away from the plane
	Vector vTop = GetTopPos();
	Vector vBottom = GetBottomPos();
	float fTopSignedDist = plane.SignedDistance (vTop);
	float fBottomSignedDist = plane.SignedDistance (vBottom);
	if (fTopSignedDist > fWidth && fBottomSignedDist > fWidth)
		return false;
	if (fTopSignedDist < -fWidth && fBottomSignedDist < -fWidth)
		return false;

	// check against the top and bottom spheres
	if (SphereToPlane (GetTopPos(), fWidth*.5f, plane.vNormal, plane.vPoint, vIntersection))
		return true;
	if (SphereToPlane (GetBottomPos(), fWidth, plane.vNormal, plane.vPoint, vIntersection))
		return true;
	// check if the plane collided with the base of the cylinder
	if (plane.CollidesWith (Line (vBottom, vTop), vIntersection))
		return true;

	return false;

}

bool VerticalCylinder::CollidesWith (Triangle& triangle, Vector* vIntersection)
{
	// test the plane on which the triangle lies
	Vector vPlaneIntersect;
	if (!CollidesWith (Plane (triangle.vNormal, triangle.v1), &vPlaneIntersect))
		return false;

	// run 3 half-space tests
	// make sure to leave some slack: if a sphere's intersection with the triangle's plane barely misses the triangle, the sphere probably still intersects the triangle.
	// leaving out the slack could cause objects to fall through the world if they're positioned exactly on the edge of one of the world's triangles!
	Vector vNormal = triangle.vNormal;
	
	// test 1
	Vector vEdge = triangle.v2 - triangle.v1;
	Vector vToLine = (vEdge & vNormal).Normalize();		//the direction of the shortest path from v3 to vEdge
	float fDotCylinder = (vPlaneIntersect - triangle.v1) * vToLine;
	float fDotPoint = (triangle.v3 - triangle.v1) * vToLine;
	if ((SIGN (fDotCylinder) != SIGN (fDotPoint)) && (ABS(fDotCylinder) > fWidth * fWidth * 0.01f))
		return false;

	// test 2
	vEdge = triangle.v3 - triangle.v2;
	vToLine = (vEdge & vNormal).Normalize();		//the direction of the shortest path from v3 to vEdge
	fDotCylinder = (vPlaneIntersect - triangle.v2) * vToLine;
	fDotPoint = (triangle.v1 - triangle.v2) * vToLine;
	if ((SIGN (fDotCylinder) != SIGN (fDotPoint)) && (ABS(fDotCylinder) > fWidth * fWidth * 0.01f))
		return false;

	// test 3
	vEdge = triangle.v1 - triangle.v3;
	vToLine = (vEdge & vNormal).Normalize();		//the direction of the shortest path from v3 to vEdge
	fDotCylinder = (vPlaneIntersect - triangle.v3) * vToLine;
	fDotPoint = (triangle.v2 - triangle.v3) * vToLine;
	if ((SIGN (fDotCylinder) != SIGN (fDotPoint)) && (ABS(fDotCylinder) > fWidth * fWidth * 0.01f))
		return false;

	if (vIntersection)
		*vIntersection = vPlaneIntersect;
	return true;
}

	
bool VerticalCylinder::CollisionClamp (BoundingShape* other)
{
	Vector vIntersection;
	if (!CollidesWith (other, &vIntersection))
		return false;

	//clamp to vertical cylinder
	if (other->iObjectType == SHAPE_VERTICAL_CYLINDER)
	{
		// get penetration depth
		Vector vPos = GetPos();
		Vector vOtherPos =other->GetPos();
		vOtherPos.fComp[1] = GetYPos();
		Vector vDirection = GetPos() - vOtherPos;	//direction in which the object will move away from the other
		float fMagnitude = fWidth + other->fWidth - vDirection.Length();
		MoveBy (vDirection.Normalize() * fMagnitude);
	}
	else return CollisionClamp (vIntersection);

	return true;
}

bool VerticalCylinder::CollisionClamp (Vector& point)
{
	if (!CollidesWith (point))
		return false;


	// if the point collides with the bottom of the cylinder, apply the same behavior as clamping a sphere
	if (SphereToPoint (GetBottomPos(), fWidth, point))
	{
		Vector vToPoint = point - GetBottomPos();
		float fPenetration = fWidth - vToPoint.Length();
		//Vector vMoveBy = vToPoint.Normalize() * (-fPenetration);
		MoveBy (vToPoint.Normalize() * (-fPenetration));
	}

	// if the point collides with the head of the cylinder, apply the same behavior as clamping a sphere
	else if (SphereToPoint (GetTopPos(), fWidth*.5f, point))
	{
		Vector vToPoint = point - GetTopPos();
		float fPenetration = fWidth*.5f - vToPoint.Length();
		//Vector vMoveBy = vToPoint.Normalize() * (-fPenetration);
		MoveBy (vToPoint.Normalize() * (-fPenetration));
	}

	// if the point collides with the central part of the cylinder, do not move the cylinder vertically
	else if (point.fComp[1] <= GetTop() && point.fComp[1] >= GetBottom())
	{
		point.fComp[1] = GetYPos();
		Vector vToPoint = point - GetPos();
		float fPenetration = fWidth - vToPoint.Length();
		//Vector vMoveBy = vToPoint.Normalize() * (-fPenetration);
		MoveBy (vToPoint.Normalize() * (-fPenetration));
	}

	return true;
}

void VerticalCylinder::Draw()
{
	Matrix mDraw = mGlobal;
	mDraw.RotateX(-90);
	Shapes::SetColor(fRed, fGreen, fBlue);
	Shapes::Cylinder (&mDraw, fWidth, fWidth, fHeight, 8, 2);
	mDraw.Translate (0, -fHeight*0.5f, 0);
	Shapes::Sphere (&mDraw, fWidth);
	mDraw.Translate (0, fHeight, 0);
	Shapes::Sphere (&mDraw, fWidth*0.5f);
}

