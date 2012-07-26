
#include "World.h"
#include "mathutils.h"
#include "direct3d.h"
#include "Game.h"				//for level design stuff
#include "OrcScout.h"
#include "OrcCutter.h"
#include "OrcLt.h"
#include "OrcBoss.h"
#include "loadscreen.h"

#include "Tree.h"
#include "Hut.h"
#include "Stump.h"
#include "Bridge.h"
#include "BoundingBox.h"		//just for the level 2 clear point
//#include "verticalcylinder.h"	//for creating bounding shapes for trees
#include "Menu.h"
#include <vector>
using std::vector;


void World::Partition(int collisionGridSize, int renderGridSize)
{
	iCollisionPartitionSize = collisionGridSize;
	fInvCollisionPartitionSize = 1.0f / collisionGridSize;
	
	iRenderPartitionSize = renderGridSize;
	fInvRenderPartitionSize = 1.0f / renderGridSize;

	iCollisionCols = 1 + int (fWidth * fInvCollisionPartitionSize);
	iCollisionPlanes = 1 + int (fDepth * fInvCollisionPartitionSize);

	iRenderCols = 1 + int (fWidth * fInvRenderPartitionSize);
	iRenderPlanes = 1 + int (fDepth * fInvRenderPartitionSize);

	// initialize the partitions
	pCollisionSegments = new WorldSegment [iCollisionCols * iCollisionPlanes];
	pRenderSegments = new WorldSegment [iRenderCols * iRenderPlanes];

	for (int segment=0; segment < iCollisionCols * iCollisionPlanes; segment++)
	{
		int iCol = segment % iCollisionCols;
		int iPlane = segment / iCollisionCols;
		WorldSegment* pSeg = &(pCollisionSegments [segment]);
		pSeg->fHalfWidth = pSeg->fHalfDepth = iCollisionPartitionSize*0.5f;
		//pSeg->MoveTo (GetXPos() + iCol * iPartitionSize + pSeg->fWidth*0.5f - fWidth * 0.5f, GetYPos(), GetZPos() + iPlane * iPartitionSize + pSeg->fDepth*0.5f - fDepth * 0.5f);
		pSeg->MoveTo (ModelGetXMin() + iCol * iCollisionPartitionSize + pSeg->fHalfWidth, ModelGetZMin() + iPlane * iCollisionPartitionSize + pSeg->fHalfDepth);
		//*(pSeg->GetRenderContext()) = *GetRenderContext();
		pSeg->pModel = pModel;

		// hack: the world segments need to know the position of the world, but can't access the world, so do this instead
		pSeg->pWorldMatrix = &mGlobal;
	}

	for (int segment=0; segment < iRenderCols * iRenderPlanes; segment++)
	{
		int iCol = segment % iRenderCols;
		int iPlane = segment / iRenderCols;
		WorldSegment* pSeg = &(pRenderSegments [segment]);
		pSeg->fHalfWidth = pSeg->fHalfDepth = iRenderPartitionSize*0.5f;
		//pSeg->MoveTo (GetXPos() + iCol * iPartitionSize + pSeg->fWidth*0.5f - fWidth * 0.5f, GetYPos(), GetZPos() + iPlane * iPartitionSize + pSeg->fDepth*0.5f - fDepth * 0.5f);
		pSeg->MoveTo (ModelGetXMin() + iCol * iRenderPartitionSize + pSeg->fHalfWidth, ModelGetZMin() + iPlane * iRenderPartitionSize + pSeg->fHalfDepth);
		//*(pSeg->GetRenderContext()) = *GetRenderContext();
		pSeg->pModel = pModel;
		// hack: the world segments need to know the position of the world, but can't access the world, so do this instead
		pSeg->pWorldMatrix = &mGlobal;
	}


	// partition: put the faces of each world segment into a temporary buffer
	vector <unsigned short> * CollisionSegmentIndices = new vector<unsigned short> [iCollisionCols * iCollisionPlanes];
	vector <unsigned short> * RenderSegmentIndices = new vector<unsigned short> [iRenderCols * iRenderPlanes];

	int iNumVertices = pModel->GetNumVertices();
	int iVertexSize = pModel->GetVertexSize();
	int iNumFaces = pModel->GetNumFaces();
	int iNumIndices = 3*iNumFaces;
	char* pVertexData;
	unsigned short* pIndexData;
	HRESULT hr;
	if (FAILED (hr = pModel->GetVertexBuffer()->Lock (0,0, (void**)&pVertexData, D3DLOCK_READONLY)))
		MessageBox (0, "failed to lock vertex buffer", "world partitioning", MB_OK);
	if (FAILED (hr = pModel->GetIndexBuffer()->Lock (0,0, (void**)&pIndexData, D3DLOCK_READONLY)))
		MessageBox (0, "failed to lock index buffer", "world partitioning", MB_OK);

	for (int face=0; face < iNumFaces; face++)
	{
		Vector v1, v2, v3;		//the positions of the face's vertices
		unsigned short sIndex1 = pIndexData [3*face];
		unsigned short sIndex2 = pIndexData [3*face +1];
		unsigned short sIndex3 = pIndexData [3*face +2];
		
		if (sIndex1 < 0 || sIndex2 < 0 || sIndex3 < 0)
		{
			MessageBox (0, "Index underflow", "World::Partition", MB_OK);
		}
		if (sIndex1 >= iNumIndices || sIndex2 >= iNumIndices || sIndex3 >= iNumIndices)
		{
			MessageBox (0, "Index overflow", "World::Partition", MB_OK);
		}

		memcpy (v1.fComp, pVertexData + (sIndex1 * iVertexSize), 3*sizeof(float));
		memcpy (v2.fComp, pVertexData + (sIndex2 * iVertexSize), 3*sizeof(float));
		memcpy (v3.fComp, pVertexData + (sIndex3 * iVertexSize), 3*sizeof(float));

		Triangle polygon (v1,v2,v3);
		Vector vCenter = (v1 + v2 + v3) * 0.33333333f;
		
		int iCollisionSegmentNumber = GetCollisionIndex (vCenter);
		int iRenderSegmentNumber = GetRenderIndex (vCenter);
		if (iCollisionSegmentNumber >= iCollisionCols * iCollisionPlanes || iRenderSegmentNumber >= iRenderCols * iRenderPlanes)
		{
			MessageBox (0, "Segment overflow", "World::Partition", MB_OK);
		}
		if (iCollisionSegmentNumber < 0 || iRenderSegmentNumber < 0)
		{
			MessageBox (0, "Segment underflow", "World::Partition", MB_OK);
		}

		// now for the punchline: put the polygon in the appropriate segments
		
		// render segments are easy: just put the polygon in the part of the grid that contains its center
		RenderSegmentIndices [iRenderSegmentNumber].push_back (sIndex1);
		RenderSegmentIndices [iRenderSegmentNumber].push_back (sIndex2);
		RenderSegmentIndices [iRenderSegmentNumber].push_back (sIndex3);

		// collision segments are trickier:
		// if a polygon overlaps multiple segments, each segment should have a copy of that polygon. Otherwise, we run the risk of falling through the world...
		// I shall check the segment that contains the polygon's center and all its neighboring segments
		// NOTE: I am assuming that no polygon is larger than a collision world segment!
		int CollisionX = GetCollisionIndexX (vCenter);
		int CollisionZ = GetCollisionIndexZ (vCenter);
		for (int x = CollisionX - 1; x <= CollisionX + 1; x++)
		{
			for (int z= CollisionZ - 1; z <= CollisionZ + 1; z++)
			{
				if (x == 0 && z == 0)
					int blah =0;
				WorldSegment* pSegment = GetCollisionSegment (x,z);
				if (pSegment && pSegment->CollidesWith (polygon))
				{
					int CollisionIndex = x + iCollisionCols * z;
					CollisionSegmentIndices [CollisionIndex].push_back (sIndex1);
					CollisionSegmentIndices [CollisionIndex].push_back (sIndex2);
					CollisionSegmentIndices [CollisionIndex].push_back (sIndex3);
				}
			}
		}
	}
	pModel->GetVertexBuffer()->Unlock();
	pModel->GetIndexBuffer()->Unlock();

	// now put the temporary data into the world segments
	for (int segment = 0; segment < iCollisionCols * iCollisionPlanes; segment++)
	{
		int iSegmentSize = (int)CollisionSegmentIndices[segment].size();
		// special case: don't do anything with empty segments
		if (iSegmentSize == 0)
			continue;

		WorldSegment* pCurrSegment = & (pCollisionSegments [segment]);
		unsigned short* pIndexData;

		Direct3D::GetDevice()->CreateIndexBuffer ( 
			sizeof (unsigned short) * iSegmentSize,
			0, 
			D3DFMT_INDEX16, 
			D3DPOOL_MANAGED,
			&(pCurrSegment->pIndexBuffer),
			0 );

		pCurrSegment->pIndexBuffer->Lock(0,0, (void**)&pIndexData, 0);
		for (int i=0; i<iSegmentSize; i++)
		{
			pIndexData[i] = CollisionSegmentIndices[segment][i];
		}
		pCurrSegment->pIndexBuffer->Unlock();

		pCurrSegment->iNumIndices = iSegmentSize;
	}

	for (int segment = 0; segment < iRenderCols * iRenderPlanes; segment++)
	{
		int iSegmentSize = (int)RenderSegmentIndices[segment].size();
		// special case: don't do anything with empty segments
		if (iSegmentSize == 0)
			continue;

		WorldSegment* pCurrSegment = & (pRenderSegments [segment]);
		unsigned short* pIndexData;

		Direct3D::GetDevice()->CreateIndexBuffer ( 
			sizeof (unsigned short) * iSegmentSize,
			0, 
			D3DFMT_INDEX16, 
			D3DPOOL_MANAGED,
			&(pCurrSegment->pIndexBuffer),
			0 );

		pCurrSegment->pIndexBuffer->Lock(0,0, (void**)&pIndexData, 0);
		for (int i=0; i<iSegmentSize; i++)
		{
			pIndexData[i] = RenderSegmentIndices[segment][i];
		}
		pCurrSegment->pIndexBuffer->Unlock();

		pCurrSegment->iNumIndices = iSegmentSize;
	}

	// don't forget to clean up your mess now
	delete[] CollisionSegmentIndices;
	delete[] RenderSegmentIndices;
}



void World::MoveBy (Vector& vOffset)
{
	Base::MoveBy (vOffset);
	ModelTranslate (vOffset);
	for (int i=0; i<iCollisionCols * iCollisionPlanes; i++)
		pCollisionSegments[i].MoveBy (vOffset);
	for (int i=0; i<iRenderCols * iRenderPlanes; i++)
		pRenderSegments[i].MoveBy (vOffset);
	fLeftBoundary += vOffset[0];
	fRightBoundary += vOffset[0];
	fFrontBoundary += vOffset[2];
	fBackBoundary += vOffset[2];
}

bool World::CollidesWithGeometry (Base* object, Vector* vIntersection, Triangle* pTriangle)
{
	Vector vPos = object->GetPos();
	int xIndex = GetCollisionIndexX(vPos);
	int zIndex = GetCollisionIndexZ(vPos);

	// check the segment that the object is in.
	// in case the object is in multiple segments, need to check the neighboring segments, too.
	for (int x=xIndex-1; x<= xIndex+1; x++)
	{
		for (int z=zIndex-1; z <= zIndex + 1; z++)
		{
			WorldSegment* pSegment = GetCollisionSegment (x,z);
			if (pSegment)
			{
				if (pSegment->CollidesWith (object))
				{
					if (pSegment->CollidesWithGeometry (object, vIntersection, pTriangle))
						return true;
				}
			}
		}
	}
	return false;
}

bool World::CollidesWith (Base* object, Vector* vIntersection)
{
	return CollidesWithGeometry (object, vIntersection);
}

bool World::CollidesWithGeometry (Line& line, Vector* vIntersection, Triangle* pTriangle)
{
	//return Base::CollidesWithGeometry (line, vIntersection);
	
	int xMin, xMax, zMin, zMax;		//the min/max x,y,z values on the line	
	Vector begin = line.vBegin;
	Vector end = line.vEnd;

	xMin = (begin.fComp[0] < end.fComp[0]) ? GetCollisionIndexX (begin) : GetCollisionIndexX (end);
	xMax = (begin.fComp[0] >= end.fComp[0]) ? GetCollisionIndexX (begin) : GetCollisionIndexX (end);

	zMin = (begin.fComp[2] < end.fComp[2]) ? GetCollisionIndexZ (begin) : GetCollisionIndexZ (end);
	zMax = (begin.fComp[2] >= end.fComp[2]) ? GetCollisionIndexZ (begin) : GetCollisionIndexZ (end);

	// generously loop through all the segments that the line goes through
	// this looks expensive, but the loop is usually very small (one or two iterations)
	for (int x=xMin; x<= xMax; x++)
	{
		for (int z=zMin; z <= zMax; z++)
		{
			WorldSegment* pSegment = GetCollisionSegment (x,z);
			if (pSegment)
			{
				if (pSegment->CollidesWith (line))
				{
					if (pSegment->CollidesWithGeometry (line, vIntersection, pTriangle))
						return true;
				}
			}
		}
	}
	return false;
}

bool World::CollidesWith (Line& line, Vector* vIntersection)
{
	return CollidesWithGeometry (line, vIntersection);
}

bool World::CollisionClamp (Base* object)
{
	int xIndex = GetCollisionIndexX (object->GetPos());
	int zIndex = GetCollisionIndexZ (object->GetPos());

	for (int x = xIndex-1; x <= xIndex+ 1; x++)
	{
		for (int z = zIndex-1; z <= zIndex + 1; z++)
		{
			WorldSegment* pSegment = GetCollisionSegment (x,z);
			if (pSegment)
			{
				if (pSegment->CollidesWith (object))
				{
					if (pSegment->CollisionClampGeometry (object))
						return true;
				}
			}
		}
	}

	return false;
}

bool World::GroundClamp (Base* object)
{
	if (!object->bGroundClamped)
		return false;

	Vector vTop = object->GetPos() + Vector (0, object->fHeight * 0.3f, 0);
	Vector vBottom = object->GetPos() - Vector (0, object->fHeight*0.55f, 0);
	Vector vIntersection;
	if (CollidesWith (Line (vTop, vBottom), &vIntersection))
	{
		object->MoveTo (vIntersection + Vector (0, object->fHeight*0.5f, 0));
		return true;
	}
	return false;
}

void World::KeepInBounds (Base* object)
{
	if (!object->bActive)
		return;

	if (object->GetYPos() < ModelGetYMin() - 1)
	{
		//object->MoveTo (object->GetXPos(), ModelGetYMax() + object->fHeight, object->GetZPos());
		object->MoveBy (0, object->fHeight - GetAltitude (object->GetPos()), 0);
		object->vExternal.fComp[1] = 0;
	}
	if (object->GetYPos() > ModelGetYMax() + 10)
		object->MoveTo (object->GetXPos(), ModelGetYMax() + 10, object->GetZPos());
	
	if (object->GetXPos() - object->fWidth*0.5f < fLeftBoundary)
		object->MoveTo (fLeftBoundary + object->fWidth*0.5f, object->GetYPos(), object->GetZPos());
	if (object->GetXPos() + object->fWidth*0.5f > fRightBoundary)
		object->MoveTo (fRightBoundary - object->fWidth*0.5f, object->GetYPos(), object->GetZPos());
	
	if (object->GetZPos() - object->fDepth*0.5f < fFrontBoundary)
		object->MoveTo (object->GetXPos(), object->GetYPos(), fFrontBoundary + object->fDepth*0.5f);
	if (object->GetZPos() + object->fDepth*0.5f > fBackBoundary)
		object->MoveTo (object->GetXPos(), object->GetYPos(), fBackBoundary - object->fDepth*0.5f);
}

void World::SetBoundaries (float xmin, float zmin, float xmax, float zmax)
{
	fLeftBoundary = xmin;
	fRightBoundary = xmax;
	fFrontBoundary = zmin;
	fBackBoundary = zmax;
}

float World::GetAltitude (Vector& vPoint)
{
	Vector vGroundPoint;		//vPoint projected onto the world's surface
	
	// case 1: point is above the world
	// this should always be the case
	Line vDown (vPoint, vPoint - Vector (0,1000,0));
	if (CollidesWith (vDown, &vGroundPoint))
		return vPoint.fComp[1] - vGroundPoint.fComp[1];
	
	// case 2: point is below the world
	// this case may or may not be of any use
	Line vUp (vPoint, vPoint + Vector (0,1000,0));
 	if (CollidesWith (vUp, &vGroundPoint))
		return vPoint.fComp[1] - vGroundPoint.fComp[1];

	// special case: seem to be over a bottomless pit if you get to this point. Just return 0 because of a lack of better alternatives
	return 0;
}

int World::GetCollisionIndexX (Vector& vPos)
{
	return int ( (vPos.fComp[0] - pModel->GetXMin()) * fInvCollisionPartitionSize);
}

int World::GetRenderIndexX (Vector& vPos)
{
	return int ( (vPos.fComp[0] - pModel->GetXMin()) * fInvRenderPartitionSize);
}

int World::GetCollisionIndexZ (Vector& vPos)
{
	return int ( (vPos.fComp[2] - pModel->GetZMin()) * fInvCollisionPartitionSize);
}

int World::GetRenderIndexZ (Vector& vPos)
{
	return int ( (vPos.fComp[2] - pModel->GetZMin()) * fInvRenderPartitionSize);
}

int World::GetCollisionIndex (Vector& vPos)
{
	//int returnvalue = GetIndexX(vPos) + iCols * GetIndexZ(vPos);
	return GetCollisionIndexX(vPos) + iCollisionCols * GetCollisionIndexZ(vPos);			
}

int World::GetRenderIndex (Vector& vPos)
{
	return GetRenderIndexX(vPos) + iRenderCols * GetRenderIndexZ (vPos);
}

WorldSegment* World::GetCollisionSegment (int index1d)
{
	if (index1d < 0 || index1d >= iCollisionCols * iCollisionPlanes)
		return NULL;

	return &pCollisionSegments [index1d];
}

WorldSegment* World::GetRenderSegment (int index1d)
{
	if (index1d < 0 || index1d >= iRenderCols * iRenderPlanes)
		return NULL;

	return &pRenderSegments [index1d];
}

WorldSegment* World::GetCollisionSegment (int xIndex, int zIndex)
{
	if (xIndex < 0 || zIndex < 0)
		return NULL;
	if (xIndex >= iCollisionCols || zIndex >= iCollisionPlanes)
		return NULL;

	return &pCollisionSegments [xIndex + iCollisionCols * zIndex];
}

WorldSegment* World::GetRenderSegment (int xIndex, int zIndex)
{
	if (xIndex < 0 || zIndex < 0)
		return NULL;
	if (xIndex >= iRenderCols || zIndex >= iRenderPlanes)
		return NULL;

	return &pRenderSegments [xIndex + iRenderCols * zIndex];
}

void World::Draw ()
{
	Direct3D::GetMatrixStack()->Push();
	Direct3D::GetMatrixStack()->LoadMatrixClass (mGlobal);
	Direct3D::GetMatrixStack()->ApplyToWorld();
	TextureMgr::Set (pModel->GetTexture(0));
	Direct3D::SetMaterial (pModel->GetMaterial (0));
	Direct3D::GetDevice()->SetFVF (pModel->GetFVF());
	Direct3D::GetDevice()->SetStreamSource (0, pModel->GetVertexBuffer(), 0, pModel->GetVertexSize());
	
	Frustum* pFrustum = (Frustum*)pCamera->pBoundingShape;
	for (int seg=0; seg<iRenderPlanes*iRenderCols; seg++)
	{
		if (pRenderSegments[seg].CollidesWith (pFrustum))
		{
			int iCol = seg % iRenderCols;
			int iPlane = seg / iRenderCols;
			pRenderSegments[seg].Draw();
		}
	}
	Direct3D::GetMatrixStack()->Pop();	
}

void World::ModelLoad (char* strFile)
{
	// get rid of the old world segments
	DELETE_ARRAY (pCollisionSegments);
	DELETE_ARRAY (pRenderSegments);
	// out with the old level
	SAFE_DELETE (pModel);
	// in with the new
	pModel = new DXModel;

	Base::ModelLoad (strFile);
	SetBoundaries (ModelGetXMin()+1, ModelGetXMax()-1, ModelGetZMin()+1, ModelGetZMax()-1);
}

void World::CreateRandomTrees (int num, float xMin, float zMin, float xMax, float zMax)
{
	LoadScreen::Draw("Generating Trees...");
	for (int i=0; i<num; i++)
	{
		int iModelNum =  RandomInt(1, TREEMODELS);
		char strFileName[64];
		sprintf (strFileName, "models\\tree%d.x", iModelNum);
		Tree* newTree = new Tree;
		newTree->ModelLoad (strFileName);
		newTree->ModelScale (0.5f);
		newTree->MoveTo (RandomFloat (xMin, xMax), 0, RandomFloat (zMin, zMax));
		newTree->MoveBy (0, newTree->fHeight * 0.5f - (GetAltitude (newTree->GetPos())) - 0.5f, 0);
		float fRadius = newTree->fWidth * 0.5f;
		newTree->pBoundingShape = new VerticalCylinder (newTree->fHeight + newTree->fWidth, fRadius);
		newTree->pBoundingShape->MoveTo (newTree->GetPos());
		newTree->RotateY (RandomFloat (0, 360));
		Game::lStaticObjects.Insert (newTree);
	}
}
void World::CreateRandomTrees (int num)
{
	CreateRandomTrees (num, ModelGetXMin(), ModelGetZMin(), ModelGetXMax(), ModelGetZMax());
}
/*
void World::CreateRandomBushes (int num, float xMin, float zMin, float xMax, float zMax)
{
	LoadScreen::Draw("Generating Bushes...");
	for (int i=0; i<num; i++)
	{
		int iModelNum =  RandomInt(1, BUSHMODELS);
		char strFileName[64];
		sprintf (strFileName, "models\\Bush%d.x", iModelNum);
		Bush* newBush = new Bush;
		newBush->ModelLoad (strFileName);
		newBush->ModelScale (0.25f);
		newBush->MoveTo (RandomFloat (xMin, xMax), 0, RandomFloat (zMin, zMax));
		newBush->MoveBy (0, newBush->fHeight * 0.5f - (GetAltitude (newBush->GetPos())) - 0.5f, 0);
		float fRadius = newBush->fWidth * 0.5f;
		newBush->pBoundingShape = new Sphere (newBush->GetPos(), fRadius);
		newBush->RotateY (RandomFloat (0, 360));
		Game::lStaticObjects.Insert (newBush);
	}
}
void World::CreateRandomBushes (int num)
{
	CreateRandomBushes (num, fLeftBoundary, fFrontBoundary, fRightBoundary, fBackBoundary);
}
*/
void World::CreateRandomStumps (int num, float xMin, float zMin, float xMax, float zMax)
{
	LoadScreen::Draw("Generating Stumps...");
	for (int i=0; i<num; i++)
	{
		int iModelNum =  RandomInt(1, STUMPMODELS);
		char strFileName[64];
		sprintf (strFileName, "models\\Stump%d.x", iModelNum);
		Stump* newStump = new Stump;
		newStump->ModelLoad (strFileName);
		newStump->ModelScale (0.25f);
		float fRadius = newStump->fWidth * 0.5f;
		newStump->pBoundingShape = new VerticalCylinder (newStump->fHeight + newStump->fWidth, fRadius);
		newStump->MoveTo (RandomFloat (xMin, xMax), 0, RandomFloat (zMin, zMax));
		newStump->MoveBy (0, newStump->fHeight * 0.5f - (GetAltitude (newStump->GetPos())) - 0.5f, 0);
		newStump->RotateY (RandomFloat (0, 360));
		Game::lStaticObjects.Insert (newStump);
	}
}
void World::CreateRandomStumps (int num)
{
	CreateRandomStumps (num, fLeftBoundary, fFrontBoundary, fRightBoundary, fBackBoundary);
}

void World::CreateRandomOrcScouts (int num, float xMin, float zMin, float xMax, float zMax)
{
	LoadScreen::Draw("Generating Orcish Scouts...");
	for (int i=0; i<num; i++)
	{
		OrcScout* newScout = new OrcScout;
		newScout->MoveTo (RandomFloat (xMin, xMax), 0, RandomFloat (zMin, zMax));
		newScout->MoveBy (0, newScout->fHeight * 0.5f - (GetAltitude (newScout->GetPos())), 0);
		int iPatrolPts = RandomInt (1,5);
		for (int p=0; p<iPatrolPts; p++)
		{
			newScout->AddPatrolPoint (RandomFloat (newScout->GetXPos() - 5, newScout->GetXPos() + 5), 0, RandomFloat (newScout->GetZPos() - 15, newScout->GetZPos() + 15));
		}
		Game::lEnemies.Insert (newScout);
		Game::iEnemies++;
	}
}

void World::CreateRandomOrcScouts (int num)
{
	CreateRandomOrcScouts (num, fLeftBoundary, fFrontBoundary, fRightBoundary, fBackBoundary);
}

void World::CreateRandomOrcCutters (int num, float xMin, float zMin, float xMax, float zMax)
{
	LoadScreen::Draw("Generating Orcish Cutters...");
	for (int i=0; i<num; i++)
	{
		OrcCutter* newCutter = new OrcCutter;
		newCutter->MoveTo (RandomFloat (xMin, xMax), 0, RandomFloat (zMin, zMax));
		newCutter->MoveBy (0, newCutter->fHeight * 0.5f - (GetAltitude (newCutter->GetPos())), 0);
		int iPatrolPts = RandomInt (1,5);
		for (int p=0; p<iPatrolPts; p++)
		{
			newCutter->AddPatrolPoint (RandomFloat (newCutter->GetXPos() - 5, newCutter->GetXPos() + 5), 0, RandomFloat (newCutter->GetZPos() - 15, newCutter->GetZPos() + 15));
		}
		Game::lEnemies.Insert (newCutter);
		Game::iEnemies++;
	}
}

void World::CreateRandomOrcCutters (int num)
{
	CreateRandomOrcCutters (num, fLeftBoundary, fFrontBoundary, fRightBoundary, fBackBoundary);
}

void World::CreateRandomOrcLts (int num, float xMin, float zMin, float xMax, float zMax)
{
	LoadScreen::Draw("Generating Orcish Brutes...");
	for (int i=0; i<num; i++)
	{
		OrcLt* newLt = new OrcLt;
		newLt->MoveTo (RandomFloat (xMin, xMax), 0, RandomFloat (zMin, zMax));
		newLt->MoveBy (0, newLt->fHeight * 0.5f - (GetAltitude (newLt->GetPos())), 0);
		int iPatrolPts = RandomInt (1,5);
		for (int p=0; p<iPatrolPts; p++)
		{
			newLt->AddPatrolPoint (RandomFloat (newLt->GetXPos() - 5, newLt->GetXPos() + 5), 0, RandomFloat (newLt->GetZPos() - 15, newLt->GetZPos() + 15));
		}
		Game::lEnemies.Insert (newLt);
		Game::iEnemies++;
	}
}

void World::CreateRandomOrcLts (int num)
{
	CreateRandomOrcLts (num, fLeftBoundary, fFrontBoundary, fRightBoundary, fBackBoundary);
}

void World::CreateRandomHuts (int num, float xMin, float zMin, float xMax, float zMax)
{
	LoadScreen::Draw("Generating Huts...");
	for (int i=0; i<num; i++)
	{
		Hut* newHut = new Hut;
		newHut->RotateY(RandomFloat(0, 360));
		newHut->MoveTo (RandomFloat (xMin, xMax), 0, RandomFloat (zMin, zMax));
		newHut->MoveBy (0, newHut->fHeight * 0.5f - (GetAltitude (newHut->GetPos())) - 1, 0);
		Game::lStaticObjects.Insert (newHut);
	}
}

void World::CreateRandomHuts (int num)
{
	CreateRandomHuts (num, fLeftBoundary, fFrontBoundary, fRightBoundary, fBackBoundary);
}
// LEVEL LOADING
void World::LoadLevel (int iLevel)
{
	LoadScreen::Reset();
	LoadScreen::LoadFont();
	LoadScreen::SetSize(1000000);

	LoadScreen::DrawText("Preparing level...");

	// CHANGED
	// need to deallocate old models before removing old entities
	if (Game::pBridge)
	{
		SAFE_DELETE (Game::pBridge->pModel);
		SAFE_DELETE (Game::pBridge);
	}
	BaseNode* pNode;
	for (pNode = Game::lEnemies.first; pNode; pNode = pNode->next)
		SAFE_DELETE (pNode->object->pModel);
	for (pNode = Game::lStaticObjects.first; pNode; pNode = pNode->next)
		SAFE_DELETE (pNode->object->pModel);

	// now that all models are gone, we may delete all objects
	Game::lEnemies.DeleteListAndObjects();
	Game::lStaticObjects.DeleteListAndObjects();
	Game::lPickups.DeleteListAndObjects();
	Game::lEventTriggers.DeleteListAndObjects();
	Game::iEnemies = 0;

	static int iMusic1;
	static int iMusic2;

	for (BaseNode* pNode = Game::lArrows.first; pNode; pNode = pNode->next)
		pNode->object->bActive = 0;
	for (BaseNode* pNode = Game::lExplosions.first; pNode; pNode = pNode->next)
		pNode->object->bActive = 0;

	LoadScreen::DrawText("Preparing character...", 50000);
	// revive the player
	Game::pPlayer1->iHitPoints = Game::pPlayer1->iMaxHitPoints;
	Game::pPlayer1->bActive = 1;
	Game::pPlayer1->bCollidable = 1;
	Game::pPlayer1->bTakesDamage = 1;
	Game::pPlayer1->GetRenderContext()->bDraw = 1;
	Game::pPlayer1->mGlobal.SetToIdentity();

	// load new level
	switch (iLevel)
	{
	case 1:
		{
		// give an overview of the level
			pCamera->SmartAttach (Game::pPlayer1);
		pCamera->MoveTo (0,20,0);
		pCamera->FaceDirection (Vector (0, -0.7f, 0.7f));
		LoadScreen::DrawText("Loading terrain...");
			
		// initialize level
		Direct3D::Fog (true, VIEWFRUSTUM_FARCLIP * 0.65f, VIEWFRUSTUM_FARCLIP * 1.25f, D3DCOLOR_XRGB (100, 100, 175));
		ModelLoad("models\\level 1.x");
		ModelScale (0.3f);
		Partition (4,25);			//10,50 - 20,100 work well for the unscaled version of this level	
		SetBoundaries (-110, -60, 133,135);

		LoadScreen::DrawText("Loading music...");
		SoundManager::StopMusic();
		//SoundManager::Play(SoundManager::LoadStreamedSound("sounds\\tribal.mp3", Menu::iSoundVol * 2));
		SoundManager::Play (SoundManager::LoadSound ("sounds\\tribal.mp3", 150, true));
	
		Game::pPlayer1->MoveTo (-115, 0, -50);
		Game::pPlayer1->MoveBy (0, 3 - GetAltitude (Game::pPlayer1->GetPos()), 0);
		pCamera->HardAttach (Game::pPlayer1);		

		LoadScreen::DrawText("Loading skybox...");
		SAFE_DELETE (pSkyBox);
		pSkyBox = new Base;
		pSkyBox->ModelLoad ("models\\skybox for treelevel.x");
		pSkyBox->ModelScale (0.5f);
		pSkyBox->MoveBy (0,0,0);

		// populate level
		
		// DEBUG
		//OrcLt* lt = new OrcLt;
		//lt->MoveTo (Game::pPlayer1->GetPos() + Game::pPlayer1->GetZ() * 10);
		//Game::lEnemies.Insert (lt);

		// dense tree population everywhere
		CreateRandomTrees (100, -105, -55, 130, 130);
		// population of cutters
		CreateRandomOrcCutters (int (30 * Game::fDifficultyScale), -90, -60, 133,135);		
		CreateRandomOrcCutters (int (15 * Game::fDifficultyScale), -120, -30, -100, 135);
		// very sparse scout population
		CreateRandomOrcScouts (int (10 * Game::fDifficultyScale), -50, -60, 133, 135);
		// some scouts guarding the exit
		CreateRandomOrcScouts (int (5 * Game::fDifficultyScale), 20, -50, 25, -60);

		// set endpoint
		EventTrigger* nextLevel = new EventTrigger;
		nextLevel->OnActivate = Game::OnClearLevel;
		nextLevel->pBoundingShape = new VerticalCylinder (10, 10);
		nextLevel->pBoundingShape->SetColor (1,0,1);
		nextLevel->MoveTo (Vector(23, -13, -66));
		nextLevel->MoveBy (0, -GetAltitude(nextLevel->GetPos()), 0);
		Game::lEventTriggers.Insert (nextLevel);
		}
		break;
	case 2:
		{
		// load the level
		Direct3D::Fog (true, VIEWFRUSTUM_FARCLIP * 0.75f, VIEWFRUSTUM_FARCLIP*1.5f, D3DCOLOR_XRGB (175, 100, 100));
		LoadScreen::DrawText("Loading terrain...");
		ModelLoad("models\\level 2.x");
		LoadScreen::DrawText("Loading music...");
		SoundManager::StopMusic();
		SoundManager::Play(SoundManager::LoadStreamedSound("sounds\\slow.mp3", Menu::iSoundVol * 2));
		ModelScale (0.5f);
		Partition (5,50);			//10,50 - 20,100 work well for the unscaled version of this level	
		SetBoundaries (-1135, -180, 1120, 166);
		Game::pPlayer1->MoveTo (Vector(-115,0,-60));
		Game::pPlayer1->MoveBy (0, - GetAltitude (Game::pPlayer1->GetPos()), 0);
		pCamera->HardAttach (Game::pPlayer1);	
		
		SAFE_DELETE (pSkyBox);
		pSkyBox = new Base;
		LoadScreen::DrawText("Loading skybox...");
		pSkyBox->ModelLoad ("models\\skybox for stumplevel.x");
		pSkyBox->ModelScale (0.20f);
		pSkyBox->MoveBy (0,-60,0);

		// populate level
		
		// random static geometry
		CreateRandomTrees (20, -130, -75, -70, 50);
		//CreateRandomTrees(5, -50, -75, 50, 50);
		CreateRandomStumps (15, -80, -75, -50, 50);
		CreateRandomStumps(40, -80, -75, 20, 50);
		CreateRandomHuts (35, -20, -40, 100, 40);

		// strategically placed bridge
		Bridge* bridge = new Bridge;
		bridge->pBoundingShape = new BoundingBox (bridge->fDepth*0.5f, bridge->fHeight*0.3f, bridge->fWidth*2.5f);
		bridge->MoveTo (138, -10, -40);
		bridge->RotateY (90);
		
		Game::lStaticObjects.Insert (bridge);

		//CreateRandomBushes (15, -110, -70, -80, -40);
		// CreateRandomStubs (80, -100, -75, 40, 50);
		/*
		// Big orcs
		//CreateRandomOrcLt (1, -80, 20, -60, 0);
		//CreateRandomOrcLt (1, -60, -40, -40, -20);
		//CreateRandomOrcLt (1, 0, 0, 40, 40);
		//CreateRandomOrcLt (3);
		// bodyguards of big orcs
		CreateRandomOrcScouts (5, -80, 20, -60, 0);
		CreateRandomOrcScouts (5, -60, -40, -40, -20);
		CreateRandomOrcScouts (5, 0, 0, 40, 40);
		CreateRandomOrcCutters (5, -80, 20, -60, 0);
		CreateRandomOrcCutters (5, -60, -40, -40, -20);
		CreateRandomOrcCutters (5, 0, 0, 40, 40);
		*/

		// a couple of big orcs
		CreateRandomOrcLts ( int (Game::fDifficultyScale * 5), -100, -70, 50, 50);
		// the little ones are now primarily archers instead of cutters
		CreateRandomOrcScouts ( int (Game::fDifficultyScale * 20), -100, -70, 50, 50);
		CreateRandomOrcCutters (int (Game::fDifficultyScale * 15), -100, -70, 50, 50);

		
		// set endpoint
		EventTrigger* nextLevel = new EventTrigger;
		nextLevel->OnActivate = Game::OnClearLevel;
		nextLevel->pBoundingShape = new BoundingBox (2, 15, 200);
		nextLevel->pBoundingShape->SetColor (1,0,1);
		nextLevel->MoveTo (Vector (130,0,0));
		nextLevel->MoveBy (0, -GetAltitude (nextLevel->GetPos()), 0);
		Game::lEventTriggers.Insert (nextLevel);

		// start music
		//SoundManager::Play(SoundManager::LoadStreamedSound("sounds\\tribal.mp3", Menu::iSoundVol * 2));

		}
		break;
	case 3:
		{
		Direct3D::Fog (false);
		LoadScreen::DrawText("Loading terrain...");
		ModelLoad("models\\level 3.x");
		ModelScale (0.3f);
		Partition (5,50);			//10,50 - 20,100 work well for the unscaled version of this level	
		SetBoundaries (-40,-30, 8.5, 15);
		Game::pPlayer1->MoveTo (Vector (0,0,0));
		Game::pPlayer1->MoveBy (0, 3 - GetAltitude (Game::pPlayer1->GetPos()), 0);
		pCamera->HardAttach (Game::pPlayer1);	

		SAFE_DELETE (pSkyBox);
		pSkyBox = new Base;
		LoadScreen::DrawText("Loading skybox...");
		pSkyBox->ModelLoad ("models\\skybox for stumplevel.x");
		pSkyBox->ModelScale (0.20f);
		pSkyBox->MoveBy (0,-50,0);

		// populate level
		//CreateRandomOrcLts (3);
		
		/*
		OrcBoss* boss = new OrcBoss;
		boss->SetTarget (Game::pPlayer1);
		boss->MoveTo (-30,0,0);
		boss->MoveBy (0, fHeight * 0.5f  -GetAltitude (boss->GetPos()), 0);
		Game::lEnemies.Insert (boss);
		Game::iEnemies++;
		*/
		CreateRandomHuts (10, -35, -20, 7, 10);
		CreateRandomOrcLts (4);
		CreateRandomOrcScouts (6);

		SoundManager::StopMusic();
		SoundManager::Play(SoundManager::LoadStreamedSound("sounds\\tribal.mp3", Menu::iSoundVol * 2));


		}

		break;		

		
	
	case 4:
		Game::iGameState = GAME_WIN;
 		break;
	}

Game::iGameState=GAME_EXPOSITION;

	LoadScreen::FadeOut();
}