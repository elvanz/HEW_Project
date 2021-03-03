/**
* @file	Data.h
*
* Contains game data archive
*/
#pragma once

#ifndef __DATA_H
#define __DATA_H

//Include
#include "GraphicDataManager.h"
#include "conioex2.h"
#include "bmp_utl2.h"

//Preprocessors
#define SCREEN_WIDTH  200		
#define SCREEN_HEIGHT 230
#define VIEWPORT_W	  200
#define VIEWPORT_H    200
#define FONT_WIDTH	   10
#define FONT_HEIGHT	   10
#define MAX_PIXELH	   64	//max rendered pixel height
#define MAX_PIXELW	   64	//max rendered pixel width
#define MAX_ENEMY	   50
#define MAX_ANIM	   10
#define NUM_KEY		  256


//--------------------------------------------------------------------
/* Game Data */
enum ability
{
	loot,
	useItem
};

enum stages
{
	startmenu,
	stage01,
	tutorial,
	victory,
	defeat
};

enum animations
{
	IDLE,
	WALK,
	RUN,
	COMBAT
};

enum directions
{
	LEFT,		//MOVING LEFTSIDE
	RIGHT,		//MOVING RIGHTSIDE
	FRONT,		//MOVING FORWARD
	BACK		//MOVING BACKWARD
};

//2D Vertices Data
typedef struct Point
{
	int			x;
	int			y;
}Point;

//Object data
typedef struct object
{
	Point		prevPos, position, move;			//spawn coordinates
	bool		interactable, actor, enable;		//object behaviour
	int			imageWidth, imageHeight;
	char		sprite[MAX_PIXELH][MAX_PIXELW];		//sprite buffer
	const char	*path;
	object		*prev, *next;						//pointer to the next node(if needed)
}object;

//Character data 
typedef struct character
{
	Point		prevPos, position, center;
	bool		moving, hasLooted, attacking;		//check if moving or not
	int			ability, collided;					//ability
	int			level, exp, HP, baseDamage;			//player stats
	int			anim, alive, direction;				//condition check
	int			imageWidth, imageHeight;			//Bitmap data height & width attributes
	char		sprite[MAX_PIXELH][MAX_PIXELW];		//sprite direction(reference to enum directions)
	const char	*path;
}chara;


//Text data
typedef struct
{
	Point		position;
	const char	*caption;
}Texts;

//8bit frame buffer for bitmap
typedef struct FrameBuffer
{
	COORD		whSize;			//frameBuffer width&height size attributes
	int			byteSize;							//frameBuffer byteSize
	char		pPix[SCREEN_HEIGHT][SCREEN_WIDTH];	//pointer to frameBuffer
}FrameBuffer;

typedef struct
{
	PFRAMEBUFFER	pFB0;
	PFRAMEBUFFER	pFB1;
	PFRAMEBUFFER	pFB2;
	char* frameBuffer;
}Image, *pImage;



//------------------------------------------------------------------------------------------------


#endif __DATA_H

