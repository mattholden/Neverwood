
// defines a melee weapon


#include "Weapon.h"
#include "Character.h"

Weapon::Weapon(Character *pWielder) : Base(BASETYPE_WEAPON)
{
		iMinDamage = 3;
		iMaxDamage = 5;
		bRightHand = true;
		bCanBlock = true;
		bCanBeBlocked = true;
		bRangedWeapon = false;
		fSpeed = 0.5f;
		iAnimationID = INVALID;
		iSoundAttackID = INVALID;
		iSoundBlockedID = INVALID;
		iActionID = INVALID;
		pOwner = pWielder;
		pParticleEmitter = NULL;

		// Set base stuff.
			bCollidable = 1;

	}


// Destroy.
Weapon::~Weapon(void)
	{ 
		//if (pParticleEmitter)
		//{
		//	pParticleEmitter->Shutdown();
  //          delete pParticleEmitter; 
		//	pParticleEmitter = NULL;
		//}
	}


	// Set a particle effect on this weapon.
	void Weapon::SetParticleEffect(char *strEffectFile)
	{
		// Replace the old effect...
		//if (pParticleEmitter)
		//{
		//	pParticleEmitter->Shutdown();
		//	delete pParticleEmitter;
		//}
		
		// Guess they didn't want an effect after all.
		//if (!strEffectFile)
		//{
		//	pParticleEmitter = NULL;
		//	return;
		//}

		//pParticleEmitter = new Emitter;
		//pParticleEmitter->Initialize(strEffectFile);

		// Wait until we turn it on.
		//pParticleEmitter->StopEmitting();
		//pParticleEmitter->StopUpdating();
	}


