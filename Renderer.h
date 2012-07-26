

#ifndef StormForge_Renderer
#define StormForge_Renderer

#include "RenderInfo.h"
#include "MemoryMgr.h"
#include "Base.h"
#include "Direct3D.h"
#include "MathUtils.h"
#include "Light.h"
#include "Camera.h"
//#include "Action.h"		<evil grin>

enum eRenderMode
{
	RENDER_TOP,			//top half of the window
	RENDER_BOTTOM,		//bottom half of the 
	RENDER_FULL,		//the entire window
	RENDER_NONE,		//renderer not used
};

// Defines the number of frames we allow to pass before we clean queues
// if 1, do it every frame
#define RENDERER_QUEUE_CLEANUP_DELAY 100


class Renderer
{
	friend class RenderManager;
private:
	
	//BaseList lTransparentList;		// Transparent objects are sorted into this list.
	int iFrameCounter;				// Used to determine when to clean queues

	// Parameters are used for split-screen mode
		void Initialize(int iMode = RENDER_FULL);

	// Shut down the Renderer.
		void Shutdown(void);

	// Add to the queue that we pass in the first node of.
		void QueueInsert (BaseList* queue, Base* object);

	// Traverse a list and draw its contents.
		void DrawList(BaseList& list);

	// Prune anything that needs to be pruned out of the queues.
		void CleanList(BaseList& list);

	// Match our Rendering Context info to the passed-in context and set OpenGL to it.
	//	void MatchContext(RenderInfo *pContext);
	
	// calculates and fills in the iPriority value of an object's rendering context
		void SetRenderPriority (Base* object);

public:
	BaseList lOpaqueList;			// List of nontransparent objects

	Camera *pCamera;				// The camera that this renderer is drawing.
	RenderInfo* RContext;			// The current context of how the Renderer is configured.
									// changing the renderer's context changes the context for all the objects the renderer is drawing

	// Add an object to the list of objects to be rendered.
		void Enqueue(Base *pBase);

	// Actually do the whole drawing of everything.
		void Renderer::Render(void);

	// Manage memory
		#if _DEBUG
			ManageMemory(Renderer, CORETYPE_RENDERER);
		#endif
};

// This class exists to provide global access to our twin renderers.
// It also handles global OpenGL setup.
class RenderManager
{
friend class Renderer;
private:
	static Renderer *pRender[2];
	static int iWhiteTextureID;
	static int iMode;
	
public:
	// Simple accessors. Nothing to get excited about.
	static Renderer *Get(int i = 0) { return pRender[i]; }
	static Camera* GetCamera (int i=0) { return pRender[i]->pCamera; }

	// Set up rendering in D3D, default to non split screen
	static void Initialize();

	// Set up for drawing to the top half of the screen, bottom half, or the entire screen
	// Handles changes to the viewport, projection matrix, and camera(s)
	//
	// input:	RENDER_TOP-> set up for drawing to the top half of the window
	//			RENDER_BOTTOM-> bottom half
	//			RENDER_FULL-> the entire window
	static void SetMode (int mode);

	static void DrawAxes (Matrix& mGlobal);

	// Kill the renderers
	static void Shutdown(void)
	{
		for (int i = 0; i < 2; i++)
		{
			if (pRender[i])
			{
				pRender[i]->Shutdown();
				delete pRender[i];
				pRender[i] = NULL;
			}
		}
	}
};


#endif
