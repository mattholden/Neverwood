#include "WorldSegment.h"
#include "Direct3d.h"
#include "BoundingBox.h"

void WorldSegment::MoveTo (float X, float Z)
{
	x=X;
	z=Z;
}

void WorldSegment::MoveBy (float X, float Z)
{
	x += X;
	z += Z;
}

Plane WorldSegment::GetRightPlane()
{
	return Plane (Vector (1,0,0), Vector (x + fHalfWidth, 0, z)); 
}

Plane WorldSegment::GetLeftPlane()
{
	return Plane (Vector (-1,0,0), Vector (x - fHalfWidth, 0, z)); 
}

Plane WorldSegment::GetBackPlane()
{
	return Plane (Vector (0,0,1), Vector (x, 0, z + fHalfDepth)); 
}

Plane WorldSegment::GetFrontPlane()
{
	return Plane (Vector (0,0,-1), Vector (x, 0, z - fHalfDepth)); 
}

bool WorldSegment::CollidesWith (Base* other, Vector* vIntersection)
{
	if (!other->pBoundingShape)
		return false;

	switch (other->pBoundingShape->iObjectType)
	{
	case SHAPE_SPHERE:
	case SHAPE_VERTICAL_CYLINDER:
		{
			Vector vPos = GetPos();
			Vector vOtherPos = other->pBoundingShape->GetPos();

			// other object too far to the right
			if (vOtherPos[0] - other->pBoundingShape->fWidth > vPos[0] + fHalfWidth)
				return false;
			// too far left
			if (vOtherPos[0] + other->pBoundingShape->fWidth < vPos[0] - fHalfWidth)
				return false;
			// too far away
			if (vOtherPos[2] - other->pBoundingShape->fDepth > vPos[2] + fHalfDepth)
				return false;
			// too close
			if (vOtherPos[2] + other->pBoundingShape->fDepth < vPos[2] - fHalfDepth)
				return false;
			return true;
		}
		break;
	default:
		return false;
	
	}
}

bool WorldSegment::CollidesWith (Vector& vPoint)
{
	// point is too far to the right
	if (vPoint[0] > x + fHalfWidth)
		return false;

	// point is too far to the left
	if (vPoint[0] < x - fHalfWidth)
		return false;

	// point is too far away
	if (vPoint[2] > z + fHalfDepth)
		return false;

	// point is too close
	if (vPoint[2] < z - fHalfDepth)
		return false;
		
	return true;
}

bool WorldSegment::CollidesWith (Triangle& triangle)
{
	// NOTE:
	// World Segment to triangle collision is slightly different from other collision routines
	// if a triangle's vertex is on the boundary of the world segment, we do NOT want collision to return true
	// this is to avoid having more duplicate polygons than necessary when doing world partitioning.

	// do some quick preliminary checks that return false ASAP
	#define ERROR_MARGIN 0.01f
	float fRight = x + fHalfWidth - ERROR_MARGIN;
	if (fRight < triangle.v1[0] && fRight < triangle.v2[0] && fRight < triangle.v3[0])
		return false;
	float fFront = z - fHalfDepth + ERROR_MARGIN;
	if (fFront > triangle.v1[2] && fFront > triangle.v2[2] && fFront > triangle.v3[2])
		return false;
	float fLeft = x - fHalfWidth + ERROR_MARGIN;
	if (fLeft > triangle.v1[0] && fLeft > triangle.v2[0] && fLeft > triangle.v3[0])
		return false;
	float fBack = z + fHalfDepth - ERROR_MARGIN;
	if (fBack < triangle.v1[2] && fBack < triangle.v2[2] && fBack < triangle.v3[2])
		return false;

	/*
	// if one of the triangle's vertices is inside the world segment, return true
	if (triangle.v1[0] > fLeft && triangle.v1[0] < fRight && triangle.v1[2] > fFront && triangle.v1[2] < fBack)
		return true;
	if (triangle.v2[0] > fLeft && triangle.v2[0] < fRight && triangle.v2[2] > fFront && triangle.v2[2] < fBack)
		return true;
	if (triangle.v3[0] > fLeft && triangle.v3[0] < fRight && triangle.v3[2] > fFront && triangle.v3[2] < fBack)
		return true;
	*/

	// if the triangle's center is in the world segment, return true
	if ( CollidesWith ((triangle.v1 + triangle.v2 + triangle.v3) * 0.33333f))
		return true;

	// if one of the triangle's edges is inside the world segment, return true
	if (CollidesWith (Line (triangle.v1, triangle.v2)))
		return true;
	if (CollidesWith (Line (triangle.v2, triangle.v3)))
		return true;
	if (CollidesWith (Line (triangle.v3, triangle.v1)))
		return true;

	return false;
}

// this one's an exact copy of the base's
bool WorldSegment::CollidesWithGeometry (Base* other, Vector* vIntersection, Triangle* pTriangle)
{
	// be extra paranoid about empty geometry
	if (iNumIndices == 0)
		return false;
	
	int iNumCollisions = 0;
	Triangle closestPoly;
	Vector vClosestIntersection, vCurrentIntersection;

	// check the other object's bounding shape against each triangle
	int iVertexSize = ModelGetVertexSize();
	char* pVertexData;
	unsigned short* pIndexData;
	ModelGetVertexBuffer()->Lock (0,0, (void**)&pVertexData, D3DLOCK_READONLY);
	ModelGetIndexBuffer()->Lock (0,0, (void**)&pIndexData, D3DLOCK_READONLY);

	for (int face=0; face < ModelGetNumFaces(); face++)
	{
		Vector v1, v2, v3;		//the positions of the face's vertices
		unsigned short sIndex1 = pIndexData [3*face];
		unsigned short sIndex2 = pIndexData [3*face +1];
		unsigned short sIndex3 = pIndexData [3*face +2];

		memcpy (v1.fComp, pVertexData + (sIndex1 * iVertexSize), 3*sizeof(float));
		memcpy (v2.fComp, pVertexData + (sIndex2 * iVertexSize), 3*sizeof(float));
		memcpy (v3.fComp, pVertexData + (sIndex3 * iVertexSize), 3*sizeof(float));

		Triangle currentPoly (v1,v2,v3);
		if (other->pBoundingShape->CollidesWith (currentPoly, &vCurrentIntersection))
		{
			iNumCollisions++;

			if (iNumCollisions == 1)
			{
				vClosestIntersection = vCurrentIntersection;
				closestPoly = currentPoly;
			}
			
			else
			{
				float fCurrentDist = (vCurrentIntersection - other->GetPos()).LengthSquared();
				float fShortestDist = (vClosestIntersection - other->GetPos()).LengthSquared();
				if (fCurrentDist < fShortestDist)
				{
					vClosestIntersection = vCurrentIntersection;
					closestPoly = currentPoly;
				}
			}

		}// if triangle collision			
	}// for each face
	ModelGetVertexBuffer()->Unlock();
	ModelGetIndexBuffer()->Unlock();
	
	if (iNumCollisions > 0)
	{
		if (vIntersection)
			*vIntersection = vClosestIntersection;
		if (pTriangle)
			*pTriangle = closestPoly;
		return true;
	}
	return false;
}

bool WorldSegment::CollidesWith (Line& line, Vector* vIntersection)
{
	// line too far to the right
	if (line.vBegin[0] > x + fHalfWidth && line.vEnd[0] > x + fHalfWidth)
		return false;

	// too far to the left
	if (line.vBegin[0] < x - fHalfWidth && line.vEnd[0] < x - fHalfWidth)
		return false;

	// too far back
	if (line.vBegin[2] > z + fHalfDepth && line.vEnd[2] > z + fHalfDepth)
		return false;

	// too close
	if (line.vBegin[2] < z - fHalfDepth && line.vEnd[2] < z - fHalfDepth)
		return false;

	// optimization: if a line collides with a world segment, it is usually because one of its endpoints lies inside of it, so check for this case first
	if (CollidesWith (line.vBegin))
		return true;
	if (CollidesWith (line.vEnd))
		return true;

	// now do a brute-force check against the 4 planes of the world segment
	Vector vPlaneIntersect;
	
	// right plane
	if (GetRightPlane().CollidesWith (line, &vPlaneIntersect))
	{
		if (vPlaneIntersect[2] <= z + fHalfDepth && vPlaneIntersect[2] >= z - fHalfDepth)
			return true;
	}

	// left plane
	if (GetLeftPlane().CollidesWith (line, &vPlaneIntersect))
	{
		if (vPlaneIntersect[2] <= z + fHalfDepth && vPlaneIntersect[2] >= z - fHalfDepth)
			return true;
	}
	
	// back plane
	if (GetBackPlane().CollidesWith (line, &vPlaneIntersect))
	{
		if (vPlaneIntersect[0] <= x + fHalfWidth && vPlaneIntersect[0] >= x - fHalfWidth)
			return true;
	}

	// front plane
	if (GetFrontPlane().CollidesWith (line, &vPlaneIntersect))
	{
		if (vPlaneIntersect[0] <= x + fHalfWidth && vPlaneIntersect[0] >= x - fHalfWidth)
			return true;
	}

	return false;		// worst case: near miss
}

bool WorldSegment::CollidesWithGeometry (Line& line, Vector* vIntersection, Triangle* pTriangle)
{
	// be extra-cautios about empty geometry
	if (!pIndexBuffer || !ModelGetVertexBuffer())
		return false;

	int iNumCollisions = 0;
	Triangle closestPoly;
	Vector vClosestIntersection, vCurrentIntersection;

	// check collision against each triangle of the world segment's geometry
	int iVertexSize = ModelGetVertexSize();
	char* pVertexData;
	unsigned short* pIndexData;
	ModelGetVertexBuffer()->Lock (0,0, (void**)&pVertexData, D3DLOCK_READONLY);
	pIndexBuffer->Lock (0,0, (void**)&pIndexData, D3DLOCK_READONLY);

	int faces = ModelGetNumFaces();
	for (int face=0; face < faces; face++)
	{
		Vector v1, v2, v3;		//the positions of the face's vertices
		unsigned short sIndex1 = pIndexData [3*face];
		unsigned short sIndex2 = pIndexData [3*face +1];
		unsigned short sIndex3 = pIndexData [3*face +2];

		memcpy (v1.fComp, pVertexData + (sIndex1 * iVertexSize), 3*sizeof(float));
		memcpy (v2.fComp, pVertexData + (sIndex2 * iVertexSize), 3*sizeof(float));
		memcpy (v3.fComp, pVertexData + (sIndex3 * iVertexSize), 3*sizeof(float));

		Triangle currentPoly (v1,v2,v3);
		if (currentPoly.CollidesWith (line, &vCurrentIntersection))
		{
			iNumCollisions++;

			if (iNumCollisions == 1)
			{
				vClosestIntersection = vCurrentIntersection;
				closestPoly = currentPoly;
			}
			
			else
			{
				float fCurrentDist = (vCurrentIntersection - GetPos()).LengthSquared();
				float fShortestDist = (vClosestIntersection - GetPos()).LengthSquared();
				if (fCurrentDist < fShortestDist)
				{
					vClosestIntersection = vCurrentIntersection;
					closestPoly = currentPoly;
				}
			}

		}// if triangle collision			
	}// for each face
	ModelGetVertexBuffer()->Unlock();
	pIndexBuffer->Unlock();
	
	if (iNumCollisions > 0)
	{
		if (vIntersection)
			*vIntersection = vClosestIntersection;
		if (pTriangle)
			*pTriangle = closestPoly;
		return true;
	}
	return false;
}

bool WorldSegment::CollidesWith (Frustum* frustum)
{
	// first, check if the segment is in the right zone along the frustum's z axis
	Vector vFrustumPos = frustum->GetPos();
	Vector vPos = Vector (x,0,z);
	float zDot = (vPos - vFrustumPos) * frustum->GetZ();
	if (zDot < frustum->GetNearClip() - (fHalfWidth + fHalfDepth))
		return false;
	if (zDot > frustum->GetFarClip() + (fHalfWidth + fHalfDepth))
		return false;

	// check if the segment is in the right zone along the frustum's x axis
	float xDot = (vPos - vFrustumPos) * frustum->GetX();
	if (ABS (xDot) > 0.5f * frustum->fWidth * zDot * frustum->GetInvFarClip() + fHalfWidth + fHalfDepth)
		return false;

	// check if you can see a corner of the world segment
	
	// back left corner
	if (frustum->CollidesWithVerticalLine (vPos + Vector (-fHalfWidth, 0, fHalfDepth)))
		return true;

	// back right corner
	if (frustum->CollidesWithVerticalLine (vPos + Vector (fHalfWidth, 0, fHalfDepth)))
		return true;

	// front left corner
	if (frustum->CollidesWithVerticalLine (vPos + Vector (-fHalfWidth, 0, -fHalfDepth)))
		return true;

	// front right corner
	if (frustum->CollidesWithVerticalLine (vPos + Vector (fHalfWidth, 0, -fHalfDepth)))
		return true;

	// if the collision check got this far, it's a special case. The segment is probably really close to the frustum.
	// need to check if any of the segment's planes cross the frustum
	if (GetFrontPlane().CollidesWith (Line (frustum->NearTopLeft(), frustum->FarTopLeft())))
		return true;
	if (GetFrontPlane().CollidesWith (Line (frustum->NearTopRight(), frustum->FarTopRight())))
		return true;
	if (GetBackPlane().CollidesWith (Line (frustum->NearTopLeft(), frustum->FarTopLeft())))
		return true;
	if (GetBackPlane().CollidesWith (Line (frustum->NearTopRight(), frustum->FarTopRight())))
		return true;
	if (GetLeftPlane().CollidesWith (Line (frustum->NearTopLeft(), frustum->FarTopLeft())))
		return true;
	if (GetLeftPlane().CollidesWith (Line (frustum->NearTopRight(), frustum->FarTopRight())))
		return true;
	if (GetRightPlane().CollidesWith (Line (frustum->NearTopLeft(), frustum->FarTopLeft())))
		return true;
	if (GetRightPlane().CollidesWith (Line (frustum->NearTopRight(), frustum->FarTopRight())))
		return true;

	return false;
}

bool WorldSegment::CollisionClampGeometry (Base* object, Vector* vIntersection, Triangle* pTriangle)
{
	// if the objects collided, reposition the object
	Vector vIntersect(0,0,0);
	if (CollidesWithGeometry (object, &vIntersect, pTriangle))
	{
		// prevent gravity from accumulating
		object->pBoundingShape->CollisionClamp (vIntersect);
		object->MoveTo (object->pBoundingShape->GetPos());
		if (vIntersection)
			*vIntersection = vIntersect;
		return true;
	}
	return false;
}

void WorldSegment::Draw()
{
	if (iNumIndices == 0)
		return;

	LPDIRECT3DDEVICE9 pDevice = Direct3D::GetDevice();
	pDevice->SetIndices (pIndexBuffer);	
	pDevice->DrawIndexedPrimitive (D3DPT_TRIANGLELIST, 0, 0, pModel->GetNumVertices(), 0, iNumIndices / 3);
}