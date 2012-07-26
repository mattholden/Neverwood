//////////////////////////////////////////////////////////////////
// File: SoundManager.cpp
//
// Author: Rolf Hendriks
//
// Created: 12-06-03
//
// Purpose: defines a singleton class that implements sound management using fmod
/////////////////////////////////////////////////////////////////


#include <windows.h>
#include "MemoryMgr.h"
#include <fstream>

using namespace std;
#include "soundmanager.h"

// CHANGED: Matt 4/10/2004
// * Capitalized first letter of function names for codespec
// * Removed calls to Tetherbomb logger
// * Replaced Tetherbomb memory monitor calls with StormForge new/delete overloads
// * Changed LoadStreamedSound() parameter 1 from string to char*
// * Added error check to Play() index parameter to prevent blowing up when a sound is missing
// * Replaced STL vector with static array of size SOUND_INITIAL_SOUNDPOOL

// CHANGED: 5/1/2004 added to replace constructor and deal with static member variables
SoundManager::soundData *SoundManager::pSounds[SOUND_INITIAL_SOUNDPOOL];
int SoundManager::iSoundCount = 0;

void SoundManager::Initialize(void) 
{ 
	FSOUND_Init(44100, 32, 0);
	iSoundCount = 0;
}

// CHANGED: 5/1/2004 Matt. Removed for static singleton conversion
//SoundManager* SoundManager::pSoundManager = NULL;
//SoundManager::~SoundManager()
//{
//}
//
//SoundManager* SoundManager::GetInstance()
//{
//
//	if (!pSoundManager)
//	{
//		pSoundManager = new SoundManager;
//	}	
//	
//	return pSoundManager;
//}

int SoundManager::LoadSound (char* filename, int volume, bool looped)
{
	
	//return -1 if file is invalid
	ifstream infile (filename);
	if (!infile.is_open())
	{
		return -1;
	}
	infile.close();

	//check if the file is already loaded
	int index;	//the value to return
	for (index=0; index< iSoundCount; index++)
	{
		if (!strcmp (pSounds[index]->fileName, filename))
			return index;
	}

	//load the file at the given volume
	if (volume > 255)
		volume = 255;
	if (volume < 1)
		volume = 1;

	// create a new sound
	soundData* newSound = new soundData;
	    
	strcpy (newSound->fileName, filename);
	char* fileExtension = filename + (strlen(filename)-3);

	// if the sound is a wav or raw file, load all of it into a sound sample
	if (!stricmp (fileExtension, "wav") || !stricmp (fileExtension, "raw"))
	{
		newSound->type = SOUND_SAMPLE;
		int flags;
		if (looped)
			flags = FSOUND_LOOP_NORMAL | FSOUND_NORMAL;
		else flags = FSOUND_LOOP_OFF | FSOUND_NORMAL;
		FSOUND_SAMPLE* soundSample = FSOUND_Sample_Load (FSOUND_FREE, filename, flags, 0); 	
		FSOUND_Sample_SetDefaults (soundSample, -1, volume, -1, -1);	
		newSound-> sample = soundSample; 
	
	}
	// if the sound is an mp3 or mp2 or ogg file, load it using streamed playback
	else if (!stricmp (fileExtension, "mp3") || !stricmp (fileExtension, "mp2") || !stricmp (fileExtension, "ogg"))
	{
		newSound->type = SOUND_STREAM;
		int flags;
		if (looped)
			flags = FSOUND_LOOP_NORMAL | FSOUND_NORMAL;
		else flags = FSOUND_LOOP_OFF | FSOUND_NORMAL;
		FSOUND_STREAM* soundStream = FSOUND_Stream_OpenFile (filename, flags, 0);
		FSOUND_Sample_SetDefaults (FSOUND_Stream_GetSample(soundStream), -1, volume, -1, -1);
		newSound-> stream = soundStream;
	
	}
	//if the sound is a midi or mod file, load it using music playback
	else if (!stricmp (fileExtension, "mid") || !stricmp (fileExtension, "mod"))
	{
		newSound->type = SOUND_MUSIC;
		FMUSIC_MODULE* musicModule = FMUSIC_LoadSong (filename);
		if (!looped)
			FMUSIC_SetLooping (musicModule, FALSE);
		FMUSIC_SetMasterVolume (musicModule, volume);
		newSound-> music = musicModule;
	
	}
	//otherwise the type is not supported
	else
	{
		delete newSound;

		return -1;
	}

	pSounds[iSoundCount] = newSound;
	iSoundCount++;

	return index;
}

int SoundManager::LoadStreamedSound (char* filename, int volume, bool looped)
{

	//return -1 if file is invalid
	ifstream infile (filename);
	if (!infile.is_open())
	{
		return -1;
	}
	infile.close();

	//check if the file is already loaded
	int index;	//the value to return
	for (index=0; index< iSoundCount; index++)
	{
		if (!stricmp(pSounds[index]->fileName, filename))
			return index;
	}

	//load the sound
	soundData* newSound = new soundData;
	

	strcpy (newSound->fileName, filename);
	newSound->type = SOUND_STREAM;
	int flags;
	if (looped)
		flags = FSOUND_LOOP_NORMAL | FSOUND_NORMAL;
	else flags = FSOUND_LOOP_OFF | FSOUND_NORMAL;
	FSOUND_STREAM* soundStream = FSOUND_Stream_OpenFile (filename, flags, 0);
	FSOUND_Sample_SetDefaults (FSOUND_Stream_GetSample(soundStream), -1, volume, -1, -1);
	newSound-> stream = soundStream;
	//vSounds.push_back (newSound);
	pSounds[iSoundCount] = newSound;
	iSoundCount++;

	return index;
}

void SoundManager::Play (int soundID)
{
	if (soundID == -1) return;
	switch (pSounds[soundID]->type)
	{
	case SOUND_SAMPLE:
		FSOUND_PlaySound (FSOUND_FREE, pSounds[soundID]-> sample);
		break;
	case SOUND_STREAM:
		FSOUND_Stream_Play (FSOUND_FREE, (pSounds[soundID]-> stream));
		break;
	case SOUND_MUSIC:
		FMUSIC_PlaySong (pSounds[soundID]-> music);
		break;
	}
}

void SoundManager::StopMusic()
{
	// stop all sequenced music files
	FMUSIC_StopAllSongs();

	// stop streamed music files
	for (int i=0; i < iSoundCount; i++)
	{
		if (pSounds[i]->type == SOUND_STREAM)
		{
			FSOUND_Stream_Stop (pSounds[i]->stream);
		}
	}

}

bool SoundManager::SkipTo (int SoundID, int time)
{
	if (pSounds[SoundID]->type == SOUND_STREAM)
	{
		FSOUND_Stream_SetTime (pSounds[SoundID]->stream, time);
		return true;
	}
	return false;
}

int SoundManager::GetVolume (int SoundID)
{
	int iVolume = 0;	//the return value
	
	switch (pSounds[SoundID]->type)
	{
	case SOUND_SAMPLE:
		FSOUND_Sample_GetDefaults (pSounds[SoundID]-> sample, NULL, &iVolume, NULL, NULL);
		break;
	case SOUND_STREAM:
		FSOUND_Sample_GetDefaults (FSOUND_Stream_GetSample (pSounds[SoundID]-> stream), NULL, &iVolume, NULL, NULL);
		break;
	case SOUND_MUSIC:
		iVolume = FMUSIC_GetMasterVolume (pSounds[SoundID]-> music);
		break;
	}
	return iVolume;
}

void SoundManager::SetVolume (int SoundID, int volume)
{
	if (volume > 255)
		volume = 255;
	if (volume < 1)
		volume = 1;

	switch (pSounds[SoundID]->type)
	{
	case SOUND_SAMPLE:
		FSOUND_Sample_SetDefaults (pSounds[SoundID]-> sample, -1, volume, -1, -1);
		break;
	case SOUND_STREAM:
		FSOUND_Sample_SetDefaults (FSOUND_Stream_GetSample (pSounds[SoundID]-> stream), -1, volume, -1, -1);
		break;
	case SOUND_MUSIC:
		FMUSIC_SetMasterVolume (pSounds[SoundID]-> music, volume);
		break;
	}
}


void SoundManager::Shutdown()
{
	

	// free samples and streams
	FSOUND_Close();
	//free all the music samples
	FMUSIC_StopAllSongs();
	for (int i=0; i < iSoundCount; i++)
	{
		if (pSounds[i]->type == SOUND_MUSIC)
		{
			FMUSIC_FreeSong (pSounds[i]-> music);
		}
		delete pSounds[i];
	}
	memset(pSounds, 0, sizeof(soundData*) * SOUND_INITIAL_SOUNDPOOL);

	// CHANGED Matt 5/1/04 static singleton conversion
	//delete pSoundManager;
	
}

