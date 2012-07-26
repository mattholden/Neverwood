/////////////////////////////////////////////////
// World.h
//
// A singleton class defining the entire level that the player is traversing
// As a necessary optimization, the world is partitioned into a 2D grid twice: one partition for collision and one partition for rendering
// the partition for collisions has a small grid size and contains duplicate polygons, whereas the partition for rendering has a large grid size and does not contain duplicate polygons
//
// Defines the world itself

#ifndef StormForge_Shared_World
#define StormForge_Shared_World

#include "Base.h"
#include "WorldSegment.h"
#include "RenderInfo.h"
#include "MemoryMgr.h"
#include "Camera.h"
#include "Utils.h"

// default sizes for rendering and collision partitions
#define COLLISION_GRID_SIZE 20
#define RENDER_GRID_SIZE 100 

#define GRAVITY 30

// level loading stats
#define LEVEL1_NUMTREES 100
#define LEVEL1_NUMCUTTERS 0
#define LEVEL2_NUMTREES 30
#define LEVEL2_NUMBUSHES 40
#define LEVEL2_NUMSCOUTS 0

//////////////////////
class World: public Base
{

private:

	// PARTITIONING DATA
	int iCollisionPartitionSize;		//the width and depth of a world segment in the 2D grid of the partition used for collision
	int iRenderPartitionSize;
	float fInvCollisionPartitionSize;		//optimization: store the inverse instead of computing it
	float fInvRenderPartitionSize;
	int iCollisionCols, iCollisionPlanes;				//the amount of segments in the grid's x directiion and z direction
	int iRenderCols, iRenderPlanes;
	WorldSegment* pCollisionSegments;		//the world's cubical segments	
											//arranged as a 1D array that povides access in a 2D manner
	WorldSegment* pRenderSegments;

	// let's not forget that a level can have a skybox
	Base* pSkyBox;

	// COLLISION DATA
	// It is possible to clip the boundaries of the level if you only want to use a certain subset of the level's geometry
	float fLeftBoundary, fRightBoundary, fFrontBoundary, fBackBoundary;

	//NOTE: it is very expensive and tedious to move/rotate the world, since this involves moving/rotating each vertex in the world's data, as well as each world segment.
	// thus, most move/rotate functions are not implemented for the world
	virtual void MoveTo (Vector& vPoint){}
	virtual void MoveTo (float x, float y, float z) { MoveTo (Vector (x,y,z)); }

	virtual void MoveByLocal (Vector& vOffset){}
	virtual void MoveByLocal (float x, float y, float z) { MoveByLocal (Vector (x,y,z)); }

	virtual void MoveForward (float amount){}
	virtual void MoveRight (float amount){}
	virtual void MoveUp (float amount){}

	virtual void RotateX (float fDegrees){}
	virtual void RotateY (float fDegrees){}
	virtual void RotateZ (float fDegrees){}
	virtual void Rotate (Vector& vAxis, float fDegrees){}

	virtual void RotateLocalX (float fDegrees){}
	virtual void RotateLocalY (float fDegrees){}
	virtual void RotateLocalZ (float fDegrees){}

	virtual void LookAt (Vector& vPoint){}
	virtual void FaceDirection (Vector& vDirection){}
	virtual void TurnTo (Vector& vPoint, float fAngle){}

public:
	// DATA MEMBERS
	Camera* pCamera;

	Vector vGravity;
	RenderInfo* RContext;
	
	World(): Base (BASETYPE_WORLD)
	{
		iCollisionPartitionSize = COLLISION_GRID_SIZE;
		iRenderPartitionSize = RENDER_GRID_SIZE;
		fInvCollisionPartitionSize = 1.0f / iCollisionPartitionSize;
		fInvRenderPartitionSize = 1.0f / iRenderPartitionSize;

		iCollisionCols = iCollisionPlanes = 0;
		iRenderCols = iRenderPlanes = 0;
		pCollisionSegments = pRenderSegments = NULL;
		vGravity = Vector (0, -GRAVITY, 0);

		// init camera
		pCamera = new Camera;
		pCamera->SetViewFrustum (VIEWFRUSTUM_ANGLE, VIEWFRUSTUM_NEARCLIP, VIEWFRUSTUM_FARCLIP, Direct3D::GetAspectRatio());

		pSkyBox = NULL;
	}

	~World() 
	{	
		DELETE_ARRAY (pCollisionSegments);
		DELETE_ARRAY (pRenderSegments);
		SAFE_DELETE (pCamera);
		SAFE_DELETE (pSkyBox);
	}

	// MOVEMENT
	virtual void MoveBy	(Vector& vOffset);
	virtual void MoveBy (float x, float y, float z) { MoveBy (Vector (x,y,z)); }


	// PARTITION
	//
	// splits the world into a grid (i.e. evenly sized cubical segments of a given length)
	// need to call this before doing anything with the world
	void Partition (int collisionGridSize = COLLISION_GRID_SIZE, int renderGridSize = RENDER_GRID_SIZE);

	void ResetBoundingShapeColor(void){}

	// COLLISION DETECTION / RESPONSE
	//
	// the world provides some predefined collision routines that traverse the areas of the world
	// that are near a given object. That way, the user does not have to filter the world's segments manually.
	// collision detection always checks against the world's geometry rather than its bounding box
	virtual bool CollidesWith (Base* object, Vector* vIntersection = 0);
	virtual bool CollidesWith (Line& line, Vector* vIntersection = 0);
	virtual bool CollidesWithGeometry (Line& line, Vector* vIntersection=0, Triangle* pTriangle=0);
	virtual bool CollidesWithGeometry (Base* object, Vector* vIntersection=0, Triangle* pTriangle=0);
	virtual bool CollisionClamp (Base* object);
	virtual bool GroundClamp (Base* object);

	// makes sure an object never leaves the boundaries of the world
	// the object's x and z position is clamped between the world's min and max
	// for the object's y position, the object gets repositioned to the top of the world if it manages to fall through the world
	void KeepInBounds (Base* object);

	// sets up boundaries within the world so that objects can only move within those boundaries
	void SetBoundaries (float xmin, float zmin, float xmax, float zmax);

	// GetAltitude
	// computes how far above the ground an object is
	// Input: the object's position
	// CAUTION: this is an expensive operation, and should not be used on a per-frame basis
	float GetAltitude (Vector& vPoint);
	
	// GetNumPartitions
	// gets the total number of world partitions (collision + rendering)
	int GetNumPartitions() { return iRenderCols * iRenderPlanes + iCollisionCols * iCollisionPlanes; }

	// GetIndex
	// given the global coordinates of a point in the world, the index accessors 
	// provide the means to obtain the proper world segment. There are two different ways to access the world's segments:
	//
	// 1. accessing the world's segments as a 1D array
	// To do so, use GetSegment(int) and pass in the value that GetIndex returns
	//
	// 2. accessing the world's segments as a 2D array
	// To do so, use GetIndexX and GetIndexZ to obtain the proper indices.
	// Then use GetSegment (int,int) on the obtained indices. This is useful for looping through a small section of the world
	//
	// note that passing in a point that is outside of the world will result in an index that is outside of the bounds of the array of world segments
	// however, the user does not need to be concerned with this, since the GetSegment function automatically checks for boundaries.
	int GetCollisionIndexX (Vector& vPos);
	int GetRenderIndexX (Vector& vPos);
	int GetCollisionIndexZ (Vector& vPos);
	int GetRenderIndexZ (Vector& vPos);
	int GetCollisionIndex (Vector& vPos);
	int GetRenderIndex (Vector& vPos);
	
	// GetSegment
	// the following enable the user to index the grid of world segments as a 1D or 3D array
	// if the given indices are outside of the boudaries of the world, GetSegment returns NULL.
	WorldSegment* GetCollisionSegment (int index1d);
	WorldSegment* GetRenderSegment (int index1d);
	WorldSegment* GetCollisionSegment (int xIndex, int zIndex);
	WorldSegment* GetRenderSegment (int xIndex, int zIndex);

	// DRAW
	// uses world partitioning to draw only the areas in pRenderSegments that are inside the camera's frustum
	virtual void Draw ();

	// load a level model
	virtual void ModelLoad (char* strFile);

	// skybox model
	Base* GetSkyBox() { return pSkyBox; }

	//////////////////////////////////////
	// LEVEL MANAGEMENT					//
	//////////////////////////////////////

	// RANDOM LEVEL GENERATION
	//
	// the world can randomly create a specified amount of trees, bushes, scouts, or cutters
	// any entities created by the following functions are automatically offset so that they rest on the ground.
	// there are two versions of random entity generation for each type of entity:
	//		1. Unrestricted - randomly places the entity anywhere in the world
	//		2. Restricted - randomly places entities with coordinates restricted to a specified range.
	//			this is Useful for putting random enemies in a certain area of the level, for example.
	
	// CreateTrees: creating a tree randomly assigns one of several tree models to that tree
	// it also assigns the tree a random orientation along the ground
	#define TREEMODELS 3
	void CreateRandomTrees (int num, float xMin, float zMin, float xMax, float zMax);
	void CreateRandomTrees (int num);
	// CreateBushes: same deal as with CreateTrees, except that it creates bushes instead. Hence that name.
	#define BUSHMODELS 3
	//void CreateRandomBushes (int num, float xMin, float zMin, float xMax, float zMax);
	//void CreateRandomBushes (int num);
	// CreateBushes: same deal as with CreateTrees, except that it creates stumps instead. Hence that name.
	#define STUMPMODELS 5
	void CreateRandomStumps (int num, float xMin, float zMin, float xMax, float zMax);
	void CreateRandomStumps (int num);
	// CreateHuts: places some random huts.
	void CreateRandomHuts (int num, float xMin, float zMin, float xMax, float zMax);
	void CreateRandomHuts (int num);
	// CreateOrcCutters: a created enemy gets random patrol points assigned to it
	void CreateRandomOrcCutters (int num, float xMin, float zMin, float xMax, float zMax);
	void CreateRandomOrcCutters (int num);
	// CreateOrcScouts: 
	void CreateRandomOrcScouts (int num, float xMin, float zMin, float xMax, float zMax);
	void CreateRandomOrcScouts (int num);
	// Create big guys
	void CreateRandomOrcLts (int num, float xMin, float zMin, float xMax, float zMax);
	void CreateRandomOrcLts (int num);
 	
	// LEVEL LOADING
	// level loading uses the random level generation functions above to create a world based on the level number
	// it also deallocates the old level
	void LoadLevel (int iLevel);

	#if _DEBUG
		ManageMemory(World, BASETYPE_WORLD);
	#endif
};


#endif
