



#ifndef StormForge_Matt_LevelEditor
#define StormForge_Matt_LevelEditor

// This acts as a subordinate to the Game class, to supplement it when acting as a level editor.
#include "Vector.h"
#include "DirectInput2.h"
#include "AllCharacters.h"
#include "StaticObjects.h"
#include "Cylinder.h"
#include "Renderer.h"
#include "Game.h"

enum EditorState
{
	EDITOR_STATE_WAITING,
	EDITOR_STATE_ADDOBJECT,
	EDITOR_STATE_SELECTOBJECT,
	EDITOR_STATE_OBJECTSELECTED,

	EDITOR_STATE_NUMSTATES,
};

// Changed
// added a constant that was apparently missing
#define EDITOR_SELECT_NONE 0

/* Objects that we can add:
	BASETYPE_TREE
	BASETYPE_BIGROCK
	BASETYPE_BUSH

	BASETYPE_ORCSCOUT
	BASETYPE_ORCCUTTER
	BASETYPE_ORCLT
	BASETYPE_ORCBOSS
	BASETYPE_RAVEN
	BASETYPE_BUZZARD

*/

class LevelEditor
{

public:

	// What state is the level editor's input in now?
	static int iEditorState;
	static int iSelectState;

	// Current position of "Placement" cursor
	static Vector vCursorPos;

	// Currently selected object.
	static Base *pSelectedObject;
	static Base *pLastObjectPlaced;
	static int iPlaceObjectType;
	static Base *pCopyObject;
	static float *pfEdit;



	// Set up initial stuff
	LevelEditor(void)
	{
		iEditorState = EDITOR_STATE_WAITING;
		iPlaceObjectType =  BASETYPE_TREE;
		vCursorPos = Vector(0,0,0);
		pSelectedObject = NULL;
		pCopyObject = NULL;
		pLastObjectPlaced = NULL;
	}
	


	// Respond to people doing things.
	static void HandleInput(void)
	{
	

		// Handle movement of cursor once.
		if (iEditorState != EDITOR_STATE_WAITING)
		{
					//////////////////////////////////////////////////////////////////////////////////////////
					// MOVE THE CURSOR
					//////////////////////////////////////////////////////////////////////////////////////////
					if (DirectInput::GetKeyboard()->CheckImmediateInput(DIK_LEFT))
							vCursorPos += Vector(-0.05f, 0, 0);
					if (DirectInput::GetKeyboard()->CheckImmediateInput(DIK_RIGHT))
							vCursorPos += Vector(0.05f, 0, 0);
					if (DirectInput::GetKeyboard()->CheckImmediateInput(DIK_UP))
							vCursorPos += Vector(0.00f, 0, 0.05f);
					if (DirectInput::GetKeyboard()->CheckImmediateInput(DIK_DOWN))
							vCursorPos += Vector(0.00f, 0, -0.05f);
					if (DirectInput::GetKeyboard()->CheckImmediateInput(DIK_PAGEUP))
							vCursorPos += Vector(0.00f, 0.05f, 0.00f);
					if (DirectInput::GetKeyboard()->CheckImmediateInput(DIK_PAGEDOWN))
							vCursorPos += Vector(0.00f, -0.05f, -0.00f);

					///////////////////////////////////////////////////////////////////////
					// Set the cursor's position equal to player x's.
					if (DirectInput::GetKeyboard()->CheckImmediateInput(DIK_TILDE))
						vCursorPos = Vector(0,0,0);
					if (DirectInput::GetKeyboard()->CheckImmediateInput(DIK_1))
						vCursorPos = Game::pPlayer1->GetPos();
					if (DirectInput::GetKeyboard()->CheckImmediateInput(DIK_2))
						vCursorPos = Game::pPlayer2->GetPos();

					//////////////////////////////////////////////////////////////////////////////////////////
					// Allow to back out of states
					//////////////////////////////////////////////////////////////////////////////////////////
					if (DirectInput::GetKeyboard()->CheckImmediateInput(DIK_BACKSPACE))
					{
						iEditorState = EDITOR_STATE_WAITING;
						return;
					}
					
		}

		// If we're in the wait state, only take input to enter a state or quit
		else
		{
			if (DirectInput::GetKeyboard()->CheckBufferedInput(DIK_O))
						iEditorState = EDITOR_STATE_ADDOBJECT;

			else if (DirectInput::GetKeyboard()->CheckBufferedInput(DIK_S))
			{
						iEditorState = EDITOR_STATE_SELECTOBJECT;
			}

			else if (DirectInput::GetKeyboard()->CheckBufferedInput(DIK_END))
						Game::iGameState = GAME_PLAYING;

			return;
		}
	
		// Deal with states we are already in.
		switch (iEditorState)
		{
				////////////////////////////////////////////
				// State: Selecting an object in the world
				case EDITOR_STATE_SELECTOBJECT:
				{
					// Finalize selection
					if (DirectInput::GetKeyboard()->CheckBufferedInput(DIK_RETURN))
					{
						iEditorState = EDITOR_STATE_OBJECTSELECTED;
						return;
					}
				
					// This isnt technically input, but since we have nowhere else to put it...
					// Check if an object is selected right now.

					// Check players.
					if (Game::pPlayer1->pBoundingShape->CollidesWith(vCursorPos))
					{
						if (pSelectedObject) pSelectedObject->ResetBoundingShapeColor();
						pSelectedObject = Game::pPlayer1;
						pSelectedObject->pBoundingShape->SetColor(1.0f, 1.0f, 1.0f);
						return;
					}
					else if (Game::pPlayer2->pBoundingShape->CollidesWith(vCursorPos))
					{
						if (pSelectedObject) pSelectedObject->ResetBoundingShapeColor();
						pSelectedObject = Game::pPlayer2;
						pSelectedObject->pBoundingShape->SetColor(1.0f, 1.0f, 1.0f);
						return;
					}

					// Check statics
					BaseNode *pBNode = Game::lStaticObjects.first;
					while (pBNode)
					{
						if (pBNode->object->pBoundingShape->CollidesWith(vCursorPos))
						{
							if (pSelectedObject) pSelectedObject->ResetBoundingShapeColor();
							pSelectedObject = pBNode->object;
							pSelectedObject->pBoundingShape->SetColor(1.0f, 1.0f, 1.0f);
							return;
						}
						pBNode = pBNode->next;
					}
					// Check enemies
					pBNode = Game::lEnemies.first;
					while (pBNode)
					{
						
						if (pBNode->object->pBoundingShape->CollidesWith(vCursorPos))
						{
							if (pSelectedObject) pSelectedObject->ResetBoundingShapeColor();
							pSelectedObject = pBNode->object;
							pSelectedObject->pBoundingShape->SetColor(1.0f, 1.0f, 1.0f);
							return;
						}
						pBNode = pBNode->next;
					}

					// Nothing is selected
					if (pSelectedObject) pSelectedObject->ResetBoundingShapeColor();
					pSelectedObject = NULL;
					
				}
		

				////////////////////////////////////////////
				// State: Adding object to world
				case EDITOR_STATE_ADDOBJECT:
				{
					if (DirectInput::GetKeyboard()->CheckBufferedInput(DIK_SPACE))
					{
						if (iPlaceObjectType == BASETYPE_TREE) iPlaceObjectType = BASETYPE_BUSH;
						else if (iPlaceObjectType == BASETYPE_BUSH) iPlaceObjectType = BASETYPE_BIGROCK;
						else if (iPlaceObjectType == BASETYPE_BIGROCK) iPlaceObjectType = BASETYPE_ORCSCOUT;
						else if (iPlaceObjectType == BASETYPE_ORCSCOUT) iPlaceObjectType = BASETYPE_ORCCUTTER;
						else if (iPlaceObjectType == BASETYPE_ORCCUTTER) iPlaceObjectType = BASETYPE_ORCLT;
						else if (iPlaceObjectType == BASETYPE_ORCLT) iPlaceObjectType = BASETYPE_BUZZARD;
						else if (iPlaceObjectType == BASETYPE_BUZZARD) iPlaceObjectType = BASETYPE_ORCBOSS;
						else if (iPlaceObjectType == BASETYPE_ORCBOSS) iPlaceObjectType = BASETYPE_RAVEN;
						else if (iPlaceObjectType == BASETYPE_RAVEN) iPlaceObjectType = BASETYPE_TREE;
					}

					if (DirectInput::GetKeyboard()->CheckBufferedInput(DIK_RETURN))
					{
							// 1. Set the state back to Waiting.
							iEditorState = EDITOR_STATE_WAITING;

							// 2. Make a new one of the type.
							Base *pNewObj = NULL;
							switch (iPlaceObjectType)
							{

							case BASETYPE_TREE:
								pNewObj = new Tree;
								Game::lStaticObjects.Insert(pNewObj);
								break;
							
							case BASETYPE_BUSH:
								pNewObj = new Bush;	
								Game::lStaticObjects.Insert(pNewObj);
								break;

							case BASETYPE_BIGROCK:
								pNewObj = new BigRock;	
								Game::lStaticObjects.Insert(pNewObj);
								break;

							case BASETYPE_ORCSCOUT:
								pNewObj = new OrcScout;	
								Game::lEnemies.Insert(pNewObj);						
								break;
					
							case BASETYPE_ORCCUTTER:
								pNewObj = new OrcCutter;	
								Game::lEnemies.Insert(pNewObj);						
								break;

							case BASETYPE_ORCLT:
								pNewObj = new OrcLt;	
								Game::lEnemies.Insert(pNewObj);						
								break;

							case BASETYPE_BUZZARD:
								pNewObj = new Buzzard;	
								Game::lEnemies.Insert(pNewObj);						
								break;

							case BASETYPE_ORCBOSS:
								pNewObj = new OrcBoss;	
								Game::lEnemies.Insert(pNewObj);						
								break;

							case BASETYPE_RAVEN:
								pNewObj = new Raven;	
								Game::lEnemies.Insert(pNewObj);						
								break;

							} // END Object Switch

							pNewObj->MoveTo(vCursorPos);			
							pLastObjectPlaced = pNewObj;
					}

				}
	
		}	// END STATE SWITCH

	}

		static void Draw(void)
		{
				// Draw the cursor at its position.
				
				// Write the cursor's position to the screen.
				if (iEditorState != EDITOR_STATE_WAITING)
				{
					glPushMatrix();
					glLoadIdentity();
					glTranslatef(vCursorPos.fComp[0], vCursorPos.fComp[1], vCursorPos.fComp[2]);
					glColor3f(1.0f, 1.0f, 0.9f);
					QuadricManager::Sphere(0.01f, 12, 12);
					glPopMatrix();

				}

				// Write the editor's state to the screen
				glColor3f(0.2f, 1.0f, 0.2f);
				switch (iEditorState)
				{
				case EDITOR_STATE_WAITING:
						
						FontManager::Text(Game::iFontArial12B, 25, (RenderManager::GetScreenSize().y - 150), "Make a selection.");
					break;

				case EDITOR_STATE_ADDOBJECT:
					FontManager::Text(Game::iFontArial12B, 25, (RenderManager::GetScreenSize().y - 150), "Add Object Mode");
					glColor3f(0, 0, 1);
					if (iPlaceObjectType == BASETYPE_TREE) FontManager::Text(Game::iFontArial12B, 25, (RenderManager::GetScreenSize().y - 110), "Object to add: Tree");
					else if (iPlaceObjectType == BASETYPE_BUSH) FontManager::Text(Game::iFontArial12B, 25, (RenderManager::GetScreenSize().y - 110), "Object to add: Bush");
					else if (iPlaceObjectType == BASETYPE_BIGROCK) FontManager::Text(Game::iFontArial12B, 25, (RenderManager::GetScreenSize().y - 110), "Object to add: Boulder");
					else if (iPlaceObjectType == BASETYPE_ORCSCOUT) FontManager::Text(Game::iFontArial12B, 25, (RenderManager::GetScreenSize().y - 110), "Object to add: Orcish Scout");
					else if (iPlaceObjectType == BASETYPE_ORCCUTTER) FontManager::Text(Game::iFontArial12B, 25, (RenderManager::GetScreenSize().y - 110), "Object to add: Orcish Woodcutter");
					else if (iPlaceObjectType == BASETYPE_ORCLT) FontManager::Text(Game::iFontArial12B, 25, (RenderManager::GetScreenSize().y - 110), "Object to add: Orcish Lieutenant");
					else if (iPlaceObjectType == BASETYPE_BUZZARD) FontManager::Text(Game::iFontArial12B, 25, (RenderManager::GetScreenSize().y - 110), "Object to add: Buzzard");
					else if (iPlaceObjectType == BASETYPE_ORCBOSS) FontManager::Text(Game::iFontArial12B, 25, (RenderManager::GetScreenSize().y - 110), "Object to add: Orc Boss");
					else if (iPlaceObjectType == BASETYPE_RAVEN) FontManager::Text(Game::iFontArial12B, 25, (RenderManager::GetScreenSize().y - 110), "Object to add: Raven");	
					break;				
			}
		}
};

#endif
