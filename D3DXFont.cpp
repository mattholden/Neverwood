
#include "D3DXFont.h"

int DXFonts::iCurrent = -1;
int DXFonts::iFontCount = 0;
LPD3DXFONT DXFonts::pFonts[5];


int DXFonts::Create(char *strFont, int iSize, bool bBold, bool bItalic, bool bUnder)
	{
		HFONT hFont = CreateFont(	iSize, 0, 0, 0, 
													(bBold) ? FW_BOLD : FW_REGULAR,
													(bItalic) ? TRUE : FALSE,
													(bUnder) ? TRUE : FALSE,
													NULL, // strikethru
													DEFAULT_CHARSET,
													OUT_DEFAULT_PRECIS,
													CLIP_DEFAULT_PRECIS,
													DEFAULT_QUALITY,
													DEFAULT_PITCH | FF_DONTCARE,
													strFont);
										
		if (!hFont) return -1;

		if (FAILED(D3DXCreateFont(Direct3D::GetDevice(), hFont, &pFonts[iFontCount])))
			return -1;
		iFontCount++;

		DeleteObject(hFont);

		return iFontCount - 1;
	}

	
	void DXFonts::Begin(int iFont) 
	{
		if (iFont >= iFontCount || iFont < 0) 
			return; 
		pFonts[iFont]->Begin();
		iCurrent = iFont;
	}
	
	void DXFonts::End(void) 
	{ 
		if (iCurrent == -1) return; 
		pFonts[iCurrent]->End(); 
		iCurrent = -1; 
	}


	bool DXFonts::Write(char *strString, int iXPos, int iYPos, float fRed, float fGreen, float fBlue, float fAlpha)
	{
		if (iCurrent == -1) return 0;
		RECT rDraw = { iXPos, iYPos, Direct3D::GetScreenWidth(), Direct3D::GetScreenHeight() };
		DWORD dwColor = FloatColor(fRed, fGreen, fBlue, fAlpha);
		return (pFonts[iCurrent]->DrawTextA(strString, -1, &rDraw, DT_LEFT, dwColor)) ? 1 : 0;
	}

	void DXFonts::Shutdown(void)
	{
		for (int i = 0; i < iFontCount; i++)
			SAFE_RELEASE(pFonts[i]);
		iCurrent = -1;
		iFontCount = 0;

	}
