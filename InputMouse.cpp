/*////////////////////////////////////////////////////////////////////////
InputMouse.cpp		///		February 5, 2004		//		1.00		///
//////////////////////////////////////////////////////////////////////////

This class defines a mouse class for use with DirectInput 8+. 

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


#include "InputMouse.h"
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
	bool CInputMouse::Initialize(HWND hWnd, LPDIRECTINPUT8 pDInput)
	{
		// Our HRESULT we'll re-use.
		HRESULT hResult = NULL;

		// Tell the base class what kind of device it is.
		eDeviceType = DEVICE_MOUSE;

		// Create the device.
		hResult = pDInput->CreateDevice(GUID_SysMouse, &pDevice, NULL);
		if (FAILED(hResult)) return false;

		// Set data format so the device knows what it is.
		hResult = pDevice->SetDataFormat(&c_dfDIMouse2);
		if (FAILED(hResult)) return false;

		// Set cooperative level.
		hResult = CooperativeLevel(hWnd, false, false);
		if (FAILED(hResult)) return false;

		// Null out the immediate input buffer.
		memset((void*)&MouseState, 0, sizeof(MouseState));

		// Put our "fake cursor" at the origin.
		nFakeX = 0;
		nFakeY = 0;
		nFakeZ = 0;

		// By default, we're showing the Windows cursor.
		bShowWindowsCursor = true;

		// Prepare our buffer for buffered input.
		SetupBuffer();

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
	/// In the case of the mouse, it will also update our "Fake" pointer.
	///
	/// Input:
	///		none
	//
	/// Returns:
	///		bool		Whether or not it's acquired and ready for our use
	////////////////////////
	bool CInputMouse::Poll(void)
	{
			// If the device isn't set up yet...
			if (!pDevice || !this) 
				return false;

			// Blank out the immediate "buffer" to hold our data.
			memset(&MouseState, 0, sizeof(MouseState));

	        // Try to get the immediate input. If we fail, we know we need to re-Acquire().
			HRESULT hResult = pDevice->GetDeviceState(sizeof(MouseState), &MouseState);

			// Something got messed up. Try to reacquire
			if (hResult == DIERR_NOTACQUIRED || hResult == DIERR_INPUTLOST)
				pDevice->Acquire();
			
			// Otherwise, we got some data. Update our "fake" pointer.
			else
			{
				nFakeX += MouseState.lX;	
				if (nFakeX < 0) nFakeX = 0;
				if (nFakeX > GetSystemMetrics(SM_CXSCREEN)) nFakeX = GetSystemMetrics(SM_CXSCREEN);

				nFakeY += MouseState.lY;	
				if (nFakeY < 0) nFakeY = 0;
				if (nFakeY > GetSystemMetrics(SM_CYSCREEN)) nFakeY = GetSystemMetrics(SM_CYSCREEN);

				nFakeZ += MouseState.lZ;	
				if (nFakeZ < 0) nFakeZ = 0;
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
	///		For the mouse, use 0-7 for the buttons and the Axis enumeration
	///
	/// Returns:
	///		DWORD		This will be 0 if dwInput was not found in the buffer. 
	///						For on/off inputs (keys, joystick/mouse buttons) you can simply 
	///						say if (CheckBufferedInput(input)) and that will be good enough.
	///
	///						For things that have ranges, like mouse/joystick actions, it will return
	///						the -amount- of the movement. For example, if you moved the mouse
	///						10 pixels up, and asked for MOUSEAXIS_X, you'd get -10 back from this
	///						function.
	////////////////////////
	long CInputMouse::CheckBufferedInput(DWORD dwInput)
	{
		// Check every input in the buffer. If we find that the key was pressed, return the high bit
		// (just for consistency with CheckImmediateInput). If we don't find it, return NULL.
		if (!this || !pDevice) return false;

		// We need to seperate axes, which don't get the logical & high-bit check, from buttons that do
		switch (dwInput)
		{

		// You're checking for an axis. If there was movement on this axis, return the whole DWORD to show
		// how much movement there was.
		case MOUSEAXIS_X :
		case MOUSEAXIS_Y :
		case MOUSEAXIS_Z :
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
	///		For the mouse, use 0-7 for the buttons and the Axis enumeration
	///
	/// Returns:
	///		DWORD		This will be 0 if dwInput was not found in the buffer. 
	///						For on/off inputs (keys, joystick/mouse buttons) you can simply 
	///						say if (CheckImmediateInput(input)) and that will be good enough.
	///
	///						For things that have ranges, like mouse/joystick actions, it will return
	///						the -amount- of the movement. For example, if you moved the mouse
	///						10 pixels up, and asked for MOUSEAXIS_X, you'd get -10 back from this
	///						function.
	////////////////////////
	long CInputMouse::CheckImmediateInput(DWORD dwInput)
	{
		if (!this || !pDevice) return false;

		// What we return will depend on the type of thing we asked for
		switch (dwInput)
		{

		// if we're checking for an axis...
		case MOUSEAXIS_X :
			return MouseState.lX;
		case MOUSEAXIS_Y :
			return MouseState.lY;
		case MOUSEAXIS_Z :
			return MouseState.lZ;
		
		// We're checking for a button
		default :
			return (MouseState.rgbButtons[dwInput - MOUSE_LEFT] & 0x80);
		}
	}






	////////////////////////
	/// ClickInRect()
	////////////////////////
	/// See if the mouse button is down inside a rect
	///
	/// Input:
	///		DWORD dwInput
	///		This is what we're looking for. 
	///		For the mouse, use 0-7 for the buttons
	//
	///		RECT &rClickZone
	///		The area to check
	///
	///		bool bBuffered
	///		If this is 'true', use buffered input. Otherwise, use immediate to check. Defaults to 'true'.
	///		Make sure the appropriate things are called each loop for the input type.
	///
	/// Returns:
	///		bool			true if the button is down and the pointer is in that rect
	bool CInputMouse::ClickInRect(DWORD dwInput, RECT &rClickZone, bool bBuffered)
	{
		if (!this || !pDevice) return false;

		// If the button isn't clicked at all, it can't be clicked in our rect
		if (bBuffered)
		{
			if (!CheckBufferedInput(dwInput))
				return false;
		}
		else
		{
			if (!CheckImmediateInput(dwInput))
				return false;
		}

		// The button we wanted is down; let's see if the mouse is in our RECT.
		// First, set up a point with our fake X and Y coordinates
		POINT mPt;
		mPt.x = nFakeX;
		mPt.y = nFakeY;

		// See if it's in the rect
		return (PtInRect(&rClickZone, mPt)) ? true : false;
	}