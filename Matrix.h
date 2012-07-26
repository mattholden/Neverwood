// Defines a matrix, which we'll be using just a few of in our game... 

#ifndef StormForge_Shared_Matrix
#define StormForge_Shared_Matrix

#include "vector.h"
#include "quaternion.h"		// for spherical linear interpolation, in case you're wondering

class Matrix
{

protected:

	
public:
	
	// Column major for vectoral access...
	float fMatrix[16];

	// Make this an identity matrix
	void SetToIdentity(void) 
	{
		memset(fMatrix, 0, 64);							//		Zero it out
		fMatrix[0] = 1.0f;										//		1 0 0 0 
		fMatrix[5] = 1.0f;										//		0 1 0 0 
		fMatrix[10] = 1.0f;									//		0 0 1 0 
		fMatrix[15] = 1.0f;									//		0 0 0 1
	}

	// default constructor: set to identity
	Matrix(void) { SetToIdentity(); } 

	// constructor: pass in an array of 16 (or more) floats
	Matrix(float* array) { memcpy (fMatrix, array, 64); }

	// constructor: create an array from a quaternion
	Matrix (Quaternion& quat, bool rightHanded = true);

	// create a quaternion from a matrix
	Quaternion GetQuaternion(bool rightHanded = true);

	// Copy a matrix
	Matrix(const Matrix &mCopy) { memcpy(fMatrix, mCopy.fMatrix, 64); } 
	Matrix operator=(const Matrix &mToCopy) { memcpy(fMatrix, mToCopy.fMatrix, 64); return *this; }

	// Spherical linear interpolation between two matrices
	//
	// Given a matrix m2, the invoking matrix rotates towards m2 by a given interpolation factor t:
	//		t=0		-> the invoking matrix does not change
	//		t=1		-> the invoking matrix becomes m2
	//		t=0.5	-> the invoking matrix rotates halfway to m2
	//		.....
	Matrix SLERP (Matrix& m2, float fInterpolationFactor, bool bRightHanded = true);

	///////////////////
	// ACCESSORS
	///////////////////
	
	//TO DO:
	//	the accessor functions don't work, and I don't know how to fix them because I'm not too familiar with contructors + operator overloading
	//	can anyone fix them?

	// Get the X axis of the matrix (with access to change it)
	Vector GetX(void) { return Vector (fMatrix); }

	// Get the Y axis of the matrix (with access to change it)
	Vector GetY(void) { return Vector (fMatrix + 4); }

	// Get the Z axis of the matrix (with access to change it)
	Vector GetZ(void) { return Vector ( fMatrix + 8); }

	// Get the position of the matrix (with access to change it)
	Vector GetPos(void) { return Vector ( fMatrix + 12); }

	// Get the nth row of the matrix
	Vector GetRow (int n) { return Vector (fMatrix[n], fMatrix[n+4], fMatrix[n+8]); }

	// Get the nth column of the matrix
	Vector GetCol (int n) { return Vector (fMatrix + 4*n); }

	// Get the nth row of the matrix, returned 4D
	Vector4D GetRow4D (int n) { return Vector4D (fMatrix[n], fMatrix[n+4], fMatrix[n+8], fMatrix[n + 12]); }

	// Get the nth column of the matrix, returned 4D
	Vector4D GetCol4D (int n) { return Vector4D (fMatrix + 4*n); }

	// Set the X axis of the matrix
	void SetX(Vector vNewX) { memcpy(fMatrix, vNewX.fComp, 12); }
	void SetY(Vector vNewY) { memcpy(fMatrix + 4, vNewY.fComp, 12); }
	void SetZ(Vector vNewZ) { memcpy(fMatrix + 8, vNewZ.fComp, 12); }
	void SetPos(Vector vNewPos) { memcpy(fMatrix + 12, vNewPos.fComp, 12); }

	void SetX(float fX, float fY, float fZ) { fMatrix[0] = fX; fMatrix[1] = fY; fMatrix[2] = fZ; }
	void SetY(float fX, float fY, float fZ) { fMatrix[4] = fX; fMatrix[5] = fY; fMatrix[6] = fZ; }
	void SetZ(float fX, float fY, float fZ) { fMatrix[8] = fX; fMatrix[9] = fY; fMatrix[10] = fZ; }
	void SetPos(float fX, float fY, float fZ) { fMatrix[12] = fX; fMatrix[13] = fY; fMatrix[14] = fZ; }

	
	// Check orthogonality of all axes
	bool CheckOrthoAxes(void) { return (GetX().IsOrthogonalTo(GetY()) && GetY().IsOrthogonalTo(GetZ())); }

	// Ensure normality of all axes (except position)
	void NormalizeIfNeeded(void) 
	{
		if (!GetX().IsNormalized()) SetX(GetX().Normalize());
		if (!GetY().IsNormalized()) SetY(GetY().Normalize());
		if (!GetZ().IsNormalized()) SetZ(GetZ().Normalize());
	}	

	/////////////////////////////////////
	// Invert
	// 
	// Inverts the 4x4 matrix
	// Returns true if the matrix is invertible, false otherwise
	//////////////////////////////////////
	bool Invert();

	//////////////////////////////////////
	// Transpose
	// 
	// flips the matrix about its major axis
	////////////////////////////////////////
	Matrix Transpose();

	// MATRIX MULTIPLY
	
	// TO DO: operator* doesn't work properly because it returns a local variable. Can anyone fix it?
	Matrix operator* (Matrix& other);
	Matrix Multiply (Matrix& other) { return (*this) * other; }
	void operator*= (Matrix& other) { (*this) = (*this) * other; }

	////////////////////////////
	// TRANSFORMATIONS
	////////////////////////////

	// Translate
	// moves the matrix's position by the given amount
	// the displacement is in terms of GLOBAL coordinates
	void Translate (float x, float y, float z) { fMatrix[12] += x; fMatrix[13] += y; fMatrix[14] += z; }
	void Translate (Vector& vTranslate) 	{ Translate (vTranslate.fComp[0], vTranslate.fComp[1], vTranslate.fComp[2]); }
	void TranslateLocal (float x, float y, float z);
	void TranslateLocal (Vector& vTranslate);
		
	
	// Scale
	void Scale (float fScale)
	{
		for (int i=0; i<16; i++)
			fMatrix[i] *= fScale;
	}

	//////////////////////////////////////////////////////
	// ROTATION
	//
	// The following functions are used to rotate the matrix by a given amount around a variety of different axes.
	// The amount of rotation is given in DEGREES
	//////////////////////////////////////////////////////
	
	// rotate around the matrix's x,y, or z axes
	void RotateX (float fDegrees);
	void RotateY (float fDegrees);
	void RotateZ (float fDegrees);

	// rotate around the global x,y, or z axes
	void RotateGlobalX (float fDegrees);
	void RotateGlobalY (float fDegrees);
	void RotateGlobalZ (float fDegrees);
	
	// rotate around an arbitrary axis
	// CAUTION: the axis needs to be unit length, or the rotation will scale the object, too!
	void RotateGlobal (Vector& vAxis, float fDegrees);	

	// algorithms
	
	// Makes the matrix's forward vector look in the same direction as vDirection
	// Also, the matrix will be orthonormal after this call
	void FaceDirection (Vector& vDirection);	
	void LookAt (Vector& vPoint) { FaceDirection (vPoint - GetPos()); }
	
	// rotates the matrix towards the given point at a given angle
	// if the matrix rotates past the point, it will look at the point instead
	void TurnTo (Vector& vPoint, float fAngle);
	// rotates the matrix around the global y axis towards the given point
	void TurnToHorizontal (Vector& vPoint, float fAngle);
	// rotates the matrix around its local x axis towards the given point
	void TurnToVertical (Vector& vPoint, float fAngle);

	// ADDED: Matt 5-16-04
	// Rotation/translation stuff added for animation
	void Matrix::SetRotationAngles(float *fAngles, bool bRadians = true);
	
	inline void Matrix::InverseTranslate(Vector *pVector)
	{
		pVector->fComp[0] = pVector->fComp[0] * fMatrix[0] + pVector->fComp[1] * fMatrix[1] + pVector->fComp[2] * fMatrix[2];
		pVector->fComp[1] = pVector->fComp[0] * fMatrix[4] + pVector->fComp[1] * fMatrix[5] + pVector->fComp[2] * fMatrix[6];
		pVector->fComp[2] = pVector->fComp[0] * fMatrix[8] + pVector->fComp[1] * fMatrix[9] + pVector->fComp[2] * fMatrix[10];		
	}

	inline void Matrix::InverseRotate(Vector *pVector)
	{
		pVector->fComp[0] = fMatrix[12];
		pVector->fComp[1] -= fMatrix[13];
		pVector->fComp[2] -= fMatrix[14];
	}

	float *Matrix::RowMajor(void)
	{
		return Transpose().fMatrix;
	}

};

#endif
