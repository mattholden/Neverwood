
// Defines the game class.

#ifndef StormForge_Shared_GameClass
#define StormForge_Shared_GameClass


enum DifficultyModes
{
	DIFFICULTY_VERYEASY,
	DIFFICULTY_EASY,
	DIFFICULTY_NORMAL,
	DIFFICULTY_HARD,
	DIFFICULTY_VERYHARD,
	DIFFICULTY_COUNT
};

#define DIFFICULTY_SCALE_VERYEASY 0.5f
#define DIFFICULTY_SCALE_EASY 0.75f
#define DIFFICULTY_SCALE_NORMAL 1.0f
#define DIFFICULTY_SCALE_HARD 1.5
#define DIFFICULTY_SCALE_VERYHARD 2.5f

#define DIFFICULTY_NAME_VERYEASY "Embarrassingly Easy"
#define DIFFICULTY_NAME_EASY "Easy"
#define DIFFICULTY_NAME_NORMAL "Normal"
#define DIFFICULTY_NAME_HARD "Hard"
#define DIFFICULTY_NAME_VERYHARD "Ridiculously Hard"


#define STARTING_LEVEL 1			//can change this for the sake of debugging
#define LEVEL1_NUMTREES 100
#define LEVEL1_NUMCUTTERS 0
#define LEVEL2_NUMTREES 30
#define LEVEL2_NUMBUSHES 40
#define LEVEL2_NUMSCOUTS 0
#define LEVEL2_NUMCUTTERS 0

#include "Renderer.h"
#include "SoundManager.h"
#include "MemoryMgr.h"
#include "World.h"
#include "Character.h"
//#include "Tree.h"
//#include "Stump.h"
//#include "Hut.h"
//#include "Bridge.h"
#include "EventTrigger.h"

//DEBUG
#include "Boundingshape.h"
#include "Sphere.h"
#include "Cylinder.h"
#include "VerticalCylinder.h"
#include "Frustum.h"
#include "DXModel.h"

enum eGameStates
{
	GAME_NOTSTARTED,
	GAME_WELCOME,
	GAME_INTRO_MENU,		//browsing the menu system before the game starts
	GAME_TAKING_SCREENSHOT,
	GAME_HELP,
	GAME_QUITTING,
	GAME_PLAYING,
	GAME_EXPOSITION,
	GAME_PAUSE,
	GAME_EDITOR,
	GAME_WIN,				//beat the game
	GAME_LOSE,				//game over
	GAME_DEBUG,
	NUMGAMESTATES
};

enum eGameModes
{
	MODE_SINGLE_PLAYER,
	MODE_COOP,
	NUMGAMEMODES,
};

#define NUMBER_OF_ARROWS 10
#define NUMBER_OF_EXPLOSIONS 3

class Game
{

private:

	// Note: Don't make a constructor/destructor! It will probably screw up since we're doing everything as a static
	// instead, make all member variables and functions static.
	// Initialize static member variables to something, anything, at top of game.cpp
	// Then, initialize them to real values if needed, in Game::Initialize().
	// Same with shutdown, put everything in Shutdown().

public:

	// Sounds
	static int iSIDBirdChirp1, iSIDBirdChirp2;
	static int iSIDSwoosh;
	static int iSIDThud;
	static float fAmbientSoundTimer;

	// Sounds
	static int iSIDArrowPullBack, iSIDArrowShot1, iSIDArrowHit1; //debug sound indices

	// for fades
	static float fFadeTime;
	static float fStartTime;
	static int iBlackTexture;
	static int iEnemies;
	static int iTotalEnemies;
	static float fLevelStartTime;

	static void GrabEnemy(void);

	// general game info
	static int iGameState;		//taken from the enum above
	static int iGameMode;		//single player, coop, deathmatch
	static int iDifficulty;
	static float fDifficultyScale;


	// level info
	static int iLevel;			//the current level number
	static World* pWorld;

	// the player
	static Character* pPlayer1;	
	static DXModel modFalcon;

	// list of each kind of entity
	static BaseList lEnemies;
	static BaseList lStaticObjects;	//trees, logs, ... world segments??
	static BaseList lArrows;	
	static BaseList lExplosions;
	static BaseList lEventTriggers;
	static BaseList lPickups;		//the list o' pickups
	static Base* pBridge;					//static geometry that the player must collide with on a per-polygon level
									//I'd feel silly making a baselist for an object that I only use once in the entire game, hence the Base*
	// Debug
	static bool bShowFPS;
	static bool bDebugGeometry;		//for bounding shapes, axes, frustums, etc
	static bool bDebugText;			//for player position, number of objects renderer, etc

	// Initializes the game Instance only. All other initialization should be done in the constructor.
	// Managers should be initialized in WinMain.
	static void Game::Initialize(int iGameDifficulty = DIFFICULTY_NORMAL);
	static void InitializeDebug(void);	//implemented in GameDebug.cpp

	// shut down the game. Don't shut down anything but Game members here; managers should be
	// shut down in WinMain.
	static void Shutdown(void);
	static void ShutdownDebug(void);	//implemented in GameDebug.cpp

	// Called each loop...
	static bool GameLoop(void);
	


	// Check for and respond to user input
	static void HandleUserInput();
	static void HandleUserInputDebug();

	// Update everything
	static void Update();
	static void UpdateDebug();
	

	// Game logic
	// this includes collision detection/response, physics, and AI!
	static void RunGameLogic();
	

	// Debug
	// this is a scratchpad for any temporary code
	// implemented in GameDebug
	static void Debug();

	// Render
	// Draws everything to the screen
	static void Render();
	static void RenderDebug();


	// Level Management
	static void OnClearLevel();



	static float GetDifficultyScale(int iScale = -1)
	{
		switch (iScale)
		{
		case -1: return GetDifficultyScale(iDifficulty);
		case DIFFICULTY_VERYEASY: return DIFFICULTY_SCALE_VERYEASY;
		case DIFFICULTY_EASY: return DIFFICULTY_SCALE_EASY;
		case DIFFICULTY_NORMAL: return DIFFICULTY_SCALE_NORMAL;
		case DIFFICULTY_HARD: return DIFFICULTY_SCALE_HARD;
		case DIFFICULTY_VERYHARD: return DIFFICULTY_SCALE_VERYHARD;
		}

		// Something is very screwed up.
		return 0.0f;
	}



	// Manage memory.
	#if _DEBUG
		ManageMemory(Game, CORETYPE_GAMECLASS);
	#endif	
};

#endif

