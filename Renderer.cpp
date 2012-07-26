
#include "Renderer.h"
#include "Game.h"
#include "Direct3D.h"
#include "TextureMgr.h"
#include "Light.h"
#include "D3DXFont.h"

// Set up the Render Manager, which will set up and store common info about our window.
 Renderer *RenderManager::pRender[2];
 int RenderManager::iWhiteTextureID = INVALID;
 int RenderManager::iMode = RENDER_FULL;


 // Set up rendering in D3D, default to non split screen
 void RenderManager::Initialize()
	{ 	
		// Load white placeholder texture.
		iWhiteTextureID = TextureMgr::LoadTexture("Textures\\White4x4.bmp");        

		// Set up our child renderer. Always just set to full at first... we'll split later.
		pRender[0] = new Renderer;
		pRender[0]->Initialize(RENDER_FULL);
		pRender[1] = new Renderer;
		pRender[1]->Initialize(RENDER_NONE);

		Direct3D::Viewport(VIEWFRUSTUM_NEARCLIP, VIEWFRUSTUM_FARCLIP);
		Direct3D::Perspective(VIEWFRUSTUM_ANGLE, VIEWFRUSTUM_NEARCLIP, VIEWFRUSTUM_FARCLIP);		
	}

	void RenderManager::SetMode (int mode)
	{
		// check if changes are even necessary
		if (iMode == mode)
			return;

		// case 1: switching to full screen display when currently drawing to half the screen
		if (mode == RENDER_FULL)
		{
			Direct3D::Viewport(VIEWFRUSTUM_NEARCLIP, VIEWFRUSTUM_FARCLIP);
			Direct3D::Perspective(VIEWFRUSTUM_ANGLE, VIEWFRUSTUM_NEARCLIP, VIEWFRUSTUM_FARCLIP);

			GetCamera(0)->SetViewFrustum (VIEWFRUSTUM_ANGLE, VIEWFRUSTUM_NEARCLIP, VIEWFRUSTUM_FARCLIP, Direct3D::GetAspectRatio());
			GetCamera(1)->SetViewFrustum (VIEWFRUSTUM_ANGLE, VIEWFRUSTUM_NEARCLIP, VIEWFRUSTUM_FARCLIP, Direct3D::GetAspectRatio());
		}


		float fHalfAspect = (2.0f * Direct3D::GetScreenWidth()) / Direct3D::GetScreenHeight();

		// case 2: switching to top half
		if (mode == RENDER_TOP)
		{
			Direct3D::Viewport(VIEWFRUSTUM_NEARCLIP, VIEWFRUSTUM_FARCLIP, 0, 0, 0, (Direct3D::GetScreenHeight() >> 1));
			if (iMode == RENDER_FULL)
			{
				Direct3D::Perspective(VIEWFRUSTUM_ANGLE * 0.5f, VIEWFRUSTUM_NEARCLIP, VIEWFRUSTUM_FARCLIP, fHalfAspect);
				GetCamera(0)->SetViewFrustum (VIEWFRUSTUM_ANGLE * 0.5f, VIEWFRUSTUM_NEARCLIP, VIEWFRUSTUM_FARCLIP, fHalfAspect);
				GetCamera(1)->SetViewFrustum (VIEWFRUSTUM_ANGLE * 0.5f, VIEWFRUSTUM_NEARCLIP, VIEWFRUSTUM_FARCLIP, fHalfAspect);
			}	
		}

		// case 3: switching to bottom half when currently displaying to full screen
		if (mode == RENDER_BOTTOM)
		{
			Direct3D::Viewport(VIEWFRUSTUM_NEARCLIP, VIEWFRUSTUM_FARCLIP, 0, (Direct3D::GetScreenHeight() >> 1), 0, (Direct3D::GetScreenHeight() >> 1));
			if (iMode == RENDER_FULL)
			{
				Direct3D::Perspective(VIEWFRUSTUM_ANGLE * 0.5f, VIEWFRUSTUM_NEARCLIP, VIEWFRUSTUM_FARCLIP, fHalfAspect);
				GetCamera(0)->SetViewFrustum (VIEWFRUSTUM_ANGLE * 0.5f, VIEWFRUSTUM_NEARCLIP, VIEWFRUSTUM_FARCLIP, fHalfAspect);
				GetCamera(1)->SetViewFrustum (VIEWFRUSTUM_ANGLE * 0.5f, VIEWFRUSTUM_NEARCLIP, VIEWFRUSTUM_FARCLIP, fHalfAspect);
			}
		}

		iMode = mode;
	}

	void RenderManager::DrawAxes(Matrix &mMatrix)
	{
		Vector vPos = mMatrix.GetPos();
		
		D3DVertex vLines[6];
		
		memset(vLines[0].fPosition, 0, 12);
		memset(vLines[2].fPosition, 0, 12);
		memset(vLines[4].fPosition, 0, 12);

		memcpy(vLines[1].fPosition, Vector(1,0,0).fComp, 12);
		memcpy(vLines[3].fPosition, Vector(0,1,0).fComp, 12);
		memcpy(vLines[5].fPosition, Vector(0,0,1).fComp, 12);

		vLines[0].dwDiffuseColor = RGB(255,0,0);
		vLines[1].dwDiffuseColor = RGB(255,0,0);
		vLines[2].dwDiffuseColor = RGB(0,255,0);
		vLines[3].dwDiffuseColor = RGB(0,255,0);
		vLines[4].dwDiffuseColor = RGB(0,0,255);
		vLines[5].dwDiffuseColor = RGB(0,0,255);

		//Direct3D::ColorVertex(true);
		Direct3D::GetDevice()->SetFVF(D3DVERTEXFVF);
		Direct3D::GetMatrixStack()->Push();	
		Direct3D::GetMatrixStack()->LoadMatrixClass(mMatrix);
		Direct3D::GetMatrixStack()->ApplyToWorld();
		Direct3D::GetDevice()->DrawPrimitiveUP(D3DPT_LINELIST, 3, vLines, sizeof(D3DVertex));
		Direct3D::GetMatrixStack()->Pop();
	}

// Set up the Renderer.
void Renderer::Initialize(int iRenderMode)
	{
		// We haven't done any frames yet
		iFrameCounter = 0;
        
		// make a camera
		pCamera = new Camera;
	
		// Let RenderInfo constructor give our renderer the same default params as a
		// Base has.
		RContext = new RenderInfo;
	}
	

// Shut down the Renderer.
void Renderer::Shutdown(void)
{
	//lTransparentList.DeleteAll();
	lOpaqueList.DeleteList();

	delete RContext;
	if (pCamera)
	{
		delete pCamera;
		pCamera = NULL;
	}
}


void Renderer::SetRenderPriority (Base* object)
{
	float fDistSquared = (object->GetPos() - pCamera->GetPos()).LengthSquared();
	int priority = (object->ModelGetTexture() << 24) - short (fDistSquared * 100);
	object->GetRenderContext()->iPriority = priority;
}

	// Add an object to its appropriate list.
	void Renderer::Enqueue(Base *pBase)
	{
		RenderInfo* rcontext = pBase->GetRenderContext();
		if (rcontext->bDraw && !rcontext->bInRenderList)
		{
			SetRenderPriority (pBase);	
			//if (pBase->GetRenderContext()->bAlpha)
			//	QueueInsert(&lTransparentList, pBase);
			//else
			QueueInsert(&lOpaqueList, pBase);
		}
	}

	// Actually do the whole drawing of everything.
	void Renderer::Render(void)
	{
		// Set the camera
		if (pCamera)
			pCamera->ApplyTransform();
			
		//2. draw non-transparent objects first
		DrawList (lOpaqueList);

		//3. draw wireframe debug information
		BaseNode* pCurrentNode = lOpaqueList.first;

		while (pCurrentNode)
		{
			Base* pCurrentObject = pCurrentNode->object;
			// draw bounding boxes
			if (pCurrentObject->GetRenderContext()->bDrawBoundShape)
			{
				if (pCurrentObject->pBoundingShape)
				{
					Direct3D::Wireframe(true);
					Direct3D::Culling(false);
					TextureMgr::Set (0);
					pCurrentObject->pBoundingShape->Draw(); 
					Direct3D::Wireframe(false);
					Direct3D::Culling(true);
				}
			}
			
			// draw axes
			if (pCurrentObject->GetRenderContext()->bDrawAxes)
			{
				Direct3D::Lighting(false);
				Direct3D::Culling (false);
				RenderManager::DrawAxes (pCurrentObject->mGlobal);
				Direct3D::Lighting(true);
 				Direct3D::Culling (true);
			}

			pCurrentNode = pCurrentNode->next;
		}

		//4. draw transparent objects
		//DrawList (lTransparentList);

		
		// 6.Take anything not renderered this frame out of the queues, if it's time.
			iFrameCounter++;
			if (iFrameCounter >= RENDERER_QUEUE_CLEANUP_DELAY) 
			{
				//CleanList(lTransparentList);
				CleanList(lOpaqueList);
				iFrameCounter = 0;
			}	
		}


		// Add to the queue that we pass in the first node of.
		void Renderer::QueueInsert(BaseList* queue, Base* object) 
		{
			object->GetRenderContext()->bInRenderList = true;
			object->GetRenderContext()->bRenderedThisFrame = true;

			// special case: empty queue
			if (!queue->first)
				queue->Insert (object);

			// special case: first element
			else if (object->GetRenderContext()->iPriority >= queue->first->object->GetRenderContext()->iPriority)
				queue->Insert (object);

			// general case
			else
			{
				//find the element in the list after which to insert
				BaseNode* iterator = queue->first;
				while (iterator->next)
				{
					if (object->GetRenderContext()->iPriority >= iterator->next->object->GetRenderContext()->iPriority)
						break;
					iterator = iterator->next;
				}

				//insert
				queue->InsertAfter (object, iterator);
			}
		}

		// Traverse a list and draw its contents.
		void Renderer::DrawList(BaseList& list)
		{
			if (GetAsyncKeyState ('R'))
				int blah=0;
			BaseNode *pIter = list.first;
			while (pIter != NULL)
			{
				if (pIter->object->GetRenderContext()->bDraw == 0)
				{
					pIter = pIter->next;
					continue;
				}
				//MatchContext(pIter->object->GetRenderContext());
				pIter->object->Draw();
				pIter = pIter->next;
			}
		}
			
		// Prune anything that needs to be pruned out of the queues.
		void Renderer::CleanList(BaseList& list)
		{		
			BaseNode *pIter = list.first;
			while (pIter != NULL)
			{
				RenderInfo* RenderContext = pIter->object->GetRenderContext();
				if (!RenderContext->bRenderedThisFrame)
				{
					// If it wasn't drawn, get it out of our queue.
					RenderContext->bInRenderList = false;
					pIter = list.Remove (pIter);					
				}
				else
				{
					// If it was drawn, set this to false for next frame.
					RenderContext->bRenderedThisFrame = false;
					pIter = pIter->next;		//yes, I put this inside the else statement on purpose
				}
			}
		}

