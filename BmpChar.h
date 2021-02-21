#pragma once
//################################################################
//	指定のフォントを使ってそのフォントで生成された文字列イメージをビットマップ画像データに変換する。
//	各文字はビットマップ画像に変換されBmpChar構造体に格納される。
//	BmpChar_Create関数を呼び出すと、BmpChar構造体が指定された文字数分BmpCharの配列として生成され、そのポインタが返される。
//
//	2021/01/18 : N.Ohmoto
//################################################################

//ビットマップ文字１文字分のバッファ用構造体
typedef struct {
	char*	pPix;	//文字のビットマップデータへのポインタ
	COORD	size;	//文字の幅と高さ
	int buf_size;	//ビットマップデータのサイズ（バイト数）
	int aaLv;		//諧調数（アンチエイリアスレベル）
	wchar_t	wch;	//変換元の文字
} BmpChar;

BmpChar* BmpChar_Create(const char* _font_name, int _font_size, int _bold, int _ggo, const wchar_t* _wtext);

//
// end of file.
//
