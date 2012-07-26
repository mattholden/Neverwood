
#include "ActionMap.h"
#include "DirectInput2.h"


// Create a new action
int ActionMap::AddAction(char *strActionName,  DWORD dwKeyboard, DWORD dwMouse, DWORD dwJoystick)
		{
			ActionMap::CAction *pNewAction = new ActionMap::CAction(strActionName, iActionCount++, dwKeyboard, dwMouse, dwJoystick);
			
			// Add to list
				// Special case: 1st action
				if (!pActions) 
					pActions = pNewAction;

				// Otherwise find last and add to it
				// Check to make sure this action isn't already used
				else
				{
					ActionMap::CAction *pAct = pActions;
					while (pAct)
					{
						// If this is the last one, add here
						if (!pAct->pNext)
						{
							pAct->pNext = pNewAction;
							break;
						}

						else
						{
							// Make sure we don't already have this key mapped
							//if (pAct->GetCurrent(DEVICE_KEYBOARD)	== dwKeyboard || pAct->GetCurrent(DEVICE_MOUSE) == dwMouse || pAct->GetCurrent(DEVICE_JOYSTICK) == dwJoystick)																						
							//{
							//	delete pNewAction;
							//	return -1;
							//}

							// Go to next one in the list
							pAct = pAct->pNext;
						}
					}

				}

			// return index
				return iActionCount - 1;
		}


	// Check for an action
		DWORD ActionMap::CheckAction(int iIndex, DeviceType eDevice, bool bBuffered)
		{
			
			// Find the action
			CAction *pAct = pActions;
			while (pAct)
			{
				if (pAct->GetIndex() == iIndex)
					break;
				pAct = pAct->pNext;
			}
			
			// This action doesn't exist
			if (!pAct) return 0;

			DWORD dwReturn;

			// Check keyboard
			if (eDevice == DEVICE_ALL || eDevice == DEVICE_KEYBOARD)
			{
				if (bBuffered) 
					dwReturn = DirectInput::GetKeyboard()->CheckBufferedInput(pAct->GetCurrent(DEVICE_KEYBOARD));
				else
					dwReturn = DirectInput::GetKeyboard()->CheckImmediateInput(pAct->GetCurrent(DEVICE_KEYBOARD));
				
				if (dwReturn) return dwReturn;
			}	
			
			// Check mouse
			if (eDevice == DEVICE_ALL || eDevice == DEVICE_MOUSE)
			{
				if (bBuffered) 
					dwReturn = DirectInput::GetMouse()->CheckBufferedInput(pAct->GetCurrent(DEVICE_MOUSE));
				else
					dwReturn = DirectInput::GetMouse()->CheckImmediateInput(pAct->GetCurrent(DEVICE_MOUSE));

				if (dwReturn) return dwReturn;
			}

			// Joystick
			if ((eDevice == DEVICE_ALL || eDevice == DEVICE_JOYSTICK) && iJoystickIndex != -1)
			{
				//CHANGED
				//checking if a joystick is available before polling it for input
				//Rolf, 5-23-04

				if (DirectInput::GetJoystick(iJoystickIndex))
				{
					if (bBuffered) 
						dwReturn = DirectInput::GetJoystick(iJoystickIndex)->CheckBufferedInput(pAct->GetCurrent(DEVICE_JOYSTICK));
					else
						dwReturn = DirectInput::GetJoystick(iJoystickIndex)->CheckImmediateInput(pAct->GetCurrent(DEVICE_JOYSTICK));

					return dwReturn;
				}
			}

			// Something went wrong if you got here
			return 0;
		
		}

		// This is specifically included so we can do a switch() to see what action is occurring.
		DWORD ActionMap::CurrentAction(DeviceType eDevice, bool bBuffered)
		{
			for (int i = 0; i < iActionCount; i++)
			{
				if (CheckAction(i, eDevice, bBuffered))
					return i;
			}

			return -1;
		}

		// Get an action's name
		void ActionMap::ActionName(int iIndex, char *strStoreHere)
		{
			// Find the action
			CAction *pAct = pActions;
			while (pAct)
			{
				if (pAct->GetIndex() == iIndex)
					break;
				pAct = pAct->pNext;
			}
			
			// This action exists
			if (pAct) 
				strcpy(strStoreHere,  pAct->GetActionName());
		}


		// Get an action's pointer
		ActionMap::CAction *ActionMap::Action(int iIndex)
		{
			// Find the action
			ActionMap::CAction *pAct = pActions;
			while (pAct)
			{
				if (pAct->GetIndex() == iIndex)
					return pAct;
				pAct = pAct->pNext;
			}
			return NULL;
		}

