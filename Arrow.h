#ifndef _ARROW_H_
#define _ARROW_H_
#include "Base.h"
#include "Sphere.h"

#include "Direct3D.h"
#include "DXModel.h"

#include "Renderer.h"
#include "Game.h"


class Arrow :
	public Base
{

private:
	float fOriginalPower;
	float fPower;
	float fChargeTime;
	float fCreationTimeStamp;

	int iArrowType;

	static DXModel *pArrowModel;
	int iDamage;

	Line* pBoundingLine;

	// added this so that the AI can attack whatever shot the arrow that hit it.
	// it should also be useful to avoid friendly fire if necessary
	Base* pOwner;

public:

	enum ArrowTypes
	{
		ARROW_TYPE_NORMAL,
		ARROW_TYPE_FLAME,
		ARROW_TYPE_FROST,
		ARROW_TYPE_POISON,
		ARROW_TYPE_NUM,
	};

	static bool LoadArrowModel()
	{
		Arrow::pArrowModel = new DXModel;
		bool bReturn = pArrowModel->Load("models\\arrow.x");
		Arrow::pArrowModel->Scale(0.2f, 0.2f, 0.075f);
		Arrow::pArrowModel->SetMaterialColor (1,0.4f,0);
		return bReturn;
	}

	static void DeleteModel()
	{
		if (pArrowModel)
		{
			delete pArrowModel;
			pArrowModel = NULL;
		}
	}

		


	void Draw()
	{
		Direct3D::GetMatrixStack()->Push();
		Matrix mArrow = mGlobal;
		mArrow.TranslateLocal (0.1f,-0.3f,fDepth*0.5f);
		Direct3D::GetMatrixStack()->LoadMatrixClass(mArrow);
		Direct3D::GetMatrixStack()->ApplyToWorld();
		Direct3D::SetMaterial (this->pModel->GetMaterial(0));
		TextureMgr::Set(0);		//no texture
		Arrow::pArrowModel->DrawSubset(0);
		//Arrow::pArrowModel->Draw();
		Direct3D::GetMatrixStack()->Pop();
	}

	Arrow(void)
	{
		pModel = Arrow::pArrowModel;
		MoveTo(FAR_SPACE_STORE_POINT);
		// CHANGED
		// now using a bounding shape...because we can. Lines are excruciatingly slow, so we're not using them for frustum collision
		pBoundingShape=new Sphere(GetPos(), 0.5f);
		pBoundingLine = new Line;

		pBoundingLine->vBegin = GetPos();
		pBoundingLine->vEnd = GetPos();
		fChargeTime=0;
		if(fChargeTime>3)
			fChargeTime=3;
		fOriginalPower=15.25f;
		fPower=fOriginalPower;

		bActive=false;

		fCreationTimeStamp=0;

		iArrowType=ARROW_TYPE_NORMAL;
		iObjectType=BASETYPE_ARROW;

		iDamage=10;

		vLastPos=Vector(FAR_SPACE_STORE_POINT);

		// CHANGED
		// made color more visible
		fRed = 1;
		fGreen = 0.2f; 
		fBlue = 0.6f;
		
		pModel->SetMaterialColor (fRed, fGreen, fBlue);
	}


	// CHANGED
	// not deleting the arrow model in the destructor because it will cause the arrow's model to be deleted several times, leading to a crash
	// I think I'll just unload all the static stuff when the game shuts down
	// The DeleteArrowModel function I added does just that
	//
	// Also, I'm deleting the bounding line
	//
	// Rolf, 6-8-04
	~Arrow(void)
	{
		delete pBoundingLine;
	}

	void SetDirection(Vector vDirection)
	{
		vVelocity=vDirection/fPower;
		FaceDirection(vDirection);
	}
	void SetArrowType(int iAType){iArrowType=iAType;}
	void SetDamage(int iDam){iDamage=iDam;}
	void SetChargeTime(float fChgTime){fChargeTime=fChgTime;}
	void SetOwner(Base* owner){pOwner = owner;}

	float GetTimeStamp(void){return fCreationTimeStamp;}
	int GetArrowType(void){return iArrowType;}
	int GetDamage(void){return iDamage;}
	float GetChargeTime(void){return fChargeTime;}
	Line* GetBoundingLine(void){return pBoundingLine;}
	Base* GetOwner(void){return pOwner;}

	void Activate()
	{
		bActive=true;
		bCollidable=true;
		fCreationTimeStamp=Timer::AppTime();
	}

	void SetArrowAttributes(int iArwType, float fChgTime=0)
	{
		if(iArwType>ARROW_TYPE_NUM||iArwType<0)
			return;

		if(fChgTime>3)
			fChgTime=3;

		fChargeTime=fChgTime;

		iArrowType=iArwType;
		switch(iArwType)
		{
		case ARROW_TYPE_NORMAL:
			iDamage=20;
			fPower=fOriginalPower*2.5f;
			break;
		case ARROW_TYPE_FROST:
			iDamage=0;
			fPower=fOriginalPower;
			break;
		case ARROW_TYPE_FLAME:
			iDamage=0;
			fPower=fOriginalPower*1.5f;
			break;
		default:
			break;
		}
	}

	virtual bool CollidesWith (Base* other, Vector* vIntersection = 0)
	{
		if (!bCollidable || !other->bCollidable)
			return false;
		return other->CollidesWith (*pBoundingLine, vIntersection);
	}

	void Deactivate(void)
	{
		bActive=false;
		bCollidable=false;
		vVelocity.SetToZero();
		MoveTo(FAR_SPACE_STORE_POINT);
		vLastPos=Vector(FAR_SPACE_STORE_POINT);
	}

	void Update(void);
};

#endif