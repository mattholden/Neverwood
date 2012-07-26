#include "Cylinder.h"
#include "Shapes.h"

Cylinder::Cylinder() { iObjectType = SHAPE_CYLINDER; }

Cylinder::Cylinder (float height, float radius)
{
	iObjectType = SHAPE_CYLINDER;
	fHeight = height;
	fWidth = fDepth = radius;
	mGlobal.fMatrix[12] = mGlobal.fMatrix[13] = mGlobal.fMatrix[14] = 0;
}

Cylinder::Cylinder (Vector& base, float height, float radius)
{
	iObjectType = SHAPE_CYLINDER;
	MoveTo (base);
	fHeight = height;
	fWidth = fDepth = radius;


	//note: don't need to set the orientation, since this constructor defaults to the identity matrix
}

Cylinder::Cylinder (Vector& base, Vector& top, float radius)
{
	iObjectType = SHAPE_CYLINDER;
	MoveTo (base);
	Vector vMajor = top - base;		//major axis: connects the two ends of the cylinder
	fHeight = vMajor.Length();
	fWidth = fDepth = radius;
	vMajor.NormalizeAndStore();

	//set the y axis
	memcpy (mGlobal.fMatrix + 4, vMajor.fComp, 12);

	//arbitrarily set the x axis to have a y coordinate of 0
	Vector xAxis;
	xAxis.fComp[0] = vMajor.fComp[2];
	xAxis.fComp[1] = 0;
	xAxis.fComp[2] = -vMajor.fComp[0];
	

	//special case: if the height of the cylinder just happens to be down the y axis, use the default x axis 
	if (xAxis == Vector (0.0f, 0.0f, 0.0f))
		xAxis = Vector (1.0f, 0.0f, 0.0f);
	
	Vector zAxis = xAxis & vMajor;

	//copy the obtained data
	memcpy (mGlobal.fMatrix, xAxis.fComp, 12);
	memcpy (mGlobal.fMatrix + 8, zAxis.fComp, 12);
}

Vector Cylinder::GetBottom() { return GetPos(); }
Vector Cylinder::GetCenter() { return GetPos() + GetY() * (fHeight * 0.5f); }
Vector Cylinder::GetTop()	{ return GetPos() + GetY() * fHeight; }

bool Cylinder::CollidesWith (BoundingShape* other, Vector* vIntersection)
{
	switch (other->iObjectType)
	{
		//Cylinder to Sphere
	case SHAPE_SPHERE:
		return other->CollidesWith (this, vIntersection);
		break;
		
		//Cylinder to Cylinder
	case SHAPE_CYLINDER:
		{
			// first, treat the cylinders as infinitely long.
			// compute the two points that are closest to each other on the axes of the infinitely long cylinders
			Vector vBottom = GetBottom();
			Vector vTop = GetTop();
			Vector vOtherBottom = ((Cylinder*)other)->GetBottom();
			Vector vOtherTop = ((Cylinder*)other)->GetTop();
			Vector vClosestThis = (Line (vBottom, vTop).ClosestPoint (Line (vOtherBottom, vOtherTop)));		//the point on the infinite line containing this cylinder's axis that is closest to the other cylinder
			Vector vClosestOther = (Line (vOtherBottom, vOtherTop).ClosestPoint (Line (vBottom, vTop)));	//the point on the infinite line containing the other cylinder's axis that is closest to this cylinder
			
			// check if the two closest points are close enough to each other
			if ( (vClosestOther - vClosestThis).LengthSquared() > (fWidth + other->fWidth) * (fWidth + other->fWidth))
				return false;
			
			// check if vClosestThis is inside the cylinder
			// OPTIMIZE: take advantage of the fact that vClosestThis lies on the axis of the cylinder
			if (CollidesWith (vClosestThis) == false)
				return false;
			if (other->CollidesWith (vClosestOther) == false)
				return false;

			// the cylinders collide, so return the intersection
			if (vIntersection)
				*vIntersection = (vClosestThis + vClosestOther) * 0.5f;
			return true;
		}//cylinder
		break;

		//Cylinder To AABB
	case SHAPE_BOX:		
		return other->CollidesWith (this, vIntersection);
		break;
	case SHAPE_FRUSTUM:
		return other->CollidesWith (this, vIntersection);
		break;

	default:
		return false;
	}
}

bool Cylinder::CollidesWith (Vector& vPoint)
{
	// check if the point is close enough to the axis
	Vector vBase = GetBottom();
	Vector vTop = GetTop();
	if (Line (vBase, vTop).DistanceTo (vPoint) > fWidth)
		return false;

	// check if the point is beyond the beginning or end point of the sphere's axis
	float fDot = (vPoint - vBase) * GetY();
	return ( fDot >= 0 && fDot <= fHeight );
}

//////////////////////////////////////////////////
// Cylinder to Line collision
// The point of intersection is slightly inaccurate:
// it returns the point on the line that is closest to the cylinder
// also, it returns this point regardless of whether the two objects intersected
///////////////////////////////////////////////////
bool Cylinder::CollidesWith (Line& line, Vector* vIntersection)
{
	// check if the line is close enough to the axis
	Vector vBase = GetBottom();
	Vector yAxis = GetY();
	Vector vTop = GetTop();
	if (Line (vBase, vTop).DistanceTo (line) > fWidth)
		return false;

	// check if the line's endpoints are over the top or bottom of the cylinder
	float fDotBegin = (line.vBegin - vBase) * yAxis;
	float fDotEnd = (line.vEnd - vBase) * yAxis;
	if (fDotBegin < 0 && fDotEnd < 0)
		return false;
	if (fDotBegin > fHeight && fDotEnd > fHeight)
		return false;

	// at this point, we know that the infinite line containing the line segment intersects the cylinder
	// all we need to do now is compute the intersection and check if it's on the line
	Vector vMaybeIntersection = line.ClosestPoint ( Line (vBase, vTop));
	if (line.ContainsPoint (vMaybeIntersection))
	{
		if (vIntersection)
			*vIntersection = vMaybeIntersection;
		return true;
	}
	return false;

}

bool Cylinder::CollidesWith (Plane& plane, Vector* vIntersection)
{
	// CAUTION: the following code has not been debugged, since I have chosen
	// never to use cylinder to plane collision (too expensive, as you can see by the amount of code in here)

	/*
	Vector yAxis = GetY();
	Vector vPos = GetPos();

	//special case: cylinder's axis is parallel to the plane

	if ( FloatEqual (yAxis * plane.vNormal, 0.0f) )
	{
		float fSignedDist = plane.SignedDistance(vPos);
		if ( ABS(fSignedDist) > fWidth)
			return false;
		//collision detection succeeded
		if (vIntersection)
			*vIntersection = vPos - plane.vNormal * fSignedDist;
		return true;
	}

	//special case: cylinder's axis is perpendicular to the plane
	else if ( (yAxis & plane.vNormal) == Vector(0,0,0))
		return plane.CollidesWith ( Line (vPos, vPos + yAxis * fHeight), vIntersection);


	//GENERAL CASE
	
	//if the cylinder's axis collides with the plane, use that intersection
	if (plane.CollidesWith ( Line (vPos, vPos + yAxis), vIntersection))
		return true;
	
	//find the endpoint of the cylinder that is closest to the plane
	float fSignedDistBase = plane.SignedDistance(vPos);
	Vector vTop = vPos + yAxis * fHeight;
	float fSignedDistTop = plane.SignedDistance(vTop);

	//first, do a generous collision test:
	//test if the endpoints of the cylinder are close enough to the plane for collision to be possible
	//to visualize, imagine attaching spheres to the endpoints of the cylinder
	Vector vMaybeIntersection;
	Vector vCloserPoint;			//stores the bottom or top of the cylinder, whichever is closer
	float fCloserSignedDist;		//stores the signed distance of the plane to the top or bottom cylinder, whichever is closer
	if (ABS (fSignedDistBase) < ABS (fSignedDistTop))
	{
		vCloserPoint = vPos;
		fCloserSignedDist = fSignedDistBase;
	}
	else 
	{
		vCloserPoint = vTop;
		fCloserSignedDist = fSignedDistTop;
	}

	if (! SphereToPlane (vCloserPoint, fWidth, plane.vNormal, plane.vPoint, &vMaybeIntersection))
		return false;

	//the generous sphere to plane collision passed, so now it's time for the REAL algorithm
	//brace yourself, this won't be pretty
	//the main goal is to find the closest point X on the intersection between the plane and the cylinder's top/bottom plane (whichever is closer)

	//first, find the point on the plane that is closest to the cylinder's endpoint
	Vector vClosestPoint = vCloserPoint - plane.vNormal * fCloserSignedDist;

	//next, find the intersection of the plane with the infinite line containing the cylinder's axis
	Vector vPlaneIntersection;
	plane.CollidesWith (Line (vPos, vPos + yAxis), &vPlaneIntersection);

	//next, find the distance between the two points computed above
	float fDist = ~(vCloserPoint - vPlaneIntersection);

	//from the above data, magically derive the distance of the mysterious point X from the closest point
	float xDist = fCloserSignedDist * fCloserSignedDist / fDist;

	//somehow, I know that X is on the line containing vClosestPoint and vPlaneIntersection
	//since I also know its distance, I can finally derive X
	Vector vIntersectionToClosest = (vClosestPoint - vPlaneIntersection).Normalize();	//unit length vector pointing from the plane intersection to the plane's closest point. Hence the name
	Vector vX = vPlaneIntersection + vIntersectionToClosest * (fDist + xDist); 

	//now for the easy part: check if X is close enough to the cylinder's endpoint
	if ( (~(vCloserPoint - vX)) > fWidth)
		return false;

	//collision passed
	if (vIntersection)
		*vIntersection = vX;
	return true;
	*/
	return false;
}

bool Cylinder::CollidesWith (Triangle& triangle, Vector* vIntersection)
{
	// just check against all the triangle's edges. This method works well,
	// ASSUMING THE TRIANGLE IS SMALL. Since triangles shall be used for world and possibly character model data, this is a fair assumption
	// OPTIMIZE: might need to run additional checks in case the triangle is completely inside the cylinder
	if (CollidesWith (Line (triangle.v1, triangle.v2), vIntersection))
		return true;
	if (CollidesWith (Line (triangle.v2, triangle.v3), vIntersection))
		return true;
	if (CollidesWith (Line (triangle.v3, triangle.v1), vIntersection))
		return true;
	return false;
}

bool Cylinder::CollidesWith (Quad& quad, Vector* vIntersection)
{
	// oversimplification: just check against all the quad's edges
	// CAUTION: this algorithm fails for very large quads (quads that span all the way around the cylinder)
	if (CollidesWith (Line (quad.v1, quad.v2), vIntersection))
		return true;
	if (CollidesWith (Line (quad.v2, quad.v3), vIntersection))
		return true;
	if (CollidesWith (Line (quad.v3, quad.v4), vIntersection))
		return true;
	if (CollidesWith (Line (quad.v4, quad.v1), vIntersection))
		return true;
	return false;

	/*
	//first, check if the cylinder collides with the plane containing the quad
	Vector vPlaneIntersection;
	if (!CollidesWith (Plane (vNormal, quad.v1), &vPlaneIntersection))
		return false;

	Vector vEdge1 = quad.v2 - quad.v1;
	Vector vEdge2 = quad.v3 - quad.v2;
	Vector vNormal = (vEdge1 & vEdge2).Normalize();

	Vector yAxis = GetY();
	//specail case: if the cylinder's axis is parallel to the quad, plane intersection is not reliable (doesn't compute a sensible intersection point)
	//in this case, we need to set a sensible collision point manually
	if ( FloatEqual (yAxis * vNormal, 0.0f))
	{
		// first, check if the quad is close enough to the cylinder's axis
		// this approach is inaccurate, though
		// Q: How the fuck do you do this for large (oh, say, view frustum sized) quads??
		Vector vBase = GetPos();
		Vector vBaseToQuad = quad.v1 - vBase;
		Vector vProj = vBaseToQuad % yAxis;
		Vector vClosestPoint = vBase + vProj;		//the point on the cylinder's axis that is closest to the quad
		float fDistanceFromAxis = ~(vProj - vBaseToQuad);
		if (fDistanceFromAxis > fWidth)
			return false;
		
		// if all the quad's points are above or below the cylinder, it does not collide
		float fDist1 = (quad.v1 - vBase) * yAxis; 
		float fDist2 = (quad.v2 - vBase) * yAxis; 
		float fDist3 = (quad.v3 - vBase) * yAxis; 
		float fDist4 = (quad.v4 - vBase) * yAxis; 

		if (fDist1 < 0 && fDist2 < 0 && fDist3 < 0 && fDist4 < 0)
			return false;
		if (fDist1 > fHeight && fDist2 > fHeight && fDist3 > fHeight && fDist4 > fHeight)
			return false;

		//collision detection occured. Return the center of the quad
		if (vIntersection)
			*vIntersection = (quad.v1 + quad.v2 + quad.v3 + quad.v4) * 0.25f;
		return true;
	}

	//then check if the plane's intersection point is on the quad
	if (!quad.CollidesWith (vPlaneIntersection))
		return false;

	// collision detection successful
	if (vIntersection)
		*vIntersection = vPlaneIntersection;
	return true;
	*/
}
	
bool Cylinder::CollisionClamp (BoundingShape* other)
{
	Vector vIntersection;
	if (!CollidesWith (other, &vIntersection))
		return false;

	return true;
}
void Cylinder::Draw()
{
	Matrix mDraw = mGlobal;
	mDraw.RotateX(-90);
	Shapes::SetColor(fRed, fGreen, fBlue);
	Shapes::Cylinder(&mDraw, fWidth, fWidth, fHeight);
}

