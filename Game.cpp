/**
* @file		Game.cpp
* @brief
* -This file contains game event handlers
* 
* @note
* TODO LIST
* - Start menu fix (frameBuffer) -- DONE --
* - More player sprite & animation -- DONE --
* - Camera Movement -- ABANDONED --
* - Spawn more enemies -- HALF DONE --
* - Bullet logic -- DONE --
* - Collision -- HALF DONE --
* - Memory Optimization -- HALF DONE --
* - Timer for Game System -- HALF DONE --
* - Timer for FPS Optimization -- DONE --
*/
#include "Game.h"

/* Preprocessors */
#define NO_MOVEMENT (!InputKeyMouse(PK_A) || !InputKeyMouse(PK_D) || !InputKeyMouse(PK_W) || !InputKeyMouse(PK_S))

/* Global Variables */
char frameBuffer[SCREEN_HEIGHT][SCREEN_WIDTH];
bool gameRunning = false;
int selectCursor;
int stage;
int wave;
int killCount;
int icon_anim = 0;
int posCount = 0;
int KeyPush[NUM_KEY];
int KeyEdge[NUM_KEY];
//int icon_anim = 0;
Point TitleT{ 36, 50 };
Point VictoryT{ 36,40 };
Point GameOverT{ 25, 50 };
Point StartT{ 69, 110 };
Point RestartT{ 68,100 };
Point QuitT{ 83, 140 };

/* Framerate settings*/
float timer{ 0.0f };
int frame = 0;
int FPSCount = 0;
int execLastTime = 0;
int currentTime = 0;
int fpsLastTime = 0;
const int FPS = 60;
const int frameDelay = 1000 / FPS;


//Initialize Sound
void SoundPlay(int *bgm)
{
	//Play sound
	MciPlaySound(bgm, 0);
	MciSetVolume(bgm, 50);
	//MciPlaySound(bgm02_id, MCI_DEVTYPE_CD_AUDIO);
}

//Initialize game 
void InitWindow()
{
	//Set Console Title
	SetCaption("Familiar");

	//Set font size settings
	SetScreenFontSize(FONT_WIDTH, FONT_HEIGHT);

	//Initialize Console I/O
	InitConio(SCREEN_WIDTH, SCREEN_HEIGHT);

	//Initialize double buffer
	InitDoubleBuffer();
}

//Tutorial menu
void Tutorial()
{
	//================================================================
	//Å°Forwaring image data to frameBuffer & render
	//================================================================
	tutorial_render();
}

//For rendering startmenu
void StartMenu()
{
	//================================================================
	//Å°Forwaring image data to frameBuffer & render
	//================================================================
	title_render();
}

//For rendering stage01
void Stage01()
{
	//================================================================
	//Å°Forwaring image data to frameBuffer & render
	//================================================================
	if (frame % 1 == 0)
	{
		//Load map sprite
		map_sprite();

		//Player sprite render
		player_sprite(&player, &gameCamera);

		//Instantiate enemy
		for (int i = 0; i < 10; i++)
		{
			if (enemy[i].alive == 1)
			{
				enemy_sprite(&enemy[i]);
				AI_Behaviour(&player, &enemy[i]);
			}
		}
			

		//TODO LIST Player shoot instantiate bullet
		for (int count = 0; count < 100; count++)
		{
			if (bullets[count].enable)
			{
				bullet_sprite(&player, &bullets[count]);
			}
		}
	}
}

//Game over screen
void GameOver()
{
	//================================================================
	//Å°Forwaring image data to frameBuffer & render
	//================================================================
	gameOver_render();
}

//Result 
void Result()
{
	//================================================================
	//Å°Forwaring image data to frameBuffer & render
	//================================================================
	Result_render();
}

//Keyboard input handler for each modes
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
	selectCursor = 0;
	stage = stages::tutorial;
	SoundPlay(bgm_id[BGMNUM::TUTORIAL]);
	MciSetVolume(bgm_id[BGMNUM::TUTORIAL], 10);

	//Initialize game data
	Start();


	//Set time
	timeBeginPeriod(1);
	execLastTime = fpsLastTime = timeGetTime();
	currentTime = frame = 0;

	//Main Game Loop
	while (gameRunning)
	{
		switch (stage)
		{
		case stages::startmenu:
		{
			//Input handle
			KeyRead();
			
			//Render graphics
			Render(&StartMenu);

			// -- SELECTION --
			if (KeyEdge[VK_UP])
			{
				SoundPlay(sfx_id[SFXNUM::SELECT]);
				selectCursor--;
			}
			else if (KeyEdge[VK_DOWN])
			{
				SoundPlay(sfx_id[SFXNUM::SELECT]);
				selectCursor++;
				//MciStopSound(bgm06_id);
			}
			if (selectCursor < 0)
				selectCursor = 0;
			if (selectCursor > 1)
				selectCursor = 1;

			if (KeyEdge[VK_RETURN])
			{	
				if (selectCursor == 0)
				{
					SoundPlay(sfx_id[SFXNUM::ENTER]);
					stage = stages::stage01;
					MciStopSound(bgm_id[BGMNUM::TITLE]);
					SoundPlay(bgm_id[BGMNUM::MAIN]);
				}
				else if (selectCursor == 1)
				{
					SoundPlay(sfx_id[SFXNUM::ENTER]);
					gameRunning = false;
				}
			}
#ifdef _DEBUG
			DisplayFPS();
#endif // _DEBUG

			MciUpdateSound(bgm_id[BGMNUM::TITLE]);
			break;
		}
		case stages::stage01:
		{
			currentTime = timeGetTime();
			if ((currentTime - fpsLastTime) >= 500)
			{
				FPSCount = frame * 1000 / (currentTime - fpsLastTime);
				fpsLastTime = currentTime;
				frame = 0;
			}
			if (currentTime - execLastTime >= frameDelay)
			{
				//For framerate
				execLastTime = currentTime;

				//Checking for input
				Input(&player);

				//Update event handler
				Update(&player, &enemy[0], &gameCamera);

				//Render game graphics
				Render(&Stage01);
			}
			frame++;

			//Update Sound
			MciUpdateSound(bgm_id[BGMNUM::MAIN]);

#ifdef _DEBUG
			DisplayFPS();
#endif // _DEBUG

			break;
		}
		case stages::tutorial:
		{
			//Read key input
			KeyRead();
			//Render tutorial
			Render(&Tutorial);

			//Go to the next stage
			if (KeyEdge[VK_RETURN])
			{
				SoundPlay(sfx_id[SFXNUM::ENTER]);
				stage = stages::startmenu;
				MciStopSound(bgm_id[BGMNUM::TUTORIAL]);
			}
			if (KeyEdge[VK_ESCAPE])
			{
				gameRunning = false;
			}

			//Play sound for startmenu
			//--Play it here so it wont be looped inside startmenu--
			SoundPlay(bgm_id[BGMNUM::TITLE]);

			break;
		}
		case stages::victory:
		{
			//Read key input
			KeyRead();
			//Render result screen
			Render(&Result);

			// -- SELECTION --
			if (KeyEdge[VK_UP])
			{
				SoundPlay(sfx_id[SFXNUM::SELECT]);
				selectCursor--;
			}
			else if (KeyEdge[VK_DOWN])
			{
				SoundPlay(sfx_id[SFXNUM::SELECT]);
				selectCursor++;
			}
			if (selectCursor < 0)
				selectCursor = 0;
			if (selectCursor > 1)
				selectCursor = 1;

			if (KeyEdge[VK_RETURN])
			{
				SoundPlay(sfx_id[SFXNUM::ENTER]);
				if (selectCursor == 0)
				{
					Start();
					MciStopSound(sfx_id[SFXNUM::VICTORY]);
					SoundPlay(bgm_id[BGMNUM::MAIN]);
					stage = stages::stage01;
				}
				else if (selectCursor == 1)
					gameRunning = false;
			}
#ifdef _DEBUG
			DisplayFPS();
#endif // _DEBUG

			break;
		}
		case stages::defeat:
		{
			//Read key input
			KeyRead();
			//Render gameover screen
			Render(&GameOver);

			// -- SELECTION --
			if (KeyEdge[VK_UP])
			{
				SoundPlay(sfx_id[SFXNUM::SELECT]);
				selectCursor--;
			}
			else if (KeyEdge[VK_DOWN])
			{
				SoundPlay(sfx_id[SFXNUM::SELECT]);
				selectCursor++;
			}
			if (selectCursor < 0)
				selectCursor = 0;
			if (selectCursor > 1)
				selectCursor = 1;

			if (KeyEdge[VK_RETURN])
			{
				SoundPlay(sfx_id[SFXNUM::ENTER]);
				if (selectCursor == 0)
				{
					Start();
					MciStopSound(sfx_id[SFXNUM::GAMEOVER]);
					SoundPlay(bgm_id[BGMNUM::MAIN]);
					stage = stages::stage01;
				}
				else if (selectCursor == 1)
					gameRunning = false;
			}

#ifdef _DEBUG
			DisplayFPS();
#endif // _DEBUG

			break;
		}
		default:
			break;
		}
	}

	//End time 
	timeEndPeriod(1);
}

//Handles all the data for initialization
void Start()
{
	//Game state 
	gameRunning = true;
	timer = 50.0f;
	icon_anim = 0;
	killCount = 0;
	player.moving = false;
	player.attacking = false;
	player.direction = FRONT;
	player.HP = 3;
	wave = 1;

	//Starting position set to center
	player.position.x = 100;
	player.position.y = 100;

	//Set player height & width
	player.imageHeight = 26;
	player.imageWidth = 16;

	//InitBullet 
	InitBullet();

	//Set enemy sprite attributes
	InitEnemy();

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
	//- KeyPush -- read pressed and hold keyboard input
	//- KeyEdge -- read pressed keyboard input once only
	KeyRead();

	//Game state check
	if (KeyEdge[VK_ESCAPE])
	{
		gameRunning = false;
	}

	//Player movement
	if (KeyPush[PK_A])
	{
		player->position.x -= 2;
		player->moving = true;
		player->direction = LEFT;
	}

	if (KeyPush[PK_D])
	{
		player->position.x += 2;
		player->moving = true;
		player->direction = RIGHT;
	}

	if (KeyPush[PK_W])
	{
		player->position.y -= 2;
		player->moving = true;
		player->direction = BACK;
	}

	if (KeyPush[PK_S])
	{
		player->position.y += 2;
		player->moving = true;
		player->direction = FRONT;
	}
	
	if NO_MOVEMENT
	{
		if (player->moving)
		{
			player->moving = false;
			player->anim++;
		}
		else if (!player->moving)
			player->anim = IDLE;
	}

	// ---------------------------------------------------------------------------
	// -- INSTANTIATE BULLETS --
	if (KeyEdge[VK_UP])
	{
		if (!player->attacking)
		{
			SetBullet(player->center.x + 2, player->position.y, 0, -3);
			player->attacking = true;
			player->direction = BACK;
			SoundPlay(sfx_id[SFXNUM::SHOOT]);
		}
	}
	else if (KeyEdge[VK_DOWN])
	{
		if (!player->attacking)
		{
			SetBullet(player->center.x + 2, player->center.y + 2, 0, 3);
			player->attacking = true;
			player->direction = FRONT;
			SoundPlay(sfx_id[SFXNUM::SHOOT]);
		}
	}
	else if (KeyEdge[VK_LEFT])
	{
		if (!player->attacking)
		{
			SetBullet(player->center.x + 2, player->center.y, -3, 0);
			player->attacking = true;
			player->direction = LEFT;
			SoundPlay(sfx_id[SFXNUM::SHOOT]);
		}
	}
	else if (KeyEdge[VK_RIGHT])
	{
		if (!player->attacking)
		{
			SetBullet(player->center.x + 2, player->center.y, 3, 0);
			player->attacking = true;
			player->direction = RIGHT;
			SoundPlay(sfx_id[SFXNUM::SHOOT]);
		}
	}
	else
	{
		if (player->attacking)
		{
			player->moving = false;
			player->attacking = false;
		}
	}

	//---------------------------------------------------------------------
}

//Game event update handler & State machine
void Update(chara *player, chara* enemy, object *camera)
{
	//Update player prev position for bullet instance
	player->prevPos = player->position;

	//Clamp player position within the border
	if (player->position.x < 0)
	{
		player->position.x = 0;
	}
	//Right side border
	if (player->position.x > (VIEWPORT_W - player->imageWidth))
	{
		player->position.x = VIEWPORT_W - player->imageWidth;
	}
	//Upper side border
	if (player->position.y < 0)
	{
		player->position.y = 0;
	}

	//Lower side border
	if (player->position.y > (VIEWPORT_H - player->imageHeight))
	{
		player->position.y = VIEWPORT_H - player->imageHeight;
	}


	//----------------------------------------------------------------
	//Looping animation 
	if (player->anim > 3)
	{
		player->anim = 0;
	}

	//Health icon animation
	icon_anim++;
	if (icon_anim > 3)
		icon_anim = 0;

	//TODO Camera Movement
	//camera->position.x = (player->position.x + (player->imageWidth / 2)) - (camera->imageWidth / 2);
	//camera->position.y = (player->position.y + (player->imageHeight / 2)) - (camera->imageHeight / 2);

	//----------------------------------------------------------------
	//Update player & enemy position for collision

	//Set player center for collision
	player->center.x = player->position.x + (player->imageWidth / 2);
	player->center.y = player->position.y + (player->imageHeight / 2);

	//Update enemy
	UpdateEnemy(player);

	//Set bullet shooting position
	UpdateBullet();
	BulletCollision();

	//Debugging for timer use
#ifdef _DEBUG
	//timer -= 0.1f;
#endif // _DEBUG
	
	//KillCount for each waves (10 waves in total)
	if (killCount == 10)
	{
		wave = 2;
		for (int i = 0; i < 10; i++)
			enemy[i].alive = 1;
	}
	else if (killCount == 20)
	{
		wave = 3;
		for (int i = 0; i < 10; i++)
			enemy[i].alive = 1;
	}
	else if (killCount == 30)
	{
		wave = 4;
		for (int i = 0; i < 10; i++)
			enemy[i].alive = 1;
	}
	else if (killCount == 40)
	{
		wave = 5;
		for (int i = 0; i < 10; i++)
			enemy[i].alive = 1;
	}
	else if (killCount == 50)
	{
		wave = 6;
		for (int i = 0; i < 10; i++)
			enemy[i].alive = 1;
	}
	else if (killCount == 60)
	{
		wave = 7;
		for (int i = 0; i < 10; i++)
			enemy[i].alive = 1;
	}
	else if (killCount == 70)
	{	
		wave = 8;
		InitEnemy();
	}
	else if (killCount == 80)
	{
		wave = 9;
		InitEnemy();
	}
	else if (killCount == 90)
	{
		wave = 10;
		InitEnemy();
	}

	//Set kill count for result
	if (wave == 10 && killCount >= 100)
	{
		MciStopSound(bgm_id[BGMNUM::MAIN]);
		stage = stages::victory;
		SoundPlay(sfx_id[SFXNUM::VICTORY]);
	}

	//Game over;
	if (player->HP <= 0)
	{
		MciStopSound(bgm_id[BGMNUM::MAIN]);
		stage = stages::defeat;
		SoundPlay(sfx_id[SFXNUM::GAMEOVER]);
	}
}

//Initialize enemy 
void InitEnemy()
{
	//Generate random number seed
	srand((unsigned)time(0));

	for (int i = 0; i < 10; i++)
	{
		//Set enemy alive
		enemy[i].alive = 1;

		//Starting position for enemy
		enemy[i].position.x = rand() % 200 + 1;
		enemy[i].position.y = rand() % 200 + 1;

		//Set image attributes
		enemy[i].imageHeight = 14;
		enemy[i].imageWidth = 11;
	}
}

//Enemy update
void UpdateEnemy(character* player)
{
	//Declare distances for each enemy
	int	distance_x[10], distance_y[10];
	int distance_x2[10], distance_y2[10];

	//Detection with broad phase
	for (int i = 0; i < 10; i++)
	{
		if (enemy[i].alive == 1)
		{
			//Update animation per frame
			enemy[i].anim++;
			//Clamp enemy animation
			if (enemy[i].anim > 2)
			{
				enemy[i].anim = 0;
			}

			//Set enemy center for collision
			enemy[i].center.x = enemy[i].position.x + (enemy[i].imageWidth / 2) - 2;
			enemy[i].center.y = enemy[i].position.y + (enemy[i].imageHeight / 2) - 2;

			//Player & enemy distance
			distance_x[i] = player->center.x - enemy[i].center.x;
			distance_y[i] = player->center.y - enemy[i].center.y;
			distance_x2[i] = (player->imageWidth / 2) + (enemy[i].imageWidth / 2) - 2;
			distance_y2[i] = (player->imageHeight / 2) + (enemy[i].imageHeight / 2) - 2;

			//Update to constant value
			if (distance_x[i] < 0)
				distance_x[i] *= -1;
			if (distance_y[i] < 0)
				distance_y[i] *= -1;

			//Collision detection
			if (distance_x[i] < distance_x2[i] && distance_y[i] < distance_y2[i])
			{
				SoundPlay(sfx_id[SFXNUM::PLAYERHIT]);
				player->HP--;
				if (player->direction == LEFT)
					player->position.x += 5;
				else if (player->direction == RIGHT)
					player->position.x -= 5;
				else if (player->direction == FRONT)
					player->position.y -= 5;
				else
					player->position.y += 5;
			}
		}
	}
}


//Render passed function as game data for rendering
void Render(void (*func)())
{
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
}

//Initialize bullet
void InitBullet()
{
	for (int x = 0; x < 100; x++)
	{
		bullets[x].position.x = 0;
		bullets[x].position.y = 0;
		bullets[x].prevPos.x = 0;
		bullets[x].prevPos.y = 0;
		bullets[x].move.x = 0;
		bullets[x].move.y = 0;
		bullets[x].enable = false;
	}
}
//Set bullet 
void SetBullet(int posX, int y, int moveX, int moveY)
{
	for (int x = 0; x < 100; x++)
	{
		if (!bullets[x].enable)
		{
			bullets[x].position.x = posX;
			bullets[x].position.y = y;
			bullets[x].prevPos = bullets[x].position;
			bullets[x].move.x = moveX;
			bullets[x].move.y = moveY;
			bullets[x].enable = true;

			break;
		}
	}
}

//Update bullet movement
void UpdateBullet()
{
	//Set bullet shooting position
	for (int y = 0; y < 100; y++)
	{
		if (bullets[y].enable)
		{
			//Set prev position to current position
			bullets[y].prevPos = bullets[y].position;

			//Update the current position to transform movement
			bullets[y].position.x += bullets[y].move.x;
			bullets[y].position.y += bullets[y].move.y;

			//The bullets hit the border or solid object
			if (bullets[y].position.x < 0 || bullets[y].position.y < 0 ||
				bullets[y].position.x > 193 || bullets[y].position.y > 200)
			{
				bullets[y].enable = false;
			}
		}
	}
}

//Handle bullet collision with enemy
void BulletCollision()
{
	//Define distances for each bullet
	int distance_x[100], distance_y[100];
	int distance_x2[100], distance_y2[100];
	int hit_flag = 0;

	//Detection with broad phase 
	for (int y = 0; y < 100; y++)
	{
		if (bullets[y].enable)
		{
			//Reference enemy
			for (int count = 0; count < 10; count++)
			{
				if (enemy[count].alive == 1)
				{
					//Player & enemy distance
					distance_x[y] = (bullets[y].position.x + (8 / 2)) - enemy[count].center.x;
					distance_y[y] = (bullets[y].position.y + (7 / 2)) - enemy[count].center.y;
					distance_x2[y] = (8 / 2) + (enemy[count].imageWidth / 2);
					distance_y2[y] = (7 / 2) + (enemy[count].imageHeight / 2);

					//Update to constant value
					if (distance_x[y] < 0)
						distance_x[y] *= -1;
					if (distance_y[y] < 0)
						distance_y[y] *= -1;

					//Collision detection
					if (distance_x[y] < distance_x2[y] && distance_y[y] < distance_y2[y])
					{
						hit_flag = 1;
						SoundPlay(sfx_id[SFXNUM::MONSTERHIT]);
					}
					else
					{
						hit_flag = 0;
						//MciStopSound(HitSound_id);
					}

					if (hit_flag == 1)
					{
						killCount++;
						enemy[count].alive = 0;
						bullets[y].enable = false;
					}
				}
			}
			
		}
	}
}

//Player sprite rendering with GUI HUD
void player_sprite(character* player, object* camera)
{                                    
	//Player sprite render
	for (int y = 0; y < 26; y++)
	{
		for (int x = 0; x < 16; x++)
		{
			//Character sprite facing front
			if (player->direction == FRONT)
			{
				if (Player_MOVINGFRONT[player->anim][y][x] != 0x00)
				{
					if (player->attacking)
					{	
						if (Player_ATTACKINGFRONT[y][x] != 0x00)
							frameBuffer[y + player->position.y][x + player->position.x] = Player_ATTACKINGFRONT[y][x];
					}
					else
						frameBuffer[y + player->position.y][x + player->position.x] = Player_MOVINGFRONT[player->anim][y][x];
				}
			}
			//Character sprite facing left
			else if (player->direction == LEFT)
			{
				if (Player_MOVINGSIDE[player->anim][y][x] != 0x00)
				{
					if (player->attacking)
					{
						if (Player_ATTACKINGSIDE[y][x] != 0x00)
							frameBuffer[y + player->position.y][16 - x + player->position.x] = Player_ATTACKINGSIDE[y][x];
					}
					else
						frameBuffer[y + player->position.y][x + player->position.x] = Player_MOVINGSIDE[player->anim][y][x];
				}
			}
			//Character sprite facing right
			else if (player->direction == RIGHT)
			{
				if (Player_MOVINGSIDE[player->anim][y][x] != 0x00)
				{
					if (player->attacking)
					{	
						if (Player_ATTACKINGSIDE[y][x] != 0x00)
							frameBuffer[y + player->position.y][x + player->position.x] = Player_ATTACKINGSIDE[y][x];
					}
					else
						frameBuffer[y + player->position.y][16 - x + player->position.x] = Player_MOVINGSIDE[player->anim][y][x];
				}
			}
			//Character sprite facing backward
			else if (player->direction == BACK)
			{
				if (Player_MOVINGBACK[player->anim][y][x] != 0x00)
				{
					frameBuffer[y + player->position.y][x + player->position.x] = Player_MOVINGBACK[player->anim][y][x];
				}
			}
		}
	}


	// -- GUI --
	//--------------------------------------------------------------------------
	//Health UI
	for (int y = 0; y < 11; y++)
	{
		for (int x = 0; x < 12; x++)
		{
			if (Health_UI[icon_anim][y][x] != 0x00)
				frameBuffer[y + 210][x + 10] = Health_UI[icon_anim][y][x];
		}
	}
	//CAN YOU SURVIVE Text
	char string[64]{ 0 };
	char string2[64]{ 0 };
	sprintf(&string[0], "WAVE %d", wave);
	if (wave < 7)
		strcpy(&string2[0], "EASY");
	else strcpy(&string2[0], "HARD");

	//ASCII Render
	for (int y = 0; y < 8; y++)
	{
		for (int x = 0; x < 8; x++)
		{
			//x text
			if (ascii[72][y][x] != 0x00)
				frameBuffer[y + 213][x + 25] = ascii[72][y][x];
			//Player HP
			for (int z = 0; z < 6; z++)
			{
				if (ascii[z][y][z] != 0x00)
					frameBuffer[y + 213][x + 36] = ascii[player->HP][y][x];
			}

			//Survive text
			for (int i = 0; string[i] != NULL; i++)
			{
				//Ignoring sentences with space
				if (string[i] == ' ' || string2[i] == ' ')
					continue;

				//To fix C6011 Deferencing NULL ptr
				if (ascii != nullptr && *ascii)
				{
					//-- 212 is the y position  & 82 is the x position 
					if (*(*(*ascii) + ((string[i] - 0x30) * 8 * 8) + (y * 8) + x) != 0x00)
					{
						*((*frameBuffer) + (((y + 212) % SCREEN_HEIGHT) * SCREEN_WIDTH) + (x + 82 + i * 7) % SCREEN_WIDTH) =
							*(*(*ascii) + ((string[i] - 0x30) * 8 * 8) + (y * 8) + x);
					}

					//-- 212 is the y position  & 152 is the x position 
					if (*(*(*ascii) + ((string2[i] - 0x30) * 8 * 8) + (y * 8) + x) != 0x00)
					{
						*((*frameBuffer) + (((y + 212) % SCREEN_HEIGHT) * SCREEN_WIDTH) + (x + 152 + i * 7) % SCREEN_WIDTH) =
							*(*(*ascii) + ((string2[i] - 0x30) * 8 * 8) + (y * 8) + x);
					}
				}
				else break;
			}
		}
	}
}

//Map sprite rendering
void map_sprite()
{
	//Load Map[20][20] to Map_Render[20 * 32][20 * 32]
	//char Map_Render[10 * 32][10 * 32];
	//[x * 20 + l]
	//char* Map_Render = new char[10 * 32]{ 0 };
	auto Map_Render = new char[10 * 32][10 * 32]{ 0 }; //Use this to fix C6262 warning using heap memory

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
						if (Dirt[k][l] != 0x00)
							//Map_Render[y * 20 + k][x * 20 + l] = Dirt[k][l];
							Map_Render[y * 20 + k][x * 20 + l] = 0x0D;
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

	//Clear heap memory
	delete[] Map_Render;
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

//Tutorial render
void tutorial_render()
{
	//creating effect for press enter text
	posCount++;

	//Clamp animation
	if (posCount > 200)
		posCount = 0;

	//Background
	for (int y = 0; y < SCREEN_HEIGHT; y++)
	{
		for (int x = 0; x < SCREEN_WIDTH; x++)
		{
			frameBuffer[y][x] = 0x05;
		}
	}

	//How to play text
	for (int y = 0; y < 14; y++)
	{
		for (int x = 0; x < 144; x++)
		{
			if (HowToPlay[y][x] != 0x05)
				frameBuffer[y + 40][x + 31] = HowToPlay[y][x];
		}
	}

	//Keymapping
	for (int y = 0; y < 30; y++)
	{
		for (int x = 0; x < 72; x++)
		{
			frameBuffer[y + 90][x + 63] = KeyMap[y][x];
		}
	}

	//Keymapping border
	for (int y = 0; y < 34; y++)
	{
		for (int x = 0; x < 76; x++)
		{
			if (KeyMapBorder[y][x] != 0x05)
				frameBuffer[y + 88][x + 61] = KeyMapBorder[y][x];
		}
	}

	//Press enter text
	for (int y = 0; y < 26; y++)
	{
		for (int x = 0; x < 128; x++)
		{
			if (PressEnter[y][x] != 0x05)
				frameBuffer[y + 150][x + posCount] = PressEnter[y][x];
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

	//DEBUG Title Sprite
	for (int y = 0; y < 33; y++)
	{
		for (int x = 0; x < 128; x++)
		{
			if (Title_UI[y][x] != 0x00)
				frameBuffer[y + TitleT.y][x + TitleT.x] = Title_UI[y][x];
		}
	}

	//Start text
	for (int y = 0; y < 16; y++)
	{
		for (int x = 0; x < 64; x++)
		{
			if (Start_UI[y][x] != 0x00)
				frameBuffer[y + StartT.y][x + StartT.x] = Start_UI[y][x];
		}
	}


	//Cursor sprite
	for (int y = 0; y < 12; y++)
	{
		for (int x = 0; x < 10; x++)
		{
			if (selectCursor == 1)
			{
				if (Cursor_UI[y][x] != 0x00)
				{
					frameBuffer[y + QuitT.y + 2][x + 70] = Cursor_UI[y][x];
				}
			}
			else
			{
				if (Cursor_UI[y][x] != 0x00)
				{
					frameBuffer[y + StartT.y + 1][x + 55] = Cursor_UI[y][x];
				}
			}
		}
	}

	//Quit Text
	for (int y = 0; y < 16; y++)
	{
		for (int x = 0; x < 36; x++)
		{
			//Quit text
			if (Quit_UI[y][x] != 0x00)
				frameBuffer[y + QuitT.y][x + QuitT.x] = Quit_UI[y][x];
		}
	}

	//Copyright logo
	for (int y = 0; y < 16; y++)
	{
		for (int x = 0; x < 16; x++)
		{
			if (Copyright[y][x] != 0x00)
				frameBuffer[y + 212][x + 182] = Copyright[y][x];
		}
	}

	//Player logo
	for (int y = 0; y < 26; y++)
	{
		for (int x = 0; x < 16; x++)
		{
			if (Player_MOVINGSIDE[0][y][x] != 0x00)
				frameBuffer[y + 10][16 - x + 10] = Player_MOVINGSIDE[0][y][x];
		}
	}

	//Enemy logo
	for (int y = 0; y < 14; y++)
	{
		for (int x = 0; x < 11; x++)
		{
			if (Enemy_MOVING[0][y][x] != 0x00)
			{
				frameBuffer[y + 15][11 - x + 175] = Enemy_MOVING[0][y][x];
				frameBuffer[y + 18][11 - x + 163] = Enemy_MOVING[0][y][x];
			}
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

	//GameOver text
	for (int y = 0; y < 24; y++)
	{
		for (int x = 0; x < 150; x++)
		{
			if (gameOver_UI[y][x] != 0x00)
				frameBuffer[y + GameOverT.y][x + GameOverT.x] = gameOver_UI[y][x];
		}
	}

	//Restart text
	for (int y = 0; y < 16; y++)
	{
		for (int x = 0; x < 70; x++)
		{
			if (Restart_UI[y][x] != 0x00)
				frameBuffer[y + RestartT.y][x + RestartT.x] = Restart_UI[y][x];
		}
	}

	//Cursor sprite
	for (int y = 0; y < 12; y++)
	{
		for (int x = 0; x < 10; x++)
		{
			if (selectCursor == 1)
			{
				if (Cursor_UI[y][x] != 0x00)
				{
					frameBuffer[y + QuitT.y + 2][x + 70] = Cursor_UI[y][x];
				}
			}
			else
			{
				if (Cursor_UI[y][x] != 0x00)
				{
					frameBuffer[y + RestartT.y + 1][x + 55] = Cursor_UI[y][x];
				}
			}
		}
	}

	//Quit Text
	for (int y = 0; y < 16; y++)
	{
		for (int x = 0; x < 36; x++)
		{
			//Quit text
			if (Quit_UI[y][x] != 0x00)
				frameBuffer[y + QuitT.y][x + QuitT.x] = Quit_UI[y][x];
		}
	}
}

//Result render
void Result_render()
{
	//Background
	for (int y = 0; y < SCREEN_HEIGHT; y++)
	{
		for (int x = 0; x < SCREEN_WIDTH; x++)
		{
			frameBuffer[y][x] = 0x07;
		}
	}

	//Victory text
	for (int y = 0; y < 33; y++)
	{
		for (int x = 0; x < 128; x++)
		{
			if (Victory_Text[y][x] != 0x00)
				frameBuffer[y + VictoryT.y][x + VictoryT.x] = Victory_Text[y][x];
		}
	}

	//Restart text
	for (int y = 0; y < 16; y++)
	{
		for (int x = 0; x < 70; x++)
		{
			if (Restart_UI[y][x] != 0x00)
				frameBuffer[y + RestartT.y][x + RestartT.x] = Restart_UI[y][x];
		}
	}

	//Cursor sprite
	for (int y = 0; y < 12; y++)
	{
		for (int x = 0; x < 10; x++)
		{
			if (selectCursor == 1)
			{
				if (Cursor_UI[y][x] != 0x00)
				{
					frameBuffer[y + QuitT.y + 2][x + 70] = Cursor_UI[y][x];
				}
			}
			else
			{
				if (Cursor_UI[y][x] != 0x00)
				{
					frameBuffer[y + RestartT.y + 1][x + 55] = Cursor_UI[y][x];
				}
			}
		}
	}

	//Quit Text
	for (int y = 0; y < 16; y++)
	{
		for (int x = 0; x < 36; x++)
		{
			//Quit text
			if (Quit_UI[y][x] != 0x00)
				frameBuffer[y + QuitT.y][x + QuitT.x] = Quit_UI[y][x];
		}
	}
}

//AI script logic
void AI_Behaviour(chara* player, chara* enemy)
{
	if (enemy->position.x <= player->position.x)
	{
		enemy->direction = RIGHT;
		enemy->position.x += 1;
	}
	else
	{
		enemy->direction = LEFT;
		enemy->position.x -= 1;
	}
	if (enemy->position.y <= player->center.y)
	{
		enemy->position.y += 1;
	}
	else
	{
		enemy->position.y -= 1;
	}
}

//bullet sprite render
void bullet_sprite(chara* player, object* bullets)
{
	for (int y = 0; y < 7; y++)
	{
		for (int x = 0; x < 8; x++)
		{
			if (bullets->enable)
			{
				if (Bullet[y][x] != 0x00)
				{
					frameBuffer[y + bullets->position.y][x + bullets->position.x] = Bullet[y][x];
				}
			}
		}
	}
}

#ifdef _DEBUG
//Display FPS 
void DisplayFPS()
{
	char fps[10];
	sprintf_s(fps, "FPS: %d", FPSCount);

	SetCaption(fps);
}
#endif // _DEBUG







//------------------------------------------------------------------------------------------------------------
//-- UNUSED  FUNCTIONS --
//Decided to keep it for future reference
//------------------------------------------------------------------------------------------------------------
//Collision handler 
bool collision(character* player, character* enemy, object* obj)
{
	//Collision detection for obstacles -- CURRENTLY NOT WORKING --
	if (Map2[player->position.y / 14][player->position.x / 16] == 2 ||
		Map2[player->position.y / 23][player->position.x / 13] == 0)
	{
		player->collided = 1;
	}
	else
	{
		player->collided = 0;
	}

	//Enemy collision with bullet -- CURRENTLY NOT WORKING --
	if (bullets->position.x + bullets->move.x == enemy->position.x
		|| bullets->position.y + bullets->move.y == enemy->position.y)
	{
		enemy->alive = 0;
		killCount++;
	}

	return false;
}

//Timer count
int CountTime()
{
	//Timer
	int seconds = 0;
	int minutes = 0;
	int hours = 0;

	while (true)
	{
		Sleep(1000);
		seconds++;

		if (seconds == 60)
		{
			seconds = 0;
			minutes++;
		}

		if (minutes == 60)
		{
			minutes = 0;
			hours++;
		}
	}

	return minutes;
}

//Free memory
void FreeMemory()
{
	//TODO
}

//end of file ---
/////////////////

