/////////////////////////////
// GameDebug.cpp
//
// This file acts as a scratchpad for temporary code / driver programs
// GameDebug.cpp will not get updated on the CVS repository, so each team member will have his own version of this file
/////////////////////////////

#include "Game.h"
#include "DirectInput2.h"
#include "Direct3D.h"
#include "Renderer.h"
#include "light.h"
#include "Timer.h"
#include "Shapes.h"
#include "D3DXFont.h"
#include "AllCharacters.h"

void Game::UpdateDebug(void)
{
	pPlayer1->ResetBoundingShapeColor();
	for (BaseNode* pNode = lEnemies.first; pNode; pNode = pNode->next)
		pNode->object->ResetBoundingShapeColor();
	for (BaseNode* pNode = lStaticObjects.first; pNode; pNode = pNode->next)
		pNode->object->ResetBoundingShapeColor();
	for (BaseNode* pNode = lArrows.first; pNode; pNode = pNode->next)
		pNode->object->ResetBoundingShapeColor();
}

void Game::RenderDebug(void)
{
	if (bShowFPS)
	{
		char buffer[100];
		sprintf(buffer, "FPS: %d      Timer Multiplier: %f       Enemies Left: %d", Timer::GetFPSCount(), Timer::GetMotionSpeed(), iEnemies);
		DXFonts::Begin();
		DXFonts::Write(buffer, 5, (Direct3D::GetScreenHeight() - 40));	
		DXFonts::End();
	}

	// debug text out
	if (bDebugText)
	{
		char strPlayerPos[64];
		char strNumEnemies[32];
		char strNumPartitions[32];
		sprintf (strPlayerPos, "Player1: x:%.2f y:%.2f z:%.2f", Game::pPlayer1->GetXPos(), Game::pPlayer1->GetYPos(), Game::pPlayer1->GetZPos());
		sprintf (strNumEnemies, "Enemies left: %d", iEnemies);
		sprintf (strNumPartitions, "World Partitions: %d", pWorld->GetNumPartitions());
		DXFonts::Begin();
		DXFonts::Write (strPlayerPos, 10, 50);
		DXFonts::Write (strNumEnemies, 10, 110);
		DXFonts::Write (strNumPartitions, 10, 140);
		DXFonts::End();
	}

	// wireframe debug info
	Direct3D::Wireframe (true);
	Direct3D::Culling (false);
	TextureMgr::Set (TextureMgr::iWhiteTexture);
	if (pPlayer1->GetRenderContext()->bDebugGeometry || bDebugGeometry)
	{
		pPlayer1->pBoundingShape->Draw();
		pPlayer1->GetViewFrustum()->Draw();
		pPlayer1->GetRTF()->Draw();
	}
	else if (pPlayer1->GetRenderContext()->bDrawAttackFrustum)
		pPlayer1->GetRTF()->Draw();

	BaseNode* pNode;
	for (pNode = lEnemies.first; pNode; pNode = pNode->next)
	{
		if (pNode->object->GetRenderContext()->bInCameraFrustum)
		{
			if (pNode->object->GetRenderContext()->bDebugGeometry || bDebugGeometry)
			{
				// bounding shape
				if (pNode->object->pBoundingShape)
					pNode->object->pBoundingShape->Draw();
		
				//frustums
				((Character*)pNode->object)->GetViewFrustum()->Draw();
				((Character*)pNode->object)->GetRTF()->Draw();
			}
			// attack frustum
			else if (pNode->object->GetRenderContext()->bDrawAttackFrustum)
				((Character*)pNode->object)->GetRTF()->Draw();
		}		
	}

	for (pNode = lStaticObjects.first; pNode; pNode = pNode->next)
	{
		if (pNode->object->GetRenderContext()->bInCameraFrustum)
		{
			if (pNode->object->GetRenderContext()->bDebugGeometry || bDebugGeometry)
			{
				// bounding shape
				if (pNode->object->pBoundingShape)
					pNode->object->pBoundingShape->Draw();
			}
		}		
	}

	for (pNode = lPickups.first; pNode; pNode = pNode->next)
	{
		if (pNode->object->GetRenderContext()->bInCameraFrustum)
		{
			if (pNode->object->GetRenderContext()->bDebugGeometry || bDebugGeometry)
			{
				// bounding shape
				if (pNode->object->pBoundingShape)
					pNode->object->pBoundingShape->Draw();
			}
		}		
	}

	for (pNode = lArrows.first; pNode; pNode = pNode->next)
	{
		if (pNode->object->GetRenderContext()->bInCameraFrustum)
		{
			if (pNode->object->GetRenderContext()->bDebugGeometry || bDebugGeometry)
				pNode->object->pBoundingShape->Draw();
		}		
	}

	for (pNode = lExplosions.first; pNode; pNode = pNode->next)
	{
		if (pNode->object->GetRenderContext()->bInCameraFrustum)
		{
			if (pNode->object->GetRenderContext()->bDebugGeometry || bDebugGeometry)
				pNode->object->pBoundingShape->Draw();
		}		
	}

	for (pNode = lEventTriggers.first; pNode; pNode = pNode->next)
	{
		if (pNode->object->GetRenderContext()->bInCameraFrustum)
		{
			if (pNode->object->GetRenderContext()->bDebugGeometry || bDebugGeometry)
				pNode->object->pBoundingShape->Draw();
		}		
	}
	Direct3D::Culling (true);
	Direct3D::Wireframe (false);
}

void Game::HandleUserInputDebug(void)
{
	
	/*

		// controls for player 1: Tempest
	//if (DirectInput::GetKeyboard()->CheckImmediateInput(DIK_LEFT))
	//	pPlayer1->RotateY (-4);
	//if (DirectInput::GetKeyboard()->CheckImmediateInput(DIK_RIGHT))
	//	pPlayer1->RotateY (4);
	//if (DirectInput::GetKeyboard()->CheckImmediateInput(DIK_UP))
	//	pPlayer1->MoveForward (.08f);
	//if (DirectInput::GetKeyboard()->CheckImmediateInput(DIK_DOWN))
	//	pPlayer1->MoveForward (-0.08f);

	if (GetAsyncKeyState ('X'))
		pPlayer1->RotateX (5);
	if (GetAsyncKeyState ('Y'))
		pPlayer1->RotateY (5);
	if (GetAsyncKeyState ('Z'))
		pPlayer1->RotateZ (5);

	// controls for player 2: Aurexis
	if (DirectInput::GetKeyboard()->CheckImmediateInput(DIK_LEFT))
		pPlayer2->RotateY (1);
	if (DirectInput::GetKeyboard()->CheckImmediateInput(DIK_RIGHT))
		pPlayer2->RotateY (-1);
	if (DirectInput::GetKeyboard()->CheckImmediateInput(DIK_UP))
		pPlayer2->MoveForward (.02f);
	if (DirectInput::GetKeyboard()->CheckImmediateInput(DIK_DOWN))
		pPlayer2->MoveForward (-0.02f);

	// F2 forces memory manager dump to file.
 	if (DirectInput::GetKeyboard()->CheckBufferedInput(DIK_F2))
		MemoryManager::DumpToFile();
		*/
}

void Game::InitializeDebug()
{	 
	/*
	// draw weapon ranges
	pPlayer1->GetRenderContext()->bDrawAttackFrustum = true;
	for (BaseNode* pNode = lEnemies.first; pNode; pNode = pNode->next)
		pNode->object->GetRenderContext()->bDrawAttackFrustum = true;
	*/
}

void Game::Debug()
{

	// draw colliding things in red
	BaseNode* pThing = lStaticObjects.first;
	while (pThing)
	{
		if (pPlayer1->CollidesWith (pThing->object))
		{
			pThing->object->pBoundingShape->SetColor (1,0,0);
			pPlayer1->pBoundingShape->SetColor (1,0,0);
		}
		pThing = pThing->next;
	}
	
	pThing = lEnemies.first;
	while (pThing)
	{
		if (pPlayer1->CollidesWith (pThing->object))
		{
			pThing->object->pBoundingShape->SetColor (1,0,0);
			pPlayer1->pBoundingShape->SetColor (1,0,0);
		}
		pThing = pThing->next;
	}
}

void Game::ShutdownDebug()
{

}