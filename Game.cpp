/**
* @file		Game.cpp
* @brief
* -This file contains game event handlers
* 
* @note
* TODO LIST
* - Start menu fix (frameBuffer) -- DONE --
* - More player sprite & animation
* - Camera Movement
* - Bullet logic
* - Memory Optimization
* - Timer for Game System
* - Timer for FPS Optimization
* - PFRAMEBUFFER for bitmap rendering(大本先生に聞く)
*/
#include "Game.h"

/* Global Variables */
char frameBuffer[SCREEN_HEIGHT][SCREEN_WIDTH];
bool gameRunning = false;
int stage;
int	distance_x[5], distance_y[5];
int KeyPush[NUM_KEY];
int KeyEdge[NUM_KEY];

/* Framerate settings*/
int frame = 0;
const int FPS = 120;
const int frameDelay = 1000 / FPS;
UINT32 frameStart;

/* Sound settings */
const char* bgm01 = "bgm\\Struggle.wav";
const char* bgm02 = "bgm\\BGM02.mp3";
int* bgm01_id = MciOpenSound(bgm01);
int* bgm02_id = MciOpenSound(bgm02);

//Initialize Sound
void InitSound(int *bgm)
{
	//Play sound
	MciPlaySound(bgm, MCI_DEVTYPE_WAVEFORM_AUDIO);
	MciSetVolume(bgm, 50);
	//MciPlaySound(bgm02_id, MCI_DEVTYPE_CD_AUDIO);
}

//Initialize game 
void InitWindow()
{
	//Set Console Title
	SetCaption("HEW Project REworked");

	//Set font size settings
	SetScreenFontSize(FONT_WIDTH, FONT_HEIGHT);

	//Initialize Console I/O
	InitConio(SCREEN_WIDTH, SCREEN_HEIGHT);

	//Initialize double buffer
	InitDoubleBuffer();
}

//For rendering startmenu
void StartMenu()
{
	//================================================================
	//■Forwaring image data to frameBuffer & render
	//================================================================

	//TODO LIST
	//
	//
	//
	//
	title_render();
}

//For rendering stage01
void Stage01()
{
	//================================================================
	//■Forwaring image data to frameBuffer & render
	//================================================================
	if (frame % 1 == 0)
	{
		//Load map sprite
		map_sprite();

		//Player sprite render
		player_sprite(&player, &gameCamera);

		//Enemy sprite render
		enemy_sprite(&enemy[0]);
		enemy_sprite(&enemy[1]);
		enemy_sprite(&enemy[2]);
		enemy_sprite(&enemy[3]);
		enemy_sprite(&enemy[4]);

		//AI Behaviour
		AI_Behaviour(&player, &enemy[0]);
		AI_Behaviour(&player, &enemy[1]);
		AI_Behaviour(&player, &enemy[2]);
		AI_Behaviour(&player, &enemy[3]);
		AI_Behaviour(&player, &enemy[4]);

		//TODO LIST Player attack animation 
		if (player.attacking == true)
		{
			bullet_sprite(&player, &bullets);
		}
	}
}

//Game over screen
void GameOver()
{
	//TODO LIST
	gameOver_render();
}

void Result()
{

}

void KeyRead()
{
	for (int n = 0; n < NUM_KEY; n++)
	{
		int k = InputKeyMouse(n);
		if ((KeyPush[n] == 0) && (k != 0))
		{
			KeyEdge[n] = 1;
		}
		else
			KeyEdge[n] = 0;
		KeyPush[n] = (k ? 1 : 0);
	}
}

//Game loop goes here
void MainGame()
{
	//Load starting game data
	Start();

	//Load BMP
	//FileLoad(&imgFile);
	//SetWindowBMP(imgFile, &imgFile);

	//Set BGM for each stages
	switch (stage)
	{
	case stages::startmenu:
		//Initialize BGM
		InitSound(bgm02_id);
		break;
	case stages::stage01:
		//Stop BGM from startmenu
		MciStopSound(bgm02_id);
		//Initialize BGM
		InitSound(bgm01_id);
		break;
	}

	//Main Game Loop
	while (gameRunning)
	{
		switch (stage)
		{
		case stages::startmenu:
		{
			//Load startmenu image
			//load_image(imgFile.pFB0, imgFile.frameBuffer);
			Render(&StartMenu);
			if (InputKeyMouse(VK_RETURN))
				stage = stages::stage01;
			break;
		}
		case stages::stage01:
		{
			//Checking for input
			Input(&player);

			//Update event handler
			Update(&player, &enemy[0], &gameCamera);

			//Render game graphics
			Render(&Stage01);
			break;
		}
		case stages::bossStage:
		{
			//TODO List
			break;
		}
		case stages::victory:
		{
			//TODO List
			break;
		}
		default:
			break;
		}
	}
}

//Handles all the data for initialization
void Start()
{
	//Assign imgFile
	//imgFile = { NULL, NULL, NULL, NULL };

	//Generate random number
	srand((unsigned)time(0));

	//Set game state to running 
	gameRunning = true;

	//Set stage to stage01 for debugging
	stage = stages::startmenu;

	//Starting position set to center
	player.position.x = 100;
	player.position.y = 100;

	//Set player height & width
	player.imageHeight = 36;
	player.imageWidth = 20;

	//Set bullet position to player center
	bullets.position.x = 0;
	bullets.position.y = 0;

	//Set enemy sprite attributes
	for (int i = 0; i < 5; i++)
	{
		//Starting position for enemy
		enemy[i].position.x = (rand() % 200) + 1;
		enemy[i].position.y = (rand() % 200) + 1;

		//Set image attributes
		enemy[i].imageHeight = 14;
		enemy[i].imageWidth = 11;
	}

	//Camera center transform
	gameCamera.imageHeight = 60;
	gameCamera.imageWidth = 60;

	//Set camera position
	gameCamera.position.x = player.center.x;
	gameCamera.position.y = player.center.y;
}

//Handles player input 
void Input(chara* player)
{
	//Reading keyboard input
	KeyRead();

	//Game state check
	if (KeyEdge[VK_ESCAPE])
	{
		gameRunning = false;
	}

	//Player movement
	if (KeyPush[PK_A])
	{
		player->position.x -= 4;
		player->moving = true;
		player->direction = LEFT;
	}

	if (KeyPush[PK_D])
	{
		player->position.x += 4;
		player->moving = true;
		player->direction = RIGHT;
	}

	if (KeyPush[PK_W])
	{
		player->position.y -= 4;
		player->moving = true;
	}

	if (KeyPush[PK_S])
	{
		player->position.y += 4;
		player->moving = true;
	}

	//If nothing is pressed then set moving to false
	if (!InputKeyMouse)
	{
		player->moving = false;
	}

	//Attack
	if (KeyEdge[VK_SPACE])
	{
		player->attacking = true;

	}
	else
	{
		player->attacking = false;
	}

	//---------------------------------------------------------------------
}

//Game event update handler & State machine
void Update(chara *player, chara* enemy, object *camera)
{
	//Clamp player position within the border
	if (player->position.x < 0)
	{
		player->position.x = 0;
	}
	//Right side border
	if (player->position.x > (SCREEN_WIDTH - player->imageWidth))
	{
		player->position.x = SCREEN_WIDTH - player->imageWidth;
	}
	//Upper side border
	if (player->position.y < 0)
	{
		player->position.y = 0;
	}

	//Lower side border
	if (player->position.y > (SCREEN_HEIGHT - player->imageHeight))
	{
		player->position.y = SCREEN_HEIGHT - player->imageHeight;
	}

	//-------------------------------------------------------------
	//Player animation state
	if (player->moving == false)
	{
		player->anim = IDLE;
	}
	else
	{
		player->anim++;
	}
	//Looping animation 
	if (player->anim > 5)
	{
		player->anim = 0;
	}

	//TODO Camera Movement
	camera->position.x = (player->position.x + (player->imageWidth / 2)) - (camera->imageWidth / 2);
	camera->position.y = (player->position.y + (player->imageHeight / 2)) - (camera->imageHeight / 2);

	//--------------------------------------------------------------
	//Update player & enemy position for collision

	//Set player center for collision
	player->center.x = player->position.x + (player->imageWidth / 2);
	player->center.y = player->position.y + (player->imageHeight / 2);

	//Set bullet shooting position
	bullets.position.x = player->center.x;
	bullets.position.y = player->center.y;

	for (int i = 0; i < 5; i++)
	{
		//Update animation per frame
		enemy[i].anim++;
		//Clamp enemy animation
		if (enemy[i].anim > 2)
		{
			enemy[i].anim = 0;
		}

		//Set enemy center for collision
		enemy[i].center.x = enemy[i].position.x + (enemy[i].imageWidth / 2);
		enemy[i].center.y = enemy[i].position.y + (enemy[i].imageHeight / 2);

		//Player & enemy distance
		distance_x[i] = player->center.x - enemy[i].center.x;
		distance_y[i] = player->center.y - enemy[i].center.y;

		//TODO LIST distance for collision detection
		if (distance_x[i] < 0)
			distance_x[i] *= -1;
		if (distance_y[i] < 0)
			distance_y[i] *= -1;

		if (distance_x[i] <= 0 && distance_y[i] <= 0)
			player->collided = 1;
	}

	collision(player, enemy, &solidObject);
}

//Collision handler 
void collision(character* player, character *enemy, object *obj)
{
	//TODO LIST
	if (player->collided == 1)
	{
		player->HP--;
	}
}

//Render passed function as game data for rendering
void Render(void (*func)())
{
	//Set for framerate
	int t = timeGetTime();

	//Clearing frameBuffer space for rendering
	ClearFrameBuffer(frameBuffer[0]);

	//Assign color palette from RGBQUAD to ConsolePalette
	GetConsolePalette();
	CopyPalette(ColorPalette);
	SetConsolePalette();

	//Render buffer for each stages
	func();

	//Forwarding image to the frameBuffer
	PrintFrameBuffer(frameBuffer[0]);

	//Flip screen to the second buffer
	FlipScreen();

	//Set framerate
	int w = frameDelay - (timeGetTime() - t);
	if (w > 0) {
		Sleep(w);
	}
	frame++;
}

//Initialize bullet 
void InitBullet(object* bullets)
{
	bullets->prevPos = bullets->position;

}

//Free memory
void FreeMemory()
{
	free(frameBuffer);
	//FileEnd(&imgFile);
	//free(pFB);
}

/**
* @brief	スプライトをオブジェクトに読み込む
*
* @param	sprite_path->sprite file name;
*			pointer to object structure
*/
void load_sprite(object* obj, const char* _path)
{
	//Pass parameter to path value
	obj->path = _path;

	//Initialize FILE struct for image rendering
	FILE* image;

	//Opening file 
	image = fopen(obj->path, "rb");

	//Acquite sprite attributes
	fseek(image, 18, SEEK_SET);
	fread(&(obj->imageWidth), 4, 2, image);

	//Acquire sprite data
	fseek(image, 118, SEEK_SET);
	for (int y = obj->imageHeight - 1; y >= 0; y--)
	{
		for (int x = 0; x < obj->imageWidth; x++)
		{
			char pixels;
			fread(&pixels, 1, 1, image);
			obj->sprite[y][x] = (pixels & 240) >> 4;
			obj->sprite[y][++x] = pixels & 15;
		}
	}

	//Close image file
	fclose(image);
	return;
}

//Player sprite rendering with camera
void player_sprite(character* player, object* camera)
{                                    
	//Player sprite render
	for (int y = 0; y < 36; y++)
	{
		for (int x = 0; x < 20; x++)
		{
			if (Player_IDLE[player->anim][y][x] != 0x00)
			{
				//Character sprite facing right
				if (player->direction == RIGHT)
					frameBuffer[y + player->position.y][x + player->position.x] = Player_IDLE[player->anim][y][x];
				//Character sprite facing left
				else
					frameBuffer[y + player->position.y][20 - x + player->position.x] = Player_IDLE[player->anim][y][x];
			}
		}
	}
}

//Map sprite rendering
void map_sprite()
{
	//Load Map[20][20] to Map_Render[20 * 32][20 * 32]
	char Map_Render[10 * 32][10 * 32];

	//Load objects data to map
	for (int y = 0; y < 10; y++)
	{
		for (int x = 0; x < 10; x++)
		{
			switch (Map2[y][x])
			{
			//TREES
			case 0:
			{
				for (int k = 0; k < 23; k++)
				{
					for (int l = 0; l < 13; l++)
					{
						if (Trees[k][l] != 0x00)
						{
							Map_Render[y * 20 + k][x * 20 + l] = Trees[k][l];
						}
					}
				}
				break;
			}
			//DIRT GROUND
			case 1:
			{
				for (int k = 0; k < 32; k++)
				{
					for (int l = 0; l < 32; l++)
					{
						Map_Render[y * 20 + k][x * 20 + l] = Dirt[k][l];
					}
				}
				break;
			}
			//OBSTACLES
			case 2:
			{
				for (int k = 0; k < 14; k++)
				{
					for (int l = 0; l < 16; l++)
					{
						if (Obstacles[k][l] != 0x00)
							Map_Render[y * 20 + k][x * 20 + l] = Obstacles[k][l];
					}
				}
				break;
			}
			//HOUSE
			case 3:
			{
				for (int k = 0; k < 14; k++)
				{
					for (int l = 0; l < 14; l++)
					{
						Map_Render[y * 20 + k][x * 20 + l] = House[k][l];
					}
				}
				break;
			}
			default:
				break;
			}
		}
	}

	//forwarding map data to frameBuffer
	for (int y = 0; y < SCREEN_HEIGHT; y++)
	{
		for (int x = 0; x < SCREEN_WIDTH; x++)
		{
			frameBuffer[y][x] = Map_Render[y][x];
			//frameBuffer[y][x] = WHITE;
		}
	}
}

//Load enemy sprite
void enemy_sprite(chara* enemy)
{
	for (int y = 0; y < 14; y++)
	{
		for (int x = 0; x < 11; x++)
		{
			if (Enemy_MOVING[enemy->anim][y][x] != 0x00)
			{
				if (enemy->direction == RIGHT)
					frameBuffer[y + enemy->position.y][x + enemy->position.x] = Enemy_MOVING[enemy->anim][y][x];
				else
					frameBuffer[y + enemy->position.y][11 - x + enemy->position.x] = Enemy_MOVING[enemy->anim][y][x];
			}
		}
	}
}

//Title render
void title_render()
{
	//Background
	for (int y = 0; y < SCREEN_HEIGHT; y++)
	{
		for (int x = 0; x < SCREEN_WIDTH; x++)
		{
			frameBuffer[y][x] = WHITE;
		}
	}

	//Foreground & texts
	for (int y = 0; y < 30; y++)
	{
		for (int x = 0; x < 30; x++)
		{
			//Title UI text
			frameBuffer[y + 60][x + 85] = Title_UI[y][x];

			//Start text
			frameBuffer[y + 80][x + 85] = Start_UI[y][x];

			//Quit text
			frameBuffer[y + 100][x + 85] = Quit_UI[y][x];
		}
	}
}

//Render gameOver
void gameOver_render()
{
	//Background
	for (int y = 0; y < SCREEN_HEIGHT; y++)
	{
		for (int x = 0; x < SCREEN_WIDTH; x++)
		{
			frameBuffer[y][x] = BLACK;
		}
	}

	//Foreground & texts
	for (int y = 0; y < 30; y++)
	{
		for (int x = 0; x < 30; x++)
		{
			//Game over text
			frameBuffer[y + 60][x + 85] = gameOver_UI[y][x];

			//restart text
			frameBuffer[y + 80][x + 85] = Restart_UI[y][x];

			//Quit text
			frameBuffer[y + 100][x + 85] = Quit_UI[y][x];
		}
	}
}

//AI script logic
void AI_Behaviour(chara* player, chara* enemy)
{
	if (enemy->position.x <= player->center.x)
	{
		enemy->direction = RIGHT;
		enemy->position.x += 2;
	}
	else
	{
		enemy->direction = LEFT;
		enemy->position.x -= 2;
	}
	if (enemy->position.y <= player->center.y)
	{
		enemy->position.y += 2;
	}
	else
	{
		enemy->position.y -= 2;
	}
}

//bullet sprite render
void bullet_sprite(chara* player, object* bullets)
{
	for (int y = 0; y < 7; y++)
	{
		for (int x = 0; x < 8; x++)
		{
			for (int z = 0; z < 20; z++)
			{
				if (Bullet[y][x] != 0x00)
				{
					if (player->direction == RIGHT)
						frameBuffer[y + bullets->position.y][x + bullets->position.x + z] = Bullet[y][x];
					else
						frameBuffer[y + bullets->position.y][z - (x - bullets->position.x)] = Bullet[y][x];
				}
			}
		}
	}
}

//Load BMP for stage01
void load_image(PFRAMEBUFFER pFB, char* frameBuffer)
{
	int t = timeGetTime();

	//Initialize Console I/O
	InitConio(pFB->width, pFB->height);

	//Initialize 4 bit bmp
	if (pFB->bih.biBitCount == 4)
	{
		GetConsolePalette();
		CopyPalette(pFB->ppal);
		SetConsolePalette();
	}
	FixWin();

	ClearFrameBufferFull(frameBuffer);
	memcpy(frameBuffer, pFB->ppx, pFB->ppx_size);

	//-------------------------------------------
	//Render
	if (pFB->bih.biBitCount == 4)
		PrintFrameBuffer(frameBuffer);
	else if (pFB->bih.biBitCount == 24)
		PrintImage((PBYTE)frameBuffer);

	FlipScreen();

	//Set framerate
	//１フレームの表示時間待ち
	int w = frameDelay - (timeGetTime() - t);
	if (w > 0) {
		Sleep(w);
	}
	frame++;
}


//Swap RGB to BGR for 24bpp bitmap
void SwapBGR(PFRAMEBUFFER pFB)
{
	if (pFB->bih.biBitCount == 24)
		bmp_swapRB(pFB);
}

//SetWindow for BMP
void SetWindowBMP(Image img, pImage pImg)
{
	pImg->frameBuffer = (char*)calloc(img.pFB0->ppx_size, sizeof(BYTE));
	InitConio(img.pFB0->width, img.pFB0->height);

	SetScreenFontSize(FONT_WIDTH, FONT_HEIGHT);
}

//Load file
void FileLoad(pImage img)
{
	if (!(img->pFB0 = bmp_load("bmp\\title.bmp")))
		return;

	//Swap RGB to BGR if bmp file is 24bpp
	SwapBGR(img->pFB0);
}

//end file
void FileEnd(pImage img)
{
	//End bmp for title
	bmp_end(img->pFB0);

	//bmp_end(img->pFB1);
}

///end of file
///-----------------------------------------------------------------