
#include "DXModel.h"

int DXModel::iModelCount = 0;

	// Load up a model file.
	// D3DXLoadMeshFromX:
	//    ms-help://MS.VSCC/MS.MSDNVS/dx8_c/directx_cpp/Graphics/Reference/CPP/D3DX/Functions/Mesh/D3DXLoadMeshFromX.htm
	bool DXModel::Load(char *strFile)
	{
		//	TODO: ?
		//ms-help://MS.VSCC/MS.MSDNVS/dx8_vb/directx_vb/Graphics/Reference/VB/D3DX/Classes/D3DXSkinMesh/D3DXSkinMesh.htm

		LPD3DXBUFFER pMaterialBuffer, pAdj, pFX;

		// Load the model in from the file		
		if (FAILED(D3DXLoadMeshFromX(strFile, D3DXMESH_SYSTEMMEM, Direct3D::GetDevice(),
			&pAdj, &pMaterialBuffer, &pFX, &dwMaterials, &pMesh))) return false;

		// Make memory to hold our stuff
		pMaterials = new D3DMATERIAL9[dwMaterials];
		pTextureIdx = new int[dwMaterials];

		// Get the buffers from LoadMesh...
		D3DXMATERIAL *pBuffMaterial = (D3DXMATERIAL*)pMaterialBuffer->GetBufferPointer();
		pAdjacency = (DWORD*)pAdj->GetBufferPointer();
		pEffects = (LPD3DXEFFECTINSTANCE)pFX->GetBufferPointer();

		// Load up each texture
		for (DWORD i = 0; i < dwMaterials; i++)
		{
			pMaterials[i] = pBuffMaterial[i].MatD3D;
			pMaterials[i].Ambient = pMaterials[i].Diffuse;
			
			// Load textures.  (See how we're gonna draw..)
			pTextureIdx[i] = TextureMgr::LoadTexture(pBuffMaterial[i].pTextureFilename);
		}

		// May need to consider the following stuff to keep models together:
		// ms-help://MS.VSCC/MS.DirectX9.1033/DirectX9_c/DirectX/graphics/reference/d3dx/interfaces/id3dxmesh/GenerateAdjacency.htm

		// Just because we can...
		//    ms-help://MS.VSCC/MS.DirectX9.1033/DirectX9_c/directx/graphics/reference/d3dx/interfaces/id3dxmesh/optimizeinplace.htm
		pMesh->OptimizeInplace(D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT, NULL, NULL, NULL, NULL);
        
		// Free the buffer we used
		SAFE_RELEASE(pMaterialBuffer);
		SAFE_RELEASE(pFX);
		SAFE_RELEASE(pAdj);
		
		// get the dimensions of the model
		int iVertexSize = pMesh->GetNumBytesPerVertex();
		char* pVertexData;
		pMesh->LockVertexBuffer (D3DLOCK_READONLY, (void**)&pVertexData);
		memcpy (&xMin, pVertexData, sizeof(float));
		memcpy (&yMin, pVertexData + sizeof(float), sizeof(float));
		memcpy (&zMin, pVertexData + 2*sizeof(float), sizeof(float));
		xMax = xMin;
		yMax = yMin;
		zMax = zMin;
		for (int v=0; v<GetNumVertices(); v++)
		{
			float x,y,z;
			memcpy (&x, pVertexData + (v*iVertexSize), sizeof(float));
			memcpy (&y, pVertexData + (v*iVertexSize + sizeof(float)), sizeof(float));
			memcpy (&z, pVertexData + (v*iVertexSize + 2*sizeof(float)), sizeof(float));
			
			xMin = (x<xMin) ? x : xMin;
			yMin = (y<yMin) ? y : yMin;
 			zMin = (z<zMin) ? z : zMin;
			xMax = (x>xMax) ? x : xMax;
			yMax = (y>yMax) ? y : yMax;
			zMax = (z>zMax) ? z : zMax;
		}
		pMesh->UnlockVertexBuffer();

		fWidth = xMax - xMin;
		fHeight = yMax - yMin;
		fDepth = zMax - zMin;

		return true;
	}

	void DXModel::Scale (float fScale)
	{
		// bookkeeping stuff
		fWidth *= fScale;
		fHeight *= fScale;
		fDepth *= fScale;
		if (fWidth < 0)
			fWidth *= -1;
		if (fHeight < 0)
			fHeight *= -1;
		if (fDepth < 0)
			fDepth *= -1;

		xMin *= fScale;
		xMax *= fScale;
		yMin *= fScale;
		yMax *= fScale;
		zMin *= fScale;
		zMax *= fScale;
		if (xMin > xMax)
		{
			float temp = xMin;
			xMin = xMax;
			xMax = temp;
		}
		if (yMin > yMax)
		{
			float temp = yMin;
			yMin = yMax;
			yMax = temp;
		}
		if (zMin > zMax)
		{
			float temp = zMin;
			zMin = zMax;
			zMax = temp;
		}

		// now for the punchline!
		char* pVerts;	//buffer pointing to the raw vertex data. Use it to extract vertex positions
		if (FAILED (GetVertexBuffer()->Lock (0,0, (void**)&pVerts, 0)))
			MessageBox (0, "Failed to lock vertex buffer", "DXModel::Scale", MB_OK);
		for (int v=0; v<GetNumVertices(); v++)
		{
			float* fVert = (float*) (pVerts + (GetVertexSize() * v));
			fVert[0] *= fScale;
			fVert[1] *= fScale;
			fVert[2] *= fScale;
		}
		GetVertexBuffer()->Unlock();
	}

	void DXModel::Scale (float fScalex, float fScaley, float fScalez)
	{
		// bookkeeping stuff
		fWidth *= fScalex;
		fHeight *= fScaley;
		fDepth *= fScalez;
		if (fWidth < 0)
			fWidth *= -1;
		if (fHeight < 0)
			fHeight *= -1;
		if (fDepth < 0)
			fDepth *= -1;

		xMin *= fScalex;
		xMax *= fScalex;
		yMin *= fScaley;
		yMax *= fScaley;
		zMin *= fScalez;
		zMax *= fScalez;
		if (xMin > xMax)
		{
			float temp = xMin;
			xMin = xMax;
			xMax = temp;
		}
		if (yMin > yMax)
		{
			float temp = yMin;
			yMin = yMax;
			yMax = temp;
		}
		if (zMin > zMax)
		{
			float temp = zMin;
			zMin = zMax;
			zMax = temp;
		}

		// now for the punchline
		char* pVerts;	//buffer pointing to the raw vertex data. Use it to extract vertex positions
		GetVertexBuffer()->Lock (0,0, (void**)&pVerts, 0);
		for (int v=0; v<GetNumVertices(); v++)
		{
			float* fVert = (float*) (pVerts + (GetVertexSize() * v));
			fVert[0] *= fScalex;
			fVert[1] *= fScaley;
			fVert[2] *= fScalez;
		}
		GetVertexBuffer()->Unlock();
	}

	void DXModel::Translate (float x, float y, float z)
	{
		// bookkeeping
		xMin += x;
		xMax += x;
		yMin += y;
		yMax += y;
		zMin += z;
		zMax += z;

		char* pVerts;	//buffer pointing to the raw vertex data. Use it to extract vertex positions
		GetVertexBuffer()->Lock (0,0, (void**)&pVerts, 0);
		for (int v=0; v<GetNumVertices(); v++)
		{
			float* fVert = (float*) (pVerts + (GetVertexSize() * v));
			fVert[0] += x;
			fVert[1] += y;
			fVert[2] += z;
		}
		GetVertexBuffer()->Unlock();
	}

	void DXModel::Draw(void)
	{
		Direct3D::SetVertexFormat (pMesh->GetFVF(), pMesh->GetNumBytesPerVertex());
		for (DWORD i = 0; i < dwMaterials; i++)
		{
			Direct3D::SetMaterial(&pMaterials[i]);
			TextureMgr::Set(pTextureIdx[i]);
			pMesh->DrawSubset(i);
		}
	}

	void DXModel::DrawSubset (int subset)
	{
		Direct3D::SetVertexFormat (pMesh->GetFVF(), pMesh->GetNumBytesPerVertex());
		TextureMgr::Set (pTextureIdx[subset]);
		pMesh->DrawSubset (subset);
	}