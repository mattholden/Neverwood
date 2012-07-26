#ifndef _EXPLOSION_H_
#define _EXPLOSION_H_
#include "Base.h"

class Explosion :
	public Base
{
private:
	float fMaxSize; //size of explosion

	int iDamage;

	// added this so that the AI can attack whatever shot the arrow that hit it.
	// it should also be useful to avoid friendly fire if necessary
	Base* pOwner;
public:
	Explosion(void);
	~Explosion(void);

	void Activate(float fChargeTime)
	{
		bActive=true;
		bCollidable=true;
		RContext->bDraw=true;
		fMaxSize=fChargeTime*2;
	}
	void Deactivate(void)
	{
		bActive=false;
		bCollidable=false;
		RContext->bDraw=false;
		pBoundingShape->fWidth=0.01f;
		MoveTo(FAR_SPACE_STORE_POINT);
	}

	void Update(void);
	void Draw();


	void SetOwner(Base* owner){pOwner = owner;}
	Base* GetOwner(void){return pOwner;}

	int GetDamage(void){return iDamage;}

};

#endif