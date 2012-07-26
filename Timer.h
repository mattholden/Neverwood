// Simple timer class for keeping the game on sync

#ifndef StormForge_Matt_Timer
#define StormForge_Matt_Timer

#include "MemoryMgr.h"
#include "Windows.h"		// for LARGE_INTEGER
#include "MathUtils.h"

class Timer
	{
	private:
	public:

			static LARGE_INTEGER liCount;

			// How long the last frame took
			static float fFrameTime;
			static float fTimeOfLastFrame;

			// For slow motion
			static float fTimeScale;

			// FPS counter..
			static float fFPSElapsed;
			static short isFPSCount;
			static short isFrames;

			// Save a divide every time we check the timer by storing this as 1/frequency in a float. Saves half the memory, too!
			static float fFrequency;

	public:

			// Memory managed. Wouldn't want to leak all two member variables of this class...
			#if _DEBUG
				ManageMemory(Timer, CORETYPE_TIMER);
			#endif
		

		// Initialize the pointer. Everything else will be done via constructor
		static void Initialize(void);

		// Get elapsed time in seconds
		static float AppTime(void)
			{
				LARGE_INTEGER liCurrent;
				QueryPerformanceCounter(&liCurrent);

				return float(liCurrent.QuadPart - liCount.QuadPart) * fFrequency;
			}	

		// Resets timer.. Whoda thunk?
		static void Reset(void) { QueryPerformanceCounter(&liCount); }					

		// Update the timer for the elapsed time since last frame. Do this ONLY ONCE per frame.
		// Returns time elapsed since last frame
		static float Timer::Update(void);

		// Get the frames-per-second count
		static short GetFPSCount(void) { return isFPSCount; }
		
		// Get the time elapsed since the last frame.
		static float GetElapsedTime(void) 
		{ 
			return MIN (fTimeScale * fFrameTime, 0.05f); 
		}

		// Scale the time (for slow motion)
		static void SetMotionSpeed(float fSpeed)
		{
			if (fSpeed < 0) fTimeScale = 0.0f;
			else if (fSpeed > 3.0f) fTimeScale = 3.0f;
			fTimeScale = fSpeed;
		}

		static float GetMotionSpeed(void) { return fTimeScale; }

		// Get difference in time.
		// Positive means the time hasn't happened yet and returns how long until it does
		// Negative means it happened this long ago.
		// 0 means it's that time right now
		static float CompareTime(float fCompareTime)
		{
			float fApp = AppTime();
			return fCompareTime - fApp;
		}
	
	};


#endif
