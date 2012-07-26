/*////////////////////////////////////////////////////////////////////////
InputKeyboard.h		///		February 5, 2004		//		1.00				///
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


#if !defined(Your2DGameHere_CInputKeyboard)
#define Your2DGameHere_CInputKeyboard


#include "inputdevice.h"
#include "dinput.h"
#include "MemoryMgr.h"

class CInputKeyboard : public CInputDevice
{

private:

	// Our Immediate Mode key buffer.
	char cKeyBuffer[256];

	// The value we're getting.
//	char cStringInput[256];
//	int iStringInputMode;

public:
	// Windows key?
	bool bKillWindowsKey;
	
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
	bool Initialize(HWND hWnd, LPDIRECTINPUT8 pDInput);


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
	bool Poll(void);

	////////////////////////
	/// CheckImmediateInput ( )	
	////////////////////////
	/// Check for a given key in the IMMEDIATE input stream.
	///
	/// Input:
	///		DWORD dwInput
	///		This is what we're looking for. 
	///		Use any of the DIK_ codes for the keyboard.
	///
	/// Returns:
	///		DWORD		This will be 0 if dwInput was not found in the buffer. 
	///						For on/off inputs (keys, joystick/mouse buttons) you can simply 
	///						say if (CheckImmediateInput(input)) and that will be good enough.
	////////////////////////
	long CheckImmediateInput(DWORD dwInput)
	{
		return (cKeyBuffer[dwInput] & 0x80);
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
	long CheckBufferedInput(DWORD dwInput);



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
	HRESULT CooperativeLevel(HWND hWnd, bool BExclusive = false, bool BBackground = false);


	/////////////////////////
	// DisableWindowsKey ( )
	/////////////////////////
	// Toggles whether or not the Windows key is active.
	// This only works if the keyboard is in Foreground mode and Non-Exclusive mode.
	// This can be set with CInputKeyboard::CooperativeLevel(hWnd);
	//
	// Input:
	//		HWND hWnd		Handle to the window. Hopefully this will be eliminated soon.
	//		bool bWinKey		True disables the Windows key.
	//
	// Return:
	//		bool					True if we were able to disable the key
	///////////////////////////
	bool DisableWindowsKey(HWND hWnd, bool bWinKey)
	{
		if (bExclusive || bBackground) 
			return false;

		bKillWindowsKey = bWinKey;
		return (this->CooperativeLevel(hWnd, bExclusive, bBackground)) ? true : false;
	}



			// Oh, my god, this is the world's biggest hack!
			// Sit in a loop until we get a float.
			// It will text it out to the position you give, but that's it. 
			// If anybody can think of a more elegant solution, PLEASE share it.
			// I am humiliated to have coded this.
			// (Definition in InputMultichar.cpp)
			float GetFloat(int iFont = -1, int iOutputX = 0, int iOutputY = 0);
			


			// Manage memory
			#if _DEBUG
				ManageMemory(CInputKeyboard, CORETYPE_INPUTDEVICE_KEYBOARD);
			#endif

};

#endif
