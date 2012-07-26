/*////////////////////////////////////////////////////////////////////////
InputKeyboard.cpp		///		February 5, 2004		//		1.00		///
//////////////////////////////////////////////////////////////////////////

This class defines a keyboard class for use with DirectInput 8+. 

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


#include "inputkeyboard.h"
#include <dinput.h>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")






	////////////////////////
	/// Initialize( )
	////////////////////////
	/// Sets us up the device.
	///
	/// Input:
	///		HWND hWnd							Handle to the window
	///		LPDIRECTINPUT8 pDInput			The DirectInput object
	//
	/// Returns:
	///		bool		true if everything set up OK.
	////////////////////////
	bool CInputKeyboard::Initialize(HWND hWnd, LPDIRECTINPUT8 pDInput)
	{
		// Our HRESULT we'll re-use.
		HRESULT hResult = NULL;

		// Tell the base class what kind of device it is.
		eDeviceType = DEVICE_KEYBOARD;

		// Create the device.
		hResult = pDInput->CreateDevice(GUID_SysKeyboard, &pDevice, NULL);
		if (FAILED(hResult)) return false;

		// Set data format so the device knows what it is.
		hResult = pDevice->SetDataFormat(&c_dfDIKeyboard);
		if (FAILED(hResult)) return false;

		// Kill windows key?
		bKillWindowsKey = false;

		// String input.
		//cStringInput = "\0";
		//iStringInputMode = INPUT_STRING_NONE;

		// Set cooperative level.
		hResult = CooperativeLevel(hWnd, false, false);
		if (FAILED(hResult)) return false;

		// Null out the immediate input buffer.
		memset((void*)cKeyBuffer, 0, sizeof(cKeyBuffer));

		// Prepare our buffer for buffered input.
		SetupBuffer();

		// Acquire the device.
		hResult = pDevice->Acquire();
		if (FAILED(hResult)) return false;

		// If we get here, everything went smoothly!
		return true;
	}


	////////////////////////
	/// CooperativeLevel ( ) 
	////////////////////////
	/// Set the cooperative level of our device.
	///
	/// Input:
	///		HWND hWnd	
	///		Handle to the current window. The DirectInput main class will provide this parameter for us.
	///
	///		bool bExclusive		
	///		If this is set to 'true', your game/application will be the only thing that can read from the device.
	///		This is usually good for joysticks and bad for keyboards/mice. Set to 'false' by default.
	///
	///		bool bBackground
	///		If this is set to 'true', the device will accept input while the game/application is not the currently
	///		active application. This is usually irritating for gameplay, and is set to 'false' by default.
	//
	/// Returns:
	///		HRESULT		The results of our Acquire() call
	////////////////////////
	HRESULT CInputKeyboard::CooperativeLevel(HWND hWnd, bool BExclusive, bool BBackground)
	{
		DWORD dwCoop = NULL;
		bExclusive = BExclusive;
		bBackground = BBackground;
		dwCoop |= (bExclusive) ? DISCL_EXCLUSIVE : DISCL_NONEXCLUSIVE;
		dwCoop |= (bBackground) ? DISCL_BACKGROUND : DISCL_FOREGROUND;
		if ( bKillWindowsKey && !bExclusive && !bBackground )
			dwCoop |= DISCL_NOWINKEY;
		
		HRESULT hResult;
		hResult =  pDevice->SetCooperativeLevel(hWnd, dwCoop);
		return hResult;
	}
	
	////////////////////////
	/// Poll ( )
	////////////////////////
	/// Make sure the device is acquired and working.
	/// Also should handle the filling of the immediate mode "buffer".
	///
	/// Input:
	///		none
	//
	/// Returns:
	///		bool		Whether or not it's acquired and ready for our use
	////////////////////////
	bool CInputKeyboard::Poll(void)
	{
			// If the keyboard isn't set up yet...
			if (!pDevice || !this) 
				return false;

			// Blank out the immediate "buffer" to hold our data.
			memset(&cKeyBuffer, 0, sizeof(cKeyBuffer));

	        // Try to get the immediate input. If we fail, we know we need to re-Acquire().
			HRESULT hResult = pDevice->GetDeviceState(sizeof(cKeyBuffer), &cKeyBuffer);

			// Something got messed up. Try to reacquire
			if (hResult == DIERR_NOTACQUIRED || hResult == DIERR_INPUTLOST)
				pDevice->Acquire();

			// Otherwise, we're all set!
			return true;
	}

	
	
	////////////////////////
	/// CheckBufferedInput ( )
	////////////////////////
	/// Check for a given key, axis or button in the BUFFERED input stream.
	/// Make sure that FillBuffer() is called every game loop, -ONCE- before any/all
	/// calls to CheckBufferedInput.
	///
	/// Input:
	///		DWORD dwInput
	///		This is what we're looking for. 
	///		Use any of the DIK_ codes for the keyboard.
	///
	/// Returns:
	///		DWORD		This will be 0 if dwInput was not found in the buffer. 
	///						For on/off inputs (keys, joystick/mouse buttons) you can simply 
	///						say if (CheckBufferedInput(input)) and that will be good enough.
	////////////////////////
	long CInputKeyboard::CheckBufferedInput(DWORD dwInput)
	{
		if (!this || !pDevice) return false;

		// Check every input in the buffer. If we find that the key was pressed, return the high bit
		// (just for consistency with CheckImmediateInput). If we don't find it, return NULL.
		for (int i = 0; i < int(dwBufferSize); i++)
		{
			if (InputBuffer[i].dwOfs == dwInput)
				if (InputBuffer[i].dwData & 0x80)
					return 0x80;
		}
		return NULL;
	}
