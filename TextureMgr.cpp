
//////////////////////////////////////////////////////
// File: "TextureMgr.cpp"
//
// Author: Matt Holden (MH)
//
// Creation Date: December 6, 2003
//
// Purpose:		Bitmap manager class
//////////////////////////////////////////////////////


#include "Direct3D.h"
#include "TextureMgr.h"

int TextureMgr::iWhiteTexture = -1;
int TextureMgr::nNextID = 0;
Texture TextureMgr::pTextures[100];
int TextureMgr::iCurrentTexture = -1;

	//////////////////////////////////////////////////////
	// Function: "LoadTexture"
	//
	// Last Modified: December 7, 2003
	//		
	// Input:			char * strFile						Filename of file to load
	//						DWORD dwColor					Color key to use (-1 = Don't use)
	// 
	// Output:		none
	//
	// Returns:		int nTextureID						the ID for our texture
	//
	// Purpose:		Load a texture into system/video memory.
	//
	//////////////////////////////////////////////////////
	int TextureMgr::LoadTexture(char * strFile, DWORD dwColor)
	{

		if (!strFile)
			return -1;

	// See if it's already loaded.
	for (int i = 0; i < nNextID; i++)
	{
		if (pTextures[i].nID != INVALID)
			if (strcmp (pTextures[i].strFileName, strFile) == 0)	
				return i;
	}

	// Set up structure...
	HRESULT hrOK;
	Texture &tTex = pTextures[nNextID];
	ZERO(tTex);

	// If they dont give me a color key, use the non color keyed version.
	if (dwColor == -1)
	{
		// try to load the texture
		hrOK = D3DXCreateTextureFromFile(Direct3D::GetDevice(), strFile, &tTex.pTexture);
		if (FAILED(hrOK)) return -1;

		// get the image info...
		D3DSURFACE_DESC tSurfInfo;
		ZERO(tSurfInfo);
		hrOK = tTex.pTexture->GetLevelDesc(0, &tSurfInfo);
		if (FAILED(hrOK)) return -1;

		// Fill out our structure
		tTex.fHeight = float(tSurfInfo.Height);
		tTex.fWidth = float(tSurfInfo.Width);
		strcpy (tTex.strFileName, strFile);		
	}	

	else
	{
		D3DXIMAGE_INFO tInfo;
		hrOK = D3DXCreateTextureFromFileEx(Direct3D::GetDevice(), strFile, NULL, NULL, NULL, NULL, D3DFMT_UNKNOWN, 
				D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, dwColor, &tInfo, NULL, &tTex.pTexture);
		
		if (FAILED(hrOK)) return -1;
		
		// Fill out our structure
		
		strcpy (tTex.strFileName,strFile);
		tTex.fHeight = float(tInfo.Height);
		tTex.fWidth = float(tInfo.Width);
	}

	// ms-help://MS.VSCC/MS.DirectX9.1033/DirectX9_c/directx/graphics/reference/d3d/enums/d3dtexturefiltertype.htm
	// Set filter modes... This should make textures look really nice.
	tTex.pTexture->SetAutoGenFilterType(D3DTEXF_ANISOTROPIC);

	// Give back the ID.
	tTex.nID = nNextID++;
	return tTex.nID;

}
