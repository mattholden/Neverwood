#include "Humanoid.h"
#include "Renderer.h"
#include "Game.h"
#include "InputDevice.h"
#include "ActionMap.h"
#include "DirectInput2.h"
#include "InputJoystick.h"
#include "Arrow.h"
//#include "Timer.h"

#define MAX_SPEED 10

#define DEBUG_DEADZONE 100

Humanoid::Humanoid(int iType, int iPlayerFlag) : Character(iType, iPlayerFlag) 
{
	bGroundClamped = true;

	SetAcceleration(100);
	SetFriction(35);
	SetJumpStrength(15);


	iSelectedArrow=Arrow::ARROW_TYPE_NORMAL;

	SetWHO(Vector(0, 0.5f, 1.0f));


	//initialize all elements in iActionArray to -1 (-1 is a bogus entry)
	for(int i=0; i< HUMANOID_NUM; ++i)
		iActionArray[i]=-1;

	if(cPlayer==CHARACTER_PLAYER1||cPlayer==CHARACTER_PLAYER2)
	{
		pActionMap->GetJoystick()->SetDeadZone(900);

		iActionArray[HUMANOID_JUMP]=pActionMap->AddAction("Jump", DIK_SPACE, -1, JOYSTICK_BUTTON(2));

		iActionArray[HUMANOID_SHOOT_ARROW]=pActionMap->AddAction("Shoot Arrow", DIK_UP, -1, JOYSTICK_BUTTON(0));

		iActionArray[HUMANOID_SWITCH_ARROW]=pActionMap->AddAction("Switch Arrow", DIK_DOWN, -1, JOYSTICK_BUTTON(3));

		iActionArray[HUMANOID_SWING_CAMERA_LEFT]=pActionMap->AddAction("Camera Left", DIK_LEFT, -1, JOYSTICK_BUTTON(4));

		iActionArray[HUMANOID_SWING_CAMERA_RIGHT]=pActionMap->AddAction("Camera Right", DIK_RIGHT, -1, JOYSTICK_BUTTON(5));

		//keyboard movement controls
		//iActionArray[HUMANOID_MOVE_FORWARD]=pActionMap->AddAction("Move Forward", DIK_W, NULL, NULL);
		//iActionArray[HUMANOID_MOVE_BACKWARD]=pActionMap->AddAction("Move Backward", DIK_S, NULL, NULL);
		//iActionArray[HUMANOID_MOVE_RIGHT]=pActionMap->AddAction("Move Right", DIK_D, NULL, NULL);
		//iActionArray[HUMANOID_MOVE_LEFT]=pActionMap->AddAction("Move Left", DIK_A, NULL, NULL);
	}
}

void Humanoid::HandleInput()
{

	// CHANGED
	// added this check so that AI controlled entities don't try to handle input
	// Rolf, 6-9-04
	if (cPlayer != CHARACTER_PLAYER1 && cPlayer != CHARACTER_PLAYER2)
		return;

	Vector vMoveDirection(0.0f, 0.0f, 0.0f);

	Vector vCamZ=Game::pWorld->pCamera->GetZ();
	Vector vCamX=Game::pWorld->pCamera->GetX();

	float fXRot=0, fYRot=0;
		
	//get analog stick input
	if (pActionMap->GetJoystick())
	{
		vMoveDirection.fComp[0]=(float)pActionMap->GetJoystick()->CheckImmediateInput(JOYSTICK_AXIS_LX);
		vMoveDirection.fComp[2]=(float)-pActionMap->GetJoystick()->CheckImmediateInput(JOYSTICK_AXIS_LY);

		//manual deadzoning
		if(vMoveDirection.fComp[0]<DEBUG_DEADZONE&&vMoveDirection.fComp[0]>-DEBUG_DEADZONE)
			vMoveDirection.fComp[0]=0;

		if(vMoveDirection.fComp[2]<DEBUG_DEADZONE&&vMoveDirection.fComp[2]>-DEBUG_DEADZONE)
			vMoveDirection.fComp[2]=0;

		//analog camera controls
		fXRot=(float)pActionMap->GetJoystick()->CheckImmediateInput(JOYSTICK_AXIS_RX)*0.001f*90;
		fYRot=(float)pActionMap->GetJoystick()->CheckImmediateInput(JOYSTICK_AXIS_RY)*0.001f*90;
	}

	//Check keyboard movement input
	if (DirectInput::GetKeyboard()->CheckImmediateInput(DIK_A))
		vMoveDirection.fComp[0]=-1000;
	if (DirectInput::GetKeyboard()->CheckImmediateInput(DIK_D))
		vMoveDirection.fComp[0]=1000;
	if (DirectInput::GetKeyboard()->CheckImmediateInput(DIK_W))
		vMoveDirection.fComp[2]=1000;
	if (DirectInput::GetKeyboard()->CheckImmediateInput(DIK_S))
		vMoveDirection.fComp[2]=-1000;

	//Handle character movement
	vMoveDirection*=0.007f;	
	vMoveDirection=vCamZ*vMoveDirection.fComp[2]+vCamX*vMoveDirection.fComp[0];
	MoveInDirection(&vMoveDirection);

	//digital camera control
	if(pActionMap->CheckAction(iActionArray[HUMANOID_SWING_CAMERA_LEFT], DEVICE_ALL, false))//if (DirectInput::GetKeyboard()->CheckImmediateInput(DIK_LEFT))
		fXRot=90;
	if(pActionMap->CheckAction(iActionArray[HUMANOID_SWING_CAMERA_RIGHT], DEVICE_ALL, false))////if (DirectInput::GetKeyboard()->CheckImmediateInput(DIK_RIGHT))
		fXRot=-90;

	//Handle camera movement
	Game::pWorld->pCamera->RotateY(fXRot*Timer::GetElapsedTime());
//	RenderManager::GetCamera(cPlayer)->RotateLocalX(fYRot*Timer::GetElapsedTime());

	//Handle jump
	if(bJump&&pActionMap->CheckAction(iActionArray[HUMANOID_JUMP], DEVICE_ALL, false))
		Jump();

	//Handle switching arrows
	if(pActionMap->CheckAction(iActionArray[HUMANOID_SWITCH_ARROW], DEVICE_ALL, true))
	{
		iSelectedArrow++;
		if(iSelectedArrow>=Arrow::ARROW_TYPE_NUM)
			iSelectedArrow=0;
	}

	//Handle arrow firing
	bIsCharging=false;
	if(bShootArrow&&pActionMap->CheckAction(iActionArray[HUMANOID_SHOOT_ARROW], DEVICE_ALL, false))
	{
		bIsCharging=true;
		fChargeTime+=Timer::GetElapsedTime();
	}
		
	if(!bIsCharging&&fChargeTime)
	{
		ShootArrow(fChargeTime);
		fChargeTime=0;
	}
}

void Humanoid::Update()
{
	// CHANGED
	// making the humanoid call the character's update so that knockback and AI get applied
	// Rolf, 6-13-04
	Character::Update();

	// CHANGED
	// optimization: entities that are out of sight and far away do nothing
	if (!RContext->bInCameraFrustum && RContext->fDistSquared > 1000)
		return;		

	float fDebugGroundHeight=0;
	Vector vDebugGroundNorm(0, 1, 0);

	HandleInput();

	Vector vDeccel(vInternal);  //(friction)

	//find friction
	if (!vDeccel.IsZeroVector())
		vDeccel=vDeccel/GetFriction()*Timer::GetElapsedTime();
	
	vDeccel.fComp[1]=0.0f;
	vDeccel *= - 1.0f;

	//add friction
	if(vInternal.LengthSquared()<vDeccel.LengthSquared())
		vInternal.SetToZero();
	else
		vInternal += vDeccel;

	//add gravity
	vExternal += Game::pWorld->vGravity*Timer::GetElapsedTime();

	vVelocity= vInternal + vExternal;

	MoveBy(vVelocity * Timer::GetElapsedTime());	

	RTF.mGlobal=mGlobal;

	// hack: ground clamp here to avoid jittery effect
		
	// version 1: geometry-level collision response
	// good for steep surfaces, bad for flat ones and high falling rates

	
	// version 2: ground clamp
	// fast and good for smooth terrain, but goes through walls
	if (Game::pWorld->GroundClamp (this))
	{
		vExternal.fComp[1] = 0;
		bJump = 1;
	}
	else bJump = 0;

	/*
	// version 1: geometry level collision response
	Sphere middleOfObject (GetPos(), fWidth);
	if (Game::pWorld->CollisionClamp (Sphere (GetPos(), fWidth)))
	{
		MoveTo (middleOfObject.mGlobal.GetPos());
		vExternal.fComp[1] = 0;
		bJump = 1;
	}
	else bJump = 0;
	*/

	/*	
	// version 3: hybrid
	// tries to be the best of 1 and 2 combined, but is expensive
	Vector vGroundPoint(0,0,0);		//for ground clamping
	Triangle tGround;			//for ground clamping
	Vector vIntersectPoint (0,0,0);		//for geometry-level collision
	Triangle tIntersect;	//for geometry-level collision
	Line lGoingDown (GetPos() + Vector (0, 0.4f * fHeight, 0), GetPos() - Vector (0, 0.6f * fHeight, 0));
	bool bGroundColl, bGeometryColl;
	// use the ground clamping version of collision detection, since it is more robust
	if ( (bGroundColl = Game::pWorld->CollidesWithGeometry (lGoingDown, &vGroundPoint, &tGround)) || (bGeometryColl = Game::pWorld->CollidesWithGeometry (this, &vIntersectPoint, &tIntersect)))
	{
		// use geometry level collision response for steep terrain
		if (bGeometryColl && tIntersect.vNormal.fComp[1] < 0.5f)
		{
			pBoundingShape->CollisionClamp (vIntersectPoint);
			MoveTo (pBoundingShape->GetPos());
		}
		// use ground clamping collision response for flat terrain
		else
		{
			MoveTo (vGroundPoint + Vector (0, fHeight * 0.5f, 0));
			vExternal.fComp[1] = 0;
			bJump = 1;
		}
	}
	else bJump = 0;
	*/
}

void Humanoid::MoveInDirection(Vector *vMove)
{
	SetMaxSpeed(vMove->Length());

	if(*vMove!=Vector(0.0f, 0.0f, 0.0f))
	{
		if(vInternal.LengthSquared()<GetMaxSpeed()*GetMaxSpeed())
		{
			Vector vForce= *vMove/(GetAcceleration()*Timer::GetElapsedTime());
			vInternal+=vForce;
		}
	}

	if(!vMove->IsZeroVector())
		FaceDirection(*vMove);
}

void Humanoid::Jump(void)
{
	vExternal.fComp[1]+=GetJumpStrength();
	bJump=false;
}

//void Humanoid::ShootArrow(float fChargeTime)
//{
//	// check if it's time
//	if (fLastAttack < fAttackDuration)
//		return;
//	fLastAttack = 0;
//
//	Arrow*pArrow=NULL;
//
//	SoundManager::Play(Game::iSIDArrowShot1);
//
//	BaseNode* pSearch=Game::lArrows.first;
//	while(pSearch)
//	{
//		if(!pArrow)
//			pArrow=((Arrow*)pSearch->object);
//
//		if(!pSearch->object->bActive)
//		{
//			pArrow=((Arrow*)pSearch->object);
//			break;
//		}
//
//		if(((Arrow*)pSearch->object)->GetTimeStamp()<pArrow->GetTimeStamp())
//			pArrow=((Arrow*)pSearch->object);
//
//		pSearch=pSearch->next;
//	}
//
//	pArrow->Activate();
//	pArrow->SetOwner (this);
//
//	pArrow->mGlobal=mGlobal;
//	pArrow->mGlobal.TranslateLocal(GetWHO());
//	pArrow->vLastPos=pArrow->GetPos();
//	pArrow->GetBoundingLine()->vBegin=pArrow->GetPos();
//	pArrow->GetBoundingLine()->vEnd=pArrow->GetPos();
//
//	FindTarget();
//
//	if(pTarget)
//		pArrow->SetDirection(pTarget->GetPos()-pArrow->GetPos());
//	else
//		pArrow->SetDirection(pArrow->GetZ());
//
//	pArrow->SetArrowAttributes(iSelectedArrow, fChargeTime);
//}
//
//void Humanoid::FindTarget(void)
//{
//	BaseNode* pEnemy = Game::lEnemies.first;
//	pTarget=NULL;
//
//	while (pEnemy)
//	{
//		if(pEnemy->object->bCollidable && RTF.CollidesWith(pEnemy->object->pBoundingShape))
//		{
//			if(!pTarget)
//			{
//				pTarget=pEnemy->object;
//				break;
//			}
//
//			if(GetPos().Subtract(pEnemy->object->GetPos()).Length()<GetPos().Subtract(pTarget->GetPos()).Length())
//				pTarget=pEnemy->object;
//		}
//		pEnemy = pEnemy->next;
//	}
//}