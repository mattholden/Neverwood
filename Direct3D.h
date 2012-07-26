
//////////////////////////////////////////////////////
// File: "Direct3D.h"
//
// Author: Matt Holden (MH)
//
// Creation Date: December 3, 2003
//
// Purpose:		Wrapper class for Direct3D
//////////////////////////////////////////////////////

#ifndef StormForge_Matt_Direct3D
#define StormForge_Matt_Direct3D

#include "Identifiers.h"
#include "matrix.h"
#include "windows.h"
#include "utils.h"


// DEFAULT VIEW FRUSTUM SETTINGS
#define VIEWFRUSTUM_NEARCLIP 1
#define VIEWFRUSTUM_FARCLIP 175
#define VIEWFRUSTUM_ANGLE 45

#define D3DVERTEXFVF D3DFVF_XYZ | D3DFVF_DIFFUSE

struct D3DVertex
{
	float fPosition[3];
	DWORD dwDiffuseColor;
};




//////////////////////////////////////////////////////////////////
// DIRECTX
//////////////////////////////////////////////////////////////////
// Direct3D stuff....	
#include <d3d9.h>
#pragma comment(lib, "d3d9.lib")
// Direct3D extensions
#include <d3dx9core.h>					
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dxguid.lib")


class Direct3D
{
public:
	
		/////////////////////////////////////////////////////////////	
		// Our contained helper classes
			#include "Direct3DHelpers.h"

private:
		////////////////////////////////////////////////////////////
		// This is our actual device, and the workhorse of the whole system.
		//ms-help://MS.VSCC/MS.DirectX9.1033/DirectX9_c/directx/graphics/reference/d3d/interfaces/idirect3ddevice9/_idirect3ddevice9.htm
		static IDirect3DDevice9 *pDevice;
		
		static D3DDISPLAYMODE d3dDisplay;
		static IDirect3D9 *pD3DObj;
		static LPD3DXSPRITE pD3DSprite;
		static D3DPRESENT_PARAMETERS tD3DSettings;
		static IDirect3DSurface9 *pBackBufferSurface;

		// What we'll use for the "global" material properties
		static D3DMaterial d3dMaterial;

		// Store point sprite info for multiple emitters
		static D3DPointSpriteInfo d3dPointSprites;

		// Store parameters for changing stuff around
		static int iWinSizeX, iWinSizeY;

		// Matrix stack.
		//static ID3DXMatrixStack *pMatrixStack;
		static D3DMatrixStack *pMatrixStack;

		// aspect ratio.. just done to save a divide when we need it
		static float fAspectRatio;

		// For screenshots...
		static int iScreenshotID;
		static IDirect3DSurface9 *pScreenshotSurface;
		static IDirect3DTexture9 *pScreenshotTexture;

		// for changing vertex formats efficiently
		static int iVertexSize;		// Size of the currently selected vertex format.
		static DWORD dwFVF;			// Currently selected flexible vertex format.
				
public:


/*/////////////////////////////////////////////////////////

These are helper functions that wrap many of the
state changes of Direct3D into simple, Boolean function
calls. This is done primarily to help people who are
used to the glEnable modes and aren't familiar 
with the enumerations and functions D3D uses to 
achieve similar results. Hopefully, this will prove
more convenient to the user. 

To add as much convenience as possible to those
wishing to learn more about what I'm doing here, 
I will include a link to the MS DSK documentation
for each mode/function I am changing in the comments.

Also, each helper function's default parameter(s) 
represent the Direct3D defaults, unless otherwise noted.

Regarding Booleans: You will notice that the functions
take in the standard C++ 'bool', but use ?: to convert
this to Windows 'BOOL' in the D3D function calls. This is 
to enable users to maintain the 'true'/'false' calls they 
are used to and not generate performance warnings.
*/

	////////////////////////////////////////////////////
	// Get the Matrix Stack. From this pointer, all the stuff you knew and loved about
	// glPushMatrix(), etc. can be yours anew. See the help file for more details.
	// ms-help://MS.VSCC/MS.DirectX9.1033/DirectX9_c/directx/graphics/reference/d3dx/interfaces/id3dxmatrixstack/_id3dxmatrixstack.htm
	//static ID3DXMatrixStack *GetMatrixStack(void) { return pMatrixStack; }
	static D3DMatrixStack *GetMatrixStack(void) { return pMatrixStack; }
	
	////////////////////////////////////////////////////
	// Toggle Wireframe Mode
	// ms-help://MS.VSCC/MS.MSDNVS/dx8_c/directx_cpp/Graphics/Reference/CPP/D3D/Enums/d3dfillmode.htm
	static void Wireframe(bool bWireframe = false)
	{
		pDevice->SetRenderState(D3DRS_FILLMODE, (bWireframe) ? D3DFILL_WIREFRAME : D3DFILL_SOLID);
	}
	
	/////////////////////////////////////////////////////
	// Toggle culling, and switch the cull mode if needed
	// Unlike OpenGL, which specifies front and back
	// using clockwise/ccw winding and then culls based
	// on front/back, D3D simply takes in a clockwise/ccw
	// winding for which side to cull.
	// 
	
	static void Culling(bool bCullingEnabled = true, bool bClockwise = false)
	{
		if (bCullingEnabled) 
			pDevice->SetRenderState(D3DRS_CULLMODE, (bClockwise) ? D3DCULL_CW : D3DCULL_CCW);	
		else
			pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);		
	}
	
	////////////////////////////////////////////////////
	// Toggle depth writing (writing into the z buffer
	// to allow the video card to do occlusion culling)
	// 
	// (For help on this, use the link below and click 
	// D3DRS_ZWRITEENABLE)
	// ms-help://MS.VSCC/MS.MSDNVS/dx8_c/directx_cpp/Graphics/Reference/CPP/D3D/Enums/d3drenderstatetype.htm
	static void DepthWriting(bool bDepth = true)
	{
		pDevice->SetRenderState(D3DRS_ZWRITEENABLE, (bDepth) ? TRUE : FALSE);
	}	
	
	static void ColorVertex(bool bColorVert = true)
	{
		pDevice->SetRenderState(D3DRS_COLORVERTEX, (bColorVert) ? TRUE : FALSE);
	}

	////////////////////////////////////////////////////
	// Toggle depth testing (checking the z buffer
	// to see if a pixel would be hidden behind other stuff)
	// 
	//ms-help://MS.VSCC/MS.MSDNVS/dx8_c/directx_cpp/Graphics/Reference/CPP/D3D/Enums/d3dcmpfunc.htm
	static void DepthTesting(bool bDepth)
	{
		pDevice->SetRenderState(D3DRS_ZFUNC, (bDepth) ? D3DCMP_LESSEQUAL : D3DCMP_ALWAYS);
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
	static void Fog(bool bEnableFog = false, float fFogStart = 0.0f, float fFogEnd = 0.0f, DWORD dwFogColor = 0, DWORD dwFogMode = D3DFOG_LINEAR, float fFogDensity = 1.0f, bool bRangeFog = false, bool bPixelFog = false);

	//////////////////////////////////////////////////////////////////////
	// Set point size
	// ms-help://MS.VSCC/MS.MSDNVS/dx8_c/directx_cpp/Graphics/Reference/CPP/D3D/Enums/d3drenderstatetype.htm
	// (see D3DRS_POINTSIZE)
	static void PointSize(float fPointSize) 
	{ 
		pDevice->SetRenderState(D3DRS_POINTSIZE, *((DWORD*)(&fPointSize)));
	}
	
	//////////////////////////////////////////////////////////////////////
	// Enable/Disable Lighting
	// ms-help://MS.VSCC/MS.MSDNVS/dx8_c/directx_cpp/Graphics/Reference/CPP/D3D/Enums/d3drenderstatetype.htm
	// (see D3DRS_LIGHTING)
	static void Lighting(bool bLighting = true)
	{
		pDevice->SetRenderState(D3DRS_LIGHTING, (bLighting) ? TRUE : FALSE);
	}

	//////////////////////////////////////////////////////////////////////
	// Set ambient light color
	// ms-help://MS.VSCC/MS.MSDNVS/dx8_c/directx_cpp/Graphics/Reference/CPP/D3D/Enums/d3drenderstatetype.htm
	// (see D3DRS_AMBIENT)
	static void AmbientColor(DWORD dwColor)
	{
		pDevice->SetRenderState(D3DRS_AMBIENT, dwColor);
	}
	
	////////////////////////////////////////////////////
	// Toggle blending (ability to draw things with transparency). Also sets the Source and Destination
	// blend modes. The enumerations for this are fairly simple, and there are really no good ways to 
	// wrap this without making another set of enums for the user to learn. Fortunately, this rarely needs
	// to be changed. See the help link below for a list of the blend modes in D3D and what they do.
	//
	// The dwOperation is a little different. The BlendOp specifies what to do with the source and 
	// destination. See the 2nd help link for more information about this.
	// 
	// For more information:
	//		Source/Destination Blend Modes:
	//			ms-help://MS.VSCC/MS.MSDNVS/dx8_c/directx_cpp/Graphics/Reference/CPP/D3D/Enums/d3dblend.htm
	//		Blend Operation:
	//			ms-help://MS.VSCC/MS.MSDNVS/dx8_c/directx_cpp/Graphics/Reference/CPP/D3D/Enums/d3dblendop.htm
	//
	static void Blending(bool bBlending = false, DWORD dwSource = D3DBLEND_ONE, DWORD dwOperation = D3DBLENDOP_ADD, DWORD dwDest = D3DBLEND_ZERO);

	//////////////////////////////////////////////////////
	// Set Z Bias.
	// Valid values: 0 to 16
	//
	static void ZBias(int iZBias)
	{
		if (iZBias < 0) iZBias = 0;
		if (iZBias > 16) iZBias = 16;
		
#ifndef D3DRS_ZBIAS
#define D3DRS_ZBIAS D3DRENDERSTATETYPE(47)
#endif
		pDevice->SetRenderState(D3DRS_ZBIAS, (DWORD)iZBias);
	}

	// CHANGED
	// passing in 0,1 instead of fNearClip and fFarClip
	// Rolf, 6-2-04

	//////////////////////////////////////////////////////////////////////
	// Set viewport. We will use this just like the GL version,
	// calling it when we want to set up split screen.
	// Use the default values for left, top, width and height in full screen (1 player) mode.
	//ms-help://MS.VSCC/MS.DirectX9.1033/DirectX9_c/directx/graphics/reference/d3d/interfaces/idirect3ddevice9/setviewport.htm
	static BOOL Viewport(float fNearClip = 1.0f, float fFarClip = 100.0f, DWORD dwLeft = 0, DWORD dwTop = 0, DWORD dwWidth = 0, DWORD dwHeight = 0)
	{
		D3DVIEWPORT9 vpView = { dwLeft, dwTop, (dwWidth) ? dwWidth : GetScreenWidth(), (dwHeight) ? dwHeight : GetScreenHeight(),  0,1 };
		return (!FAILED(pDevice->SetViewport(&vpView)));
	}

	// CHANGED
	// changed default coordinate system from right-handed (OpenGL) to left-handed (DirectX)
	// Rolf, 6-2-04

	/////////////////////////////////////////////////////////////////////////
	// Set perspective. We'll use this just like GL, too. This will set the projection 
	// matrix for us.
	// D3DXMatrixPerspectiveFov: 
	//    ms-help://MS.VSCC/MS.MSDNVS/dx8_c/directx_cpp/Graphics/Reference/CPP/D3DX/Functions/Math/D3DXMatrixPerspectiveFovLH.htm
	// SetTransform:
	//    ms-help://MS.VSCC/MS.DirectX9.1033/DirectX9_c/directx/graphics/reference/d3d/interfaces/idirect3ddevice9/settransform.htm
	static bool Perspective(float fViewAngle, float fNearClip, float fFarClip, float fAspect = 0, bool bRightHanded = false);



	
	//////////////////////////////////////
	// Set view ("modeling") matrix.
	// ms-help://MS.VSCC/MS.DirectX9.1033/DirectX9_c/directx/graphics/reference/d3d/interfaces/idirect3ddevice9/settransform.htm
	static void SetViewMatrix(Matrix &mMatrix)
	{ 	pDevice->SetTransform(D3DTS_VIEW, (D3DMATRIX*)mMatrix.RowMajor()); }
	
	//////////////////////////////////////
	// Set view ("modeling") matrix.
	// ms-help://MS.VSCC/MS.DirectX9.1033/DirectX9_c/directx/graphics/reference/d3d/interfaces/idirect3ddevice9/settransform.htm
	static void SetViewMatrix(D3DMATRIX &mMatrix)
	{ 	pDevice->SetTransform(D3DTS_VIEW, &mMatrix); }

	//////////////////////////////////////
	// Set projection matrix.
	// ms-help://MS.VSCC/MS.DirectX9.1033/DirectX9_c/directx/graphics/reference/d3d/interfaces/idirect3ddevice9/settransform.htm
	static void SetProjectionMatrix(Matrix &mMatrix)
	{	pDevice->SetTransform(D3DTS_PROJECTION, (D3DMATRIX*)mMatrix.RowMajor()); }
	
	//////////////////////////////////////
	// Set projection matrix.
	// ms-help://MS.VSCC/MS.DirectX9.1033/DirectX9_c/directx/graphics/reference/d3d/interfaces/idirect3ddevice9/settransform.htm
	static void SetProjectionMatrix(D3DMATRIX &mMatrix)
	{	pDevice->SetTransform(D3DTS_PROJECTION, &mMatrix); }
	
	//////////////////////////////////////
	// Set World matrix.
	// ms-help://MS.VSCC/MS.DirectX9.1033/DirectX9_c/directx/graphics/reference/d3d/interfaces/idirect3ddevice9/settransform.htm
	static void SetWorldMatrix(Matrix &mMatrix)
	{	pDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX*)mMatrix.RowMajor()); }
	
	//////////////////////////////////////
	// Set World matrix.
	// ms-help://MS.VSCC/MS.DirectX9.1033/DirectX9_c/directx/graphics/reference/d3d/interfaces/idirect3ddevice9/settransform.htm
	static void SetWorldMatrix(D3DMATRIX &mMatrix)
	{	pDevice->SetTransform(D3DTS_WORLD, &mMatrix); }
	
	//////////////////////////////////////
	// Get World matrix.
	// ms-help://MS.VSCC/MS.DirectX9.1033/DirectX9_c/directx/graphics/reference/d3d/interfaces/idirect3ddevice9/gettransform.htm
	static Matrix GetWorldMatrix(void) 
	{ 
		Matrix mReturn; 
		pDevice->GetTransform(D3DTS_WORLD, (D3DMATRIX*)&mReturn);
		return mReturn.Transpose();
	}

	static void SetDefaultMaterial(void)
	{
		D3DMATERIAL9 material;
		D3DCOLORVALUE gray = {0.5f, 0.5f, 0.5f, 1};
		D3DCOLORVALUE black = {0,0,0,1};
		material.Ambient = gray;
		material.Diffuse = gray;
		material.Emissive = black;
		material.Specular = gray;
		material.Power = 1;
		Direct3D::SetMaterial (&material);
	}

	//////////////////////////////////////
	// Get Projection matrix.
	// ms-help://MS.VSCC/MS.DirectX9.1033/DirectX9_c/directx/graphics/reference/d3d/interfaces/idirect3ddevice9/gettransform.htm
	static Matrix GetProjectionMatrix(void) 
	{ 
		Matrix mReturn; 
		pDevice->GetTransform(D3DTS_PROJECTION, (D3DMATRIX*)&mReturn);
		return mReturn.Transpose();
	}

	//////////////////////////////////////
	// Get View matrix.
	// ms-help://MS.VSCC/MS.DirectX9.1033/DirectX9_c/directx/graphics/reference/d3d/interfaces/idirect3ddevice9/gettransform.htm
	static Matrix GetViewMatrix(void) 
	{ 
		Matrix mReturn; 
		pDevice->GetTransform(D3DTS_VIEW, (D3DMATRIX*)&mReturn);
		return mReturn.Transpose();
	}




	///////////////////////////////////////
	// Set the "global" color.
	static void SetColor(float fRed, float fGreen, float fBlue, float fAlpha = 1.0f) 
	{ d3dMaterial.SetColor(fRed, fGreen, fBlue, fAlpha); SetMaterial(); }

	///////////////////////////////////////
	// Set the "specular" color.
	static void SetSpecular(float fRed, float fGreen, float fBlue, float fAlpha = 1.0f, float fPower = 8.0f) 
	{ d3dMaterial.SetSpecularColor(fRed, fGreen, fBlue, fAlpha, fPower);  SetMaterial(); }

	///////////////////////////////////////
	// Set the "emissive" color.
	static void SetEmission(float fRed, float fGreen, float fBlue, float fAlpha = 1.0f) 
	{ d3dMaterial.SetEmission(fRed, fGreen, fBlue, fAlpha);  SetMaterial(); }

	///////////////////////////////////////
	// Set the current material
	// ms-help://MS.VSCC/MS.DirectX9.1033/DirectX9_c/directx/graphics/reference/d3d/interfaces/idirect3ddevice9/setmaterial.htm
	static void SetMaterial(D3DMATERIAL9 *pMaterial = NULL) 
	{ 
		if (!pMaterial) d3dMaterial.SetActive();
		else pDevice->SetMaterial(pMaterial); 
	}
	

















    ////////////////////////////////////
	// Show the mouse cursor?
	//ms-help://MS.VSCC/MS.DirectX9.1033/DirectX9_c/directx/graphics/reference/d3d/interfaces/idirect3ddevice9/showcursor.htm
	static void ShowMouseCursor(bool bCursor = true) 
	{
		pDevice->ShowCursor(bCursor ? TRUE : FALSE); 
		ShowCursor(bCursor ? TRUE : FALSE);
	}

	//////////////////////////////////////////////////////////////////////
	// If mouse cursor is on, put it where DirectInput thinks it is.
	// Pass in the POINT structure that DirectInput's mouse GetCursorPosition returns
	//ms-help://MS.VSCC/MS.DirectX9.1033/DirectX9_c/directx/graphics/reference/d3d/interfaces/idirect3ddevice9/setcursorposition.htm
	static void SetMousePosition(POINT ptPosition)
	{
		pDevice->SetCursorPosition(ptPosition.x, ptPosition.y, D3DCURSOR_IMMEDIATE_UPDATE);
	}

	////////////////////////////////////
	// If this is true, we can use GDI message boxes and dialogs in Direct3D.
	// Gonna be mighty handy for the level editor.
	// ms-help://MS.VSCC/MS.DirectX9.1033/DirectX9_c/directx/graphics/reference/d3d/interfaces/idirect3ddevice9/setdialogboxmode.htm
	static void AllowDialogBoxes(BOOL bDialog = false) { pDevice->SetDialogBoxMode(bDialog); }






















	///////////////////////////////////////////////////////
	// Start taking a screenshot...
	static void StartScreenshot(void)
	{	pDevice->SetRenderTarget(0, pScreenshotSurface); }
	
	///////////////////////////////////////
	// Switch the render target to the back buffer from the screenshot 'surface'
	static void EndScreenshot(void)
	{
		// Set the rendering target to the back buffer surface instead of the screenshot
		// For more information:	ms-help://MS.VSCC/MS.DirectX9.1033/DirectX9_c/directx/graphics/reference/d3d/interfaces/idirect3ddevice9/setrendertarget.htm
		pDevice->SetRenderTarget(0, pBackBufferSurface);
	}

	////////////////////////////////////////////////////////
	// Finish taking a screenshot... Save it to a file, and put rendering back to the backbuffer.
	// ms-help://MS.VSCC/MS.MSDNVS/dx8_c/directx_cpp/Graphics/Reference/CPP/D3DX/Functions/Texture/D3DXSaveSurfaceToFile.htm
	static void SaveScreenshot(char *strFile);
	
	///////////////////////////////////////
	// Set the screenshot texture as the current texture (for Render-to-Texture)
	// For more information:	ms-help://MS.VSCC/MS.DirectX9.1033/DirectX9_c/directx/graphics/reference/d3d/interfaces/idirect3ddevice9/settexture.htm
	static void SetTextureToScreenshot(void)
	{
		pDevice->SetTexture(0, pScreenshotTexture);
	}












	/////////////////////////////////////////////////////////
	// Get methods for private stuff
	static HWND GetHwnd(void) 	{ 		return tD3DSettings.hDeviceWindow; 	}
	static BOOL IsWindowed(void) { return tD3DSettings.Windowed; }
	static IDirect3DDevice9 *GetDevice(void) { return pDevice; }
	static D3DPRESENT_PARAMETERS &GetSettings(void) { return tD3DSettings; }
	static IDirect3D9 *GetObject(void) { IDirect3D9 *pObj; pDevice->GetDirect3D(&pObj); return pObj; }
	static LPD3DXSPRITE GetSprite(void) { return pD3DSprite; }
	static RECT GetScreenRect(void) { RECT R = { 0, 0, tD3DSettings.BackBufferWidth, tD3DSettings.BackBufferHeight }; return R; }
	static DWORD GetScreenWidth(void) { return tD3DSettings.BackBufferWidth; }
	static DWORD GetScreenHeight(void) { return tD3DSettings.BackBufferHeight; }
	static float GetAspectRatio(void) { return fAspectRatio; }
	
	// Get a string containing the name of the video card
	static char *GetVideoCardName(unsigned int uiAdapter = D3DADAPTER_DEFAULT)
	{
		 D3DADAPTER_IDENTIFIER9 d3dAID;
			Direct3D::GetObject()->GetAdapterIdentifier(uiAdapter, 0, &d3dAID);
		 return d3dAID.Description;
	}

	////////////////////////////////////////////////////////
	// See how much texture memory is left on the system (to the nearest megabyte)
	static unsigned int GetFreeTextureMemory(void) { return pDevice->GetAvailableTextureMem(); }
	
	////////////////////////////////////////////////////////
	// Initialize and shutdown

	//////////////////////////////////////////////////////
	// Function: "Initialize"
	// Input:			HWND hWnd			the window to use
	//						BOOL bWindowed	TRUE if windowed mode
	//						int nBackBuffers		How many back buffers to use
	//						bool Alpha16			true = use A1R5G5B5 false = use R5G6B5
	static BOOL Direct3D::Initialize(HWND hWNd, DWORD dwWidth, DWORD dwHeight, BOOL bWindowed = FALSE, int nBackBuffers = 1, bool Alpha16 = true);
	static void Shutdown(void);

	/////////////////////////////////////////////////////
	// Toggle full screen mode
	static BOOL FullScreen(void);
	
	/////////////////////////////////////////////////////
	// Restores the device when we're multitasking
	static BOOL HandleAltTab(void);

	////////////////////////////////////////////////////
	// Clear the back buffer
	//	Input:			int nColorR, nColorG, nColorB		A color to fill the back with (R, G, B)
	//						float fDistance			floating-point used for depth
	//						int nStencilBuffer		Stencil buffer information...
	static void Direct3D::Clear(int nColorR = 0, int nColorG = 0, int nColorB = 0, float fDistance = 1.0f, int nStencilBuffer = 0)
	{
		DWORD dwFlag = D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER;
		dwFlag |= (nStencilBuffer) ? D3DCLEAR_STENCIL : 0;
		pDevice->Clear(NULL, NULL, dwFlag, D3DCOLOR_XRGB(nColorR, nColorG, nColorB), fDistance, nStencilBuffer);

	}

	////////////////////////////////////////////////////
	// Present the back buffer to the screen
	// Input:			RECT *rSource		Source rect...
	// Input:			RECT *rDest			Destination rect...
	static void Present(RECT *rSource = NULL, RECT *rDest = NULL)
	{
		pDevice->Present(rSource, rDest, NULL, NULL);
	}

	//////////////////////////////////////////////
	// Draw a colored rect.		
	// Input:			int nColorR, nColorG, nColorB		A color to fill the back with (R, G, B)
	//						RECT rArea				the rect dimensions to draw
	//						float fDistance			floating-point used for depth
	//						int nStencilBuffer		Stencil buffer information...
	static BOOL Direct3D::Rectangle(int nColorR, int nColorG, int nColorB, RECT &rArea, float fDistance = 1.0f, int nStencilBuffer = 0);

	// start the scene...
	static void Direct3D::Begin(void)
	{
		pDevice->BeginScene();
	}

	// end the scene...
	static void Direct3D::End(void)
	{
		pDevice->EndScene();
 	}

#define FloatToDWORD(FLOAT) *(DWORD*)(&(FLOAT))

#define CheckPSI(Value, Parameter, Function) if (psi.Value != Parameter) { psi.Value = Parameter; Function; }

	// Set up point sprites
	static void SetupPointSprites(float fPointSize = 2.0f, float fMinSize = 1.0f, float fMaxSize = 5.0f, float fScaleA = 1.0f, float fScaleB = 1.0f, float fScaleC = 0.0f)
	{
		// shorthand
		D3DPointSpriteInfo &psi = d3dPointSprites;
		
		CheckPSI(fSize, fPointSize, pDevice->SetRenderState(D3DRS_POINTSIZE, FloatToDWORD(fPointSize)));
		CheckPSI(fMinSize, fMinSize, pDevice->SetRenderState(D3DRS_POINTSIZE_MIN, FloatToDWORD(fMinSize)));
		CheckPSI(fMaxSize, fMaxSize, pDevice->SetRenderState(D3DRS_POINTSIZE_MAX, FloatToDWORD(fMaxSize)));

		CheckPSI(fScaleA, fScaleA, pDevice->SetRenderState(D3DRS_POINTSCALE_A, FloatToDWORD(fScaleA)));
		CheckPSI(fScaleB, fScaleB, pDevice->SetRenderState(D3DRS_POINTSCALE_B, FloatToDWORD(fScaleB)));
		CheckPSI(fScaleC, fScaleC, pDevice->SetRenderState(D3DRS_POINTSCALE_C, FloatToDWORD(fScaleC)));

	}
	

					///////////////////////////////////////////////////////
					// DEBUGGING TOOLS
					///////////////////////////////////////////////////////
					

					/////////////////////////////////////////////////////////
					// Check an HRESULT for an error, and fully deal with any that occur.
					// We'll return 'true' if there was an error.
					//
					// Parameters:
					// HRESULT hResult.................	The result we're checking for an error
					// char *strFunction................	The name of the function that generated hResult
					// char *strFile........................	The file the function is in. Pass __FILE__.
					// int iLine...............................	The line the function is on. Pass __LINE__.
					//
					static bool HandleDirectXError(HRESULT hResult, char *strFunction, char *strFile, int iLine);
		
					// PRIMITIVES
					// The following functions have been added for debugging purposes

					//////////////////////////////////////////////////
	                //  
                    // Sets the vertex format to use for primitive drawing.
                    // Rather than call this function directly, you should use the SetD3DVertexType() macro.                  
                    // Parameters:
                    // DWORD dwVertexFVF            The flexible vertex format (FVF) of the vertex type
                    // int iVertexFormatSize            The size of the vertex structure we will be using.
                    //
                    // Returns 'false' if there were no errors.
                    //
                    static bool SetVertexFormat(DWORD dwVertexFVF, int iVertexFormatSize)
                    {                       
                        // Return if this is a redundant set
                        if (dwVertexFVF == dwFVF) return false;

                        // Store the new FVF and size in our wrapper
                        dwFVF = dwVertexFVF; 
                        iVertexSize = iVertexFormatSize;

                        // Set the vertex format
                        // For more information:    ms-help://MS.VSCC/MS.DirectX9.1033/DirectX9_c/directx/graphics/reference/d3d/interfaces/idirect3ddevice9/setfvf.htm
                        return ERRORCHECKDEBUG(pDevice->SetFVF(dwVertexFVF), "SetVertexFormat SetFVF");
                    }

                    //////////////////////////////////////////////////
                    //  
                    // Draws the vertices stored in pVertices as a list of triangles.
                    //
                    // Parameters:
                    // void *pVertices                  The vertex data, stored in one of our vertex structures.
                    //                                          Before calling this function, you should call the macro SetD3DVertexType.
                    //
                    // int iNumToDraw               The number of vertices to draw.
                    //                                          The number of triangles is calculated as the number of vertices / 3.
                    //
                    // Returns 'false' if there were no errors.
                    //
                    static bool DrawTriangles(void *pVertices, int iNumToDraw)
                    {
                        // Calculate number of shapes to draw
                        // (0.35 because we want to be higher, since the int() typecast truncates)
                        int iShapes = int(float(iNumToDraw) * 0.35f);

                        // Use DrawPrimitiveUP to draw the shapes.
                        // For more information:    ms-help://MS.VSCC/MS.DirectX9.1033/DirectX9_c/directx/graphics/reference/d3d/interfaces/idirect3ddevice9/drawprimitiveup.htm
                        return ERRORCHECKDEBUG(pDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, iShapes, pVertices, iVertexSize), "DrawTriangles");
                    }

                    //////////////////////////////////////////////////
                    //  
                    // Draws the vertices stored in pVertices as a connected triangle strip
                    //
                    // Parameters:
                    // void *pVertices                  The vertex data, stored in one of our vertex structures.
                    //                                          Before calling this function, you should call the macro SetD3DVertexType.
                    //
                    // int iNumToDraw               The number of vertices to draw.
                    //                                          The number of triangles is calculated as the number of vertices - 2.
                    //
                    // Returns 'false' if there were no errors.
                    //
                    static bool DrawTriangleStrip(void *pVertices, int iNumToDraw)
                    {
                        // Use DrawPrimitiveUP to draw the shapes.
                        // For more information:    ms-help://MS.VSCC/MS.DirectX9.1033/DirectX9_c/directx/graphics/reference/d3d/interfaces/idirect3ddevice9/drawprimitiveup.htm
                        return ERRORCHECKDEBUG(pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, iNumToDraw - 2, pVertices, iVertexSize), "DrawTriangleStrip");
                    }

                    
                    //////////////////////////////////////////////////
                    //  
                    // Draws the vertices stored in pVertices as a fan of triangles.
                    //
                    // Parameters:
                    // void *pVertices                  The vertex data, stored in one of our vertex structures.
                    //                                          Before calling this function, you should call the macro SetD3DVertexType.
                    //
                    // int iNumToDraw               The number of vertices to draw.
                    //                                          The number of triangles is calculated as the number of vertices - 2.
                    //
                    // Returns 'false' if there were no errors.
                    //
                    static bool DrawTriangleFan(void *pVertices, int iNumToDraw)
                    {
                        // Use DrawPrimitiveUP to draw the shapes.
                        // For more information:    ms-help://MS.VSCC/MS.DirectX9.1033/DirectX9_c/directx/graphics/reference/d3d/interfaces/idirect3ddevice9/drawprimitiveup.htm
                        return ERRORCHECKDEBUG(pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, iNumToDraw - 2, pVertices, iVertexSize), "DrawTriangleFan");
                    }

                    //////////////////////////////////////////////////
                    //  
                    // Draws the vertices stored in pVertices as a list of points.
                    //
                    // Parameters:
                    // void *pVertices                  The vertex data, stored in one of our vertex structures.
                    //                                          Before calling this function, you should call the macro SetD3DVertexType.
                    //
                    // int iNumToDraw               The number of vertices to draw.
                    //                                          The number of points is calculated as 1 point 1 = 1 vertex
                    //
                    // Returns 'false' if there were no errors.
                    //
                    static bool DrawPoints(void *pVertices, int iNumToDraw)
                    {
                        // Use DrawPrimitiveUP to draw the shapes.
                        // For more information:    ms-help://MS.VSCC/MS.DirectX9.1033/DirectX9_c/directx/graphics/reference/d3d/interfaces/idirect3ddevice9/drawprimitiveup.htm
                        return ERRORCHECKDEBUG(pDevice->DrawPrimitiveUP(D3DPT_POINTLIST, iNumToDraw, pVertices, iVertexSize), "DrawPoints");
                    }

                    //////////////////////////////////////////////////
                    //  
                    // Draws the vertices stored in pVertices as a list of lines.
                    //
                    // Parameters:
                    // void *pVertices                  The vertex data, stored in one of our vertex structures.
                    //                                          Before calling this function, you should call the macro SetD3DVertexType.
                    //
                    // int iNumToDraw               The number of vertices to draw.
                    //                                          The number of lines is calculated as number of vertices / 2.
                    //
                    // Returns 'false' if there were no errors.
                    //
                    static bool DrawLines(void *pVertices, int iNumToDraw)
                    {
                        // Calculate number of shapes to draw
                        int iShapes = int(float(iNumToDraw) * 0.5000f);
						if (iShapes <= 0)
							iShapes = 1;

                        // Use DrawPrimitiveUP to draw the shapes.
                        // For more information:    ms-help://MS.VSCC/MS.DirectX9.1033/DirectX9_c/directx/graphics/reference/d3d/interfaces/idirect3ddevice9/drawprimitiveup.htm
                        return ERRORCHECKDEBUG(pDevice->DrawPrimitiveUP(D3DPT_LINELIST, iShapes, pVertices, iVertexSize), "DrawLines");
                    }

                    //////////////////////////////////////////////////
                    //  
                    // Draws the vertices stored in pVertices as a list of lines.
                    //
                    // Parameters:
                    // void *pVertices                  The vertex data, stored in one of our vertex structures.
                    //                                          Before calling this function, you should call the macro SetD3DVertexType.
                    //
                    // int iNumToDraw               The number of vertices to draw.
                    //                                          The number of lines is calculated as number of vertices - 1
                    //
                    // Returns 'false' if there were no errors.
                    //
                    static bool DrawLineStrip(void *pVertices, int iNumToDraw)
                    {
                        // Use DrawPrimitiveUP to draw the shapes.
                        // For more information:    ms-help://MS.VSCC/MS.DirectX9.1033/DirectX9_c/directx/graphics/reference/d3d/interfaces/idirect3ddevice9/drawprimitiveup.htm
                        return ERRORCHECKDEBUG(pDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, iNumToDraw - 1, pVertices, iVertexSize), "DrawLineStrip");
                    }


                    //////////////////////////////////////////////////
                    //  
                    // Draws the vertices stored in pVertices as a loop of lines.
                    //
                    // Parameters:
                    // void *pVertices                  The vertex data, stored in one of our vertex structures.
                    //                                          Before calling this function, you should call the macro SetD3DVertexType.
                    //
                    // int iNumToDraw               The number of vertices to draw.
                    //                                          The number of lines is calculated as number of vertices (due to extra one)
                    //
                    // Returns 'false' if there were no errors.
                    //
                    static bool DrawLineLoop(void *pVertices, int iNumToDraw);


};

#endif
