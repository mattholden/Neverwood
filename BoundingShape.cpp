#include "BoundingShape.h"

// translates the object by a given amount
void BoundingShape::MoveBy	(Vector& vOffset) { mGlobal.Translate (vOffset); }
void BoundingShape::MoveForward (float amount) 
{
	mGlobal.fMatrix[12] += amount * mGlobal.fMatrix[8];
	mGlobal.fMatrix[13] += amount * mGlobal.fMatrix[9];
	mGlobal.fMatrix[14] += amount * mGlobal.fMatrix[10];
}
void BoundingShape::MoveRight (float amount) 
{
	mGlobal.fMatrix[12] += amount * mGlobal.fMatrix[0];
	mGlobal.fMatrix[13] += amount * mGlobal.fMatrix[1];
	mGlobal.fMatrix[14] += amount * mGlobal.fMatrix[2];
}
void BoundingShape::MoveUp (float amount) 
{
	mGlobal.fMatrix[12] += amount * mGlobal.fMatrix[4];
	mGlobal.fMatrix[13] += amount * mGlobal.fMatrix[5];
	mGlobal.fMatrix[14] += amount * mGlobal.fMatrix[6];
}
	
// rotate around the world's axes
void BoundingShape::RotateX (float fDegrees) { mGlobal.RotateGlobalX (fDegrees); }
void BoundingShape::RotateY (float fDegrees) { mGlobal.RotateGlobalY (fDegrees); }
void BoundingShape::RotateZ (float fDegrees) { mGlobal.RotateGlobalZ (fDegrees); }
void BoundingShape::Rotate (Vector& vAxis, float fDegrees) { mGlobal.RotateGlobal (vAxis, fDegrees); }

// rotate around the object's local axes
void BoundingShape::RotateLocalX (float fDegrees) { mGlobal.RotateX (fDegrees); }
void BoundingShape::RotateLocalY (float fDegrees) { mGlobal.RotateY (fDegrees); }
void BoundingShape::RotateLocalZ (float fDegrees) { mGlobal.RotateZ (fDegrees); }
	
// more sophisticated movement algorithms
void BoundingShape::LookAt (Vector& vPoint) { mGlobal.LookAt (vPoint); }
void BoundingShape::FaceDirection (Vector& vDirection) { mGlobal.FaceDirection (vDirection); }
void BoundingShape::TurnTo (Vector& vPoint, float fAngle) { mGlobal.TurnTo (vPoint, fAngle); }


// default collision algorithms
bool BoundingShape::CollidesWith (Triangle& triangle, Vector* vIntersection)
{
	// test the plane on which the triangle lies
	Vector vPlaneIntersect;
	if (!CollidesWith (Plane (triangle.vNormal, triangle.v1), &vPlaneIntersect))
		return false;

	// run 3 half-space tests
	if (! Line(triangle.v1, triangle.v2).SameSide (triangle.v3, vPlaneIntersect))
		return false;
	if (! Line(triangle.v2, triangle.v3).SameSide (triangle.v1, vPlaneIntersect))
		return false;
	if (! Line(triangle.v3, triangle.v1).SameSide (triangle.v2, vPlaneIntersect))
		return false;

	if (vIntersection)
		*vIntersection = vPlaneIntersect;
	return true;
}

bool BoundingShape::CollidesWith (Quad& quad, Vector* vIntersection)
{
	// test the plane on which the triangle lies
	Vector vPlaneIntersect;
	Vector vQuadNormal = ((quad.v2 - quad.v1) & (quad.v3 - quad.v1)).Normalize();
	if (!CollidesWith (Plane (vQuadNormal, quad.v1), &vPlaneIntersect))
		return false;

	// run 4 half-space tests
	if (! Line(quad.v1, quad.v2).SameSide (quad.v3, vPlaneIntersect))
		return false;
	if (! Line(quad.v2, quad.v3).SameSide (quad.v4, vPlaneIntersect))
		return false;
	if (! Line(quad.v3, quad.v4).SameSide (quad.v1, vPlaneIntersect))
		return false;
	if (! Line(quad.v4, quad.v1).SameSide (quad.v2, vPlaneIntersect))
		return false;
	
	if (vIntersection)
		*vIntersection = vPlaneIntersect;
	return true;
}