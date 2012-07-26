#ifndef StormForge_Matt_DXModel
#define StormForge_Matt_DXModel

#include "MemoryMgr.h"
#include "Direct3D.h"
#include "Direct3DHelpers.h"
#include "TextureMgr.h"

#include "Sphere.h"

		struct D3DXMESHCONTAINER2 : public D3DXMESHCONTAINER
		{
			LPDIRECT3DTEXTURE9 *ppTextures;
			D3DMATERIAL9 *pMaterials9;
			LPD3DXMESH pSkinMesh;
			LPD3DXMATRIX pBoneOffsets;
			LPD3DXMATRIX *ppFrameMatrices;
			LPD3DXATTRIBUTERANGE pAttributeTable;
			DWORD NumAttributeGroups;
		};

		struct D3DXFRAME2 : public D3DXFRAME
		{
			D3DXMATRIX matCombined;
		};

class DXModel
{
private:

	LPD3DXMESH pMesh;

	DWORD dwMaterials;	                       // Total number of materials.
	// CHANGED
	// none of our models have different material properties, so we just don't use these
	D3DMATERIAL9* pMaterials;  // List of materials for the model.
	DWORD *pAdjacency;
	LPD3DXEFFECTINSTANCE pEffects;
	int *pTextureIdx;
    float fWidth, fHeight, fDepth;		//dimensions of the model. Upon loading the model, these are set based on the minimum and maximum vertex positions of the model
	D3DXMESHCONTAINER2 *pMeshCtr;
	float xMin, yMin, zMin;						// the minimum x,y,and z coordinates in the level
    float xMax, yMax, zMax;						// the maximum x,y,and z coordiant
//	float fWidth, fHeight, fDepth;		//dimensions of the model. Upon loading the model, these are set based on the minimum and maximum vertex positions of the model

	// Unique ID tracking.
	static int iModelCount;
	int iModelID;

	// Model Animation data
	D3DXFRAME *pRootFrame;
	D3DXMATRIX *pBones;
	unsigned int uMaxBones;
	int iCurrentAnim;
	int iAnimCount;
 
	ID3DXAnimationController *pController;

	// Animation control stuff
	int iCurrentFrame;			// What frame are we at now
	int iLastFrame;					// What is the last frame of ALL anims
	int iNumAnimations;			// How many anims do we have	
	int iCurrentAnimation;		// What anim are we on now

	// There will be NumAnimations of these
	int *pStartFrames;			// What are the starting frames for each anim
	int *pEndFrames;				// What are the ending frames for each anim

	public:

/*
	// Draw a frame of the animation
	void DrawFrame(D3DXFRAME2 * pFrame)
	{
		D3DXMESHCONTAINER2* pFrameMesh = (D3DXMESHCONTAINER2*) pFrame->pMeshContainer;
		
		while (pFrameMesh)
		{
			 // Get the subset mesh to draw
			LPD3DXMESH pDrawMesh = (pFrameMesh->pSkinInfo) ? pFrameMesh->pSkinMesh : pFrameMesh->MeshData.pMesh;
			for (unsigned int i = 0; i < pFrameMesh->NumMaterials; i++)
			{
				// TODO: Set texture
				pDrawMesh->DrawSubset(i);
			}
			pFrameMesh = (D3DXMESHCONTAINER2*) pFrameMesh->pNextMeshContainer;
		}

		// Check sibling...
		if (pFrame->pFrameSibling)
			DrawFrame((D3DXFRAME2 *) pFrame->pFrameSibling);
		if (pFrame->pFrameFirstChild)
			DrawFrame((D3DXFRAME2 *) pFrame->pFrameFirstChild);
	}

	void BoneSetup(D3DXFRAME2 * pFrame, D3DXMATRIX *pParent)
	{
		D3DXMESHCONTAINER2* pFrameMesh = (D3DXMESHCONTAINER2*) pFrame->pMeshContainer;
		
		if (pFrameMesh)
		{
			// First mesh
			if (pMeshCtr)
				pMeshCtr = pFrameMesh;

			// If there's a skin, set up bones
			if (pFrameMesh->pSkinInfo)
			{
				// Copy the mesh
				pFrameMesh->MeshData.pMesh->CloneMeshFVF(D3DXMESH_MANAGED, pFrameMesh->MeshData.pMesh->GetFVF(), Direct3D::GetDevice(), &pFrameMesh->pSkinMesh);
				
				// Get number of bones
				if (uMaxBones < pFrameMesh->pSkinInfo->GetNumBones())
					uMaxBones = pFrameMesh->pSkinInfo->GetNumBones();

				D3DXFRAME2 * pTemp = NULL;

				for (unsigned int u = 0; u < pFrameMesh->pSkinInfo->GetNumBones(); u++)
				{
					
					// Get bone's frame
					pTemp = (D3DXFRAME2 *)D3DXFrameFind(pRootFrame, pFrameMesh->pSkinInfo->GetBoneName(u));
					// Assign bone frame
					pFrameMesh->ppFrameMatrices[u] = &pTemp->matCombined;
				}
			}
		}
		if (pFrame->pFrameSibling)
			BoneSetup((D3DXFRAME2 *)pFrame->pFrameSibling, pParent);
		if (pFrame->pFrameFirstChild)
			BoneSetup((D3DXFRAME2 *)pFrame->pFrameFirstChild, pParent);
	}

	void UpdateFrameMatrix(D3DXFRAME2 *pFrame, D3DXMATRIX *pParent)
	{
		if (pParent)
			D3DXMatrixMultiply(&pFrame->matCombined, &pFrame->TransformationMatrix, pParent);
		else
			pFrame->matCombined = pFrame->TransformationMatrix;

		// Apply to siblings
		if (pFrame->pFrameSibling)
			UpdateFrameMatrix((D3DXFRAME2 *)pFrame->pFrameSibling, pParent);
		if (pFrame->pFrameFirstChild)
			UpdateFrameMatrix((D3DXFRAME2 *)pFrame->pFrameFirstChild, &pFrame->matCombined);

	}

	void Update(int iFrame)
	{
		static int i = 0;

		if (pController && iCurrentAnim != -1)
			pController->SetTime((float) iFrame / (float) iLastFrame);

		if (pRootFrame)
		{
			UpdateFrameMatrix((D3DXFRAME2 *)pRootFrame, 0);

			// Get first mesh
			D3DXMESHCONTAINER2* pFrameMesh = pMesh;
			if (pFrameMesh)
			{
				if (pFrameMesh->pSkinInfo)
				{
					// Get number of bones
					unsigned int uBones = pFrameMesh->pSkinInfo->GetNumBones();

					// Multiply matrices for all the bones
					for (unsigned int u = 0; u < uBones; ++u)
					{
						D3DXMatrixMultiply(&pBones[u], &pFrameMesh->pBoneOffsets[u], pFrameMesh->ppFrameMatrices[u]);
					}

					void *pSource;
					void *pDest;

					// Lock mesh vertex buffers
					pFrameMesh->MeshData.pMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pSource);
					pFrameMesh->pSkinMesh->LockVertexBuffer(0, (void**)&pDest);

					// Update.
					pFrameMesh->pSkinInfo->UpdateSkinnedMesh(pBones, 0, pSource, pDest);
	
					// Unlock.
					pFrameMesh->pSkinMesh->UnlockVertexBuffer();
					pFrameMesh->MeshData.pMesh->UnlockVertexBuffer();
				}
			}
		}
	}

	
	// See if the animation is over
	bool IsAnimationOver(void) 	{ 	return (iCurrentFrame >= iLastFrame); 	}

	// Stop animation
	void Stop(void)
	{
		iCurrentAnimation = 0;
		iCurrentFrame = 0;
		iLastFrame = 0;

		// Todo: Update
	}

	int GetCurrentFrame(void) { return iCurrentFrame; }
	int GetCurrentAnimation(void) { return iCurrentAnimation; }
	void SetAnimationCount(int iCount) { iNumAnimations = iCount; }

	void LoadX(char *strFIle)
	{
		DXHierarchy dxhRead;

		D3DXLoadMeshHierarchyFromX(strFile, D3DXMESH_MANAGED, Direct3D::GetDevice(), &dxhRead, 0, &pRootFrame, &pController);

		if (pRootFrame)
		{
			BoneSetup((D3DXFRAME2 *)pRootFrame, 0);
			pBones = new D3DXMATRIX[uMaxBones];
			memset(pBones, 0, sizeof(D3DXMATRIX) * uMaxBones);
		}
	}

	void SetAnimation(int iAnimation)
	{
		iCurrAnim = iAnimation;
		LPD3DXANIMATIONSET pAnimSet = 0;
		pController->GetAnimationSet(iCurrentAnim, &pAnimSet);
		pController->SetTrackAnimationSet(0, pAnimSet);
		SAFE_RELEASE(pAnimSet);
	}

	void Update(void)
	{
		// Reset animation if we need to
		if (IsAnimationOver())
		{	
			iCurrentFrame = pStartFrames[iCurrentAnimation];
			SetFrame(iCurrentFrame);
		}

		// Todo: update

		// Hack the time
		iCurrentFrame += 7;

	}

	DXModel(void) 
	{	
		pMesh = NULL;
		dwMaterials = 0;
		pMaterials = NULL;
		pTextureIdx = NULL;
		iModelID = iModelCount++;
	}

	*/

	~DXModel(void)
	{
		Delete();
	}

	D3DMATERIAL9 *GetMaterial(int iIndex)
	{
		return &pMaterials[iIndex];
	}

	void Delete(void)
	{
		SAFE_RELEASE(pMesh);
		DELETE_ARRAY (pMaterials);
		dwMaterials = 0;
		DELETE_ARRAY (pTextureIdx);
		// TO DO: delete additional stuff...
	}

	// Generate a bounding sphere...
	// NOTE: This makes a shape with new, so make sure to delete it!
	// Good thing the base class does it for you.
	//
	// D3DXComputeBoundingSphere:
	//		ms-help://MS.VSCC/MS.DirectX9.1033/DirectX9_c/directx/graphics/reference/d3dx/functions/mesh/d3dxcomputeboundingsphere.htm
	/*Sphere* ComputeBoundingSphere(void)
	{
		Sphere *pSphere = new Sphere;
		D3DXVECTOR3 vCenter;
		float fRadius;
		LPDIRECT3DVERTEXBUFFER9 pVB;
		pMesh->GetVertexBuffer(&pVB);
		D3DXComputeBoundingSphere(pVB, GetNumVertices(), pMesh->GetNumBytesPerVertex(), &vCenter, &pSphere->fWidth);
		pSphere->mGlobal.SetPos(*(Vector*)(&vCenter));
		return pSphere;

	}*/

	float GetWidth() { return fWidth; }
	float GetHeight() { return fHeight; }
	float GetDepth() { return fDepth; }
	float GetXMin() { return xMin; }
	float GetXMax() { return xMax; }
	float GetYMin() { return yMin; }
	float GetYMax() { return yMax; }
	float GetZMin() { return zMin; }
	float GetZMax() { return zMax; }

	// scale a model
	// multiplies all the vertex positions in the model by a given factor
	void Scale (float fScale);
	void Scale (float fScalex, float fScaley, float fScalez);

	// translate model
	void Translate (float x, float y, float z);
	void Translate (Vector& vTrans) { Translate (vTrans[0], vTrans[1], vTrans[2]); }

	////////////////////////////////////////
	// Accessors to get handy mesh stuff:
	// 
	LPD3DXMESH GetMesh(void) { return pMesh; }
	int GetNumVertices(void) { return int (pMesh->GetNumVertices()); }	
	int GetNumFaces(void) { return int (pMesh->GetNumFaces()); }
	int GetNumTriangles(void) { return int (pMesh->GetNumFaces()); }
	int GetVertexSize(void) { return int (pMesh->GetNumBytesPerVertex()); }
	DWORD GetFVF(void)	{ return pMesh->GetFVF(); }
	int GetTexture (int index) { if (index<0 || index >= (int)dwMaterials) return -1; return pTextureIdx[index]; }
	//D3DMATERIAL9* GetMaterial (int index) { return pMaterials + index; }

	void SetMaterialColor(float fRed = 1.0f, float fGreen = 1.0f, float fBlue = 1.0f, float fAlpha = 1.0f,int iIndex = 0, bool bDiffuseToo = true)
	{
		D3DCOLORVALUE d3dColor;
		d3dColor.a = fAlpha;
		d3dColor.r = fRed;
		d3dColor.g = fGreen;
		d3dColor.b = fBlue;
		GetMaterial(iIndex)->Ambient = d3dColor;
		if (bDiffuseToo)
		GetMaterial(iIndex)->Diffuse = d3dColor;

	}

	///////////////////////////////////////////
	// Accessors for actual mesh data
	//
	//// GetVertexBuffer:
	//// ms-help://MS.VSCC/MS.DirectX9.1033/DirectX9_c/DirectX/graphics/reference/d3dx/interfaces/id3dxmesh/GetVertexBuffer.htm
	LPDIRECT3DVERTEXBUFFER9 GetVertexBuffer(void) { LPDIRECT3DVERTEXBUFFER9 pBuffer; pMesh->GetVertexBuffer(&pBuffer); return pBuffer; }
	LPDIRECT3DINDEXBUFFER9  GetIndexBuffer(void) { LPDIRECT3DINDEXBUFFER9 pIndexBuffer; pMesh->GetIndexBuffer (&pIndexBuffer); return pIndexBuffer; }

	////////////////////////////////////////
	// Draw the model...
	// DrawSubset:
	//		ms-help://MS.VSCC/MS.DirectX9.1033/DirectX9_c/DirectX/graphics/reference/d3dx/interfaces/id3dxmesh/DrawSubset.htm
	

	// CHANGED
	// included setting the FVF
	// Rolf, 6-7-04
	void Draw(void);

	// drawing only a specified subset of the mesh
	void DrawSubset (int subset);
	
	// Load up a model file.
	// D3DXLoadMeshFromX:
	//    ms-help://MS.VSCC/MS.MSDNVS/dx8_c/directx_cpp/Graphics/Reference/CPP/D3DX/Functions/Mesh/D3DXLoadMeshFromX.htm
	bool Load(char *strFile);
	
public:
#if _DEBUG
	ManageMemory(DXModel, CORETYPE_DXMODEL);
#endif

	
};


#endif
