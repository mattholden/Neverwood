
#include "Game.h"
//#include "LevelEditor.h"		/* cut? */
#include "DirectInput2.h"
#include "DXModel.h"
#include "Direct3D.h"
#include "D3DXFont.h"
#include "Arrow.h"
#include "falcon.h"
#include "Ranger.h"
#include "menu.h"
#include "Explosion.h"
#include "hud.h"
#include "pickup.h"
#include "hut.h"

// Static member variable initialization
int Game::iGameState = GAME_NOTSTARTED;
int Game::iGameMode = MODE_SINGLE_PLAYER;

int Game::iDifficulty = DIFFICULTY_NORMAL;
float Game::fDifficultyScale = DIFFICULTY_SCALE_NORMAL;

int Game::iLevel = 1;
int Game::iBlackTexture = INVALID;
World* Game::pWorld = NULL;
Character* Game::pPlayer1 = NULL;
BaseList Game::lEnemies = BaseList();
BaseList Game::lStaticObjects;
BaseList Game::lArrows;
BaseList Game::lExplosions;
BaseList Game::lEventTriggers;
Base* Game::pBridge = NULL;
int Game::iTotalEnemies = 1;
int Game::iEnemies = 1;
BaseList Game::lPickups = BaseList();
float Game::fFadeTime = 0.0f;
float Game::fStartTime = 0.0f;
float Game::fLevelStartTime = 0.0f;
DXModel Game::modFalcon;
int Game::iSIDBirdChirp1 = -1;
int Game::iSIDBirdChirp2 = -1;
float Game::fAmbientSoundTimer = 0.0f;
int Game::iSIDArrowPullBack=-1;
int Game::iSIDArrowShot1=-1;
int Game::iSIDArrowHit1=-1;  //debug sound indices
int Game::iSIDSwoosh=-1;
int Game::iSIDThud=-1;

bool Game::bShowFPS = 0;
bool Game::bDebugGeometry = 0;
bool Game::bDebugText = 0;

// Bring me an enemy to kill!
void Game::GrabEnemy(void)
{
	BaseNode *pNode = lEnemies.first;
	Character *pChar = (Character*)lEnemies.first->object;

	while (pNode)
	{
		pChar = (Character*)pNode->object;
		
		// If you're alive, come to me!
		if (pChar->GetHP() > 0)
		{
			pChar->MoveTo(pPlayer1->GetPos() + (pPlayer1->GetZ() * 5.0f));
			return;
		}
		
		else 
			pNode = pNode->next;
	}
}

// Start the game, already!
void Game::Initialize(int iGameDifficulty)
{
	// set up what we're going to see
	Direct3D::Fog(true, 60.0f, 150.0f, FloatColor(0.3f, 0.3f, 0.7f, 1.0f));

	iDifficulty = iGameDifficulty;
	fDifficultyScale = GetDifficultyScale();

	// load models
	Arrow::LoadArrowModel();
	for(int i=0; i< NUMBER_OF_ARROWS;++i)
	{
		Arrow* pNewArrow=new Arrow();
		lArrows.Insert(pNewArrow);
	}

	//load sounds here!!
	Game::iSIDArrowPullBack=SoundManager::LoadSound("Sounds\\Arrow Pull Back.wav");
	Game::iSIDArrowShot1=SoundManager::LoadSound("Sounds\\Arrow Shot 1.wav");
	Game::iSIDBirdChirp1=SoundManager::LoadSound("Sounds\\Distant Bird Chirp.wav");
	Game::iSIDBirdChirp2=SoundManager::LoadSound("Sounds\\Distant Bird 2.wav");
	Game::iSIDSwoosh = SoundManager::LoadSound ("Sounds\\Sword Swoosh 2.wav");
	Game::iSIDThud = SoundManager::LoadSound ("Sounds\\thud5.wav");

	if (DirectInput::GetJoystick(0))
		DirectInput::GetJoystick(0)->FeedbackLoadEffects("feedback\arrow.ffe");

	fFadeTime = 0.0f;
	fStartTime = 0.0f;

	iGameMode = MODE_SINGLE_PLAYER;

	//initialize player	
	pPlayer1 = new Ranger(CHARACTER_PLAYER1);
	pPlayer1->pBoundingShape->SetColor (1, .75f, .75f);

	for(int i=0; i< NUMBER_OF_EXPLOSIONS;++i)
	{
		Explosion* pNewExp=new Explosion();
		lExplosions.Insert(pNewExp);
	}

	Arrow::LoadArrowModel();
	// falcon model (for powerups)
	modFalcon.Load("models\\falcon.x");
	modFalcon.Scale(0.05f, 0.05f, 0.05f);

	HUD::Initialize();

	// make a new world
	pWorld = new World;
	iLevel = STARTING_LEVEL;

	InitializeDebug();
}

// Game loop...
bool Game::GameLoop(void)
{
	// Ambient sounds
	fAmbientSoundTimer += Timer::GetElapsedTime();
	if (fAmbientSoundTimer > 2.0f)
	{
		int iRand = rand() % 100;
		if (iRand < 8)
		{
			SoundManager::Play(iSIDBirdChirp1);
			fAmbientSoundTimer = -8.0f;
		}	
		else if (iRand >= 8 && iRand <= 10)
		{
			SoundManager::Play(iSIDBirdChirp2);
			fAmbientSoundTimer = -8.0f;
		}
		else
			fAmbientSoundTimer = 0.0f;
	}

	switch (iGameState)
	{

	case GAME_WELCOME:
	{
		HandleUserInput();
		Direct3D::Clear(55, 64, 102);
			Direct3D::Begin();
				Render();
				Menu::Welcome();
			Direct3D::End();
		Direct3D::Present();
	}
	break;

	case GAME_EXPOSITION:
		{
		HandleUserInput();
		Direct3D::Clear(0, 0, 0);
			Direct3D::Begin();
				Menu::Exposition();
			Direct3D::End();
		Direct3D::Present();
		}
		break;

	case GAME_LOSE:
		// We want to fade to black, but we didnt take alpha into consideration
		// throughout, so we'll cheat by fading IN a black box, instead of fading OUT.

		Direct3D::Clear(55, 64, 102);
		Direct3D::Begin();
		Render();
		
		// How long since we died?
		fFadeTime = Timer::AppTime() - fStartTime;

		// Fade for 1 second
		if (fFadeTime < 1.0f)
		{
			TextureMgr::Draw(iBlackTexture, 0, 0, NULL,	float(Direct3D::GetScreenWidth()), 
																					float(Direct3D::GetScreenHeight()),
																					0.0f, int(255.0f * fFadeTime));
		}

		// If we're at full black, show the movie
		else
		{
			Menu::StartDeathMovie();
			fFadeTime = 0.0f;
		}

		Direct3D::End();
		Direct3D::Present();
		
		break;

		case GAME_WIN:
		// We want to fade to black, but we didnt take alpha into consideration
		// throughout, so we'll cheat by fading IN a black box, instead of fading OUT.

		Direct3D::Clear(55, 64, 102);
		Direct3D::Begin();
		Render();
		
		// How long since we died?
		fFadeTime = Timer::AppTime() - fStartTime;

		// Fade for 1 second
		if (fFadeTime < 1.0f)
		{
			TextureMgr::Draw(iBlackTexture, 0, 0, NULL,	float(Direct3D::GetScreenWidth()), 
																					float(Direct3D::GetScreenHeight()),
																					0.0f, int(255.0f * fFadeTime));
		}

		// If we're at full black, show the movie
		else
		{
			Menu::StartWinMovie();
			fFadeTime = 0.0f;
		}

		Direct3D::End();
		Direct3D::Present();
		
		break;


	case GAME_QUITTING:
		return false;
		break;

	case GAME_PLAYING:
		Update();
		HandleUserInput();
		RunGameLogic();

		Direct3D::Clear(55, 64, 102);
			Direct3D::Begin();
				Render();
			Direct3D::End();
		Direct3D::Present();
		break;

	case GAME_TAKING_SCREENSHOT:
		{
			Direct3D::StartScreenshot();
				Direct3D::Clear(55, 64, 102);
					Direct3D::Begin();
						Render();
					Direct3D::End();
				Direct3D::Present();
			Direct3D::SaveScreenshot("Screenshots\\SSHOT");
			iGameState = GAME_PLAYING;
		}
		break;

	case GAME_HELP:
		{
			HandleUserInput();
			Direct3D::Clear(55, 64, 102);
				Direct3D::Begin();
					Render();
					Menu::HelpScreen();
				Direct3D::End();
			Direct3D::Present();
		}
		break;


	case GAME_PAUSE:
		HandleUserInput();
		Direct3D::Clear(55, 64, 102);
		Direct3D::Begin();
		Render();
		Direct3D::End();
		Direct3D::Present();
		break;
	case GAME_INTRO_MENU:
		break;
	}

	return true;
}

void Game::HandleUserInput()
{
	//cheat!
	if (DirectInput::GetKeyboard()->CheckImmediateInput(DIK_F5))
	{
		if (lEventTriggers.first)
		{
			pPlayer1->MoveTo (lEventTriggers.first->object->GetPos() + Vector (-10, 0, -10));
			pPlayer1->MoveBy ( 0, pPlayer1->fHeight - pWorld->GetAltitude (pPlayer1->GetPos()), 0);
			//pPlayer1->LookAt (lEventTriggers.first->object->GetPos());
		}
	}
	
	// State change controls
	if (DirectInput::GetKeyboard()->CheckBufferedInput(DIK_SYSRQ))
	{
		iGameState = GAME_TAKING_SCREENSHOT	;
		return; 
	}
	

	if (DirectInput::GetKeyboard()->CheckBufferedInput(DIK_END))
	{
		pPlayer1->bTakesDamage = !pPlayer1->bTakesDamage;
		return;
	}

	// Dead or done - menu takes care of this
	if (iGameState == GAME_LOSE || iGameState == GAME_WIN)
		return;
	
		
	// Exit welcome screen
	if (iGameState == GAME_WELCOME)
	{
		if (DirectInput::GetKeyboard()->CheckBufferedInput(DIK_ENTER))
		{
			iGameState = GAME_PLAYING;
			return;
		}

		if (DirectInput::GetJoystick(0) && iGameState)
		{
			if (DirectInput::GetJoystick(0)->CheckBufferedInput(JOYSTICK_BUTTON(0)))
			{
				iGameState = GAME_PLAYING;
				return;
			}
		}

		// Don't let them do anything else but get out of the help screen
		return;
	}

	// Exit Exposition screens
	if (iGameState == GAME_EXPOSITION)
	{
		if (DirectInput::GetKeyboard()->CheckBufferedInput(DIK_ENTER))
		{
			iGameState = GAME_PLAYING;
			Menu::bIsPlaying=false;
			return;
		}

		if (DirectInput::GetJoystick(0))
		{
			if (DirectInput::GetJoystick(0)->CheckBufferedInput(JOYSTICK_BUTTON(0)))
			{
				iGameState = GAME_PLAYING;
				Menu::bIsPlaying=false;
				return;
			}
		}

		// Don't let them do anything else but get out of the help screen
		return;
	}

	// Speed
	if (DirectInput::GetKeyboard()->CheckBufferedInput(DIK_PGUP))
		Timer::SetMotionSpeed(Timer::GetMotionSpeed() + 0.1f);
	if (DirectInput::GetJoystick(0))
		if (DirectInput::GetJoystick(0)->CheckBufferedInput(JOYSTICK_BUTTON(8)))
			Timer::SetMotionSpeed(Timer::GetMotionSpeed() - 0.1f);

	if (DirectInput::GetKeyboard()->CheckBufferedInput(DIK_PGDN))
		Timer::SetMotionSpeed(Timer::GetMotionSpeed() - 0.1f);
	if (DirectInput::GetJoystick(0))
		if (DirectInput::GetJoystick(0)->CheckBufferedInput(JOYSTICK_BUTTON(9)))
			Timer::SetMotionSpeed(Timer::GetMotionSpeed() + 0.1f);
	
	// State change controls
	if (DirectInput::GetKeyboard()->CheckBufferedInput(DIK_F4))
	{
		GrabEnemy();
	}

	if (DirectInput::GetKeyboard()->CheckBufferedInput(DIK_F1))
	{
		iGameState = GAME_HELP;
		return; 
	}
	
	// TOGGLE DEBUGGING INFO
	if (DirectInput::GetKeyboard()->CheckBufferedInput(DIK_F9))
	{
		bShowFPS = !bShowFPS;
	}
	if (DirectInput::GetKeyboard()->CheckBufferedInput(DIK_F10))
	{
		bDebugGeometry = !bDebugGeometry;
	}
	if (DirectInput::GetKeyboard()->CheckBufferedInput(DIK_F11))
	{
		bDebugText = !bDebugText;
	}

	/*
	if (DirectInput::GetKeyboard()->CheckBufferedInput(DIK_F10))
	{
		bRenderDebug = !bRenderDebug;
	}
	*/


	// TEMP: ESC escapes the game.
	if (DirectInput::GetKeyboard()->CheckBufferedInput(DIK_ESCAPE))
	{
		if (iGameState == GAME_HELP)
		{
			iGameState = GAME_PLAYING;
		}
		else
		{
				iGameState = GAME_INTRO_MENU;
				Menu::bInMenu = true;
				Menu::iWhere = MENU_MAIN;
		}
	}

	//////////////////////////////////
	// Handle debug
	//////////////////////////////////
	#if _DEBUG
		HandleUserInputDebug();
	#endif

}

void Game::Update()
{
	pPlayer1->Update();
	pWorld->pCamera->Update();
	
	// for the red sky, it looks good if the skybox moves along with the player
	if (pWorld->GetSkyBox() && iLevel != 1)
		pWorld->GetSkyBox()->MoveTo (pPlayer1->GetPos() + Vector (0,50,0));

	BaseNode* pNode = lEnemies.first;
	while (pNode)
	{
		pNode->object->Update();
		pNode = pNode->next;
	}

	pNode = lArrows.first;
	while (pNode)
	{
		pNode->object->Update();
		pNode = pNode->next;
	}

	pNode = lExplosions.first;
	while(pNode)
	{
		pNode->object->Update();
		pNode = pNode->next;
	}
	
	// OPTIMIZATION: make sure to only check every object against the camera once
	Camera* camera = pWorld->pCamera;
	Vector vCameraPos = camera->GetPos();
	pPlayer1->GetRenderContext()->bInCameraFrustum = camera->CollidesWith (pPlayer1);
	pPlayer1->GetRenderContext()->fDistSquared = (vCameraPos - pPlayer1->GetPos()).LengthSquared();
	for (pNode = lEnemies.first; pNode; pNode = pNode->next)
	{
		pNode->object->GetRenderContext()->bInCameraFrustum = camera->CollidesWith (pNode->object);
		pNode->object->GetRenderContext()->fDistSquared = (vCameraPos - pNode->object->GetPos()).LengthSquared();
	}
	for (pNode = lStaticObjects.first; pNode; pNode = pNode->next)
	{
		pNode->object->GetRenderContext()->bInCameraFrustum = camera->CollidesWith (pNode->object);
		pNode->object->GetRenderContext()->fDistSquared = (vCameraPos - pNode->object->GetPos()).LengthSquared();
	}
	for (pNode = lArrows.first; pNode; pNode = pNode->next)
	{
		pNode->object->GetRenderContext()->bInCameraFrustum = camera->CollidesWith (pNode->object);
		pNode->object->GetRenderContext()->fDistSquared = (vCameraPos - pNode->object->GetPos()).LengthSquared();
	}
	for (pNode = lExplosions.first; pNode; pNode = pNode->next)
	{
		pNode->object->GetRenderContext()->bInCameraFrustum = camera->CollidesWith (pNode->object);
		pNode->object->GetRenderContext()->fDistSquared = (vCameraPos - pNode->object->GetPos()).LengthSquared();
	}
	for (pNode = lEventTriggers.first; pNode; pNode = pNode->next)
	{
		pNode->object->GetRenderContext()->bInCameraFrustum = camera->CollidesWith (pNode->object);
		pNode->object->GetRenderContext()->fDistSquared = (vCameraPos - pNode->object->GetPos()).LengthSquared();
	}
	for (pNode = lPickups.first; pNode; pNode = pNode->next)
	{
		pNode->object->GetRenderContext()->bInCameraFrustum = camera->CollidesWith (pNode->object);
		pNode->object->GetRenderContext()->fDistSquared = (vCameraPos - pNode->object->GetPos()).LengthSquared();
	}


	// Handle debug
	#if _DEBUG
		UpdateDebug();
	#endif
}

void Game::RunGameLogic()
{

	
	// First things first... Are we dead??
	if (pPlayer1->GetHP() <= 0)
	{
		iGameState = GAME_LOSE;
		fStartTime = Timer::AppTime();
		return;
	}

	// Are all the baddies dead?
	if (iEnemies <= 0) 
	{
		iGameState = GAME_WIN;
		fStartTime = Timer::AppTime();
		return;
	}

	BaseNode* pEnemy, *pObject;

	// ENEMY AI	
	for (pEnemy = lEnemies.first; pEnemy; pEnemy = pEnemy->next)
		((Character*)pEnemy->object)->ApplyAI();	

	// just to be on the safe side, make sure nothing ever leaves the boundaries of the world
	pWorld->KeepInBounds (pPlayer1);
	for (pEnemy = lEnemies.first; pEnemy; pEnemy = pEnemy->next)
		pWorld->KeepInBounds (pEnemy->object);

	// ENTITY COLLISION

	// player triggering events
	BaseList lEvents = Game::lEventTriggers;
	for (pObject = lEventTriggers.first; pObject; pObject = pObject->next)
	{
		if (pPlayer1->CollidesWith (pObject->object)) break;
	}
	if (pObject) 
		((EventTrigger*)pObject->object)->OnActivate();


	// players push moving objects
	for (pEnemy = lEnemies.first; pEnemy; pEnemy = pEnemy->next)
	{	
		pPlayer1->CollisionPush (pEnemy->object, 0.5f);
	}

	// players get pickups
	for (pObject = lPickups.first; pObject; pObject = pObject->next)
	{	
		if (pObject->object->bActive)
		{
			if (pPlayer1->CollidesWith(pObject->object))
			{
				Pickup *pUp = (Pickup*)pObject->object;
				pUp->Get();
			}
		}
	}


	// players clamp to static objects
	for (pObject = lStaticObjects.first; pObject; pObject = pObject->next)
	{
		pPlayer1->CollisionClamp (pObject->object);
	}

	// player ground clamps to bridge
	if (pBridge)
	{
		pPlayer1->CollisionClamp (pBridge);
	}

	// enemy colliison
	for (pEnemy = lEnemies.first; pEnemy; pEnemy = pEnemy->next)
	{
		// optimizations
		if (!pEnemy->object->bCollidable)
			continue;
		if (!pEnemy->object->GetRenderContext()->bInCameraFrustum 
			&& pEnemy->object->GetRenderContext()->fDistSquared > 1000)
			continue;

		// enemy -> enemy
		for (BaseNode* pNextEnemy = pEnemy->next; pNextEnemy; pNextEnemy = pNextEnemy->next)
			pEnemy->object->CollisionPush (pNextEnemy->object);		

		// enemy -> object
		for (pObject = lStaticObjects.first; pObject; pObject = pObject->next)
			pEnemy->object->CollisionClamp (pObject->object);
	}


	//explosion collisions
	for (pObject = lExplosions.first; pObject; pObject = pObject->next)
	{
		if (!pObject->object->bActive)
			continue;

		if (pPlayer1->CollidesWith (pObject->object))
		{
			pPlayer1->OnHit (pObject->object);
		}

		for (pEnemy = lEnemies.first; pEnemy; pEnemy = pEnemy->next)
		{
			if(pEnemy->object->CollidesWith(pObject->object))
				((Character*)pEnemy->object)->OnHit(pObject->object);
		}

		// explosions can take down huts
		/*
		for (BaseNode* pStatic = lStaticObjects.first; pStatic; pStatic = pStatic->next)
		{
			if (pStatic->object->iObjectType == BASETYPE_HUT)
			{
				((Hut*)pStatic->object)->TakeDamage ( ((Explosion*)pObject->object)->GetDamage() );
			}
		}
		*/

	}

	//check arrow collisions
	for (pObject = lArrows.first; pObject; pObject = pObject->next)
	{
		if (!pObject->object->bActive)
			continue;

		for (pEnemy = lEnemies.first; pEnemy; pEnemy = pEnemy->next)
			if(pEnemy->object->CollidesWith(*((Arrow*)pObject->object)->GetBoundingLine()))
			{
				((Character*)pEnemy->object)->OnHit(pObject->object);
				((Arrow*)pObject->object)->Deactivate();
			}

		for (BaseNode* pNode = lStaticObjects.first; pNode; pNode = pNode->next)
		{
			if (pObject->object->CollidesWith (pNode->object))
			{
				((Arrow*)pObject->object)->Deactivate();
			}
		}

		if(pObject->object->CollidesWith(pPlayer1))
		{
			if( ((Arrow*)pObject->object)->iObjectType == BASETYPE_RANGER)
			{
				pPlayer1->OnHit(pObject->object);
				((Arrow*)pObject->object)->Deactivate();
			}
		}
	}



	// special case: in level 3, enemies respawn until you beat the boss
	/*
	if (iLevel == 3)
	{
		const int ENEMY_COUNT = 5;	//minimum number of enemies
		if (Game::iEnemies <= ENEMY_COUNT)
		{
			// find out if the boss is still alive
			bool bBossAlive = false;
			for (BaseNode* pEnemy = lEnemies.first; pEnemy; pEnemy = pEnemy->next)
			{
				if (pEnemy->object->iObjectType == BASETYPE_ORCBOSS)
					bBossAlive = true;
			}

			// if he is, spawn some random enemies
			if (bBossAlive)
			{
				int iRandomUnitType = RandomInt (BASETYPE_ORCSCOUT, BASETYPE_ORCLT);
					if (iRandomUnitType == BASETYPE_ORCSCOUT)
						pWorld->CreateRandomOrcScouts (1, -39, -1, 41, 1);
					if (iRandomUnitType == BASETYPE_ORCCUTTER)
						pWorld->CreateRandomOrcCutters (1, -39, -1, 41, 1);
					if (iRandomUnitType == BASETYPE_ORCBOSS)
						pWorld->CreateRandomOrcLts (1, -39, -1, 41, 1);
			}
		}
	}
	*/

	// Do debug version
	#if _DEBUG
		Debug();
	#endif

}

void Game::Render()
{
	pWorld->pCamera->ApplyTransform();

	// these will always be visible, so draw them no matter what
	Direct3D::Perspective (VIEWFRUSTUM_ANGLE, VIEWFRUSTUM_NEARCLIP, 1000, Direct3D::GetAspectRatio());
	pWorld->GetSkyBox()->Draw();
	Direct3D::Perspective (VIEWFRUSTUM_ANGLE, VIEWFRUSTUM_NEARCLIP, 1000, Direct3D::GetAspectRatio());

	pPlayer1->Draw();
	pWorld->Draw();
	

	// draw all visible entities
	BaseNode* pNode;
	for (pNode = lEnemies.first; pNode; pNode = pNode->next)
	{
		if (pNode->object->GetRenderContext()->bInCameraFrustum)
			pNode->object->Draw();
	}
	for (pNode = lStaticObjects.first; pNode; pNode = pNode->next)
	{
		if (pNode->object->GetRenderContext()->bInCameraFrustum)
			pNode->object->Draw();
	}
	for (pNode = lPickups.first; pNode; pNode = pNode->next)
	{
		if (pNode->object->GetRenderContext()->bInCameraFrustum)
			pNode->object->Draw();
	}
	for (pNode = lArrows.first; pNode; pNode = pNode->next)
	{
		if (pNode->object->GetRenderContext()->bInCameraFrustum)
			pNode->object->Draw();
	}
	for (pNode = lExplosions.first; pNode; pNode = pNode->next)
	{
		if (pNode->object->GetRenderContext()->bInCameraFrustum)
			pNode->object->Draw();
	}
	for (pNode = lEventTriggers.first; pNode; pNode = pNode->next)
	{
		if (pNode->object->GetRenderContext()->bInCameraFrustum)
			pNode->object->Draw();
	}

	// Draw 2D stuff
	HUD::Draw();
	
	if (pPlayer1->bTakesDamage == false)
	{
		DXFonts::Begin();
		DXFonts::WriteShadow("Goddess Mode", int(HUD::fX), int(HUD::fY));
		DXFonts::End();
	}

	// Do Debug version: 
#if _DEBUG
	RenderDebug();
#endif

}

void Game::OnClearLevel()
{
	// TO DO: cinematics stuff
	iLevel++;
	if (iLevel > 3)
		// TO DO: replace this with ending
		iLevel = 1;
	pWorld->LoadLevel (iLevel);
}

// Shutdown...
void Game::Shutdown(void)
{	
	// CHANGED
	// we have to delete models manually
	SAFE_DELETE (pPlayer1->pModel);
	SAFE_DELETE (pWorld->pModel);
	if (pBridge)
	{
		SAFE_DELETE (pBridge->pModel);
		SAFE_DELETE (pBridge);
	}

	BaseNode* pNode;
	for (pNode = lEnemies.first; pNode; pNode = pNode->next)
		SAFE_DELETE (pNode->object->pModel);
	for (pNode = lStaticObjects.first; pNode; pNode = pNode->next)
		SAFE_DELETE (pNode->object->pModel);
	Arrow::DeleteModel();

	// now that all models are gone, we may delete all objects
	SAFE_DELETE (pPlayer1);
	SAFE_DELETE (pWorld);
	lEnemies.DeleteListAndObjects();
	lStaticObjects.DeleteListAndObjects();
	lArrows.DeleteListAndObjects();
	lExplosions.DeleteListAndObjects();
	lPickups.DeleteListAndObjects();
	lEventTriggers.DeleteListAndObjects();
	
	/*
	for (int i=0; i<Tree::GetNumModels(); i++)
		Tree::DeleteModel (i);
	for (int i=0; i<Bush::GetNumModels(); i++)
		Bush::DeleteModel (i);
	*/

	ShutdownDebug();
}


