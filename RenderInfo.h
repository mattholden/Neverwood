//////////////////////////
// CHANGED
// Removed almost everything from this class, since we weren't using any of it

// Defines a RenderInfo class, which will contain information about our rendering context
// for purposes of providing the renderer with a means to sort geometry.

#ifndef StormForge_Shared_RenderInfo
#define StormForge_Shared_RenderInfo

#include <Windows.h>
#include "MemoryMgr.h"

// *Hand wave* This is not the header you're looking for...
class Base;
class DXModel;

class RenderInfo
{
public:
			bool bDraw;					//whether or not to draw the object at all
			bool bDebugGeometry;		//when this is set, the object's bounding shape, matrix, view frustum, and aim frustum get drawn
			bool bDrawAttackFrustum;	//whether or not to draw the character's range of attack
										//this flag only applies to character classes
										//very useful for testing / tweaking combat
			// NOTE: the following variables are updated every frame in Game::Update
			bool bInCameraFrustum;		//marks whether or not an object is inside the camera's view frustum this frame
										//this is an optimization that prevents us from checking against the camera frustum multiple times
			float fDistSquared;			//the squared distance of the object from the camera. Again, this is used to avoid recomputation					
			
			///////////////////////////////////
			// Manage memory
			///////////////////////////////////
			#if _DEBUG
				ManageMemory(RenderInfo, CORETYPE_RENDERER_RENDERINFO);
			#endif

			// Set base object default parameters.
			RenderInfo(void)
			{
				bDraw = true;
				bDebugGeometry = false;
				bInCameraFrustum = false;
				bDrawAttackFrustum = false;
				fDistSquared = 527527;
			}
};
#endif
