/*////////////////////////////////////////////////////////////////////////
InputJoystick.cpp		///		February 5, 2004		//		1.00		///
//////////////////////////////////////////////////////////////////////////

This class defines a Joystick class for use with DirectInput 8+. 

//////////////////////////	Your 2D Game Here ////////////////////////
(C) Matthew Holden, 2004
This code is free for use in any non-commercial project, so long as these
comments remain intact. You may change this code as you see fit in your
own game, but you may not re-release the Your 2D Game Here API as a 
programming tool. If you have made a modification to the API that you feel
warrants inclusion in a future version of the official API, please let us know
by emailing Improvements@Your2DGameHere.com. The Y2GDH community
is important to us and we look forward to improving this toolset for other 
programmers.
*//////////////////////////////////////////////////////////////////////////


#include "InputJoystick.h"
#include <dinput.h>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")


// Initialize static joystick count...
int CInputJoystick::nJoystickCount = 0;
CInputJoystick* CInputJoystick::pEnumerating = 0;
int CInputJoystick::ForceFeedbackEffect::siEffectCount = 0;



	////////////////////////
	/// Initialize( )
	////////////////////////
	/// Sets us up the device.
	/// For joysticks, make sure to call SetGuid() before Initialize, out of the EnumerateJoysticks function.
	///
	/// Input:
	///		HWND hWnd							Handle to the window
	///		LPDIRECTINPUT8 pDInput			The DirectInput object
	//
	/// Returns:
	///		bool		true if everything set up OK.
	////////////////////////
	bool CInputJoystick::Initialize(HWND hWnd, LPDIRECTINPUT8 pDInput)
	{
		// Our HRESULT we'll re-use.
		HRESULT hResult = NULL;

		// Tell the base class what kind of device it is.
		eDeviceType = DEVICE_JOYSTICK;

		// Create the device.
		hResult = pDInput->CreateDevice(JoystickGUID, &pDevice, NULL);
		if (FAILED(hResult)) return false;

		// Null out feedack effects for now
		pFeedbackEffects = NULL;

		// Set data format so the device knows what it is.
		hResult = pDevice->SetDataFormat(&c_dfDIJoystick2);
		if (FAILED(hResult)) return false;

		// Set cooperative level. Joysticks will default to exclusive; how many games can you play at once?
		hResult = CooperativeLevel(hWnd, true, true);
		if (FAILED(hResult)) return false;

		// Null out the immediate input buffer.
		memset((void*)&JoystickState, 0, sizeof(JoystickState));

		// Prepare our buffer for buffered input.
		SetupBuffer();

		// Set up pointer
		nPointerX = 0;
		nPointerY = 0;
		fPointerSensitivity = DEFAULT_JOYSTICK_POINTER_SENSITIVITY;

		// Increment the counter of joysticks we have set up
		nJoystickCount++;

		// Check for force feedback capabilities.
		DIDEVCAPS DevCaps;
		ZeroMemory(&DevCaps, sizeof(DevCaps));
		DevCaps.dwSize = sizeof(DevCaps);
		pDevice->GetCapabilities   (&DevCaps);
		bSupportsForceFeedback =  ((DevCaps.dwFlags & DIDC_FORCEFEEDBACK) == DIDC_FORCEFEEDBACK) ? true : false;
		ToggleForceFeedback(bSupportsForceFeedback);

		if (bSupportsForceFeedback)
		{
			// Kill auto-centering spring if using force feedback
			DIPROPDWORD dipdw;
			dipdw.diph.dwSize       = sizeof(DIPROPDWORD);
			dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
			dipdw.diph.dwObj        = 0;
			dipdw.diph.dwHow        = DIPH_DEVICE;
			dipdw.dwData            = FALSE;
			if( FAILED(pDevice->SetProperty( DIPROP_AUTOCENTER, &dipdw.diph ) ) )
				return false;
		}

		// Acquire the device.
		hResult = pDevice->Acquire();
		if (FAILED(hResult)) return false;


		// If we get here, everything went smoothly!
		return true;
	}

	
	
	////////////////////////
	/// Poll ( )
	////////////////////////
	/// Make sure the device is acquired and working.
	/// Also should handle the filling of the immediate mode "buffer".
	/// In the case of the Joystick, it will also update our "Fake" pointer.
	///
	/// Input:
	///		none
	//
	/// Returns:
	///		bool		Whether or not it's acquired and ready for our use
	////////////////////////
	bool CInputJoystick::Poll(void)
	{
			// If the device isn't set up yet...
			if (!pDevice || !this) 
				return false;

			// Blank out the immediate "buffer" to hold our data.
			memset(&JoystickState, 0, sizeof(JoystickState));

	        // Try to get the immediate input. If we fail, we know we need to re-Acquire().
			HRESULT hResult = pDevice->Poll();
			if (FAILED(hResult))
			{
				// Something got messed up. Try to reacquire
				hResult = pDevice->Acquire();

				// If we couldn't reacquire, we can't get data from it. 
				if (FAILED(hResult)) 
					return false;
			}

			// It's not messed up, so we can try and read data from it.
			else
			{
				hResult = pDevice->GetDeviceState(sizeof(JoystickState), &JoystickState);
				if (FAILED(hResult)) 
					return false;

				// Update the pointer.
				DWORD dwX = CheckImmediateInput(JOYSTICK_AXIS_LX);
				DWORD dwY = CheckImmediateInput(JOYSTICK_AXIS_LY);

				if (abs(dwX) > nDeadZone)
					nPointerX += ((int)dwX < 0) ? -1 : 1;
				if (abs(dwY) > nDeadZone)
					nPointerY += ((int)dwY < 0) ? -1 : 1;

				// Check D-Pad (in case we have no "axis"
				DWORD dwPad = CheckImmediateInput(JOYSTICK_DPAD);
				if (SimplifyPOV(dwPad) == JOYSTICK_POV_LEFT || SimplifyPOV(dwPad) == JOYSTICK_POV_UP_LEFT || SimplifyPOV(dwPad) == JOYSTICK_POV_DOWN_LEFT)
					nPointerX--;
				if (SimplifyPOV(dwPad) == JOYSTICK_POV_RIGHT || SimplifyPOV(dwPad) == JOYSTICK_POV_UP_RIGHT || SimplifyPOV(dwPad) == JOYSTICK_POV_DOWN_RIGHT)
					nPointerX++;
				if (SimplifyPOV(dwPad) == JOYSTICK_POV_UP || SimplifyPOV(dwPad) == JOYSTICK_POV_UP_LEFT || SimplifyPOV(dwPad) == JOYSTICK_POV_UP_RIGHT)
					nPointerY--;
				if (SimplifyPOV(dwPad) == JOYSTICK_POV_DOWN || SimplifyPOV(dwPad) == JOYSTICK_POV_DOWN_LEFT || SimplifyPOV(dwPad) == JOYSTICK_POV_DOWN_RIGHT)
					nPointerY++;

				if (nPointerX < 0) nPointerX = 0;
				if (nPointerY < 0) nPointerY = 0;
				if (nPointerX > GetSystemMetrics(SM_CXSCREEN)) nPointerX = GetSystemMetrics(SM_CXSCREEN);
				if (nPointerY > GetSystemMetrics(SM_CYSCREEN)) nPointerY = GetSystemMetrics(SM_CYSCREEN);

			}
			
			// Otherwise, we're all set!
			return true;
	}

	

	////////////////////////
	/// CheckBufferedInput ( )
	////////////////////////
	/// Check for a given axis or button in the BUFFERED input stream.
	///
	/// Input:
	///		DWORD dwInput
	///		This is what we're looking for. 
	///		For the Joystick, use 0-7 for the buttons and the Axis enumeration
	///
	/// Returns:
	///		DWORD		This will be 0 if dwInput was not found in the buffer. 
	///						For on/off inputs (keys, joystick/Joystick buttons) you can simply 
	///						say if (CheckBufferedInput(input)) and that will be good enough.
	///
	///						For things that have ranges, like Joystick/joystick actions, it will return
	///						the -amount- of the movement. For example, if you moved the Joystick
	///						10 pixels up, and asked for JoystickAXIS_X, you'd get -10 back from this
	///						function.
	////////////////////////
	long CInputJoystick::CheckBufferedInput(DWORD dwInput)
	{
		// Check every input in the buffer. If we find that the key was pressed, return the high bit
		// (just for consistency with CheckImmediateInput). If we don't find it, return NULL.
		if (!this || !pDevice) return false;

		// We need to seperate axes, which don't get the logical & high-bit check, from buttons that do
		switch (dwInput)
		{

		// You're checking for an axis. If there was movement on this axis, return the whole DWORD to show
		// how much movement there was.
		case JOYSTICK_AXIS_X :
		case JOYSTICK_AXIS_Y : 
		case JOYSTICK_AXIS_Z :
		case JOYSTICK_AXIS_RX :
		case JOYSTICK_POV(0):
		case JOYSTICK_POV(1):
		case JOYSTICK_POV(2):
		case JOYSTICK_POV(3):
			for (int i = 0; i < int(dwBufferSize); i++)
		        if (InputBuffer[i].dwOfs == dwInput) return InputBuffer[i].dwOfs;

			// If you get here, you were looking for an axis and didn't find one. Why bother checking for buttons?
			return NULL;

		// If you get here, you must be checking for a button.
		// Check every input in the buffer. If we find that the button was pressed, return the high bit
		// (just for consistency with CheckImmediateInput). If we don't find it, return NULL.

		default:
			for (int i = 0; i < int(dwBufferSize); i++)
		        if (InputBuffer[i].dwOfs == dwInput && (InputBuffer[i].dwData & 0x80)) return 0x80; 
		}

		// Whatever you were looking for, you didn't find it.
		return NULL;
	}
	

	////////////////////////
	/// CheckImmediateInput ( )	
	////////////////////////
	/// Check for a given key in the IMMEDIATE input stream.
	///
	/// Input:
	///		DWORD dwInput
	///		This is what we're looking for. 
	///		For the Joystick, use 0-7 for the buttons and the Axis enumeration
	///
	/// Returns:
	///		DWORD		This will be 0 if dwInput was not found in the buffer. 
	///						For on/off inputs (keys, joystick/Joystick buttons) you can simply 
	///						say if (CheckImmediateInput(input)) and that will be good enough.
	///
	///						For things that have ranges, like Joystick/joystick actions, it will return
	///						the -amount- of the movement. For example, if you moved the Joystick
	///						10 pixels up, and asked for JoystickAXIS_X, you'd get -10 back from this
	///						function.
	////////////////////////
	long CInputJoystick::CheckImmediateInput(DWORD dwInput)
	{
		if (!this || !pDevice) return false;

		// What we return will depend on the type of thing we asked for
		switch (dwInput)
		{

		// if we're checking for an axis...
		case JOYSTICK_AXIS_X :
			return JoystickState.lX;
		case JOYSTICK_AXIS_Y :
			return JoystickState.lY;
		case JOYSTICK_AXIS_RX :	//	case JOYSTICK_AXIS_Z: (same thing)
			return JoystickState.lRz;
		case JOYSTICK_AXIS_RY:
			return JoystickState.lZ;

		case JOYSTICK_POV(0):		// case JOYSTICK_DPAD
			return JoystickState.rgdwPOV[0];
		case JOYSTICK_POV(1):
			return JoystickState.rgdwPOV[1];
		case JOYSTICK_POV(2):
			return JoystickState.rgdwPOV[2];
		case JOYSTICK_POV(3):
			return JoystickState.rgdwPOV[3];
		
		// We're checking for a button
		default :
			return (JoystickState.rgbButtons[dwInput - JOYSTICK_BUTTON(0)] & 0x80);
		}
	}


	
	////////////////////////
	/// SetRanges ( )
	////////////////////////
	/// Set the maximum range for the joystick X, Y, Z axes.
	///	 By default, if you pass in only an X value, it will make Y and Z the same size.
	/// Pass in only a positive number (the maximum range). We will set the minimum to be 
	/// the maximum * -1. 
	///
	/// Input:
	///		int nRangeMaxX		Maximum range in the X
	///		int nRangeMaxY		Maximum range in the Y (default = same as X)
	///		int nRangeMaxRx		Maximum range in the Rx (default = same as X)
	///		int nRangeMaxRy		Maximum range in the Ry (default = same as X)
	///
	/// Returns:
	///		bool		true if we succeeded
	////////////////////////
	bool CInputJoystick::SetRanges(int nRangeMaxX, int nRangeMaxY, int nRangeMaxRx, int nRangeMaxRy)
	{

		if (!this || !pDevice) return false;

		// Catch function returns.
		HRESULT hResult;

		// Prepare the range structure for X.
			DIPROPRANGE diProp;
			memset(&diProp, 0, sizeof(diProp));
			diProp.diph.dwSize = sizeof(diProp);
			diProp.diph.dwHeaderSize = sizeof(DIPROPHEADER);
			diProp.diph.dwHow = DIPH_BYOFFSET;
			diProp.diph.dwObj = DIJOFS_X;
			diProp.lMin = (nRangeMaxX * -1);
			diProp.lMax = nRangeMaxX;
			
			nMaxX = nRangeMaxX;

		 // Set the range for X.
			hResult = pDevice->SetProperty(DIPROP_RANGE, &diProp.diph);
			if (FAILED(hResult)) return false;
			
		 // Prepare the structure for Y.
			diProp.diph.dwObj = DIJOFS_Y;
			if (nRangeMaxY != -1)
			{
				diProp.lMin = (nRangeMaxY * -1);
				diProp.lMax = nRangeMaxY;
				nMaxY = nRangeMaxY;
			}
			else
			{
				nMaxY = nRangeMaxX;
			}

		// Set range for Y
			hResult = pDevice->SetProperty(DIPROP_RANGE, &diProp.diph);
			if (FAILED(hResult)) return false;
		
			// Prepare the structure for Z.
			diProp.diph.dwObj = DIJOFS_Z;
			if (nRangeMaxRx == -1)
			{
				diProp.lMin = (nRangeMaxX * -1);
				diProp.lMax = nRangeMaxX;
				nMaxRX = nRangeMaxX;
			}
			else
			{
				diProp.lMin = (nRangeMaxRx * -1);
				diProp.lMax = nRangeMaxRx;
				nMaxRX = nRangeMaxRx;
			}

			// Set range for Z
			hResult = pDevice->SetProperty(DIPROP_RANGE, &diProp.diph);
			if (FAILED(hResult)) return false;

			// Prepare the structure for RZ.
			diProp.diph.dwObj = DIJOFS_RZ;
			if (nRangeMaxRy == -1)
			{
				diProp.lMin = (nRangeMaxX * -1);
				diProp.lMax = nRangeMaxX;
				nMaxRY = nRangeMaxX;
			}
			else
			{
				diProp.lMin = (nRangeMaxRy * -1);
				diProp.lMax = nRangeMaxRy;
				nMaxRY = nRangeMaxRy;
			}
			// Set range for rZ
			hResult = pDevice->SetProperty(DIPROP_RANGE, &diProp.diph);
			if (FAILED(hResult)) return false;


		// Nothing left to mess up, so...
			return true;
			
	}

	
	////////////////////////
	/// SetDeadZone ( )
	////////////////////////
	/// Sets the minimum position on the axis a joystick must be pressed in order to register.
	/// This can help buffer against calibration errors.
	///
	/// Pass in only a positive number (the maximum range). We will set the minimum to be 
	/// the maximum * -1. 
	///
	/// Input:
	///		DWORD dwDeadZone		
	///		Size of the dead zone (in all directions, centered on the 'rest' position of 0,0,0.
	///
	/// Returns:
	///		bool		true if we succeeded
	////////////////////////
	bool CInputJoystick::SetDeadZone(DWORD dwDeadZone)
	{
			if (!this || !pDevice) return false;

			// Prepare the structure...
			DIPROPDWORD diDead;
			memset(&diDead, 0, sizeof(diDead));

			diDead.diph.dwHeaderSize = sizeof(DIPROPHEADER);
			diDead.diph.dwSize = sizeof(DIPROPDWORD);
			diDead.diph.dwHow = DIPH_DEVICE;
			diDead.diph.dwObj = 0;
			diDead.dwData = dwDeadZone;

			nDeadZone = (int)dwDeadZone;

			// Set the property and return the results.
			return (!FAILED(pDevice->SetProperty(DIPROP_DEADZONE, &diDead.diph)));
			
	}

	/////////////////////////////////
	// SimplifyPOV [ static ] 
	/////////////////////////////////
	// Reduce a POV's degrees into one of nine easy to read directions.
	// Recommended syntax to check: if (CInputJoystick::SimplifyPOV(pJoystick->CheckImmediateInput(JOYSTICK_DPAD)) == UP_LEFT) ... 
	// Input:
	//		DWORD dwPOV		The degrees (Returned from CheckImmediateInput / CheckBufferedInput)
	//
	// Returns:
	//		int			One of 9 enumerated directions. 
	/////////////////////////////////
	int CInputJoystick::SimplifyPOV(DWORD dwPOV)
	{

		/* OK, what's up with this complicated math voodoo garbage?

			 The DWORD that the POV returns from DirectInput (which we're taking in here as dwPOV) returns the angle of the
			 D-pad/POV Hat, clockwise, from straight up, in 100ths of a degree. If it only has five positions (up, down, left, right, none), 
			 those would be at -1 (none), 0 (up), 9000 (right), 18000 (down) and 27000 (left). A little quick geometry will confirm this.

			 To allow SimplifyPOV to handle diagonals, we needed to be able to break the circle into eight sections. To do this properly,
			 I subdivided the circle into 45-degree sections, beginning at 0. The problem becomes that the numbers I gained at these 
			 divisions represented the "angle bisector" (think, the middle point) of the range we wanted. Since joysticks can be a hair off, 
			 we want to accept a RANGE for each direction. 

			 I quickly calculated that half of 45 degrees is 22.5, or 2250 since POV uses 100ths of a degree. Now, I could go 2250 degrees
			 before and after each of the eight directionals, and create equal, seamless ranges around each. The first version of SimplifyPOV
			 used a series of if() checks with ranges to determine where the pad was leaning. However, I looked at the problem again
			 and realized that if the directions were enumerated clockwise, in the same pattern as the degrees, I could simply iterate through
			 the circle, multiplying the iterator variable by 2250 (and the iterator + 2 by it to represent the upper boundary) and if POV's 
			 return fell between them, I knew what direction I was facing. Since the UP range is a bit wierd (having small numbers to the 
			 right of the directional and large numbers to the left, I opted to make it the default case rather than perform the necessary
			 calculations.

		*/
		if ((int)dwPOV < 0 || (int)dwPOV > 36000)
			return JOYSTICK_POV_NONE;

		for (int i = 1; i < 15; i += 2)
		{
			if ((int)dwPOV > (2250 * i) && (int)dwPOV <= (2250 * (i + 2)))
				return i >>1;
		}
		return JOYSTICK_POV_UP;
	}


	/////////////////////////////////
	// Enumerates force feedback effects in a file.
	// [callback]
	//
	// Input:
	//		LPCDIFILEEFFECT			File where the effects go
	//		void* pvRef					Extra stuff
	//
	// Returns:
	//		TRUE if we should continue searching.
	/////////////////////////////////
	BOOL CALLBACK FeedbackEnumerateEffects( LPCDIFILEEFFECT pDIFileEffect, VOID* pvRef )
	{  
		if (!CInputJoystick::pEnumerating) return false;

		HRESULT hr;
		LPDIRECTINPUTEFFECT pFFEffect = NULL;


		// Create the file if we can
		hr = CInputJoystick::pEnumerating->GetDevice()->CreateEffect(pDIFileEffect->GuidEffect, pDIFileEffect->lpDiEffect, &pFFEffect, NULL);
		if (FAILED(hr) || !pFFEffect) 
			return DIENUM_CONTINUE;

		// Add it to our list
		CInputJoystick::pEnumerating->FeedbackAddEffect(pFFEffect);
				
		return DIENUM_CONTINUE;
}

	
	/////////////////////////////////
	// Runs a force feedback effect
	//
	// Input:
	//		int iEffectID					The effect to play
	//		bool bSoloEffect			If this is true, stop all other effects before playing this one
	//
	// Returns:
	//		TRUE if it played right
	/////////////////////////////////
	bool CInputJoystick::FeedbackRunEffect(int iEffectID, bool bSoloEffect) 
	{ 

		// Dont play if FF is off
		if (!this || !pDevice || !bForceFeedback || iEffectID < 0) return false;
		
		// Find the effect
		ForceFeedbackEffect *pCurrent = pFeedbackEffects;
		while (pCurrent)
		{
			// found it
			if (pCurrent->iID == iEffectID) 
				break;
			// keep looking
			pCurrent = pCurrent->pNext;
		}

		// Didnt find
		if (!pCurrent) return false;

		// Stop already running effects
		 if( FAILED(pDevice->SendForceFeedbackCommand(DISFFC_STOPALL)))
			 return false;

		// Play!
		HRESULT hr = pCurrent->pEffect->Start(pCurrent->dwRepeat , (bSoloEffect) ? DIES_SOLO : NULL);

		return (!FAILED(hr));

	}
	
	
	/////////////////////////////////
	// Clears the force feedback effects
	//
	// Input:
	//	none
	// 
	// Returns:
	//	none 
	/////////////////////////////////
	void CInputJoystick::FeedbackEmptyList(void)
	{
		if (!this || !pDevice) return;

		ForceFeedbackEffect *pCurrent = pFeedbackEffects;
		ForceFeedbackEffect *pCurNext = NULL;
	
		while (pCurrent)
		{
			if (pCurrent)
				pCurNext = pCurrent->pNext;

			// Release the effect
			if (pCurrent->pEffect)  
			{
				pCurrent->pEffect->Release();
                pCurrent->pEffect = NULL;
			}

			// Destroy the node itself
			delete pCurrent;

			// Next one
			pCurrent = pCurNext;
		}

		pFeedbackEffects = NULL;
	}

		////////////////////////
	/// Shutdown( )
	////////////////////////
	/// Kill the device.
	///
	/// Input:
	///		none
	//
	/// Returns:
	///		bool		True if everything went OK
	////////////////////////
	bool CInputJoystick::Shutdown(void)
	{
		// Decrement the count...
		nJoystickCount--;
		
		// Empty FF effects
		FeedbackEmptyList();

		// Shutdown as normal
		return CInputDevice::Shutdown();
	}


	/////////////////////////////////
	// Add an FF effect to our linked list
	//
	// Input:
	//		LPDIRECTINPUTEFFECT pFFEffect		Effect to add
	//
	// Returns:
	//		none
	/////////////////////////////////
	void CInputJoystick::FeedbackAddEffect(LPDIRECTINPUTEFFECT pFFEffect)
	{
		if (!this || !pDevice) return;
	
		// New effect.
		ForceFeedbackEffect *pNewEffect = new ForceFeedbackEffect(pFFEffect);
		
		// Get to the last one in the list.
		ForceFeedbackEffect *pCurrent = pFeedbackEffects;
		
		if (!pCurrent)
		{
			// First item
			pFeedbackEffects = pNewEffect;
		}
		else
		{
			while (pCurrent)
			{
				if (!pCurrent->pNext)
				{
					// last one
					pCurrent->pNext = pNewEffect;
					break;
				}	
				pCurrent = pCurrent->pNext;
			}
		}
	}

	/////////////////////////////////
	// Loads feedback effects from a file
	//
	// Input:
	//		char* strFileName		File to load effects from
	//
	// Returns:
	//		TRUE if we loaded OK.
	/////////////////////////////////
	bool CInputJoystick::FeedbackLoadEffects(char *strFileName)
	{
		// Don't load FF effects if FF is off
		if (!this || !pDevice || !bForceFeedback) return false;

		// Set this as the currently enumerating joystick
		CInputJoystick::pEnumerating = this;

	    // Enumerate the effects in the file selected, and create them in the callback
		HRESULT hr = pDevice->EnumEffectsInFile(strFileName, FeedbackEnumerateEffects,  NULL, DIFEF_MODIFYIFNEEDED);
		
		// We no longer need the active joystick
		CInputJoystick::pEnumerating = NULL;

		return (!FAILED(hr));
    }
