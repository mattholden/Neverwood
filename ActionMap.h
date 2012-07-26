

// Defines an Action Map, which contains all our actions. 
// Represents the complete control structure for a character.


#ifndef StormForge_Matt_ActionMap
#define StormForge_Matt_ActionMap

#include "InputDevice.h"
#include "MemoryMgr.h"


#include "InputJoystick.h"
#include "DirectInput2.h"

class CAction; 


class ActionMap
{
private:

	// How many actions in this map?
	int iActionCount;

	// Which joystick are we mapping to?
	int iJoystickIndex;

	

public:


	// Manage memory
	#if _DEBUG
		ManageMemory(ActionMap, CORETYPE_INPUT_ACTIONMAP);
	#endif


	// Set up.
		ActionMap(void) { iJoystickIndex = -1; pActions = NULL; iActionCount = 0; }
		ActionMap(int iJoystick) { iJoystickIndex = iJoystick; pActions = NULL; iActionCount = 0; }


	// Add an action. Return the "index" 
		int AddAction(char *strActionName,  DWORD dwKeyboard, DWORD dwMouse = NULL, DWORD dwJoystick = NULL);

	// Check for an action
		DWORD ActionMap::CheckAction(int iIndex, DeviceType eDevice = DEVICE_ALL, bool bBuffered = true);

	// Destroy list
		void Shutdown(void) 
		{
			DeleteAction(pActions);
		}

		//int GetJoystickIndex(){return iJoystickIndex;}
	

				// This is specifically included so we can do a switch() to see what action is occurring.
				DWORD ActionMap::CurrentAction(DeviceType eDevice, bool bBuffered);


		void ActionMap::SetJoystick(int iStick) { iJoystickIndex = iStick; }
		
		CInputJoystick*ActionMap::GetJoystick(void)
		{
			return DirectInput::GetJoystick(iJoystickIndex);
		}



			private:
							
			class CAction
			{
				
			private:

				/////////////////////////////////
				// Member Variables
				/////////////////////////////////

				// The name, in words, of what this action does. Will be shown on-screen during action 
				// map configurations. Example: Jump, Shoot, Strafe.
				char czActionName[21];

				// The currently mapped button for the keyboard, mouse, and joystick, respectively.
				DWORD dwCurrent[3];
				
				// The default buttons for the keyboard, mouse, and joystick, respectively.
				// RestoreDefaults() will set the key to this.
				DWORD dwDefault[3];

				// Indexing
				int iIndex;

				/////////////////////////////////
				// Private Member Functions
				/////////////////////////////////


				
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
				void CAction::GetJoystickInputText(char *czBuffer);

				
				////////////////////////
				/// GetMouseInputText()
				////////////////////////
				/// Gets the default input for a mouse
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
				void ActionMap::CAction::GetMouseInputText(char *czBuffer);
				
				

			public:


				// For linked list functionality
				ActionMap::CAction *pNext;

				int ActionMap::CAction::GetIndex(void) { return iIndex; }
				////////////////////////
				/// GetDefault()
				////////////////////////
				/// Gets the default input for a particular device.
				///
				/// Input:
				///		DeviceType eDevice		
				///		Which device you want. See the DeviceTypes enumeration for help.
				///
				/// Returns:
				///		DWORD	The default input for this action.
				////////////////////////
				DWORD GetDefault(DeviceType eDevice)
				{
					return dwDefault[eDevice];
				}

				////////////////////////
				/// GetCurrent()
				////////////////////////
				/// Gets the current input for a particular device.
				///
				/// Input:
				///		DeviceType eDevice		
				//		Which device you want. See the DeviceTypes enumeration for help.
				///
				/// Returns:
				///		DWORD	The current input for this action.
				////////////////////////
				DWORD GetCurrent(DeviceType eDevice)
				{
					return dwCurrent[eDevice];
				}

				////////////////////////
				/// Operator [ ] 
				////////////////////////
				/// A shortcut for GetCurrent().
				///
				/// Input:
				///		DeviceType eDevice
				///		Which device you want. See the DeviceTypes enumeration for help.
				///	
				/// Returns:
				///		DWORD	The result of GetCurrent
				////////////////////////
				DWORD operator [] ( DeviceType eDevice )
				{
					return GetCurrent(eDevice);
				}

				////////////////////////
				/// SetCurrent()
				////////////////////////
				/// Sets the current input for a particular device.
				///
				/// Input:
				///		DeviceType eDevice		
				///		Which device you want. See the DeviceTypes enumeration for help.
				///
				///		DWORD dwInput
				///		Which button / key you want to map to this action.
				///
				/// Returns:
				///		none
				////////////////////////
				void SetCurrent(DeviceType eDevice, DWORD dwInput)
				{
					dwCurrent[eDevice] = dwInput;
				}

				////////////////////////
				/// SetDefault()
				////////////////////////
				/// Sets the Default input for a particular device.
				///
				/// Input:
				///		DeviceType eDevice		
				///		Which device you want. See the DeviceTypes enumeration for help.
				///
				///		DWORD dwInput
				///		Which button / key you want to map to this action.
				///
				/// Returns:
				///		none
				////////////////////////
				void SetDefault(DeviceType eDevice, DWORD dwInput)
				{
					// Set the device.
					dwDefault[eDevice] = dwInput;
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
				DWORD RestoreDefaults(DeviceType eDevice = DEVICE_ALL);

				
				////////////////////////
				/// GetActionName()
				////////////////////////
				/// Gets the name of the action.
				///
				/// Input:
				///		none		
				///
				/// Returns:
				///		char*		The name of the action.
				////////////////////////
				char* GetActionName(void)
				{
					return czActionName;
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
				void SetActionName(char* czString);

				
				////////////////////////
				/// CAction()
				////////////////////////
				/// Sets up all of our default values, which will be currents by default. 
				/// Hey, who'd have thought? :)
				///
				/// Input:
				///		char *czString					The desired name. It'll be truncated to 20 characters!
				///		int iIdx								The index for this action
				///		DWORD dwKeyboard		Default keyboard setting for this action
				///		DWORD dwMouse			Default mouse setting for this action
				///		DWORD dwJoystick			Default joystick setting for this action
				///
				/// Returns:
				///		none
				////////////////////////
				CAction(char* czString, int iIdx, DWORD dwKeyboard = NULL, DWORD dwMouse = NULL, DWORD dwJoystick = NULL)
				{
					SetDefaults(czString, dwKeyboard, dwMouse, dwJoystick);
					pNext = NULL;
					iIndex = iIdx;
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
				void SetDefaults(char* czString, DWORD dwKeyboard, DWORD dwMouse = NULL, DWORD dwJoystick = NULL);


				
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
				void GetActionText(DeviceType eDevice, char* czBuffer);
				
				// Manage memory:
				#if _DEBUG
					ManageMemory(ActionMap::CAction, CORETYPE_INPUT_ACTION);
				#endif

			};

			// All of our actions
			CAction *pActions;

			// Try deleting recursively
			void DeleteAction(ActionMap::CAction *pAct)
			{
				if (pAct)
					DeleteAction(pAct->pNext);	
				delete pAct;		
			}

		// Get an action's name
		void ActionMap::ActionName(int iIndex, char *strStoreHere);

		// Get an action's pointer
		ActionMap::CAction *ActionMap::Action(int iIndex);


};
#endif
