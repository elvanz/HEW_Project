// ------------------------------------------------------------------
// HAL Osaka Event Week Game Contest Project
// @author	Elvan Mulyono(エルファンムルヨノ）
// AT-11A-026_10 / CP-11S-02A_10
//
// @note
//	If the window size does not match the bmp attributes
//	・manually resize the window in command prompt properties
//	・uncheck wrap text output option button 
// -- TODO LIST --
// @Game.cpp look for notes
// @Render.cpp fix 
// -------------------------------------------------------------------
#define _CRT_SECURE_NO_WARNINGS
#include "Game.h"


//================================================================
//						MAIN FUNCTION
//================================================================
int main(void)
{
	//int stage = stages::startmenu;
	//================================================================
	// Game Phase
	//================================================================
	//Initialize Game & Generate Window
	InitWindow();

	//Game Event Handlers 
	InitializeSound();
	MainGame();

	//free memory
	CloseSound();
	//FreeMemory();

	return APP_EXIT;
}	//main


////
// end of file.
////