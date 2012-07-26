

#if !defined(Y2DGH_DirectInput)
#define Y2DGH_DirectInput


// InputDevices...
#include "InputKeyboard.h"
#include "InputMouse.h"
#include "InputJoystick.h"

// Memory management
#include "MemoryMgr.h"

#include <dinput.h>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")



class DirectInput 
{

private:

	// System keyboard and mouse. There shouldn't ever be more than one of each.
	static CInputKeyboard idKeyboard;
	static CInputMouse idMouse;

	// Our joysticks. We'll support up to eight. Dynamic memory in this situation is a royal pain.
	static CInputJoystick idJoysticks[MAX_JOYSTICKS];

	// Our DirectInput object.
	static LPDIRECTINPUT8 pDInputObject;

	// Things we need to know about our window.
	static HWND hWnd;
	static HINSTANCE hInstance;

	////////////////////////
	/// GetNextJoystick ( )
	////////////////////////
	/// Returns a pointer to the next joystick to be created. 
	/// If this is NULL, we've hit our max joysticks
	///
	/// Input:
	///		none
	///
	/// Returns:
	///		CInputJoystick *	Our joystick, or NULL if they asked for a bad one.
	////////////////////////
	static CInputJoystick *GetNextJoystick(void)
	{
		return (CInputJoystick::GetJoystickCount() >= MAX_JOYSTICKS) ? NULL : &idJoysticks[CInputJoystick::GetJoystickCount()];
	}


public:

	// Manage memory.
	#if _DEBUG
		ManageMemory(DirectInput, CORETYPE_DIRECTINPUT);
	#endif

	////////////////////////
	/// GetHWnd ( )
	////////////////////////
	/// Returns the handle to the window that DirectInput is linked to.
	///
	/// Input:
	///		none
	///
	/// Returns:
	///		HWND hWnd		Our local window's handle
	////////////////////////
	static HWND GetHWnd(void)
	{
		return hWnd;
	}

	////////////////////////
	/// GetDirectInputObject ( )
	////////////////////////
	/// Returns a pointer to our DirectInput object. This shouldn't
	/// be needed much outside the class but is here for completeness.
	///
	/// Input:
	///		none
	///
	/// Returns:
	///		LPDIRECTINPUT8 pDInputObject		Our DirectInput object
	////////////////////////
	static LPDIRECTINPUT8 GetDirectInputObject(void)
	{
		return pDInputObject;
	}

	////////////////////////
	/// GetKeyboard ( )
	////////////////////////
	/// Returns a pointer to our InputKeyboard, to access functionality 
	/// that may not be explicitly passed through DirectInput.
	///
	/// Input:
	///		none
	///
	/// Returns:
	///		CInputKeyboard *	Our keyboard
	////////////////////////
	static CInputKeyboard *GetKeyboard(void)
	{
		return &idKeyboard;
	}


	////////////////////////
	/// GetMouse ( )
	////////////////////////
	/// Returns a pointer to our InputMouse, to access functionality 
	/// that may not be explicitly passed through DirectInput.
	///
	/// Input:
	///		none
	///
	/// Returns:
	///		CInputMouse *	Our Mouse
	////////////////////////
	static CInputMouse *GetMouse(void)
	{
		return &idMouse;
	}

	////////////////////////
	/// GetJoystick ( )
	////////////////////////
	/// Returns a pointer to the requested InputJoystick to get functionality
	/// that may not be explicitly passed through DirectInput.
	///
	/// Input:
	///		int nJoystick		Which joystick they want.
	///
	/// Returns:
	///		CInputJoystick *	Our joystick, or NULL if they asked for a bad one.
	////////////////////////
	static CInputJoystick *GetJoystick(int nJoystick)
	{
		return (nJoystick < 0 || nJoystick >= CInputJoystick::GetJoystickCount()) ? NULL : &idJoysticks[nJoystick];
	}

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
	static void Poll(void);
	

	////////////////////////
	/// GetJoystickCount ( )
	////////////////////////
	/// Returns the number of actively connected joysticks.
	///
	/// Input:
	///		none
	///
	/// Returns:
	///		int			the number of active joysticks
	////////////////////////
	static int GetJoystickCount(void)
	{
		return CInputJoystick::GetJoystickCount();
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
	static bool DirectInput::Initialize(HWND HWnd, HINSTANCE HInst, bool bKillWinKey = true);

	
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
	friend BOOL CALLBACK EnumerateJoysticks(const DIDEVICEINSTANCE *pDInstance, void *pVoid);

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
	static bool DirectInput::Shutdown(void);
	



};

#endif
