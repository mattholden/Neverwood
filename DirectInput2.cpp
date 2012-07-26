/*////////////////////////////////////////////////////////////////////////
DirectInput.cpp	///		February 5, 2004		//		1.00				///
//////////////////////////////////////////////////////////////////////////

This class provides a liason between Y2DGH and DirectInput 8+. It will
contain our InputDevice instances as well as the DirectInput 8 object,
exposing functionality as needed. This class, and the ActionMap class
that uses it, should be the sole method of input device polling in the 
project.

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


// Our own headers
#include "DirectInput2.h"
#include "InputKeyboard.h"
#include "InputMouse.h"
#include "InputJoystick.h"

// Important stuff to make DirectInput go
#include <dinput.h>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

// Static stuff
CInputKeyboard DirectInput::idKeyboard;
CInputMouse DirectInput::idMouse;
CInputJoystick DirectInput::idJoysticks[MAX_JOYSTICKS];
LPDIRECTINPUT8 DirectInput::pDInputObject = NULL;
HWND DirectInput::hWnd = NULL;
HINSTANCE DirectInput::hInstance = NULL;

	////////////////////////
	/// Poll ( )
	////////////////////////
	/// Performs all per-loop operations necessary for our InputDevices.
	/// Includes device acquisition, immediate input gathering, and 
	/// buffered input filling.
	///
	/// Input:
	///		none
	///
	/// Returns:
	///		none
	////////////////////////
	void DirectInput::Poll(void)
	{
		if (!pDInputObject) return;

		// Poll keyboard and mouse
		idKeyboard.Poll();
		idMouse.Poll();

		// Fill keyboard/mouse buffers
		idKeyboard.FillBuffer();
		idMouse.FillBuffer();

		// Handle joysticks..
		for (int i = 0; i < CInputJoystick::GetJoystickCount(); i++)
		{
			idJoysticks[i].Poll();
			idJoysticks[i].FillBuffer();
		}
	}

	////////////////////////
	/// Initialize ( )
	////////////////////////
	/// Sets up DirectInput for use. First, initialize the DirectInput8 object.
	/// Then, use it to enumerate joysticks and create all our devices.
	///
	/// Input:
	///		HWND HWnd		Handle to the application window.
	///		HINSTANCE HInst	Handle to the window's instance.
	///
	/// Returns:
	///		bool			true if everything went OK.
	////////////////////////
	bool DirectInput::Initialize(HWND HWnd, HINSTANCE HInst, bool bKillWinKey)
	{
		// Save our window's details for use later.
		hWnd = HWnd;
		hInstance = HInst;

		bool bNoErrors = true;

		// Initialize the DirectInput 8 object. Save it in pDInputObject.
		if FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&pDInputObject, NULL))
			return false;

		// Initialize the keyboard (check for failure)
		idKeyboard.bKillWindowsKey = bKillWinKey;
		if (!idKeyboard.Initialize(hWnd, pDInputObject)) 
			bNoErrors = false;

		// Initialize the mouse. (check for failure)
		if (!idMouse.Initialize(hWnd, pDInputObject))
			bNoErrors = false;
	
		// Now, the tricky part. We need to "enumerate" the joysticks, which is a fancy way to say
		// that we need to count how many there are and set them all up. To do this, we have to use
		// this handy, if convoluted, callback function (think recursion...) In this case, the function we're calling
		// as a 'callback' is DirectInput::EnumerateJoysticks(). This line of code is actually telling DirectInput:
		// "Find every last game pad you know about that's attached and call EnumerateJoysticks on it."
		if (FAILED(pDInputObject->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumerateJoysticks, NULL, DIEDFL_ATTACHEDONLY))) 
			bNoErrors = false;
		
		// Everything went well..
		return bNoErrors;
	}

	////////////////////////
	/// Shutdown ( )
	////////////////////////
	/// Shuts down DirectInput object and all devices.
	/// You should still call DeleteInstance( ) on the DirectInput class after this call.
	///
	/// Input:
	///		none
	///
	/// Returns:
	///		bool		true if there were no errors
	////////////////////////
	bool DirectInput::Shutdown(void)
	{
		
		// Catch errors
		bool bErrors = true;

		// Shut down all joysticks.
		for (int i = 0; i < CInputJoystick::GetJoystickCount(); i++)
			if (!idJoysticks[i].Shutdown()) 
				bErrors = false;

		// Shut down mouse.
		if (!idMouse.Shutdown()) 
			bErrors = false;

		// Shut down keyboard
		if (!idKeyboard.Shutdown())
			bErrors = false;

		// Shut down DirectInput itself.
		pDInputObject->Release();
		pDInputObject = NULL;

		// Return how we did.
		return bErrors;
	}

	////////////////////////
	/// EnumerateJoysticks ( ) 
	/// [ callback, friend ]
	////////////////////////
	/// This function will hunt down all of our joysticks and set them up.
	/// It is not actually a member of DirectInput, but a friend function.
	///
	/// Input:
	///		const DIDEVICEINSTANCE *pDInstance			
	///			Some device we found.
	///		void *pVoid
	///			A pointer to some extra data we might need
	///
	/// Returns:
	///		BOOL			returns DIENUM_STOP if we don't find any more joysticks.
	///						DIENUM_CONTINUE means we found a joystick and should
	///						look for more.
	////////////////////////
	BOOL CALLBACK EnumerateJoysticks(const DIDEVICEINSTANCE *pDInstance, void *pVoid)
	{
		// when we've found them all, or found as many as we're going to hold
		if (!pDInstance || DirectInput::GetJoystickCount() >= MAX_JOYSTICKS)
			return DIENUM_STOP;

		// We found a joystick! Let's get its GUID and its name for setup purposes.
		DirectInput::GetNextJoystick()->SetGuid(pDInstance->guidInstance);
		
		// Set the device name.
		char *buffer = new char[260];
		strncpy(buffer, pDInstance->tszProductName, 260);
		DirectInput::GetNextJoystick()->SetDeviceName(buffer);
		delete [ ] buffer;

		// Initialize the joystick. Note that this will increment the GetJoystickCount().
		DirectInput::GetNextJoystick()->Initialize(
			DirectInput::GetHWnd(), DirectInput::GetDirectInputObject());

		int nJoystickCount = DirectInput::GetJoystickCount();
		// Give the joystick a default range and dead zone. This can be changed by using GetJoystick().
		DirectInput::GetJoystick(nJoystickCount - 1)->SetRanges(DEFAULT_JOYSTICK_MAX_RANGE);
		DirectInput::GetJoystick(nJoystickCount - 1)->SetDeadZone(DEFAULT_JOYSTICK_DEAD_ZONE);
		

		// Look for more!
		return DIENUM_CONTINUE;
	}
	