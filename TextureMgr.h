
//////////////////////////////////////////////////////
// File: "TextureMgr.h"
//
// Author: Matt Holden (MH)
//
// Creation Date: December 6, 2003
//
// Purpose:		Bitmap manager class
//////////////////////////////////////////////////////

#ifndef Holden_BmpManager
#define Holden_BmpManager

#include "Direct3D.h"

#define TEXTURE_MAXTEXTURES 100

struct Texture
{
	LPDIRECT3DTEXTURE9 pTexture;
	float fHeight, fWidth;
	int nID;
    char strFileName[120];
};

class TextureMgr 
{

private:
	static int nNextID;
	static Texture pTextures[100];
	static int iCurrentTexture;

public: 
	static int iWhiteTexture;

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
	//\
	// Purpose:		Load a texture into system/video memory.
	//
	//////////////////////////////////////////////////////
	static int LoadTexture(char * strFile, DWORD dwColor = -1);

	//////////////////////////////////////////////////////
	// Function: "Count"
	//
	// Last Modified: December 7, 2003
	//		
	// Input:			none
	// 
	// Output:		none
	//
	// Returns:		int		the number of items I have stored.
	//
	// Purpose:		See how many items are stored right now.
	//////////////////////////////////////////////////////
	static int GetCount(void) { return nNextID; }

	//////////////////////////////////////////////////////
	// Function: "GetBmpHeight"
	//
	// Last Modified: December 7, 2003
	//		
	// Input:			int nBitmap		Which bitmap we wanted
	// 
	// Output:		none
	//
	// Returns:		int						the height of the bitmap
	//
	// Purpose:		Get the height of a passed bitmap or -1 if not found
	//////////////////////////////////////////////////////
	static float GetBmpHeight(int nBitmap)
	{
		// Look up the texture
		if (nBitmap < 0 || nBitmap >= nNextID) return -1;
		return pTextures[nBitmap].fHeight;
	}

	//////////////////////////////////////////////////////
	// Function: "GetBmpFilename"
	//
	// Last Modified: December 7, 2003
	//		
	// Input:			int nBitmap		Which bitmap we wanted
	// 
	// Output:		none
	//
	// Returns:		char *						the name of the bmp file
	//
	// Purpose:		Get the name of a bitmap or "" if not found
	//////////////////////////////////////////////////////
	static char * GetBmpFilename(int nBitmap)
	{
		// Look up the texture
		if (nBitmap < 0 || nBitmap >= nNextID) return "";
		return pTextures[nBitmap].strFileName;
	}
	
	//////////////////////////////////////////////////////
	// Function: "GetBmpWidth"
	//
	// Last Modified: December 7, 2003
	//		
	// Input:			int nBitmap		Which bitmap we wanted
	// 
	// Output:		none
	//
	// Returns:		int						the width of the bitmap
	//
	// Purpose:		Get the width of a passed bitmap or -1 if not found
	//////////////////////////////////////////////////////
	static float GetBmpWidth(int nBitmap)
	{
		// Look up the texture
		if (nBitmap < 0 || nBitmap >= nNextID) return -1;
		return pTextures[nBitmap].fWidth;
	}

	//////////////////////////////////////
	// "Bind" the texture
	// ms-help://MS.VSCC/MS.DirectX9.1033/DirectX9_c/directx/graphics/reference/d3d/interfaces/idirect3ddevice9/settexture.htm
	static void Set(int iIndex)
	{
		if (iIndex != iCurrentTexture)
		{
			Direct3D::GetDevice()->SetTexture(0, pTextures[iIndex].pTexture);
			iCurrentTexture = iIndex;
		}
	}

	////////////////////////////////////////
	// Set the texture stuff up.. The equivalent of glTexEnv.
	//ms-help://MS.VSCC/MS.DirectX9.1033/DirectX9_c/directx/graphics/reference/d3d/enums/d3dtexturefiltertype.htm
	static void Initialize(void)
	{
		Direct3D::GetDevice()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		Direct3D::GetDevice()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

		// Currently nothing is selected
		iCurrentTexture = -1;

		// Clean slate.
		memset(pTextures, 0, sizeof(Texture) * TEXTURE_MAXTEXTURES);

		iWhiteTexture = TextureMgr::LoadTexture ("textures\\white4x4.bmp");
	}
	
	//////////////////////////////////////////////////////
	// Function: "Shutdown"
	// If you need to be told what a Shutdown function does...
	// well... McDonald's is always hiring.
	static void Shutdown(void)
	{
		Clear();
	}

	//////////////////////////////////////////////////////
	// Function: "Clear"
	//
	// Last Modified: December 7, 2003
	//		
	// Input:			none
	// 
	// Output:		none
	//
	// Returns:		none
	//
	// Purpose:		Unload all textures, don't shut down
	//
	//////////////////////////////////////////////////////
	static void Clear(void)
	{
		for (int i = 0; i < nNextID; i++)
		{
			UnloadTexture(i);
		}
		
		memset(pTextures, 0, sizeof(Texture) * TEXTURE_MAXTEXTURES);
		nNextID = 0;
	}

	static void UnloadTexture(int iTextureID)
	{
		if (iTextureID < 0 || iTextureID >= nNextID)
			return;
 		SAFE_RELEASE(pTextures[iTextureID].pTexture);
		memset(&pTextures[iTextureID], 0, sizeof(Texture));
		pTextures[iTextureID].nID = INVALID;
	}

	

/////////////////////////////////////////////////////
// Function: "Draw"
//
// Last Modified: December 7, 2003
//		
// Input:			int nTextureID		The texture to draw
//						int nXPos, nYPos	Screen coordinates of upper-left hand corner
//						RECT *rDrawArea	A portion of the texture to draw instead of the whole thing
//						float fScaleX, fScaleY	Scaling factors
//						float fRotateRads		Angle of rotation (radians)
//						int nAlpha				Alpha blending (0: full transparency; 255: full opacity)
// 
// Output:		none  
//
// Returns:		TRUE if there were no errors
//
// Purpose:		Draw a texture to the screen
//
//////////////////////////////////////////////////////
static BOOL Draw(int nTextureID,  float fXPos = 0, float fYPos = 0, RECT *rDrawArea = NULL, float fScaleX = 1.0f, float fScaleY = 1.0f, float fRotateRads = 0.0f, int nAlpha = 255)
	{
			
		// Look up the texture
		if (nTextureID < 0 || nTextureID >= nNextID) return FALSE;
		
		return SUCCEEDED(Direct3D::GetSprite()->Draw(
			pTextures[nTextureID].pTexture,
			rDrawArea,
			&D3DXVECTOR2(fScaleX, fScaleY), 
			&D3DXVECTOR2(pTextures[nTextureID].fWidth * fScaleX * 0.5f, pTextures[nTextureID].fWidth * fScaleY * 0.5f), 
			fRotateRads,
			&D3DXVECTOR2(fXPos, fYPos),
			ALPHA16(nAlpha)));

	}




};

#endif
