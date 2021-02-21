/**
 * @file	conioex.h
 * @brief	Win32 コンソール I/O 拡張
 *
 * @date	2020/08/15 2020年度初版
 * @version	2.00 パレット変更機能などの追加
 * @note
 *  コンソールウィンドウのプロパティの設定を以下に変更すること
 *  ・「従来のコンソールを使う」のチェックを外す
 *  ・「簡易編集モード」のチェックを外す
 *	ver1.09以降変更は INFO_EX の定義で有効になる
 */

#ifndef __CONIOEX_H
#define __CONIOEX_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <process.h>
#include <windows.h>
#include <tchar.h>
#include <mmsystem.h>
#include <digitalv.h>
#include <xinput.h>
#include <time.h>
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "xinput.lib")
#pragma warning(disable:4996)

#define	INFO_EX	//この定義を有効にするとScreenBufferInfoExを使う。

#define NOCURSOR		0
#define SOLIDCURSOR		1
#define NORMALCURSOR	2

#define DEF_FONTSIZE_X	1
#define DEF_FONTSIZE_Y	1

	// https://en.wikipedia.org/wiki/Color_Graphics_Adapter
	enum COLORS {
		BLACK,			/* #000000	黒				*/
		BLUE,			/* #0000AA	青				*/
		GREEN,			/* #00AA00	緑				*/
		CYAN,			/* #00AAAA	シアン			*/
		RED,			/* #AA0000	赤				*/
		MAGENTA,		/* #AA00AA	マゼンタ		*/
		BROWN,			/* #AA5500	茶				*/
		LIGHTGRAY,		/* #AAAAAA	明るい灰色		*/
		DARKGRAY,		/* #555555	暗い灰色		*/
		LIGHTBLUE,		/* #5555FF	明るい青		*/
		LIGHTGREEN,		/* #55FF55	明るい緑		*/
		LIGHTCYAN,		/* #55FFFF	明るいシアン	*/
		LIGHTRED,		/* #FF5555	明るい赤		*/
		LIGHTMAGENTA,	/* #FF55FF	明るいマゼンタ	*/
		YELLOW,			/* #FFFF55	黄				*/
		WHITE			/* #FFFFFF	白				*/
	};

	// キーボード用
#define PK_ENTER				VK_RETURN
#define PK_ESC					VK_ESCAPE
#define PK_F1					VK_F1
#define PK_F2					VK_F2
#define PK_F3					VK_F3
#define PK_F4					VK_F4
#define PK_F5					VK_F5
#define PK_F6					VK_F6
#define PK_F7					VK_F7
#define PK_F8					VK_F8
#define PK_F9					VK_F9
#define PK_F10					VK_F10
#define PK_F11					VK_F11
#define PK_F12					VK_F12
#define PK_SP					VK_SPACE
#define PK_UP					VK_UP
#define PK_DOWN					VK_DOWN
#define PK_RIGHT				VK_RIGHT
#define PK_LEFT					VK_LEFT
#define PK_SHIFT				VK_SHIFT
#define PK_CTRL					VK_CONTROL
#define PK_ALT					VK_MENU
#define PK_BS					VK_BACK
#define PK_PAUSE				VK_PAUSE
#define PK_INS					VK_INSERT
#define PK_DEL					VK_DELETE
#define PK_TAB					VK_TAB
#define PK_NFER					VK_KANA		/* [無変換]	*/
#define PK_XFER					VK_CONVERT	/* [変換]	*/
#define PK_0					0x30
#define PK_1					0x31
#define PK_2					0x32
#define PK_3					0x33
#define PK_4					0x34
#define PK_5					0x35
#define PK_6					0x36
#define PK_7					0x37
#define PK_8					0x38
#define PK_9					0x39
#define PK_NUMPAD0				VK_NUMPAD0	/* テンキーの[0]	*/
#define PK_NUMPAD1				VK_NUMPAD1	/* テンキーの[1]	*/
#define PK_NUMPAD2				VK_NUMPAD2	/* テンキーの[2]	*/
#define PK_NUMPAD3				VK_NUMPAD3	/* テンキーの[3]	*/
#define PK_NUMPAD4				VK_NUMPAD4	/* テンキーの[4]	*/
#define PK_NUMPAD5				VK_NUMPAD5	/* テンキーの[5]	*/
#define PK_NUMPAD6				VK_NUMPAD6	/* テンキーの[6]	*/
#define PK_NUMPAD7				VK_NUMPAD7	/* テンキーの[7]	*/
#define PK_NUMPAD8				VK_NUMPAD8	/* テンキーの[8]	*/
#define PK_NUMPAD9				VK_NUMPAD9	/* テンキーの[9]	*/
#define PK_A					0x41
#define PK_B					0x42
#define PK_C					0x43
#define PK_D					0x44
#define PK_E					0x45
#define PK_F					0x46
#define PK_G					0x47
#define PK_H					0x48
#define PK_I					0x49
#define PK_J					0x4A
#define PK_K					0x4B
#define PK_L					0x4C
#define PK_M					0x4D
#define PK_N					0x4E
#define PK_O					0x4F
#define PK_P					0x50
#define PK_Q					0x51
#define PK_R					0x52
#define PK_S					0x53
#define PK_T					0x54
#define PK_U					0x55
#define PK_V					0x56
#define PK_W					0x57
#define PK_X					0x58
#define PK_Y					0x59
#define PK_Z					0x5A
#define PK_LT					0x0BC		/* [,]	*/
#define PK_GT					0x0BE		/* [.]	*/
#define PK_SLUSH				0x0BF		/* [?]	*/
#define PK_DOT					VK_DECIMAL	/* テンキーの[.]	*/
#define PK_DIV					VK_DIVIDE	/* テンキーの[/]	*/
#define PK_BSLUSH				0x0E2		/* [_]	*/
#define PK_SEMICOLON			0x0BB		/* [;]	*/
#define PK_ADD					VK_ADD		/* テンキーの[+]	*/
#define PK_COLON				0x0BA		/* [:]	*/
#define PK_MUL					VK_MULTIPLY	/* テンキーの[*]	*/
#define PK_RBRACE				0x0DD		/* []]	*/
#define PK_ATMARK				0x0C0		/* [@]	*/
#define PK_LBRACE				0x0DB		/* [[]	*/
#define PK_MINUS				0x0BD		/* [-]	*/
#define PK_SUB					VK_SUBTRACT	/* テンキーの[-]	*/
#define PK_XOR					0x0DE		/* [^]	*/
#define PK_YEN					0x0DC		/* [\]	*/
#define PK_KANJI				0x0F3		/* [半角/全角]	*/
#define PK_CAPS					0x0F0		/* [英数][ひらがな]	*/

// マウス用
#define PM_LEFT					VK_LBUTTON
#define PM_MID					VK_MBUTTON
#define PM_RIGHT				VK_RBUTTON
#define PM_CURX					0x0101
#define PM_CURY					0x0102

// DirectInput用
#define PJ1_XPOS				0x0200
#define PJ1_YPOS				0x0201
#define PJ1_ZPOS				0x0202
#define PJ1_BTNS				0x0203
#define PJ2_XPOS				0x0210
#define PJ2_YPOS				0x0211
#define PJ2_ZPOS				0x0212
#define PJ2_BTNS				0x0213
#define PJ3_XPOS				0x0220
#define PJ3_YPOS				0x0221
#define PJ3_ZPOS				0x0222
#define PJ3_BTNS				0x0223
#define PJ4_XPOS				0x0230
#define PJ4_YPOS				0x0231
#define PJ4_ZPOS				0x0232
#define PJ4_BTNS				0x0233
#define PJ_XPOS					PJ1_XPOS
#define PJ_YPOS					PJ1_YPOS
#define PJ_ZPOS					PJ1_ZPOS
#define PJ_BTNS					PJ1_BTNS

// DirectInput拡張用
#define PJEX_XPOS				0x0001
#define PJEX_YPOS				0x0002
#define PJEX_ZPOS				0x0003
#define PJEX_RPOS				0x0004
#define PJEX_UPOS				0x0005
#define PJEX_VPOS				0x0006
#define PJEX_BTNS				0x0007
#define PJEX_BTNNO				0x0008
#define PJEX_POV				0x000A
#define PJEX_SIZE				0x000B
#define PJEX_FLAGS				0x000C

// XInput用
#define PJX1_LXPOS				0x0200
#define PJX1_LYPOS				0x0201
#define PJX1_LTRG				0x0202
#define PJX1_RXPOS				0x0203
#define PJX1_RYPOS				0x0204
#define PJX1_RTRG				0x0205
#define PJX1_BTNS				0x0206
#define PJX2_LXPOS				0x0210
#define PJX2_LYPOS				0x0211
#define PJX2_LTRG				0x0212
#define PJX2_RXPOS				0x0213
#define PJX2_RYPOS				0x0214
#define PJX2_RTRG				0x0215
#define PJX2_BTNS				0x0216
#define PJX3_LXPOS				0x0220
#define PJX3_LYPOS				0x0221
#define PJX3_LTRG				0x0222
#define PJX3_RXPOS				0x0223
#define PJX3_RYPOS				0x0224
#define PJX3_RTRG				0x0225
#define PJX3_BTNS				0x0226
#define PJX4_LXPOS				0x0230
#define PJX4_LYPOS				0x0231
#define PJX4_LTRG				0x0232
#define PJX4_RXPOS				0x0233
#define PJX4_RYPOS				0x0234
#define PJX4_RTRG				0x0235
#define PJX4_BTNS				0x0236
#define PJX_LXPOS				PJX1_LXPOS
#define PJX_LYPOS				PJX1_LYPOS
#define PJX_LTRG				PJX1_LTRG
#define PJX_RXPOS				PJX1_RXPOS
#define PJX_RYPOS				PJX1_RYPOS
#define PJX_RTRG				PJX1_RTRG
#define PJX_BTNS				PJX1_BTNS

// 初期化
	void InitConio(int width, int height);			// 初期化
	int InitDoubleBuffer(void);						// ダブルバッファ初期化
	void SetScreenFontSize(int width, int height);	// フォントサイズ設定

	// 描画
	int GetCursorX(void);							// 水平方向のカーソル位置を取得
	int GetCursorY(void);							// 垂直方向のカーソル位置を取得
	void SetCursorPosition(int x, int y);			// カーソル位置の移動
	void PrintString(const char* buf, int size);	// 文字列の出力
	void PrintImage(unsigned char* buf);			// 画像の出力
	void ClearLine(void);							// 行末まで消去
	void ClearScreen(void);							// 画面消去
	void FlipScreen(void);							// ダブルバッファ時の描画面切替
	void SetHighVideoColor(void);					// 文字色高輝度化
	void SetLowVideoColor(void);					// 文字色低輝度化
	void SetNormalVideoColor(void);					// 既定文字色設定
	void SetTextBackColor(int color);				// 文字背景色設定
	void SetConsoleTextColor(int color);			// 文字色設定
	void SetTextAttribute(int attribute);			// 文字色背景色同時設定
	void SetCursorType(int type);					// カーソルタイプ設定
	void InsertLine(void);							// 現在行に挿入
	void DeleteLine(void);							// 現在行の削除
	void PrintFrameBuffer(char* buf);				// 文字列の一括転送
	void ClearFrameBuffer(char* buf);				// 一括転送用バッファクリア

	// ウィンドウ
	void SetCaption(const char* title);					// コンソールウィンドウのタイトルバーに表示されるテキストを設定
	int GetCaption(char* title, int len);			// コンソールウィンドウのタイトルバーに表示されるテキストを取得

	// 入力系
	void ResetKeyMap(void);							// キー情報リセット
	int InputJoystick(int port);					// ゲームパッド入力(DirectInput対応)
	int InputJoystickEx(int id, int port);			// ゲームパッド詳細入力(DirectInput対応)
	int InputJoystickX(int id, int port);			// ゲームパッド入力(XInput対応)
	int InputKeyMouse(int port);					// キーボード・マウス入力

	// 音声出力
	int* MciOpenSound(const char* path);			// サウンドファイルを開く
	void MciCloseSound(int* sound_id);				// サウンドファイルを閉じる
	void MciPlaySound(int* sound_id, int repeat);	// サウンドを再生する
	void MciStopSound(int* sound_id);				// サウンド再生を停止する
	int MciCheckSound(int* sound_id);				// サウンド再生状態の取得
	void MciUpdateSound(int* sound_id);				// ループ再生の強制更新
	void MciSetVolume(int* sound_id, int percent);	// 再生音量を設定する

#ifdef INFO_EX
#define	NUM_PALETTE	16	//パレット数
	//グローバル変数
	extern COLORREF	g_ConsoleColorTable[NUM_PALETTE];	//コンソール画面のパレット全色を入れておく為のバッファ
	//Window関連
	void FixWin(void);	//ウィンドウサイズを固定する
	//パレット関連
	void SetConsolePalette(void);	//パレットに１６色を設定する
	void GetConsolePalette(void);	//パレットから１６色を取得する
	void SetConsolePaletteRange(const COLORREF* p_cr16, unsigned int start, unsigned int count);	//
	void GetConsolePaletteRange(COLORREF* p_cr16, unsigned int start, unsigned int count);	//
	void CopyPalette_N(const RGBQUAD* sourcePAL, int start_num, int num_palette);	//パレット配列指定数分コピー：RGBQUAD型配列からConsoleColorTableへコピー
	void CopyPaletteQ2R_N(COLORREF* destPAL, const RGBQUAD* sourcePAL, int start_num, int num_palette);	//パレット配列指定数分コピー：RGBQUAD型配列からCOLORREF型配列へコピー
	void CopyPaletteR2Q_N(RGBQUAD* destPAL, const COLORREF* sourcePAL, int start_num, int num_palette);	//パレット配列指定数分コピー：COLORREF型配列からRGBQUAD型配列へコピー
	void CopyPalette(const RGBQUAD* sourcePAL);	//パレット配列全てコピー：RGBQUAD型配列からConsoleColorTable配列へコピー
	void CopyPaletteQ2R(COLORREF* destPAL, const RGBQUAD* sourcePAL);	//パレット配列全てコピー：RGBQUAD型配列からCOLORREF型配列へコピー
	void CopyPaletteR2Q(RGBQUAD* destPAL, const COLORREF* sourcePAL);	//パレット配列全てコピー：COLORREF型配列からRGBQUAD型配列へコピー
	//バッファ転送関連
	BOOL DrawImageRect(const CHAR_INFO* p_srcBuff, const COORD srcBuffSize, const COORD srcPos, SMALL_RECT destRect);
	void PrintFrameBufferAttribute(const char* buf);	//文字色だけのバッファから文字属性部分にだけ転送する
	void ClearFrameBufferFull(char* buf);				// 一括転送用バッファクリア
#endif // INFO_EX

#ifdef __cplusplus
}
#endif

#endif /* __CONIOEX_H */

/**
 * @copyright (c) 2018-2020 HAL Osaka College of Technology & Design (Ihara, H.)
 */
