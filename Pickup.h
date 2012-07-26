

#ifndef Stormforge_pickup
#define Stormforge_pickup

#include "base.h"
#include "vector.h"
#include "matrix.h"
#include "direct3D.h"
#include "Game.h"
#include "Dxmodel.h"
#include "shapes.h"


enum PowerUpType
{
	POWERUP_HEALTH,
	POWERUP_UNLOCKABLE,
	POWERUP_TYPECOUNT
};

#define NUM_UNLOCKABLES 10
#define HEALTH_DROP_CHANCE 10 

class Pickup : public Base
{	
public:
	int iType;
	int iValue;

	virtual void Get(void) = 0;
	virtual void Draw(void) = 0;
};


/////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
class HealthUp : public Pickup
{
public:
	HealthUp(Vector *vPosition = NULL)
	{
		pBoundingShape = new Sphere(0.25f);
		pBoundingShape->fBlue = 0.8f;
		
		iType = POWERUP_HEALTH;
		iValue = 20;
		bActive=true;
		this->RContext->bDraw = true;

		if (vPosition)
		{
			mGlobal.SetPos(*vPosition);		
			pBoundingShape->SetPos(*vPosition);
		}

	}

	void Get(void) 
	{
		Game::pPlayer1->iHitPoints += iValue;
		if (Game::pPlayer1->iHitPoints > Game::pPlayer1->iMaxHitPoints)
			Game::pPlayer1->iHitPoints = Game::pPlayer1->iMaxHitPoints;

		this->RContext->bDraw = false;
		bActive = false;
	}

	void Draw(void)
	{
		Direct3D::GetMatrixStack()->Push();
		Direct3D::GetMatrixStack()->LoadMatrixClass(mGlobal);
		Direct3D::GetMatrixStack()->ApplyToWorld();
		Shapes::SetColor(0,0,1,0.5f);
		//Direct3D::Wireframe(true);
		Shapes::Sphere(&mGlobal, pBoundingShape->fWidth, 8,8);
		

		//Direct3D::Wireframe(false);
		Game::modFalcon.Draw();
		
		Direct3D::GetMatrixStack()->Pop();
	}

};


/////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
class Unlockable : public Pickup
{
	public:
	Unlockable(Vector *vPosition = NULL)
	{
		pBoundingShape = new Sphere(0.4f);
		float f255 = 1.0f / 255.0f;
		pBoundingShape->fRed = 173.0f * f255;
		pBoundingShape->fGreen = 152.0f * f255;
		pBoundingShape->fBlue = 87.0f * f255;
		bActive=true;

		iType = POWERUP_UNLOCKABLE;
		iValue = rand() % NUM_UNLOCKABLES;
        		
		if (vPosition)
			mGlobal.SetPos(*vPosition);		
	}

	void Get(void) 
	{

	}

	void Draw(void)
	{
	}

};






#endif