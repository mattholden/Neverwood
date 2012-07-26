
#include "MathUtils.h"

//////////////////////////////////////////////////////////
// Helper function for checking float equality
/////////////////////////////////////////////////////////
bool FloatEqual(float fCheck, float fCheckAgainst)
{ return (fCheckAgainst - FLOAT_EPSILON < fCheck && fCheckAgainst + FLOAT_EPSILON > fCheck); }


int RandomInt (int min, int max)
{
	return min + rand() % (1 + max - min);
}
/*

// ROLF: Ewwww

//////////////////////////////////////////////////////////
// Helper function for making random floats
/////////////////////////////////////////////////////////
float RandFloat(float fMin, float fMax, int iPrecision)
{
	int iToInt = int(pow(10, iPrecision));

	int iMin = int(fMin * iToInt);
	int iMax = int(fMax * iToInt);

	bool bSignDifference = false;
	
	if (iMin < 0 && iMax >= 0)
	{
		iMax += (iMin * -1);
		bSignDifference = true;
	}
	
	int iRand;
	if (iMin != iMax)
		iRand = (bSignDifference) ? (rand() % iMax) : ((rand() % (iMax - iMin)) + iMin);
	else
		iRand = iMin;

	if (bSignDifference)
		iRand += iMin;

	return iRand * float(pow(10, (iPrecision * -1)));

}*/

float RandomFloat()
{
	return float(rand()) / RAND_MAX;
}

float RandomFloat (float fMin, float fMax)
{
	return fMin + RandomFloat() * (fMax - fMin);
}