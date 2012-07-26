

#ifndef StormForge_Shared_Menu
#define StormForge_Shared_Menu

//#define SCALE_1024_TO_800 0.78125f

#include "Direct3D.h"
#include "D3DXFont.h"
#include "DirectInput2.h"
#include "SoundManager.h"
#include "game.h"
#include "LoadScreen.h"

#ifndef Center
#define Center(XSize) ((Direct3D::GetScreenWidth() - XSize) * 0.5f)
#define CenterY(YSize) ((Direct3D::GetScreenHeight() - YSize) * 0.5f)
#endif 

enum Resolution
{
	RESOLUTION_640X480,
	RESOLUTION_800X600,
	RESOLUTION_1024X768,
	RESOLUTION_COUNT,
};



#define NUMOPTS 3

#define Center(XSize) ((Direct3D::GetScreenWidth() - XSize) * 0.5f)

enum MenuStates
{
	MENU_MAIN,
	MENU_CREDITS1,
	MENU_CREDITS2,
	MENU_CREDITS3,
	MENU_CREDITS4,
	MENU_CREDITS5,
	MENU_OPTIONS,
	MENU_INTROMOVIE,
	MENU_WINMOVIE,
	MENU_DEATHMOVIE,
	MENU_NEWGAME,
	MENU_NEWGAME_DIFF,
	MENU_NEWGAME_SPEED,
	MENU_EXTRAS,
	MENU_SETSOUND,
	MENU_SETRESOLUTION,
	MENU_SETCONTROLS,
	MENU_SETFEEDBACK,
};

class Menu
{

public:

	static bool bInMenu;
	static int iWhere;
	static int iBark;
	static int iTechLogos;
	static int iGoNTitle;
	static int iMenuOpt, iMenuPlay, iMenuCreds, iMenuExtra, iMenuQuit, iMenuSelect, iMenuResume;
	static int iMenuOptRes, iMenuOptSound, iMenuOptBack, iMenuOptFeedback, iMenuOptControl;
	static int iDeath1, iDeath2;
	static int iWin1, iWin2;
	static int iL2S1, iL2S2, iL3S1, iL3S2;
	static int iSelection;
	static int iSelectOpt;
	static int iSoundVol;
	static int iResolution;
	static int iSlider, iSliderTrack;
	static int iMenuOn, iMenuOff;
	static int iDrums;
	static int iSelectNew;
	static int iSelectDiff;
	static float fSelectSpeed;
	static bool bScreenshot;
	static bool bMusic;
	static int iHelpTexture;
	static bool bFirstGame;
	static int iMenuNewDifficulty, iMenuNewSpeed, iMenuNewStart;
	static int iSlides[5];
	static float fMovieTime, fMovieStart, fJoyTime, fMovieEnd;

	static bool bIsPlaying;

	static bool bFeedbackOK;


	// Set sound volume
	static void MoveSound(int iChange)
	{
		iSoundVol += iChange;
		if (iSoundVol < 0) iSoundVol = 0;
		if (iSoundVol > 255) iSoundVol = 255;
	}



	// Set resolution
	static void SetResolution(int iChange)
	{
		iResolution += iChange;
		if (iResolution < 0) iResolution = 0;
		if (iResolution >= RESOLUTION_COUNT) iResolution = RESOLUTION_COUNT - 1;
	}

	// Set difficulty
	static void SetDifficulty(int iChange)
	{
		iSelectDiff += iChange;
		if (iSelectDiff < 0) iSelectDiff = 0;
		if (iSelectDiff >= DIFFICULTY_COUNT) iSelectDiff = DIFFICULTY_COUNT - 1;
	}

	// Set speed
	static void SetSpeed(float fChange)
	{
		fSelectSpeed += fChange;
		if (fSelectSpeed < 0.1f) fSelectSpeed = 0.1f;
		if (fSelectSpeed >= 3.0f) fSelectSpeed = 3.0f;
	}

	static bool Loop(void)
	{
		if (bScreenshot) Direct3D::StartScreenshot();

		switch (iWhere)
		{
			case MENU_MAIN:						Main();				break;

			case MENU_OPTIONS:					
			case MENU_SETSOUND:				
			case MENU_SETFEEDBACK:
			case MENU_SETCONTROLS:
			case MENU_SETRESOLUTION:		Options();		break;

			case MENU_NEWGAME_DIFF:
			case MENU_NEWGAME_SPEED:
			case MENU_NEWGAME:				NewGameMenu(); break;

			case MENU_WINMOVIE:				WinMovie();		break;
			case MENU_DEATHMOVIE:			DeathMovie();	break;
			case MENU_INTROMOVIE:			IntroMovie();	break;

			case MENU_CREDITS1:				Credits1();		break;
			case MENU_CREDITS2:				Credits2();		break;
			case MENU_CREDITS3:				Credits3();		break;
			case MENU_CREDITS4:				Credits4();		break;
			case MENU_CREDITS5:				Credits5();		break;
		}


		if (bScreenshot)
		{
			bScreenshot = false;
			Direct3D::SaveScreenshot("screenshots\\SSHOT");
		}

		return HandleInput();
	}

	static void NewGame(void)
	{
		Game::pWorld->LoadLevel (STARTING_LEVEL);
		bFirstGame = false;
		Menu::Shutdown();

		Game::iGameState = GAME_WELCOME;
	}


	static void LoadFonts(void)
	{
		DXFonts::Shutdown();
		DXFonts::Create("Arial", 18, true);
		DXFonts::Create("Verdana", 24, true, true);
		DXFonts::Create("Verdana", 36, true, true);
		DXFonts::Create("Verdana", 26, true, true);
	}

	static void Initialize(void)
	{


		bInMenu = true;
		bScreenshot = false;
		

		LoadFonts();

		// Sheesh! Set up the load screen
		LoadScreen::LoadFont();
		LoadScreen::Reset();

		LoadScreen::AddSize("textures\\sl_viewfromthetop.jpg");
		LoadScreen::AddSize("textures\\sl_house.jpg");
		LoadScreen::AddSize("textures\\sl_fortdoor.jpg");
		LoadScreen::AddSize("textures\\sl_fungus.jpg");
		LoadScreen::AddSize("textures\\sl_vista.jpg");
		LoadScreen::AddSize("textures\\sl_rockstrees.jpg");
		LoadScreen::AddSize("textures\\sl_stumps.jpg");
		LoadScreen::AddSize("textures\\sl_cave.jpg");
		LoadScreen::AddSize("textures\\sl_stonefort.jpg");
		LoadScreen::AddSize("textures\\bark4.bmp");
		LoadScreen::AddSize("textures\\technologies.bmp");
		LoadScreen::AddSize("textures\\gontitle_green.png");
		LoadScreen::AddSize("textures\\menu_newgame.png");
		LoadScreen::AddSize("textures\\menu_resume.png");
		LoadScreen::AddSize("textures\\menu_options.png");
		LoadScreen::AddSize("textures\\menu_credits.png");
		LoadScreen::AddSize("textures\\menu_extras.png");
		LoadScreen::AddSize("textures\\menu_quit.png");
		LoadScreen::AddSize("textures\\menu_select.png");
		LoadScreen::AddSize("textures\\water.bmp");
		LoadScreen::AddSize("textures\\menu_opt_resolution.png");
		LoadScreen::AddSize("textures\\menu_opt_control.png");
		LoadScreen::AddSize("textures\\menu_opt_feedback.png");
		LoadScreen::AddSize("textures\\menu_opt_sound.png");
		LoadScreen::AddSize("textures\\menu_back.png");
		LoadScreen::AddSize("textures\\menu_on.png");
		LoadScreen::AddSize("textures\\menu_off.png");
		LoadScreen::AddSize("sounds\\menudrums.mp3");
		LoadScreen::AddSize("Textures\\menu_slider_thingy.png");
		LoadScreen::AddSize("Textures\\menu_slider_track.png");
		LoadScreen::AddSize("Textures\\menu_new_start.png");
		LoadScreen::AddSize("Textures\\menu_new_diff.png");
		LoadScreen::AddSize("Textures\\menu_new_speed.png");

 		 // Load textures (intro movie)
		LoadScreen::Draw("Textures\\sl_vista.jpg");
		iSlides[0] = TextureMgr::LoadTexture("textures\\sl_vista.jpg");
		LoadScreen::Draw("Textures\\sl_rockstrees.jpg");
		iSlides[1] = TextureMgr::LoadTexture("textures\\sl_rockstrees.jpg");
		LoadScreen::Draw("Textures\\sl_stumps.jpg");
		iSlides[2] = TextureMgr::LoadTexture("textures\\sl_stumps.jpg");
		LoadScreen::Draw("Textures\\sl_cave.jpg");
		iSlides[3] = TextureMgr::LoadTexture("textures\\sl_cave.jpg");
		LoadScreen::Draw("Textures\\sl_stonefort.jpg");
		iSlides[4] = TextureMgr::LoadTexture("textures\\sl_stonefort.jpg");		

		// Death movie
		LoadScreen::Draw("Textures\\sl_fungus.jpg");
		iDeath1 = TextureMgr::LoadTexture("textures\\sl_fungus.jpg");
		LoadScreen::Draw("Textures\\sl_house.jpg");
		iDeath2 = TextureMgr::LoadTexture("textures\\sl_house.jpg");

		// Win movie
		LoadScreen::Draw("Textures\\sl_fortdoor.jpg");
		iWin1 = TextureMgr::LoadTexture("textures\\sl_fortdoor.jpg");
		LoadScreen::Draw("Textures\\sl_viewfromthetop.jpg");
		iWin2 = TextureMgr::LoadTexture("textures\\sl_viewfromthetop.jpg");

		//Level exposition picts
		LoadScreen::Draw("Textures\\sl_level2.png");
		iL2S1=iL2S2=TextureMgr::LoadTexture("Textures\\sl_level2.png");
		LoadScreen::Draw("Textures\\sl_level3.png");
		iL3S1=iL3S2=TextureMgr::LoadTexture("Textures\\sl_level3.png");


		// Menu textures
		LoadScreen::Draw("Textures\\bark4.bmp");
		iBark = TextureMgr::LoadTexture("textures\\Bark4.bmp");
		LoadScreen::Draw("Textures\\technologies.bmp");
		iTechLogos = TextureMgr::LoadTexture("textures\\technologies.bmp");
		LoadScreen::Draw("Textures\\gontitle_green.png");
		iGoNTitle = TextureMgr::LoadTexture("Textures\\GoNTitle_green.png");
		LoadScreen::Draw("Textures\\menu_newgame.png");
		iMenuPlay = TextureMgr::LoadTexture("Textures\\menu_newgame.png");
		LoadScreen::Draw("Textures\\menu_resume.png");
		iMenuResume = TextureMgr::LoadTexture("Textures\\menu_resume.png");
		LoadScreen::Draw("Textures\\menu_options.png");
		iMenuOpt = TextureMgr::LoadTexture("Textures\\menu_options.png");
		LoadScreen::Draw("Textures\\menu_credits.png");
		iMenuCreds = TextureMgr::LoadTexture("Textures\\menu_credits.png");
		LoadScreen::Draw("Textures\\menu_extras.png");
		iMenuExtra = TextureMgr::LoadTexture("Textures\\menu_extras.png");
		LoadScreen::Draw("Textures\\menu_quit.png");
		iMenuQuit = TextureMgr::LoadTexture("Textures\\menu_quit.png");
		LoadScreen::Draw("Textures\\menu_select.png");
		iMenuSelect = TextureMgr::LoadTexture("Textures\\menu_select.png");
		LoadScreen::Draw("Textures\\water.bmp");
		iHelpTexture = TextureMgr::LoadTexture("Textures\\water.bmp");
		LoadScreen::Draw("Textures\\menu_opt_resolution.png");
		iMenuOptRes = TextureMgr::LoadTexture("Textures\\menu_opt_resolution.png");
		LoadScreen::Draw("Textures\\menu_opt_sound.png");
		iMenuOptSound = TextureMgr::LoadTexture("Textures\\menu_opt_sound.png");
		LoadScreen::Draw("Textures\\menu_opt_feedback.png");
		iMenuOptFeedback = TextureMgr::LoadTexture("Textures\\menu_opt_feedback.png");
		LoadScreen::Draw("Textures\\menu_opt_control.png");
		iMenuOptControl = TextureMgr::LoadTexture("Textures\\menu_opt_control.png");
		LoadScreen::Draw("Textures\\menu_back.png");
		iMenuOptBack = TextureMgr::LoadTexture("Textures\\menu_back.png");
		LoadScreen::Draw("Textures\\menu_on.png");
		iMenuOn = TextureMgr::LoadTexture("Textures\\menu_on.png");
		LoadScreen::Draw("Textures\\menu_off.png");
		iMenuOff = TextureMgr::LoadTexture("Textures\\menu_off.png");
		LoadScreen::Draw("Textures\\menu_slider_thingy.png");
		iSlider = TextureMgr::LoadTexture("Textures\\menu_slider_thingy.png");
		LoadScreen::Draw("Textures\\menu_slider_track.png");
		iSliderTrack = TextureMgr::LoadTexture("Textures\\menu_slider_track.png");
		LoadScreen::Draw("Textures\\menu_new_start.png");
		iMenuNewStart = TextureMgr::LoadTexture("Textures\\menu_new_start.png");
		LoadScreen::Draw("Textures\\menu_new_diff.png");
		iMenuNewDifficulty = TextureMgr::LoadTexture("Textures\\menu_new_diff.png");
		LoadScreen::Draw("Textures\\menu_new_speed.png");
		iMenuNewSpeed = TextureMgr::LoadTexture("Textures\\menu_new_speed.png");

		LoadScreen::Draw("sounds\\menudrums.mp3");
		iDrums = SoundManager::LoadStreamedSound("sounds\\menudrums.mp3", Menu::iSoundVol, true);

		// Kill load screen
		LoadScreen::Draw(NULL);
		LoadScreen::FadeOut();

		bScreenshot = false;
		DXFonts::Shutdown();
		DXFonts::Create("Arial", 18, true);
		DXFonts::Create("Verdana", 24, true, true);
		DXFonts::Create("Verdana", 36, true, true);
		DXFonts::Create("Verdana", 26, true, true);

		// Does the joystick support FF?
		if (DirectInput::GetJoystick(0))
			bFeedbackOK = DirectInput::GetJoystick(0)->SupportsForceFeedback();
		else
			bFeedbackOK = false;

		if (!bMusic)
		{
			int iSound = SoundManager::LoadStreamedSound("sounds\\menudrums.mp3", 150, true);
			SoundManager::Play(iSound);
			bMusic = true;
		}

	}
	
private:
	static bool KeyOrJoy(DWORD dwKey, DWORD dwJoy)
	{
		if (DirectInput::GetKeyboard()->CheckBufferedInput(dwKey))
			return true;
		if (DirectInput::GetJoystick(0))
			if (DirectInput::GetJoystick(0)->CheckBufferedInput(dwJoy))
				return true;
			
		return false;
	}

	

	
	static bool HandleInput(void)
	{

		long lYAxisAmount = 0;
		long lXAxisAmount = 0;

		if (DirectInput::GetJoystick(0))
		{
			if (Timer::AppTime() - fJoyTime > 0.1f)
			{
				fJoyTime = Timer::AppTime();
				lXAxisAmount = DirectInput::GetJoystick(0)->CheckImmediateInput(JOYSTICK_AXIS_LX);
				lYAxisAmount = DirectInput::GetJoystick(0)->CheckImmediateInput(JOYSTICK_AXIS_LY);
			}
		}
		
		// Screenshot
		if (KeyOrJoy(DIK_SYSRQ, JOYSTICK_BUTTON(10)))
		{
			bScreenshot = true;
			return true;
		}

		// Movies - just allow escape
		if (iWhere == MENU_INTROMOVIE || iWhere == MENU_DEATHMOVIE || iWhere == MENU_WINMOVIE)
		{
			if (KeyOrJoy(DIK_ESCAPE, JOYSTICK_BUTTON(0)))
				fMovieTime = 100.0f;

			return true;
		}
		

		// New game
		if (iWhere == MENU_NEWGAME)
		{
			if (KeyOrJoy(DIK_RETURN, JOYSTICK_BUTTON(0)))
			{
				switch(iSelectNew)
				{
				case 0:
					Timer::SetMotionSpeed(fSelectSpeed);
					StartIntroMovie();
					return true;

				case 1: 
					iWhere = MENU_NEWGAME_SPEED;
					return true;
				case 2: 
					 iWhere = MENU_NEWGAME_DIFF;
					return true;

				case 3: 
					iWhere = MENU_MAIN;
					return true;
				}
			}

			if (DirectInput::GetKeyboard()->CheckBufferedInput(DIK_UP) || lYAxisAmount < -900)
				iSelectNew = (iSelectNew) ? iSelectNew - 1 : 3;
		
			if (DirectInput::GetKeyboard()->CheckBufferedInput(DIK_DOWN) || lYAxisAmount > 900)
				iSelectNew = (iSelectNew == 3) ? 0 : iSelectNew + 1;

		}

		// Opts
		if (iWhere == MENU_OPTIONS)
		{
			if (KeyOrJoy(DIK_RETURN, JOYSTICK_BUTTON(0)))
			{
				switch(iSelectOpt)
				{
				case 0:
					iWhere = MENU_SETRESOLUTION;

					return true;
				case 1: 
					iWhere = MENU_SETSOUND;
					return true;
				case 2: 
					if (bFeedbackOK)
						DirectInput::GetJoystick(0)->ToggleForceFeedback(!DirectInput::GetJoystick(0)->IsFeedbackOn()); 
					return true;

				case 3: 
					iWhere = MENU_MAIN;
					return true;
				}
			}


			if (DirectInput::GetKeyboard()->CheckBufferedInput(DIK_UP) || lYAxisAmount < -900)
				iSelectOpt = (iSelectOpt) ? iSelectOpt - 1 : NUMOPTS;
		
			if (DirectInput::GetKeyboard()->CheckBufferedInput(DIK_DOWN) || lYAxisAmount > 900)
				iSelectOpt = (iSelectOpt == NUMOPTS) ? 0 : iSelectOpt + 1;


		}

		// Set sound resolution
		if (iWhere == MENU_SETSOUND)
		{
			if (DirectInput::GetKeyboard()->CheckImmediateInput(DIK_LEFT) || lXAxisAmount < -900)
				MoveSound(-1);

			if (DirectInput::GetKeyboard()->CheckImmediateInput(DIK_RIGHT) || lXAxisAmount > 900) 
				MoveSound(1);
			
			if (KeyOrJoy(DIK_RETURN, JOYSTICK_BUTTON(0)))
			{
				// CHANGED 

				// Set volumes for all loaded sounds
				SoundManager::SetAllVolumes(iSoundVol);

				// Change the volume of the MP3 currently playing.
				int iTime = SoundManager::GetTime(iDrums);
				//SoundManager::StopMusic();
				SoundManager::SkipTo(iDrums, iTime + 2);
				SoundManager::Play(iDrums);
								
				iWhere = MENU_OPTIONS;
			}

        }

		// Set sound volume
		if (iWhere == MENU_SETRESOLUTION)
		{
			if (DirectInput::GetKeyboard()->CheckBufferedInput(DIK_LEFT) || lXAxisAmount < -900 )
				SetResolution(-1);

			if (DirectInput::GetKeyboard()->CheckBufferedInput(DIK_RIGHT) || lXAxisAmount > 900)
				SetResolution(1);
			
			if (KeyOrJoy(DIK_RETURN, JOYSTICK_BUTTON(0)))
				iWhere = MENU_OPTIONS;
		}

		// Set difficulty
		if (iWhere == MENU_NEWGAME_SPEED)
		{
			if (DirectInput::GetKeyboard()->CheckImmediateInput(DIK_LEFT) || lXAxisAmount < -900 )
				SetSpeed(-0.1f);

			if (DirectInput::GetKeyboard()->CheckImmediateInput(DIK_RIGHT) || lXAxisAmount > 900)
				SetSpeed(0.1f);
			
			if (KeyOrJoy(DIK_RETURN, JOYSTICK_BUTTON(0)))
				iWhere = MENU_NEWGAME;
		}



		// Set sound resolution
		if (iWhere == MENU_NEWGAME_DIFF)
		{
			if (DirectInput::GetKeyboard()->CheckBufferedInput(DIK_LEFT) || lXAxisAmount < -900 )
				SetDifficulty(-1);

			if (DirectInput::GetKeyboard()->CheckBufferedInput(DIK_RIGHT) || lXAxisAmount > 900)
				SetDifficulty(1);
			
			if (KeyOrJoy(DIK_RETURN, JOYSTICK_BUTTON(0)))
				iWhere = MENU_NEWGAME;
		}



		if (iWhere == MENU_MAIN)
		{
			if (DirectInput::GetKeyboard()->CheckBufferedInput(DIK_UP) ||  lYAxisAmount<-900)
				iSelection = (iSelection) ? iSelection - 1 : 5;

			if (DirectInput::GetKeyboard()->CheckBufferedInput(DIK_DOWN) || lYAxisAmount>900)
				iSelection = (iSelection == 5) ? 0 : iSelection + 1;

			if (KeyOrJoy(DIK_RETURN, JOYSTICK_BUTTON(0)))
			{ 
				switch (iSelection)
				{
				case 0:
					iWhere = MENU_NEWGAME;
					return true;
				case 1:
					// Don't resume nonexistant games... Game go bang.
					if (Game::iGameState == GAME_NOTSTARTED) return true;

					Game::iGameState = GAME_PLAYING;
					bInMenu = false;
					Shutdown();
					return true;
				case 2:
					Menu::iWhere = MENU_OPTIONS;
					return true;
				case 3: 
					Menu::iWhere = MENU_CREDITS1;
					return true;
				case 4:
				//	Menu::iWhere = MENU_EXTRAS;
					return true;
				case 5:
					Shutdown();
					return false;
				}
			}
		}

		else if (KeyOrJoy(DIK_RETURN, JOYSTICK_BUTTON(0)) && Menu::iWhere == MENU_CREDITS1)
		{
			iWhere = MENU_CREDITS2;
		}

		else if (KeyOrJoy(DIK_RETURN, JOYSTICK_BUTTON(0)) && Menu::iWhere == MENU_CREDITS2)
		{
			iWhere = MENU_CREDITS3;
		}
		else if (KeyOrJoy(DIK_RETURN, JOYSTICK_BUTTON(0)) && Menu::iWhere == MENU_CREDITS3)
		{
			iWhere = MENU_CREDITS4;
		}
		else if (KeyOrJoy(DIK_RETURN, JOYSTICK_BUTTON(0)) && Menu::iWhere == MENU_CREDITS4)
		{
			iWhere = MENU_CREDITS5;
		}
		else if (KeyOrJoy(DIK_RETURN, JOYSTICK_BUTTON(0)) && Menu::iWhere == MENU_CREDITS5)
		{
			iWhere = MENU_MAIN;
		}

		return true;

		
	}





	static void Shutdown(void)  
	{ 

		// Unload textures
		TextureMgr::UnloadTexture(iBark);
		TextureMgr::UnloadTexture(iTechLogos);
		TextureMgr::UnloadTexture(iGoNTitle);
		TextureMgr::UnloadTexture(iMenuPlay);
		TextureMgr::UnloadTexture(iMenuResume);
		TextureMgr::UnloadTexture(iMenuOpt);
		TextureMgr::UnloadTexture(iMenuCreds);
		TextureMgr::UnloadTexture(iMenuExtra);
		TextureMgr::UnloadTexture(iMenuQuit);
		TextureMgr::UnloadTexture(iMenuSelect);
		
		// Invalidate all texture IDs
		iBark = INVALID;
		iTechLogos = INVALID;
		iGoNTitle = INVALID;
		iMenuPlay = INVALID;
		iMenuResume = INVALID;
		iMenuOpt = INVALID;
		iMenuCreds = INVALID;
		iMenuExtra = INVALID;
		iMenuQuit = INVALID;
		iMenuSelect = INVALID;

		// Put it on "new game"
		iSelection = 0;

		// Kill fonts
		DXFonts::Shutdown();
		DXFonts::Create("Arial", 18, true);
		LoadScreen::LoadFont();
	
		// Kill the drums
		//SoundManager::StopMusic();

		// Set other values
		bInMenu = false;
		bScreenshot = false;
		bMusic = false;
		
		

	}


public:
	private:


	static void Credits1(void)
	{
		Direct3D::Clear();
		Direct3D::Begin();

			float fInvSize = (1.0f / 512.0f);
			float fSizeX = fInvSize * Direct3D::GetScreenWidth();
			float fSizeY = fInvSize * Direct3D::GetScreenHeight();
			
			TextureMgr::Draw(iBark, 0.0f, 0.0f, NULL, fSizeX, fSizeY, 3.141592f);
		
			#define SHADOW 4

			DXFonts::Begin(2);
				DXFonts::WriteShadow("StormForge Entertainment Group", 125, 20, SHADOW, 0.3f, 0.3f, 0.7f);
			DXFonts::End();
			DXFonts::Begin(3);

			// Headings
				DXFonts::WriteShadow("Programming", Direct3D::GetScreenWidth() - 185, 56, SHADOW);
				DXFonts::WriteShadow("Matt Holden",				150, 125, SHADOW, 0.3f, 0.3f, 0.7f);
				DXFonts::WriteShadow("Rolf Hendriks",				150, 275, SHADOW, 0.3f, 0.3f, 0.7f);
				DXFonts::WriteShadow("Christopher Winings",	150, 425, SHADOW, 0.3f, 0.3f, 0.7f);
				DXFonts::WriteShadow("Next: Art Department ",  Direct3D::GetScreenWidth() - 300, Direct3D::GetScreenHeight() - 70, SHADOW);
				DXFonts::WriteShadow("(Press Enter)", Direct3D::GetScreenWidth() - 175, Direct3D::GetScreenHeight() - 35, SHADOW);

			DXFonts::End();
			DXFonts::Begin(0);		

				// Matt
				DXFonts::WriteShadow("Direct3D Rendering Engine", 175, 150);
				DXFonts::WriteShadow("Direct3D Model Loading and Animation System", 175, 170);
				DXFonts::WriteShadow("Direct3D Point Sprites", 175, 190);
				DXFonts::WriteShadow("DirectInput Action Mapping System", 175, 210);
				DXFonts::WriteShadow("User Interface", 175, 230);
				DXFonts::WriteShadow("Memory Manager", 175, 250);

				// Rolf
				DXFonts::WriteShadow("Collision Detection and Response", 175, 300);
				DXFonts::WriteShadow("Matrix and Vector Math Library", 175, 320);
				DXFonts::WriteShadow("Enemy Decision-Making AI", 175, 340);
				DXFonts::WriteShadow("Optimization", 175, 360);
				DXFonts::WriteShadow("Terrain Following and Partitioning", 175, 380);
				DXFonts::WriteShadow("FMOD Sound Module", 175, 400);
				
				// Chris
				DXFonts::WriteShadow("Control Implementation", 175, 450);
				DXFonts::WriteShadow("Arrow Motion and Physics", 175, 470);
				DXFonts::WriteShadow("Arrow Targeting System", 175, 490);
				DXFonts::WriteShadow("Special Attacks", 175, 510);
				DXFonts::WriteShadow("Level Design", 175, 530);
				
			DXFonts::End();
		


		Direct3D::End();
		Direct3D::Present();



	}



	static void Main(void)
	{
		// If we need to load stuff, do it.
		if (iBark == INVALID)
			Initialize();

		Direct3D::Clear();
		Direct3D::Begin();

			float fScaleX = float(Direct3D::GetScreenWidth()) / 1024.0f;
			float fScaleY = float(Direct3D::GetScreenHeight()) / 800.0f;

			float fInvSize = (1.0f / 512.0f);
			float fSizeX = fInvSize * Direct3D::GetScreenWidth();
			float fSizeY = fInvSize * Direct3D::GetScreenHeight();
			
			TextureMgr::Draw(iBark, 0.0f, 0.0f, NULL, fSizeX, fSizeY, 3.141592f);
			TextureMgr::Draw(iGoNTitle, 100, 25, NULL, fScaleX, fScaleY);


			float fLeft = (.25f * Direct3D::GetScreenWidth() * fScaleX);

			// Draw the "selection" bar
			TextureMgr::Draw(iMenuSelect, fLeft, (240 + (iSelection * 65)) * fScaleY, NULL, fScaleX, fScaleY);
			
			// Draw selections
			TextureMgr::Draw(iMenuPlay, fLeft,  (240 * fScaleY), NULL, fScaleX, fScaleY);
			TextureMgr::Draw(iMenuResume, fLeft,  (305 * fScaleY), NULL, fScaleX, fScaleY, 0, (Game::iGameState == GAME_NOTSTARTED) ? 64 : 255);
			TextureMgr::Draw(iMenuOpt, fLeft, (370 * fScaleY), NULL, fScaleX, fScaleY);
			TextureMgr::Draw(iMenuCreds, fLeft, (435 * fScaleY), NULL, fScaleX, fScaleY);
			TextureMgr::Draw(iMenuExtra, fLeft, (500 * fScaleY), NULL, fScaleX, fScaleY, 0, 64);
			TextureMgr::Draw(iMenuQuit, fLeft, (565 * fScaleY), NULL, fScaleX, fScaleY);
			

			Direct3D::End();
			Direct3D::Present();

	}


	static void NewGameMenu(void)
	{
			Direct3D::Clear();
			Direct3D::Begin();

			// Set background stuff
			float fScaleX = float(Direct3D::GetScreenWidth()) / 1024.0f;
			float fScaleY = float(Direct3D::GetScreenHeight()) / 800.0f;
			float fInvSize = (1.0f / 512.0f);
			float fSizeX = fInvSize * Direct3D::GetScreenWidth();
			float fSizeY = fInvSize * Direct3D::GetScreenHeight();
			TextureMgr::Draw(iBark, 0.0f, 0.0f, NULL, fSizeX, fSizeY, 3.141592f);
			TextureMgr::Draw(iGoNTitle, 100, 25, NULL, fScaleX, fScaleY);
			float fLeft = (.25f * Direct3D::GetScreenWidth() * fScaleX);

			// Draw the "selection" bar
			TextureMgr::Draw(iMenuSelect, fLeft, (200 + (iSelectNew * 95)) * 1.0f, NULL, 1.0f, 1.0f);

			// Draw selections
			TextureMgr::Draw(iMenuNewStart, fLeft,  200.0f, NULL, 1.0f, 1.0f);
			TextureMgr::Draw(iMenuNewSpeed, fLeft,  295.0f, NULL, 1.0f, 1.0f);
			TextureMgr::Draw(iMenuNewDifficulty, fLeft,  (390 * 1.0f), NULL, 1.0f, 1.0f);
			TextureMgr::Draw(iMenuOptBack, fLeft, (485 * 1.0f), NULL, 1.0f, 1.0f);
			
			DXFonts::Begin(3);


			// Draw any inner state stuff
			TextureMgr::Draw(iSliderTrack, fLeft, 340.0f, NULL, 267.0f, 1.0f);
			TextureMgr::Draw(iSlider, float(fLeft + (72 + int(175 * (fSelectSpeed - 0.5f)))), 350.0f, NULL, 0.75f, 0.6f);

			// Draw any inner state stuff
			if (iWhere == MENU_NEWGAME_DIFF)
			{
				switch (iSelectDiff)
				{
				case DIFFICULTY_VERYEASY: 	DXFonts::WriteShadow(DIFFICULTY_NAME_VERYEASY, int(fLeft + 50), 448);	break;
				case DIFFICULTY_EASY: 	DXFonts::WriteShadow(DIFFICULTY_NAME_EASY, int(fLeft + 50), 448);	break;
				case DIFFICULTY_NORMAL: 	DXFonts::WriteShadow(DIFFICULTY_NAME_NORMAL, int(fLeft + 50), 448);	break;
				case DIFFICULTY_HARD: 	DXFonts::WriteShadow(DIFFICULTY_NAME_HARD, int(fLeft + 50), 448);	break;
				case DIFFICULTY_VERYHARD: 	DXFonts::WriteShadow(DIFFICULTY_NAME_VERYHARD, int(fLeft + 50), 448);	break;
				}
			}
			else
			{
				switch (iSelectDiff)
				{
				case DIFFICULTY_VERYEASY: 	DXFonts::WriteShadow(DIFFICULTY_NAME_VERYEASY, int(fLeft + 50), 448, 3, 0.3f, 0.7f, 0.3f);	break;
				case DIFFICULTY_EASY: 	DXFonts::WriteShadow(DIFFICULTY_NAME_EASY, int(fLeft + 50), 448, 3, 0.3f, 0.7f, 0.3f);	break;
				case DIFFICULTY_NORMAL: 	DXFonts::WriteShadow(DIFFICULTY_NAME_NORMAL, int(fLeft + 50), 448, 3, 0.3f, 0.7f, 0.3f);	break;
				case DIFFICULTY_HARD: 	DXFonts::WriteShadow(DIFFICULTY_NAME_HARD, int(fLeft + 50), 448, 3, 0.3f, 0.7f, 0.3f);	break;
				case DIFFICULTY_VERYHARD: 	DXFonts::WriteShadow(DIFFICULTY_NAME_VERYHARD, int(fLeft + 50), 448, 3, 0.3f, 0.7f, 0.3f);	break;
				}
				
			}
			// Signify this as the "new game" menu
			
			DXFonts::WriteShadow("Create New Game", Direct3D::GetScreenWidth() - 280, 120, SHADOW);
			DXFonts::End();

			Direct3D::End();
			Direct3D::Present();


	}

	static void Options(void)
	{

		Direct3D::Clear();
		Direct3D::Begin();

			// Set background stuff
			float fScaleX = float(Direct3D::GetScreenWidth()) / 1024.0f;
			float fScaleY = float(Direct3D::GetScreenHeight()) / 800.0f;
			float fInvSize = (1.0f / 512.0f);
			float fSizeX = fInvSize * Direct3D::GetScreenWidth();
			float fSizeY = fInvSize * Direct3D::GetScreenHeight();
			TextureMgr::Draw(iBark, 0.0f, 0.0f, NULL, fSizeX, fSizeY, 3.141592f);
			TextureMgr::Draw(iGoNTitle, 100, 25, NULL, fScaleX, fScaleY);
			float fLeft = (.25f * Direct3D::GetScreenWidth() * fScaleX);

			// Draw the "selection" bar
			TextureMgr::Draw(iMenuSelect, fLeft, (200 + (iSelectOpt * 95)) * 1.0f, NULL, 1.0f, 1.0f);

			// Force feedback
			if (bFeedbackOK)
			{
				TextureMgr::Draw(iMenuOptFeedback, fLeft,  (386 * 1.0f), NULL, 1.0f, 1.0f);
				if (DirectInput::GetJoystick(0)->IsFeedbackOn()) 
					TextureMgr::Draw(iMenuOn, fLeft + 383,  (386 * 1.0f), NULL, 1.0f, 1.0f);
				else
					TextureMgr::Draw(iMenuOff, fLeft + 383,  (386 * 1.0f), NULL, 1.0f, 1.0f);
			}
			else
			{
				TextureMgr::Draw(iMenuOptFeedback, fLeft,  (386 * 1.0f), NULL, 1.0f, 1.0f, 0, 64);
				TextureMgr::Draw(iMenuOff, fLeft + 383,  (386 * 1.0f), NULL, 1.0f, 1.0f, 0, 64);
			}

			// Draw selections
			TextureMgr::Draw(iMenuOptRes, fLeft,  200.0f, NULL, 1.0f, 1.0f);
			TextureMgr::Draw(iMenuOptSound, fLeft,  295.0f, NULL, 1.0f, 1.0f);
			
			TextureMgr::Draw(iMenuOptControl, fLeft,  (435 * 1.0f), NULL, 1.0f, 1.0f);
			TextureMgr::Draw(iMenuOptBack, fLeft, (480 * 1.0f), NULL, 1.0f, 1.0f);
			
			DXFonts::Begin(3);


			// Draw any inner state stuff
			TextureMgr::Draw(iSliderTrack, fLeft, 340.0f, NULL, 267.0f, 1.0f);
			TextureMgr::Draw(iSlider, float(fLeft + (2 * iSoundVol)), 350.0f, NULL, 0.75f, 0.6f);

			// Draw any inner state stuff
			if (iWhere == MENU_SETRESOLUTION)
			{
				switch (iResolution)
				{
				case RESOLUTION_640X480: 	DXFonts::WriteShadow("640 x 480", int(fLeft + 50), 258);	break;
				case RESOLUTION_800X600: 	DXFonts::WriteShadow("800 x 600", int(fLeft + 50), 258);	break;
				case RESOLUTION_1024X768: DXFonts::WriteShadow("1024 x 768", int(fLeft + 50), 258);	break;
				}
			}
			else
			{
				switch (iResolution)
				{
				case RESOLUTION_640X480: 	DXFonts::WriteShadow("640 x 480", int(fLeft + 50), 258, 3, 0.3f, 0.7f, 0.3f);	break;
				case RESOLUTION_800X600: 	DXFonts::WriteShadow("800 x 600", int(fLeft + 50), 258, 3, 0.3f, 0.7f, 0.3f);	break;
				case RESOLUTION_1024X768: DXFonts::WriteShadow("1024 x 768", int(fLeft + 50), 258, 3, 0.3f, 0.7f, 0.3f);	break;
				}
			}
			// Signify this as the "options" menu
			
			DXFonts::WriteShadow("Options Menu", Direct3D::GetScreenWidth() - 280, 120, SHADOW);
			DXFonts::End();

			Direct3D::End();
			Direct3D::Present();

	}




public:
	static void ShowClassLogo(int iWindowWidth)
	{

				// Load the drums and play 'em
				int iSound = SoundManager::LoadStreamedSound("sounds\\menudrums.mp3", 150, true);
				
				// Show the class logo for 5 seconds (required)
				int iClassLogo = TextureMgr::LoadTexture("textures\\404namenotfound.bmp");
				int iForge = TextureMgr::LoadTexture("textures\\forgetemp.jpg");

				// Play dem drums!
				SoundManager::Play(iSound);
				bMusic = true;
				float fSoundStartTime = Timer::AppTime();

				// Scale the logo in 800x600
				//float fScaleSize = (iWindowWidth == 800) ? SCALE_1024_TO_800 : 1.0f;

				
				float fScaleX = Direct3D::GetScreenWidth() / TextureMgr::GetBmpWidth(iClassLogo);
				float fScaleY = Direct3D::GetScreenHeight() / TextureMgr::GetBmpHeight(iClassLogo);

				// Draw it fully opaque for 4 seconds
				float fTimeLogo = Timer::AppTime();
				while (Timer::AppTime() <= fTimeLogo + 4.0f)
				{
					Direct3D::Clear();
					Direct3D::Begin();
					TextureMgr::Draw(iClassLogo, 0.0f, 0.0f, NULL, fScaleX, fScaleY);
					Direct3D::End();
					Direct3D::Present();
				}
				
				// Fade it out over one second	
				fTimeLogo = Timer::AppTime();
				float fCurrentTime = fTimeLogo;

				while (fCurrentTime <= fTimeLogo + 1.0f)
				{
					Direct3D::Clear();
					Direct3D::Begin();
					TextureMgr::Draw(iClassLogo, 0.0f, 0.0f, NULL, fScaleX, fScaleY, 0.0f, int(255.0f * (fTimeLogo + 1.0f - fCurrentTime)));
					Direct3D::End();
					Direct3D::Present();
					fCurrentTime = Timer::AppTime();
				}

				fScaleX = Direct3D::GetScreenWidth() / TextureMgr::GetBmpWidth(iForge);
				fScaleY = Direct3D::GetScreenHeight() / TextureMgr::GetBmpHeight(iForge);

				// Draw it fully opaque for 4 seconds
				fTimeLogo = Timer::AppTime();
				while (Timer::AppTime() <= fTimeLogo + 4.0f)
				{
					Direct3D::Clear();
					Direct3D::Begin();
					TextureMgr::Draw(iForge, 0.0f, 0.0f, NULL, fScaleX, fScaleY);
					Direct3D::End();
					Direct3D::Present();
				}
				
				// Fade it out over one second	
				fTimeLogo = Timer::AppTime();
				fCurrentTime = fTimeLogo;

 				while (fCurrentTime <= fTimeLogo + 1.0f)
				{
					Direct3D::Clear();
					Direct3D::Begin();
					TextureMgr::Draw(iForge, 0.0f, 0.0f, NULL, fScaleX, fScaleY, 0.0f, int(255.0f * (fTimeLogo + 1.0f - fCurrentTime)));
					Direct3D::End();
					Direct3D::Present();
					fCurrentTime = Timer::AppTime();
				}

				// Unload 2.1 meg texture from memory!
				TextureMgr::UnloadTexture(iClassLogo);
				TextureMgr::UnloadTexture(iForge);

				// Wait for the timing in the sound
				while (fSoundStartTime + 10.5f > Timer::AppTime()) { } 		
	}

	private:


	static void Credits2(void)
	{
		Direct3D::Clear();
		Direct3D::Begin();

			float fInvSize = (1.0f / 512.0f);
			float fSizeX = fInvSize * Direct3D::GetScreenWidth();
			float fSizeY = fInvSize * Direct3D::GetScreenHeight();
			
			TextureMgr::Draw(iBark, 0.0f, 0.0f, NULL, fSizeX, fSizeY, 3.141592f);
		
			#define SHADOW 4

			DXFonts::Begin(2);
				DXFonts::WriteShadow("StormForge Entertainment Group", 125, 20, SHADOW, 0.3f, 0.3f, 0.7f);
			DXFonts::End();
			DXFonts::Begin(3);

			// Headings
				DXFonts::WriteShadow("Art Department", Direct3D::GetScreenWidth() - 210, 56, SHADOW);
				DXFonts::WriteShadow("Mike \"Chops\" Washington",	150, 115, SHADOW, 0.3f, 0.3f, 0.7f);
				DXFonts::WriteShadow("Jon Devin",				150, 180, SHADOW, 0.3f, 0.3f, 0.7f);
				DXFonts::WriteShadow("Jamie Fiore",	150, 235, SHADOW, 0.3f, 0.3f, 0.7f);
				DXFonts::WriteShadow("Rik Levins",	150, 290, SHADOW, 0.3f, 0.3f, 0.7f);
				DXFonts::WriteShadow("Brad Acree",				150, 345, SHADOW, 0.3f, 0.3f, 0.7f);
				DXFonts::WriteShadow("Jason McClesky",	150, 420, SHADOW, 0.3f, 0.3f, 0.7f);
				DXFonts::WriteShadow("Samantha Winings",	150, 485, SHADOW, 0.3f, 0.3f, 0.7f);

				DXFonts::WriteShadow("Next: Full Sail Staff ",  Direct3D::GetScreenWidth() - 280, Direct3D::GetScreenHeight() - 70, SHADOW);
				DXFonts::WriteShadow("(Press Enter)", Direct3D::GetScreenWidth() - 175, Direct3D::GetScreenHeight() - 35, SHADOW);

			DXFonts::End();
			DXFonts::Begin(0);		

			// Chops
				DXFonts::WriteShadow("Tempest Model, Texture Map and Animation", 175, 140);
				DXFonts::WriteShadow("Kara'Tok Texture Map", 175, 160);
			// Jon
				DXFonts::WriteShadow("Aurexis and Kara'Tok Models", 175, 205);
			// Jamie
				DXFonts::WriteShadow("Orc Scout Model, Texture Map and Animation", 175, 260);
			// Rik
				DXFonts::WriteShadow("Forest Level Model and Texture Map", 175, 315);
			// Brad
				DXFonts::WriteShadow("Static Object Models and Texture Maps", 175, 370);
				DXFonts::WriteShadow("Fort Level Model and Texture Maps", 175, 390);		
			// Jason
				DXFonts::WriteShadow("Music Composition", 175, 445);
				DXFonts::WriteShadow("Sound Effect and Dialogue Recording", 175, 465);
			// Samantha
				DXFonts::WriteShadow("Forest Photography (Sugarloaf Mountain, MD)", 175, 510);
				
			DXFonts::End();

		Direct3D::End();
		Direct3D::Present();

	}

	
	static void Credits3(void)
	{
		Direct3D::Clear();
		Direct3D::Begin();

			float fInvSize = (1.0f / 512.0f);
			float fSizeX = fInvSize * Direct3D::GetScreenWidth();
			float fSizeY = fInvSize * Direct3D::GetScreenHeight();
			
			TextureMgr::Draw(iBark, 0.0f, 0.0f, NULL, fSizeX, fSizeY, 3.141592f);
		
			#define SHADOW 4

			DXFonts::Begin(2);
				DXFonts::WriteShadow("StormForge Entertainment Group", 125, 20, SHADOW, 0.3f, 0.3f, 0.7f);
			DXFonts::End();
			DXFonts::Begin(3);

			int iLeft = int(.1875f * Direct3D::GetScreenWidth());

			// Headings
				DXFonts::WriteShadow("Full Sail Staff Thanks", Direct3D::GetScreenWidth() - 280, 56, SHADOW);
				DXFonts::WriteShadow("Rob Garfield",	iLeft, 125, SHADOW, 0.3f, 0.3f, 0.7f);
				DXFonts::WriteShadow("Justin Gallo",				iLeft, 175, SHADOW, 0.3f, 0.3f, 0.7f);
				DXFonts::WriteShadow("John Fairchild",	iLeft, 225, SHADOW, 0.3f, 0.3f, 0.7f);
				DXFonts::WriteShadow("Liam Hislop",	iLeft, 275, SHADOW, 0.3f, 0.3f, 0.7f);
				DXFonts::WriteShadow("Carlos Quiñenos",				iLeft, 325, SHADOW, 0.3f, 0.3f, 0.7f);
				DXFonts::WriteShadow("Will McGuire",	iLeft, 375, SHADOW, 0.3f, 0.3f, 0.7f);
				DXFonts::WriteShadow("Tony Whittaker",	iLeft, 425, SHADOW, 0.3f, 0.3f, 0.7f);
				DXFonts::WriteShadow("Lee Wood",	iLeft, 475, SHADOW, 0.3f, 0.3f, 0.7f);

				int iScrWidth = int(Direct3D::GetScreenWidth() * 0.375f);

				// Second column
				DXFonts::WriteShadow("Keith Staines",	iLeft + iScrWidth, 125, SHADOW, 0.3f, 0.3f, 0.7f);
				DXFonts::WriteShadow("John Fordinal",		iLeft + iScrWidth, 175, SHADOW, 0.3f, 0.3f, 0.7f);
				DXFonts::WriteShadow("Brandon Patterson ",	iLeft + iScrWidth, 225, SHADOW, 0.3f, 0.3f, 0.7f);
				DXFonts::WriteShadow("Rob Muscarella",	iLeft + iScrWidth, 275, SHADOW, 0.3f, 0.3f, 0.7f);
				DXFonts::WriteShadow("Susan Capozza",	iLeft + iScrWidth, 325, SHADOW, 0.3f, 0.3f, 0.7f);
				DXFonts::WriteShadow("Dustin Clingman",		iLeft + iScrWidth, 375, SHADOW, 0.3f, 0.3f, 0.7f);
				DXFonts::WriteShadow("Syrus Mesdaghi", iLeft + iScrWidth, 425, SHADOW, 0.3f, 0.3f, 0.7f);
				DXFonts::WriteShadow("Steve Murray",	iLeft + iScrWidth, 475, SHADOW, 0.3f, 0.3f, 0.7f);
				

				DXFonts::WriteShadow("Next: Third-Party Technologies ",  Direct3D::GetScreenWidth() - 420, Direct3D::GetScreenHeight() - 70, SHADOW);
				DXFonts::WriteShadow("(Press Enter)", Direct3D::GetScreenWidth() - 175, Direct3D::GetScreenHeight() - 35, SHADOW);
	
			DXFonts::End();

		Direct3D::End();
		Direct3D::Present();

	}

	
	static void Credits4(void)
	{
		
		Direct3D::Clear();
		Direct3D::Begin();

			float fInvSize = (1.0f / 512.0f);
			float fSizeX = fInvSize * Direct3D::GetScreenWidth();
			float fSizeY = fInvSize * Direct3D::GetScreenHeight();
			
			TextureMgr::Draw(iBark, 0.0f, 0.0f, NULL, fSizeX, fSizeY, 3.141592f);
		
			#define SHADOW 4

			DXFonts::Begin(2);
				DXFonts::WriteShadow("StormForge Entertainment Group", 125, 20, SHADOW, 0.3f, 0.3f, 0.7f);
			DXFonts::End();

			DXFonts::Begin(3);

			// Headings
				DXFonts::WriteShadow("Third-Party Technologies",  Direct3D::GetScreenWidth() - 330, 56, SHADOW);
           		DXFonts::WriteShadow("Next: Special Thanks ",  Direct3D::GetScreenWidth() - 300, Direct3D::GetScreenHeight() - 70, SHADOW);
				DXFonts::WriteShadow("(Press Enter)", Direct3D::GetScreenWidth() - 175, Direct3D::GetScreenHeight() - 35, SHADOW);	

			DXFonts::End();

			// Draw logos
				
		
				RECT rFMOD = { 3, 1, 305, 94 };		
				
				TextureMgr::Draw(iTechLogos, Center(rFMOD.right), 190.0f, &rFMOD);
				RECT rD3D = { 3, 128, 305, 175 };
				TextureMgr::Draw(iTechLogos, Center(rD3D.right), 320.0f, &rD3D);
				RECT rDInput = { 3, 179, 305, 225 };
				TextureMgr::Draw(iTechLogos, Center(rDInput.right), 390.0f, &rDInput);
			

		Direct3D::End();
		Direct3D::Present();

	}

	static void Credits5(void)
	{
		
		Direct3D::Clear();
		Direct3D::Begin();

			float fInvSize = (1.0f / 512.0f);
			float fSizeX = fInvSize * Direct3D::GetScreenWidth();
			float fSizeY = fInvSize * Direct3D::GetScreenHeight();
			
			TextureMgr::Draw(iBark, 0.0f, 0.0f, NULL, fSizeX, fSizeY, 3.141592f);
		
			#define SHADOW 4

			DXFonts::Begin(2);
				DXFonts::WriteShadow("StormForge Entertainment Group", 125, 20, SHADOW, 0.3f, 0.3f, 0.7f);
			DXFonts::End();
			DXFonts::Begin(3);

			DXFonts::WriteShadow("Thanks to all our friends and families!", 50, 300);

			// Headings
				DXFonts::WriteShadow("Special Thanks",  Direct3D::GetScreenWidth() - 208, 56, SHADOW);
           		DXFonts::WriteShadow("Next: Back to Main ",  Direct3D::GetScreenWidth() - 270, Direct3D::GetScreenHeight() - 70, SHADOW);
				DXFonts::WriteShadow("(Press Enter)", Direct3D::GetScreenWidth() - 175, Direct3D::GetScreenHeight() - 35, SHADOW);

				
			DXFonts::End();

		Direct3D::End();
		Direct3D::Present();
	}

	

public:

	static void Welcome(void)
	{
		// We're already doing clear, etc... Draw just like Help.
		#define SCALEX 2.25f
		#define SCALEY 1.55f
		//#define Center(XSize) ((Direct3D::GetScreenWidth() - XSize) * 0.5f)
		int iX = int(Center(int(TextureMgr::GetBmpWidth(iHelpTexture) * SCALEX)));
		int iY = int(CenterY(int(TextureMgr::GetBmpHeight(iHelpTexture) * SCALEY)));
	
		 TextureMgr::Draw(iHelpTexture, float(iX), float(iY), NULL, SCALEX, SCALEY, 0, 192);
		 DXFonts::Begin(1);
		 DXFonts::WriteShadow("Welcome to Guardians of Neverwood!", iX + 30, iY + 20);
		 DXFonts::End();
		 DXFonts::Begin(0);
		 DXFonts::Write("As Tempest, your objective is simple - protect the forest Neverwood", iX + 30, iY + 50);
		 DXFonts::Write("from the orcs who would destroy it!", iX + 30, iY + 70);

		 DXFonts::Write("Tempest fights with her powerful bow, and has three types of arrows", iX + 30, iY + 120);
		 DXFonts::Write("at her disposal: normal, flame, and frost. Frost arrows deal no damage,", iX + 30, iY + 140);
		 DXFonts::Write("but stop the enemy in his tracks, whereas flame arrows can explode, ", iX + 30, iY + 160);
		 DXFonts::Write("damaging all of the enemies in the immediate area.", iX + 30, iY + 180);

		 DXFonts::Write("To take advantage of these powers, you can hold your arrow shots for", iX + 30, iY + 230);
		 DXFonts::Write("up to three seconds to build power. Tempest's charge power is indicated", iX + 30, iY + 250);
		 DXFonts::Write("on the display below as a rising blue bar. Her health is represented by the", iX + 30, iY + 270);
		 DXFonts::Write("orange bar.", iX + 30, iY + 290);

		 DXFonts::Write("For more help with controls and objectives, press F1 during play.", iX + 30, iY + 340);
		 DXFonts::Write("Gather your courage, and press Enter to play Guardians of Neverwood!", iX + 30, iY + 360);
		 DXFonts::End();
	}

	  static void FreeIntroTextures(void)
	  {
		  for (int i = 0; i < 5; i++)
			  TextureMgr::UnloadTexture(iSlides[i]); 

		  fMovieTime = 0.0f;
		  fMovieStart = 0.0f;
	  }

public:
	 static void StartIntroMovie(void)
	 {
		// Set state
		iWhere = MENU_INTROMOVIE;
		// Set times
		fMovieStart = Timer::AppTime();
		fMovieTime = 0;
		// Call the first frame of the movie
		IntroMovie();
	 }

 	 static void StartDeathMovie(void)
	 {
		// Set state
		iWhere = MENU_DEATHMOVIE;
		// Set times
		fMovieStart = Timer::AppTime();
		fMovieTime = 0;
		bInMenu = true;

		LoadFonts();

		// Call the first frame of the movie
		DeathMovie();
	 }

  	 static void StartWinMovie(void)
	 {
		// Set state
		iWhere = MENU_WINMOVIE;
		// Set times
		fMovieStart = Timer::AppTime();
		fMovieTime = 0;
		bInMenu = true;
		
		LoadFonts();

		// Call the first frame of the movie
		WinMovie();
	 }


#define PicWindowX int((Direct3D::GetScreenWidth() * 0.6f) + 10)
#define PicWindowY int((Direct3D::GetScreenHeight() * 0.6f) + 10)

	 static void DeathMovie(void)
	 {
		 // Start D3D scene
		 Direct3D::Clear();
		 Direct3D::Begin();

		 // First screen...
		 if (fMovieTime < 7.0f)
			DeathSlide1(255);
         
		 // Transition time...
		 else if (fMovieTime >= 7.0f && fMovieTime <= 8.0f)
		 {
			DeathSlide1(255 * (1.0f - (fMovieTime - 7.0f)));
			DeathSlide2(255 * (fMovieTime - 7.0f));
		 }

 		 // Second screen...
		 else if (fMovieTime > 8.0f && fMovieTime < 16.0f)
			DeathSlide2(255);

		 // Fade to black
		 else if (fMovieTime >= 16.0f && fMovieTime < 17.0f)
		 {
			 	DeathSlide2(255 * (1.0f - (fMovieTime - 16.0f)));
		 }

		 // Finish the movie
		 else if (fMovieTime >= 21.0f)
		 {
			 iWhere = MENU_MAIN;
			 Game::iGameState = GAME_NOTSTARTED;
		 }

		 // End D3D Scene
		 Direct3D::End();
		 Direct3D::Present();

		 // Get the time now
		 fMovieTime = Timer::AppTime() - fMovieStart;		
	 }

 	 static void WinMovie(void)
	 {
		 // Start D3D scene
		 Direct3D::Clear();
		 Direct3D::Begin();

		 // First screen...
		 if (fMovieTime < 7.0f)
			WinSlide1(255);
         
		 // Transition time...
		 else if (fMovieTime >= 7.0f && fMovieTime <= 8.0f)
		 {
			WinSlide1(255 * (1.0f - (fMovieTime - 7.0f)));
			WinSlide2(255 * (fMovieTime - 7.0f));
		 }

 		 // Second screen...
		 else if (fMovieTime > 8.0f && fMovieTime < 16.0f)
			WinSlide2(255);

		 // Fade to black
		 else if (fMovieTime >= 16.0f && fMovieTime < 17.0f)
		 {
			 	WinSlide2(255 * (1.0f - (fMovieTime - 16.0f)));
		 }

		 // Finish the movie
		 else if (fMovieTime >= 21.0f)
		 {
			 iWhere = MENU_MAIN;
		 }

		 // End D3D Scene
		 Direct3D::End();
		 Direct3D::Present();

		 // Get the time now
		 fMovieTime = Timer::AppTime() - fMovieStart;		
	 }

	 static void DeathSlide1(float fAlpha)
	 {
		float fScaleX = PicWindowX / TextureMgr::GetBmpWidth(iDeath1);
		float fScaleY = PicWindowY / TextureMgr::GetBmpHeight(iDeath1);

		if (FloatEqual(fAlpha, 255.0f))
		{
		DXFonts::Begin(1);
		DXFonts::Write("Tempest Silvreleaf fought bravely, but was overwhelmed by the forces of Orkdusk.", 20, PicWindowY + 30, 1,1,1,1.0f);
		DXFonts::Write("Its best and last guardian slain, the forest Neverwood quickly followed suit.", 20, PicWindowY + 80, 1,1,1,1.0f);
		DXFonts::Write("The orcs mowed down countless trees and stripped Kaelah's Point bare of iron ore.", 20, PicWindowY + 130, 1,1,1,1.0f);
		DXFonts::End();
		}
		TextureMgr::Draw(iDeath1, 10, 10, NULL, fScaleX, fScaleY, NULL, int(fAlpha));
	 }

	 static void DeathSlide2(float fAlpha)
	 {
		float fScaleX = PicWindowX / TextureMgr::GetBmpWidth(iDeath2);
		float fScaleY = PicWindowY / TextureMgr::GetBmpHeight(iDeath2);

		if (FloatEqual(fAlpha, 255.0f))
		{
		
		DXFonts::Begin(1);
		DXFonts::Write("Several weeks later, the Black Horde did, in fact, leave the forest Neverwood.", 20, PicWindowY + 30, 1,1,1,1);
		DXFonts::Write("Armed with vicious new siege weaponry constructed from the bounty of Neverwood's boughs,", 20, PicWindowY + 80, 1,1,1,1);
		DXFonts::Write("they quickly advanced upon and destroyed the nearby town of Ahnala.", 20, PicWindowY + 130, 1,1,1,1);
		DXFonts::Write("Unchecked, they press forward still, crushing more peaceful places under their gangly feet.", 20, PicWindowY + 180, 1,1,1,1);
		DXFonts::End();
		}

		TextureMgr::Draw(iDeath2, 300, 10, NULL, fScaleX, fScaleY, NULL, int(fAlpha));
	 }


	 	 static void WinSlide1(float fAlpha)
	 {
		float fScaleX = PicWindowX / TextureMgr::GetBmpWidth(iWin1);
		float fScaleY = PicWindowY / TextureMgr::GetBmpHeight(iWin1);

		if (FloatEqual(fAlpha, 255.0f))
		{
		DXFonts::Begin(1);
		DXFonts::Write("Tempest emerged from the stockade, exhausted but relieved. The orcs of the ", 20, PicWindowY + 30, 1,1,1,1.0f);
		DXFonts::Write("Black Horde would be committed to the ravens, and the forest would survive in peace.", 20, PicWindowY + 80, 1,1,1,1.0f);
		DXFonts::Write("Staring out at the patch of tree stumps, Tempest vowed she would be ready if Neverwood ", 20, PicWindowY + 130, 1,1,1,1.0f);
		DXFonts::Write("was ever menaced again by such a force.", 20, PicWindowY + 180, 1,1,1,1.0f);
		DXFonts::End();
		}
		TextureMgr::Draw(iWin1, 10, 10, NULL, fScaleX, fScaleY, NULL, int(fAlpha));
	 }

	 static void WinSlide2(float fAlpha)
	 {
		float fScaleX = PicWindowX / TextureMgr::GetBmpWidth(iWin2);
		float fScaleY = PicWindowY / TextureMgr::GetBmpHeight(iWin2);

		if (FloatEqual(fAlpha, 255.0f))
		{
		
		DXFonts::Begin(1);
		DXFonts::Write("Though she seriously considered traveling to the heart of Orkdursk to finish off the Horde", 20, PicWindowY + 30, 1,1,1,1);
		DXFonts::Write("and its leader, General Hr'arak, Tempest decided to remain, preferring the quiet breeze", 20, PicWindowY + 80, 1,1,1,1);
		DXFonts::Write("of tranquility to the howling winds of war. One thing was certain, however. No more armies", 20, PicWindowY + 130, 1,1,1,1);
		DXFonts::Write("would tread lightly in her forest. Tempest was now, in her mind, the Guardian of Neverwood.", 20, PicWindowY + 180, 1,1,1,1);
		DXFonts::End();
		}

		TextureMgr::Draw(iWin2, 300, 10, NULL, fScaleX, fScaleY, NULL, int(fAlpha));
	 }





	 static void IntroSlide1(float fAlpha)
	 {
		float fScaleX = PicWindowX / TextureMgr::GetBmpWidth(iSlides[0]);
		float fScaleY = PicWindowY / TextureMgr::GetBmpHeight(iSlides[0]);

		if (FloatEqual(fAlpha, 255.0f))
		{
		
		DXFonts::Begin(1);
		DXFonts::Write("Six years ago, the village of Ruralia was razed by the Black Horde of Orkdursk.", 20, PicWindowY + 30, 1,1,1,1.0f);
		DXFonts::Write("The orcs of the horde burned every building, sparing no structure and no life.", 20, PicWindowY + 80, 1,1,1,1.0f);
		DXFonts::Write("Only one citizen of Ruralia remained, an eleven-year-old girl wandering in the woods.", 20, PicWindowY + 130, 1,1,1,1.0f);
		DXFonts::End();

		}

		TextureMgr::Draw(iSlides[0], 10, 10, NULL, fScaleX, fScaleY, NULL, int(fAlpha));
	 }

	 static void IntroSlide2(float fAlpha)
	 {
		float fScaleX = PicWindowX / TextureMgr::GetBmpWidth(iSlides[1]);
		float fScaleY = PicWindowY / TextureMgr::GetBmpHeight(iSlides[1]);

		if (FloatEqual(fAlpha, 255.0f))
		{
		
		DXFonts::Begin(1);
		DXFonts::Write("The girl returned to find her home destroyed and her parents murdered.", 20, PicWindowY + 30, 1,1,1,1);
		DXFonts::Write("Having no bastion of civilization to run to, Tempest took to the Forest Neverwood.", 20, PicWindowY + 80, 1,1,1,1);
		DXFonts::Write("There, she learned of the plants and animals of the forest, coexisting with them in peace.", 20, PicWindowY + 130, 1,1,1,1);
		DXFonts::Write("Her dearest friend was a perigrine falcon she named Aurexis that lived with her freely.", 20, PicWindowY + 180, 1,1,1,1);
		DXFonts::End();
		}

		TextureMgr::Draw(iSlides[1], 300, 10, NULL, fScaleX, fScaleY, NULL, int(fAlpha));

	 }
	 static void IntroSlide3(float fAlpha)
	 {
		float fScaleX = PicWindowX / TextureMgr::GetBmpWidth(iSlides[2]);
		float fScaleY = PicWindowY / TextureMgr::GetBmpHeight(iSlides[2]);

		if (FloatEqual(fAlpha, 255.0f))
		{
		DXFonts::Begin(1); 
		DXFonts::Write("Yet, strange tidings are abound in Neverwood these days.", 20, PicWindowY + 30, 1,1,1,1);
		DXFonts::Write("A host of orcs, possibly from the Black Horde, has entered the forest under cover of night.", 20, PicWindowY + 80, 1,1,1,1);
		DXFonts::Write("They have begun clear-cutting the forest, using the wood to construct weapons of war.", 20, PicWindowY + 130, 1,1,1,1);
		DXFonts::End();

		}
		TextureMgr::Draw(iSlides[2], 150, 10, NULL, fScaleX, fScaleY, NULL, int(fAlpha));


	 }

	 static void IntroSlide4(float fAlpha)
	 {
		float fScaleX = PicWindowX / TextureMgr::GetBmpWidth(iSlides[3]);
		float fScaleY = PicWindowY / TextureMgr::GetBmpHeight(iSlides[3]);

		if (FloatEqual(fAlpha, 255.0f))
		{
		DXFonts::Begin(1);
		DXFonts::Write("Also of interest to the orcish brutes is the iron mine atop Kaelah's Point.", 20, PicWindowY + 30, 1,1,1,1);
		DXFonts::Write("This peak, at the crown of the mountain and the heart of Neverwood, was soon taken.", 20, PicWindowY + 80, 1,1,1,1.0f);
		DXFonts::Write("Day and night, the pick-axes fall, and more and more of the mountain is carted away.", 20, PicWindowY + 130, 1,1,1,1.0f);
		DXFonts::End();
		}
		TextureMgr::Draw(iSlides[3], 210, 10, NULL, fScaleX, fScaleY, NULL, int(fAlpha));

	 }

	 static void IntroSlide5(float fAlpha)
	 {
		float fScaleX = PicWindowX / TextureMgr::GetBmpWidth(iSlides[4]);
		float fScaleY = PicWindowY / TextureMgr::GetBmpHeight(iSlides[4]);

		if (FloatEqual(fAlpha, 255.0f))
		{
		DXFonts::Begin(1);
		DXFonts::Write("The orcs have constructed a crude stockade surrounding Kaelah's Point.", 20, PicWindowY + 30, 1,1,1,1);
		DXFonts::Write("There, they gather the resources they pry from the land, assembling their vicious weaponry.", 20, PicWindowY + 80, 1,1,1,1);
		DXFonts::Write("It is there that their leader, Kara'tok, stays. It is there that Tempest's destiny lies as well.", 20, PicWindowY + 130, 1,1,1,1);
		DXFonts::End();
		}

		TextureMgr::Draw(iSlides[4], 60, 10, NULL, fScaleX, fScaleY, NULL, int(fAlpha));


	 }


static void IntroMovie(void)
	 {			

		// Start D3D scene
		 Direct3D::Clear();
		 Direct3D::Begin();

		 // First screen...
		 if (fMovieTime < 10.0f)
			IntroSlide1(255);
           
		 // Transition time...
		 else if (fMovieTime >= 10.0f && fMovieTime <= 11.0f)
		 {
			IntroSlide1(255 * (1.0f - (fMovieTime - 10.0f)));
			IntroSlide2(255 * (fMovieTime - 10.0f));
		 }

 		 // Second screen...
		 else if (fMovieTime > 11.0f && fMovieTime < 20.0f)
			IntroSlide2(255);
           
		 // Transition time...
		 if (fMovieTime >= 20.0f && fMovieTime <= 21.0f)
		 {
			IntroSlide2(255 * (1.0f - (fMovieTime - 20.0f)));
			IntroSlide3(255 * (fMovieTime - 20.0f));
		 }
		 
		 // 3rd screen...
		 else if (fMovieTime > 21.0f && fMovieTime < 30.0f)
			IntroSlide3(255);
         
		 // Transition time...
		 if (fMovieTime >= 30.0f && fMovieTime <= 31.0f)
		 {
			IntroSlide3(255 * (1.0f - (fMovieTime - 30.0f)));
			IntroSlide4(255 * (fMovieTime - 30.0f));
		 }
		 
		 // 4th screen...
		 else if (fMovieTime > 31.0f && fMovieTime < 40.0f)
			IntroSlide4(255);
         
		 // Transition time...
		 if (fMovieTime >= 40.0f && fMovieTime <= 41.0f)
		 {
			IntroSlide4(255 * (1.0f - (fMovieTime - 40.0f)));
			IntroSlide5(255 * (fMovieTime - 40.0f));
		 }

		 // 5th screen...
		 else if (fMovieTime > 41.0f && fMovieTime < 50.0f)
			IntroSlide5(255);
         
		 // Fade to black
		 else if (fMovieTime >= 50.0f && fMovieTime < 51.0f)
		 {
			 	IntroSlide5(255 * (1.0f - (fMovieTime - 50.0f)));
		 }

		 // Finish the movie
		 else if (fMovieTime >= 51.0f)
		 {
			 FreeIntroTextures();
			 iWhere = MENU_MAIN;
			 NewGame();
		 }

		 // End D3D Scene
		 Direct3D::End();
		 Direct3D::Present();

		 // Get the time now
		 fMovieTime = Timer::AppTime() - fMovieStart;

		
	 }		// End IntroMovie()


	 // hELP SCREEN - SHOULDNT CLEAR THE BUFFER, ETC SINCE ITS DRAWN OVER TOP
	 static void HelpScreen(void)
	 {
		 float fX = Direct3D::GetScreenWidth() * 0.1f;
		 float fY = Direct3D::GetScreenHeight() * 0.1f;
	
		 TextureMgr::Draw(iHelpTexture, fX, fY, NULL, 1.8f, 1.8f, 0, 128);
		 DXFonts::Begin(0);
		 DXFonts::Write("Guardians of Neverwood Help", int(fX + 30), int(fY + 30));
		 DXFonts::Write("Level 1 Objectives:", int(fX + 40), int(fY + 55));
		 DXFonts::Write("1. Kill all the orcs in this area of the forest!", int(fX + 50), int(fY + 75));

		 DXFonts::Write("Keyboard Control:", int(fX + 40), int(fY + 125));
		 DXFonts::Write(" W", int(fX + 55), int(fY + 145));
		 DXFonts::Write("A   S                              Movement", int(fX + 50), int(fY + 160));
		 DXFonts::Write(" D", int(fX + 55), int(fY + 175));

		 DXFonts::Write("Fire arrow: Up Arrow (hold to charge)", int(fX + 50), int(fY + 195));
		 DXFonts::Write("Switch arrows: Down Arrow", int(fX + 50), int(fY + 210));
		 DXFonts::Write("Jump: SpaceBar", int(fX + 50), int(fY + 255));
		 DXFonts::Write("Move camera: Left and Right Arrow", int(fX + 50), int(fY + 240));
		 
		 DXFonts::Write("Joystick Control:", int(fX + 40), int(fY + 280));
		 DXFonts::Write("Movement: Left analog stick/D-Pad", int(fX + 50), int(fY + 295));
		 DXFonts::Write("Fire arrow: Button 0 (hold to charge)", int(fX + 50), int(fY + 310));
		 DXFonts::Write("Jump: Button 2", int(fX + 50), int(fY + 325));
		 DXFonts::Write("Switch arrows: Button 3", int(fX + 50), int(fY + 340));
		 DXFonts::Write("Move camera: Right analog stick", int(fX + 50), int (fY + 355));

		 DXFonts::Write("Screenshot: PrintScreen", int(fX + 50), int(fY + 390));
		 DXFonts::Write("Return to Menu: Escape", int(fX + 50), int(fY + 405));
		 
		 DXFonts::Write("Press Escape to Return to Game", int(fX + 50), int(fY + 440));
	 
		 DXFonts::End();
	 }

#define LEVEL1_NAME "The Depths of Neverwood"
#define LEVEL2_NAME "The Field of the Fallen"
#define LEVEL3_NAME "The Heart of the Horde"
#define LEVEL1_TIME 900
#define LEVEL2_TIME 900
#define LEVEL3_TIME 1200


	 static void LevelSummary(int iLevel)
	 {
		// Get everything we need
		 int iDamaged = Game::pPlayer1->iDamageTaken;		/* dmg taken */
		 float fTime = Timer::AppTime() - Game::fLevelStartTime;
		 float fHP = float(Game::pPlayer1->iHitPoints);
		 float fMaxHP = float(Game::pPlayer1->iMaxHitPoints);
		 float fKills = float(Game::iTotalEnemies - Game::iEnemies);
		 float fTotal = float(Game::iTotalEnemies);

		 float fGoodTime = 0.0f;
		 char strName[60];

					if	(iLevel == 1) { strcpy(strName, LEVEL1_NAME); fGoodTime = LEVEL1_TIME; }
		 else		if	(iLevel == 2) { strcpy(strName, LEVEL2_NAME); fGoodTime = LEVEL2_TIME; }
		 else		if	(iLevel == 3) { strcpy(strName, LEVEL3_NAME); fGoodTime = LEVEL3_TIME; }
		 else		return;
		 
		 // Calculate time score. 
		 // If we beat the "great!" time, just round it off to us having no time penalty.
		 // Allows us to have a "best possible" score, which will always be 2.0
		 float fTimeScore = fTime / fGoodTime;
 		 if (fTimeScore <= 1.0f) fTimeScore = 0.0f;

		 // Calculate your "Score"
		 float fScore = ((fHP / fMaxHP) + (fKills / fTotal) - fTimeScore) / 2.0f;

		 char strGrade[10];

		 if (FloatEqual(fScore, 1.0f) && 1 /*treasures */) strcpy(strGrade, "Perfect!");
		 else if (fScore > 0.9f) strcpy(strGrade, "A");
		 else if (fScore > 0.8f) strcpy(strGrade, "B");
		 else if (fScore > 0.7f) strcpy(strGrade, "C");
		 else if (fScore > 0.6f) strcpy(strGrade, "D");
		 else strcpy(strGrade, "F");


	 }

	 	 static void Exposition(void)
	 {
		float fTotalMovieTime=4;
		float fSlideDurration=fTotalMovieTime*0.5f;

		 if(bIsPlaying&&Timer::AppTime()>fMovieEnd)
		 {
			 bIsPlaying=false;
			 Game::iGameState=GAME_PLAYING;
			 return;
		 }

		if(!bIsPlaying&&Timer::AppTime()>fMovieEnd)
		{
			bIsPlaying=true;
			fMovieEnd=Timer::AppTime()+fTotalMovieTime;
		}

		switch(Game::iLevel)
		{
		case 2:
			L2S1(255);
			break;
		case 3:
			L3S1(255);
			break;
		default:
			ErrorSlide();
			break;
		}

		//if(Timer::AppTime()>=fMovieEnd-fTotalMovieTime&&Timer::AppTime()<fMovieEnd-fSlideDurration)
		//{
		//	switch(Game::iLevel)
		//	{
		//	case 2:
		//		L2S1(255);
		//		break;
		//	case 3:
		//		L3S1(255);
		//		break;
		//	default:
		//		ErrorSlide();
		//		break;
		//	}
		//}
		//else
		//{
		//	switch(Game::iLevel)
		//	{
		//	case 2:
		//		L2S2(255);
		//		break;
		//	case 3:
		//		L3S2(255);
		//		break;
		//	default:
		//		ErrorSlide();
		//		break;
		//	}
		//}

	 }

	 static void L2S1(float fAlpha)
	 {
		float fScaleX = PicWindowX / TextureMgr::GetBmpWidth(iL2S1);
		float fScaleY = PicWindowY / TextureMgr::GetBmpHeight(iL2S1);
		if (FloatEqual(fAlpha, 255.0f))
		{
		DXFonts::Begin(1);
		DXFonts::Write("After following the trail of Orc filth through the caverns, Tempest emerges to the", 20, PicWindowY + 30, 1,1,1,1);
		DXFonts::Write("surface only to personally witness the horrors wrought upon her forest.  It's now", 20, PicWindowY + 80, 1,1,1,1);
		DXFonts::Write("painfully clear that only one thing can clense this defilement: Orc blood.", 20, PicWindowY + 130, 1,1,1,1);
		DXFonts::End();
		}
		TextureMgr::Draw(iL2S1, 300, 10, NULL, fScaleX, fScaleY, NULL, int(fAlpha));
	 }

	 static void L2S2(float fAlpha)
	 {
		float fScaleX = PicWindowX / TextureMgr::GetBmpWidth(iL2S2);
		float fScaleY = PicWindowY / TextureMgr::GetBmpHeight(iL2S2);
		if (FloatEqual(fAlpha, 255.0f))
		{
		DXFonts::Begin(1);
		DXFonts::Write("L3S1.", 20, PicWindowY + 30, 1,1,1,1);
		DXFonts::Write("test,", 20, PicWindowY + 80, 1,1,1,1);
		DXFonts::Write("test.", 20, PicWindowY + 130, 1,1,1,1);
		DXFonts::Write("test.", 20, PicWindowY + 180, 1,1,1,1);
		DXFonts::End();
		}
		TextureMgr::Draw(iL2S2, 300, 10, NULL, fScaleX, fScaleY, NULL, int(fAlpha));
	 }

	 static void L3S1(float fAlpha)
	 {
		float fScaleX = PicWindowX / TextureMgr::GetBmpWidth(iL3S1);
		float fScaleY = PicWindowY / TextureMgr::GetBmpHeight(iL3S1);
		if (FloatEqual(fAlpha, 255.0f))
		{
		DXFonts::Begin(1);
		DXFonts::Write("The end is finally near.  Tempest has brought the fight to their front door.", 20, PicWindowY + 30, 1,1,1,1);
		DXFonts::Write("The next few minutes will determine the fate of Neverwood.", 20, PicWindowY + 80, 1,1,1,1);
		DXFonts::End();
		}
		TextureMgr::Draw(iL3S1, 300, 10, NULL, fScaleX, fScaleY, NULL, int(fAlpha));
	 }

	 static void L3S2(float fAlpha)
	 {
		float fScaleX = PicWindowX / TextureMgr::GetBmpWidth(iL3S2);
		float fScaleY = PicWindowY / TextureMgr::GetBmpHeight(iL3S2);
		if (FloatEqual(fAlpha, 255.0f))
		{
		DXFonts::Begin(1);
		DXFonts::Write("L3S2.", 20, PicWindowY + 30, 1,1,1,1);
		DXFonts::Write("test,", 20, PicWindowY + 80, 1,1,1,1);
		DXFonts::Write("test.", 20, PicWindowY + 130, 1,1,1,1);
		DXFonts::Write("test.", 20, PicWindowY + 180, 1,1,1,1);
		DXFonts::End();
		}
		TextureMgr::Draw(iL3S2, 300, 10, NULL, fScaleX, fScaleY, NULL, int(fAlpha));
	 }

	 static void ErrorSlide(void)
	 {
		DXFonts::Begin(1);
		DXFonts::Write("Error!", 20, 80, 1,1,1,1);
		DXFonts::End();
	 }

};

#endif