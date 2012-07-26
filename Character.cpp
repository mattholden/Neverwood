#include "Character.h"
#include "PatrolNode.h"

#include "Game.h"
#include "Arrow.h"
#include "Explosion.h"
#include "MathUtils.h"	//for RandomFloat()
#include "pickup.h"

#define DEAD_AND_BURIED -527

Character::Character(int iObjectType, int iPlayerFlag): Base (iObjectType)
	{	
		// Fill in Character values.
		cPlayer = iPlayerFlag;

		fChargeTime = 0.0f;
		bool bIsCharging = false;

		if(cPlayer==CHARACTER_PLAYER1)
			pActionMap = new ActionMap(0);	
		else if(cPlayer==CHARACTER_PLAYER2)
			pActionMap = new ActionMap(1);
		else 
			pActionMap = NULL;

		bWeaponList[WEAPON_SWORD]=true;
		iSelectedWeapon=WEAPON_SWORD;
		fAttackRange = 5;

		iCurrentPowerup = INVALID;
		iHitPoints = 50;
		iMaxHitPoints = 50;

		// Fill in Base values.
		bCollidable = 1;
		bTakesDamage = 1;
		bGroundClamped = 1;
		
		// fill in state / timing values
		iCurrentPowerup = 0;		//iCurrentPowerup? WTF? -Rolf
		bFrozen = bConfused = bOnFire = bKnockBack = 0;
		fLastStatusChange = fLastHit = 0;
		vKnockBack = Vector (0,0,0);
		fKnockBackDuration = 0;
		iAIState = iPatrolMode = 0;
		fTimeInAIState = 0;
		fLastAIUpdate = 0;
		iCurrentPatrolPoint = 0;
		fLastAttack = 0;
		
		// AI data
		fBlockProb = 0;
		fMinBlockDuration = 0.5f;
		fAttackProb = 0.2f;
		fApproachProb = 0.4f;
		fChangeTargetProb = 0.4f;
		fAlertProb = 0.8f;
		fAttackRange = 2.0f;
		fAttackDuration = 1.0f;
		fReactionTime = 0.25f;
		
		pTarget = NULL;

	fFreezeTime=0;

	vWHO.SetToZero();

	bMove=1;						//Disable the players's abilitity to move the character
	bCameraControl=1;		//Disable the ability to control the camera..  This will probably never be false, but lets keep it around for consistancy and just in case.
	bDefCamPos=1;				//Snaps the camera back into it's default position.. This probably will never be false either.
	bShootArrow=1;			//Disables the ability to fire an arrow for the humanoids.
	bSlashCharge=1;				//Disables the ability to slash with sword for humanoids and the ability to charge attack with the birds.
	bThrustScratch=1;			//Disables the ability to thrust for the humanoids and the scratching ability for the avian types.
	bCrouch=1;					//Disables the ability to crouch for the humanoids.
	bBlock=1;						//Disables the abiliity to block for the humanoids.
	bDodge=1;						//Disables the ability to dodge for the humanoids.
	bJump=1;						//Disables the ability to jump for the humanoids;
	bUseTakeDrop=1;			//Disables the ability to use switches, take items or drop items;
	bTogBStates=1;				//Disables the ability to toggle the bird's AI states in one player mode for the humanoids.
	bTauntScreech=1;			//Disables the ability to taunt as a humanoid or screech as a bird.
	bSniper=1;						//Disables the ability to go into sniper mode;

	fTurningRate=180.0f;
	fAccel=20.0f;
	fMaxSpd = 1.0f;
	fFriction=10.0f;
	fJumpStrength=0.0f;
	fSpeedMod=1;

		// Set frustums
		viewFrustum.Set (20, 0.1f, 50);
		viewFrustum.SetColor (0.5f, 0.5f, 0.5f);
		RTF.Set (10, 0.1f, 50);
		RTF.SetColor (1,0,0);			

	//pRTF=new Frustum(60, 0.1f, 50, 0.05f);
	//pMTF=new Frustum(60, 0.1f, 1, 0.05f);
	bIsCharging=false;
	iSelectedArrow=Arrow::ARROW_TYPE_NORMAL;


	for(int i=0; i<WEAPON_COUNT; ++i)
		bWeaponList[i]=false;

	iSelectedWeapon=WEAPON_INVALID;
	}

void Character::Update()
{
	// I'm dead and buried, don't waste time on me
	if(!bActive) return;
	if (iHitPoints == DEAD_AND_BURIED) return;

	Base::Update();

	// update time stamps
	float fElapsedTime = Timer::GetElapsedTime();
	fLastStatusChange += fElapsedTime;
	fLastAttack += fElapsedTime;
	fLastHit += fElapsedTime;
	fTimeInAIState += fElapsedTime;
	fLastAIUpdate += fElapsedTime;
	//fPoisonTime1 += fElapsedTime;
	//fPoisonTime2 += fElapsedTime;

	// apply time-based state transitions
	if (bKnockBack && (fLastHit > fKnockBackDuration))
		bKnockBack = false;
	if (bFrozen && Timer::AppTime() > fFreezeTime)
	{
		bFrozen = false;
		pModel->SetMaterialColor (fRed, fGreen, fBlue);
	}

	// TO DO: poison

	// update position
	if (bKnockBack)
	{
		MoveBy (vKnockBack * fElapsedTime);		
	}

	// update the frustums
	viewFrustum.mGlobal = mGlobal;
	RTF.mGlobal = mGlobal;
	MTF.mGlobal = mGlobal;

	if(iHitPoints<=0)
		OnDeath();	
}

void Character::ApplyAI()
{	
	if(fFreezeTime>Timer::AppTime())
		return;
	
	// make sure that your target is always valid
	if (!pTarget)
		pTarget = Game::pPlayer1;

	// optimization: make the entity passive if he's off the screen and far away
	if (!RContext->bInCameraFrustum && iAIState != STATE_PASSIVE)
	{
		float fDist = RContext->fDistSquared;

		if (fDist > 1000)
			SetAIState (STATE_PASSIVE);
	}

	// DETERMINISTIC AI
	// the following AI runs every frame to ensure smooth operation
	switch (iAIState)
	{

	// PASSIVE
	case STATE_PASSIVE:
		pBoundingShape->SetColor (0.4f, 0.4f, 0.4f);
		{
			float fDist = RContext->fDistSquared;

			if (fDist < 1000 || RContext->bInCameraFrustum)
				SetAIState (STATE_PATROL);
		}
		break;
	// PATROLLING
	case STATE_PATROL:
		{
			pBoundingShape->SetColor (1,1,0);
			//special case: don't patrol if there is no patrol route 
			if (!Patrol.pFirst)
				break;

			//transitions: change current patrol point when reaching the target point
			float fDistSquared = (GetPos() - Patrol[iCurrentPatrolPoint]->vPatrolPt).LengthSquared();
			if (fDistSquared < 1.0f || fTimeInAIState > PATROL_TIMEOUT)		//distance of approximately 0.5 meters from the target
			{
				fTimeInAIState = 0;

				// special case: if there is only one patrol point, don't bother choosing another
				if (Patrol.iPointCount == 1)
					break;

				if (iPatrolMode == PATROL_RANDOM)
					iCurrentPatrolPoint = rand() % Patrol.iPointCount;
				else if (iPatrolMode == PATROL_CIRCULAR)
				{
					iCurrentPatrolPoint++;
					if (iCurrentPatrolPoint == Patrol.iPointCount)
						iCurrentPatrolPoint = 0;
				}
				else if (iPatrolMode == PATROL_PACING)
				{
					if (bForwardPatrol)
					{
						iCurrentPatrolPoint++;
						if (iCurrentPatrolPoint == Patrol.iPointCount)
						{
							iCurrentPatrolPoint = Patrol.iPointCount - 2;
							bForwardPatrol = false;
						}
					}

					else
					{
						iCurrentPatrolPoint--;
						if (iCurrentPatrolPoint == -1)
						{
							iCurrentPatrolPoint = 1;
							bForwardPatrol = true;
						}
					}
				}
			}// if you've reached your patrol point

			//try to reach the next patrol point
			TurnToHorizontal (Patrol[iCurrentPatrolPoint]->vPatrolPt, Timer::GetElapsedTime() * fTurningRate);
			if (!bGroundClamped)
				TurnToVertical (Patrol[iCurrentPatrolPoint]->vPatrolPt, Timer::GetElapsedTime() * fTurningRate);
			MoveForward (Timer::GetElapsedTime() * fMaxSpd);
		}// Patrol mode
		break;

	// alerted
	case STATE_ALERTED:
		pBoundingShape->SetColor (1,0.7f,0);
		break;

	// approach a target
	case STATE_APPROACH:
		{
			pBoundingShape->SetColor (1,0.4f,0);
			// TO DO: might have to swerve around objects that are in the way...
			TurnToHorizontal (pTarget->GetPos(), Timer::GetElapsedTime() * fTurningRate);
			if (!bGroundClamped)
				TurnToVertical (pTarget->GetPos(), Timer::GetElapsedTime() * fTurningRate);

			// don't go any closer than your attack range. It's just pointless to do so...
			if ( (pTarget->GetPos() - GetPos()).LengthSquared() > fAttackRange * fAttackRange)
				MoveForward (Timer::GetElapsedTime() * fMaxSpd);
		}
		break;

	// attack
	case STATE_ATTACK:
		pBoundingShape->SetColor (1,0,0);
		break;

	// defend
	case STATE_BLOCK:
		pBoundingShape->SetColor (0,1,0);
		break;
	}// switch (AI state)
	

	// PROBABILISTIC AI
	// the following AI only runs a couple of times per second
	// this ensures that the AI does not react at a superhuman rate, and that the AI's behavior does not depend on the game's framerate
	// it also just so happens to save a lot of computation, too :)
	if (fLastAIUpdate >= fReactionTime)
	{
		fLastAIUpdate = 0;

		// MELEE COMBAT
		switch (iAIState)
		{
		// patrol -> alerted transition
		case STATE_PATROL:
			{
				// if an enemy can see a player, it will approach her
				if (cPlayer == CHARACTER_AI_ENEMY)
				{
					if (CanSee (Game::pPlayer1))
					{
						pTarget = Game::pPlayer1;
						SetAIState (STATE_ALERTED);
					}					
				}
			}
			break;

		// alerted: this state is basically a random delay before approaching an enemy
		case STATE_ALERTED:
			{
				float fRand = RandomFloat();
				if (fRand <= this->fApproachProb)
					this->SetAIState (STATE_APPROACH);
			}
			break;

		case STATE_APPROACH:
			{
				// if for some reason your target got eliminated, go back to patrolling
				if (!pTarget)
					SetAIState (STATE_PATROL);

				// time out: don't follow your target forever. Instead, go back to patrolling if the target has been out of sight for a long time
				if (CanSee (pTarget))
					fTimeInAIState = 0;
				if (fTimeInAIState > 10)
					SetAIState (STATE_PATROL);


				// Attacking a target: check if it's not friendly and in the strike zone
				// TO DO: account for several different types of attacks if we have them
				if (cPlayer == CHARACTER_AI_ENEMY)
				{
					if (pTarget == Game::pPlayer1)
					{
						float fDist = (pTarget->GetPos() - GetPos()).LengthSquared();
						if (fDist <= fAttackRange * fAttackRange && CanSee(pTarget))
						{
							float fRand = RandomFloat();
							if (fRand < this->fAttackProb)
							{
								SetAIState (STATE_ATTACK);
								fLastAttack = 0;
							}
						}
					}
				}

				// blocking incoming attacks
				if (cPlayer == CHARACTER_AI_ENEMY)
				{
					for (BaseNode* pNode = Game::lArrows.first; pNode; pNode = pNode->next)
					{
						if (!pNode || !pNode->object || !pNode->object->bActive)
							continue;

						// CHANGED: using the arrow's bounding sphere instead of line
						if (viewFrustum.CollidesWith (pNode->object->pBoundingShape))
						{
							float fRand = RandomFloat();
							if (fRand < this->fBlockProb)
								SetAIState (STATE_BLOCK);
						}
					}
				}
			break;
			}// approaching

		case STATE_ATTACK:
			{
				Attack();
			}
			break;

		case STATE_BLOCK:
			{
				if (fTimeInAIState < fMinBlockDuration)
					break;
		
				bool bChangeState = true;

				// ALWAYS block additional incoming attacks
				if (cPlayer == CHARACTER_AI_ENEMY)
				{
					for (BaseNode* pNode = Game::lArrows.first; pNode; pNode = pNode->next)
					{
						if (!pNode->object->bActive)
							continue;

						if (viewFrustum.CollidesWith (*((Arrow*)pNode->object)->GetBoundingLine()))
							bChangeState = false;
					}
				}

				float fRand = RandomFloat();
				if (fRand < 1.0f - fBlockProb)
					bChangeState = false;

				if (bChangeState)
					SetAIState (STATE_APPROACH);
			}
			break;
		}// switch (AIState)
	}// if time for an AI iteration
}

void Character::SetPatrolMode (int newMode)
{
	iPatrolMode = newMode;
	
	// special case: if there are no patrol points, there is no need to modify that patrol points. DUH!
	if (!Patrol.pFirst)
		return;

	// close the linked list for circular patrol mode
	if (iPatrolMode == PATROL_CIRCULAR)
	{
		Patrol.pFirst->pPrev = Patrol.pLast;
		Patrol.pLast->pNext = Patrol.pFirst;
	}

	// open the linked list for the other patrol modes
	else
	{
		Patrol.pFirst->pPrev = NULL;
		Patrol.pLast->pNext = NULL;
	}
}

void Character::SetAIState (int newState)
{
	iAIState = newState;
	fTimeInAIState = 0;
	if (newState == STATE_ATTACK)
		SoundManager::Play (Game::iSIDSwoosh);
}
void Character::OnHit(Base* pObject)
{
	// make sure each hit only registers once
	if (!bTakesDamage || !bCollidable)
		return;

	// BLOCKING:
	// can block incoming attacks unless they are charged up
	if (iAIState == STATE_BLOCK)
	{
		if (pObject->iObjectType != BASETYPE_ARROW)
			return;
		if ( ((Arrow*)pObject)->GetChargeTime() < 1.0f )
			return;
	}

	// apply AI changes
	if (cPlayer == CHARACTER_AI_ENEMY)
	{
		switch (iAIState)
		{
		// getting hit will knock an enemy out of patrolling state and make him pursue whatever hit him
		case STATE_PATROL:
			if (pTarget->iObjectType == BASETYPE_ARROW)
				pTarget = ((Arrow*)pObject)->GetOwner();
			SetAIState (STATE_ATTACK);
			break;
		// hitting an enemy might cause him to change targets if he's already pursuing another target
		case STATE_APPROACH:
			{
				float RandFloat = RandomFloat();
				if (RandFloat < fApproachProb)
					pTarget = ((Arrow*)pObject)->GetOwner();
			}
			break;
		}
	}
			

	// do some damage
	int iDamageDone=0;
	float fDamageMod=0;

	switch(pObject->iObjectType)
	{
	case BASETYPE_ARROW:
		// can't hit yourself
		if (((Arrow*)pObject)->GetOwner() == this)
			return;

		iDamageDone=((Arrow*)pObject)->GetDamage();
		switch(((Arrow*)pObject)->GetArrowType())
		{
		case Arrow::ARROW_TYPE_NORMAL:
			fDamageMod=((Arrow*)pObject)->GetChargeTime();
			if(fDamageMod<1)
				fDamageMod=1;
			iDamageDone=(int)((float)iDamageDone*fDamageMod);
			break;
		case Arrow::ARROW_TYPE_FLAME:
			{
				Explosion* pExp=NULL;
				BaseNode* pSearch=Game::lExplosions.first;
				while(pSearch)
				{
					if(!pSearch->next)
						pExp=((Explosion*)pSearch->object);

					if(!pSearch->object->bActive)
					{
						pExp=((Explosion*)pSearch->object);
						break;
					}
					pSearch=pSearch->next;
				}

				if(pExp->bActive)
					pExp->Deactivate();

				pExp->Activate(((Arrow*)pObject)->GetChargeTime());

				pExp->SetOwner(((Arrow*)pObject)->GetOwner());

				pExp->mGlobal=pObject->mGlobal;
			}
			break;
		case Arrow::ARROW_TYPE_FROST:
			fFreezeTime=Timer::AppTime()+1+((Arrow*)pObject)->GetChargeTime()*3;
			// CHANGED: changing colors when objects are frozen
			bFrozen = 1;
			pModel->SetMaterialColor (0,1,1);
			break;
		//case Arrow::ARROW_TYPE_POISON:
		//	fPoisonTime1 = fPoisonTime2 = 0;
		//	break;
		default:
			break;
		}
		SoundManager::Play(Game::iSIDArrowHit1);
		TakeDamage(iDamageDone);
		break;
	case BASETYPE_EXPLOSION:
		KnockBack((GetPos().Subtract(pObject->GetPos()))/10);
		vExternal.fComp[1]=10;
		TakeDamage(((Explosion*)pObject)->GetDamage());
		fFreezeTime=Timer::AppTime()+1;
		break;
	default:
		break;
	}// switch (object type)


	// knock back																	//to account for fast-moving objects like arrows, of course
	KnockBack (pObject->vVelocity * 0.5f);
}

bool Character::TakeDamage (int damage)
{
	// make sure each hit only registers once
	if (!bTakesDamage || !bCollidable)
		return false;

	fLastHit = 0;
	iDamageTaken += damage;

	if (cPlayer == CHARACTER_PLAYER1 && DirectInput::GetJoystick(0))
		DirectInput::GetJoystick(0)->FeedbackRunEffect(0,true);

	// do some damage
	iHitPoints -= damage;
	if (iHitPoints <= 0)
	{
		OnDeath();
		return true;
	}

	// clamp HP to 0
	if (iHitPoints < 0)
		iHitPoints = 0;

	return true;
}

bool Character::CanSee (Base* other)
{
	// if the object is really close to you, it should alert you even if you can't see it
	if ( (other->GetPos() - GetPos()).LengthSquared() < 9)
		return true;

	// check for visibility
	if (!viewFrustum.CollidesWith (other->pBoundingShape))
		return false;


	// check for objects in the way
	Line lToOther (GetPos(), other->GetPos());
	for (BaseNode* pNode = Game::lStaticObjects.first; pNode; pNode = pNode->next)
	{
		if (pNode->object->CollidesWith (lToOther))
			return false;
	}
	
	// check for world geometry
	if (Game::pWorld->CollidesWith (lToOther))
		return false;

	return true;
}

void Character::OnDeath(void)
{
	if(!bActive)
		return;

	// Drop powerups
	if (rand() % 100 < HEALTH_DROP_CHANCE&&cPlayer!=CHARACTER_PLAYER1)
	{
		HealthUp *pNew = new HealthUp(&mGlobal.GetPos());
		Game::lPickups.Insert(pNew);
	}

	bActive=false;

	// CHANGED
	//not moving player to the same point as arrows + explosions
	if (cPlayer != CHARACTER_PLAYER1) 
		MoveTo(FAR_SPACE_STORE_POINT);
	else MoveTo (527,527,527);
	bCollidable=false;
	RContext->bDraw=false;
	pBoundingShape->SetColor(0,0,1);

	iHitPoints = DEAD_AND_BURIED;
	if (this->cPlayer != CHARACTER_PLAYER1)
	{
		Game::iEnemies--;
	}

}

void Character::KnockBack (Vector& vDirection)
{
	if (fLastHit < 0.1f)
		return;

	fLastHit = 0;

	Vector vKnockBack = vDirection;
	// ground units can't get knocked in the vertical direction
	if (bGroundClamped)
	{

		vKnockBack.fComp[1] = 0;
		float fLength = vKnockBack.Length();
		if (FloatEqual (fLength, 0))
			vKnockBack = Vector (0, 0, -1);
		else vKnockBack = vKnockBack * ( vDirection.Length() / fLength);
	}

	vInternal += vKnockBack;

	/*
	bKnockBack = 1;
	vKnockBack = vDirection;
	fKnockBackDuration = fTime;
	*/	
}


void Character::HandleInput()
{

}

void Character::ShootArrow(float fChrgTime)
{
	// check if it's time
	if (fLastAttack < fAttackDuration)
		return;
	
	fLastAttack = 0;

	Arrow*pArrow=NULL;

	SoundManager::Play(Game::iSIDArrowShot1);

	BaseNode* pSearch=Game::lArrows.first;
	while(pSearch)
	{
		if(!pArrow)
			pArrow=((Arrow*)pSearch->object);

		if(!pSearch->object->bActive)
		{
			pArrow=((Arrow*)pSearch->object);
			break;
		}

		if(((Arrow*)pSearch->object)->GetTimeStamp()<pArrow->GetTimeStamp())
			pArrow=((Arrow*)pSearch->object);

		pSearch=pSearch->next;
	}

	pArrow->Activate();
	pArrow->SetOwner (this);

	pArrow->mGlobal=mGlobal;
	pArrow->mGlobal.TranslateLocal(GetWHO());
	pArrow->vLastPos=pArrow->GetPos();
	pArrow->GetBoundingLine()->vBegin=pArrow->GetPos();
	pArrow->GetBoundingLine()->vEnd=pArrow->GetPos();

	if(cPlayer==CHARACTER_PLAYER1)
		FindTarget();

	if(pTarget)
		pArrow->SetDirection(pTarget->GetPos()-pArrow->GetPos());
	else
		pArrow->SetDirection(pArrow->GetZ());

	pArrow->SetArrowAttributes(iSelectedArrow, fChrgTime);

	// how's about some different colors for different types of arrows?
	switch (iSelectedArrow)
	{
	case Arrow::ARROW_TYPE_NORMAL:
		pArrow->pModel->SetMaterialColor (0.9f,0.7f,0);
		break;
	case Arrow::ARROW_TYPE_FROST:
		pArrow->pModel->SetMaterialColor (0, 0.5f, 1);
		break;
	case Arrow::ARROW_TYPE_FLAME:
		pArrow->pModel->SetMaterialColor (1,0,0);
		break;
	}
}

void Character::FindTarget(void)
{
	BaseNode* pEnemy = Game::lEnemies.first;
	pTarget=NULL;

	while (pEnemy)
	{
		if(pEnemy->object->bCollidable&&RTF.CollidesWith(pEnemy->object->pBoundingShape))
		{
			RTF.SetColor(1,1,0);
			if(!pTarget)
			{
				pTarget=pEnemy->object;
				break;
			}

			if(GetPos().Subtract(pEnemy->object->GetPos()).LengthSquared()<GetPos().Subtract(pTarget->GetPos()).LengthSquared())
				pTarget=pEnemy->object;
		}
		pEnemy = pEnemy->next;
	}
}

void Character::Attack(void)
{
	// paranoid error-checkin'
	if (!pTarget)
		return;

	static bool bHitTarget = false;
	if (fLastAttack >= fAttackDuration)
		SetAIState (STATE_APPROACH);

	switch(iSelectedWeapon)
	{
	case WEAPON_SWORD:
		{
			if (fLastAttack > fAttackDuration*0.75f && !bHitTarget)
			{
				Line lAttack (GetPos(), GetPos() + GetZ() * fAttackRange);
				if (pTarget->CollidesWith (lAttack))
				{
					bHitTarget = true;
					((Character*)pTarget)->TakeDamage (10);
					Vector vKnockBackDirection = (pTarget->GetPos() - GetPos()).Normalize();
					((Character*)pTarget)->KnockBack (vKnockBackDirection*10);
					SoundManager::Play (Game::iSIDThud);
				}
			}
			else bHitTarget = false;
		}
		break;
	case WEAPON_BOW:
		ShootArrow(0);
		break;
	default:
		break;
	}
}