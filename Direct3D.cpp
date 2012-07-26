//////////////////////////////////////////////////////
// File: "Direct3D.cpp"
//
// Author: Matt Holden (MH)
//
// Creation Date: December 3, 2003
//
// Purpose:		Wrapper class for Direct3D
//////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////
// DIRECTX
//////////////////////////////////////////////////////////////////
// Direct3D stuff....	
#include <d3d9.h>
#include <windows.h>
#pragma comment(lib, "d3d9.lib")
// Direct3D extensions
#include <d3dx9core.h>					
#pragma comment(lib, "d3dx9.lib")
// DirectX GUIDs
#pragma comment(lib, "dxguid.lib")	
// DirectX Errors
#include <dxerr9.h>
#pragma comment(lib, "dxerr9.lib")
//////////////////////////////////////////////////////////////////
#include <stdio.h>

#include "Direct3D.h"
#include "LogFile.h"
#include "Renderer.h"

IDirect3D9 *Direct3D::pD3DObj = NULL; 
IDirect3DDevice9 *Direct3D::pDevice= NULL; 
D3DDISPLAYMODE Direct3D::d3dDisplay;
Direct3D::D3DPointSpriteInfo Direct3D::d3dPointSprites;
LPD3DXSPRITE Direct3D::pD3DSprite = NULL; 
IDirect3DSurface9 * Direct3D::pBackBufferSurface = NULL;
IDirect3DSurface9 *Direct3D::pScreenshotSurface = NULL;
IDirect3DTexture9 *Direct3D::pScreenshotTexture = NULL;
D3DPRESENT_PARAMETERS Direct3D::tD3DSettings;
Direct3D::D3DMaterial Direct3D::d3dMaterial;
Direct3D::D3DMatrixStack *Direct3D::pMatrixStack = NULL;

float Direct3D::fAspectRatio = 0.0f;
int Direct3D::iScreenshotID = 0;
int Direct3D::iWinSizeX = 0, Direct3D::iWinSizeY = 0;
int Direct3D::iVertexSize = 0;
DWORD Direct3D::dwFVF = 0;

//////////////////////////////////////////////////////
// Function: "Initialize"
//
// Last Modified: December 3, 2003
//		
// Input:			HWND hWnd			the window to use
//						BOOL bWindowed	TRUE if windowed mode
//						itn nBackBuffers		How many back buffers to use
//						bool Alpha16			true = use A1R5G5B5 false = use R5G6B5
// 
// Output:		none
//
// Returns:		TRUE if everything went OK.
//
// Purpose:		Get the instance of the singleton
//
//////////////////////////////////////////////////////
	BOOL Direct3D::Initialize(HWND hWnd, DWORD dwWidth, DWORD dwHeight, BOOL bWindowed, int nBackBuffers, bool Alpha16)
	{ 
		// Store parameters
		iWinSizeX = dwWidth;
		iWinSizeY = dwHeight;

		// Initialize object
		IDirect3D9 *pD3DObj = Direct3DCreate9(D3D_SDK_VERSION);
		if (!pD3DObj) 
			return FALSE;

		 //Get the display modes of the device
		 if(FAILED(pD3DObj->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3dDisplay))) return FALSE;

		 // Get the capabilities of the hardware.
		 D3DCAPS9 d3dCaps;
		 if(FAILED(pD3DObj->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &d3dCaps)))
			return FALSE;

		// Test which is supported, hardware or software vertex processing.
			DWORD dwVertProc = 0;
			if(d3dCaps.VertexProcessingCaps)
				dwVertProc |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
			else
				dwVertProc |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;

			//D3DCREATE_MIXED_VERTEXPROCESSING

			// See what kind of depth buffer we can use
			if (FAILED(pD3DObj->CheckDeviceFormat(D3DADAPTER_DEFAULT, d3dCaps.DeviceType, d3dDisplay.Format, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D16)))
				return FALSE;

		// Init structure...
		ZERO(tD3DSettings)		// Wipe it out (in case of re-init)
		tD3DSettings.BackBufferHeight = (bWindowed) ? dwHeight : d3dDisplay.Height;
		tD3DSettings.BackBufferWidth  = (bWindowed) ? dwWidth : d3dDisplay.Width;
		tD3DSettings.BackBufferFormat = d3dDisplay.Format;
		tD3DSettings.BackBufferCount = nBackBuffers;
		tD3DSettings.hDeviceWindow = hWnd;
		tD3DSettings.Windowed = (bWindowed) ? TRUE : FALSE;
		tD3DSettings.SwapEffect = D3DSWAPEFFECT_DISCARD;
		tD3DSettings.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
		tD3DSettings.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
		tD3DSettings.EnableAutoDepthStencil = TRUE;        
		tD3DSettings.AutoDepthStencilFormat = D3DFMT_D16;
	
		
		// Store the aspect ratio
		fAspectRatio = float(tD3DSettings.BackBufferWidth) / tD3DSettings.BackBufferHeight;

		// Use the structure to create the device pointer.
		if (FAILED(pD3DObj->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, dwVertProc, &tD3DSettings, &pDevice)))
			return FALSE;
		
		// Initialize back buffer pointer.. We need this to switch back after screenshots
		pDevice->GetRenderTarget(0, &pBackBufferSurface);

		// Create the texture and surface we are going to use to handle screenshots.
		pDevice->CreateTexture(tD3DSettings.BackBufferWidth, tD3DSettings.BackBufferHeight,1,D3DUSAGE_RENDERTARGET,D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT,&pScreenshotTexture,NULL);
		pScreenshotTexture->GetSurfaceLevel(0, &pScreenshotSurface);

		// Initialize sprite
		if (FAILED(D3DXCreateSprite(pDevice, &pD3DSprite)))
			return FALSE;

		// Initialize matrix stack
		if (FAILED(D3DXCreateMatrixStack(NULL, (LPD3DXMATRIXSTACK*)&pMatrixStack)))
			return FALSE;
		pMatrixStack->LoadIdentity();

		// Set up for point sprites
		pDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, TRUE);
		pDevice->SetRenderState(D3DRS_POINTSCALEENABLE, TRUE);
		memset(&d3dPointSprites, 0, sizeof(d3dPointSprites));


		// Set full material properties
		SetDefaultMaterial();

		return TRUE;
	}

	
//////////////////////////////////////////////////////
// Function: "Shutdown"
//
// Last Modified: December 3, 2003
//		
// Input:			none
// 
// Output:		none
//
// Returns:		none
//
// Purpose:	Shut down Direct3D.
//
//////////////////////////////////////////////////////

	void Direct3D::Shutdown(void)
	{
		SAFE_RELEASE(pMatrixStack);
		SAFE_RELEASE(pD3DSprite);
		SAFE_RELEASE(pScreenshotTexture);
		SAFE_RELEASE(pScreenshotSurface);
		SAFE_RELEASE(pDevice);

	}


/////////////////////////////////////////////////////
// Function: "FullScreen"
//
// Last Modified: December 3, 2003
//		
// Input:			none
// 
// Output:		none
//
// Returns:		TRUE if there were no errors
//
// Purpose:		Toggle between full screen and window mode
//
//////////////////////////////////////////////////////

	BOOL Direct3D::FullScreen(void)
	{
		if (tD3DSettings.Windowed)
		{
			// We were in Windowed mode.. Go to Full.
			//tD3DSettings.BackBufferFormat = tD3DSettings.BackBufferFormat;
			tD3DSettings.BackBufferFormat = D3DFMT_R5G6B5;

			tD3DSettings.Windowed = FALSE;

			D3DDISPLAYMODE DMODE;
			IDirect3D9 *pObj;
			pDevice->GetDirect3D(&pObj);
			pObj->EnumAdapterModes(D3DADAPTER_DEFAULT, D3DFMT_R5G6B5, 0, &DMODE);
	
			tD3DSettings.BackBufferHeight = DMODE.Height;
			tD3DSettings.BackBufferWidth = DMODE.Width;
			tD3DSettings.FullScreen_RefreshRateInHz = DMODE.RefreshRate;
			tD3DSettings.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;

			//SetWindowLong(tD3DSettings.hDeviceWindow, GWL_STYLE, WS_VISIBLE | WS_POPUP);
			//SetWindowPos(tD3DSettings.hDeviceWindow, NULL, NULL, d3dDisplay.Width, d3dDisplay.Height, NULL, SWP_NOMOVE | SWP_NOSIZE);
		}
		else
		{
			// Going to windowed..
			tD3DSettings.BackBufferFormat = D3DFMT_UNKNOWN;
			//tD3DSettings.BackBufferHeight = iWinSizeY;
			//tD3DSettings.BackBufferWidth = iWinSizeX;
			tD3DSettings.Windowed = TRUE;
			tD3DSettings.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
			tD3DSettings.FullScreen_RefreshRateInHz = 0;
			//SetWindowLong(tD3DSettings.hDeviceWindow, GWL_STYLE, WS_VISIBLE | WS_OVERLAPPEDWINDOW);
			//SetWindowPos(tD3DSettings.hDeviceWindow, HWND_NOTOPMOST, NULL, NULL, iWinSizeX, iWinSizeY, SWP_NOMOVE | SWP_NOSIZE);
		}
		
		// Reset things to take the new settings.
			//pD3DSprite->OnLostDevice();
			HRESULT hR = pDevice->Reset(&tD3DSettings);
			
			char buffer[50];
			strcpy(buffer, DXGetErrorString9(hR));

			if (FAILED(hR))
				return FALSE;

			if (FAILED(pD3DSprite->OnResetDevice()))
				return FALSE;
		
		return TRUE;
	}



	
	//////////////////////////////////////////////////////
// Function: "Rectangle"
//
// Last Modified: December 3, 2003
//		
// Input:			int nColorR, nColorG, nColorB		A color to fill the back with (R, G, B)
//						RECT rArea				the rect dimensions to draw
//						float fDistance			floating-point used for depth
//						int nStencilBuffer		Stencil buffer information...
//
// Output:		none
//
// Returns:		TRUE if everything went OK
//
// Purpose:		Draws a rectangle to the screen
//
//////////////////////////////////////////////////////
	BOOL Direct3D::Rectangle(int nColorR, int nColorG, int nColorB, RECT &rArea, float fDistance, int nStencilBuffer)
	{
		D3DRECT d3Rect = { rArea.left, rArea.top, rArea.right, rArea.bottom };

		DWORD dwFlag = D3DCLEAR_TARGET;
		if (nStencilBuffer) dwFlag |= D3DCLEAR_ZBUFFER; 
		pDevice->Clear(1, &d3Rect, dwFlag, D3DCOLOR_XRGB(nColorR, nColorG, nColorB), fDistance, nStencilBuffer);
		return TRUE;
	}




	
//////////////////////////////////////////////////////
// Function: "HandleAltTab"
//
// Last Modified: December 3, 2003
//		
// Input:			none
// 
// Output:		none
//
// Returns:		TRUE if we can draw...
//
// Purpose:		Run in game loop to recover from Alt-Tabbing back in to fullscreen mode
//
//////////////////////////////////////////////////////
	BOOL Direct3D::HandleAltTab(void)
	{		
		if (pDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
		{	
			// CHANGED
			// added a call to OnLostDevice
			// Rolf, 6-11-04
			if (FAILED (pD3DSprite->OnLostDevice())) return FALSE;
			if (FAILED(pDevice->Reset(&tD3DSettings))) return FALSE;
			return (SUCCEEDED(pD3DSprite->OnResetDevice()));
		}
		return TRUE;
	}


	////////////////////////////////////////////////////////
	// Finish taking a screenshot... Save it to a file, and put rendering back to the backbuffer.
	// ms-help://MS.VSCC/MS.MSDNVS/dx8_c/directx_cpp/Graphics/Reference/CPP/D3DX/Functions/Texture/D3DXSaveSurfaceToFile.htm
	void Direct3D::SaveScreenshot(char *strFile)
	{

		// Generate the filename of the screenshot...
			char strTemp[200];
			sprintf(strTemp, "%s%d.bmp", strFile, iScreenshotID);
			
		// increment for the next shot
			iScreenshotID++;
			
		// Save the surface to the file.
			D3DXSaveTextureToFile(strTemp, D3DXIFF_BMP, pScreenshotTexture, NULL);

		// Put the renderer's target back to the back buffer
			pDevice->SetRenderTarget(0, pBackBufferSurface);

	}



	///////////////////////////////////////////////////
	// Sets up fog in D3D.
	// 
	// bool bEnableFog				if false, there's no fog
	//											ms-help://MS.VSCC/MS.MSDNVS/dx8_c/directx_cpp/Graphics/Reference/CPP/D3D/Enums/d3drenderstatetype.htm
	//											(see D3DRS_FOGENABLE)
	// 
	// float fFogStart, fFogEnd	Where fog begins and ends
	//											ms-help://MS.VSCC/MS.MSDNVS/dx8_c/directx_cpp/Graphics/Reference/CPP/D3D/Enums/d3drenderstatetype.htm
	//											(see D3DRS_FOGSTART and D3DRS_FOGEND)
	// 
	// DWORD dwFogColor			Color to fade to. 
	//											ms-help://MS.VSCC/MS.MSDNVS/dx8_c/directx_cpp/Graphics/Reference/CPP/D3D/Enums/d3drenderstatetype.htm
	//											(see D3DRS_FOGCOLOR)
	//
	// DWORD dwFogMode			The fog table mode. Default for D3D is D3DFOG_NONE, 
	//											but if we won't use this parameter unless we enable
	//											fog so I left it this way.
	//											ms-help://MS.VSCC/MS.MSDNVS/dx8_c/directx_cpp/Graphics/Reference/CPP/D3D/Enums/d3dfogmode.htm
	//
	//	float fFogDensity				Density of the fog :) Should be 0.0f - 1.0f
	//											Only matters for EXP or EXP2 fog.
	//											ms-help://MS.VSCC/MS.MSDNVS/dx8_c/directx_cpp/Graphics/Reference/CPP/D3D/Enums/d3drenderstatetype.htm
	//											(see D3DRS_FOGDENSITY)
	//
	//
	// bool bRangeFog				If this is true, fogging is calculated based on distance, not
	//											depth. Used when we want to fog vertically too.
	//											ms-help://MS.VSCC/MS.MSDNVS/dx8_c/directx_cpp/Graphics/Reference/CPP/D3D/Enums/d3drenderstatetype.htm
	//											(see D3DRS_RANGEFOGENABLE)
	// 
	// bool bPixelFog					If this is true, fog is calculated per-pixel.
	//											ms-help://MS.VSCC/MS.MSDNVS/dx8_c/directx_cpp/Graphics/ProgrammersGuide/UsingDirect3D/Rendering/FogAlphaDepth/Fog/pixelfog.htm
	//
	// NOTE: The wierd typecast on the 3 float parameters is demanded by Direct3D
	// for some ungodly reason. See the fFogStart help file for more info.
	// 
	void Direct3D::Fog(bool bEnableFog, float fFogStart, float fFogEnd, DWORD dwFogColor, DWORD dwFogMode, float fFogDensity, bool bRangeFog, bool bPixelFog)
	{
		// Set fog true/false
		pDevice->SetRenderState(D3DRS_FOGENABLE, (bEnableFog) ? TRUE : FALSE);

		// Nothing else to change if we shut fog off
		if (!bEnableFog) return; 

		// Set fog parameters
		/* Start	*/ pDevice->SetRenderState(D3DRS_FOGSTART, *((DWORD*) (&fFogStart)));
		/* End		*/ pDevice->SetRenderState(D3DRS_FOGEND, *((DWORD*) (&fFogEnd)));
		/* Color	*/ pDevice->SetRenderState(D3DRS_FOGCOLOR, dwFogColor);
		/* Density*/ pDevice->SetRenderState(D3DRS_FOGDENSITY, *((DWORD*) (&fFogDensity)));
		/* Ranged*/ pDevice->SetRenderState(D3DRS_RANGEFOGENABLE, (bRangeFog) ? TRUE : FALSE);
		/* Mode	*/ 
		if (bPixelFog)
			pDevice->SetRenderState(D3DRS_FOGTABLEMODE, dwFogMode);
		else
			pDevice->SetRenderState(D3DRS_FOGVERTEXMODE, dwFogMode);
		
	}

		////////////////////////////////////////////////////
	// Toggle blending (ability to draw things with
	// transparency). Also sets the Source and Destination
	// blend modes, very much like glBlendFunc().
	// The enumerations for this are fairly simple and close
	// to the OpenGL types, and there are really no good
	// ways to wrap this without making another set
	// of enums for the user to learn. See the 2nd help
	// link below for a list of the blend modes in D3D and
	// what they do.
	// The dwOperation is a little different. Unlike OpenGL,
	// where you would say GL_ONE_MINUS_SRC_ALPHA
	// or so forth, the BlendOp specifies what to do with
	// the source and destination. See the 3rd help link for
	// more information about this.
	// 
	// Toggling blending: (click D3DRS_ALPHABLENDENABLE)
	//		ms-help://MS.VSCC/MS.MSDNVS/dx8_c/directx_cpp/Graphics/Reference/CPP/D3D/Enums/d3drenderstatetype.htm
	// Source/Destination Blend Modes:
	//		ms-help://MS.VSCC/MS.MSDNVS/dx8_c/directx_cpp/Graphics/Reference/CPP/D3D/Enums/d3dblend.htm
	// Blend Operation:
	//		ms-help://MS.VSCC/MS.MSDNVS/dx8_c/directx_cpp/Graphics/Reference/CPP/D3D/Enums/d3dblendop.htm
	void Direct3D::Blending(bool bBlending, DWORD dwSource, DWORD dwOperation, DWORD dwDest)
	{
		pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, (bBlending) ? TRUE : FALSE);
		if (!bBlending) return;
		pDevice->SetRenderState(D3DRS_SRCBLEND, dwSource);
		pDevice->SetRenderState(D3DRS_DESTBLEND, dwDest);
		pDevice->SetRenderState(D3DRS_BLENDOP, dwOperation);
	}

	
	/////////////////////////////////////////////////////////////////////////
	// Set perspective. We'll use this just like GL, too. This will set the projection
	// matrix for us.
	// D3DXMatrixPerspectiveFov: 
	//    ms-help://MS.VSCC/MS.MSDNVS/dx8_c/directx_cpp/Graphics/Reference/CPP/D3DX/Functions/Math/D3DXMatrixPerspectiveFovLH.htm
	// SetTransform:
	//    ms-help://MS.VSCC/MS.DirectX9.1033/DirectX9_c/directx/graphics/reference/d3d/interfaces/idirect3ddevice9/settransform.htm
	bool Direct3D::Perspective(float fViewAngle, float fNearClip, float fFarClip, float fAspect, bool bRightHanded)
	{
		if (FloatEqual(fAspect, 0.0f))
			fAspect = fAspectRatio;

		D3DXMATRIXA16 mProjection;

		if (bRightHanded)
			D3DXMatrixPerspectiveFovRH(&mProjection, fViewAngle, fAspect, fNearClip, fFarClip);
		else
			D3DXMatrixPerspectiveFovLH(&mProjection, fViewAngle, fAspect, fNearClip, fFarClip);

		return (FAILED(pDevice->SetTransform(D3DTS_PROJECTION, &mProjection))) ? false : true;
	}


	//////////////////////////////////
	// DEBUG						//
	//////////////////////////////////

	///////////////////////////////////////////////////
	// Check an HRESULT for an error, and fully deal with any that occur.
	// We'll return 'true' if there was an error.
	//
	// Parameters:
	// HRESULT hResult.................	The result we're checking for an error
	// char *strFunction................	The name of the function that generated hResult
	// char *strFile........................	The file the function is in. Pass __FILE__.
	// int iLine...............................	The line the function is on. Pass __LINE__.
	bool Direct3D::HandleDirectXError(HRESULT hResult, char *strFunction, char *strFile, int iLine)
	{
		// If it even is an error...
		if (FAILED(hResult))
		{
			/////////////////////////////
			// Log it to the error log
				char strError[300];

				// Generate the log string.
					sprintf(strError, "Function %s failed with HRESULT %s in file %s, line %d\n   %s\n", 
						strFunction,										// The name of the function being called

						DXGetErrorString9(hResult),			// The enumeration of the failed HRESULT
																				// For more info: ms-help://MS.VSCC/MS.DirectX9.1033/DirectX9_c/directx/intro/program/debugging/dxgeterrorstring9.htm

						strFile,	iLine,									// The file and line number where the error happened

						DXGetErrorDescription9(hResult));	// The text description of what actually went wrong.
																				// For more info: ms-help://MS.VSCC/MS.DirectX9.1033/DirectX9_c/directx/intro/program/debugging/dxgeterrordescription9.htm
					
				// Write the error to the log.
					LogFile::Log(strError);

			////////////////////////////
			// Throw a message box telling the user that a problem happened
			// For more info: ms-help://MS.VSCC/MS.MSDNVS/dx8_c/directx_cpp/Intro/ProgramCPP/Debugging/dxtrace.htm
				DXTrace(strFile, (DWORD)iLine, hResult, strFunction, TRUE);

			// Report that an error happened
			return true;
		}

		// No error!
		else return false;

	}	// End function HandleError


