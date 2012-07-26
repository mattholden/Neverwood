#include "Frustum.h"
#include "BoundingBox.h"
#include "Cylinder.h"
#include "VerticalCylinder.h"
#include "Sphere.h"
#include "Direct3D.h"

Frustum::Frustum() { iObjectType = SHAPE_FRUSTUM; }

Frustum::Frustum (float	angle, float nearClip, float farClip, float aspectRatio)
{
	iObjectType = SHAPE_FRUSTUM;
	Set (angle, nearClip, farClip, aspectRatio);
}

void Frustum::Set (float angle, float nearClip, float farClip, float aspectRatio)
{
	fAngle = angle;
	fNearClip = nearClip;
	fFarClip = farClip;
	fAspectRatio = aspectRatio;

	fNearToFar = fNearClip / fFarClip;
	fInvNearClip = 1 / fNearClip;
	fInvFarClip = 1 / fFarClip;
	fDepth = fFarClip;
	fHeight = 2 * fFarClip * tanf (fAngle * 0.017435f);
	fWidth = fHeight * fAspectRatio;
}

void Frustum::SetAngle (float angle) { Set(angle, fNearClip, fFarClip, fAspectRatio); }
void Frustum::SetNearClip (float nearClip) { Set (fAngle, nearClip, fFarClip, fAspectRatio); }
void Frustum::SetFarClip (float farClip) { Set (fAngle, fNearClip, farClip, fAspectRatio); }
void Frustum::SetAspectRatio (float aspectRatio) { Set (fAngle, fNearClip, fFarClip, aspectRatio); }

Vector Frustum::FarBottomLeft()
{
	Vector vFarCenter = GetPos() + GetZ() * fFarClip;
	float fDeltaY = fHeight * 0.5f;
	float fDeltaX = fWidth * 0.5f;

	return vFarCenter - GetX() * fDeltaX - GetY() * fDeltaY;
}

Vector Frustum::FarBottomRight()
{
	Vector vFarCenter = GetPos() + GetZ() * fFarClip;
	float fDeltaY = fHeight * 0.5f;
	float fDeltaX = fWidth * 0.5f;

	return vFarCenter + GetX() * fDeltaX - GetY() * fDeltaY;
}

Vector Frustum::FarTopLeft()
{
	Vector vFarCenter = GetPos() + GetZ() * fFarClip;
	float fDeltaY = fHeight * 0.5f;
	float fDeltaX = fWidth * 0.5f;

	return vFarCenter - GetX() * fDeltaX + GetY() * fDeltaY;
}

Vector Frustum::FarTopRight()
{
	Vector vFarCenter = GetPos() + GetZ() * fFarClip;
	float fDeltaY = fHeight * 0.5f;
	float fDeltaX = fWidth * 0.5f;

	return vFarCenter + GetX() * fDeltaX + GetY() * fDeltaY;
}

Vector Frustum::NearBottomLeft()
{
	Vector vNearCenter = GetPos() + GetZ() * fNearClip;
	float fDeltaY = fHeight * fNearToFar * 0.5f;
	float fDeltaX = fWidth * fNearToFar * 0.5f;

	return vNearCenter - GetX() * fDeltaX - GetY() * fDeltaY;
}

Vector Frustum::NearBottomRight()
{
	Vector vNearCenter = GetPos() + GetZ() * fNearClip;
	float fDeltaY = fHeight * fNearToFar * 0.5f;
	float fDeltaX = fWidth * fNearToFar * 0.5f;

	return vNearCenter + GetX() * fDeltaX - GetY() * fDeltaY;
}

Vector Frustum::NearTopLeft()
{
	Vector vNearCenter = GetPos() + GetZ() * fNearClip;
	float fDeltaY = fHeight * fNearToFar * 0.5f;
	float fDeltaX = fWidth * fNearToFar * 0.5f;

	return vNearCenter - GetX() * fDeltaX + GetY() * fDeltaY;
}
	
Vector Frustum::NearTopRight()
{
	Vector vNearCenter = GetPos() + GetZ() * fNearClip;
	float fDeltaY = fHeight * fNearToFar * 0.5f;
	float fDeltaX = fWidth * fNearToFar * 0.5f;

	return vNearCenter + GetX() * fDeltaX + GetY() * fDeltaY;
}

Quad Frustum::GetNearPlane() { return Quad (NearBottomLeft(), NearBottomRight(), NearTopRight(), NearTopLeft()); }
Quad Frustum::GetFarPlane() { return Quad (FarBottomRight(), FarBottomLeft(), FarTopLeft(), FarTopRight()); }
Quad Frustum::GetLeftPlane() { return Quad (FarBottomLeft(), NearBottomLeft(), NearTopLeft(), FarTopLeft()); }
Quad Frustum::GetRightPlane() { return Quad (NearBottomRight(), FarBottomRight(), FarTopRight(), NearTopRight()); }
Quad Frustum::GetBottomPlane() { return Quad (FarBottomLeft(), FarBottomRight(), NearBottomRight(), NearBottomLeft()); }
Quad Frustum::GetTopPlane() { return Quad (NearTopLeft(), NearTopRight(), FarTopRight(), FarTopLeft()); }

///////////////////////////////
// COLLISION DETECTION
///////////////////////////////

bool Frustum::CollidesWith (BoundingShape* other, Vector* vIntersection)
{
	switch (other->iObjectType)
	{
	case SHAPE_BOX:
		{
			//this one is particularly important, since it is used to cull geometry
			//it needs to be very accurate and work on large boxes as well as large view frustums
			Vector vOtherPos = other->GetPos();
			Vector vPos = GetPos();

			// do a quick check if the box is past the near/far clipping area
			float fDot = (vOtherPos - vPos) * GetZ();
			if (fDot + (other->fWidth + other->fHeight + other->fDepth)*0.5f < fNearClip)
				return false;
			if (fDot - (other->fWidth + other->fHeight + other->fDepth)*0.5f > fFarClip)
				return false;

			// most of the time collision detection returns true is if the object is inside the frustum, so check this case first
			if (CollidesWith (vOtherPos))
			{
				if (vIntersection)
					*vIntersection = vOtherPos;
				return true;
			}
			if (other->CollidesWith (vPos))
			{
				if (vIntersection)
					*vIntersection = vPos;
				return true;
			}

			// check if any of the box's lines intersect the frustum
			// OPTIMIZE: some lines are being checked twice
			if (CollidesWith ( ((BoundingBox*) other)->GetLeftFace(), vIntersection))
				return true;
			if (CollidesWith ( ((BoundingBox*) other)->GetRightFace(), vIntersection))
				return true;
			if (CollidesWith ( ((BoundingBox*) other)->GetTopFace(), vIntersection))
				return true;
			if (CollidesWith ( ((BoundingBox*) other)->GetBottomFace(), vIntersection))
				return true;

			// check if any of the view frustum's lines intersect the box
			// OPTIMIZE: some lines are being checked twice
			if (other->CollidesWith (GetLeftPlane(), vIntersection))
				return true;
			if (other->CollidesWith (GetRightPlane(), vIntersection))
				return true;
			if (other->CollidesWith (GetTopPlane(), vIntersection))
				return true;
			if (other->CollidesWith (GetBottomPlane(), vIntersection))
				return true;
	
		return false;
		}
		break;
	case SHAPE_CYLINDER:
		{
		//cylinders don't collide properly with large quads like the view frustum's boundaries, so we have to fudge. A lot.
		Vector vCenter = ((Cylinder*)other)->GetCenter();
		if (!CollidesWith (vCenter))
			return false;
		if (vIntersection)
			*vIntersection = vCenter;
		return true;
		}
		break;

	case SHAPE_VERTICAL_CYLINDER:
		{
			if (GetAsyncKeyState ('D'))
				int blah = 0;

			// do a quick check if the cylinder is past the near/far clipping area
			Vector vOtherPos = other->GetPos();
			float fDot = (vOtherPos - GetPos()) * GetZ();
			if (fDot < fNearClip)
				return false;
			if (fDot > fFarClip)
				return false;

			// CHANGED
			// added this optimization
			// it applies because the view frustum never rotates around its z axis


			// most of the time collision detection returns true is if the object's center is inside the frustum, so check this case first
			if (CollidesWith (vOtherPos))
			{
				if (vIntersection)
					*vIntersection = vOtherPos;
				return true;
			}

			// now, do a brute-force check against all 6 sides of the view frustum
			if (other->CollidesWith (GetLeftPlane(), vIntersection))
				return true;
			if (other->CollidesWith (GetRightPlane(), vIntersection))
				return true;
			if (other->CollidesWith (GetTopPlane(), vIntersection))
				return true;
			if (other->CollidesWith (GetBottomPlane(), vIntersection))
				return true;
			if (other->CollidesWith (GetFarPlane(), vIntersection))
				return true;
			if (other->CollidesWith (GetNearPlane(), vIntersection))
				return true;

			// CHANGED
			// removed this because it's a bit anal. Frustum to cylinder should be really fast because we use it a lot

			// lastly, check against the cylinder's head
			//Vector vOtherTop = ((VerticalCylinder*)other)->GetTopPos();
			//Sphere head (vOtherTop, other->fWidth*.5f);
			//return CollidesWith (&head, vIntersection);			
		}
	
	case SHAPE_SPHERE:
		{
		//OPTIMIZE: just check against the sphere's center. Since view frustums are large, this might be good enough

		// do a QUICK check if the sphere's projection onto the z axis is too close or too far away
		float fDot = (other->GetPos() - GetPos()) * GetZ();
		if (fDot < fNearClip - other->fWidth || fDot > fFarClip + other->fWidth)
			return false;

		// check if the objects are remotely close, since the rest of the algorithm can get expensive
		float fDistance = ~(other->GetPos() - GetPos());
		if (fDistance > fWidth + fHeight + fDepth + other->fWidth + other->fHeight + other->fDepth)
			return false;

		// most of the time a collision occurs, it is because the object is completely inside the frustum.
		// thus, it is efficient to check for this case first
		if (CollidesWith (other->GetPos()))
			return true;

		// check against the other sides
		// OPTIMIZE: only need to check against the plane containing the side
		if (other->CollidesWith (GetFarPlane(), vIntersection))
			return true;
		if (other->CollidesWith (GetNearPlane(), vIntersection))
			return true;
		if (other->CollidesWith (GetLeftPlane(), vIntersection))
			return true;
		if (other->CollidesWith (GetRightPlane(), vIntersection))
			return true;
		if (other->CollidesWith (GetTopPlane(), vIntersection))
			return true;
		if (other->CollidesWith (GetBottomPlane(), vIntersection))
			return true;
		
		return false;
		}//sphere
		break;
		//OPTIMIZE: instead of checking against all the quads, check if key points of the other shape are inside the frustum
	default:
		{
		// since this can get very expensive, check if the two shapes are even close to each other
		float fDistance = ~(other->GetPos() - GetPos());
		if (fDistance > fWidth + fHeight + fDepth + other->fWidth + other->fHeight + other->fDepth)
			return false;

		// most of the time collision detection returns true is if the object is completely inside the frustum, so check this case first
		if (CollidesWith (other->GetPos()))
			return true;

		// in the rare case that an object is near the boundaries of the frustum, do an expensive brute-force check against the quads composing the frustum
		if (other->CollidesWith (GetLeftPlane()))
			return true;
		if (other->CollidesWith (GetRightPlane()))
			return true;
		if (other->CollidesWith (GetTopPlane()))
			return true;
		if (other->CollidesWith (GetBottomPlane()))
			return true;
		if (other->CollidesWith (GetNearPlane()))
			return true;
		if (other->CollidesWith (GetFarPlane()))
			return true;
		
		return false;
		}//default
	}//switch (object type)
}//CollidesWith (shape)

bool Frustum::CollidesWith (Vector& vPoint)
{
	//check if the point is too close or too far away
	Vector vToPoint = vPoint - GetPos();
	float zDot = vToPoint * GetZ();
	if (zDot < fNearClip || zDot > fFarClip)
		return false;

	//OPTIMIZATION: could do half-space tests against planes  instead
	float  xDist = ABS (vToPoint * GetX());
	float  yDist = ABS (vToPoint * GetY());
	float  angleTan = fHeight / (2 * fFarClip);

	// check if the point is too far up or down
	if (yDist / zDot > angleTan)
		return false;

	// check if teh point is too far to the left or right
	if (xDist / zDot > angleTan * fAspectRatio)
		return false;

	return true;
}

bool Frustum::CollidesWith (Line& line, Vector* vIntersection)
{
	//check if one of the points has the right distance down the z axis
	Vector vToBegin = line.vBegin - GetPos();
	float beginDist = vToBegin * GetZ();
	Vector vToEnd = line.vEnd - GetPos();
	float endDist = vToEnd * GetZ();
	if (beginDist < fNearClip && endDist < fNearClip)
		return false;
	if (beginDist > fFarClip && endDist > fFarClip)
		return false;

	//now do 6 line to quad checks
	if (GetLeftPlane().CollidesWith (line, vIntersection))
		return true;
	if (GetRightPlane().CollidesWith (line, vIntersection))
		return true;
	if (GetNearPlane().CollidesWith (line, vIntersection))
		return true;
	if (GetFarPlane().CollidesWith (line, vIntersection))
		return true;
	if (GetTopPlane().CollidesWith (line, vIntersection))
		return true;
	if (GetBottomPlane().CollidesWith (line, vIntersection))
		return true;
	return false;
}

bool Frustum::CollidesWith (Triangle& triangle, Vector* vIntersection)
{
	if (CollidesWith (Line (triangle.v1, triangle.v2), vIntersection))
		return true;
	if (CollidesWith (Line (triangle.v2, triangle.v3), vIntersection))
		return true;
	if (CollidesWith (Line (triangle.v3, triangle.v1), vIntersection))
		return true;
	return false;
}

bool Frustum::CollidesWith (Quad& quad, Vector* vIntersection)
{
	if (CollidesWith (Line (quad.v1, quad.v2), vIntersection))
		return true;
	if (CollidesWith (Line (quad.v2, quad.v3), vIntersection))
		return true;
	if (CollidesWith (Line (quad.v3, quad.v4), vIntersection))
		return true;
	if (CollidesWith (Line (quad.v4, quad.v1), vIntersection))
		return true;
	return false;
}

bool Frustum::CollidesWithVerticalLine (Vector& vPoint, Vector* vIntersection)
{
	//check if the point has the right distance down the z axis
	Vector vToPoint = vPoint - GetPos();
	float zDot = vToPoint * GetZ();
	if (zDot < fNearClip || zDot > fFarClip)
		return false;

	// OPTIMIZE: run some additional checks here to return false quickly

	//now check against the top quad
	Quad qTop = GetTopPlane();
	Vector vNormal = ((qTop.v2 - qTop.v1) & (qTop.v3 - qTop.v1)).Normalize();
	
	// get the intersection with the plane containing the top quad
	Vector vPlaneIntersect;
	Plane (vNormal, qTop.v1).CollidesWith (Line (vPoint, vPoint + Vector(0,1,0)), &vPlaneIntersect);
	
	// check if the plane intersection is inside the quad
	// OPTIMIZE: do this manually to avoid recomputing the quad's normal
	if (qTop.CollidesWith (vPlaneIntersect))
	{
		if (vIntersection)
			*vIntersection = vPlaneIntersect;
		return true;
	}
	return false;
}

void Frustum::Draw()
{
	// CREATE VERTICES FOR D3D
	/*
	struct Vertex
	{
		float x,y,z;
	};
	Vertex vVerts[18];

	memcpy (vVerts, NearBottomLeft().fComp, 12);
	memcpy (vVerts+1, NearBottomRight().fComp, 12);
	memcpy (vVerts+2, FarBottomRight().fComp, 12);
	memcpy (vVerts+3, FarBottomLeft().fComp, 12);
	memcpy (vVerts+4, NearBottomLeft().fComp, 12);
	memcpy (vVerts+5, FarBottomLeft().fComp, 12);
	memcpy (vVerts+6, FarTopLeft().fComp, 12);
	memcpy (vVerts+7, NearTopLeft().fComp, 12);
	memcpy (vVerts+8, NearBottomLeft().fComp, 12);
	memcpy (vVerts+9, NearTopLeft().fComp, 12);
	memcpy (vVerts+10, FarTopLeft().fComp, 12);
	memcpy (vVerts+11, FarTopRight().fComp, 12);
	memcpy (vVerts+12, NearTopRight().fComp, 12);
	memcpy (vVerts+13, NearTopLeft().fComp, 12);
	memcpy (vVerts+14, NearTopRight().fComp, 12);
	memcpy (vVerts+15, NearBottomRight().fComp, 12);
	memcpy (vVerts+16, FarBottomRight().fComp, 12);
	memcpy (vVerts+17, FarTopRight().fComp, 12);
	*/
	float w = fWidth * 0.5f;
	float h = fHeight * 0.5f;

	Vector vVerts[16];
	vVerts[0] = Vector (-w*fNearToFar, -h*fNearToFar, fNearClip);
	vVerts[2] = Vector (w*fNearToFar, -h*fNearToFar, fNearClip);
	vVerts[4] = Vector (w*fNearToFar, h*fNearToFar, fNearClip);
	vVerts[6] = Vector (-w*fNearToFar, h*fNearToFar, fNearClip);
	vVerts[1] = Vector (-w, -h, fFarClip);
	vVerts[3] = Vector (w, -h, fFarClip);
	vVerts[5] = Vector (w, h, fFarClip);
	vVerts[7] = Vector (-w, h, fFarClip);
	vVerts[8] = vVerts[1];
	vVerts[9] = vVerts[3];
	vVerts[10] = vVerts[3];
	vVerts[11] = vVerts[5];
	vVerts[12] = vVerts[5];
	vVerts[13] = vVerts[7];
	vVerts[14] = vVerts[7];
	vVerts[15] = vVerts[1];	

	// DRAW 'EM
	Direct3D::SetColor (fRed, fGreen, fBlue);
	Direct3D::GetDevice()->SetFVF (D3DFVF_XYZ);
	Direct3D::GetMatrixStack()->Push();
	//Matrix mIdentity;
	//Direct3D::GetMatrixStack()->LoadMatrixClass(mIdentity);
	Direct3D::GetMatrixStack()->LoadMatrixClass(mGlobal);
	Direct3D::GetMatrixStack()->ApplyToWorld();
	Direct3D::GetDevice()->DrawPrimitiveUP (D3DPT_LINELIST, 8, vVerts, sizeof (Vector));
	Direct3D::GetMatrixStack()->Pop();
}