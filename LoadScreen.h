

#ifndef StormForge_LoadScreen
#define StormForge_LoadScreen

#include "D3DXFont.h"
#include "Direct3D.h"
#include <stdio.h>
#include "Texturemgr.h"
#include "timer.h"

#ifndef HALF_PI
#define HALF_PI 1.5708f
#endif

class LoadScreen
{
public:
	static int iTexture;
	static int iFont;
	static int iTitle;
	static int iBar;

	// Store how many bytes we're loading, and how many are loaded already
	static int iTotalLoadSize;
	static int iCurrentLoaded;

	// Load the font for use.. Call this any time we reset the font manager
	static void LoadFont(void) { iFont = DXFonts::Create("Arial", 20, true); }

	static void SetSize(int iSize) { iTotalLoadSize = iSize; }

	// Add the size of the file
	static void AddSize(char *strFile)
	{
		// See how big the file was
		FILE *pFile = fopen(strFile, "rb");
		if (!pFile) return;

		fseek(pFile, 0, SEEK_END);
		long iSize = ftell(pFile);
		fclose(pFile);

		// Add that size to the currently loaded size
		if (iTotalLoadSize == 1) iTotalLoadSize = 0;			/* Compensate for the 1 to evade DBZ */
		iTotalLoadSize += iSize;
	}
	


	// load textures
	static void LoadTextures(void) 
	{
		iBar = TextureMgr::LoadTexture("textures\\hudbar_charge.png");
		iTexture = TextureMgr::LoadTexture("textures\\sl_loadscreen.jpg");
		iTitle = TextureMgr::LoadTexture("textures\\GoNTitle_green.png");
	}

	// set it back to 0 size
	static void Reset(void) { iCurrentLoaded = 0; iTotalLoadSize = 1; }
 

	// Call this after every file successfully loads
	static void UpdateLoaded(char *strFile)
	{
		// See how big the file was
		FILE *pFile = fopen(strFile, "rb");
		if (!pFile) return;
		fseek(pFile, 0, SEEK_END);
		long iSize = ftell(pFile);
		fclose(pFile);

		// Add that size to the currently loaded size
		iCurrentLoaded += iSize;
	}

	// Draw the progress
	static void Draw(char *strFile, float fAlpha = 255.0f)
	{

		
		// Check if we need to initialize
		if (iTexture == INVALID) LoadTextures();
		if (iFont == INVALID) LoadFont();

		Direct3D::Clear();
		Direct3D::Begin();

		// Draw the screen across the whole screen no matter what
		float fScaleX = Direct3D::GetScreenWidth() / TextureMgr::GetBmpWidth(iTexture);
		float fScaleY = Direct3D::GetScreenHeight() / TextureMgr::GetBmpHeight(iTexture);

		float fScaleTX = float(Direct3D::GetScreenWidth()) / 1024.0f;
		float fScaleTY = float(Direct3D::GetScreenHeight()) / 800.0f;

		TextureMgr::Draw(iTexture, 0.0f, 0.0f, NULL, fScaleX, fScaleY, 0, int(fAlpha));
		TextureMgr::Draw(iTitle, 100.0f, 25.0f, NULL, fScaleTX, fScaleTY, 0, int(fAlpha));

 
		// Write out the currently loading file.
		DXFonts::Begin(iFont);
		{
			char strBuffer[500];
			if (strFile)
				sprintf(strBuffer, "Loading file: %s", strFile);
			else
				sprintf(strBuffer, "Initializing Guardians of Neverwood...");

			DXFonts::WriteShadow(strBuffer, 250, Direct3D::GetScreenHeight() - 75, 3, 1,1,1, (fAlpha/255.0f));
		}
		DXFonts::End();

		// Do progress bar.
			float fPct = float(iCurrentLoaded) / float(iTotalLoadSize);
			float fY = Direct3D::GetScreenHeight() * 0.8f;
			float fX = Direct3D::GetScreenWidth() * 0.8f;

			#define LoadBarScale 2.5f

			float fYPos = fY - (TextureMgr::GetBmpHeight(iBar) * LoadBarScale * fPct);

 			TextureMgr::Draw(iBar, fX, fY - (LoadBarScale * TextureMgr::GetBmpHeight(iBar)), NULL, 3.0f, LoadBarScale, 0, int(fAlpha * 0.25f));
			TextureMgr::Draw(iBar, fX, fYPos, NULL,  3.0f,  LoadBarScale * fPct, 0, int(fAlpha));

		Direct3D::End();
		Direct3D::Present();

	// Update size
		if (strFile)
			UpdateLoaded(strFile);
	}

	
	// Draw the progress
	static void DrawText(char *strText, int iAddSize = -1, float fAlpha = 255.0f)
	{

		if (iAddSize == -1) iAddSize = int(0.1f * iTotalLoadSize);

		// Check if we need to initialize
		if (iTexture == INVALID) LoadTextures();
		if (iFont == INVALID) LoadFont();

		Direct3D::Clear();
		Direct3D::Begin();

		// Draw the screen across the whole screen no matter what
		float fScaleX = Direct3D::GetScreenWidth() / TextureMgr::GetBmpWidth(iTexture);
		float fScaleY = Direct3D::GetScreenHeight() / TextureMgr::GetBmpHeight(iTexture);

		float fScaleTX = float(Direct3D::GetScreenWidth()) / 1024.0f;
		float fScaleTY = float(Direct3D::GetScreenHeight()) / 800.0f;

		TextureMgr::Draw(iTexture, 0.0f, 0.0f, NULL, fScaleX, fScaleY, 0, int(fAlpha));
		TextureMgr::Draw(iTitle, 100.0f, 25.0f, NULL, fScaleTX, fScaleTY, 0, int(fAlpha));

 
		// Write out the currently loading file.
		DXFonts::Begin(iFont);
		{
			DXFonts::WriteShadow(strText, 250, Direct3D::GetScreenHeight() - 75, 3, 1,1,1, (fAlpha/255.0f));
		}
		DXFonts::End();

		// Do progress bar.
			float fPct = float(iCurrentLoaded) / float(iTotalLoadSize);
			float fY = Direct3D::GetScreenHeight() * 0.8f;
			float fX = Direct3D::GetScreenWidth() * 0.8f;

			#define LoadBarScale 2.5f

			float fYPos = fY - (TextureMgr::GetBmpHeight(iBar) * LoadBarScale * fPct);

 			TextureMgr::Draw(iBar, fX, fY - (LoadBarScale * TextureMgr::GetBmpHeight(iBar)), NULL, 3.0f, LoadBarScale, 0, int(fAlpha * 0.25f));
			TextureMgr::Draw(iBar, fX, fYPos, NULL,  3.0f,  LoadBarScale * fPct, 0, int(fAlpha));

		Direct3D::End();
		Direct3D::Present();

		iCurrentLoaded += iAddSize;

	}

	static void FadeOut(void)
	{
		float fFadeStart = Timer::AppTime();
		float fCurrentTime = fFadeStart;

		// Fill bar
		LoadScreen::iCurrentLoaded = LoadScreen::iTotalLoadSize;
			
		while (fCurrentTime <= fFadeStart + 1.0f)
		{
			Draw(NULL, (255.0f * (fFadeStart + 1.0f - fCurrentTime)));
			fCurrentTime = Timer::AppTime();
		}

		// Destroy textures at the end.
		// Dont destroy bar or title since we use them elsewhere
		TextureMgr::UnloadTexture(iTexture);
		iTexture = INVALID;
	}


};

#endif