// Defines a Vector primitive.
// No collision done here; that will be handled in the Shapes which will contain a ToVector() and/or ToRay()  routine(s).

#ifndef StormForge_Matt_Vector
#define StormForge_Matt_Vector

// Microsoft-specific compiler optimization: passes parameters via registers rather than call stack
#define FAST __fastcall

// For sqrtf()
#include <math.h>

// for memcpy()
#include <memory.h>

// Included to gain access to helper function FloatEqual() and RandFloat()
#include "MathUtils.h"

struct D3DXVECTOR3;

class Vector4D;

class Vector
{

	public:

	// Components
	float fComp[3];

	// default constructor
	Vector(void) { };
	
	// Basic constructor, components
	Vector(float fX, float fY, float fZ) { fComp[0] = fX; fComp[1] = fY; fComp[2] = fZ; }

	// Basic constructor, array of floats
	Vector(float *pFloats) { memcpy (fComp, pFloats, 12);   }

	// Copy constructor
	Vector(const Vector &vCopy) { memcpy (fComp, vCopy.fComp, 12);   }

	// Assignment
	Vector operator=	(const Vector &vCopy) { memcpy(fComp, vCopy.fComp, 12); return *this;   }

	// Destructor
	~Vector(void) 	{ 	 }	

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Operator Overloads
	////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Add two vectors
	Vector FAST		operator+	(const Vector &vAdd)		{ return Vector(fComp[0] + vAdd.fComp[0], fComp[1] + vAdd.fComp[1], fComp[2] + vAdd.fComp[2]); }
	// Add two vectors and store the results in the calling Vector instance
	void	FAST		operator+=	(const Vector &vAdd)		{ fComp[0] += vAdd.fComp[0]; fComp[1] += vAdd.fComp[1]; fComp[2] += vAdd.fComp[2]; }

	// Subtract two vectors
	Vector 	FAST 	operator-	(const Vector &vSub2)		
	{ 
		return Vector((fComp[0] - vSub2.fComp[0]),(fComp[1] - vSub2.fComp[1]), (fComp[2] - vSub2.fComp[2]));
	}
	// Subtract two vectors  and store the results in the calling Vector instance
	void	FAST		operator-=	(const Vector &vSub)		{ fComp[0] -= vSub.fComp[0]; fComp[1] -= vSub.fComp[1]; fComp[2] -= vSub.fComp[2]; }

	// Multiply by a scalar ("Scale" the vector")
	Vector FAST		operator*		(float fScalar)					{ return Vector(fComp[0] * fScalar, fComp[1] * fScalar, fComp[2] * fScalar); }
	// Multiply by a scalar ("Scale" the vector") and store the results in the calling Vector instance
	void FAST			operator*=	(float fScalar)					{ fComp[0] *= fScalar; fComp[1] *= fScalar; fComp[2] *= fScalar; }

	// Compare two vectors
	// Two vectors are equal if all their components are equal, give or take a small error for floating point precision loss

	// COMMENT:
	// My god, I can't believe I forgot this one. - Matt 4/13/04
	bool FAST operator== (Vector& other) { return FloatEqual (fComp[0], other.fComp[0]) && FloatEqual (fComp[1], other.fComp[1])	 && FloatEqual (fComp[2], other.fComp[2]);}	
	bool FAST operator!= (Vector& other) { return !(FloatEqual (fComp[0], other.fComp[0]) && FloatEqual (fComp[1], other.fComp[1])	 && FloatEqual (fComp[2], other.fComp[2]));}	

	// Scale to a scalar ("Set Scale" the vector") 
	// Note: /= 1.0f can be used as an operator shortcut to achieve the same results as NormalizeAndStore().
	// Note 2: This involves a normalize and is expensive, so use sparingly
	Vector FAST		operator/		(float fScalar)					{ return (!(*this)) * fScalar; }	

	// Scale to a scalar ("Set Scale" the vector")  and store the results in the calling Vector instance
	// Note: /= 1.0f can be used as an operator shortcut to achieve the same results as NormalizeAndStore().
	// Note 2: This involves a normalize and is expensive, so use sparingly
	void	FAST		operator/=	(float fScalar)					{ Vector(!(*this) * fScalar); }

	// Negate a vector
	Vector	FAST operator-		(void)								{ return Vector(fComp[0] * -1.0f, fComp[1] * -1.0f, fComp[2] * -1.0f); }
	
	// Dot product.	
	float		FAST operator*		(const Vector &vDot)		{ return (fComp[0] * vDot.fComp[0]) + (fComp[1] * vDot.fComp[1]) + (fComp[2] * vDot.fComp[2]); }

	// Project vector vProject onto this vector
	Vector FAST 	operator%	(const Vector &vProj)		
	{ 
		Vector vNormalized = vProj.Normalize();
		return vNormalized * (vNormalized * (*this));
	}

	// Project vector vProject onto this vector and store the results in the calling Vector instance
	void	FAST 	operator%=	(const Vector &vProj)		{ 	Vector((!(*this)) * ((*this) * vProj));	}

	// Cross product		(AyBz - AzBy) = x; (AzBx - AxBz) = y; (AxBy - AyBx) = z;
	Vector FAST  operator&	(const Vector &vCross)	
	{	
		return Vector( 
			/* X Component: */  ((fComp[1] * vCross.fComp[2]) - (fComp[2] * vCross.fComp[1])),
			/* Y Component: */  ((fComp[2] * vCross.fComp[0]) - (fComp[0] * vCross.fComp[2])),
			/* Z Component: */  ((fComp[0] * vCross.fComp[1]) - (fComp[1] * vCross.fComp[0])));
	}
	
	// Cross product  and store the results in the calling Vector instance
	void	FAST 	operator&=		(const Vector &vCross)	
	{	
			Vector vTemp;
			memcpy(&vTemp, this, 12);

			/* X Component: */	fComp[0]	= ((vTemp.fComp[1] * vCross.fComp[2]) - (vTemp.fComp[2] * vCross.fComp[1]));
			/* Y Component: */ fComp[1]	= ((vTemp.fComp[2] * vCross.fComp[0]) - (vTemp.fComp[0] * vCross.fComp[2]));
			/* Z Component: */ fComp[2]	= ((vTemp.fComp[0] * vCross.fComp[1]) - (vTemp.fComp[1] * vCross.fComp[0]));
	}

	// Magnitude (with sqrt)
	float		operator~(void) { return sqrtf(	(fComp[0] * fComp[0])	+ (fComp[1] * fComp[1]) + (fComp[2] * fComp[2])); }

	// Normalize 
	Vector operator!(void) const
	{
		float fLength = sqrtf((fComp[0] * fComp[0])	+ (fComp[1] * fComp[1]) + (fComp[2] * fComp[2]));
		if (FloatEqual (fLength, 0))
			return Vector (0,0,0);
		float fMag = 1.0f / fLength;
		return Vector(fComp[0] * fMag, fComp[1] * fMag, fComp[2] * fMag);
	}

	// Get an individual component
	float operator[](int iIndex) { return (iIndex >= 0 && iIndex < 3) ? fComp[iIndex] : 0; }
	
	// Long function name equivalents of operators (slower than operators)

	// Add two vectors
	Vector		FAST Add						(const Vector &vAdd)		{ return (*this) + vAdd; }
	// Add two vectors and store the results in the calling Vector instance
	void			FAST AddAndStore		(const Vector &vAdd)		{ (*this) += vAdd; }
	// Subtract two vectors
	Vector		FAST Subtract				(const Vector &vSub)		{ return (*this) - vSub; }
	// Subtract two vectors and store the results in the calling Vector instance
	void			FAST SubtractAndStore	(const Vector &vSub)		{ (*this) -= vSub; }
	// Dot product
	float			FAST Dot						(const Vector &vDot)		{ return (*this) * vDot; }
	// Project vProj onto the vector object
	Vector		FAST Project					(const Vector &vProj)		{ return (*this) % vProj; }
	// Project vProj onto the vector object and store the results in the calling Vector instance
	void			FAST ProjectAndStore	(const Vector &vProj)		{ (*this) %= vProj; }
	// Cross product
	Vector		FAST Cross					(const Vector &vCross)	{ return (*this) & vCross; }
	// Cross product and store the results in the calling Vector instance
	void			FAST CrossAndStore		(const Vector &vCross)	{ (*this) &= vCross; }
	// Scale by a scalar
	Vector		FAST Scale					(float fScale)						{ return (*this) * fScale; }
	// Scale by a scalar and store the results in the calling Vector instance
	void			FAST ScaleAndStore		(float fScale)						{ (*this) *= fScale; }
	// Scale to a scalar length
	Vector		FAST SetScale				(float fScale)						{ return (*this) / fScale; }
	// Scale to a scalar length and store the results in the calling Vector instance
	void			FAST SetScaleAndStore(float fScale)						{ (*this) /= fScale; }
	// Get vector magnitude (uses sqrtf())
	float			Length					(void)								{ return ~(*this); }
	// Normalize a vector 
	Vector		Normalize				(void)		const					{ return !(*this); }  
	// Negate a vector
	Vector		Negate					(void)								{ return -(*this); }


	//////////////////////////////////////////////////////////////////////////////////////////
	// Functionality NOT covered by operators:
	//////////////////////////////////////////////////////////////////////////////////////////
	
	// Zero out the vector.
	void			SetToZero				(void)	{ memset(fComp, 0, 12); }

	// See if it is zeroed out
	bool			IsZeroVector			(void)	{ return FloatEqual( (fComp[0] + fComp[1] + fComp[2]), 0.0f ); }

	// Normalize, storing the normal in 'this'
	void			NormalizeAndStore	(void)	{ *this /= 1.0f; }

	// Get 'relative' magnitude (without doing sqrt()...)
	float			LengthSquared		(void)	{ return (fComp[0] * fComp[0])	+ (fComp[1] * fComp[1]) + (fComp[2] * fComp[2]); }

	// Test if a vector is normalized. This uses "Relative" distance, since that should still be one, since 1 squared = 1.
	bool			IsNormalized			(void)	{ return FloatEqual((fComp[0] + fComp[1] + fComp[2]), 1.0f); }

	// Test if a vector is orthogonal to this one.
	bool	FAST IsOrthogonalTo		(const Vector &vOrtho)	{	return FloatEqual((*this) * vOrtho, 0.0f); }

	// Find the angle between two vectors, measured in DEGREES
	// The angle will always be between 0 and 180
	// pretty expensive computationally, though
	float			AngleTo (Vector& vOther)
	{
		return MATH_TRIG_RADIANS_TO_DEGREES * acosf (*this * vOther / ( ~(*this) * ~vOther));
	}


	/*
	// This is added to quickly get random directions within a range for things like the particle engine.
	// Values will have 3 digits of precision.
	static Vector GetRandomVector(float fXMin, float fXMax, float fYMin, float fYMax, float fZMin, float fZMax, int iPrecision = 3)
	{
		return Vector(RandomFloat(fXMin, fXMax), RandomFloat(fYMin, fYMax), RandomFloat(fZMin, fZMax));
	}

	static Vector GetRandomVector(Vector &vMin, Vector &vMax, int iPrecision = 3)
	{
		return Vector(
			RandomFloat(vMin.fComp[0], vMax.fComp[0]),
			RandomFloat(vMin.fComp[1], vMax.fComp[1]),
			RandomFloat(vMin.fComp[2], vMax.fComp[2]));
	}
	*/

	};


class Vector4D
{
public:

	float fComp[4];

	Vector4D(void) { };

	// Make a 4d vector out of a 3d one
	Vector4D(const Vector &Vector3D, char cBottomRow = 0) 
	{
		memcpy(fComp, Vector3D.fComp, 12);
		fComp[3] = cBottomRow;
	}	

	// Make a 3d out of a 4d
	Vector Make3D(void) { return Vector(fComp[0], fComp[1], fComp[2]); }

	// Basic constructor, components
	Vector4D(float fX, float fY, float fZ, float fW = 1.0f) { fComp[0] = fX; fComp[1] = fY; fComp[2] = fZ; fComp[3] = fW; }

	// Basic constructor, array of floats
	Vector4D(float *pFloats) { memcpy (fComp, pFloats, 16);   }

	// Copy constructor
	Vector4D(const Vector4D &vCopy) { memcpy (fComp, vCopy.fComp, 16);   }

	// Assignment
	Vector4D operator=	(const Vector4D &vCopy) { memcpy(fComp, vCopy.fComp, 16); return *this;   }
	Vector4D operator=	(const Vector &vCopy) { memcpy(fComp, vCopy.fComp, 12); fComp[3] = 1.0f;  return *this;   }

	// Destructor
	~Vector4D(void) 	{ 	 }	

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Operator Overloads
	////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Add two vectors
	Vector4D FAST		operator+	(const Vector &vAdd)		{ return Vector4D(fComp[0] + vAdd.fComp[0], fComp[1] + vAdd.fComp[1], fComp[2] + vAdd.fComp[2], fComp[3]); }
	void	FAST				operator+=	(const Vector &vAdd)		{ fComp[0] += vAdd.fComp[0]; fComp[1] += vAdd.fComp[1]; fComp[2] += vAdd.fComp[2];  }
	Vector4D FAST		operator+	(const Vector4D &vAdd)	{ return Vector4D(fComp[0] + vAdd.fComp[0], fComp[1] + vAdd.fComp[1], fComp[2] + vAdd.fComp[2], fComp[3] + vAdd.fComp[3]); }
	void	FAST				operator+=	(const Vector4D &vAdd)	{ fComp[0] += vAdd.fComp[0]; fComp[1] += vAdd.fComp[1]; fComp[2] += vAdd.fComp[2]; fComp[3] += vAdd.fComp[3]; }

	// Subtract two vectors	
	Vector4D 	FAST 	operator-		(const Vector &vSub2)		{ return Vector4D((fComp[0] - vSub2.fComp[0]),(fComp[1] - vSub2.fComp[1]), (fComp[2] - vSub2.fComp[2]), fComp[3]);	}
	void	FAST				operator-=	(const Vector &vSub)		{ fComp[0] -= vSub.fComp[0]; fComp[1] -= vSub.fComp[1]; fComp[2] -= vSub.fComp[2]; }
	Vector4D 	FAST 	operator-		(const Vector4D &vSub2)	{ return Vector4D((fComp[0] - vSub2.fComp[0]),(fComp[1] - vSub2.fComp[1]), (fComp[2] - vSub2.fComp[2]), fComp[3] - vSub2.fComp[3]);	}
	void	FAST				operator-=	(const Vector4D &vSub)	{ fComp[0] -= vSub.fComp[0]; fComp[1] -= vSub.fComp[1]; fComp[2] -= vSub.fComp[2]; fComp[3] -= vSub.fComp[3];}

	// Multiply by a scalar ("Scale" the vector")
	Vector4D FAST		operator*		(float fScalar)					{ return Vector4D(fComp[0] * fScalar, fComp[1] * fScalar, fComp[2] * fScalar, fComp[3] * fScalar); }
	// Multiply by a scalar ("Scale" the vector") and store the results in the calling Vector instance
	void FAST				operator*=	(float fScalar)					{ fComp[0] *= fScalar; fComp[1] *= fScalar; fComp[2] *= fScalar; fComp[3] *= fScalar;}

	// Compare two vectors
	// Two vectors are equal if all their components are equal, give or take a small error for floating point precision loss

	// COMMENT:
	// My god, I can't believe I forgot this one. - Matt 4/13/04
	bool FAST operator== (Vector4D& other) { return FloatEqual (fComp[0], other.fComp[0]) && FloatEqual (fComp[1], other.fComp[1])	 && FloatEqual (fComp[2], other.fComp[2]) && FloatEqual (fComp[3], other.fComp[3]);}	
	bool FAST operator!=	(Vector4D& other) { return !(FloatEqual (fComp[0], other.fComp[0]) && FloatEqual (fComp[1], other.fComp[1])	 && FloatEqual (fComp[2], other.fComp[2]) && FloatEqual (fComp[3], other.fComp[3]));}	
	bool FAST operator== (Vector& other) { return FloatEqual (fComp[0], other.fComp[0]) && FloatEqual (fComp[1], other.fComp[1])	 && FloatEqual (fComp[2], other.fComp[2]);}	
	bool FAST operator!= (Vector& other) { return !(FloatEqual (fComp[0], other.fComp[0]) && FloatEqual (fComp[1], other.fComp[1])	 && FloatEqual (fComp[2], other.fComp[2]));}	

	// Scale to a scalar ("Set Scale" the vector") 
	// Note: /= 1.0f can be used as an operator shortcut to achieve the same results as NormalizeAndStore().
	// Note 2: This involves a normalize and is expensive, so use sparingly
	Vector4D FAST		operator/		(float fScalar)					{ return (!(*this)) * fScalar; }	

	// Scale to a scalar ("Set Scale" the vector")  and store the results in the calling Vector instance
	// Note: /= 1.0f can be used as an operator shortcut to achieve the same results as NormalizeAndStore().
	// Note 2: This involves a normalize and is expensive, so use sparingly
	void	FAST		operator/=	(float fScalar)					{ this->NormalizeAndStore(); *this *= fScalar; }

	// Negate a vector
	Vector4D	FAST operator-		(void)								{ return Vector4D(fComp[0] * -1.0f, fComp[1] * -1.0f, fComp[2] * -1.0f, fComp[3] * -1.0f);}
	
	// Dot product.	 Ignores [3]
	float		FAST operator*		(const Vector &vDot)		{ return (fComp[0] * vDot.fComp[0]) + (fComp[1] * vDot.fComp[1]) + (fComp[2] * vDot.fComp[2]); }

	// Project vector vProject onto this vector - ignores [3]
	Vector FAST 	operator%	(const Vector &vProj)		
	{ 
		Vector vNormalized = vProj.Normalize();
		return vNormalized * (vNormalized * this->Make3D());
	}

	// Project vector vProject onto this vector and store the results in the calling Vector instance - ignores [3]
	void	FAST 	operator%=	(const Vector &vProj)		{ 	Vector4D((!(*this)) * ((*this) * vProj));	}

	// Cross product		(AyBz - AzBy) = x; (AzBx - AxBz) = y; (AxBy - AyBx) = z; Ignores [3]
	Vector FAST  operator&	(const Vector &vCross)	
	{	
		return Vector( 
			/* X Component: */  ((fComp[1] * vCross.fComp[2]) - (fComp[2] * vCross.fComp[1])),
			/* Y Component: */  ((fComp[2] * vCross.fComp[0]) - (fComp[0] * vCross.fComp[2])),
			/* Z Component: */  ((fComp[0] * vCross.fComp[1]) - (fComp[1] * vCross.fComp[0])));
	}
	
	// Cross product  and store the results in the calling Vector instance
	void	FAST 	operator&=		(const Vector &vCross)	
	{	
			Vector vTemp;
			memcpy(&vTemp, this, 12);

			/* X Component: */	fComp[0]	= ((vTemp.fComp[1] * vCross.fComp[2]) - (vTemp.fComp[2] * vCross.fComp[1]));
			/* Y Component: */ fComp[1]	= ((vTemp.fComp[2] * vCross.fComp[0]) - (vTemp.fComp[0] * vCross.fComp[2]));
			/* Z Component: */ fComp[2]	= ((vTemp.fComp[0] * vCross.fComp[1]) - (vTemp.fComp[1] * vCross.fComp[0]));
	}

	// Magnitude (with sqrt)
	float		operator~(void) { return sqrtf(	(fComp[0] * fComp[0])	+ (fComp[1] * fComp[1]) + (fComp[2] * fComp[2])); }

	// Normalize 
	Vector4D operator!(void) const
	{
		float fMag = 1.0f / sqrtf((fComp[0] * fComp[0])	+ (fComp[1] * fComp[1]) + (fComp[2] * fComp[2]));
		return Vector4D(fComp[0] * fMag, fComp[1] * fMag, fComp[2] * fMag, fComp[3]);
	}

	// Get an individual component
	float operator[](int iIndex) { return (iIndex >= 0 && iIndex < 4) ? fComp[iIndex] : 0; }
	

	//////////////////////////////////////////////////////////////////////////////////////////
	// Functionality NOT covered by operators:
	//////////////////////////////////////////////////////////////////////////////////////////
	
	// Zero out the vector. (0,0,0,1)
	void			SetToZero				(void)	{ memset(fComp, 0, 12); fComp[3] = 1.0f; }

	// Zero out the vector. (0,0,0,0)
	void			SetToZeroXYZW					(void)	{ memset(fComp, 0, 16); }

	// See if it is zeroed out
	bool			IsZeroVector			(void)	{ return FloatEqual( (fComp[0] + fComp[1] + fComp[2] + fComp[3]), 0.0f ); }

	// See if it is zeroed out except for W
	bool			IsZeroVectorXYZ		(void)	{ return FloatEqual( (fComp[0] + fComp[1] + fComp[2]), 0.0f ); }

	// Normalize, storing the normal in 'this'
	void			NormalizeAndStore	(void)	{ Vector4D(!(*this)); }

	// Get 'relative' magnitude (without doing sqrt()...)
	float			LengthSquared		(void)	{ return (fComp[0] * fComp[0])	+ (fComp[1] * fComp[1]) + (fComp[2] * fComp[2]); }

	// Test if a vector is normalized. This uses "Relative" distance, since that should still be one, since 1 squared = 1.
	bool			IsNormalized			(void)	{ return FloatEqual((fComp[0] + fComp[1] + fComp[2]), 1.0f); }

	// Test if a vector is orthogonal to this one.
	bool	FAST IsOrthogonalTo		(const Vector &vOrtho)	{	return FloatEqual((*this) * vOrtho, 0.0f); }

	// Find the angle between two vectors, measured in DEGREES
	// The angle will always be between 0 and 180
	// pretty expensive computationally, though
	float			AngleTo (Vector& vOther)
	{
		return MATH_TRIG_RADIANS_TO_DEGREES * acosf (*this * vOther / ( ~(*this) * ~vOther));
	}

	/*
	// This is added to quickly get random directions within a range for things like the particle engine.
	// Values will have 3 digits of precision.
	static Vector4D GetRandomVector(float fXMin, float fXMax, float fYMin, float fYMax, float fZMin, float fZMax, float fWMin, float fWMax, int iPrecision = 3)
	{
		return Vector4D(RandomFloat(fXMin, fXMax), RandomFloat(fYMin, fYMax), RandomFloat(fZMin, fZMax, iPrecision), RandFloat(fWMin, fWMax, iPrecision));
	}

	static Vector4D GetRandomVector(Vector4D &vMin, Vector4D &vMax, int iPrecision = 3)
	{
		return Vector4D(
			RandFloat(vMin.fComp[0], vMax.fComp[0], iPrecision),
			RandFloat(vMin.fComp[1], vMax.fComp[1], iPrecision),
			RandFloat(vMin.fComp[2], vMax.fComp[2], iPrecision),
			RandFloat(vMin.fComp[3], vMax.fComp[3], iPrecision));
	}
	*/
};



#endif

