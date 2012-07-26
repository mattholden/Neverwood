#include "Explosion.h"
#include "Shapes.h"

Explosion::Explosion(void)
{
	pBoundingShape=new Sphere(0.1f);
	pBoundingShape->SetColor(1, 0, 0, 0.5f);
	mGlobal.SetToIdentity();
	iObjectType=BASETYPE_EXPLOSION;
	bActive=false;
	bCollidable=false;
	MoveTo(BLACKHOLE);
	iDamage=5;
}

Explosion::~Explosion(void)
{}

void Explosion::Update()
{
	if(!bActive)
		return;

	if(pBoundingShape->fWidth>=fMaxSize)
	{
		Deactivate();
	}
	pBoundingShape->mGlobal=mGlobal;
	pBoundingShape->fWidth+=0.5f;

	//if(pBoundingShape->fWidth>=fMaxSize)
	//{
	//	bCollidable=true;
	//}
}

void Explosion::Draw()
{
	if (RContext->bDraw)
	{
		Shapes::SetColor(pBoundingShape->fRed, pBoundingShape->fGreen, pBoundingShape->fBlue);
		TextureMgr::Set(0);
		Shapes::Sphere(&mGlobal, pBoundingShape->fWidth);
	}
}