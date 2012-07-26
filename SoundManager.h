
////////////////////////////////////////////////////////////////////////////////
// File: SoundManager.h
//
// Author: Rolf Hendriks
//
// Created: 12-06-03
//
// Purpose: defines a singleton class that implements sound management using fmod
//
// Usage:	for the most part, you can just use loadSound to load a sound from a file,
//			and use its return value to play that sound. There is one exception
//			If you are loading a large WAV file, you should use loadStreamedSound instead of loadSound.		
////////////////////////////////////////////////////////////////////////////////

#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

// CHANGED: 4/10/2004 Matt
// * Changed soundData struct to class
// * overloaded new and delete with memory manager for soundData
// * capitalized 1st letter of function names per codespec
// * Replaced STL vector with static array of size SOUND_INITIAL_SOUNDPOOL

// CHANGED: 5/1/2004 Matt
// * Converted to static singleton

#define SOUND_INITIAL_SOUNDPOOL 40
#include "MemoryMgr.h"
#include "fmod.h"

class SoundManager
{

private:
	// CHANGED: 5/1/2004 Matt. Removed all this stuff for static singleton conversion. Constructor is now
	// SoundManager::Initialize().
	/*
	/////////////////////////////////////////////////////////////////////////////////////
	// Function: SoundManager
	//
	// Last modified: 1-11-04
	//
	// Input: None
	//
	// Output: None
	//
	// Returns: A SoundManager object with uninitialized variables
	//
	// Purpose: constructor
	//////////////////////////////////////////////////////////////////////////////////////
	SoundManager();

	/////////////////////////////////////////////////////////////////////////////////////
	// Function: ~SoundManager
	//
	// Last modified: 1-13-04
	//
	// Input: None
	//
	// Output: None
	//
	// Returns: None
	//
	// Purpose: destructor
	//////////////////////////////////////////////////////////////////////////////////////
	~SoundManager();

	//prevent copying and assigning
	SoundManager (const SoundManager&){}
	SoundManager& operator= (const SoundManager&){}
	
	//make the class a singleton
	static SoundManager* pSoundManager;

	*/

	//keep a database of existing sounds
	
	class soundData
	{
	public:
		char	fileName[128];			//the name of the file storing the sound
		union 				//FMOD's internal representation of the file. It can be one of the following:
		{
			FSOUND_SAMPLE* sample;		//used for regular playback of sounds (WAV, MP3, OGG, RAW, ...).
			FSOUND_STREAM* stream;		//used for streamed playback of sounds. Streamed playback should be used for large WAV files.
			FMUSIC_MODULE* music;		//used to play sequenced music files (MIDI, MOD, ...)
		};
		int type;		//what type of playback the sound is using. Its value is taken from the enum below

		// Manage new and delete
		#if _DEBUG
			ManageMemory(soundData, CORETYPE_SOUNDFILE);
		#endif
	};
	
	enum eSoundTypes {SOUND_SAMPLE, SOUND_STREAM, SOUND_MUSIC };

	//this is where all the internal data is stored
	static soundData *pSounds[SOUND_INITIAL_SOUNDPOOL];
	static int iSoundCount;

public:

	// CHANGED: Matt 5/2/04. This isn't being referenced anywhere else in the manager, don't know why it wasn't
	// giving a warning.
	//static DWORD dwLastPlayed;		//the last time a sound was played. You need to keep track of it manually

	//////////////////////////////////////////////////////////////////////////////////////////
	// Function: GetInstance
	//
	// Last modified: 1-11-04
	//
	// Input: None
	//
	// Output: none
	//
	// Returns: the (one and only) static SoundManager pointer
	//
	// Purpose: to gain access to the SoundManager's functionality
	/////////////////////////////////////////////////////////////////////////////////////////////
	//static SoundManager* GetInstance();

	// For memory manager
	#if _DEBUG
		ManageMemory(SoundManager, CORETYPE_SOUNDMANAGER);
	#endif


	// Replace constructor for static-ness
	static void Initialize(void);

	static int GetTime(int iSound)
	{
		if (iSound < 0 || iSound > iSoundCount) return -1;

		if (pSounds[iSound]->type == SOUND_STREAM)
			return FSOUND_Stream_GetTime (pSounds[iSound]->stream);

		return -1;
	}


	//////////////////////////////////////////////////////////////////////////////////////////
	// Function: LoadSound
	//
	// Last modified: 1-11-04
	//
	// Input:	filename-> the file name of the sound you're loading
	//			looped(optional)-> set this to true if you want the sound to loop back to the beginning after it's done
	//			volume(optional)-> the volume level at which the sound should play. Ranges from 1 to 255, defaults to 150
	//
	// Output: none
	//
	// Returns: an integer representing the ID of the file
	//
	// Purpose: To load a sound into RAM. Makes sure not to load the same sound twice.
	//			if you enter an mp3 file, it will automatically load it using streamed playback
	//			if you enter a midi or mod file, it will automatically load it using music playback;
	//////////////////////////////////////////////////////////////////////////////////////////
	static int LoadSound (char* filename, int volume = 150, bool looped = false);

	//////////////////////////////////////////////////////////////////////////////////////////
	// Function: LoadStreamedSound
	//
	// Last modified: 1-26-04
	//
	// Input:	filename-> the file name of the sound you're loading
	//			volume(optional)-> the volume level at which the sound should play. Ranges from 1 to 255, defaults to 150
	//			looped(optional)-> whether or not the sound should start over from the beginning after it is done
	//
	// Output: none
	//
	// Returns: an integer representing the ID of the file
	//
	// Purpose: To load a sound using streamed playback to conserve memory usage.
	//			Use this when loading large WAV files
	//////////////////////////////////////////////////////////////////////////////////////////
	static int LoadStreamedSound (char* filename, int volume = 150, bool looped = false);

	/////////////////////////////////////////////////////////////////////////////////////////
	// Function: Play
	//
	// Last modified: 1-11-04
	//
	// Input: the ID representing the sound to play. You can get the ID by using LoadSound
	//
	// Output: none
	//
	// Returns: none
	//
	// Purpose: to play a sound. Hence the name.
	/////////////////////////////////////////////////////////////////////////////////////////
	static void Play (int soundID);

	/////////////////////////////////////////////////
	// Function: stopMusic
	// Last modified: 1-11-04
	// Input: none
	// Output: none
	// Returns: none
	// Purpose: stops all the music currently playing
	/////////////////////////////////////////////////
	static void StopMusic ();

	/////////////////////////////////////////////////
	// Function: skipTo
	// Last modified: 1-28-04
	// Input:	ID-> the ID representing the sound to play. You can get the ID by using LoadSound
	//			time-> the amount of milliseconds into the song to which to set the track
	// Output: none
	// Returns: true if successful, false otherwise
	// Purpose: to play a sound. Hence the name.
	/////////////////////////////////////////////////
	static bool SkipTo (int soundID, int time);

	//////////////////////////////////////////////////////////////////////////////////////////
	// Function: GetVolume
	//
	// Last modified: 1-11-04
	//
	// Input:	soundID-> the ID representing the sound whose default volume to obtain
	//
	// Output: none
	//
	// Returns: the volume at which the sound will play. Ranges from 1-255
	//
	// Purpose: get the volume at which a sound will play
	////////////////////////////////////////////////////////////////////////////////////////////
	static int GetVolume (int soundID);

	//////////////////////////////////////////////////////////////////////////////////////////
	// Function: SetVolume
	//
	// Last modified: 1-11-04
	//
	// Input:	soundID-> the ID representing the sound whose default volume to change
	//			volume-> the new volume of the sound
	// Output: none
	//
	// Returns: none
	//
	// Purpose: permanently change the volume at which the sound will play
	////////////////////////////////////////////////////////////////////////////////////////////
	static void SetVolume (int soundID, int volume);

	static void SetAllVolumes(int volume)
	{
		for (int i = 0; i < iSoundCount; i++)
		   SetVolume(i, volume);
	}

	////////////////////////////////////////////////////////////////////////////////////////
	// Function: shutDown
	//
	// Last modified: 1-13-04
	//
	// Input: none
	//
	// Output: none
	//
	// Returns: none
	//
	// Purpose: deallocate all the memory the sound manager is using up
	////////////////////////////////////////////////////////////////////////////////////////
	static void Shutdown();
};

#endif