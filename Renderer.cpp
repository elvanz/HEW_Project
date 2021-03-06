/**
* @file Renderer.h
* 
* @note
* -Working on a fix for it
* -- ABANDONED --
* 1. Put render function and allocate memory to it to pass to Game.cpp file
* 2. When allocating memory to Game.cpp, the frameBuffer does not render
* 
*/
#include "Renderer.h"

////Render passed function as game data for rendering
//void Render(void (*func)())
//{
//	//Clearing frameBuffer space for rendering
//	ClearFrameBuffer(frameBuffer[0]);
//
//	//Assign color palette from RGBQUAD to ConsolePalette
//	GetConsolePalette();
//	CopyPalette(ColorPalette);
//	SetConsolePalette();
//
//	//Render buffer for each stages
//	func();
//
//	//Forwarding image to the frameBuffer
//	PrintFrameBuffer(frameBuffer[0]);
//
//	//Flip screen to the second buffer
//	FlipScreen();
//}
//
////Player sprite rendering with GUI HUD
//void player_sprite(character* player, object* camera)
//{
//	//Player sprite render
//	for (int y = 0; y < 26; y++)
//	{
//		for (int x = 0; x < 16; x++)
//		{
//			//Character sprite facing front
//			if (player->direction == FRONT)
//			{
//				if (Player_MOVINGFRONT[player->anim][y][x] != 0x00)
//				{
//					if (player->attacking)
//					{
//						if (Player_ATTACKINGFRONT[y][x] != 0x00)
//							frameBuffer[y + player->position.y][x + player->position.x] = Player_ATTACKINGFRONT[y][x];
//					}
//					else
//						frameBuffer[y + player->position.y][x + player->position.x] = Player_MOVINGFRONT[player->anim][y][x];
//				}
//			}
//			//Character sprite facing left
//			else if (player->direction == LEFT)
//			{
//				if (Player_MOVINGSIDE[player->anim][y][x] != 0x00)
//				{
//					if (player->attacking)
//					{
//						if (Player_ATTACKINGSIDE[y][x] != 0x00)
//							frameBuffer[y + player->position.y][16 - x + player->position.x] = Player_ATTACKINGSIDE[y][x];
//					}
//					else
//						frameBuffer[y + player->position.y][x + player->position.x] = Player_MOVINGSIDE[player->anim][y][x];
//				}
//			}
//			//Character sprite facing right
//			else if (player->direction == RIGHT)
//			{
//				if (Player_MOVINGSIDE[player->anim][y][x] != 0x00)
//				{
//					if (player->attacking)
//					{
//						if (Player_ATTACKINGSIDE[y][x] != 0x00)
//							frameBuffer[y + player->position.y][x + player->position.x] = Player_ATTACKINGSIDE[y][x];
//					}
//					else
//						frameBuffer[y + player->position.y][16 - x + player->position.x] = Player_MOVINGSIDE[player->anim][y][x];
//				}
//			}
//			//Character sprite facing backward
//			else if (player->direction == BACK)
//			{
//				if (Player_MOVINGBACK[player->anim][y][x] != 0x00)
//				{
//					frameBuffer[y + player->position.y][x + player->position.x] = Player_MOVINGBACK[player->anim][y][x];
//				}
//			}
//		}
//	}
//
//
//	// -- GUI --
//	//--------------------------------------------------------------------------
//	//Health UI
//	for (int y = 0; y < 11; y++)
//	{
//		for (int x = 0; x < 12; x++)
//		{
//			if (Health_UI[icon_anim][y][x] != 0x00)
//				frameBuffer[y + 210][x + 10] = Health_UI[icon_anim][y][x];
//		}
//	}
//	//CAN YOU SURVIVE Text
//	char string[64]{ 0 };
//	char string2[64]{ 0 };
//	sprintf(&string[0], "WAVE %d", wave);
//	if (wave < 7)
//		strcpy(&string2[0], "EASY");
//	else strcpy(&string2[0], "HARD");
//
//	//ASCII Render
//	for (int y = 0; y < 8; y++)
//	{
//		for (int x = 0; x < 8; x++)
//		{
//			//x text
//			if (ascii[72][y][x] != 0x00)
//				frameBuffer[y + 213][x + 25] = ascii[72][y][x];
//			//Player HP
//			for (int z = 0; z < 6; z++)
//			{
//				if (ascii[z][y][z] != 0x00)
//					frameBuffer[y + 213][x + 36] = ascii[player->HP][y][x];
//			}
//
//			//Survive text
//			for (int i = 0; string[i] != NULL; i++)
//			{
//				//Ignoring sentences with space
//				if (string[i] == ' ' || string2[i] == ' ')
//					continue;
//
//				//To fix C6011 Deferencing NULL ptr
//				if (ascii != nullptr && *ascii)
//				{
//					//-- 212 is the y position  & 82 is the x position 
//					if (*(*(*ascii) + ((string[i] - 0x30) * 8 * 8) + (y * 8) + x) != 0x00)
//					{
//						*((*frameBuffer) + (((y + 212) % SCREEN_HEIGHT) * SCREEN_WIDTH) + (x + 82 + i * 7) % SCREEN_WIDTH) =
//							*(*(*ascii) + ((string[i] - 0x30) * 8 * 8) + (y * 8) + x);
//					}
//
//					//-- 212 is the y position  & 152 is the x position 
//					if (*(*(*ascii) + ((string2[i] - 0x30) * 8 * 8) + (y * 8) + x) != 0x00)
//					{
//						*((*frameBuffer) + (((y + 212) % SCREEN_HEIGHT) * SCREEN_WIDTH) + (x + 152 + i * 7) % SCREEN_WIDTH) =
//							*(*(*ascii) + ((string2[i] - 0x30) * 8 * 8) + (y * 8) + x);
//					}
//				}
//				else break;
//			}
//		}
//	}
//}
//
////Map sprite rendering
//void map_sprite()
//{
//	//Load Map[20][20] to Map_Render[20 * 32][20 * 32]
//	//char Map_Render[10 * 32][10 * 32];
//	//[x * 20 + l]
//	//char* Map_Render = new char[10 * 32]{ 0 };
//	auto Map_Render = new char[10 * 32][10 * 32]{ 0 }; //Use this to fix C6262 warning using heap memory
//
//	//Load objects data to map
//	for (int y = 0; y < 10; y++)
//	{
//		for (int x = 0; x < 10; x++)
//		{
//			switch (Map2[y][x])
//			{
//				//TREES
//			case 0:
//			{
//				for (int k = 0; k < 23; k++)
//				{
//					for (int l = 0; l < 13; l++)
//					{
//						if (Trees[k][l] != 0x00)
//						{
//							Map_Render[y * 20 + k][x * 20 + l] = Trees[k][l];
//						}
//					}
//				}
//				break;
//			}
//			//DIRT GROUND
//			case 1:
//			{
//				for (int k = 0; k < 32; k++)
//				{
//					for (int l = 0; l < 32; l++)
//					{
//						if (Dirt[k][l] != 0x00)
//							//Map_Render[y * 20 + k][x * 20 + l] = Dirt[k][l];
//							Map_Render[y * 20 + k][x * 20 + l] = 0x0D;
//					}
//				}
//				break;
//			}
//			//OBSTACLES
//			case 2:
//			{
//				for (int k = 0; k < 14; k++)
//				{
//					for (int l = 0; l < 16; l++)
//					{
//						if (Obstacles[k][l] != 0x00)
//							Map_Render[y * 20 + k][x * 20 + l] = Obstacles[k][l];
//					}
//				}
//				break;
//			}
//			//HOUSE
//			case 3:
//			{
//				for (int k = 0; k < 14; k++)
//				{
//					for (int l = 0; l < 14; l++)
//					{
//						Map_Render[y * 20 + k][x * 20 + l] = House[k][l];
//					}
//				}
//				break;
//			}
//			default:
//				break;
//			}
//		}
//	}
//
//	//forwarding map data to frameBuffer
//	for (int y = 0; y < SCREEN_HEIGHT; y++)
//	{
//		for (int x = 0; x < SCREEN_WIDTH; x++)
//		{
//			frameBuffer[y][x] = Map_Render[y][x];
//			//frameBuffer[y][x] = WHITE;
//		}
//	}
//
//	//Clear heap memory
//	delete[] Map_Render;
//}
//
////Load enemy sprite
//void enemy_sprite(chara* enemy)
//{
//	for (int y = 0; y < 14; y++)
//	{
//		for (int x = 0; x < 11; x++)
//		{
//			if (Enemy_MOVING[enemy->anim][y][x] != 0x00)
//			{
//				if (enemy->direction == RIGHT)
//					frameBuffer[y + enemy->position.y][x + enemy->position.x] = Enemy_MOVING[enemy->anim][y][x];
//				else
//					frameBuffer[y + enemy->position.y][11 - x + enemy->position.x] = Enemy_MOVING[enemy->anim][y][x];
//			}
//		}
//	}
//}
//
////Tutorial render
//void tutorial_render()
//{
//	//creating effect for press enter text
//	posCount++;
//
//	//Clamp animation
//	if (posCount > 200)
//		posCount = 0;
//
//	//Background
//	for (int y = 0; y < SCREEN_HEIGHT; y++)
//	{
//		for (int x = 0; x < SCREEN_WIDTH; x++)
//		{
//			frameBuffer[y][x] = 0x05;
//		}
//	}
//
//	//How to play text
//	for (int y = 0; y < 14; y++)
//	{
//		for (int x = 0; x < 144; x++)
//		{
//			if (HowToPlay[y][x] != 0x05)
//				frameBuffer[y + 40][x + 31] = HowToPlay[y][x];
//		}
//	}
//
//	//Keymapping
//	for (int y = 0; y < 30; y++)
//	{
//		for (int x = 0; x < 72; x++)
//		{
//			frameBuffer[y + 90][x + 63] = KeyMap[y][x];
//		}
//	}
//
//	//Keymapping border
//	for (int y = 0; y < 34; y++)
//	{
//		for (int x = 0; x < 76; x++)
//		{
//			if (KeyMapBorder[y][x] != 0x05)
//				frameBuffer[y + 88][x + 61] = KeyMapBorder[y][x];
//		}
//	}
//
//	//Press enter text
//	for (int y = 0; y < 26; y++)
//	{
//		for (int x = 0; x < 128; x++)
//		{
//			if (PressEnter[y][x] != 0x05)
//				frameBuffer[y + 150][x + posCount] = PressEnter[y][x];
//		}
//	}
//}
//
////Title render
//void title_render()
//{
//	//Background
//	for (int y = 0; y < SCREEN_HEIGHT; y++)
//	{
//		for (int x = 0; x < SCREEN_WIDTH; x++)
//		{
//			frameBuffer[y][x] = WHITE;
//		}
//	}
//
//	//DEBUG Title Sprite
//	for (int y = 0; y < 33; y++)
//	{
//		for (int x = 0; x < 128; x++)
//		{
//			if (Title_UI[y][x] != 0x00)
//				frameBuffer[y + TitleT.y][x + TitleT.x] = Title_UI[y][x];
//		}
//	}
//
//	//Start text
//	for (int y = 0; y < 16; y++)
//	{
//		for (int x = 0; x < 64; x++)
//		{
//			if (Start_UI[y][x] != 0x00)
//				frameBuffer[y + StartT.y][x + StartT.x] = Start_UI[y][x];
//		}
//	}
//
//
//	//Cursor sprite
//	for (int y = 0; y < 12; y++)
//	{
//		for (int x = 0; x < 10; x++)
//		{
//			if (selectCursor == 1)
//			{
//				if (Cursor_UI[y][x] != 0x00)
//				{
//					frameBuffer[y + QuitT.y + 2][x + 70] = Cursor_UI[y][x];
//				}
//			}
//			else
//			{
//				if (Cursor_UI[y][x] != 0x00)
//				{
//					frameBuffer[y + StartT.y + 1][x + 55] = Cursor_UI[y][x];
//				}
//			}
//		}
//	}
//
//	//Quit Text
//	for (int y = 0; y < 16; y++)
//	{
//		for (int x = 0; x < 36; x++)
//		{
//			//Quit text
//			if (Quit_UI[y][x] != 0x00)
//				frameBuffer[y + QuitT.y][x + QuitT.x] = Quit_UI[y][x];
//		}
//	}
//
//	//Copyright logo
//	for (int y = 0; y < 16; y++)
//	{
//		for (int x = 0; x < 16; x++)
//		{
//			if (Copyright[y][x] != 0x00)
//				frameBuffer[y + 212][x + 182] = Copyright[y][x];
//		}
//	}
//
//	//Player logo
//	for (int y = 0; y < 26; y++)
//	{
//		for (int x = 0; x < 16; x++)
//		{
//			if (Player_MOVINGSIDE[0][y][x] != 0x00)
//				frameBuffer[y + 10][16 - x + 10] = Player_MOVINGSIDE[0][y][x];
//		}
//	}
//
//	//Enemy logo
//	for (int y = 0; y < 14; y++)
//	{
//		for (int x = 0; x < 11; x++)
//		{
//			if (Enemy_MOVING[0][y][x] != 0x00)
//			{
//				frameBuffer[y + 15][11 - x + 175] = Enemy_MOVING[0][y][x];
//				frameBuffer[y + 18][11 - x + 163] = Enemy_MOVING[0][y][x];
//			}
//		}
//	}
//}
//
////Render gameOver
//void gameOver_render()
//{
//	//Background
//	for (int y = 0; y < SCREEN_HEIGHT; y++)
//	{
//		for (int x = 0; x < SCREEN_WIDTH; x++)
//		{
//			frameBuffer[y][x] = BLACK;
//		}
//	}
//
//	//GameOver text
//	for (int y = 0; y < 24; y++)
//	{
//		for (int x = 0; x < 150; x++)
//		{
//			if (gameOver_UI[y][x] != 0x00)
//				frameBuffer[y + GameOverT.y][x + GameOverT.x] = gameOver_UI[y][x];
//		}
//	}
//
//	//Restart text
//	for (int y = 0; y < 16; y++)
//	{
//		for (int x = 0; x < 70; x++)
//		{
//			if (Restart_UI[y][x] != 0x00)
//				frameBuffer[y + RestartT.y][x + RestartT.x] = Restart_UI[y][x];
//		}
//	}
//
//	//Cursor sprite
//	for (int y = 0; y < 12; y++)
//	{
//		for (int x = 0; x < 10; x++)
//		{
//			if (selectCursor == 1)
//			{
//				if (Cursor_UI[y][x] != 0x00)
//				{
//					frameBuffer[y + QuitT.y + 2][x + 70] = Cursor_UI[y][x];
//				}
//			}
//			else
//			{
//				if (Cursor_UI[y][x] != 0x00)
//				{
//					frameBuffer[y + RestartT.y + 1][x + 55] = Cursor_UI[y][x];
//				}
//			}
//		}
//	}
//
//	//Quit Text
//	for (int y = 0; y < 16; y++)
//	{
//		for (int x = 0; x < 36; x++)
//		{
//			//Quit text
//			if (Quit_UI[y][x] != 0x00)
//				frameBuffer[y + QuitT.y][x + QuitT.x] = Quit_UI[y][x];
//		}
//	}
//}
//
////Result render
//void Result_render()
//{
//	//Background
//	for (int y = 0; y < SCREEN_HEIGHT; y++)
//	{
//		for (int x = 0; x < SCREEN_WIDTH; x++)
//		{
//			frameBuffer[y][x] = 0x07;
//		}
//	}
//
//	//Victory text
//	for (int y = 0; y < 33; y++)
//	{
//		for (int x = 0; x < 128; x++)
//		{
//			if (Victory_Text[y][x] != 0x00)
//				frameBuffer[y + VictoryT.y][x + VictoryT.x] = Victory_Text[y][x];
//		}
//	}
//
//	//Restart text
//	for (int y = 0; y < 16; y++)
//	{
//		for (int x = 0; x < 70; x++)
//		{
//			if (Restart_UI[y][x] != 0x00)
//				frameBuffer[y + RestartT.y][x + RestartT.x] = Restart_UI[y][x];
//		}
//	}
//
//	//Cursor sprite
//	for (int y = 0; y < 12; y++)
//	{
//		for (int x = 0; x < 10; x++)
//		{
//			if (selectCursor == 1)
//			{
//				if (Cursor_UI[y][x] != 0x00)
//				{
//					frameBuffer[y + QuitT.y + 2][x + 70] = Cursor_UI[y][x];
//				}
//			}
//			else
//			{
//				if (Cursor_UI[y][x] != 0x00)
//				{
//					frameBuffer[y + RestartT.y + 1][x + 55] = Cursor_UI[y][x];
//				}
//			}
//		}
//	}
//
//	//Quit Text
//	for (int y = 0; y < 16; y++)
//	{
//		for (int x = 0; x < 36; x++)
//		{
//			//Quit text
//			if (Quit_UI[y][x] != 0x00)
//				frameBuffer[y + QuitT.y][x + QuitT.x] = Quit_UI[y][x];
//		}
//	}
//}
//
////AI script logic
//void AI_Behaviour(chara* player, chara* enemy)
//{
//	if (enemy->position.x <= player->position.x)
//	{
//		enemy->direction = RIGHT;
//		enemy->position.x += 1;
//	}
//	else
//	{
//		enemy->direction = LEFT;
//		enemy->position.x -= 1;
//	}
//	if (enemy->position.y <= player->center.y)
//	{
//		enemy->position.y += 1;
//	}
//	else
//	{
//		enemy->position.y -= 1;
//	}
//}
//
////bullet sprite render
//void bullet_sprite(chara* player, object* bullets)
//{
//	for (int y = 0; y < 7; y++)
//	{
//		for (int x = 0; x < 8; x++)
//		{
//			if (bullets->enable)
//			{
//				if (Bullet[y][x] != 0x00)
//				{
//					frameBuffer[y + bullets->position.y][x + bullets->position.x] = Bullet[y][x];
//				}
//			}
//		}
//	}
//}

//TODO LIST