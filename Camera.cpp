

#include "camera.h"
#include "matrix.h"
#include "Game.h"
#include "Direct3D.h"
#include "Shapes.h"


// Hard attach to an object
void Camera::HardAttach(Base *AttachTo)
{
	iCameraMode = CAMERA_HARD_ATTACH;
	vOffset = Vector (DEFAULT_CAMERA_OFFSET);
	if (AttachTo)
		pAttachedObject = AttachTo;
	mGlobal = pAttachedObject->mGlobal;
}

// Soft attach to an object
void Camera::SoftAttach(Base* AttachTo)
{
	iCameraMode = CAMERA_SOFT_ATTACH;
	vOffset = Vector (DEFAULT_CAMERA_OFFSET);
	fStateTime = 0;
	if (AttachTo)
		pAttachedObject = AttachTo;
	for (int i=0; i<PREVIOUS_POSITIONS; i++)
		mPrevious[i] = pAttachedObject->mGlobal;
	mGlobal = pAttachedObject->mGlobal;
}

// first person attach
void Camera::FirstPersonAttach(Base *AttachTo)
{
	iCameraMode = CAMERA_FIRST_PERSON;
	vOffset = Vector (DEFAULT_FIRST_PERSON_OFFSET);
	if (AttachTo)
		pAttachedObject = AttachTo;
	mGlobal = pAttachedObject->mGlobal;
}

// smart attach to an object
void Camera::SmartAttach (Base* AttachTo)
{
	iCameraMode = CAMERA_SMART_ATTACH;
	vOffset = Vector (DEFAULT_CAMERA_OFFSET);
	if (AttachTo)
		pAttachedObject = AttachTo;
	mGlobal = pAttachedObject->mGlobal;
}

// move the OpenGL matrix against the camera's
void Camera::ApplyTransform(bool bRightHanded)
{
	
	D3DXMATRIXA16 mView;
	Vector vLookAt = mGlobal.GetPos() + mGlobal.GetZ();

	if (bRightHanded)
		D3DXMatrixLookAtRH(&mView, 
										  (D3DXVECTOR3*)(&mGlobal.GetPos().fComp),
										  (D3DXVECTOR3*)(&vLookAt.fComp),
										  (D3DXVECTOR3*)(&mGlobal.GetY().fComp));
	else
		D3DXMatrixLookAtLH(&mView, 
										  (D3DXVECTOR3*)(&mGlobal.GetPos().fComp),
										  (D3DXVECTOR3*)(&vLookAt.fComp),
										  (D3DXVECTOR3*)(&mGlobal.GetY().fComp));

	Direct3D::GetMatrixStack()->LoadMatrix((D3DXMATRIX*)&mView);
	Direct3D::GetMatrixStack()->ApplyToView();
}

void Camera::RotateX (float fDegrees)
{
	mGlobal.RotateGlobalX (-fDegrees);
}

void Camera::RotateY (float fDegrees)
{
	mGlobal.RotateGlobalY (-fDegrees);
}

void Camera::RotateZ (float fDegrees)
{
	mGlobal.RotateGlobalZ (-fDegrees);
}

void Camera::RotateLocalX (float fDegrees)
{
	mGlobal.RotateX (fDegrees);
}

void Camera::RotateLocalY (float fDegrees)
{
	mGlobal.RotateY (fDegrees);
}

void Camera::RotateLocalZ (float fDegrees)
{
	mGlobal.RotateZ (fDegrees);
}

bool Camera::CollidesWith (Base* other)
{
	if (!other->GetRenderContext()->bDraw)
		return false;

	return pBoundingShape->CollidesWith (other->pBoundingShape);
}

	// Draws the camera's location as a blue sphere in space
void Camera::Draw(void)
{
	if (GetRenderContext()->bDraw)
	{
		Shapes::SetColor(0,0,1);
		Shapes::Sphere(&mGlobal, 0.25f);
	}
}

void Camera::Update()
{
	if (!bUpdate)
		return;

	fStateTime += Timer::GetElapsedTime();

	if (!pAttachedObject)
		return;

	// MOVE CAMERA TO THE OBJECT IT'S ATTACHED TO

	// hard attach: reposition the camera WRT the object it's attached to		
	switch (iCameraMode)
	{

	case CAMERA_HARD_ATTACH:
		MoveTo (pAttachedObject->GetPos());
	break;

	// soft attach: update previous positions 
	//				attach to the oldest stored position
	case CAMERA_SOFT_ATTACH:
		// fix camera motion to 50 FPS
		if (fStateTime >= 0.02f)
		{
			for (int i = PREVIOUS_POSITIONS - 1; i>0; i--)
				mPrevious [i] = mPrevious[i-1];
		
			mPrevious[0] = pAttachedObject->mGlobal;
			MoveTo  (mPrevious [PREVIOUS_POSITIONS-1].GetPos());

			fStateTime -= 0.02f;
		}
	break;

	// smart attach: offset the camera in the direction of the attached object's movement
	case CAMERA_SMART_ATTACH:
		MoveTo (pAttachedObject->GetPos());
		mLocal.SetPos ( pAttachedObject->vVelocity * Timer::GetElapsedTime() * 1000.0f * fLookAhead);
	break;

	case CAMERA_FIRST_PERSON:
		MoveTo (pAttachedObject->GetPos());
		mLocal = pAttachedObject->mGlobal;
		mLocal.SetPos (0,0,0);
	break;
	}


	// OFFSET THE CAMERA APPROPRIATELY
	Vector vOff (0,0,0);
	vOff += mGlobal.GetX() * vOffset[0];
	vOff += mGlobal.GetY() * vOffset[1];
	vOff += mGlobal.GetZ() * vOffset[2];	
	MoveBy (vOff);
	MoveBy (mLocal.GetPos());

	// don't forget to move the camera's frustum along with the camera
	pBoundingShape->mGlobal = mGlobal;
	//pBoundingShape->MoveTo (GetPos());	
}

