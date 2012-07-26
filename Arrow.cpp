#include "Arrow.h"
DXModel *Arrow::pArrowModel = 0;

void Arrow::Update(void)
{
	// CHANGED:
	// not updating passive arrows anymore
	if(!bActive)
		return;

	if(Timer::AppTime() - fCreationTimeStamp > 3.0f)
	{
		Deactivate();
		return;
	}

	pBoundingShape->mGlobal = mGlobal;

	switch(iArrowType)
	{
	case ARROW_TYPE_NORMAL:
		//pBoundingShape->SetColor(0.75f, 0.75f, 0.75f);
		break;
	case ARROW_TYPE_FLAME:
		//pBoundingShape->SetColor(0.75f, 0.0f, 0.0f);
		break;
	case ARROW_TYPE_FROST:
		//pBoundingShape->SetColor(0.25f, 0.25f, 1.0f);
		break;
	};

//	vVelocity=vVelocity/fPower;

	//redefine line bounding shape
	pBoundingLine->vBegin=vLastPos;
	pBoundingLine->vEnd=GetPos();

	vLastPos=GetPos();

	MoveBy(vVelocity*Timer::GetElapsedTime());
}