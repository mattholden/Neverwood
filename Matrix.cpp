#include "Matrix.h"
#include "MathTrigTable.h"
#include "MathUtils.h"

// macro used to acces matrix in a 2D way
#define M(row,col) ( fMatrix [row + 4*col] )

// Constructor
Matrix::Matrix (Quaternion& quat, bool rightHanded)
{
	this->SetToIdentity();

	// extract the angle: 
	//		w coordinate = cos (angle/2), in radians
	float fHalfAngle = acosf (quat.w);
	float fAngle = 2 * fHalfAngle * MATH_TRIG_RADIANS_TO_DEGREES;
	if (!rightHanded)
		fAngle *= -1;

	// rotate
	RotateGlobal (Vector (quat.x, quat.y, quat.z).Normalize(), fAngle);
}

Quaternion Matrix::GetQuaternion (bool rightHanded)
{
	float w,x,y,z;

	float fDiagonal = fMatrix[0] + fMatrix[5] + fMatrix[10] + 1.0f;
	float fScale = 0;
	float fInvScale = 0;

	// most of the time, the following instant creation suffices:
	if (fDiagonal > FLOAT_EPSILON)
	{
		fScale =  (sqrtf (fDiagonal) * 2);
		fInvScale = 1 / fScale;

		x = (fMatrix[9] - fMatrix[6]) * fInvScale;
		y = (fMatrix[2] - fMatrix[8]) * fInvScale;
		z = (fMatrix[4] - fMatrix[1]) * fInvScale;
		w = 0.25f * fScale;
	}

	else
	{
		// If the first element of the diagonal is the greatest value
		if ( fMatrix[0] > fMatrix[5] && fMatrix[0] > fMatrix[10] )  
		{	
			// Find the scale according to the first element, and double that value
			fScale  = sqrtf( 1.0f + fMatrix[0] - fMatrix[5] - fMatrix[10] ) * 2.0f;
			fInvScale = 1 / fScale;

			// Calculate the x, y, x and w of the quaternion through the respective equation
			x = 0.25f * fScale;
			y = (fMatrix[4] + fMatrix[1] ) * fInvScale;
			z = (fMatrix[2] + fMatrix[8] ) * fInvScale;
			w = (fMatrix[9] - fMatrix[6] ) * fInvScale;	
		} 
		// Else if the second element of the diagonal is the greatest value
		else if ( fMatrix[5] > fMatrix[10] ) 
		{
			// Find the scale according to the second element, and double that value
			fScale  = sqrtf( 1.0f + fMatrix[5] - fMatrix[0] - fMatrix[10] ) * 2.0f;
			fInvScale = 1 / fScale;

			// Calculate the x, y, x and w of the quaternion through the respective equation
			x = (fMatrix[4] + fMatrix[1] ) * fInvScale;
			y = 0.25f * fScale;
			z = (fMatrix[9] + fMatrix[6] ) * fInvScale;
			w = (fMatrix[2] - fMatrix[8] ) * fInvScale;
		} 
		// Else the third element of the diagonal is the greatest value
		else 
		{	
			// Find the scale according to the third element, and double that value
			fScale  = sqrtf( 1.0f + fMatrix[10] - fMatrix[0] - fMatrix[5] ) * 2.0f;

			// Calculate the x, y, x and w of the quaternion through the respective equation
			x = (fMatrix[2] + fMatrix[8] ) * fInvScale;
			y = (fMatrix[9] + fMatrix[6] ) * fInvScale;
			z = 0.25f * fScale;
			w = (fMatrix[4] - fMatrix[1] ) * fInvScale;
		}
	}

	if (!rightHanded)
		return Quaternion (w,x,y,z);

	else return Quaternion (w,-x,-y,-z);
}

// SLERP
Matrix Matrix::SLERP (Matrix& m2, float t, bool bRightHanded)
{
	Quaternion q1 = GetQuaternion (bRightHanded);
	Quaternion q2 = m2.GetQuaternion (bRightHanded);
	
	Quaternion qInterpolated = ::SLERP (q1, q2, t);
	Matrix answer (qInterpolated);
	answer.SetPos (m2.GetPos());
	return answer;
}

//////////////////////////////////////
// Transpose
// 
// flips the matrix about its major axis
////////////////////////////////////////
Matrix Matrix::Transpose()
{
	float answer[16];
	
	for (int r=0; r<4; r++)
	{
		for (int c=0; c<4; c++)
		{
			answer [r+4*c] = M(c,r);			
		}
	}

	return Matrix(answer);
}

void Matrix::TranslateLocal (float x, float y, float z)
{
	Vector vTranslation = GetX() * x + GetY() * y + GetZ() * z;
	fMatrix[12] += vTranslation[0];
	fMatrix[13] += vTranslation[1];
	fMatrix[14] += vTranslation[2];
}
void Matrix::TranslateLocal (Vector& vTranslate) 
{ 
	TranslateLocal (vTranslate.fComp[0], vTranslate.fComp[1], vTranslate.fComp[2]); 
}

// Matrix Multiply
Matrix Matrix::operator* (Matrix& other)
{
	float fNewMatrix[16];
	memcpy (fNewMatrix, fMatrix, 64);

	#define Mat(r,c) (fNewMatrix[r+4*c])
	for (int r=0; r<3; r++)
	{
		for (int c=0; c<3; c++)
			Mat(r,c) = GetRow(r) * other.GetCol(c);
	}
	#undef Mat

	return Matrix (fNewMatrix);
}

// Matrix Inverse
bool Matrix::Invert()
{
	#define SWAP_ROWS(a , b) { float *_tmp = a; (a)=(b); (b)=_tmp; }
    #define MAT(m,r,c) ((m)[(c)*4+(r)])

	float dst[16];		//destination matrix (i.e. inverse)

	float wtmp[4][8];
	float m0, m1, m2, m3, s;
	float *r0, *r1, *r2, *r3;

	r0 = wtmp[0], r1 = wtmp[1], r2 = wtmp[2], r3 = wtmp[3];

	r0[0] = MAT(fMatrix,0,0), r0[1] = MAT(fMatrix,0,1),
	r0[2] = MAT(fMatrix,0,2), r0[3] = MAT(fMatrix,0,3),
	r0[4] = 1.0, r0[5] = r0[6] = r0[7] = 0.0,

	r1[0] = MAT(fMatrix,1,0), r1[1] = MAT(fMatrix,1,1),
	r1[2] = MAT(fMatrix,1,2), r1[3] = MAT(fMatrix,1,3),
	r1[5] = 1.0, r1[4] = r1[6] = r1[7] = 0.0,

	r2[0] = MAT(fMatrix,2,0), r2[1] = MAT(fMatrix,2,1),
	r2[2] = MAT(fMatrix,2,2), r2[3] = MAT(fMatrix,2,3),
	r2[6] = 1.0, r2[4] = r2[5] = r2[7] = 0.0,

	r3[0] = MAT(fMatrix,3,0), r3[1] = MAT(fMatrix,3,1),
	r3[2] = MAT(fMatrix,3,2), r3[3] = MAT(fMatrix,3,3),
	r3[7] = 1.0, r3[4] = r3[5] = r3[6] = 0.0;

	/* choose pivot - or die */
	if (fabs(r3[0])>fabs(r2[0])) SWAP_ROWS(r3, r2);
	if (fabs(r2[0])>fabs(r1[0])) SWAP_ROWS(r2, r1);
	if (fabs(r1[0])>fabs(r0[0])) SWAP_ROWS(r1, r0);
	if (0.0 == r0[0])  return false;

	/* eliminate first variable     */
	m1 = r1[0]/r0[0]; m2 = r2[0]/r0[0]; m3 = r3[0]/r0[0];
	s = r0[1]; r1[1] -= m1 * s; r2[1] -= m2 * s; r3[1] -= m3 * s;
	s = r0[2]; r1[2] -= m1 * s; r2[2] -= m2 * s; r3[2] -= m3 * s;
	s = r0[3]; r1[3] -= m1 * s; r2[3] -= m2 * s; r3[3] -= m3 * s;
	s = r0[4];
	if (s != 0.0) { r1[4] -= m1 * s; r2[4] -= m2 * s; r3[4] -= m3 * s; }
	s = r0[5];
	if (s != 0.0) { r1[5] -= m1 * s; r2[5] -= m2 * s; r3[5] -= m3 * s; }
	s = r0[6];
	if (s != 0.0) { r1[6] -= m1 * s; r2[6] -= m2 * s; r3[6] -= m3 * s; }
	s = r0[7];
	if (s != 0.0) { r1[7] -= m1 * s; r2[7] -= m2 * s; r3[7] -= m3 * s; }

	/* choose pivot - or die */
	if (fabs(r3[1])>fabs(r2[1])) SWAP_ROWS(r3, r2);
	if (fabs(r2[1])>fabs(r1[1])) SWAP_ROWS(r2, r1);
	if (0.0 == r1[1])  return false;

	/* eliminate second variable */
	m2 = r2[1]/r1[1]; m3 = r3[1]/r1[1];
	r2[2] -= m2 * r1[2]; r3[2] -= m3 * r1[2];
	r2[3] -= m2 * r1[3]; r3[3] -= m3 * r1[3];
	s = r1[4]; if (0.0 != s) { r2[4] -= m2 * s; r3[4] -= m3 * s; }
	s = r1[5]; if (0.0 != s) { r2[5] -= m2 * s; r3[5] -= m3 * s; }
	s = r1[6]; if (0.0 != s) { r2[6] -= m2 * s; r3[6] -= m3 * s; }
	s = r1[7]; if (0.0 != s) { r2[7] -= m2 * s; r3[7] -= m3 * s; }

	/* choose pivot - or die */
	if (fabs(r3[2])>fabs(r2[2])) SWAP_ROWS(r3, r2);
	if (0.0 == r2[2])  return false;

	/* eliminate third variable */
	m3 = r3[2]/r2[2];
	r3[3] -= m3 * r2[3], r3[4] -= m3 * r2[4],
	r3[5] -= m3 * r2[5], r3[6] -= m3 * r2[6],
	r3[7] -= m3 * r2[7];

	/* last check */
	if (0.0 == r3[3]) return false;

	s = 1.0f/r3[3];              /* now back substitute row 3 */
	r3[4] *= s; r3[5] *= s; r3[6] *= s; r3[7] *= s;

	m2 = r2[3];                 /* now back substitute row 2 */
	s  = 1.0f/r2[2];
	r2[4] = s * (r2[4] - r3[4] * m2), r2[5] = s * (r2[5] - r3[5] * m2),
	r2[6] = s * (r2[6] - r3[6] * m2), r2[7] = s * (r2[7] - r3[7] * m2);
	m1 = r1[3];
	r1[4] -= r3[4] * m1, r1[5] -= r3[5] * m1,
	r1[6] -= r3[6] * m1, r1[7] -= r3[7] * m1;
	m0 = r0[3];
	r0[4] -= r3[4] * m0, r0[5] -= r3[5] * m0,
	r0[6] -= r3[6] * m0, r0[7] -= r3[7] * m0;

	m1 = r1[2];                 /* now back substitute row 1 */
	s  = 1.0f/r1[1];
	r1[4] = s * (r1[4] - r2[4] * m1), r1[5] = s * (r1[5] - r2[5] * m1),
	r1[6] = s * (r1[6] - r2[6] * m1), r1[7] = s * (r1[7] - r2[7] * m1);
	m0 = r0[2];
	r0[4] -= r2[4] * m0, r0[5] -= r2[5] * m0,
	r0[6] -= r2[6] * m0, r0[7] -= r2[7] * m0;

	m0 = r0[1];                 /* now back substitute row 0 */
	s  = 1.0f/r0[0];
	r0[4] = s * (r0[4] - r1[4] * m0), r0[5] = s * (r0[5] - r1[5] * m0),
	r0[6] = s * (r0[6] - r1[6] * m0), r0[7] = s * (r0[7] - r1[7] * m0);

	MAT(dst,0,0) = r0[4]; MAT(dst,0,1) = r0[5],
	MAT(dst,0,2) = r0[6]; MAT(dst,0,3) = r0[7],
	MAT(dst,1,0) = r1[4]; MAT(dst,1,1) = r1[5],
	MAT(dst,1,2) = r1[6]; MAT(dst,1,3) = r1[7],
	MAT(dst,2,0) = r2[4]; MAT(dst,2,1) = r2[5],
	MAT(dst,2,2) = r2[6]; MAT(dst,2,3) = r2[7],
	MAT(dst,3,0) = r3[4]; MAT(dst,3,1) = r3[5],
	MAT(dst,3,2) = r3[6]; MAT(dst,3,3) = r3[7]; 

	memcpy (fMatrix, dst, 64);

	return true;

	#undef MAT
	#undef SWAP_ROWS
}

////////////////////////////////////////
//
// ROTATIONS
//
/////////////////////////////////////////

// rotate around the matrix's x,y, or z axes
void Matrix::RotateX (float fDegrees)
{
	// Calculation:
	//
	//	x0	y0	z0	a0		1	0	0	0		
	//	x1	y1	z1	a1	*	0	cos	-sin0
	//	x2	y2	z2	a2		0	sin	cos	0
	//	0	0	0	1		0	0	0	1

	float Cos = cosf (MATH_TRIG_DEGREES_TO_RADIANS * fDegrees);
	float Sin = sinf (MATH_TRIG_DEGREES_TO_RADIANS * fDegrees);

	float y0 = fMatrix[4];
	float y1 = fMatrix[5];
	float y2 = fMatrix[6];
	
	float z0 = fMatrix[8];
	float z1 = fMatrix[9];
	float z2 = fMatrix[10];

	fMatrix[4] = y0*Cos + z0*Sin;
	fMatrix[5] = y1*Cos + z1*Sin;
	fMatrix[6] = y2*Cos + z2*Sin;

	fMatrix[8] = z0*Cos - y0*Sin;
	fMatrix[9] = z1*Cos - y1*Sin;
	fMatrix[10]= z2*Cos - y2*Sin;
}

void Matrix::RotateY (float fDegrees)
{
	// Calculation:
	//
	//	x0	y0	z0	a0		cos	0	sin	0	
	//	x1	y1	z1	a1	*	0	1	0	0
	//	x2	y2	z2	a2		-sin0	cos	0
	//	0	0	0	1		0	0	0	1

	float Cos = cosf (MATH_TRIG_DEGREES_TO_RADIANS * fDegrees);
	float Sin = sinf (MATH_TRIG_DEGREES_TO_RADIANS * fDegrees);

	float x0 = fMatrix[0];
	float x1 = fMatrix[1];
	float x2 = fMatrix[2];
	
	float z0 = fMatrix[8];
	float z1 = fMatrix[9];
	float z2 = fMatrix[10];

	fMatrix[0] = x0*Cos - z0*Sin;
	fMatrix[1] = x1*Cos - z1*Sin;
	fMatrix[2] = x2*Cos - z2*Sin;

	fMatrix[8] = x0*Sin + z0*Cos;
	fMatrix[9] = x1*Sin + z1*Cos;
	fMatrix[10]= x2*Sin + z2*Cos;
}

void Matrix::RotateZ (float fDegrees)
{
	// Calculation:
	//
	//	x0	y0	z0	a0		cos	-sin0	0	
	//	x1	y1	z1	a1	*	sin	cos	0	0
	//	x2	y2	z2	a2		0	0	1	0
	//	0	0	0	1		0	0	0	1

	float Cos = cosf (MATH_TRIG_DEGREES_TO_RADIANS * fDegrees);
	float Sin = sinf (MATH_TRIG_DEGREES_TO_RADIANS * fDegrees);

	float x0 = fMatrix[0];
	float x1 = fMatrix[1];
	float x2 = fMatrix[2];
	
	float y0 = fMatrix[4];
	float y1 = fMatrix[5];
	float y2 = fMatrix[6];

	fMatrix[0] = x0*Cos + y0*Sin;
	fMatrix[1] = x1*Cos + y1*Sin;
	fMatrix[2] = x2*Cos + y2*Sin;

	fMatrix[4] = y0*Cos - x0*Sin;
	fMatrix[5] = y1*Cos - x1*Sin;
	fMatrix[6] = y2*Cos - x2*Sin;
}

// rotate around the global x,y, or z axes

void Matrix::RotateGlobal (Vector& vAxis, float fDegrees)
{
	// get the matrix corresponding to the rotation only
	float Cos = cosf (MATH_TRIG_DEGREES_TO_RADIANS * fDegrees);
	float Sin = sinf (MATH_TRIG_DEGREES_TO_RADIANS * fDegrees);
	
	float x = vAxis[0];
	float y = vAxis[1];
	float z = vAxis[2];

	Matrix mRot;		//column-major matrix representing the rotation
	mRot.fMatrix[0] = (1-Cos) * x*x + Cos;
	mRot.fMatrix[1] = (1-Cos) * x*y + z*Sin;
	mRot.fMatrix[2] = (1-Cos) * x*z - y*Sin;

	mRot.fMatrix[4] = (1-Cos) * y*x - z*Sin;
	mRot.fMatrix[5] = (1-Cos) * y*y + Cos;
	mRot.fMatrix[6] = (1-Cos) * y*z + x*Sin;

	mRot.fMatrix[8] = (1-Cos) * z*x + y*Sin;
	mRot.fMatrix[9] = (1-Cos) * z*y - x*Sin;
	mRot.fMatrix[10] = (1-Cos) * z*z + Cos;

	// apply the matrix
	(*this) *= mRot;	
}

void Matrix::RotateGlobalX (float fDegrees)
{

	// calculation:
	//
	//	1	0	0	0		x0	y0	z0	a0
	//	0	cos	-sin0	*	x1	y1	z1	a1
	//	0	sin	cos	0		x2	y2	z2	a2
	//	0	0	0	1		0	0	0	1

	// get the matrix corresponding to the rotation only
	float Cos = cosf (MATH_TRIG_DEGREES_TO_RADIANS * fDegrees);
	float Sin = sinf (MATH_TRIG_DEGREES_TO_RADIANS * fDegrees);

	float x1 = fMatrix[1];
	float x2 = fMatrix[2];

	float y1 = fMatrix[5];
	float y2 = fMatrix[6];

	float z1 = fMatrix[9];
	float z2 = fMatrix[10];

	fMatrix[1] = x1*Cos - x2*Sin;
	fMatrix[2] = x1*Sin + x2*Cos;

	fMatrix[5] = y1*Cos - y2*Sin;
	fMatrix[6] = y1*Sin + y2*Cos;

	fMatrix[9] = z1*Cos - z2*Sin;
	fMatrix[10]= z1*Sin + z2*Cos;
}

void Matrix::RotateGlobalY (float fDegrees)
{
	// calculation:
	//
	//	cos	0	sin	0		x0	y0	z0	a0
	//	0	1	0	0	*	x1	y1	z1	a1
	//	-sin0	cos	0		x2	y2	z2	a2
	//	0	0	0	1		0	0	0	1

	// get the matrix corresponding to the rotation only
	float Cos = cosf (MATH_TRIG_DEGREES_TO_RADIANS * fDegrees);
	float Sin = sinf (MATH_TRIG_DEGREES_TO_RADIANS * fDegrees);

	float x0 = fMatrix[0];
	float x2 = fMatrix[2];

	float y0 = fMatrix[4];
	float y2 = fMatrix[6];

	float z0 = fMatrix[8];
	float z2 = fMatrix[10];

	fMatrix[0] = x0*Cos + x2*Sin;
	fMatrix[2] = x2*Cos - x0*Sin;

	fMatrix[4] = y0*Cos + y2*Sin;
	fMatrix[6] = y2*Cos - y0*Sin;

	fMatrix[8] = z0*Cos + z2*Sin;
	fMatrix[10] = z2*Cos - z0*Sin;
}

void Matrix::RotateGlobalZ (float fDegrees)
{
	// calculation:
	//
	//	cos	-sin0	0		x0	y0	z0	a0
	//	sin	cos	0	0	*	x1	y1	z1	a1
	//	0	0	1	0		x2	y2	z2	a2
	//	0	0	0	1		0	0	0	1

	// get the matrix corresponding to the rotation only
	float Cos = cosf (MATH_TRIG_DEGREES_TO_RADIANS * fDegrees);
	float Sin = sinf (MATH_TRIG_DEGREES_TO_RADIANS * fDegrees);

	float x0 = fMatrix[0];
	float x1 = fMatrix[1];

	float y0 = fMatrix[4];
	float y1 = fMatrix[5];

	float z0 = fMatrix[8];
	float z1 = fMatrix[9];

	fMatrix[0] = x0*Cos - x1*Sin;
	fMatrix[1] = x0*Sin + x1*Cos;

	fMatrix[4] = y0*Cos - y1*Sin;
	fMatrix[5] = y0*Sin + y1*Cos;

	fMatrix[8] = z0*Cos - z1*Sin;
	fMatrix[9] = z0*Sin + z1*Cos;	
}

	// Makes the matrix's forward vector look in the same direction as vDirection
	// Also, the matrix will be orthonormal after this call
	void Matrix::FaceDirection (Vector& vDirection)
	{
		// find the new axes
		Vector vForward =vDirection.Normalize();
		Vector vRight =-(vForward & Vector(0.0f, 1.0f, 0.0f)).Normalize();
		
		// special case: if you're facing straight up or down, don't do anything
		if (vRight == Vector (0.0f, 0.0f, 0.0f))
			return;

		Vector vUp = -(vRight & vForward).Normalize();

		// set them
		memcpy (fMatrix, vRight.fComp, 12);
		memcpy (fMatrix + 4, vUp.fComp, 12);
		memcpy (fMatrix + 8, vForward.fComp, 12);
	}
	
	// rotates the matrix towards the given point at a given angle
	// if the matrix rotates past the point, it will look at the point instead
	void Matrix::TurnTo (Vector& vPoint, float fAngle)
	{
		TurnToHorizontal (vPoint, fAngle);
		TurnToVertical (vPoint, fAngle);
	}

	void Matrix::TurnToHorizontal (Vector& vPoint, float fAngle)
	{
		Vector vToPoint = vPoint - GetPos();
		vToPoint.fComp[1] = 0;
		Vector vForward = GetZ();
		vForward.fComp[1] = 0;
		if (vToPoint == Vector (0,0,0) || vForward == Vector (0,0,0))
			return;
		
		// turn slower if the matrix is less than 90 degrees from the target point
		vToPoint = vToPoint.Normalize();
		//float fFinalAngle = fAngle;
		float xDot = GetX() * vToPoint;
		if (vForward * vToPoint < 0)
		{
			if (xDot < 0.0f)
				xDot = -1;
			else xDot = 1;
		}
		RotateY (fAngle * xDot);

		/*
		// if the angle of rotation makes you rotate past the target point, face the target point instead
		float fFinalAngle = fAngle;
		//float fAngleToPoint = vToPoint.AngleTo (vForward);
		if (fAngleToPoint < fAngle)
			fFinalAngle = fAngleToPoint;

		// find out whether to rotate left or right
		float xDot = GetX() * vToPoint;
		if (xDot < 0)
			fFinalAngle = -fFinalAngle;

		// now for the punchline
		RotateGlobalY (fFinalAngle);
		*/
	}

	void Matrix::TurnToVertical (Vector& vPoint, float fAngle)
	{
		Vector vToPoint = vPoint - GetPos();
		vToPoint.fComp[0] = 0;
		Vector vForward = GetZ();
		vForward.fComp[0] = 0;
		if (vToPoint == Vector (0,0,0) || vForward == Vector (0,0,0))
			return;

		// if the angle of rotation makes you rotate past the target point, face the target point instead
		float fFinalAngle = fAngle;
		float fAngleToPoint = vToPoint.AngleTo (vForward);
		if (fAngleToPoint < fAngle)
			fFinalAngle = fAngleToPoint;

		// find out whether to rotate left or right
		float yDot = -(GetY() * vToPoint);
		if (yDot < 0)
			fFinalAngle = -fFinalAngle;

		// now for the punchline
		RotateX (fFinalAngle);
	}





	void Matrix::SetRotationAngles(float *fAngles, bool bRadians)
	{
		float fRollCos = (bRadians) ? MathCosR(fAngles[0]) : MathCosD(fAngles[0]);
		float fRollSin = (bRadians) ? MathSinR(fAngles[0]) : MathSinD(fAngles[0]);
		float fPitchCos = (bRadians) ? MathCosR(fAngles[1]) : MathCosD(fAngles[1]);
		float fPitchSin = (bRadians) ? MathSinR(fAngles[1]) : MathSinD(fAngles[1]);
		float fYawCos = (bRadians) ? MathCosR(fAngles[2]) : MathCosD(fAngles[2]);
		float fYawSin = (bRadians) ? MathSinR(fAngles[2]) : MathSinD(fAngles[2]);

		fMatrix[0] = fPitchCos * fYawCos;
		fMatrix[1] = fPitchCos * fYawSin;
		fMatrix[2] = -fPitchSin;

		fMatrix[4] = (fRollSin * fPitchSin * fYawCos) - (fRollCos * fYawSin);
		fMatrix[5] = (fRollSin * fPitchSin * fYawSin) + (fRollCos * fYawCos);
		fMatrix[6] = (fRollSin * fPitchCos);

		fMatrix[8] = (fRollCos * fPitchSin * fYawCos) + (fRollSin * fYawSin);
		fMatrix[9] = (fRollCos * fPitchSin * fYawSin) - (fRollSin * fYawCos);
		fMatrix[10] = fRollCos * fPitchCos;

	}
