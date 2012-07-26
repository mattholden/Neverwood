/*////////////////////////////////////////////////////////////////////////
InputJoystick.h		///		February 5, 2004		//		1.00				///
//////////////////////////////////////////////////////////////////////////

This class defines a joystick class for use with DirectInput 8+. 

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


#if !defined(Your2DGameHere_CInputJoystick)
#define Your2DGameHere_CInputJoystick


#include "inputdevice.h"
#include "MemoryMgr.h"
#pragma comment (lib, "dxguid.lib")

	// We'll make the ForceFeedbackEffect structure local to the joystick; no one else needs it.
	//#include "InputJoystickFeedback.h"

class CInputJoystick : public CInputDevice
{

private:

			// Manage memory
			#if _DEBUG
				ManageMemory(CInputJoystick, CORETYPE_INPUTDEVICE_JOYSTICK);
			#endif

			////////////////////////////////////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////////////////////////////////
			// Defines a force feedback effect. 
			// Done as a class for the mem manager's benefit
			class ForceFeedbackEffect
			{
				friend class CInputJoystick;

				// How many times to repeat?
				DWORD dwRepeat;

				bool bActive : 1;

				// The effect
				LPDIRECTINPUTEFFECT pEffect;

				// Linked list
				ForceFeedbackEffect *pNext; 

				// ID
				int iID;
				static int siEffectCount;

				// Construct! :)
				ForceFeedbackEffect(void) { iID = siEffectCount; siEffectCount++; dwRepeat = 1; bActive = false; pEffect = NULL; pNext = NULL;}
				ForceFeedbackEffect(LPDIRECTINPUTEFFECT pFFEffect) { iID = siEffectCount; siEffectCount++; dwRepeat = 1; pEffect = pFFEffect; bActive = true;  pNext = NULL;}

				// Destroy.
				~ForceFeedbackEffect(void) { siEffectCount--; dwRepeat = 0; bActive = false; pEffect = NULL; iID = -1; pNext = NULL;}

				// Manage memory
				#if _DEBUG
					ManageMemory(ForceFeedbackEffect, CORETYPE_INPUT_FFEFFECT);
				#endif

			};
			////////////////////////////////////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////////////////////////////////










	// Force feedback effects:
	ForceFeedbackEffect *pFeedbackEffects;

	// Our Immediate Mode buffer.
	DIJOYSTATE2 JoystickState;

	// The joystick currently having effects enumerated for it. I know this is kind of a wierd syntax; sorry
	public: static CInputJoystick *pEnumerating; private:

	// The GUID of this joystick.
	GUID JoystickGUID;

	// Whether or not to play force feedback events.
	bool bForceFeedback;
	bool bSupportsForceFeedback;

	// How many joysticks are currently active on the system.
	static nJoystickCount;

	// The name of the device. This will be handy when action mapping.
	char czDeviceName[260];
	
	// When we're using the joystick as a pointer device....
	int nPointerX, nPointerY;
	float fPointerSensitivity;
	bool UsingPointer;

	// Store our ranges.
	int nMaxX, nMaxY, nMaxRX, nMaxRY, nDeadZone;

public:

	bool IsFeedbackOn(void) { return bForceFeedback; }
	
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
	///		For the joystick, use 0-127 for the buttons and the Axis enumeration
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
	///		For the joystick, use 0-127 for the buttons and the Axis enumeration
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
	///		int nRangeMaxRy	Maximum range in the Ry (default = same as X)
	///
	/// Returns:
	///		bool		true if we succeeded
	////////////////////////
	bool SetRanges(int RangeMaxX, int RangeMaxY = -1, int RangeMaxRx = -1, int RangeMaxRy = -1);

	
	////////////////////////
	/// SetGuid ( )
	////////////////////////
	/// Set the Globally Unique Identifier (GUID) of the joystick.
	/// This should be done in the EnumerateJoysticks callback function and
	/// BEFORE the joystick's Initialize().
	///
	/// Input:
	///		GUID NewGuid			The guid for this joystick
	///
	/// Returns:
	///		none
	////////////////////////
	void SetGuid(GUID NewGuid)
	{
		JoystickGUID = NewGuid;
	}

	
	////////////////////////
	/// GetJoystickCount ( ) 
	/// [ static ]
	////////////////////////
	/// Gets a count of the active joysticks in the system
	///
	/// Input:
	///		none
	///
	/// Returns:
	///		int nJoystickCount		How many joysticks are hooked to DirectInput right now?
	////////////////////////
	static int GetJoystickCount(void)
	{
		return nJoystickCount;
	}

	////////////////////////
	/// GetCurrentJoystick ( ) 
	/// [ static ]
	////////////////////////
	/// Get the joystick currently being worked on.
	///
	/// Input:
	///		none
	///
	/// Returns:
	///		CInputJoystick *pEnumerating			the current joystick
	////////////////////////
	static CInputJoystick *GetCurrentJoystick(void)
	{
		return pEnumerating;
	}
	
	////////////////////////
	/// ToggleForceFeedback ( )
	////////////////////////
	/// Turn Force Feedback on or off. 
	///
	/// Input:
	///		bool bForce		true if force feedback should be turned on
	///
	/// Returns:
	///		none
	////////////////////////
	void ToggleForceFeedback(bool bForce)
	{
		bForceFeedback = bForce;
	}

	////////////////////////
	/// SupportsForceFeedback ( )
	////////////////////////
	/// Returns whether or not the joystick handles force feedback.
	///
	/// Input:
	///		none
	///
	/// Returns:
	///		bool		True if Force Feedback is supported.
	////////////////////////
	bool SupportsForceFeedback(void)
	{
		return bSupportsForceFeedback;
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
	private:
	void FeedbackEmptyList(void);
	public:


	////////////////////////
	/// SetPointerSensitivity
	////////////////////////
	/// Determines how much to read from the joystick axis when we're using it like a pointer.
	///
	/// Input:
	///		int nSensitivity		Should be a percentage value between 1 (lowest) and 100(highest).
	///								
	///
	/// Returns:
	///		none
	////////////////////////
	void SetPointerSensitivity(int nSensitivity)
	{
		if (nSensitivity > 100) nSensitivity = 100;
		if (nSensitivity < 0) nSensitivity = 0;
		fPointerSensitivity = nSensitivity / 100.0f;
	}

	////////////////////////
	/// SetDeviceName ( )
	////////////////////////
	/// Sets the name of the device
	///
	/// Input:
	///		char *czName			the name of the device
	///
	/// Returns:
	///		none
	////////////////////////
	void SetDeviceName(char *czName)
	{
		strncpy(czDeviceName, czName, 260);
	}

	////////////////////////
	/// GetDeviceName ( )
	////////////////////////
	/// Gets the name of the device
	///
	/// Input:
	///		none
	///
	/// Returns:
	///		char *czDeviceName			the name of the device
	////////////////////////
	char* GetDeviceName(void)
	{
		return czDeviceName;
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
	bool SetDeadZone(DWORD dwDeadZone);

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
	bool Shutdown(void);


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
	static int SimplifyPOV(DWORD dwPOV);

	
	////////////////////////
	/// GetMaximum () 
	////////////////////////
	/// Check the maximum value of an axis in any direction
	///
	/// Input:
	///		int Axis		Which axis you want (use the #defines from InputDevice.h)
	//
	/// Returns:
	///		The maximum value along that axis.
	////////////////////////
	int GetMaximum(int Axis)
	{
		switch (Axis)
		{
		case (JOYSTICK_AXIS_LX): return nMaxX;
		case (JOYSTICK_AXIS_LY): return nMaxY;
		case (JOYSTICK_AXIS_RX): return nMaxRX;
		case (JOYSTICK_AXIS_RY): return nMaxRY;
		default: return NULL;
		}
	}


	////////////////////////
	/// GetMinimum () 
	////////////////////////
	/// Check the minimum value of an axis in any direction
	///
	/// Input:
	///		int Axis		Which axis you want (use the #defines from InputDevice.h)
	//
	/// Returns:
	///		The minimum value along that axis.
	////////////////////////
	int GetMinimum(int Axis)
	{
		switch (Axis)
		{
		case (JOYSTICK_AXIS_LX): return nMaxX * -1;
		case (JOYSTICK_AXIS_LY): return nMaxY * -1;
		case (JOYSTICK_AXIS_RX): return nMaxRX * -1;
		case (JOYSTICK_AXIS_RY): return nMaxRY * -1;
		default: return NULL;
		}
	}

	////////////////////////
	/// GetDeadZone() 
	////////////////////////
	/// Get the "Dead zone" (range around the center that we don't read data from).
	///
	/// Input:
	///		none
	//
	/// Returns:
	///		The dead zone value
	////////////////////////
	int GetDeadZone(void)
	{
		return nDeadZone;
	}

	////////////////////////
	/// GetCursorPosition()
	////////////////////////
	/// Get the location of our joystick pointer.
	///
	/// Input:
	///		none
	///
	/// Returns:
	///		POINT		structure that contains the location of our cursor
	POINT GetCursorPosition(void)
	{
		POINT ptStick;
		ptStick.x = nPointerX;
		ptStick.y = nPointerY;
		return ptStick;
	}


	

		
	/////////////////////////////////
	// Stops all feedback effects on this joystick
	//
	// Input:
	//		none
	//
	// Returns:
	//		TRUE if everything went OK
	/////////////////////////////////
	bool CInputJoystick::FeedbackStop(void)
	{
		// Don't  if FF is off
		if (!bForceFeedback) 
			return false;

		return !(FAILED(pDevice->SendForceFeedbackCommand(DISFFC_STOPALL)));
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
	bool CInputJoystick::FeedbackRunEffect(int iEffectID, bool bSoloEffect);

	 
	/////////////////////////////////
	// Loads feedback effects from a file
	//
	// Input:
	//		char* strFileName		File to load effects from
	//
	// Returns:
	//		TRUE if we loaded OK.
	/////////////////////////////////
	bool CInputJoystick::FeedbackLoadEffects(char *strFileName);

	
	/////////////////////////////////
	// Add an FF effect to our linked list
	//
	// Input:
	//		LPDIRECTINPUTEFFECT pFFEffect		Effect to add
	//
	// Returns:
	//		none
	/////////////////////////////////
	void CInputJoystick::FeedbackAddEffect(LPDIRECTINPUTEFFECT pFFEffect);


			
};

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
	BOOL CALLBACK FeedbackEnumerateEffects( LPCDIFILEEFFECT pDIFileEffect, VOID* pvRef );



#endif
