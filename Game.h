#pragma once
/**
* @file		Game.h
* 
* Header file for game event handlers
*/

#ifndef __GAME_H
#define __GAME_H

/* Include data header file */
#include "Data.h"
#include "Renderer.h"
#include <math.h>
#include <time.h>

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

static int icon_anim;
//-------------------------------------------------------------
/* All functions goes here */
extern void InitSound(int *);
extern void	InitWindow();
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
extern void InitBullet(int , int, int, int);
extern void KeyRead();
extern void player_sprite(character* player, object* camera);
extern void enemy_sprite(chara* enemy);
extern void map_sprite();
extern void bullet_sprite(chara* player, object*);
extern void title_render();
extern void gameOver_render();
extern void Result_render();
extern bool collision(chara*, chara*, object*);
extern void AI_Behaviour(chara*, chara*);
extern void DisplayFPS();
extern int CountTime();

//---------------------------------------------------------------
//-- UNUSED --
extern void load_sprite(object*, const char* _path);
extern void load_image(PFRAMEBUFFER pFB, char *frameBuffer);
extern void SwapBGR(PFRAMEBUFFER pFB);
extern void SetWindowBMP(Image img, pImage pImg);
extern void FileLoad(pImage img);
extern void FileEnd(pImage img);
//-------------------------------------------------------------

#endif __GAME_H
