#ifndef NEVERWOOD_UTILS_H
#define NEVERWOOD_UTILS_H

// memory management macros
#define ZERO(a) memset(&(a), 0, sizeof(a));
#define SAFE_DELETE(x) if(x) {delete x; x=0;}
#define SAFE_RELEASE(x) if(x) {x->Release(); x=0;}
#define DELETE_ARRAY(a) if(a) {delete[] a; a=0;}

// Macro to let us only error check in debug mode if we want to.
#if _DEBUG
	// In debug mode, actually check for errors
	#define ERRORCHECKDEBUG(HResult, strFunction) HandleDirectXError((HResult), (strFunction), __FILE__, __LINE__)
#else
	// In release mode, just replace the ERRORCHECKDEBUG call with the function call inside
	#define ERRORCHECKDEBUG(HResult, strFunction) ((HResult) < 0)
#endif

// Color Macros
#define FloatColor(r, g, b, a) D3DCOLOR_ARGB(BYTE(a * 255), BYTE(r * 255), BYTE(g * 255), BYTE(b * 255))
#define VectorColor(v) RGB(BYTE((v[0]) * 255), BYTE((v[1]) * 255), BYTE((v[2]) * 255))
#define ALPHA16(a)				 ((a << 24)|0xFFFFFF)
#define WHITE D3DCOLOR_XRGB(1,1,1)
#define BLACK D3DCOLOR_XRGB(0,0,0)

#endif