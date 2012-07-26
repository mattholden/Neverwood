/*////////////////////////////////////////////////////////////////////////
Action.CPP			///		February 5, 2004		//		1.00				///
//////////////////////////////////////////////////////////////////////////

This class defines a CAction for use in our DirectInput action map. It
contains support for mouse, joystick and keybaord inputs all mapped 
to an action simultaneously. We will be using an array of these in our 
Action Map in order to create selectable actions. Declare new actions
using the constructor, in MappedActions.h.

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


// Windows header. Used for input -> character mapping
#include "windows.h"	

// For the DeviceType enumeration and Axes.
#include "InputDevice.h"

// Our own header.
#include "actionmap.h"




	////////////////////////
	/// GetJoystickInputText()
	////////////////////////
	/// Gets the default input for a joystick.
	/// PRIVATE.
	///
	/// Input:
	///		char*	czBuffer		This will be filled with the string.
	///
	///	 Output:
	///		char*	czBuffer		This will be filled with the string.
	///
	/// Returns:
	///		none
	////////////////////////
void ActionMap::CAction::GetJoystickInputText(char *czBuffer)
	{

			// This much is a given.
			strcpy(czBuffer, "Joystick ");

			//////////////////////////////////////////////////////////////////////////////
			// Buttons
			//////////////////////////////////////////////////////////////////////////////
			if (dwCurrent[DEVICE_JOYSTICK] >= 0 && dwCurrent[DEVICE_JOYSTICK] < 128)
			{
				// Append "Button" to the string
				strcpy(&czBuffer[9], "Button ");

				// Append the button #
				itoa(dwCurrent[DEVICE_JOYSTICK], &czBuffer[16], 10);

				// Null terminate dependent on how many digits the number is.
				if (dwCurrent[DEVICE_JOYSTICK] > 99) 
					czBuffer[19] = '\0';
				else if (dwCurrent[DEVICE_JOYSTICK] > 9) 
					czBuffer[18] = '\0';
				else
					czBuffer[17] = '\0';

				return;
			}

			//////////////////////////////////////////////////////////////////////////////////
			// Axes and Directional Pads
			// Note: My sincere apologies for the hackish nature of this gargantuan 
			// switch statement. If you know of a cleaner way to do this, please suggest!
			//////////////////////////////////////////////////////////////////////////////////
			switch (dwCurrent[DEVICE_JOYSTICK])
			{

				// General axis checks
				case JOYSTICK_AXIS_LX :
				case JOYSTICK_AXIS_LY :
					strcpy(&czBuffer[9], "Left Analog\0");
					break; 
				
				case JOYSTICK_AXIS_RX :
				case JOYSTICK_AXIS_RY :
					strcpy(&czBuffer[9], "Right Analog\0");
					break;

				case JOYSTICK_DPAD : 
					strcpy(&czBuffer[9], "D-Pad\0");
					break;
				
				case JOYSTICK_POV(2):
					strcpy(&czBuffer[9], "PoV Hat 2\0");
					break;
				case JOYSTICK_POV(3):
					strcpy(&czBuffer[9], "PoV Hat 3\0");
					break;
				case JOYSTICK_POV(4):
					strcpy(&czBuffer[9], "PoV Hat 4\0");
					break;
		
			}
				
		
	}


	////////////////////////
	/// GetMouseInputText()
	////////////////////////
	/// Gets name of the currently mapped mouse action
	/// PRIVATE.
	///
	/// Input:
	///		char*	czBuffer		This will be filled with the string.
	///
	///	 Output:
	///		char*	czBuffer		This will be filled with the string.
	///
	/// Returns:
	///		none
	////////////////////////
	void ActionMap::CAction::GetMouseInputText(char *czBuffer)
	{

			// This much is a given.
			strcpy(czBuffer, "Mouse ");

			//////////////////////////////////////////////////////////////////////////////
			// Buttons
			//////////////////////////////////////////////////////////////////////////////
			// Note: My sincere apologies for the hackish nature of this gargantuan 
			// switch statement. If you know of a cleaner way to do this, please suggest!
			
			switch (dwCurrent[DEVICE_MOUSE])
			{

			// First, the pretty looking buttons that users would expect.
			case 0 :
				strcpy(&czBuffer[7], "Left Button\0");
				break;
			case 1 :
				strcpy(&czBuffer[7], "Right Button\0");
				break;
			case 2 :
				strcpy(&czBuffer[7], "Middle Button\0");
				break;

			// Now, any extraneous mouse buttons.
			default :
				strcpy(&czBuffer[7], "Button ");
				itoa(dwCurrent[DEVICE_MOUSE], &czBuffer[14], 10);
				czBuffer[15] = '\0';
				break;

			//////////////////////////////////////////////////////////////////////////////////
			// Movement
			// Since the mouse's motion is rarely observed (outside of DirectInput) as a 
			// change in X or Y, and the player almost certainly doesn't view it that way,
			// the mouse is handled primarily with one "axis", called MOUSEMOVEMENT, 
			// which allows us to deal with all four directions. A special case is made for 
			// scroll wheels. MOUSEAXIS_Z and MOUSEAXIS_SCROLL are the same value.
			//////////////////////////////////////////////////////////////////////////////////
			
			case MOUSEMOVEMENT :
				strcpy(&czBuffer[7], "Movement\0");
				break;

			case MOUSEAXIS_SCROLL :
				strcpy(&czBuffer[7], "Scroll Wheel\0");
				break;
					
			case MOUSEAXIS_SCROLLUP : 
				strcpy(&czBuffer[7], "Scroll Up\0");
				break;
			
			case MOUSEAXIS_SCROLLDOWN : 
				strcpy(&czBuffer[7], "Scroll Down\0");
				break;
				
				
			}	// End mouse Uberswitch
	}




	////////////////////////
	/// RestoreDefaults()
	////////////////////////
	/// Sets this action to its default value.
	///
	/// Input:
	///		DeviceType eDevice		
	///		Which device you want. See the DeviceTypes enumeration for help.
	///		Use the default parameter (pass in no parameters) to reset all devices.
	///
	/// Returns:
	///		DWORD	The default input for this action. 
	///		Will return NULL if the default parameter is used.
	////////////////////////
	DWORD ActionMap::CAction::RestoreDefaults(DeviceType eDevice)
	{
		// If it's the default value, reset them all and return nothing.
		if (eDevice == -1)
		{
			for (int i = DEVICE_KEYBOARD; i <= DEVICE_JOYSTICK; i++)
				dwCurrent[i] = dwDefault[i];
			return NULL;
		}
		
		// Otherwise, just restore the device you gave me, and return it.
		dwCurrent[eDevice] = dwDefault[eDevice];
		return dwCurrent[eDevice];
	}


	////////////////////////
	/// SetActionName()
	////////////////////////
	/// Sets the name of this action. If your string is longer than 20 characters,
	/// it will be clipped.
	///
	/// Input:
	///		char *czString					The desired name.
	///
	/// Returns:
	///		none
	////////////////////////
	void ActionMap::CAction::SetActionName(char* czString)
	{
		// If we get an empty string, clear ours nicely.
		if (!int(strlen(czString))) 
			czActionName[0] = '\0';
		else
		{
			// Otherwise, copy the 1st 20 characters and make sure we null terminate.
			strncpy(czActionName, czString, 20);
			czActionName[20] = '\0';
		}
	}


	////////////////////////
	/// GetActionText()
	////////////////////////
	/// Gets a text representation of the key or axis/button this action
	///	 is currently mapped to for the selected device. Primarily implemented
	///	 for on-screen display during control configuration.
	///
	/// Input:
	///		DeviceType eDevice			Which device to check
	///		char*	czBuffer					This will be filled with the string.
	///
	///	 Output:
	///		char*	czBuffer		This will be filled with the string.
	///
	/// Returns:
	///		none
	////////////////////////
	void ActionMap::CAction::GetActionText(DeviceType eDevice, char *czBuffer)
	{
		switch(eDevice)
		{
			// Keyboard
			case DEVICE_KEYBOARD:
			{
				UINT unKey = MapVirtualKeyEx(dwCurrent[0], 1, NULL);
				GetKeyNameText(unKey, czBuffer, 21);
				break;
			}

			// Mouse
			case DEVICE_MOUSE:
				GetMouseInputText(czBuffer);
				break;

			// Joystick
			case DEVICE_JOYSTICK:
				GetJoystickInputText(czBuffer);	
				break;
		}
	}

	////////////////////////
	/// SetDefaults()
	////////////////////////
	/// Sets up all of our default values, which will be currents by default. 
	/// Hey, who'd have thought? :)
	///
	/// Input:
	///		char *czString					The desired name. It'll be truncated to 20 characters!
	///		DWORD dwKeyboard		Default keyboard setting for this action
	///		DWORD dwMouse			Default mouse setting for this action
	///		DWORD dwJoystick			Default joystick setting for this action
	///
	/// Returns:
	///		none
	////////////////////////
	void ActionMap::CAction::SetDefaults(char* czString, DWORD dwKeyboard, DWORD dwMouse, DWORD dwJoystick)
	{
		// Set the name...
		SetActionName(czString);

		// Set the default actions.
		SetDefault(DEVICE_KEYBOARD, dwKeyboard);
		SetDefault(DEVICE_MOUSE, dwMouse);
		SetDefault(DEVICE_JOYSTICK, dwJoystick);

		// Set all the currents equal to the defaults
		RestoreDefaults();
	}
