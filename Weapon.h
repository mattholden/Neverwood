

// Defines a hand-held weapon. -NOT- a Projectile.

#ifndef StormForge_Shared_Weapon
#define StormForge_Shared_Weapon


#include "base.h"
#include "MemoryMgr.h"


// We cheat so bad! :)
class Character;
class Emitter;

class Weapon : public Base
{
public:

	float fSpeed;
	int iMinDamage;
	int iMaxDamage;
	int iAnimationID;
	int iSoundAttackID;
	int iSoundBlockedID;
	int iActionID;
	Character *pOwner;
	Emitter *pParticleEmitter;
	bool bRightHand;
	bool bCanBlock;
	bool bRangedWeapon;	
	bool bCanBeBlocked;
    
	// Giveth and taketh away.
	Weapon(Character *pWielder = NULL);
	~Weapon(void);

	// Set a particle effect on this weapon.
	void SetParticleEffect(char *strEffectFile);

	// Memory management
	#if _DEBUG
		ManageMemory(Weapon, BASETYPE_WEAPON);
	#endif

	void ResetBoundingShapeColor(void)
		{}
};

#endif
