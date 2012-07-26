

// Defines a camera.

#ifndef StormForge_Matt_Camera
#define StormForge_Matt_Camera

#include "Base.h"
#include "Frustum.h"
#include "Direct3d.h"

extern int iWindowWidth, iWindowHeight;

#define DEFAULT_CAMERA_OFFSET 0,1, -4	// relates the position of the camera relative to the object it's attached to 
												// note that this is a GLOBAL offset, i.e. does not depend on the object's orientation!											
#define DEFAULT_FIRST_PERSON_OFFSET 0,.5f,.5f	// first person perspective

#define PREVIOUS_POSITIONS 10			//for soft attach mode
										//each position represents a delay of 1/50 of a second
#define DEFAULT_LOOKAHEAD 0.1f		//determines how much the camera responds to the attached object's movement in smart attach mode
									
// Define some camera modes.
enum CameraMode 
{
	CAMERA_SOFT_ATTACH,			//camera keeps track of teh object's previous positions for a realistic trailing behind effect
	CAMERA_HARD_ATTACH,			//this is the default camera mode. It places the camera behind the player
	CAMERA_SMART_ATTACH,		//looks ahead of the object it's attached to
	CAMERA_FIRST_PERSON,
	CAMERA_NONE,

	CAMERA_MODECOUNT
};


class Camera: public Base
{
private:
	int iCameraMode;		//soft attach or hard attach
	Matrix mLocal;			//stores the rotation of the camera in relation to the object it's attached to
							//the position of the mLocal matrix is used to adjust the camera's position according to the object's velocity
public:
	Vector vOffset;			//stores the offset of the camera in relation to the object it's attached to
							//I could have used the position vector of mLocal for this, but it would have been confusing
	Base* pAttachedObject;	//the object that the camera is viewing
	Matrix mPrevious[PREVIOUS_POSITIONS];
	float fLookAhead;	//determines how much the camera adjusts to the attached object's velocity. Applies to smart attach mode only. 
						// 0.0f -> the camera does not adjust to the object's movement at all, making smart attach equivalent to hard attach
						// 1.0f (default)-> offsets the camera by the attached object's velocity, essentially allowing the player to look ahead by a second
						// -2.0f-> camera's position trails behind the player's movement by two seconds
						// get the point??

	// Manage memory.
	#if _DEBUG
		ManageMemory(Camera, BASETYPE_CAMERA);			
	#endif

		// Define a base camera.
		Camera(): Base (BASETYPE_CAMERA)
		{
			mGlobal.SetToIdentity();
			// the camera faces the -z axis instead of the +z axis, so turn it around
			mGlobal.RotateY (180);

			GetRenderContext()->bDraw = false;
			pBoundingShape = new Frustum (VIEWFRUSTUM_ANGLE, VIEWFRUSTUM_NEARCLIP, VIEWFRUSTUM_FARCLIP, float(Direct3D::GetScreenWidth()) / Direct3D::GetScreenHeight());
			
			mLocal.SetToIdentity();
			iCameraMode = CAMERA_NONE;	
			this->bGravityAffected = false;
			this->bGroundClamped = false;
			pAttachedObject = 0;
			vOffset = Vector (DEFAULT_CAMERA_OFFSET);
			fLookAhead = DEFAULT_LOOKAHEAD;
		}

		void SetViewFrustum (float fAngle, float fNearClip, float fFarClip, float fAspectRatio) 
		{ 
			((Frustum*)pBoundingShape)->Set (fAngle, fNearClip, fFarClip, fAspectRatio); 
		}		

		// Camera actions inherited from the matrix itself
		void FaceDirection (Vector& vDirection) { mGlobal.FaceDirection(vDirection); }
		void LookAt (Vector& vPoint) { mGlobal.FaceDirection (vPoint - mGlobal.GetPos()); }
		void LookAt (Base *LookAt) { mGlobal.FaceDirection (LookAt->GetPos() - mGlobal.GetPos()); }
		
		// Camera collision
		// to check if an object is in the camera's view frustum, call this function
		bool CollidesWith (Base* other);
		

		// HardAttach
		//
		// Attaches a camera to an object. 
		// The camera's position will be fixed at a given offset (in global coordinates) away from the object
		// The camera will always look at the center of the object
		//
		// Input:
		//		AttachTo - the object to attach the camera to. If no object is specified or the object is NULL, the camera remains attached to the current object
		//
		// Note: if you do not like the position the camera is in after hard attaching it to an object,
		//		 you may change its position relative to the attached object by altering the vOffset variable
		void HardAttach (Base *AttachTo = NULL); 

		// SoftAttach
		//
		// Attaches a camera to an object, trailing behind the object a couple of frames
		//
		// Note: if you do not like the position the camera is in after hard attaching it to an object,
		//		 you may change its position relative to the attached object by altering the vOffset variable		
		void SoftAttach (Base* AttachTo = NULL);

		// FirstPersonAttach
		//
		// Attaches a camera to an object, trailing behind the object a couple of frames
		//
		// Note: if you do not like the position the camera is in after hard attaching it to an object,
		//		 you may change its position relative to the attached object by altering the vOffset variable		
		//
		// Note: I was contemplating about implementing first-person mode as a hard attach with a different offset.
		//		 However, I figured that I might need special functionality for first-person mode later on, so I made it a separate mode
		void FirstPersonAttach (Base* AttachTo = NULL);

		// Smart Attach
		//
		// Hard attaches to an object while adjusting the camera in the object's direction of movement
		//
		// Note: if you do not like the position the camera is in after hard attaching it to an object,
		//		 you may change its position relative to the attached object by altering the vOffset variable
		//
		// Note: if you think the camera is adjusting to the object's movement too much or too little, you can change this 
		//		 by altering the fSmartAttachFlexibility variable
		void SmartAttach (Base* AttachTo = NULL);

		// moves the OpenGL matrix in the opposite direction of the camera's
		// call this before drawing any objects
		void ApplyTransform(bool bRightHanded = false);

		// Draws the camera's location as a blue sphere in space
		virtual void Draw(void);


		// MOVEMENT
		//
		// the camera needs to rotate around the object it's attached to instead of rotating in place.
		// thus, we need to override rotation functions
		virtual void RotateX (float fDegrees);
		virtual void RotateY (float fDegrees);
		virtual void RotateZ (float fDegrees);
		virtual void RotateLocalX (float fDegrees);
		virtual void RotateLocalY (float fDegrees);
		virtual void RotateLocalZ (float fDegrees);

		// UPDATE
		//
		// repositions the camera according to its mode
		virtual void Update();
};

#endif