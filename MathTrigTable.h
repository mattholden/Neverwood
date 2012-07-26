/*////////////////////////////////////////////////////////////////////////
MathTrigTable.h	///		March 17, 2004		//		1.00		///
//////////////////////////////////////////////////////////////////////////

Defines a lookup table for our trigonometric functions, to -drastically-
speed up any trig functions you need.

To use, make sure you call MATH_USING_TRIG. This will grant the function
(or file, if you call it as a global) access to the table we've generated.
At initialization time, we need to call MathTrigInitialize() to initialize the table.

There are three sets of macros, with suffixes N, D, and R. 
N is a native format, and will be a circular "degree" between 0-255.
D is for degree angles and R is for radian angles. 
Native is a little faster than D or R, but D and R are easier to use. 
See the charts in comments below for speed references.

These macros will all return floating-point values. To check if you had an undefined
value (not needed on Sin or Cos) check for MATH_UNDEFINED.

Warning: Secant and Cotangent tend to be very lossy between 315 degrees and 
360 degrees, so watch out for that range if you can.
*/

#ifndef StormForge_Matt_MathTrigTable
#define StormForge_Matt_MathTrigTable


#include <math.h>


// 0.71111 will be the conversion factor we'll use to make sure that we end up between 0 and 360.
#define MATH_TRIG_TABLESIZE 256
#define MATH_TRIG_DEGREES_TO_INDEX 0.71111f
#define MATH_TRIG_DEGREES_TO_RADIANS 0.017453f
#define MATH_TRIG_RADIANS_TO_INDEX 40.7436f
#define MATH_TRIG_RADIANS_TO_DEGREES 57.29578f
#define MATH_TRIG_INDEX_TO_RADIANS 0.02455f
//#define MATH_USING_TRIG extern float pSin[MATH_TRIG_TABLESIZE];
#define MATH_UNDEFINED -999.99f

// CHANGED:
// Made the trig table a global variable
// Rolf, 4-12-04

// This is our actual sin lookup table. Doing this by 256 instead of 360 lets us do a few speed cheats.
static float pSin[MATH_TRIG_TABLESIZE];


///////////////////////////////////////////////////////////////////////////////////	//
// Here's a quick reference for speeds of these.								//
// Obviously, Sin and Cos are fastest.												//
// The "checks" are to keep us from dividing by zero.						//
// The ~ on divide means that the speed of this operation varies.	//
/////////////////////////////////////////////////////////////////////////////////////
/////				MACRO DEFINITIONS FOR -NATIVE- DATA					/////
/////									(0 - 255 VALUES)									/////
/////////////////////////////////////////////////////////////////////////////////////
//						Add			Divide		Multiply		Checks		Cycles		//
//	MathSinN:		0				0				0				0				0			//
//	MathCosN:		1				0				0				0				1			//
//	MathTanN:		1				~46			0				1				~48		//
//	MathSecN:		0				~46			0				1				~47		//
//	MathCscN:		2				~46			0				1				~49		//
//	MathCotN:		2				~46			0				1				~49		//
//																										//
/////////////////////////////////////////////////////////////////////////////////////
#define MathSinN(angle) pSin[(unsigned char)((angle))]
#define MathCosN(angle) MathSinN((angle) + 90)
#define MathCotN(angle) ( (MathSinN((angle))) ? (MathCosN((angle)) / MathSinN((angle))) : MATH_UNDEFINED)
#define MathSecN(angle) ((MathSinN((angle))) ? (1 / MathSinN((angle))) : MATH_UNDEFINED)
#define MathCscN(angle) ((MathCosN((angle))) ? (1 / MathCosN((angle))) : MATH_UNDEFINED)
#define MathTanN(angle) ( (MathCosN((angle))) ? (MathSinN((angle)) / MathCosN((angle))) : MATH_UNDEFINED)

///////////////////////////////////////////////////////////////////////////////////	//
// Here's a quick reference for speeds of these.								//
// Obviously, Sin and Cos are fastest.												//
// The "checks" are to keep us from dividing by zero.						//
// The ~ on divide means that the speed of this operation varies.	//
/////////////////////////////////////////////////////////////////////////////////////
/////		MACRO DEFINITIONS FOR -DEGREE- OR -RADIAN- DATA	/////
/////////////////////////////////////////////////////////////////////////////////////
//						Add			Divide		Multiply		Checks		Cycles		//
//	MathSinD:		0				0				1				0				1			//
//	MathCosD:		1				0				1				0				2			//
//	MathTanD:		1				~46			3				1				~51		//
//	MathSecD:		0				~46			2				1				~49		//
//	MathCscD:		2				~46			2				1				~51		//
//	MathCotD:		2				~46			3				1				~52		//
//																										//
/////////////////////////////////////////////////////////////////////////////////////
#define MathSinD(angle) pSin[(unsigned char)((angle) * MATH_TRIG_DEGREES_TO_INDEX)]
#define MathCosD(angle) MathSinD((angle) + 90)
#define MathTanD(angle) ( (MathCosD((angle))) ? (MathSinD((angle)) / MathCosD((angle))) : MATH_UNDEFINED)
#define MathCotD(angle) ( (MathSinD((angle))) ? (MathCosD((angle)) / MathSinD((angle))) : MATH_UNDEFINED)
#define MathSecD(angle) ((MathSinD((angle))) ? (1 / MathSinD((angle))) : MATH_UNDEFINED)
#define MathCscD(angle) ((MathCosD((angle))) ? (1 / MathCosD((angle))) : MATH_UNDEFINED)

///////////////////////////////////////////////////////////////////////////////////	//
// Here's a quick reference for speeds of these.								//
// Obviously, Sin and Cos are fastest.												//
// The "checks" are to keep us from dividing by zero.						//
// The ~ on divide means that the speed of this operation varies.	//
/////////////////////////////////////////////////////////////////////////////////////
/////		MACRO DEFINITIONS FOR -DEGREE- OR -RADIAN- DATA	/////
/////////////////////////////////////////////////////////////////////////////////////
//						Add			Divide		Multiply		Checks		Cycles		//
//	MathSinR:			0				0				1				0				1			//
//	MathCosR:		1				0				1				0				2			//
//	MathTanR:		1				~46			3				1				~51		//
//	MathSecR:		0				~46			2				1				~49		//
//	MathCscR:		2				~46			2				1				~51		//
//	MathCotR:		2				~46			3				1				~52		//
//																										//
/////////////////////////////////////////////////////////////////////////////////////
#define MathSinR(angle) pSin[(unsigned char)((angle) * MATH_TRIG_RADIANS_TO_INDEX)]
#define MathCosR(angle) MathSinR((angle) + 90)
#define MathTanR(angle) ( (MathCosR((angle))) ? (MathSinR((angle)) / MathCosR((angle))) : MATH_UNDEFINED)
#define MathCotR(angle) ( (MathSinR((angle))) ? (MathCosR((angle)) / MathSinR((angle))) : MATH_UNDEFINED)
#define MathSecR(angle) ((MathSinR((angle))) ? (1 / MathSinR((angle))) : MATH_UNDEFINED)
#define MathCscR(angle) ((MathCosR((angle))) ? (1 / MathCosR((angle))) : MATH_UNDEFINED)


////////////////////////
/// MathTrigInitialize( )
////////////////////////
/// Wow, a real function! :)
/// Sets up our sin table with values. Should be called once at the start of the program, and that's all.
/// No shutdown is needed as these are not dynamic memory.
///
/// Input:
///		none
//
/// Returns:
///		none
////////////////////////
void MathTrigInitialize(void);

// Handy 1-cycle conversions added here for user convenience
#define MathDegToRad(Deg) ((Deg) * MATH_TRIG_DEGREES_TO_RADIANS)
#define MathRadToDeg(Rad) ((Rad) * MATH_TRIG_RADIANS_TO_DEGREES)





#endif
