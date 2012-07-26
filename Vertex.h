
// Defines a simple base vertex structure
// The layout of it coordinates with the OpenGL glInterleavedArrays option GL_T2_C4_N3_V3
// For the glInterleavedArrays "stride" parameter, pass in VERTEX_STRIDE.
// DO NOT USE DYNAMIC MEMORY IN THIS STRUCTURE FOR ANY REASON.

#ifndef StormForge_Matt_Vertex
#define StormForge_Matt_Vertex 

#include "MemoryMgr.h"

class Vertex
{
public:

	float fTextureCoord[2];
	float fColor[4];
	float fNormal[3];
	float fPosition[3];

	// Place any additional member variables BELOW this line!
	// USE OF DYNAMIC MEMORY IN THIS STRUCTURE IS A ONE-WAY TICKET TO PAIN AND FIERY DEATH!

	// Manage memory
	#if _DEBUG
		ManageMemory(Vertex, MATH_VERTEX);
	#endif

	// Start empty
	Vertex(void) { memset(this, 0, sizeof(*this)); }
	void Clear(void) { memset(this, 0, sizeof(*this)); }
	
};

#endif
