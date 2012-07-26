
#include "Timer.h"

// Initialize static stuff
float Timer::fFrameTime = 0.0f;
float Timer::fTimeOfLastFrame = 0.0f;
float Timer::fFPSElapsed = 0.0f;
short Timer::isFPSCount = 0;
short Timer::isFrames = 0;
float Timer::fFrequency = 0.0f;
float Timer::fTimeScale = 1.0f;
LARGE_INTEGER Timer::liCount;

		// Update the timer for the elapsed time since last frame. Do this ONLY ONCE per frame.
		// Returns time elapsed since last frame
		float Timer::Update(void)
		{				
			
			// Calculate time elapsed this frame
			float fApp = AppTime();
			fFrameTime = fApp - fTimeOfLastFrame;
			fTimeOfLastFrame = fApp;

			// Add this to the elapsed time since last FPS count
			fFPSElapsed += fFrameTime;

			// Increment frames counter
			isFrames++;

			// See if it's time to count the frames.
			if (fFPSElapsed >= 1.0f)
			{
				// We're doing it, no more elapsed time.
				fFPSElapsed = 0.0f;

				// Store the frame count and reset the counter.
				isFPSCount = isFrames;
				isFrames = 0;

			}

			// return the elapsed time
			return fFrameTime;
		}


		// Create a new timer.
		void Timer::Initialize(void)
				{
					QueryPerformanceCounter(&liCount);
			
					LARGE_INTEGER liFrequency;
					QueryPerformanceFrequency(&liFrequency);
					fFrequency = 1.0f / float(liFrequency.QuadPart);
			
					isFrames = 0;
					isFPSCount = 0;
					fFPSElapsed = 0.0f;
					fTimeOfLastFrame = 0.0f;
					fFrameTime = AppTime();
				}