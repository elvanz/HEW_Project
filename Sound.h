#ifndef SOUND_H
#define SOUND_H

#include "Game.h"

enum BGMNUM
{
	MAIN,
	TUTORIAL,
	TITLE,
};

enum SFXNUM
{
	VICTORY,
	GAMEOVER,
	SHOOT,
	PLAYERHIT,
	SELECT,
	ENTER,
	MONSTERHIT
};

//BGM & SFX PATH
extern const char* BGM[4];
extern const char* SFX[8];

//BGM & SFX ID
extern int* bgm_id[sizeof(BGM) / sizeof(char*)];
extern int* sfx_id[sizeof(SFX) / sizeof(char*)];

//BGM & SFX COUNT
extern int bgmCount;
extern int sfxCount;

extern void InitializeSound();
extern void CloseSound();

#endif SOUND_H
