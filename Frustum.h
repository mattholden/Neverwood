////////////////////////////////////////////////////
// Frustum.h
//
// Implements a frustum bounding shape
// A frustum is a chopped off pyramid, and is meant to be used to represent a field of view.
// Another way to imagine a frustum is as two parallel squares (i.e. near and far viewing plane) connected to each other
// A frustum is represented by the following:
//		position (inherited from the base class): 
//			the position represents the ORIGIN of the view frustum. In other words, if the view frustum were extended to a pyramid, the position is that of the pyramid's tip.
//		forward vector (inherited from the base class):
//			the direction in which the frustum is facing
//		angle: represents the vertical angle of the view frustum. Should be >0 and <90
//		near clip: represents the distance of the frustum's near plane from its origin. Should be positive
//		far clip: represents the distance of the frustum's far plane from its origin. Should be greater than near clip
//		aspect ratio: the ratio of the width of a clipping plane to its height
//
// Collision detection is slightly different than with other bounding shapes:
// a frustum does not compute the point of intersection with another shape
////////////////////////////////////////////////////

#ifndef FRUSTUM_H
#define FRUSTUM_H

#include "BoundingShape.h"

class Frustum: public BoundingShape
{
private:
	// data members
	float fNearClip, fFarClip;
	float fAspectRatio;	// = width / height
	float fAngle;		// the vertical angle
	float fNearToFar;	//keeps track of fNearClip / fFarClip so you don't need to waste time computing it
	float fInvFarClip, fInvNearClip;	//keeps track of inverses so that you don't have to use expensive divisions
	//also uses fWidth, fHeight, and fDepth from the base class
	
public:
		// Manage memory
	#if _DEBUG
		ManageMemory(Frustum, SHAPE_FRUSTUM);
	#endif

	// CONSTRUCTORS
	Frustum ();
	Frustum (float angle, float nearClip = 0.1f, float farClip = 10, float aspectRatio = 1.0f);
	virtual ~Frustum(){}
	
	// MUTATORS
	void Set (float angle, float nearClip = 0.1f, float farClip = 10, float aspectRatio = 1.0f);
	void SetAngle (float angle);
	void SetNearClip (float nearClip);
	void SetFarClip (float farClip);
	void SetAspectRatio (float aspectRatio);

	// ACCESSORS
	float GetAngle() { return fAngle; }
	float GetNearClip() { return fNearClip; }
	float GetFarClip() { return fFarClip; }
	float GetAspectRatio() { return fAspectRatio; }
	float GetInvNearClip() { return fInvNearClip; }
	float GetInvFarClip() { return fInvFarClip; }

	// CONVENIENCE METHODS
	
	// calculates the vertices of the frustum's near plane and returns them as a Quad
	// the results are expressed in terms of global cooridinates
	// the order of the vertices is: near bottom left, near bottom right, near top right, near top left
	Quad GetNearPlane();

	// the order of the vertices is: far bottom right, far bottom left, far top left, far top right
	Quad GetFarPlane();

	// the order of the vertices is: far bottom left, near bottom left, near top left, far top left
	Quad GetLeftPlane();

	// the order of the vertices is: near bottom right, far bottom right, far top right, near top right
	Quad GetRightPlane();

	// the order of the vertices is: far bottom left, far bottom right, near bottom right, near bottom left
	Quad GetBottomPlane();

	// the order of the vertices is: near top left, near top right, far top right, far top left
	Quad GetTopPlane();


	Vector FarBottomLeft();	//returns the bottom left vertex of the far clipping plane
	Vector FarBottomRight();
	Vector FarTopLeft();
	Vector FarTopRight();
	Vector NearBottomLeft();	//returns the bottom left vertex of the near clipping plane
	Vector NearBottomRight();
	Vector NearTopLeft();
	Vector NearTopRight();


	// COLLISION DETECTION
	virtual bool CollidesWith (BoundingShape* other, Vector* vIntersection = NULL);
	virtual bool CollidesWith (Vector& vPoint);
	virtual bool CollidesWith (Line& line, Vector* vIntersection = 0);
	virtual bool CollidesWith (Triangle& triangle, Vector* vIntersection = 0);
	virtual bool CollidesWith (Quad& quad, Vector* vIntersection = 0);
	// CollidesWithVerticalLine
	// This is an optimization over the standard frustum to line collision
	// it assumes that the frustum rotates only around the global y axis
	// CollidesWithVerticalLine simply checks a line against the top of the frustum
	//
	// Input: any point on the line. It is implied that the line has infinite length and is vertical
	virtual bool CollidesWithVerticalLine (Vector& vPoint, Vector* vIntersection = NULL);

	//need a draw function for debugging
	virtual void Draw();
	
};

#endif