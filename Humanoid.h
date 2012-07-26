
// Defines creates that walk on two legs.

#ifndef StormForge_Shared_Humanoid
#define StormForge_Shared_Humanoid

#include "Character.h"
#include "MemoryMgr.h"
#include "cylinder.h"

class Humanoid : public Character
{
private:
	enum PlayerActions
	{
		//different type of movement actions, just in case
		HUMANOID_MOVE,
		HUMANOID_MOVE_FORWARD_AND_BACKWARD,
		HUMANOID_MOVE_LEFT_AND_RIGHT,
		HUMANOID_MOVE_FORWARD,
		HUMANOID_MOVE_BACKWARD,
		HUMANOID_MOVE_LEFT,
		HUMANOID_MOVE_RIGHT,
		HUMANOID_JUMP,
		HUMANOID_SHOOT_ARROW,
		HUMANOID_SWITCH_ARROW,
		HUMANOID_SWING_CAMERA_LEFT,
		HUMANOID_SWING_CAMERA_RIGHT,
		//HUMANOID_,
		HUMANOID_NUM
	};

	int iActionArray[HUMANOID_NUM];

	//All of this stuff was moved to Character.h
//public:
//	int iSelectedArrow;	//selected arrow type to fire
//private:
//	// CHANGED
//	// moved the RTF down to character
//
//	Base *pTarget; //the object you have targeted

public:
	Humanoid(int iType = BASETYPE_HUMANOID, int iPlayerFlag=CHARACTER_AI_ENEMY);
	~Humanoid(){}

	#if _DEBUG
		ManageMemory(Humanoid, BASETYPE_HUMANOID);
	#endif

	void Update();
	void HandleInput();

	//movement functions
	void MoveInDirection(Vector *vMove);
	void Jump(void);

	void ResetBoundingShapeColor(void)	{}

	//moved to Character.h
	//void ShootArrow(float fPower);
	//void FindTarget(void);
};

#endif
