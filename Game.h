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

/* Assign struct data */
static character player;
static character NPC;
static character enemy[5];

static object gameCamera;
static object interactables[5];
static object bullets;
static object lootItems;
static object solidObject;
static object dynamicObject;

static Texts Title;
static Image imgFile;
static FrameBuffer pFB;
//------------------------------------------------------------
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
extern void InitBullet(object* );
extern void KeyRead();
extern void load_sprite(object*, const char* _path);
extern void player_sprite(character* player, object* camera);
extern void enemy_sprite(character* enemy);
extern void map_sprite();
extern void bullet_sprite(chara* player, object*);
extern void title_render();
extern void gameOver_render();
extern void Result_render();
extern void collision(chara*, chara*, object*);
extern void AI_Behaviour(chara*, chara*);
extern void load_image(PFRAMEBUFFER pFB, char *frameBuffer);
extern void SwapBGR(PFRAMEBUFFER pFB);
extern void SetWindowBMP(Image img, pImage pImg);
extern void FileLoad(pImage img);
extern void FileEnd(pImage img);
//-------------------------------------------------------------

#endif __GAME_H
