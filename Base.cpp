
// Defines the functionality for the base class.

#include "base.h"
#include "Identifiers.h"
#include "RenderInfo.h"
#include "Direct3d.h"
#include "DXModel.h"

// Initialize static count variables.
int Base::siObjectCount = 0;


// Generic constructor (for arrays)
Base::Base(int iObjType)
{
	
	// Set my object type.
	iObjectType = iObjType;

	// Set my unique ID and increment for the next one
	iObjectID = siObjectCount++;

	// Set object flags
	bCollidable = 1;
	bTakesDamage = 0;
	bGroundClamped = 0;
	bGravityAffected = 0;
	bUpdate = 1;

	bActive=true;

	// Set state
	iCurrentState = 0;
	fStateTime = 0;

	// Set Rendering Context
	RContext = new RenderInfo;

	vVelocity.SetToZero();
	mGlobal.SetToIdentity();
	vLastPos = GetPos();

	// No bounding shape
	pBoundingShape = NULL;
	fMass = 1;

	// No model yet
	pModel = NULL;
	
	fWidth = fHeight = fDepth = 1;

	vInternal.SetToZero();
	vExternal.SetToZero();

	// default to white color
	fRed = fGreen = fBlue = 1;
}

// Destructor
Base::~Base(void)
{
	if (pBoundingShape)
	{
		delete pBoundingShape;
		pBoundingShape = NULL;
	}

	// PSEUDO-HACK:
	// world segments share the world's model and rendering context, and we want to make abso-friggin-lutely sure this doesn't cause any problems
	if (iObjectType == BASETYPE_WORLDSEGMENT)
		return;

	if (RContext)
	{
		delete RContext;
		RContext = NULL;
	}

	// CHANGED
	// not deleting models anymore
	// I anticipate that most objects will share a static model rather than each instance have a separate one
}

// puts the object to the given position
void Base::MoveTo (Vector& vPoint)
{
	memcpy (mGlobal.fMatrix + 12, vPoint.fComp, 12);
	if (pBoundingShape)
		memcpy (pBoundingShape->mGlobal.fMatrix + 12, vPoint.fComp, 12);
}

// translates the object by a given amount
void Base::MoveBy	(Vector& vOffset) 
{ 
	mGlobal.Translate (vOffset); 
	if (pBoundingShape)
		pBoundingShape->mGlobal.Translate (vOffset);
}

// local translation
void Base::MoveByLocal	(Vector& vOffset) 
{ 
	mGlobal.TranslateLocal (vOffset); 
	if (pBoundingShape)
		pBoundingShape->mGlobal.TranslateLocal (vOffset);
}


// move along local z axis
void Base::MoveForward (float amount) 
{
	mGlobal.fMatrix[12] += amount * mGlobal.fMatrix[8];
	mGlobal.fMatrix[13] += amount * mGlobal.fMatrix[9];
	mGlobal.fMatrix[14] += amount * mGlobal.fMatrix[10];

	if (pBoundingShape)
	{
	pBoundingShape->mGlobal.fMatrix[12] += amount * mGlobal.fMatrix[8];
	pBoundingShape->mGlobal.fMatrix[13] += amount * mGlobal.fMatrix[9];
	pBoundingShape->mGlobal.fMatrix[14] += amount * mGlobal.fMatrix[10];
	}
}

// move along local x axis
void Base::MoveRight (float amount) 
{
	mGlobal.fMatrix[12] += amount * mGlobal.fMatrix[0];
	mGlobal.fMatrix[13] += amount * mGlobal.fMatrix[1];
	mGlobal.fMatrix[14] += amount * mGlobal.fMatrix[2];

	if (pBoundingShape)
	{
	mGlobal.fMatrix[12] += amount * mGlobal.fMatrix[0];
	mGlobal.fMatrix[13] += amount * mGlobal.fMatrix[1];
	mGlobal.fMatrix[14] += amount * mGlobal.fMatrix[2];
	}

}

// move along local y axis
void Base::MoveUp (float amount) 
{
	mGlobal.fMatrix[12] += amount * mGlobal.fMatrix[4];
	mGlobal.fMatrix[13] += amount * mGlobal.fMatrix[5];
	mGlobal.fMatrix[14] += amount * mGlobal.fMatrix[6];
	if (pBoundingShape)
	{
		pBoundingShape->mGlobal.fMatrix[12] += amount * mGlobal.fMatrix[4];
		pBoundingShape->mGlobal.fMatrix[13] += amount * mGlobal.fMatrix[5];
		pBoundingShape->mGlobal.fMatrix[14] += amount * mGlobal.fMatrix[6];
	}
}
	
// rotate around the world's axes
void Base::RotateX (float fDegrees) 
{ 
	mGlobal.RotateGlobalX (fDegrees); 
	
	if (pBoundingShape)
		pBoundingShape->mGlobal.RotateGlobalX (fDegrees); 
}
void Base::RotateY (float fDegrees) 
{ 
	mGlobal.RotateGlobalY (fDegrees); 

	if (pBoundingShape)
		pBoundingShape->mGlobal.RotateGlobalY(fDegrees);
}
void Base::RotateZ (float fDegrees) 
{ 
	mGlobal.RotateGlobalZ (fDegrees);
	if (pBoundingShape)
		pBoundingShape->mGlobal.RotateGlobalZ(fDegrees);
}
void Base::Rotate (Vector& vAxis, float fDegrees) 
{ 
	mGlobal.RotateGlobal (vAxis, fDegrees); 
	if (pBoundingShape)
		pBoundingShape->mGlobal.RotateGlobal (vAxis, fDegrees);
}

// rotate around the object's local axes
void Base::RotateLocalX (float fDegrees) 
{ 
	mGlobal.RotateX (fDegrees); 
	if (pBoundingShape)
		pBoundingShape->mGlobal.RotateX (fDegrees);
}
void Base::RotateLocalY (float fDegrees) 
{ 
	mGlobal.RotateY (fDegrees); 
	if (pBoundingShape)
		pBoundingShape->mGlobal.RotateY (fDegrees);
}
void Base::RotateLocalZ (float fDegrees) 
{ 
	mGlobal.RotateZ (fDegrees); 
	if (pBoundingShape)
		pBoundingShape->mGlobal.RotateZ (fDegrees);
}
	
// more sophisticated movement algorithms
void Base::LookAt (Vector& vPoint) 
{ 
	mGlobal.LookAt (vPoint); 
	if (pBoundingShape)
		pBoundingShape->mGlobal.LookAt(vPoint);
}
void Base::FaceDirection (Vector& vDirection) 
{ 
	mGlobal.FaceDirection (vDirection); 
	if (pBoundingShape)
		pBoundingShape->mGlobal.FaceDirection(vDirection);
}
void Base::TurnTo (Vector& vPoint, float fAngle) 
{ 
	mGlobal.TurnTo (vPoint, fAngle); 
	if (pBoundingShape)
		pBoundingShape->mGlobal.TurnTo (vPoint, fAngle);
}
void Base::TurnToHorizontal (Vector& vPoint, float fAngle) 
{ 
	mGlobal.TurnToHorizontal (vPoint, fAngle); 
	if (pBoundingShape)
		pBoundingShape->mGlobal.TurnToHorizontal (vPoint, fAngle);
}
void Base::TurnToVertical (Vector& vPoint, float fAngle) 
{ 
	mGlobal.TurnToVertical (vPoint, fAngle); 
	if (pBoundingShape)
		pBoundingShape->mGlobal.TurnToVertical (vPoint, fAngle);
}

// COLLISION DETECTION
bool Base::CollidesWith (Base* other, Vector* vIntersection)
{
	if (bCollidable && other->bCollidable)
		return (pBoundingShape->CollidesWith (other->pBoundingShape, vIntersection));
	else return false;
}

bool Base::CollidesWith (Line& line, Vector* vIntersection)
{
	if (bCollidable)
		return pBoundingShape->CollidesWith (line, vIntersection);
	else return false;
}

bool Base::CollidesWith (Vector& vPoint)
{
	if (bCollidable)
		return pBoundingShape->CollidesWith (vPoint);
	else return false;
}

bool Base::CollidesWithGeometry (Base* other, Vector* vIntersection, Triangle* pTriangle)
{
	if (!bCollidable || !other->bCollidable || !other->pModel)
		return false;

	// be extra paranoid about empty geometry
	if (!ModelGetVertexBuffer() || !ModelGetIndexBuffer())
		return false;
	
	int iNumCollisions = 0;
	Triangle closestPoly;
	Vector vClosestIntersection, vCurrentIntersection;

	// check collision against each triangle of the other object's geometry
	int iVertexSize = other->ModelGetVertexSize();
	char* pVertexData;
	unsigned short* pIndexData;
	other->ModelGetVertexBuffer()->Lock (0,0, (void**)&pVertexData, D3DLOCK_READONLY);
	other->ModelGetIndexBuffer()->Lock (0,0, (void**)&pIndexData, D3DLOCK_READONLY);

	for (int face=0; face < other->ModelGetNumFaces(); face++)
	{
		Vector v1, v2, v3;		//the positions of the face's vertices
		unsigned short sIndex1 = pIndexData [3*face];
		unsigned short sIndex2 = pIndexData [3*face +1];
		unsigned short sIndex3 = pIndexData [3*face +2];

		memcpy (v1.fComp, pVertexData + (sIndex1 * iVertexSize), 3*sizeof(float));
		memcpy (v2.fComp, pVertexData + (sIndex2 * iVertexSize), 3*sizeof(float));
		memcpy (v3.fComp, pVertexData + (sIndex3 * iVertexSize), 3*sizeof(float));

		Triangle currentPoly (v1,v2,v3);
		if (pBoundingShape->CollidesWith (currentPoly, &vCurrentIntersection))
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
	other->ModelGetVertexBuffer()->Unlock();
	other->ModelGetIndexBuffer()->Unlock();
	
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

// detailed collision detection: line to object
// suggested use: use this after collision with the bounding box returned true
bool Base::CollidesWithGeometry (Line& line, Vector* vIntersection, Triangle* pTriangle)
{
	if (!bCollidable)
		return false;

	// waddya know, DirectX has a function that does this for us!
	BOOL bHit;
	DWORD dwFace;
	float u,v,dist;
	D3DXVECTOR3 vBegin (line.vBegin.fComp);
	D3DXVECTOR3 vDir ( (line.vEnd - line.vBegin).fComp);

	D3DXIntersect(
		pModel->GetMesh(),
		&vBegin,
		&vDir,
		&bHit,
		&dwFace,
		&u,
		&v,
		&dist,
		0,0);

	if (bHit)
	{
		// check if the point of intersection is on the line rather than the ray
		if (dist > line.GetLength())
			return false;

		// get the triangle that the line collided with
		Vector v0,v1,v2;
		char* pBuffer;		//used to read into the vertex data
		unsigned short* sIndices;	//indices of the triangle's vertices
		
		int iVertexSize = pModel->GetVertexSize();
		pModel->GetVertexBuffer()->Lock (0,0, (void**) &pBuffer, D3DLOCK_READONLY);
		pModel->GetIndexBuffer()->Lock (3*sizeof(unsigned short) * dwFace, 3*sizeof(unsigned short), (void**) &sIndices, D3DLOCK_READONLY);
		memcpy (v0.fComp, pBuffer + (iVertexSize * sIndices[0]), 3*sizeof(float));
		memcpy (v1.fComp, pBuffer + (iVertexSize * sIndices[1]), 3*sizeof(float));
		memcpy (v2.fComp, pBuffer + (iVertexSize * sIndices[2]), 3*sizeof(float));
		pModel->GetVertexBuffer()->Unlock();
		pModel->GetIndexBuffer()->Unlock();

		// return the triangle if appropriate
		if (pTriangle)
			*pTriangle = Triangle (v1,v1,v2);

		// calculate the intersection point from the barycentric coordinates u,v
		if (vIntersection)
		{
			*vIntersection = v0 + (v1-v0)*u + (v2-v0)*v;		
		}

		return true;
	}
	return false;
}

bool Base::CollisionClamp (Base* other)
{
	if (!bCollidable || !other->bCollidable)
		return false;

	// if the objects collided, reposition the object
	if (pBoundingShape->CollisionClamp (other->pBoundingShape))
	{
		MoveTo (pBoundingShape->GetPos());
		return true;
	}
	return false;
}

bool Base::CollisionClampGeometry (Base* other)
{
	if (!bCollidable || !other->bCollidable)
		return false;

	// if the objects collided, reposition the object
	Vector vIntersection(0,0,0);
	if (CollidesWithGeometry (other, &vIntersection))
	{
		// prevent gravity from accumulating
		pBoundingShape->CollisionClamp (vIntersection);
		MoveTo (pBoundingShape->GetPos());
		return true;
	}
	return false;
}

bool Base::CollisionPush (Base* other, float fDampFactor)
{
	if (!CollisionClamp (other))
		return false;

	Vector vDirection = vVelocity;
	vDirection.fComp[1] = 0;
	other->MoveBy (vDirection * fDampFactor * Timer::GetElapsedTime());
	return true;
}

void Base::Draw()
{
	if (RContext->bDraw && pModel)
	{
		Direct3D::GetMatrixStack()->Push();
		Direct3D::GetMatrixStack()->LoadMatrixClass(mGlobal);
		Direct3D::GetMatrixStack()->ApplyToWorld();
			pModel->Draw();
		Direct3D::GetMatrixStack()->Pop();
	}
}

void Base::ModelLoad (char* strFile)
{
	//CHANGED
	// not removing old model anymore
	// the user is responsible for doing this manually

	//if (pModel)
	//	delete pModel;
	pModel = new DXModel;
	pModel->Load (strFile);
	fWidth = pModel->GetWidth();
	fHeight = pModel->GetHeight();
	fDepth = pModel->GetDepth();
}

void Base::ModelScale (float fScale)
{
	pModel->Scale (fScale);

	fWidth *= fScale;
	fHeight *= fScale;
	fDepth *= fScale;
	if (fWidth < 0)
		fWidth *= -1;
	if (fHeight < 0)
		fHeight *= -1;
	if (fDepth < 0)
		fDepth *= -1;
}

void Base::ModelScale (float fScalex, float fScaley, float fScalez)
{
	pModel->Scale (fScalex, fScaley, fScalez);

	fWidth *= fScalex;
	fHeight *= fScaley;
	fDepth *= fScalez;
	if (fWidth < 0)
		fWidth *= -1;
	if (fHeight < 0)
		fHeight *= -1;
	if (fDepth < 0)
		fDepth *= -1;
}

void Base::Update()
{
	if (!bUpdate)
		return;

	// keep track of the last position
	vLastPos = GetPos();

	fStateTime += Timer::GetElapsedTime();

	if (pBoundingShape)
	{
		pBoundingShape->mGlobal = mGlobal;
	}
}

























//////////////////////////////////
// LIST FUNCTIONALITY
///////////////////////////////////

		

		// Insert
		// inserts an element into the head of a doubly linked list of base objects
		void BaseList::Insert (Base* newObject)
		{
			iNumNodes++;
			BaseNode* newNode = new BaseNode(newObject, NULL, first);

			// special case: empty list
			if (!first)
				first = last = newNode;

			// general case
			else
			{
				first->prev = newNode;
				first = newNode;
			}
		}

		// InsertAfter
		// inserts an element after a given element
		void BaseList::InsertAfter (Base* newObject, BaseNode* spot)
		{
			++iNumNodes;

			BaseNode* newNode = new BaseNode (newObject, spot, spot->next);
			//special case: end of list
			if (!spot->next)
				last = newNode;
			else spot->next->prev = newNode;
			spot->next = newNode;						
		}

		// DeleteList
		// Destroys all nodes in the list and deallocates their memory
		// does not delete the objects stored in the list, though
		void BaseList::DeleteList()
		{

			BaseNode* current = first;
			while (current)
			{
				BaseNode* temp = current;
				current = current->next;
				delete temp;
			}

			first = last = NULL;
			iNumNodes = 0;
		}

		// DeleteListAndObjects
		// Destroys all nodes and deallocates their memory
		// also deletes the objects stored in the list
		void BaseList::DeleteListAndObjects()
		{
			iNumNodes = 0;

			BaseNode* current = first;
			while (current)
			{
				SAFE_DELETE (current->object);
				BaseNode* temp = current;
				current = current->next;
				delete temp;
			}

			first = last = NULL;
		}

		// Remove
		BaseNode* BaseList::Remove (BaseNode* iterator, bool bForward)
		{
			// special check: prevent dumbasses from deleting from empty lists
			if (!first)
				return NULL;

			iNumNodes--;

			BaseNode* newIterator;

			if (bForward)
				newIterator = iterator->next;
			else newIterator = iterator->prev;	
			
			if (iterator->prev)
				iterator->prev->next = iterator->next;
			//special case: removing the first element
			// CHANGED: setting to 0 instead of next
			else first = 0;

			if (iterator->next)
				iterator->next->prev = iterator->prev;
			//special case: removing the last element
			else last = last->prev;

			delete iterator;		
			return newIterator;
		}



