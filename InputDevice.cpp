/*////////////////////////////////////////////////////////////////////////
InputDevice.cpp	///		February 5, 2004		//		1.00				///
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


#include "InputDevice.h"
#include "dinput.h"

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
	void CInputDevice::SetupBuffer(void)
	{
			// Set up the DirectInput structure.
			DIPROPDWORD dwProp;
			memset(&dwProp, 0, sizeof(DIPROPDWORD));
			dwProp.diph.dwSize = sizeof(dwProp);
			dwProp.diph.dwHeaderSize = sizeof(DIPROPHEADER);
			dwProp.diph.dwHow = DIPH_DEVICE;
			dwProp.diph.dwObj = 0;
			dwProp.dwData = INPUT_BUFFER_SIZE;
		
			// Initialize the buffer to nice, sparkling clean memory
			memset(InputBuffer, 0, (sizeof(DIDEVICEOBJECTDATA) * INPUT_BUFFER_SIZE));

			// Bind the buffer to the device.
			pDevice->SetProperty(DIPROP_BUFFERSIZE, &dwProp.diph);
	}


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
	bool CInputDevice::Shutdown(void)
	{
		// Let go of control of the device
		if (pDevice)
		{
			HRESULT hResult = pDevice->Unacquire();
			if (FAILED(hResult)) return false;
		}
		else
			return false;

		// Clean up the device structures we created
		pDevice->Release();

		// Just for safety's sake...
		pDevice = NULL;

		return true;
	}
