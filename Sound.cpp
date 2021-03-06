#include "Sound.h"

//BGM & SFX PATH
const char* BGM[4] =
{
	"bgm\\BGM.mp3", //MAIN
	"bgm\\Tutorial.mp3", //TUTORIAL
	"bgm\\BGM02.mp3", //TITLE
	NULL
};

const char* SFX[8] =
{
	"bgm\\VICTORY.mp3", //VICTORY
	"bgm\\GAMEOVER.wav", //GAMEOVER
	"bgm\\Shoot.wav", //SHOOT
	"bgm\\Squish.wav", //PLAYERHIT
	"bgm\\Select.wav", //SELECT
	"bgm\\Enter.wav", //ENTER
	"bgm\\monsterhit.wav", //MONSTERHIT
	NULL
};

int* bgm_id[sizeof(BGM) / sizeof(char*)]{ 0 };
int* sfx_id[sizeof(SFX) / sizeof(char*)]{ 0 };
int bgmCount{ 0 };
int sfxCount{ 0 };

/* Sound settings */
void InitializeSound()
{
	//Open BGM File
	bgmCount = 0;
	while (BGM[bgmCount] != NULL) {
		// ファイルを開く
		bgm_id[bgmCount] = MciOpenSound(BGM[bgmCount]);
		bgmCount++;
	}


	///Open SFX File
	sfxCount = 0;
	while (SFX[sfxCount] != NULL) {
		// ファイルを開く
		sfx_id[sfxCount] = MciOpenSound(SFX[sfxCount]);
		sfxCount++;
	}

	return;
}

/* Close sound */
void CloseSound()
{
	//Close sound
	bgmCount = 0;
	while (BGM[bgmCount] != NULL) {
		MciCloseSound(bgm_id[bgmCount]);
		bgmCount++;
	}

	sfxCount = 0;
	while (SFX[sfxCount] != NULL) {
		MciCloseSound(sfx_id[sfxCount]);
		sfxCount++;
	}

	return;
}