#pragma once
/**
* @file		Game.h
* 
* Header file for game event handlers
*/

#ifndef GAME_H
#define GAME_H

/* Include data header file */
#include "Data.h"
//#include "Renderer.h"
#include "Sound.h"
#include <math.h>
#include <time.h>
#pragma warning (disable:6386)
#pragma warning (disable:6385)

//Preprocessor
#define APP_EXIT	0

/* Assign struct data */
static character player;
static character NPC;
static character enemy[10];

static object gameCamera;
static object bullets[100];
static object lootItems;
static object solidObject;
static object dynamicObject;

static Texts Title;
static Image imgFile;
static FrameBuffer pFB;

extern int icon_anim;
extern float timer;
extern int frame;
extern int FPSCount;
extern int execLastTime;
extern int currentTime;
extern int fpsLastTime;
extern const int FPS;
extern const int frameDelay;

//-------------------------------------------------------------
/* All functions goes here */
extern void SoundPlay(int *bgm);
extern void	InitWindow();
extern void Tutorial();
extern void MainGame();
extern void Start();
extern void Input(chara *);
extern void Update(chara *, chara *, object *);
extern void Render(void (*func)());
extern void FreeMemory();
extern void StartMenu();
extern void Stage01();
extern void GameOver();
extern void Result();
extern void InitEnemy();
extern void UpdateEnemy(character *);
extern void InitBullet();
extern void SetBullet(int , int, int, int);
extern void UpdateBullet();
extern void BulletCollision();
extern void KeyRead();
extern void player_sprite(character* player, object* camera);
extern void enemy_sprite(chara* enemy);
extern void map_sprite();
extern void bullet_sprite(chara* player, object*);
extern void tutorial_render();
extern void title_render();
extern void gameOver_render();
extern void Result_render();
extern void AI_Behaviour(chara*, chara*);
extern void DisplayFPS();



//---------------------------------------------------------------
//-- UNUSED (SAVED FOR FUTURE REFERENCE) -- 
extern bool collision(chara*, chara*, object*);
extern int CountTime();
//-------------------------------------------------------------

#endif GAME_H
