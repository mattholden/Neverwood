
#include "Frustum.h"
#include "Vector.h"
#include "DXModel.h"
#include "Base.h"

/////////////////////
// WorldSegment
//
// As a necessary optimization, the world gets partitioned into a 2D grid.
// The WorldSegment structure stores the geometry in a segment of the grid
//
// the world segment structure is designed for memory efficiency, since there will be A LOT of them
//////////////////////
class WorldSegment
{

public:

	Matrix* pWorldMatrix;		//pointer to the position of the world
								//this is a hack: I needed to get the world's position to draw a world segment appropriately, since the world segment's data is local to the world's position
								//however, I cannot include world.h to get the world's data, since this would lead to a circular include

	float fHalfWidth, fHalfDepth;
	float x,z;

	// geometry / rendering data
	DXModel* pModel;
	LPDIRECT3DINDEXBUFFER9 pIndexBuffer;
	int iNumIndices;	

	WorldSegment ()
	{
		pModel = NULL;
		pIndexBuffer = 0;
		iNumIndices = 0;
	}

	~WorldSegment()
	{
		// since a world segment uses the same model as the world, the model should not get deleted
		SAFE_RELEASE (pIndexBuffer);
		iNumIndices = 0;
	}

	// convenience methods
	Vector GetPos() { return Vector (x,0,z); }
	void MoveTo (float x, float z);
	void MoveTo (Vector& vNewPos) { MoveTo (vNewPos.fComp[0], vNewPos.fComp[2]); }
	void MoveBy (float x, float z);
	void MoveBy (Vector& vMove) { MoveBy (vMove.fComp[0], vMove.fComp[2]); }
	Plane GetRightPlane();
	Plane GetLeftPlane();
	Plane GetBackPlane();
	Plane GetFrontPlane();
	int ModelGetNumFaces() { return iNumIndices / 3; }
	int ModelGetNumTriangles() { return iNumIndices / 3; }
	LPDIRECT3DVERTEXBUFFER9 ModelGetVertexBuffer() { return pModel->GetVertexBuffer(); }
	LPDIRECT3DINDEXBUFFER9 ModelGetIndexBuffer() { return pIndexBuffer; }
	int ModelGetVertexSize() { return pModel->GetVertexSize(); }

	// collision detection
	// note that world segments do not support point of intersection. The 2nd parameter is included only for compatibility with the base class
	bool CollidesWith (Base* other, Vector* vIntersection = 0);
	bool CollidesWith (Line& line, Vector* vIntersection = 0);
	bool CollidesWith (Triangle& triangle);
	bool CollidesWith (Vector& vPoint);
	bool CollidesWith (Frustum* frustum);	//optimized collision because we use it a LOT
	bool CollidesWithGeometry (Base* object, Vector* vIntersection=0, Triangle* pTriangle=0);
	bool CollidesWithGeometry (Line& line, Vector* vIntersection=0, Triangle* pTriangle=0);
	bool CollisionClampGeometry (Base* object, Vector* vIntersection=0, Triangle* pTriangle=0);
	
	// draw
	// a world segment uses the world's vertex data combined with its own indices
	// it does so by using DrawIndexedPrimitives
	void Draw();

	//#if _DEBUG
	//	ManageMemory(WorldSegment, BASETYPE_WORLDSEGMENT);
	//#endif
};