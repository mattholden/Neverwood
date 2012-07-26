

#include "Menu.h"

bool Menu::bInMenu = true;
int Menu::iWhere = MENU_MAIN;
int Menu::iBark = INVALID;
int Menu::iGoNTitle = INVALID;
int Menu::iTechLogos = INVALID;
int Menu::iMenuOpt = INVALID;
int Menu::iMenuPlay = INVALID;
int Menu::iMenuCreds = INVALID;
int Menu::iMenuExtra = INVALID;
int Menu::iMenuResume = INVALID;
int Menu::iMenuQuit = INVALID;
int Menu::iMenuSelect = INVALID;
int Menu::iHelpTexture = INVALID;
int Menu::iSlides[5] = { INVALID, INVALID, INVALID, INVALID, INVALID };
int Menu::iSelection = 0;
bool Menu::bFirstGame = true;
float Menu::fMovieTime = 0.0f;
float Menu::fMovieStart = 0.0f;
float Menu::fJoyTime = 0.0f;
int Menu::iDrums = INVALID;
bool Menu::bMusic = false;
bool Menu::bScreenshot = false;
int Menu::iMenuOptBack = INVALID;
int Menu::iMenuOptControl = INVALID;
int Menu::iMenuOptFeedback = INVALID;
int Menu::iMenuOptRes = INVALID;
int Menu::iMenuOptSound = INVALID;
int Menu::iResolution = RESOLUTION_800X600;
int Menu::iSelectOpt = 0;
int Menu::iSoundVol = 150;
int Menu::iDeath1 = INVALID;
int Menu::iDeath2 = INVALID;
int Menu::iWin1 = INVALID;
int Menu::iWin2 = INVALID;
int Menu::iSlider = INVALID;
int Menu::iSliderTrack = INVALID;
int Menu::iMenuOff = INVALID;
int Menu::iMenuOn = INVALID;
bool Menu::bFeedbackOK = false;
int Menu::iSelectNew = 0;
int Menu::iSelectDiff = DIFFICULTY_NORMAL;
int Menu::iMenuNewDifficulty = INVALID;
int Menu::iMenuNewStart = INVALID;
int Menu::iMenuNewSpeed = INVALID;
float Menu::fSelectSpeed = 1.0f;
float Menu::fMovieEnd=0;
bool Menu::bIsPlaying=false;
int Menu::iL2S1=Menu::iDeath1;
int Menu::iL2S2=Menu::iDeath2;
int Menu::iL3S1=Menu::iWin1;
int Menu::iL3S2=Menu::iWin2;

