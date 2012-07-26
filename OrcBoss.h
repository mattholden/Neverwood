
// Defines Kara'tok

#ifndef StormForge_Shared_OrcBoss
#define StormForge_Shared_OrcBoss

#include "Humanoid.h"
#include "Character.h"
#include "MemoryMgr.h"
#include "arrow.h"

class OrcBoss : public Humanoid
{
public:

	float fAttackDurationMelee;
	float fAttackDurationArrow;
	float fDamageMelee;
	float fDamageArrow;

	OrcBoss(void) : Humanoid(BASETYPE_ORCBOSS) 
	{
		iAIState = STATE_APPROACH;

		ModelLoad ("models\\orc_static.x");
		fRed = 0.2f;
		fGreen = 0;
		fBlue = 1;
		pModel->SetMaterialColor (fRed, fGreen, fBlue);
		float fBoundingShapeRadius = fWidth * 0.3f;
		pBoundingShape = new VerticalCylinder (fHeight - fBoundingShapeRadius*1.5f, fBoundingShapeRadius); 
		ResetBoundingShapeColor();

		viewFrustum.Set (30.0f, 0.1f, 100.0f);

		bWeaponList[WEAPON_SWORD]=true;
		bWeaponList[WEAPON_BOW] = true;
		iSelectedWeapon=WEAPON_SWORD;

		fMaxSpd = 4.5f;
		fTurningRate = 360;
		fFriction = 5.0f;

		fAttackProb = 0.6f;
		fAttackDurationMelee = 0.2f;
		fAttackDurationArrow = 0.5f;
		fDamageMelee = 25;
		fDamageArrow = 10;
		fAttackRange = 4;			//not really used
		iSelectedArrow = Arrow::ARROW_TYPE_NORMAL;
		
		iHitPoints = 300;
		fBlockProb = 1.0f;
		fMinBlockDuration = 1.0f;

		MTF.Set (2, 0.1f, fAttackRange, 15);
		RTF.Set (20, 0.1f, 50);		
	}//constructor

	#if _DEBUG
		ManageMemory(OrcLt, BASETYPE_ORCLT);
	#endif

	//for the sake of laziness
	void SetTarget (Base* target) { pTarget = target; }

	void ResetBoundingShapeColor(void)
	{
		pBoundingShape->SetColor(1,0,0);
	}

	void Draw()
	{
		if (RContext->bDraw && pModel)
		{
			
			Direct3D::GetMatrixStack()->Push();
			Direct3D::GetMatrixStack()->LoadMatrixClass(mGlobal);
		
			// when we load the model, its center is on the ground. However, we want the object to be centered around the middle.
			Direct3D::GetMatrixStack()->TranslateLocal (0, -fHeight*0.5f, 0);
			Direct3D::GetMatrixStack()->ApplyToWorld();
				pModel->Draw();	
			Direct3D::GetMatrixStack()->Pop();	
		}
	}

	void Update() 
	{ 
		Humanoid::Update(); 
	}

virtual void ApplyAI()
{	
	if(fFreezeTime>Timer::AppTime())
		return;
	
	// make sure that your target is always valid
	if (!pTarget)
		pTarget = Game::pPlayer1;

	// DETERMINISTIC AI
	// the following AI runs every frame to ensure smooth operation
	switch (iAIState)
	{

	// approach a target
	case STATE_APPROACH:
		{
			pBoundingShape->SetColor (1,0.4f,0);
			TurnToHorizontal (pTarget->GetPos(), Timer::GetElapsedTime() * fTurningRate);
		
			// don't get too close. It's just pointless to do so...
			if ( (pTarget->GetPos() - GetPos()).LengthSquared() > 3)
			MoveForward (Timer::GetElapsedTime() * fMaxSpd);
		}
		break;

	// attack
	case STATE_ATTACK_MELEE:
		pBoundingShape->SetColor (1,0,0);
		break;
	case STATE_ATTACK_BOW:
		pBoundingShape->SetColor (1,.5f,.5f);
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
		case STATE_APPROACH:
			{
				// if for some reason your target got eliminated, go back to patrolling
				if (!pTarget)
					break;

				// Attacking a target: check if it's not friendly and in the strike zone
				if (cPlayer == CHARACTER_AI_ENEMY && pTarget == Game::pPlayer1)
				{
						float fRand = RandomFloat();
						
						// only try to attack in random intervals
						if (fRand <= fAttackProb)
						{
							float fDistSquared = (GetPos() - pTarget->GetPos()).LengthSquared();
							
							// close enough for melee attack!
							if (MTF.CollidesWith (pTarget->pBoundingShape))
								SetAIState (STATE_ATTACK_MELEE);							
						
							else if (RTF.CollidesWith (pTarget->pBoundingShape))
								SetAIState (STATE_ATTACK_BOW);
						}			
				}//if AI vs player
			}// approaching
		break;
		case STATE_ATTACK_MELEE:
			{
				static bool bAlreadyHit = false;	//hack: make sure no one gets hit twice
				if (!bAlreadyHit && this->fTimeInAIState > 0.5f * fAttackDurationMelee)
				{
					//note: getting hit by the boss's melee should have incredible knockback
					if (MTF.CollidesWith (pTarget->pBoundingShape))
					{
						Vector vKnockBack (pTarget->GetPos() - GetPos());
						vKnockBack.fComp[1] = 0;
						vKnockBack = vKnockBack / 30;
						if (((Character*)pTarget)->TakeDamage ((int)fDamageMelee))
							((Character*)pTarget)->KnockBack (vKnockBack);
						bAlreadyHit = true;
					}
					else bAlreadyHit = false;
				}
				if (fTimeInAIState > fAttackDurationMelee)
					SetAIState (STATE_APPROACH);
			}//melee attack
			break;

			case STATE_ATTACK_BOW:
			{
				if (fTimeInAIState > 0.5f * fAttackDurationArrow)
				{
					//shoot maxed out explosive arrows at the target
					if (RTF.CollidesWith (pTarget->pBoundingShape))
					{
						ShootArrow (3.0f);
					}
				}
				if (fTimeInAIState > fAttackDurationArrow)
					SetAIState (STATE_APPROACH);
			}
			break;

		}// AI state
	}// if time for another iteration
}//apply AI

};

#endif
