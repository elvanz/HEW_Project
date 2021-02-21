//################################################################
//	指定のフォントを使ってそのフォントで生成された文字列イメージをビットマップ画像データに変換する。
//	各文字はビットマップ画像に変換されBmpChar構造体に格納される。
//	BmpChar_Create関数を呼び出すと、BmpChar構造体が指定された文字数分BmpCharの配列として生成され、そのポインタが返される。
//
//	2021/01/18 : N.Ohmoto
//################################################################
#include "conioex.h"
#include "BmpChar.h"

void ConvBpp1ToB(BmpChar* _pbc, GLYPHMETRICS* _pgm);
void DrawBmpChar(BmpChar* _pbc, GLYPHMETRICS* _pgm, TEXTMETRIC* _ptxm);

//================================================================
//	1bppの画像を8bppの画像に変換する。
//arg:
//	BmpChar* _pbc : ビットマップ文字データへのポインタ
//	GLYPHMETRICS* _pgm : 変換元文字のグリフ情報
//return:
//	なし
//================================================================
void ConvBpp1ToB(BmpChar* _pbc, GLYPHMETRICS* _pgm)
{
	int w_pix = _pgm->gmBlackBoxX;
	int h_pix = _pgm->gmBlackBoxY;
	int stride = (_pbc->buf_size / _pgm->gmBlackBoxY);
	int stride4 = (w_pix + 0b0011)& (~0b0011);			//８bpp画像の４バイト境界のバイト数
	int bits_size = stride4 * h_pix;
	char* pFontBitmap = (char*)malloc(bits_size);
	ZeroMemory(pFontBitmap, bits_size);
	for (int y = 0; y < h_pix; y++)
	{
		for (int x = 0; x < stride; x++)
		{
			int idxSrc = (y * stride + x);
			UINT bit8 = _pbc->pPix[idxSrc];
			int idxDest = (y * stride4) + (x * 8);
			for (int bitN = 0; bitN < 8; bitN++)
			{
				if ((idxDest + bitN) < bits_size)
				{
					//pFontBitmap[idxDest + bitN] = (bit8 & (0b10000000 >> bitN)) ? 1 : 0;	//0xFF : 0x00;
					if ((bit8 & (0b10000000 >> bitN)) != 0) {
						pFontBitmap[idxDest + bitN] = 1;
					}
				}
			}
		}
	}
	//古い1bppのバッファは削除して新しく作った8bppバッファに入れ替える。
	free(_pbc->pPix);
	_pbc->pPix = pFontBitmap;
	_pbc->buf_size = bits_size;
}	//ConvBpp1ToB

//================================================================
//	ビットマップ文字１文字の表示位置を調整してビットマップを作り直す。
//arg:
//	BitmapChar* _pbc : ビットマップ文字のポインタ。このポインタが指すビットマップ文字データの表示位置を調整してバッファが作り直される。
//	GLYPHMETRICS* _pgm : 変換元文字のグリフ情報
//	TEXTMETRIC* _ptxm : 変換元フォントの計測（文字の寸法）情報
//return:
//	無し
//================================================================
void DrawBmpChar(BmpChar* _pbc, GLYPHMETRICS* _pgm, TEXTMETRIC* _ptxm)
{
	int	dest_width = _pgm->gmCellIncX;
	int dest_height = _ptxm->tmHeight;
	int dest_buf_size = dest_width * dest_height;
	char* pDest = (char*)malloc(dest_buf_size);
	ZeroMemory(pDest, dest_buf_size);
	int width = _pgm->gmBlackBoxX;
	int widthBytes = (width + 0b0011)& (~0b0011);	//横幅のバイト数は４の倍数に合わせる
	int height = _pgm->gmBlackBoxY;
	//
	int pn = 0;
	int stride = _pbc->buf_size / _pbc->size.Y;
	//
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			int xp = _pgm->gmptGlyphOrigin.x + x;
			int yp = (_ptxm->tmAscent - _pgm->gmptGlyphOrigin.y) + y;
			if (yp < 0)
			{
				continue;
			}
			if ((xp >= 0) && (xp < dest_width) && (yp >= 0) && (yp < dest_height))
			{
				int read_idx = (y * widthBytes + x);
				if ((read_idx >= 0) && ((int)_pbc->buf_size > read_idx))
				{
					unsigned char dot = _pbc->pPix[y * stride + x];
					if (dot != 0x00)
					{
						if (_pbc->aaLv == 2)
						{
							dot = 0x0F;
						}
						else
						{
							dot = (unsigned char)((double)(16.0 / (double)(_pbc->aaLv - 1)) *  (double)dot);
						}
						pDest[yp * dest_width + xp] = dot;
					}
				}
			}
		}
	}
	free(_pbc->pPix);
	_pbc->pPix = pDest;
	_pbc->buf_size = dest_buf_size;
	_pbc->size.X = dest_width;
	_pbc->size.Y = dest_height;
	return;
}	//DrawBmpChar

//================================================================
//	指定のフォントで出来たビットマップ文字列を作成し、そのポインタを返す。
//args:
//	const char* _font_name : フォント名
//	int _font_size : フォント・サイズ
//	int _bold : 太字指定：trueで太字
//	int _ggo : アンチ・エイリアスの諧調指定
//	const wchar_t* _wtext : 変換したい文字列（ワイド文字）
//return:
//	BitmapChar* : 変換後のビットマップ文字の配列へのポインタ。最後の文字は
//================================================================
BmpChar* BmpChar_Create(const char* _font_name, int _font_size, int _bold, int _ggo, const wchar_t* _wtext)
{
	//回転行列
	MAT2	mat2{ {0,1},{0,0},{0,0},{0,1} };
	//フォントの設定～作成
	LOGFONT	lf;
	lf.lfHeight = _font_size;
	lf.lfWidth = 0;
	lf.lfEscapement = 0;	//文字単位の回転角度左回り
	lf.lfOrientation = 0;
	if (_bold)
	{
		lf.lfWeight = FW_BOLD;	//太字設定
	}
	else
	{
		lf.lfWeight = FW_NORMAL;
	}
	lf.lfItalic = FALSE;	//斜体
	lf.lfUnderline = FALSE;
	lf.lfStrikeOut = FALSE;
	lf.lfCharSet = ANSI_CHARSET;
	lf.lfOutPrecision = OUT_TT_PRECIS;	//OUT_DEFAULT_PRECIS
	lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lf.lfQuality = PROOF_QUALITY;
	lf.lfPitchAndFamily = (DEFAULT_PITCH | FF_MODERN);
	lf.lfFaceName[0] = '\0';
	//書体名をコピー（文字数制限あり）
	CopyMemory(lf.lfFaceName, _font_name, LF_FACESIZE * sizeof(CHAR));
	//フォント生成
	HFONT hFont = CreateFontIndirect(&lf);
	if (hFont == NULL)
	{
		return	NULL;
	}
	// デバイスにフォントを選択する
	HWND hWnd = GetConsoleWindow();
	HDC hdc = GetDC(hWnd);
	HFONT oldFont = (HFONT)SelectObject(hdc, hFont);
	//生成したフォントの計測データを取得する
	TEXTMETRIC	txm;		//変換したフォントの情報を入れる構造体
	GetTextMetrics(hdc, &txm);	//計測データを取得
	int aa_level = 2;
	switch (_ggo)
	{
	default:	//２値
	case GGO_BITMAP:		aa_level = 2;		break;	//２値
	case GGO_GRAY2_BITMAP:	aa_level = 5;		break;	//５階調
	case GGO_GRAY4_BITMAP:	aa_level = 17;	break;	//１７階調
	case GGO_GRAY8_BITMAP:	aa_level = 65;	break;	//６５階調
	}
	//指定のフォントで出来たビットマップ文字で文字列を作成する。
	GLYPHMETRICS	gm;	//グリフ設定データ
	UINT code;
	int length = 0;
	//文字列の文字数を求める。
	while (_wtext[length] != '\0') {
		length++;
	}
	BmpChar*	pBmpChr = (BmpChar*)malloc(sizeof(BmpChar) * (length + 1));
	ZeroMemory(pBmpChr, sizeof(BmpChar) * (length + 1));
	for (int txn = 0; txn < length; txn++) {
		code = (UINT)_wtext[txn];
		//これから生成する文字ビットマップデータのバイト数を取得する。
		int buff_size = GetGlyphOutlineW(hdc, code, _ggo, &gm, 0, NULL, &mat2);
		if (buff_size > 0)
		{
			//取得したサイズ分のバッファを確保する。
			pBmpChr[txn].pPix = (char*)malloc(buff_size);
			GetGlyphOutlineW(hdc, code, _ggo, &gm, buff_size, pBmpChr[txn].pPix, &mat2);
			if (_ggo == GGO_BITMAP)
			{
				//1bppのビットマップは表示しにくいので８bppに変換する。
				pBmpChr[txn].buf_size = buff_size;		//バッファサイズ
				ConvBpp1ToB(&pBmpChr[txn], &gm);
				buff_size = pBmpChr[txn].buf_size;
			}
			pBmpChr[txn].size.X = gm.gmBlackBoxX;	//横ピクセル数
			pBmpChr[txn].size.Y = gm.gmBlackBoxY;	//縦ピクセル数
			pBmpChr[txn].buf_size = buff_size;		//バッファサイズ
			pBmpChr[txn].aaLv = aa_level;			//アンチエイリアスの諧調レベル
			pBmpChr[txn].wch = code;				//変換元の文字コード
			//文字位置を調整してバッファを作り直す。
			DrawBmpChar(&pBmpChr[txn], &gm, &txm);
		}
	}
	return	pBmpChr;
}	//BmpChar_Create

//
// end of file.
//
