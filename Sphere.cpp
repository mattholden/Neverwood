#include "Sphere.h"
#include "VerticalCylinder.h"
#include "Direct3D.h"
#include "Shapes.h"

bool Sphere::CollidesWith (BoundingShape* other, Vector* vIntersection)
{
	switch (other->iObjectType)
	{
		// Sphere to Sphere 
		case SHAPE_SPHERE:
			return SphereToSphere (GetPos(), fWidth, other->GetPos(), other->fWidth, vIntersection);
			break;

		// Sphere to Cylinder
		case SHAPE_CYLINDER:
			{
				// check if the sphere is close enough to the cylinder's axis
				Vector vThisPos = GetPos();				//store for the sake of optimization
				Vector vOtherPos = other->GetPos();
			
				Line cylinderAxis = Line (vOtherPos, vOtherPos + other->GetY());
				Vector vClosestPoint = cylinderAxis.ClosestPoint (vThisPos);	//the closest point on the infinite line containing the cylinder's axis
				if ( (vThisPos - vClosestPoint).LengthSquared() > (fWidth + other->fWidth) * (fWidth + other->fWidth))
					return false;

				// check if the sphere is too high or too low
				float fDot = (vThisPos - vOtherPos) * other->GetY();
				if (fDot < -fWidth || fDot > other->fHeight + fWidth)
					return false;

				// compute the intersection
				if (vIntersection)
				{
					//special case: sphere is on the cylinder's axis
					if (vThisPos == vClosestPoint)
						*vIntersection = vClosestPoint;
					//general case
					else *vIntersection = vClosestPoint + (vThisPos - vClosestPoint).Normalize() * other->fWidth;
				}

				return true;
			}
			break;

		case SHAPE_VERTICAL_CYLINDER:
			{
				// check if you're close enough horizontally
				Vector vToOther = other->GetPos() - GetPos();
				vToOther.fComp[1] = 0;
				if (vToOther.LengthSquared() > (fWidth + other->fWidth) * (fWidth + other->fWidth))
					return false;

				// check if you're close enough vertically
				float fY = GetYPos();
				float fTop = ((VerticalCylinder*)other)->GetTop();
				if (fY > fTop + other->fWidth + fWidth)
					return false;
				float fBottom = ((VerticalCylinder*)other)->GetBottom();
				if (fY < fBottom - other->fWidth - fWidth)
					return false;

				// collided with the base of the cylinder
				if (fY < fTop && fY > fBottom)
				{
					if (vIntersection)
					{
						//return the point on the outside of the cylinder that is closest to the sphere
						Vector vAxisPoint = other->GetPos();		//eventually, this will be the point on the cylinder's axis closest to the sphere
						vAxisPoint.fComp[1] = GetYPos();

						Vector vToSphere = GetPos() - vAxisPoint;
						*vIntersection = vAxisPoint + vToSphere.Normalize() * other->fWidth;
					}
					return true;
				}
				
				// collided with the top of the cylinder
				if (SphereToSphere (GetPos(), fWidth, ((VerticalCylinder*)other)->GetTopPos(), other->fWidth*.5f, vIntersection))
					return true;
				// collided with the bottom of the cylinder
				if (SphereToSphere (GetPos(), fWidth, ((VerticalCylinder*)other)->GetBottomPos(), other->fWidth*.5f, vIntersection))
					return true;
				
				return false;				
			}
			break;

		// Sphere to AABB
		case SHAPE_BOX:
			{
				//sphere is too far to the right
				if (GetXPos() - fWidth > other->GetXPos() + other->fWidth * 0.5f)
					return false;
				
				//sphere is too far to the left
				if (GetXPos() + fWidth < other->GetXPos() - other->fWidth * 0.5f)
					return false;
					
				//sphere is too far to the back
				if (GetZPos() + fWidth < other->GetZPos() - other->fDepth * 0.5f)
					return false;
				
				//sphere is too far to the front
				if (GetZPos() - fWidth > other->GetZPos() + other->fDepth * 0.5f)
					return false;
								
				//sphere is too high
				if (GetYPos() - fWidth > other->GetYPos() + other->fHeight * 0.5f)
					return false;

				//sphere is too low
				if (GetYPos() + fWidth < other->GetYPos() - other->fHeight * 0.5f)
					return false;

				//passed all the tests, so collision detection returns true (slight approximation here)
				if (vIntersection)
				{
					//do a sphere to plane test against all the box's planes, and return the first one that is true
					
					// right face
					Vector vPos = other->GetPos();
					Vector vTemp = vPos + Vector (0.5f * other->fWidth, 0.0f, 0.0f);	//a point on the right face of the box
					if (SphereToPlane (GetPos(), fWidth, Vector (1.0f, 0.0f, 0.0f), vTemp, vIntersection))
						return true;

					// left face
					vTemp = vPos - Vector (0.5f * other->fWidth, 0.0f, 0.0f);	//a point on the right face of the box
					if (SphereToPlane (GetPos(), fWidth, Vector (-1.0f, 0.0f, 0.0f), vTemp, vIntersection))
						return true;

					// top face
					vTemp = other->GetPos() + Vector (0.0f, 0.5f * other->fHeight, 0.0f);	//a point on the right face of the box
					if (SphereToPlane (GetPos(), fWidth, Vector (0.0f, 1.0f, 0.0f), vTemp, vIntersection))
						return true;

					// bottom face
					vTemp = other->GetPos() - Vector (0.0f, 0.5f * other->fHeight, 0.0f);	//a point on the right face of the box
					if (SphereToPlane (GetPos(), fWidth, Vector (0.0f, 1.0f, 0.0f), vTemp, vIntersection))
						return true;

					// front face
					vTemp = other->GetPos() + Vector (0.0f, 0.0f, 0.5f * other->fDepth);	//a point on the right face of the box
					if (SphereToPlane (GetPos(), fWidth, Vector (0.0f, 0.0f, 1.0f), vTemp, vIntersection))
						return true;

					// back face
					vTemp = other->GetPos() - Vector (0.0f, 0.0f, 0.5f * other->fDepth);	//a point on the right face of the box
					if (SphereToPlane (GetPos(), fWidth, Vector (0.0f, 0.0f, -1.0f), vTemp, vIntersection))
						return true;

					// by this point, the sphere is INSIDE the box, so the most sensible point of collision would be the sphere's center
					*vIntersection = GetPos();
				}	// if (intersection)
				return true;	//in this case, the sphere is INSIDE the box, so collision response will fail!
								//still, in the case of projectiles, this is not an issue. Anything that is not a projectile shouldn't be fast enough to get to this point.
			} //box intersection
			break;
		case SHAPE_FRUSTUM:
			return other->CollidesWith (this, vIntersection);
			break;
		default:
			return false;
	}//switch(shape)
}

bool Sphere::CollidesWith (Plane& plane, Vector* vIntersection)
{
	return SphereToPlane (GetPos(), fWidth, plane.vNormal, plane.vPoint, vIntersection);
}

bool Sphere::CollidesWith (Vector& vPoint)
{
	return ( (vPoint - GetPos()).LengthSquared() <= fWidth * fWidth) ;
}

bool Sphere::CollidesWith (Line& line, Vector* vIntersection)
{
	return SphereToLine (GetPos(), fWidth, line.vBegin, line.vEnd, vIntersection);
}


bool Sphere::CollidesWith (Triangle& triangle, Vector* vIntersection)
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
	float fDotSphere = (vPlaneIntersect - triangle.v1) * vToLine;
	float fDotPoint = (triangle.v3 - triangle.v1) * vToLine;
	if ((SIGN (fDotSphere) != SIGN (fDotPoint)) && (ABS(fDotSphere) > fWidth * fWidth * 0.01f))
		return false;

	// test 2
	vEdge = triangle.v3 - triangle.v2;
	vToLine = (vEdge & vNormal).Normalize();		//the direction of the shortest path from v3 to vEdge
	fDotSphere = (vPlaneIntersect - triangle.v2) * vToLine;
	fDotPoint = (triangle.v1 - triangle.v2) * vToLine;
	if ((SIGN (fDotSphere) != SIGN (fDotPoint)) && (ABS(fDotSphere) > fWidth * fWidth * 0.01f))
		return false;

	// test 3
	vEdge = triangle.v1 - triangle.v3;
	vToLine = (vEdge & vNormal).Normalize();		//the direction of the shortest path from v3 to vEdge
	fDotSphere = (vPlaneIntersect - triangle.v3) * vToLine;
	fDotPoint = (triangle.v2 - triangle.v3) * vToLine;
	if ((SIGN (fDotSphere) != SIGN (fDotPoint)) && (ABS(fDotSphere) > fWidth * fWidth * 0.01f))
		return false;

	if (vIntersection)
		*vIntersection = vPlaneIntersect;
	return true;
}


bool Sphere::CollidesWith (Quad& quad, Vector* vIntersection)
{
	// first, do collision against the plane containing the quad
	Vector vNormal = ( (quad.v2 - quad.v1) & (quad.v3 - quad.v1) ).Normalize();
	Vector vPlaneIntersection;
	if (!SphereToPlane (GetPos(), fWidth, vNormal, quad.v1, &vPlaneIntersection))
		return false;

	//check if the point of collision is on the quad
	// also make damned sure there are no false negatives
	if (!quad.CollidesWith (vPlaneIntersection))
	{
		if (CollidesWith (Line (quad.v2, quad.v1), vIntersection))
			return true;
		if (CollidesWith (Line (quad.v3, quad.v2), vIntersection))
			return true;
		if (CollidesWith (Line (quad.v4, quad.v3), vIntersection))
			return true;
		if (CollidesWith (Line (quad.v1, quad.v4), vIntersection))
			return true;
		return false;
	}

	if (vIntersection)
		*vIntersection = vPlaneIntersection;
	return true;
}

bool Sphere::CollisionClamp (BoundingShape* other)
{
	// compute collision point
	Vector vIntersection;
	if (!CollidesWith (other, &vIntersection))
		return false;

	// resolve collision

	// sphere to sphere / cylinder
	// find how deep the sphere is into the other
	Vector vToIntersection = vIntersection - GetPos();
	float fPenetration = fWidth - vToIntersection.Length();
	// move it away by that much
	//Vector vMoveBy = vToIntersection.Normalize() * (-fPenetration);
	MoveBy (vToIntersection.Normalize() * (-fPenetration));

	return true;
}

bool Sphere::CollisionClamp (Vector& point)
{
	if (!CollidesWith (point))
		return false;

	Vector vToPoint = point - GetPos();
	float fPenetration = fWidth - vToPoint.Length();
	// move it away by that much
	//Vector vMoveBy = vToPoint.Normalize() * (-fPenetration);
	MoveBy (vToPoint.Normalize() * (-fPenetration));
	
	return true;
}


void Sphere::Draw()
{
	Shapes::SetColor(fRed, fGreen, fBlue);
	Shapes::Sphere(&mGlobal, fWidth);
}