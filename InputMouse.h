/*////////////////////////////////////////////////////////////////////////
InputMouse.h		///		February 5, 2004		//		1.00				///
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


#if !defined(Your2DGameHere_CInputMouse)
#define Your2DGameHere_CInputMouse


#include "inputdevice.h"
#include "MemoryMgr.h"

class CInputMouse : public CInputDevice
{

private:

	// Our Immediate Mode buffer.
	DIMOUSESTATE2 MouseState;

	// Coordinates of our "fake" mouse cursor and scroll wheel.
	int nFakeX, nFakeY, nFakeZ;
	
	// Are we showing the Windows mouse cursor?
	bool bShowWindowsCursor;

public:
	
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
	long CheckImmediateInput(DWORD dwInput);

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
	long CheckBufferedInput(DWORD dwInput);







	
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
	bool ClickInRect(DWORD dwInput, RECT &rClickZone, bool bBuffered = true);

	////////////////////////
	/// GetCursorPosition()
	////////////////////////
	/// Get the location of our "fake" mouse cursor.
	///
	/// Input:
	///		none
	///
	/// Returns:
	///		POINT		structure that contains the location of our cursor
	POINT GetCursorPosition(void)
	{
		POINT ptMouse;
		ptMouse.x = nFakeX;
		ptMouse.y = nFakeY;
		return ptMouse;
	}
	
	////////////////////////
	/// GetScroll()
	////////////////////////
	/// Get the position of our scroll wheel
	///
	/// Input:
	///		none
	///
	/// Returns:
	///		int		The position of our "z" axis (scroll wheel)
	int GetScroll(void)
	{
		return nFakeZ;
	}


	////////////////////////
	/// ShowWindowsCursor()
	////////////////////////
	/// Toggle the Windows mouse cursor
	///
	/// Input:
	///		bool bCursor		true/false: should we draw the Windows cursor?
	///
	/// Returns:
	///		none
	void ShowWindowsCursor(bool bCursor)
	{
		bShowWindowsCursor = bCursor;
		ShowCursor(bCursor);
	}



			// Manage memory
			#if _DEBUG
				ManageMemory(CInputMouse, CORETYPE_INPUTDEVICE_MOUSE);
			#endif

};

#endif
