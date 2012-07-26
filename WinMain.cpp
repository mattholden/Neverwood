//	WinMain.cpp : Basic Windows Shell.
///////////////////////////////////////////////////////////////
// INTEGRATED CORE TECHNOLOGY COMPONENTS
///////////////////////////////////////////////////////////////
#include <ctime>

#include "MathUtils.h"

#include "DirectInput2.h"
#include "SoundManager.h"
#include "Timer.h"
	
#include "Direct3D.H"
	#include "Renderer.h"
	#include "TextureMgr.h"	
	#include "Light.h"
	#include "D3DXFont.h"

#include "MemoryMgr.h"

#include "Game.h"
#include "Menu.h"
#include "Character.h"

// Trim out some junk we don't need.
#define WIN32_LEAN_AND_MEAN 1
#define VC_EXTRALEAN 1


#define WINDOW_WIDTH	800								//	Window Width.
#define WINDOW_HEIGHT  600								//	Window Height
#define WINDOW_FULLSCREEN 1
#define CLASSNAME "Guardians of Neverwood"		//	Window Class Name.

// Make the actual window
HWND WindowCreate(HINSTANCE hinstance);

////////////////////////////////////////////////////////////////
// Called when we start up.
// Place any initialization of core tech modules here
////////////////////////////////////////////////////////////////
void OnStartup(HWND hWnd, HINSTANCE hInst)
{
			// Somebody set up us the rand()
			srand(unsigned int(time(0)));

			// Set up  Memory Monitor so we can use it for other stuff.
			// Must be done before any other managers are initialized or they will blow up in debug mode trying 
			// to register themselves with a manager that doesn't yet exist
			#if _DEBUG
				MemoryManager::Initialize();
			#endif 

			// Start the app timer.
				Timer::Initialize();

			// Set up math utils
				MathTrigInitialize();
		
			// Set up input.
				DirectInput::Initialize(hWnd, hInst);
			
			// Sound.
				SoundManager::Initialize();

			// Renderersssssssssssssssssssss
				Direct3D::Initialize(hWnd, WINDOW_WIDTH, WINDOW_HEIGHT, !WINDOW_FULLSCREEN);
				Direct3D::ShowMouseCursor(false);
				Direct3D::Viewport();
				Direct3D::Culling(true, false);
				Direct3D::DepthTesting(true);
				Direct3D::DepthWriting(true);
				Direct3D::Perspective (VIEWFRUSTUM_ANGLE, VIEWFRUSTUM_NEARCLIP, VIEWFRUSTUM_FARCLIP);
				
				LightManager::SetSceneAmbient(0.5f, 0.5f, 0.5f);
				LightManager::Enable(true);
				LightManager::GetLight(0)->SetAmbient(0.4f, 0.4f, 0.4f);
				LightManager::GetLight(0)->SetDiffuse(0.4f, 0.4f, 0.4f);
				LightManager::GetLight(0)->Enable(true);
				LightManager::GetLight()->MoveBy (Vector(0,1000,100));
				
				TextureMgr::Initialize();
				//RenderManager::Initialize();
				
				LoadScreen::LoadFont();
				LoadScreen::LoadTextures();

				Game::Initialize();
				//Game::iLevel = 1;
				//Game::pWorld->LoadLevel (Game::iLevel);
			// Finally, initialize the menu
				Menu::Initialize();
				//Menu::ShowClassLogo(WINDOW_WIDTH);

}


///////////////////////////////////////////////////////////////
// Called when we shut down.
// Place any shutdown of core tech modules here
////////////////////////////////////////////////////////////////
void OnShutdown(void)
{
	// Game class.
	Game::Shutdown();

	// Kill sound
	SoundManager::Shutdown();

	// Kill DirectInput.
	DirectInput::Shutdown();

	// Kill renderers
	//RenderManager::Shutdown();

	// Renderer
	Direct3D::Shutdown();
		TextureMgr::Shutdown();
		// LIGHT MANAGER HAS NO SHUTDOWN
		DXFonts::Shutdown();


	// Kill memory monitor last if we used it. Dump out a log file of any un-released memory before we go.
	#if _DEBUG
		MemoryManager::DumpToFile();
		MemoryManager::Shutdown();
	#endif


}


//////////////////
// WinMain!
//////////////////
int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprevinstance, LPSTR lpcmdline, int ncmdshow)
{
	/////////////////////////////////////
	// Initialize core technologies.
	/////////////////////////////////////
	HWND hWnd = WindowCreate(hinstance);
	if (!hWnd) return 0;
    MSG msg;	
	OnStartup(hWnd, hinstance);		// Run all our manager initializations.
	
	////////////////////////////////////////
	// Main loop... 
	////////////////////////////////////////
 	while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) 
	{
		
		/////////////////////////////////////////////
		// Windows gets first dibs...
		/////////////////////////////////////////////
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		
		/////////////////////////////////////////////
		// CORE TECH PER-FRAME TASKS
		/////////////////////////////////////////////
		
			// Update elapsed time
			Timer::Update();

			// Cull input from buffers and re-acquire devices.
			DirectInput::Poll();

			// Handle Windows commands ALT-TAB and ALT-ENTER with Direct3D.
			Direct3D::HandleAltTab();
			if ((	DirectInput::GetKeyboard()->CheckImmediateInput(DIK_LALT) ||
					DirectInput::GetKeyboard()->CheckImmediateInput(DIK_RALT)) &&
					DirectInput::GetKeyboard()->CheckImmediateInput(DIK_RETURN))
			{
				Direct3D::FullScreen();
				continue;
			}

			//////////////////////////
			// Run game loop!
			//////////////////////////
					
			if (Menu::bInMenu)
			{
				if (!Menu::Loop())
					break;			
			}
			
			else 
			{
				if (!Game::GameLoop()) 
					break; 
			}	
	}
	////////////////////////////////////////////
	// Do all core tech shutdown tasks
	////////////////////////////////////////////
	OnShutdown();
	
	/////////////////////////////////
	// And back to the OS...
	/////////////////////////////////
	UnregisterClass(CLASSNAME, hinstance);
	return (int)(msg.wParam);
}


////////////////////////////////////////////////
// Process Windows messages
////////////////////////////////////////////////
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	
	//	What is the message 
	switch(msg)
	{
		
		case WM_SIZE:			
			return 0;
		break;

		case WM_DESTROY: 
		{
			PostQuitMessage(0);
			return(0);
		}
		break;

		case WM_SYSCOMMAND:							// Intercept System Commands
		{
			switch (wparam)									
			{
				case SC_SCREENSAVE:					// Screensaver Trying To Start?
				case SC_MONITORPOWER:				// Monitor Trying To Enter Powersave?
				return 0;											// Prevent From Happening
			}
			break;													// Exit
		}

	}

		//////////////////////////////////////////////////
		//	Give Windows the leftover messages.
		/////////////////////////////////////////////////
		return (DefWindowProc(hwnd, msg, wparam, lparam));
}

// Make the actual window
HWND WindowCreate(HINSTANCE hinstance)
{
		WNDCLASS winclass;		//	This will hold the class we create
	
	///////////////////////////////////
	// Create the Window Class..
	///////////////////////////////////

		//	First fill in the window class stucture
		winclass.style					= CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
		winclass.lpfnWndProc		= WindowProc;
		winclass.cbClsExtra			= 0;
		winclass.cbWndExtra		= 0;
		winclass.hInstance			= hinstance;
		winclass.hIcon					= LoadIcon(NULL, IDI_APPLICATION);
		winclass.hCursor				= LoadCursor(NULL, IDC_ARROW);
		winclass.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
		winclass.lpszMenuName	= NULL; 
		winclass.lpszClassName	= CLASSNAME;

		//	Register the window class
		if (!RegisterClass(&winclass))
			return(0);
	
		int iScrX = GetSystemMetrics(SM_CXSCREEN);
		int iScrY = GetSystemMetrics(SM_CYSCREEN);
		int iWinX = WINDOW_WIDTH;
		int iWinY = WINDOW_HEIGHT;

		//	Create the window
		HWND hWnd = CreateWindow(	
										CLASSNAME,											//	Class Name.
										CLASSNAME,											//	Title of the Window.

										//	Window Flags.
										(WINDOW_FULLSCREEN) ? WS_POPUP | WS_VISIBLE : WS_OVERLAPPEDWINDOW 	| WS_VISIBLE,		

										(iScrX >> 1) -	(iWinX >> 1),		// Window start X
										(iScrY >> 1) -	(iWinY >> 1),		// Window start Y
										
										(WINDOW_FULLSCREEN) ? iScrX : iWinX,				//	Width of Window.
										(WINDOW_FULLSCREEN) ? iScrY : iWinY,				//	Height of Window.
										NULL,																	//	Handle to parent.
										NULL,																	//	Handle to menu.
										hinstance,															//	Application Instance.
										NULL);																	//	Creation parms.

			return(hWnd);

}


