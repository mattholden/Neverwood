
#ifndef StormForge_Matt_D3DXFont
#define StormForge_Matt_D3DXFont

#include "Direct3D.h"

class DXFonts
{
private:
	static LPD3DXFONT pFonts[5];
	static int iFontCount;
	static int iCurrent;

public:

	static int Create(char *strFont, int iSize, bool bBold = false, bool bItalic = false, bool bUnder = false);

	static void Begin(int iFont = 0);
	
	static void WriteShadow(char *strString, int iXPos, int iYPos, int iShadow = 3, float fRed = 1.0f, float fGreen = 1.0f, float fBlue = 1.0f, float fAlpha = 1.0f)
	{
		Write(strString, iXPos, iYPos, 0,0,0,1);
		Write(strString, iXPos + iShadow, iYPos + iShadow, fRed, fGreen, fBlue, fAlpha);
	}

	static void End(void);

    static bool Write(char *strString, int iXPos, int iYPos, float fRed = 1.0f, float fGreen = 1.0f, float fBlue = 1.0f, float fAlpha = 1.0f);

	static void Shutdown(void);

};
#endif