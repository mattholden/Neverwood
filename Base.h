// Defines the base object from which all other things spring.

#ifndef StormForge_Shared_BaseClass
#define StormForge_Shared_BaseClass

#include "Matrix.h"
#include "MemoryMgr.h"
#include "Vector.h"
#include "BoundingShape.h"
#include "RenderInfo.h"
#include "Timer.h"
#include "DXModel.h"


//#define AGA0 0.01f //as good as zero


#define FAR_SPACE_STORE_POINT 0, -10000, 0
#define BLACKHOLE FAR_SPACE_STORE_POINT 

class Base
{
public:

	// rendering
	DXModel* pModel;
	RenderInfo* RContext;

public:

	////////////////////////////////////////
	// GEOMETRY
	////////////////////////////////////////
		float fWidth, fHeight, fDepth;
		Matrix mGlobal;						// Our matrix as it relates to the global space
		float fRed, fGreen, fBlue;			// the color of the object's vertices

	////////////////////////////////////////
	// COLLISION & MOTION
	////////////////////////////////////////
		Vector vVelocity;						// Where are we going and how fast? (this should be on a per-"frame" basis)
		BoundingShape *pBoundingShape;		// A bounding shape to use for collision
		float fMass;								// For conservation of momentum, knockback, etc.
		Vector vLastPos;				//useful for collision related stuff

		//Chris's stuff
		Vector vInternal;					//self generated velocity
		Vector vExternal;					//outside generated velocity

	////////////////////////////////////////
	// DATA SORTING AND STORAGE
	////////////////////////////////////////
			
		int iObjectID;						// A unique object ID for this item.
		int iObjectType;					// Classification of what this object is - camera, powerup, character, enemy, etc...
		static int siObjectCount;		// How many Base objects are active right now? Used for generating unique IDs.
	
	////////////////////////////////////////
	// FLAGS
	////////////////////////////////////////
			
		// Motion and gameplay
			char bCollidable : 1;				// Should this object be tested for collision?
			char bTakesDamage : 1;		// Can it be damaged or destroyed?
			char bGroundClamped : 1;		// Should this object clamp to the ground as it moves?
			char bGravityAffected : 1;		// Should this object accelerate downward due to gravity?	
			char bUpdate : 1;					// Does this object receive updates?
			char __PADDING_DONT_USE : 3;

	bool bActive;		//a bool to see if object should be in the game or not... useful for creating a bunch of stuff, like arrows, but keep them out of the game when they're not in use.

	/////////////////////////////
	// MEMORY MANAGER
	/////////////////////////////
	//#if _DEBUG
	//	ManageMemory(Base, BASETYPE_BASE);			
	//#endif

	////////////////////////////
	// STATES
	////////////////////////////
	int iCurrentState;			// What state are we in
	float fStateTime;			// How long have we been in it
	
	// Generic constructor for making arrays of Base objects.
	Base(int iObjType = INVALID);

	// Destroy dynamic memory
	Base::~Base(void);

	// MATRIX ACCESSORS
	Vector GetPos(void)	{ return mGlobal.GetPos(); }
	Vector GetZ(void)		{ return mGlobal.GetZ(); }
	Vector GetY(void)		{ return mGlobal.GetY(); }
	Vector GetX(void)		{ return mGlobal.GetX(); }
	float GetXPos(void)	{ return mGlobal.fMatrix[12]; }
	float GetYPos(void)	{ return mGlobal.fMatrix[13]; }
	float GetZPos(void)	{ return mGlobal.fMatrix[14]; }

	
	/////////////////////////////////////////////////
	// MOVEMENT FUNCTIONS
	//////////////////////////////////////////////////
		
	// puts the object to the given position
	virtual void MoveTo (Vector& vPoint); 
	virtual void MoveTo (float x, float y, float z) { MoveTo (Vector (x,y,z)); }

	// translates the object globally by a given amount
	virtual void MoveBy	(Vector& vOffset);
	virtual void MoveBy (float x, float y, float z) 
	{ 
		MoveBy (Vector (x,y,z)); 
	}

	// translates the object loally by a given amount 
	virtual void MoveByLocal (Vector& vOffset);
	virtual void MoveByLocal (float x, float y, float z) { MoveByLocal (Vector (x,y,z)); }

	// moves the vector along its local axes
	virtual void MoveForward (float amount);
	virtual void MoveRight (float amount);
	virtual void MoveUp (float amount);	

	// rotate around the world's axes
	virtual void RotateX (float fDegrees);
	virtual void RotateY (float fDegrees);
	virtual void RotateZ (float fDegrees);
	virtual void Rotate (Vector& vAxis, float fDegrees);

	// rotate around the object's local axes
	virtual void RotateLocalX (float fDegrees);
	virtual void RotateLocalY (float fDegrees);
	virtual void RotateLocalZ (float fDegrees);
	
	// more sophisticated movement algorithms
	virtual void LookAt (Vector& vPoint);
	virtual void FaceDirection (Vector& vDirection);
	virtual void TurnTo (Vector& vPoint, float fAngle);
	virtual void TurnToHorizontal (Vector& vPoint, float fAngle);
	virtual void TurnToVertical (Vector& vPoint, float fAngle);

	//////////////////////////////////////////////////////////////////////////////
	// COLLISION DETECTION / RESPONSE
	//////////////////////////////////////////////////////////////////////////////
	
	// checks if the object's bounding box intersects another. Optionally computes and stores their point of intersection
	virtual bool CollidesWith (Base* other, Vector* vIntersection=0);		
	virtual bool CollidesWith (Line& line, Vector* vIntersection = 0);
	virtual bool CollidesWith (Vector& vPoint);
	// checks the object's bounding box against each polygon in the passed in model's data. Useful for colliding against the world
	// returns the closest point and polygon of intersection if there are multiple intersections
	virtual bool CollidesWithGeometry (Base* other, Vector* vIntersection = 0, Triangle* pTriangle = 0);
	// checks a line against each polygon of this model. Useful for accurate collision detection in sniper mode and for collision against world geometry
	// returns the closest point and polygon of intersection if there are multiple intersections
	virtual bool CollidesWithGeometry (Line& line, Vector* vIntersection = 0, Triangle* pTriangle = 0);

	// ensures that the object's bounding box never overlaps another object's. 
	// also does not move the other object
	// Collision detection is handled as part of this function.
	// this routine is meant to be used for collision with static objects
	// returns true if the invoking object got clamped (i.e. intersected the other object), false otherwise
	virtual bool CollisionClamp(Base *other);
	// ensures that the object's bounding box never overlaps the other object's geometry
	virtual bool CollisionClampGeometry (Base* other);

	// just like collision clamp, with the additional effect that the other object gets moved slightly
	// the direction + magnitude in which the other object gets pushed depends on the velocity of the invoking object
	// the optional damp factor essentially determines how much the invoking object slows down upon collidiing with the other object (0 = complete stop, 1 = no slowdown, only pass in values between 0 and 1, please)
	// this routine is meant to be used for collisions between moving objects, and ensures that moving objects are not in each other's way too much
	virtual bool CollisionPush (Base* other, float fDampFactor = 1.0f);

	//////////////////////////////////////////////////////////////////////////////
	// GEOMETRY AND RENDERING
	//////////////////////////////////////////////////////////////////////////////
	RenderInfo* GetRenderContext() { return RContext; }
	RenderInfo* SetRenderContext (RenderInfo* newContext) { if (RContext) delete RContext; RContext = newContext; }
	virtual void ResetBoundingShapeColor(void){}
	virtual void Draw();
	virtual void ModelLoad (char *strFile);
	void ModelSet (DXModel* newModel) { pModel = newModel; }
	void ModelTranslate (float x, float y, float z) { pModel->Translate (x,y,z); }
	void ModelTranslate (Vector& vTrans) { pModel->Translate (vTrans); }
	void ModelScale (float fScale);
	void ModelScale (float fScalex, float fScaley, float fScalez);
	LPDIRECT3DVERTEXBUFFER9 ModelGetVertexBuffer() { return pModel->GetVertexBuffer(); }
	virtual LPDIRECT3DINDEXBUFFER9 ModelGetIndexBuffer() { return pModel->GetIndexBuffer(); }
	int ModelGetNumVertices() { return pModel->GetNumVertices(); }
	int ModelGetVertexSize() { return pModel->GetVertexSize(); }
	virtual int ModelGetNumFaces() { return pModel->GetNumFaces(); }
	virtual int ModelGetNumTriangles() { return pModel->GetNumTriangles(); }
	int ModelGetTexture(int index=0) { return !pModel ? -1 : pModel->GetTexture (index); }
	float ModelGetWidth() { return pModel->GetWidth(); }
	float ModelGetHeight() { return pModel->GetHeight(); }
	float ModelGetDepth() { return pModel->GetDepth(); }
	float ModelGetXMin() { return pModel->GetXMin(); }
	float ModelGetXMax() { return pModel->GetXMax(); }
	float ModelGetYMin() { return pModel->GetYMin(); }
	float ModelGetYMax() { return pModel->GetYMax(); }
	float ModelGetZMin() { return pModel->GetZMin(); }
	float ModelGetZMax() { return pModel->GetZMax(); }

	//////////////////////////////////////
	// General updating
	/////////////////////////////////////
	virtual void Update(void);

	
};

	/////////////////////////
	// LIST FUNCTIONALITY
	// The following functions have been added to use doubly linked lists of base objects (or children thereof)
	/////////////////////////

	// node structure for a doubly linked list
	struct BaseNode
		{
			Base* object;
			BaseNode* next;
			BaseNode* prev;

			BaseNode (Base* obj, BaseNode* pr, BaseNode* nxt)
			{
				object = obj;
				next = nxt;
				prev = pr;
			}

			// CHANGED:
			// removed this, since it was causing bugs
			//~BaseNode()
			//{
			//	next = prev = NULL;
			//}

		};

	class BaseList
	{
	public:

		// MEMBER VARIABLES	
		int iNumNodes;
		BaseNode* first;
		BaseNode* last;



		// MEMBER FUNCTIONS
		

		// ADDED: Matt 5-10-04
		// Traverse the BaseList and find the next item in it.
		Base *GetNext(Base *pHere)
		{
			BaseNode *pSearch = first;
			while (pSearch != NULL)
			{
				if (pSearch->object == pHere)
					return pSearch->next->object;

				pSearch = pSearch->next;
			}
			return NULL;

		}

		BaseList() { first = last = NULL; iNumNodes = 0;}

		// Insert
		// inserts an element into the head of a doubly linked list of base objects
		void Insert (Base* newObject);

		// InsertAfter
		// inserts an element after a given element
		void InsertAfter (Base* newObject, BaseNode* spot);

		// DeleteList
		// Destroys all nodes in the list
		// does not delete the actual objects stored in the list
		void DeleteList();

		// DeleteListAndObjects
		// Destroys all nodes in the list
		// also deletes the objects stored in those nodes
		void DeleteListAndObjects();

		// Remove
		// Removes a given node from a list
		// assumes that the node is already inside the list
		// 
		// input:	BaseNode* theNode-> a node in the list that you want to remove 
		//			bool forward-> true if you're traversing the list in forward order, false if you're traversing it backwards.
		//
		// CAUTION: Remove only deletes a node from a list. It does not delete the object contained in that node.
		//			If you want to actually destroy the object in the list's node, call delete on that object first!
		//			Why? because sometimes you want to remove an object from a list without destroying it (example: rendering queue)
		//			
		// CAUTION: removing an element messes up the data stored in the iterator, since the iterator points to deleted data after the function returns.
		//			to resolve this, Remove returns the node after the one you've just deleted.
		//
		// example: removing all the elements in a list in reverse order
		// BaseList list;
		// BaseNode* iterator = list.last;
		// while (*iterator)
		//		iterator = list.Remove (iterator, false);
		BaseNode* Remove (BaseNode* iterator, bool bForward = true);
		

	};
	
#endif
