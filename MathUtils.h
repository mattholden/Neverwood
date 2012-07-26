
// Provide some basic general-purpose utility functions for our math libraries

#ifndef StormForge_Shared_MathUtils
#define StormForge_Shared_MathUtils

#include <cmath>
#include <cstdlib>

// Constants to help us out
#define PI 3.141529f;
#define FLOAT_EPSILON .00001f
#define FLOAT_BIGEPSILON 0.01f

// macros
#define ABS(x)	( (x)<0 ? -(x) : x )
#define SIGN(x) ( (x)<0 ? -1 : 1)
#define MIN(x,y) ( (x)<(y) ? (x) : (y) )
#define MAX(x,y) ( (x)>(y) ? (x) : (y) )

// Other sub-utility files
#include "mathtrigtable.h"

//////////////////////////////////////////////////////////
// Helper function for checking float equality
/////////////////////////////////////////////////////////
static bool FloatEqual(float fCheck, float fCheckAgainst)
{ 
	return (fCheckAgainst - FLOAT_EPSILON < fCheck && fCheckAgainst + FLOAT_EPSILON > fCheck); 
}

//////////////////////////////////////////////////////////
// Helper function for making random floats
/////////////////////////////////////////////////////////
//float RandFloat(float fMin, float fMax, int iPrecision);

//////////////////////////////////////////////////////////
// Make a random integer in a given range
//////////////////////////////////////////////////////////
static int RandomInt (int min, int max)
{
	return min + rand() % (1 + max - min);
}

//////////////////////////////////////////////////////////
// Make a random float between 0 and 1
//////////////////////////////////////////////////////////
static float RandomFloat()
{
	return float(rand()) / RAND_MAX;
}

///////////////////////////////////////////////////////////
// Create a random float. The EASY way
///////////////////////////////////////////////////////////
static float RandomFloat (float fMin, float fMax)
{
	return fMin + RandomFloat() * (fMax - fMin);
}

 #endif