

#ifndef StormForge_Shared_Character
#define StormForge_Shared_Character

#include "Base.h"
#include "MemoryMgr.h"
#include "ActionMap.h"
#include "Frustum.h"
#include "Sphere.h"
#include "PatrolNode.h"

enum PlayerFlag 
{ 
	CHARACTER_PLAYER1,
	CHARACTER_PLAYER2,
	CHARACTER_AI_PLAYER2,
	CHARACTER_AI_ENEMY,
	CHARACTER_TYPE_COUNT,
};

enum WeaponIDs
{
	WEAPON_BOW,
	WEAPON_SWORD,
	WEAPON_COUNT,
	WEAPON_INVALID=-1
};

// defines the available states in the enemy's finite state machine
enum AIState
{
	// AI states
	STATE_PASSIVE,			//can use this to disable AI for enemies that are really far away
	STATE_PATROL,			//moving in a behavior defined by the patrol route and patrol mode
	STATE_BLOCK,			//block incoming attacks. Enemies that can't block will never transition to this state
	STATE_APPROACH,			//approach a target. Could be to help an ally, attack an enemy, retrieve an item, etc
	STATE_ATTACK,			//entity is attacking its target. Attacking means that an attack animation is currently playing
	STATE_ATTACK_MELEE,
	STATE_ATTACK_BOW,
	STATE_ALERTED,			//entity just got alerted of an enemy presence. This is a very temporary state, and is used only to introduce small delays in enemy reaction times, making their behavior more natural.
	NUM_STATES
};

enum PatrolMode
{
	PATROL_PACING,			//going back and forth along a predefined path
	PATROL_CIRCULAR,		//going around in circles along a predefined path
	PATROL_RANDOM,			//switching randomly between predefined points
	NUM_PATROLMODES
};

#define PATROL_TIMEOUT 5.0f	//change patrol point if it's taken you this long to reach one

class Character : public Base
{

protected:
	/////////////////////////////////////
	// CHARGE INFO
	/////////////////////////////////////

	#define CHARGE_MAX_TIME 3.0f
	
	bool bIsCharging;
	float fChargeTime;

  	/////////////////////////////////////
	// STATE INFORMATION
	////////////////////////////////////
public:
	int iCurrentPowerup;			// The current powerup type the player has. 
	int iHitPoints;					// The player's current hit points
	int iMaxHitPoints;				// Player's max hit points
	int iDamageTaken;

protected:

	// timing information
	float fLastAttack;				// how many seconds ago the character last attacked

	// special status states
	bool bFrozen;
	bool bConfused;
	bool bOnFire;
	float fLastStatusChange;	// time stamp of the last time the entity got confused / frozen / on fire. Used to ensure that the above flags last temporarily

	float fFreezeTime;			//the time when the person should thaw

	//float fPoisonTime, fPoisonTime2;	
	bool bKnockBack;			// True after the character has been hit by a weapon
	Vector vKnockBack;			// the direction in which the character is being knocked back. We need this since we're not using real-world physics
	float fLastHit;				// time elapsed since the last time a character was hit. Useful for temporary invulnerability after a hit + temporary knockback after a hit.
	float fKnockBackDuration;	// for how long the object should get knocked back. That's right, Guardians of Neverwood does NOT use real-world physics for knockback.

	/////////////////////////////////////
	// ARTIFICIAL INTELLIGENCE
	////////////////////////////////////
	int iAIState;							// which state of the entity's FSM it's currently in
	float fTimeInAIState;					// how long the entity has been in the current AI state. Used for temporary states and time-outs
	float fLastAIUpdate;					// how long ago the enemy's AI updated the last time. This is used in conjunction with fReactionTime to have the enemy react at a more human rate
	
	int iPatrolMode;						// determines how the character's AI uses its patrol points
	bool bForwardPatrol;					// whether the AI is traversing the patrol points in a forward or backward order. Only relevant for the pacing patrol mode
	PatrolRoute Patrol;						// DLL of patrol points
	int iCurrentPatrolPoint;				// which patrol point you're currently trying to reach
									// this is used to prevent the AI from having super-human reflexes
	float fBlockProb;						// the probability of a character blocking an incoming attack if it sees one. Note that this is applied each AI iteration
	float fAttackProb;						// the probability of a character attacking another if it is in range. Note that this is applied each AI iteration
	float fChangeTargetProb;				// the probability of a character switching targets if it can attack more than one. Note that this is applied each AI iteration
	float fAlertProb;						// the probability of a character being alerted from a near miss or enemy signal. This is applied only once
	float fApproachProb;					// the probability of transitioning from alert to approach. 
	Base* pTarget;							// the object the character's AI is pursuing. Could be a friend, foe, powerup, etc. Use your imagination.	

	float fAttackRange;
	float fAttackDuration;					// the minimum amount of time between two successive attacks
	float fMinBlockDuration;				// if the AI decides to block, it will keep its guard up for at least this long
	float fReactionTime;					// the amount of time, in seconds, between two consecutive AI updates. Default: 0.35
		

	/////////////////////////////////////
	// WEAPON INFORMATION
	////////////////////////////////////
	
	Vector vWHO; //Weapon Hardpoint Offset
	//BoundingShape* pRTF;	//Ranged Targeting Frustum. Represents the weapon's range of attack
	//BoundingShape* pMTF; //Melee Targeting Frustum
	Frustum RTF;	//Ranged Targeting Frustum. Represents the weapon's range of attack
	Frustum MTF; //Melee Targeting Frustum

	int iSelectedArrow;	//selected arrow type to fire

	bool bWeaponList[WEAPON_COUNT]; //Selected weapon to attack with
	int iSelectedWeapon;

	/////////////////////////////////////
	// MOVEMENT/PHYSICS INFORMATION
	////////////////////////////////////

	float fTurningRate;		//the rate at which you turn.. my only really effect the camera pos
	float fAccel;					//an acceleration constant.  It's added to the velocity every frame (modified by time elapsed)
	float fMaxSpd;				//The maximum speed the character can generate under their own will.
	float fFriction;				//A mostly constant variable to represent friction (decceleration).
	float fJumpStrength;	//the strength o' the jump
	float fSpeedMod;			//the speed modifier.. used to speed up or slow down.. mostly for the slow down for ice arrows.

	/////////////////////////////////////
	// OTHER INFORMATION
	/////////////////////////////////////
	Frustum	viewFrustum;		//determines what an AI controlled character can see

	void Attack(void);

public:

	~Character() { if(pActionMap) { delete pActionMap; pActionMap = 0; } }

	ActionMap *pActionMap;		// Stores the mappings of keys to actions.
												// In AI-controlled characters, this pointer is NULL.

	char cPlayer;						// Will store one of the PlayerFlag enumerations above, 
												// to help us sort out who this is.


	//action bools!
	char bMove:1;						//Disable the players's abilitity to move the character
	char bCameraControl:1;		//Disable the ability to control the camera..  This will probably never be false, but lets keep it around for consistancy and just in case.
	char bDefCamPos:1;				//Snaps the camera back into it's default position.. This probably will never be false either.
	char bShootArrow:1;			//Disables the ability to fire an arrow for the humanoids.
	char bSlashCharge:1;				//Disables the ability to slash with sword for humanoids and the ability to charge attack with the birds.
	char bThrustScratch:1;			//Disables the ability to thrust for the humanoids and the scratching ability for the avian types.
	char bCrouch:1;					//Disables the ability to crouch for the humanoids.
	char bBlock:1;						//Disables the abiliity to block for the humanoids.
	char bDodge:1;						//Disables the ability to dodge for the humanoids.
	char bJump:1;						//Disables the ability to jump for the humanoids;
	char bUseTakeDrop:1;			//Disables the ability to use switches, take items or drop items;
	char bTogBStates:1;				//Disables the ability to toggle the bird's AI states in one player mode for the humanoids.
	char bTauntScreech:1;			//Disables the ability to taunt as a humanoid or screech as a bird.
	char bSniper:1;						//Disables the ability to go into sniper mode;
	char __DONT_USE_PADDING:2; //padding for the action bools;

	
	// AI Functions
	
	// implements behavior for each state of the finite state machine
	virtual void ApplyAI();

	void AddPatrolPoint(Vector &vPointToAdd) { Patrol.Add(vPointToAdd); }
	void AddPatrolPoint(float x, float y, float z) { Patrol.Add (Vector (x,y,z)); }
	void RemovePatrolPoint(Vector &vPointToRemove) { Patrol.Remove(vPointToRemove); }
	void ClearPatrolPoints(void) { Patrol.Clear(); }
	
	// Collision functions

	// pushes the object in a given direction + magnitude for a given amount of time
	// the magnitude is in units (meters) per second, and the time is in seconds
	void KnockBack (Vector& vKnockback);

	// Handles HP loss, state transitions, special FX, etc associated with getting hit by an object
	// also, only applies damage if the object isn't blocking or otherwise invulnerable. 
	virtual void OnHit (Base* pObject);
	virtual bool TakeDamage (int damage);

	void ShootArrow(float fChrgTime);
	void FindTarget(void);

	// determines whether the character can see an object
	// checks if any other objects or world geometry are in the way
	// this can get pretty expensive, so use it sparingly, OK?
	bool CanSee (Base* other);

	// Set up a character
	Character(int iType = BASETYPE_CHARACTER, int iPlayerFlag=CHARACTER_AI_ENEMY);
	
	// override the base class's update to do additional things
	virtual void Update();

	virtual void HandleInput();

	virtual void OnDeath(void);


	// Manage Memory
	#if _DEBUG	
		ManageMemory(Character, BASETYPE_CHARACTER);
	#endif

	//ACCESSORS
	Frustum* GetViewFrustum() {return &viewFrustum;}
	float GetTurningRate (){return fTurningRate;}
	float GetAcceleration (){return fAccel;}
	float GetMaxSpeed (){return fMaxSpd;}
	float GetFriction (){return fFriction;}
	float GetJumpStrength(){return fJumpStrength;}
	float GetSpeedMod(){return fSpeedMod;}
	Vector GetWHO(){return vWHO;}
	int GetSelectedArrow(void){return iSelectedArrow;}

	int GetAIState() { return iAIState; }
	int GetHP(void) { return iHitPoints; }
	int GetMaxHP(void) { return iMaxHitPoints; }

	float GetChargeTime(void) { return fChargeTime; }
	float GetChargePct(void) { float fChargePct = fChargeTime / CHARGE_MAX_TIME; return (fChargePct > 1) ? 1.0f : fChargePct; }

	BoundingShape* GetRTF(void){return &RTF;}

	// MUTATORS
	void SetTurningRate(float fNewTurnRate){fTurningRate=fNewTurnRate;}
	void SetAcceleration(float fNewAccel){fAccel=fNewAccel;}
	void SetMaxSpeed(float fNewMaxSpd){fMaxSpd=fNewMaxSpd;}
	void SetFriction(float fNewFriction){fFriction=fNewFriction;}
	void SetJumpStrength(float fNewJumpStrength){fJumpStrength=fNewJumpStrength;}
	void SetSpeedMod(float fNewSpeedMod){fSpeedMod=fNewSpeedMod;}
	void SetWHO(Vector vNewWHO){vWHO=vNewWHO;}


	void SetAIState (int newState);
	void SetReactionTime (float newTime) { fReactionTime = newTime; }
	void SetBlockProb (float newProb) { fBlockProb = newProb; }
	void SetAttackProb (float newProb) { fAttackProb = newProb; }
	void SetChangeTargetProb (float newProb) { fChangeTargetProb = newProb; }
	void SetAlertProb (float newProb) { fAlertProb = newProb; }
	void SetPatrolMode (int newMode);	
};

#endif
