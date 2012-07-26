/*////////////////////////////////////////////////////////////////////////
InputDevice.h		///		February 5, 2004		//		1.00				///
//////////////////////////////////////////////////////////////////////////

This class defines a base Input Device for use with DirectInput 8+. 
We will derive from it a Keyboard, Mouse, and Joystick class, each
with the same basic interface. This level of granularity prevents a 
pitfall when dealing with two modes (immediate and buffered input)
for three different devices (and in the case of joysticks, more than one
of them!) 

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


#ifndef Your2DGameHere_CInputDevice
#define Your2DGameHere_CInputDevice

#ifndef DIRECTINPUT_VERSION
	#define DIRECTINPUT_VERSION 0x0800
#endif

#include <dinput.h>
#pragma comment(lib, "dinput8.lib")

///////////////////////////////////////////////
// Preferences for how we want input devices to be handled
///////////////////////////////////////////////
// Size of the buffer (in terms of # of inputs to hold) 
#define INPUT_BUFFER_SIZE 20

// Joystick settings
#define MAX_JOYSTICKS 8
#define DEFAULT_JOYSTICK_MAX_RANGE 1000
#define DEFAULT_JOYSTICK_DEAD_ZONE DEFAULT_JOYSTICK_MAX_RANGE / 10
#define DEFAULT_JOYSTICK_POINTER_SENSITIVITY 2.0f

///////////////////////////////////////////////
// A few identifier tags to help things out a little bit.
///////////////////////////////////////////////

	


// Use these in the various functions that ask for a device.
enum DeviceType { DEVICE_ALL = -1, DEVICE_KEYBOARD, DEVICE_MOUSE, DEVICE_JOYSTICK,  };

// Extra DIK keyboard scans, to cover some of the counterintuitive ones.
#define DIK_PAGEUP DIK_PRIOR
#define DIK_PAGEDOWN DIK_NEXT
#define DIK_ENTER DIK_RETURN
#define DIK_ALT DIK_MENU
#define DIK_TILDE DIK_GRAVE


// Axes for joystick and mouse. Begin at 900 and 1000 to make sure we're well
// out of the range of values DirectInput uses for its identifiers.
enum Axis				{	MOUSEMOVEMENT = 900, 
								MOUSEAXIS_X = DIMOFS_X, MOUSEAXIS_Y = DIMOFS_Y, MOUSEAXIS_Z = DIMOFS_Z, 
								MOUSEAXIS_SCROLL = DIMOFS_Z, 
								MOUSEAXIS_SCROLLUP, MOUSEAXIS_SCROLLDOWN,
								
								JOYSTICK_AXIS_X = DIJOFS_X, JOYSTICK_AXIS_Y = DIJOFS_Y, JOYSTICK_AXIS_Z = DIJOFS_Z,
								JOYSTICK_AXIS_LX = JOYSTICK_AXIS_X, JOYSTICK_AXIS_LY = JOYSTICK_AXIS_Y, JOYSTICK_AXIS_LZ = JOYSTICK_AXIS_Z,
								JOYSTICK_AXIS_RY = JOYSTICK_AXIS_Z, JOYSTICK_AXIS_RX = DIJOFS_RZ,
						
							};

// For use with CInputJoystick::SimplifyPOV()
enum POV_Direction {  JOYSTICK_POV_UP_RIGHT, JOYSTICK_POV_RIGHT, JOYSTICK_POV_DOWN_RIGHT, JOYSTICK_POV_DOWN, 
									 JOYSTICK_POV_DOWN_LEFT, JOYSTICK_POV_LEFT, JOYSTICK_POV_UP_LEFT, JOYSTICK_POV_UP, JOYSTICK_POV_NONE,};


// Buttons for our mice and joysticks. These #defines are a little more intuitive than DirectInput's, and add a bit of flexibility within the wrapper classes.
#define MOUSE_BUTTON(button_number) (FIELD_OFFSET(DIMOUSESTATE2, rgbButtons) + button_number)
#define MOUSE_LEFT			MOUSE_BUTTON(0)
#define MOUSE_RIGHT		MOUSE_BUTTON(1)
#define MOUSE_MIDDLE		MOUSE_BUTTON(2)

#define JOYSTICK_BUTTON(button_number)		(FIELD_OFFSET(DIJOYSTATE2, rgbButtons) + button_number)
#define JOYSTICK_POV(pov_number)					FIELD_OFFSET(DIJOYSTATE2, rgdwPOV) + (pov_number) * sizeof(DWORD)
#define JOYSTICK_DPAD JOYSTICK_POV(0)





class CInputDevice
{

protected:

	// Our actual DirectInput device pointer.
	LPDIRECTINPUTDEVICE8 pDevice;

	// What type of device is this? Saves a lot of dynamic_cast ugliness.
	DeviceType eDeviceType;

	// A buffer of 20 inputs, used to catch buffered mode input.
	DIDEVICEOBJECTDATA InputBuffer[INPUT_BUFFER_SIZE];

	// We'll use this to hold the actual size of our buffer right now.
	DWORD dwBufferSize;

	// Cooperative level stuff.
	bool bBackground, bExclusive;


	private: short __PADDING_DONT_USE;
	protected:


	////////////////////////
	/// SetupBuffer ()		[ PROTECTED ] 
	////////////////////////
	/// Sets us up the input buffer for buffered input.
	///
	/// Input:
	///		none
	//
	/// Returns:
	///		none
	////////////////////////
	void SetupBuffer(void); 
	

public:

	////////////////////////
	/// Constructor
	////////////////////////
	/// Set up the device
	///
	/// Input:
	///		none
	//
	/// Returns:
	///		none
	////////////////////////
	CInputDevice(void)
	{
		// Just for safety's sake...
		pDevice = NULL;
	}


	////////////////////////
	/// Destructor
	////////////////////////
	/// Destroy the device
	///
	/// Input:
	///		none
	//
	/// Returns:
	///		none
	////////////////////////
	~CInputDevice(void)
	{
	}


	////////////////////////
	/// Initialize( )	[ PURE VIRTUAL ]
	////////////////////////
	/// Sets us up the device.
	///
	/// Input:
	///		HWND hWnd							Handle to the window
	///		LPDIRECTINPUT8 pDInput			Pointer to the DirectInput object
	//
	/// Returns:
	///		bool		True if everything set up OK
	////////////////////////
	virtual bool Initialize(HWND hWnd, LPDIRECTINPUT8 pDInput) = 0;
	
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
	virtual bool Shutdown(void);



		
	////////////////////////
	/// Poll ( )	[ PURE VIRTUAL ]
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
	virtual bool Poll(void) = 0;

	////////////////////////
	/// CheckImmediateInput ( )	[ PURE VIRTUAL ]
	////////////////////////
	/// Check for a given key, axis or button in the IMMEDIATE input stream.
	///
	/// Input:
	///		DWORD dwInput
	///		This is what we're looking for. 
	///		Use any of the DIK_ codes for the keyboard.
	///		For the mouse, use 0-7 for the buttons and the Axis enumeration
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
	virtual long CheckImmediateInput(DWORD dwInput) = 0;
	
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
	virtual long CheckBufferedInput(DWORD dwInput) = 0;
	











	////////////////////////
	/// GetDeviceType()
	////////////////////////
	/// Gets the device type. 
	///		(Either DEVICE_KEYBOARD, DEVICE_MOUSE, or DEVICE_JOYSTICK.)
	///
	/// Input:
	///		none
	///
	/// Returns:
	///		int			the device type.
	////////////////////////
	int GetDeviceType(void)
	{
		return eDeviceType;
	}
	
	////////////////////////
	/// GetDevice()
	////////////////////////
	/// Gets the device itself. 
	/// This should probably be avoided, unless you need to do something
	/// that the YD2GH classes don't explicitly provide.
	///
	/// Input:
	///		none
	///
	/// Returns:
	///		LPDIRECTINPUTDEVICE8 pDevice			The device.
	////////////////////////
	LPDIRECTINPUTDEVICE8 GetDevice(void)
	{
		return pDevice;
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
	virtual HRESULT CooperativeLevel(HWND hWnd, bool BExclusive = false, bool BBackground = false)
	{
		bExclusive = BExclusive;
		bBackground = BBackground;
		int nExclusive = (bExclusive) ? DISCL_EXCLUSIVE : DISCL_NONEXCLUSIVE;
		int nBackground = (bBackground) ? DISCL_BACKGROUND : DISCL_FOREGROUND;

		HRESULT hResult;
		hResult =  pDevice->SetCooperativeLevel(hWnd, nExclusive | nBackground);
		return hResult;
	}


	////////////////////////
	/// FillBuffer()
	////////////////////////
	/// Fills the buffer (InputBuffer) with data. 
	///	 This should be called ONCE and ONLY ONCE per game loop for each device.
	///	 The number of inputs read in the buffer will be stored in dwBufferSize.
	///
	/// Input:
	///		none
	///
	/// Returns:
	///		none
	////////////////////////
	DWORD FillBuffer(void)
	{	
		
		// If the device isn't set up yet...
			if (!pDevice) 
				return 0;

		dwBufferSize = INPUT_BUFFER_SIZE;
		pDevice->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), InputBuffer, &dwBufferSize, NULL);	
		return dwBufferSize;
	}





};
#endif

