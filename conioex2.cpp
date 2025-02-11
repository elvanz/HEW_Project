/**
 * @file	conioex.cpp
 * @brief	Win32 コンソール I/O 拡張
 *
 * @date	2020/09/18 2020年度初版
 * @date	2020/10/09 不具合修正
 * @version	1.08
 *			2020/09/24 シングルバッファ使用時にウィンドウサイズが
 *					   反映されない問題の再修正
 *			2020/10/09 バッファサイズが1多くなりすぎている問題の修正
 *			1.09
 *			2020/10/20	CONSOLE_SCREEN_BUFFER_INFOEXを使ってパレットデータを使えるように変更し
 *						パレット変換関数の追加した
 *			2020/10/20	ウィンドウサイズを固定する関数を追加。
 *			2.00
 *			2020/11/10	以前のバージョンと混在するとまずいのでファイル名を変更した
 *						→ conioex.h を conioex2.h に変更
 *						→ conioex.cpp を conioex2.cpp に変更
 * @note
 *	コンソールウィンドウのプロパティの設定を以下に変更すること
 *	・「従来のコンソールを使う」のチェックを外す
 *	・「簡易編集モード」のチェックを外す
 *	ver1.09以降変更は INFO_EX の定義で有効になる（conioex2.h参照）
 */
#ifdef __cplusplus
extern "C" {
#endif

#include "conioex2.h"

#define CONSOLE_INPUT_MODE	( ENABLE_EXTENDED_FLAGS | ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT | ENABLE_PROCESSED_INPUT )
#define CONSOLE_OUTPUT_MODE	(ENABLE_PROCESSED_OUTPUT | ENABLE_LVB_GRID_WORLDWIDE | ENABLE_VIRTUAL_TERMINAL_PROCESSING )

	typedef struct {
		int				device_type;
		MCIDEVICEID		device_id;
		char			path[MAX_PATH];
		int				repeat;
	} MciSoundInfo;

	HANDLE	display_handle[2];
	HANDLE	input_handle;
	DWORD	conio_key_map[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	COORD	conio_mouse_position = { 0, 0 };
	COORD	screen_buffer_size;
	WORD	screen_buffer_attribute = LIGHTGRAY;
#ifdef INFO_EX
	CONSOLE_SCREEN_BUFFER_INFOEX screen_buffer_info_ex = { sizeof(CONSOLE_SCREEN_BUFFER_INFOEX) };
	CONSOLE_FONT_INFOEX font_size = { sizeof(CONSOLE_FONT_INFOEX) };
#else
	CONSOLE_SCREEN_BUFFER_INFO screen_buffer_info;
	CONSOLE_FONT_INFOEX font_size = { sizeof font_size };
#endif
	SMALL_RECT window_size;
	int swap_flg = 0;

	CHAR_INFO* screen_buffer_16;
	char* screen_buffer_full;

#ifdef INFO_EX
	//アトリビュート専用バッファ
	WORD*	lpAttribute;
	//パレットテーブル
	COLORREF	g_ConsoleColorTable[NUM_PALETTE] = { 0 };
	//RGB値（数字３文字）"０００"〜"２５５"のテーブル
	char CharRGBconvTBL[3][256] = {
		{'0','0','0'},
	};
#endif	//INFO_EX

	/**
	 * @brief	コンソール I/O 初期化
	 *
	 * @param	width [入力] コンソールウィンドウの横サイズ(1〜)
	 * @param	height [入力] コンソールウィンドウの縦サイズ(1〜)
	 */
	void InitConio(int width, int height)
	{
		CONSOLE_HISTORY_INFO history_info;
		char *out_buf;

#ifdef INFO_EX
		// ディスプレイ情報取得
		display_handle[0] = GetStdHandle(STD_OUTPUT_HANDLE);
		display_handle[1] = NULL;
		input_handle = GetStdHandle(STD_INPUT_HANDLE);
		// コンソールウィンドウのモード設定
		SetConsoleMode(display_handle[0], CONSOLE_OUTPUT_MODE);	//ディスプレイハンドルを出力モードに設定
		SetConsoleMode(input_handle, CONSOLE_INPUT_MODE);	//入力ハンドルを入力モードに設定
		// コマンド履歴を保存しない
		history_info.cbSize = sizeof(CONSOLE_HISTORY_INFO);
		history_info.HistoryBufferSize = 0;
		history_info.NumberOfHistoryBuffers = 0;
		history_info.dwFlags = 0;
		SetConsoleHistoryInfo(&history_info);

		// フォントサイズ変更
		//----------------------------------------------------------
		//CONSOLE_FONT_INFOEX {
		//	ULONG cbSize;
		//	DWORD nFont;		//システムのコンソールフォントテーブル内のフォントのインデックス。
		//	COORD dwFontSize;	//フォント内の各文字の幅と高さ (論理単位) を格納する
		//	UINT  FontFamily;	//フォントのピッチとファミリ。 詳細については、 Textmetric構造体の tmPitchAndFamily メンバーの説明を参照。
		//		下位4ビット：[bit3-TMPF_DEVICE/bit2-TMPF_TRUETYPE/bit1-TMPF_VECTOR/bit0-TMPF_FIXED_PITCH]
		//		※モノスペースビットマップフォントでは、これらの下位ビットがすべてクリアされています。
		//		上位4ビット：Font Families
		//		FF_DONTCARE	(0<<4)	-- Don't care or don't know.
		//		FF_ROMAN	(1<<4)	-- Variable stroke width, serifed. Times Roman, Century Schoolbook, etc. 
		//		FF_SWISS	(2<<4)	-- Variable stroke width, sans-serifed. Helvetica, Swiss, etc.
		//		FF_MODERN	(3<<4)	-- Constant stroke width, serifed or sans-serifed. Pica, Elite, Courier, etc.
		//		FF_SCRIPT	(4<<4)	-- Cursive, etc.
		//		FF_DECORATIVE	(5<<4)	-- Old English, etc.
		//	UINT  FontWeight;				//フォントの太さ。 重みの範囲は 100 ~ 1000 で、100の倍数です。 たとえば、通常の重みは400、700は太字です。
		//	WCHAR FaceName[LF_FACESIZE];	//タイプフェイスの名前 (媒体使用や Arial など)。
		//};
		//----------------------------------------------------------
		//GetCurrentConsoleFontEx:
		//	HANDLE					hConsoleOutput：コンソール画面バッファーへのハンドル。
		//	BOOL					bMaximumWindow：このパラメーターが TRUE の場合、最大ウィンドウサイズに対してフォント情報が設定されます。 
		//											このパラメーターが FALSE の場合、現在のウィンドウサイズに対してフォント情報が設定されます。
		//	PCONSOLE_FONT_INFOEX	lpConsoleCurrentFontEx：フォント情報を格納している「PCONSOLE_FONT_INFOEX」構造体へのポインタ。
		GetCurrentConsoleFontEx(display_handle[0], TRUE, &font_size);
		//----------------------------------------------------------
		//SetCurrentConsoleFontEx:
		//	HANDLE					hConsoleOutput：コンソール画面バッファーへのハンドル。
		//	BOOL					bMaximumWindow：このパラメーターが TRUE の場合、最大ウィンドウサイズに対してフォント情報が設定されます。 
		//											このパラメーターが FALSE の場合、現在のウィンドウサイズに対してフォント情報が設定されます。
		//	PCONSOLE_FONT_INFOEX	lpConsoleCurrentFontEx：フォント情報を格納している「PCONSOLE_FONT_INFOEX」構造体へのポインタ。
		font_size.FontFamily = (FF_DONTCARE | 0x00);	//書体不明＆モノスペース	//初期値？54（0x36:0b0011_0110）
		font_size.FontWeight = 100;	//細字	//初期値？400;
		ZeroMemory(font_size.FaceName, sizeof(font_size.FaceName) * sizeof(WCHAR));	//書体名無し	//初期値？L"MS ゴシック"
		font_size.dwFontSize = { DEF_FONTSIZE_X,DEF_FONTSIZE_Y };
		SetCurrentConsoleFontEx(display_handle[0], TRUE, &font_size);

		// スクリーンバッファの情報を取得
		GetConsoleScreenBufferInfoEx(display_handle[0], &screen_buffer_info_ex);
		// ウィンドウサイズ変更
		window_size.Left = 0;
		window_size.Top = 0;
		window_size.Right = width;// - 1;
		window_size.Bottom = height;// - 1;
		// バッファサイズ変更
		screen_buffer_size.X = width;
		screen_buffer_size.Y = height;
		//
		screen_buffer_info_ex.dwSize = screen_buffer_size;	//文字の列と行のコンソール画面バッファーのサイズ
		screen_buffer_info_ex.dwCursorPosition = { 0,0 };	//COORD{x,y}:コンソール画面バッファー内のカーソルの列座標と行座標
		screen_buffer_info_ex.wAttributes;					//画面バッファーの文字属性【変更なし】
		screen_buffer_info_ex.srWindow = window_size;		//表示ウィンドウの左上隅と右下隅のコンソール画面のバッファー座標
		screen_buffer_info_ex.dwMaximumWindowSize = screen_buffer_info_ex.dwSize;	//コンソールウィンドウの最大サイズ
		screen_buffer_info_ex.bFullscreenSupported;			//全画面表示モードのサポート【変更なし】
		screen_buffer_info_ex.ColorTable;	//コンソールの色設定:COLORREF[16]{0x00bbggrr,,,}
		SetConsoleScreenBufferInfoEx(display_handle[0], &screen_buffer_info_ex);

		// スクリーンバッファ属性(16色用)を生成
		lpAttribute = (WORD*)malloc(sizeof(WORD) * width * height);
		memset(lpAttribute, 0, sizeof(WORD) *width * height);

		//カーソル表示OFF
		CONSOLE_CURSOR_INFO	cci;
		GetConsoleCursorInfo(display_handle[0], &cci);
		cci.dwSize = 1;
		cci.bVisible = FALSE;
		SetConsoleCursorInfo(display_handle[0], &cci);
#else

		// ディスプレイ情報取得
		display_handle[0] = GetStdHandle(STD_OUTPUT_HANDLE);
		display_handle[1] = NULL;
		input_handle = GetStdHandle(STD_INPUT_HANDLE);

		// コンソールウィンドウのモード設定
		SetConsoleMode(display_handle[0], CONSOLE_OUTPUT_MODE);
		SetConsoleMode(input_handle, CONSOLE_INPUT_MODE);

		// コマンド履歴を保存しない
		history_info.cbSize = sizeof(CONSOLE_HISTORY_INFO);
		history_info.HistoryBufferSize = 0;
		history_info.NumberOfHistoryBuffers = 0;
		history_info.dwFlags = 0;
		SetConsoleHistoryInfo(&history_info);

		// ウィンドウサイズ変更
		window_size.Left = 0;
		window_size.Top = 0;
		window_size.Right = width - 1;
		window_size.Bottom = height - 1;
		SetConsoleWindowInfo(display_handle[0], TRUE, &window_size);

		// バッファサイズ変更
		screen_buffer_size.X = width;
		screen_buffer_size.Y = height;
		SetConsoleScreenBufferSize(display_handle[0], screen_buffer_size);

		// フォントサイズ変更
		font_size.dwFontSize.X = DEF_FONTSIZE_X;
		font_size.dwFontSize.Y = DEF_FONTSIZE_Y;
		SetCurrentConsoleFontEx(display_handle[0], TRUE, &font_size);

		// ウィンドウサイズ変更
		window_size.Left = 0;
		window_size.Top = 0;
		window_size.Right = width - 1;
		window_size.Bottom = height - 1;
		SetConsoleWindowInfo(display_handle[0], TRUE, &window_size);

		// スクリーンバッファの情報を取得
		GetConsoleScreenBufferInfo(display_handle[0], &screen_buffer_info);
#endif // INFO_EX

		// スクリーンバッファ(16色用)を生成
		screen_buffer_16 = (CHAR_INFO*)malloc(sizeof(CHAR_INFO) * width * height);
		memset(screen_buffer_16, 0, sizeof(CHAR_INFO) * width * height);
		//memset(screen_buffer_16, 0, width * height);

		// スクリーンバッファ(フルカラー用)を生成
		screen_buffer_full = (char *)malloc(sizeof(char) * (((width * 20) * height) + height));

		// フルカラー用にエスケープシーケンス文字列を設定
		out_buf = screen_buffer_full;
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				sprintf_s(out_buf, 21, "\033[48;2;000;000;000m ");
				out_buf += 20;
			}
			*out_buf = '\n';
			out_buf++;
		}
		out_buf--;
		*out_buf = '\0';
#ifdef INFO_EX
		for (int i = 0; i < 256; i++) {
			CharRGBconvTBL[0][i] = 0x30 + (i / 100);
			CharRGBconvTBL[1][i] = 0x30 + ((i % 100) / 10);
			CharRGBconvTBL[2][i] = 0x30 + (i % 10);
		}
#endif	//INFO_EX
	}


#ifdef INFO_EX
	/**
	 * @brief	ダブルバッファ初期化
	 *
	 * @retval	0	正常終了
	 * @retval	-1	バッファハンドル取得失敗
	 */
	int InitDoubleBuffer(void)
	{
		CONSOLE_CURSOR_INFO	cci;

		if (display_handle[0]) {
			CloseHandle(display_handle[0]);
		}
		// ダブルバッファ用のメモリーを確保
		display_handle[0] = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
		if (display_handle[0] == INVALID_HANDLE_VALUE) {
			printf("ダブルバッファ1のハンドル取得に失敗しました\n");
			return -1;
		}
		display_handle[1] = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
		if (display_handle[1] == INVALID_HANDLE_VALUE) {
			printf("ダブルバッファ2のハンドル取得に失敗しました\n");
			return -1;
		}

		//=== buffer #0 ===
		SetConsoleActiveScreenBuffer(display_handle[0]);	// バッファを入れ替え表示
		// バッファを上書きモードに
		SetConsoleMode(display_handle[0], CONSOLE_OUTPUT_MODE);
		// フォントサイズ変更
		SetCurrentConsoleFontEx(display_handle[0], TRUE, &font_size);
		SetConsoleScreenBufferInfoEx(display_handle[0], &screen_buffer_info_ex);
		//カーソル表示OFF
		GetConsoleCursorInfo(display_handle[0], &cci);
		cci.dwSize = 1;
		cci.bVisible = FALSE;
		SetConsoleCursorInfo(display_handle[0], &cci);

		//=== buffer #1 ===
		SetConsoleActiveScreenBuffer(display_handle[1]);	// バッファを入れ替え表示
		// バッファを上書きモードに
		SetConsoleMode(display_handle[1], CONSOLE_OUTPUT_MODE);
		// フォントサイズ変更
		SetCurrentConsoleFontEx(display_handle[1], TRUE, &font_size);
		SetConsoleScreenBufferInfoEx(display_handle[1], &screen_buffer_info_ex);
		//カーソル表示OFF
		GetConsoleCursorInfo(display_handle[1], &cci);
		cci.dwSize = 1;
		cci.bVisible = FALSE;
		SetConsoleCursorInfo(display_handle[1], &cci);

		SetConsoleActiveScreenBuffer(display_handle[0]);	// バッファを入れ替え表示

		return 0;
	}



#else



	/**
	 * @brief	ダブルバッファ初期化
	 *
	 * @retval	0	正常終了
	 * @retval	-1	バッファハンドル取得失敗
	 */
	int InitDoubleBuffer(void)
	{
		// ダブルバッファ用のメモリーを確保
		display_handle[0] = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
		if (display_handle[0] == INVALID_HANDLE_VALUE) {
			printf("ダブルバッファ1のハンドル取得に失敗しました\n");
			return -1;
		}

		display_handle[1] = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
		if (display_handle[1] == INVALID_HANDLE_VALUE) {
			printf("ダブルバッファ2のハンドル取得に失敗しました\n");
			return -1;
		}

		// バッファを上書きモードに
		SetConsoleMode(display_handle[0], CONSOLE_OUTPUT_MODE);
		SetConsoleMode(display_handle[1], CONSOLE_OUTPUT_MODE);

		// ウィンドウサイズ変更
		SetConsoleWindowInfo(display_handle[0], TRUE, &window_size);
		SetConsoleWindowInfo(display_handle[1], TRUE, &window_size);

		// バッファサイズ変更
		SetConsoleScreenBufferSize(display_handle[0], screen_buffer_size);
		SetConsoleScreenBufferSize(display_handle[1], screen_buffer_size);

		// フォントサイズ変更
		SetCurrentConsoleFontEx(display_handle[0], TRUE, &font_size);
		SetCurrentConsoleFontEx(display_handle[1], TRUE, &font_size);

		// ウィンドウサイズ変更
		SetConsoleWindowInfo(display_handle[0], TRUE, &window_size);
		SetConsoleWindowInfo(display_handle[1], TRUE, &window_size);

		return 0;
	}



#endif	//INFO_EX

	/**
	 * @brief	フォントサイズ変更
	 *
	 * @param	width [入力] フォントの横サイズ(1〜)
	 * @param	height [入力] フォントの縦サイズ(1〜)
	 */
	void SetScreenFontSize(int width, int height)
	{
		// フォントサイズ変更
		font_size.dwFontSize.X = width;
		font_size.dwFontSize.Y = height;
		if (display_handle[1] == NULL)
		{
			SetCurrentConsoleFontEx(display_handle[0], TRUE, &font_size);
		}
		else
		{
			SetCurrentConsoleFontEx(display_handle[0], TRUE, &font_size);
			SetCurrentConsoleFontEx(display_handle[1], TRUE, &font_size);
		}
	}

	/**
	 * @brief	水平方向のカーソル位置を取得
	 *
	 * @return	現在のカーソル位置のX座標(1〜)
	 */
	int GetCursorX(void)
	{
#ifdef INFO_EX
		return screen_buffer_info_ex.dwCursorPosition.X - screen_buffer_info_ex.srWindow.Left + 1;
#else
		return screen_buffer_info.dwCursorPosition.X - screen_buffer_info.srWindow.Left + 1;
#endif	//INFO_EX
	}

	/**
	 * @brief	垂直方向のカーソル位置を取得
	 *
	 * @return	現在のカーソル位置のY座標(1〜)
	 */
	int GetCursorY(void)
	{
#ifdef INFO_EX
		return screen_buffer_info_ex.dwCursorPosition.Y - screen_buffer_info_ex.srWindow.Top + 1;
#else
		return screen_buffer_info.dwCursorPosition.Y - screen_buffer_info.srWindow.Top + 1;
#endif // INFO_EX

	}

	/**
	 * @brief	カーソル位置の移動
	 *
	 * @param	x [入力] X座標(1〜)
	 * @param	y [入力] Y座標(1〜)
	 */
	void SetCursorPosition(int x, int y)
	{
		COORD lc;

		lc.X = x - 1;
#ifdef INFO_EX
		lc.Y = screen_buffer_info_ex.srWindow.Top + y - 1;
#else
		lc.Y = screen_buffer_info.srWindow.Top + y - 1;
#endif // INFO_EX
		SetConsoleCursorPosition(display_handle[swap_flg], lc);
	}

	/**
	 * @brief	文字列の出力
	 *
	 * @param	buf [入力] 出力文字列配列のポインタ
	 * @param	size [入力] 出力文字数
	 */
	void PrintString(const char* buf, int size)
	{
		DWORD write_num;

		WriteConsole(display_handle[swap_flg], buf, size, &write_num, NULL);
	}

#ifdef INFO_EX
	/**
	 * @brief	画像の出力
	 *
	 * @param	buf [入力] RGB画像データ配列のポインタ
	 * @note
	 *	RGB画像データ配列はスクリーンの横幅×縦幅のバイト数以上の配列とし、
	 *	配列の中身はRGB各1バイト(合計3バイト)を1画素としたデータにする。
	 *	全ての画素は連続している必要あり。
	 *	例)横80文字×縦25行の場合、80x25=200バイト以上の配列を渡す
	 */
	void PrintImage(unsigned char* buf)
	{
		DWORD write_num;
		unsigned char *in_buf = buf;
		char *out_buf = screen_buffer_full;

		for (int y = 0; y < screen_buffer_size.Y; y++)
		{
			for (int x = 0; x < screen_buffer_size.X; x++)
			{
				out_buf += 7;
				// R設定
				*(out_buf + 0) = CharRGBconvTBL[0][*in_buf];	//0x30 + (*in_buf / 100);
				*(out_buf + 1) = CharRGBconvTBL[1][*in_buf];	//0x30 + (*in_buf % 100 / 10);
				*(out_buf + 2) = CharRGBconvTBL[2][*in_buf];	//0x30 + (*in_buf % 10);
				in_buf++;
				// G設定
				*(out_buf + 4) = CharRGBconvTBL[0][*in_buf];	//0x30 + (*in_buf / 100);
				*(out_buf + 5) = CharRGBconvTBL[1][*in_buf];	//0x30 + (*in_buf % 100 / 10);
				*(out_buf + 6) = CharRGBconvTBL[2][*in_buf];	//0x30 + (*in_buf % 10);
				in_buf++;
				// B設定
				*(out_buf + 8) = CharRGBconvTBL[0][*in_buf];	//0x30 + (*in_buf / 100);
				*(out_buf + 9) = CharRGBconvTBL[1][*in_buf];	//0x30 + (*in_buf % 100 / 10);
				*(out_buf + 10) = CharRGBconvTBL[2][*in_buf];	//0x30 + (*in_buf % 10);
				in_buf++;
				out_buf += 13;
			}
			out_buf++;
		}
		WriteConsole(display_handle[swap_flg], screen_buffer_full, screen_buffer_size.Y * (screen_buffer_size.X * 20) + (screen_buffer_size.Y - 1), &write_num, NULL);
	}
#else
	/**
	 * @brief	画像の出力
	 *
	 * @param	buf [入力] RGB画像データ配列のポインタ
	 * @note
	 *	RGB画像データ配列はスクリーンの横幅×縦幅のバイト数以上の配列とし、
	 *	配列の中身はRGB各1バイト(合計3バイト)を1画素としたデータにする。
	 *	全ての画素は連続している必要あり。
	 *	例)横80文字×縦25行の場合、80x25=200バイト以上の配列を渡す
	 */
	void PrintImage(unsigned char* buf)
	{
		DWORD write_num;
		unsigned char *in_buf = buf;
		char *out_buf = screen_buffer_full;

		for (int y = 0; y < screen_buffer_size.Y; y++)
		{
			for (int x = 0; x < screen_buffer_size.X; x++)
			{
				out_buf += 7;
				// R設定
				*(out_buf + 0) = 0x30 + (*in_buf / 100);
				*(out_buf + 1) = 0x30 + (*in_buf % 100 / 10);
				*(out_buf + 2) = 0x30 + (*in_buf % 10);
				in_buf++;
				// G設定
				*(out_buf + 4) = 0x30 + (*in_buf / 100);
				*(out_buf + 5) = 0x30 + (*in_buf % 100 / 10);
				*(out_buf + 6) = 0x30 + (*in_buf % 10);
				in_buf++;
				// B設定
				*(out_buf + 8) = 0x30 + (*in_buf / 100);
				*(out_buf + 9) = 0x30 + (*in_buf % 100 / 10);
				*(out_buf + 10) = 0x30 + (*in_buf % 10);
				in_buf++;
				out_buf += 13;
			}
			out_buf++;
		}
		WriteConsole(display_handle[swap_flg], screen_buffer_full, screen_buffer_size.Y * (screen_buffer_size.X * 20) + (screen_buffer_size.Y - 1), &write_num, NULL);
	}
#endif // INFO_EX

	/**
	 * @brief	行末まで消去
	 */
	void ClearLine(void)
	{
		DWORD fill_num;
#ifdef INFO_EX
		FillConsoleOutputAttribute(display_handle[swap_flg],
			screen_buffer_info_ex.wAttributes,
			screen_buffer_info_ex.srWindow.Right - screen_buffer_info_ex.dwCursorPosition.X + 1,
			screen_buffer_info_ex.dwCursorPosition,
			&fill_num);
		FillConsoleOutputCharacter(display_handle[swap_flg],
			TEXT(' '),
			screen_buffer_info_ex.srWindow.Right - screen_buffer_info_ex.dwCursorPosition.X + 1,
			screen_buffer_info_ex.dwCursorPosition,
			&fill_num);
#else
		FillConsoleOutputAttribute(display_handle[swap_flg],
			screen_buffer_info.wAttributes,
			screen_buffer_info.srWindow.Right - screen_buffer_info.dwCursorPosition.X + 1,
			screen_buffer_info.dwCursorPosition,
			&fill_num);
		FillConsoleOutputCharacter(display_handle[swap_flg],
			TEXT(' '),
			screen_buffer_info.srWindow.Right - screen_buffer_info.dwCursorPosition.X + 1,
			screen_buffer_info.dwCursorPosition,
			&fill_num);
#endif
	}

	/**
	 * @brief	画面消去
	 */
	void ClearScreen(void)
	{
		DWORD fill_num;
		COORD screen_origin = { 0, 0 };
#ifdef INFO_EX
		screen_buffer_attribute = screen_buffer_info_ex.wAttributes;
		FillConsoleOutputAttribute(display_handle[swap_flg],
			screen_buffer_info_ex.wAttributes,
			screen_buffer_info_ex.dwSize.X * screen_buffer_info_ex.dwSize.Y,
			screen_origin,
			&fill_num);
		FillConsoleOutputCharacter(display_handle[swap_flg],
			TEXT(' '),
			screen_buffer_info_ex.dwSize.X * screen_buffer_info_ex.dwSize.Y,
			screen_origin,
			&fill_num);
		SetCursorPosition(screen_buffer_info_ex.srWindow.Left + 1, screen_buffer_info_ex.srWindow.Top + 1);
#else
		screen_buffer_attribute = screen_buffer_info.wAttributes;
		FillConsoleOutputAttribute(display_handle[swap_flg],
			screen_buffer_info.wAttributes,
			screen_buffer_info.dwSize.X * screen_buffer_info.dwSize.Y,
			screen_origin,
			&fill_num);
		FillConsoleOutputCharacter(display_handle[swap_flg],
			TEXT(' '),
			screen_buffer_info.dwSize.X * screen_buffer_info.dwSize.Y,
			screen_origin,
			&fill_num);
		SetCursorPosition(screen_buffer_info.srWindow.Left + 1, screen_buffer_info.srWindow.Top + 1);
#endif // INFO_EX
	}

	/**
	 * @brief	ダブルバッファ時の描画面切替
	 */
	void FlipScreen(void)
	{
		SetConsoleActiveScreenBuffer(display_handle[swap_flg]);	// バッファを入れ替え表示
		swap_flg = (swap_flg) ? 0 : 1;
	}

	/**
	 * @brief	文字色高輝度化
	 */
	void SetHighVideoColor(void)
	{
#ifdef INFO_EX
		SetConsoleTextAttribute(display_handle[swap_flg], screen_buffer_info_ex.wAttributes |= FOREGROUND_INTENSITY);
#else
		SetConsoleTextAttribute(display_handle[swap_flg], screen_buffer_info.wAttributes |= FOREGROUND_INTENSITY);
#endif // INFO_EX
	}

	/**
	 * @brief	文字色低輝度化
	 */
	void SetLowVideoColor(void)
	{
#ifdef INFO_EX
		SetConsoleTextAttribute(display_handle[swap_flg], screen_buffer_info_ex.wAttributes &= ~FOREGROUND_INTENSITY);
#else
		SetConsoleTextAttribute(display_handle[swap_flg], screen_buffer_info.wAttributes &= ~FOREGROUND_INTENSITY);
#endif // INFO_EX
	}

	/**
	 * @brief	既定文字色設定
	 */
	void SetNormalVideoColor(void)
	{
		SetConsoleTextAttribute(display_handle[swap_flg], LIGHTGRAY);
	}

	/**
	 * @brief	文字背景色設定
	 *
	 * @param	color [入力] 文字背景色
	 * @note
	 *	背景色はenum COLORSを参照する
	 */
	void SetTextBackColor(int color)
	{
#ifdef INFO_EX
		screen_buffer_info_ex.wAttributes &= ~0x00f0;
		screen_buffer_info_ex.wAttributes |= ((color & 0x07) << 4);
		SetConsoleTextAttribute(display_handle[swap_flg], screen_buffer_info_ex.wAttributes);
#else
		screen_buffer_info.wAttributes &= ~0x00f0;
		screen_buffer_info.wAttributes |= ((color & 0x07) << 4);
		SetConsoleTextAttribute(display_handle[swap_flg], screen_buffer_info.wAttributes);
#endif // INFO_EX
	}

	/**
	 * @brief	文字色設定
	 *
	 * @param	color [入力] 文字色
	 * @note
	 *	文字色はenum COLORSを参照する
	 */
	void SetConsoleTextColor(int color)
	{
#ifdef INFO_EX
		screen_buffer_info_ex.wAttributes &= ~0x000f;
		screen_buffer_info_ex.wAttributes |= (color & 0x0f);
		SetConsoleTextAttribute(display_handle[swap_flg], screen_buffer_info_ex.wAttributes);
#else
		screen_buffer_info.wAttributes &= ~0x000f;
		screen_buffer_info.wAttributes |= (color & 0x0f);
		SetConsoleTextAttribute(display_handle[swap_flg], screen_buffer_info.wAttributes);
#endif // INFO_EX
	}

	/**
	 * @brief	文字色背景色同時設定
	 *
	 * @param	attribute [入力] 文字背景情報
	 * @note
	 *	以下の各設定値をビット毎のOR演算を用いて引数に指定する
	 *	  FOREGROUND_BLUE			 0x0001 // text color contains blue.
	 *	  FOREGROUND_GREEN			 0x0002 // text color contains green.
	 *	  FOREGROUND_RED			 0x0004 // text color contains red.
	 *	  FOREGROUND_INTENSITY		 0x0008 // text color is intensified.
	 *	  BACKGROUND_BLUE			 0x0010 // background color contains blue.
	 *	  BACKGROUND_GREEN			 0x0020 // background color contains green.
	 *	  BACKGROUND_RED			 0x0040 // background color contains red.
	 *	  BACKGROUND_INTENSITY		 0x0080 // background color is intensified.
	 *	  COMMON_LVB_LEADING_BYTE	 0x0100 // Leading Byte of DBCS
	 *	  COMMON_LVB_TRAILING_BYTE	 0x0200 // Trailing Byte of DBCS
	 *	  COMMON_LVB_GRID_HORIZONTAL 0x0400 // DBCS: Grid attribute: top horizontal.
	 *	  COMMON_LVB_GRID_LVERTICAL  0x0800 // DBCS: Grid attribute: left vertical.
	 *	  COMMON_LVB_GRID_RVERTICAL  0x1000 // DBCS: Grid attribute: right vertical.
	 *	  COMMON_LVB_REVERSE_VIDEO	 0x4000 // DBCS: Reverse fore/back ground attribute.
	 *	  COMMON_LVB_UNDERSCORE 	 0x8000 // DBCS: Underscore.
	 *	  COMMON_LVB_SBCSDBCS		 0x0300 // SBCS or DBCS flag.
	 */
	void SetTextAttribute(int attribute)
	{
		SetConsoleTextAttribute(display_handle[swap_flg], attribute);
	}

	/**
	 * @brief	カーソルタイプ設定
	 *
	 * @param	type [入力] NOCURSOR カーソル表示なし
	 *						SOLIDCURSOR (非対応)
	 *						NORMALCURSOR カーソルの通常表示
	 */
	void SetCursorType(int type)
	{
		CONSOLE_CURSOR_INFO	cursor_info;
		int size = -1;

		if (size < 0) {
			if (GetConsoleCursorInfo(display_handle[swap_flg], &cursor_info)) {
				size = (int)cursor_info.dwSize;
			}
			else {
				size = 25;
			}
		}
		cursor_info.dwSize = (type < NORMALCURSOR) ? 100 : size;
		cursor_info.bVisible = (type != NOCURSOR);
		SetConsoleCursorInfo(display_handle[swap_flg], &cursor_info);
	}

	/**
	 * @brief	現在行に挿入
	 */
	void InsertLine(void)
	{
		COORD	lc;
		DWORD	len;
		DWORD	num;
		LPTSTR	psz;
		LPWORD	pw;
#ifdef INFO_EX
		lc.X = screen_buffer_info_ex.srWindow.Left;
		len = screen_buffer_info_ex.srWindow.Right - screen_buffer_info_ex.srWindow.Left + 1;
		psz = (LPTSTR)_alloca(len * sizeof(TCHAR));
		pw = (LPWORD)_alloca(len * sizeof(WORD));
		for (lc.Y = screen_buffer_info_ex.srWindow.Bottom; lc.Y > screen_buffer_info_ex.dwCursorPosition.Y; lc.Y--) {
			lc.Y--;
			ReadConsoleOutputAttribute(display_handle[swap_flg], pw, len, lc, &num);
			ReadConsoleOutputCharacter(display_handle[swap_flg], psz, len, lc, &num);
			lc.Y++;
			WriteConsoleOutputAttribute(display_handle[swap_flg], pw, len, lc, &num);
			WriteConsoleOutputCharacter(display_handle[swap_flg], psz, len, lc, &num);
		}
		FillConsoleOutputAttribute(display_handle[swap_flg], screen_buffer_info_ex.wAttributes, len, lc, &num);
		FillConsoleOutputCharacter(display_handle[swap_flg], TEXT(' '), len, lc, &num);
#else
		lc.X = screen_buffer_info.srWindow.Left;
		len = screen_buffer_info.srWindow.Right - screen_buffer_info.srWindow.Left + 1;
		psz = (LPTSTR)_alloca(len * sizeof(TCHAR));
		pw = (LPWORD)_alloca(len * sizeof(WORD));
		for (lc.Y = screen_buffer_info.srWindow.Bottom; lc.Y > screen_buffer_info.dwCursorPosition.Y; lc.Y--) {
			lc.Y--;
			ReadConsoleOutputAttribute(display_handle[swap_flg], pw, len, lc, &num);
			ReadConsoleOutputCharacter(display_handle[swap_flg], psz, len, lc, &num);
			lc.Y++;
			WriteConsoleOutputAttribute(display_handle[swap_flg], pw, len, lc, &num);
			WriteConsoleOutputCharacter(display_handle[swap_flg], psz, len, lc, &num);
		}
		FillConsoleOutputAttribute(display_handle[swap_flg], screen_buffer_info.wAttributes, len, lc, &num);
		FillConsoleOutputCharacter(display_handle[swap_flg], TEXT(' '), len, lc, &num);
#endif // INFO_EX
	}

	/**
	 * @brief	現在行の削除
	 */
	void DeleteLine(void)
	{
		DWORD	read_num;
		DWORD	write_num;
		DWORD	fill_num;
		COORD	calc_coord;
		DWORD	line_len;
		LPTSTR	receive_character;
		LPWORD	receive_attribute;
#ifdef INFO_EX
		calc_coord.X = screen_buffer_info_ex.srWindow.Left;
		line_len = screen_buffer_info_ex.srWindow.Right - screen_buffer_info_ex.srWindow.Left + 1;
		receive_character = (LPTSTR)_alloca(line_len * sizeof(TCHAR));
		receive_attribute = (LPWORD)_alloca(line_len * sizeof(WORD));

		for (calc_coord.Y = screen_buffer_info_ex.dwCursorPosition.Y; calc_coord.Y < screen_buffer_info_ex.srWindow.Bottom; calc_coord.Y++) {
			calc_coord.Y++;
			ReadConsoleOutputAttribute(display_handle[swap_flg], receive_attribute, line_len, calc_coord, &read_num);
			ReadConsoleOutputCharacter(display_handle[swap_flg], receive_character, line_len, calc_coord, &read_num);
			calc_coord.Y--;
			WriteConsoleOutputAttribute(display_handle[swap_flg], receive_attribute, line_len, calc_coord, &write_num);
			WriteConsoleOutputCharacter(display_handle[swap_flg], receive_character, line_len, calc_coord, &write_num);
		}

		FillConsoleOutputAttribute(display_handle[swap_flg], screen_buffer_attribute, line_len, calc_coord, &fill_num);
		FillConsoleOutputCharacter(display_handle[swap_flg], TEXT(' '), line_len, calc_coord, &fill_num);
#else
		calc_coord.X = screen_buffer_info.srWindow.Left;
		line_len = screen_buffer_info.srWindow.Right - screen_buffer_info.srWindow.Left + 1;
		receive_character = (LPTSTR)_alloca(line_len * sizeof(TCHAR));
		receive_attribute = (LPWORD)_alloca(line_len * sizeof(WORD));

		for (calc_coord.Y = screen_buffer_info.dwCursorPosition.Y; calc_coord.Y < screen_buffer_info.srWindow.Bottom; calc_coord.Y++) {
			calc_coord.Y++;
			ReadConsoleOutputAttribute(display_handle[swap_flg], receive_attribute, line_len, calc_coord, &read_num);
			ReadConsoleOutputCharacter(display_handle[swap_flg], receive_character, line_len, calc_coord, &read_num);
			calc_coord.Y--;
			WriteConsoleOutputAttribute(display_handle[swap_flg], receive_attribute, line_len, calc_coord, &write_num);
			WriteConsoleOutputCharacter(display_handle[swap_flg], receive_character, line_len, calc_coord, &write_num);
		}

		FillConsoleOutputAttribute(display_handle[swap_flg], screen_buffer_attribute, line_len, calc_coord, &fill_num);
		FillConsoleOutputCharacter(display_handle[swap_flg], TEXT(' '), line_len, calc_coord, &fill_num);
#endif // INFO_EX
	}

	/**
	 * @brief	文字列の一括転送
	 *
	 * @param	buf [入力] スクリーンバッファのポインタ
	 */
	void PrintFrameBuffer(char* buf)
	{
		CHAR_INFO *char_info = screen_buffer_16;

		// 画像描画(画面外へのはみ出し処理なし)
		for (int count = 0; count < screen_buffer_size.Y * screen_buffer_size.X; count++) {
			// 背景色を4ビット左シフトしてセット
			char_info->Attributes = (*buf & 0x0F) | ((*buf << 4) & 0xF0);
			char_info++;
			buf++;
		}

		// 画面の一括描画
		WriteConsoleOutputA(display_handle[swap_flg], screen_buffer_16, screen_buffer_size, { 0,0 }, &window_size);
	}

#ifdef INFO_EX
	/*
		矩形転送
		const CHAR_INFO* p_srcBuff：転送元バッファの先頭アドレス
		const COORD srcBuffSize：転送元バッファ全体のサイズ
		COORD srcPos：転送元の転送開始する座標（サイズは転送先のサイズと同じになる）
		SMALL_RECT destRect：転送先の座標とサイズ
	*/
	BOOL DrawImageRect(const CHAR_INFO* p_srcBuff, const COORD srcBuffSize, const COORD srcPos, SMALL_RECT destRect)
	{
		//ZeroMemory(screen_buffer_16, sizeof(CHAR_INFO) * screen_buffer_size.X * screen_buffer_size.Y);
		//const COORD rectSize = { destRect.Right - destRect.Left + 1,destRect.Bottom - destRect.Top + 1 };
		//int srcBaseAddr = (srcPos.Y*srcBuffSize.X + srcPos.X);
		//int destBaseAddr = (destRect.Top*screen_buffer_size.X + destRect.Left);
		//CHAR_INFO *char_info = screen_buffer_16;
		////転送矩形のサイズ{ X:横dot, Y:縦dot };
		//// 画像描画(画面外へのはみ出し処理なし)
		//for (int y = 0; y < rectSize.Y; y++) {
		//	for (int x = 0; x < rectSize.X; x++) {
		//		//screen_buffer_16[destBaseAddr + (y * srcBuffSize.X) + x] = p_srcBuff[srcBaseAddr + y * srcBuffSize.X + x];
		//		screen_buffer_16[destBaseAddr + (y * srcBuffSize.X) + x] = p_srcBuff[srcBaseAddr + y * srcBuffSize.X + x];
		//	}
		//}
		//WriteConsoleOutput(display_handle[swap_flg], screen_buffer_16, screen_buffer_size, { 0,0 }, &window_size);
		//return TRUE;
		// 画面の一括描画
		return WriteConsoleOutput(display_handle[swap_flg], p_srcBuff, srcBuffSize, srcPos, &destRect);
	}
	/**
	 * @brief	文字列(属性)の一括転送
	 *
	 * @param	buf [入力] スクリーンバッファのポインタ
	 */
	void PrintFrameBufferAttribute(const char* buf)
	{
		DWORD	NumberOfAttrsWritten;
		int buffSize = screen_buffer_size.X * screen_buffer_size.Y;
		WORD* attrib = lpAttribute;	//サイズはInitConioで指定したサイズ。
		// 画像描画(画面外へのはみ出し処理なし)
		for (int n = 0; n < buffSize; n++) {
			// 背景色を4ビット左シフトしてセット
			//attrib[n] = (*buf & 0x00) | ((*buf << 4) & 0xF0);
			attrib[n] = (((*buf) << 4) & 0x00F0);
			buf++;
		}
		WriteConsoleOutputAttribute(display_handle[swap_flg], lpAttribute, buffSize, { 0,0 }, &NumberOfAttrsWritten);
	}
	/**
	 * @brief	一括転送用バッファクリア
	 *
	 * @param	buf [入力] スクリーンバッファのポインタ
	 * @note
	 *	スクリーンバッファはウィンドウサイズの横幅×縦幅の
	 *	合計バイトサイズ以上のchar型配列とする
	 *	スクリーンバッファの内容を全て0でクリアする
	 */
	void ClearFrameBufferFull(char* buf)
	{
		ZeroMemory(buf, sizeof(char) * screen_buffer_size.X * screen_buffer_size.Y * 3);
	}
#endif // INFO_EX

	/**
	 * @brief	一括転送用バッファクリア
	 *
	 * @param	buf [入力] スクリーンバッファのポインタ
	 * @note
	 *	スクリーンバッファはウィンドウサイズの横幅×縦幅の
	 *	合計バイトサイズ以上のchar型配列とする
	 *	スクリーンバッファの内容を全て0でクリアする
	 */
	void ClearFrameBuffer(char* buf)
	{
		ZeroMemory(buf, sizeof(char) * screen_buffer_size.X * screen_buffer_size.Y);
	}

	/**
	 * @brief	コンソールウィンドウのタイトルバーに表示されるテキストを設定
	 *
	 * @param	title [入力] ウィンドウタイトルに表示するテキスト
	 */
	void SetCaption(const char* title)
	{
		SetConsoleTitleA(title);
	}

	/**
	 * @brief	コンソールウィンドウのタイトルバーに表示されるテキストを取得
	 *
	 * @param	title [出力] 現在のウィンドウタイトルのテキスト
	 * @param	len [入力] ウィンドウタイトルの文字数
	 *
	 * @retval	非0	現在のウィンドウタイトルの文字数
	 * @retval	0	エラー
	 */
	int GetCaption(char* title, int len)
	{
		return GetConsoleTitleA(title, len);
	}

	/**
	 * @brief	キー情報リセット
	 */
	void ResetKeyMap(void)
	{
		for (int count = 0; count < 8; count++) {
			conio_key_map[count] = 0;
		}
	}

	/**
	 * @brief	ジョイパッド入力
	 *
	 * @param	port [入力] ポート番号(P*_*)
	 *
	 * @retval	0		正常終了
	 * @retval	1以上	Joystickの入力値
	 * @retval	-1		エラー
	 */
	int InputJoystick(int port)
	{
		JOYINFO	joy_info;
		int id;
		int func;

		// ゲームパッド入力
		if ((port & 0xfe00) == 0x0200) {
			id = (port & 0x01f0) >> 4;
			func = port & 0x0f;

			switch (func) {
			case 0:
			case 1:
			case 2:
			case 3:

				if (joyGetPos(id, &joy_info) != JOYERR_NOERROR) {
					return -1;
				}
				switch (func) {
				case 0:
					return joy_info.wXpos;
				case 1:
					return joy_info.wYpos;
				case 2:
					return joy_info.wZpos;
				case 3:
					return joy_info.wButtons;
				}
				break;
			default:
				break;
			}
			return 0;
		}
		return -1;
	}

	/**
	 * @brief	ジョイパッド詳細入力
	 *
	 * @param	port [入力] コントローラ番号
	 * @param	port [入力] ポート番号(P*_*)
	 *
	 * @retval	0		正常終了
	 * @retval	1以上	Joystickの入力値
	 * @retval	-1		エラー
	 */
	int InputJoystickEx(int id, int port)
	{
		JOYINFOEX joy_info;

		if (joyGetPosEx(id, &joy_info) != JOYERR_NOERROR) {
			return -1;
		}

		switch (port) {
		case PJEX_XPOS:
			return joy_info.dwXpos;
		case PJEX_YPOS:
			return joy_info.dwYpos;
		case PJEX_ZPOS:
			return joy_info.dwZpos;
		case PJEX_RPOS:
			return joy_info.dwRpos;
		case PJEX_UPOS:
			return joy_info.dwUpos;
		case PJEX_VPOS:
			return joy_info.dwVpos;
		case PJEX_BTNS:
			return joy_info.dwButtons;
		case PJEX_BTNNO:
			return joy_info.dwButtonNumber;
		case PJEX_POV:
			return joy_info.dwPOV;
		case PJEX_SIZE:
			return joy_info.dwSize;
		case PJEX_FLAGS:
			return joy_info.dwFlags;
		default:
			break;
		}
		return 0;
	}

	/**
	 * @brief	ジョイパッド入力(XInput対応)
	 *
	 * @param	port [入力] ポート番号(P*_*)
	 *
	 * @retval	0		正常終了
	 * @retval	1以上	Joystickの入力値
	 * @retval	-1		エラー
	 *
	 * @note
	 *	LRのスティックは中心から左右に移動する際のデッドゾーンが用意されている
	 *	（デッドゾーンは左右に移動したとみなさないエリアのこと）
	 *	 #define XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE  7849
	 *	 #define XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE 8689
	 */
	int InputJoystickX(int id, int port)
	{
		XINPUT_STATE controller_state;	// XInputコントローラ情報

		if (XInputGetState(id, &controller_state) != ERROR_SUCCESS) {
			return -1;
		}

		switch (port) {
		case PJX_LXPOS:
			return controller_state.Gamepad.sThumbLX;
		case PJX_LYPOS:
			return controller_state.Gamepad.sThumbLY;
		case PJX_LTRG:
			return controller_state.Gamepad.bLeftTrigger;
		case PJX_RXPOS:
			return controller_state.Gamepad.sThumbRX;
		case PJX_RYPOS:
			return controller_state.Gamepad.sThumbRY;
		case PJX_RTRG:
			return controller_state.Gamepad.bRightTrigger;
		case PJX_BTNS:
			if (controller_state.dwPacketNumber) {
				return controller_state.Gamepad.wButtons;
			}
		}
		return 0;
	}

	/**
	 * @brief	キーボード・マウス入力
	 *
	 * @param	port [入力] ポート番号(P*_*)
	 * @return	入力値
	 */
	int InputKeyMouse(int port)
	{
		DWORD event = 0;
		DWORD read = 0;
		PINPUT_RECORD input_record;
		KEY_EVENT_RECORD* key_event;
		MOUSE_EVENT_RECORD* mouse_event;

		// キーボードイベントチェック
		if (GetNumberOfConsoleInputEvents(input_handle, &event) && event) {
			//入力イベントが正常に取得でき、かつ、イベントが０じゃない場合。
			read = 0;
			//入力データ（レコード）を受け取るローカルバッファ（スタック上に確保）
			input_record = (PINPUT_RECORD)_alloca(event * sizeof(INPUT_RECORD));
			//コンソールの入力バッファーからデータを読み取り、バッファーから削除
			if (ReadConsoleInput(input_handle, input_record, event, &read) && read) {
				//入力バッファからの読み取りが正常に終了し、かつ、読み取ったレコード数が０じゃない場合。
				input_record = input_record;
				for (unsigned int count = 0; count < read; count++, input_record++) {
					switch (input_record->EventType) {
					case KEY_EVENT: {
						key_event = &input_record->Event.KeyEvent;
						if (key_event->wVirtualKeyCode > 0x0FF) {
							break;
						}
						if (key_event->bKeyDown) {
							conio_key_map[key_event->wVirtualKeyCode >> 5] |= (0x01 << (key_event->wVirtualKeyCode & 31));
						}
						else {
							conio_key_map[key_event->wVirtualKeyCode >> 5] &= ~(0x01 << (key_event->wVirtualKeyCode & 31));
						}
						if (key_event->dwControlKeyState & (LEFT_ALT_PRESSED | RIGHT_ALT_PRESSED)) {
							conio_key_map[VK_MENU >> 5] |= (0x01 << (VK_MENU & 31));
						}
						else {
							conio_key_map[VK_MENU >> 5] &= ~(0x01 << (VK_MENU & 31));
						}
						if (key_event->dwControlKeyState & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED)) {
							conio_key_map[VK_CONTROL >> 5] |= (0x01 << (VK_CONTROL & 31));
						}
						else {
							conio_key_map[VK_CONTROL >> 5] &= ~(0x01 << (VK_CONTROL & 31));
						}
						if (key_event->dwControlKeyState & SHIFT_PRESSED) {
							conio_key_map[VK_SHIFT >> 5] |= (0x01 << (VK_SHIFT & 31));
						}
						else {
							conio_key_map[VK_SHIFT >> 5] &= ~(0x01 << (VK_SHIFT & 31));
						}
						break;
					}
					case MOUSE_EVENT: {
						mouse_event = &input_record->Event.MouseEvent;
						conio_mouse_position = mouse_event->dwMousePosition;
						if (mouse_event->dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) {
							conio_key_map[VK_LBUTTON >> 5] |= (0x01 << (VK_LBUTTON & 31));
						}
						else {
							conio_key_map[VK_LBUTTON >> 5] &= ~(0x01 << (VK_LBUTTON & 31));
						}
						if (mouse_event->dwButtonState & FROM_LEFT_2ND_BUTTON_PRESSED) {
							conio_key_map[VK_MBUTTON >> 5] |= (0x01 << (VK_MBUTTON & 31));
						}
						else {
							conio_key_map[VK_MBUTTON >> 5] &= ~(0x01 << (VK_MBUTTON & 31));
						}
						if (mouse_event->dwButtonState & RIGHTMOST_BUTTON_PRESSED) {
							conio_key_map[VK_RBUTTON >> 5] |= (0x01 << (VK_RBUTTON & 31));
						}
						else {
							conio_key_map[VK_RBUTTON >> 5] &= ~(0x01 << (VK_RBUTTON & 31));
						}
						break;
					}
					default:
						break;
					}
				}
			}
		}

		// マウス座標を返す
		switch (port) {
		case PM_CURX:
			return conio_mouse_position.X + 1;
		case PM_CURY:
			return conio_mouse_position.Y + 1;
		default:
			break;
		}
		// キー状態を返す
		return (conio_key_map[(port & 0x0FF) >> 5] & (0x01 << (port & 31))) != 0;
	}

	/**
	 * @brief	サウンド ファイルを開く
	 *
	 * @param	path [入力] ファイル名
	 *
	 * @retval	非0	サウンド ハンドル
	 * @retval	0	エラー
	 */
	int* MciOpenSound(const char* path)
	{
		union {
			MCI_WAVE_OPEN_PARMSA	wave_param;
			MCI_OPEN_PARMSA			open_param;
		} MciParam;
		TCHAR error_str[256];
		const char midi_ext[] = ".mid|.midi|.rmi";
		const char wave_ext[] = ".wav|.wave";
		const char mp3_ext[] = ".mp3";
		char input_ext[_MAX_EXT];
		DWORD_PTR mci_command;
		MCIERROR mci_error;
		MciSoundInfo* sound_info;

		sound_info = (MciSoundInfo *)malloc(sizeof(MciSoundInfo));
		if (sound_info == NULL) {
			return 0;
		}
		ZeroMemory(sound_info, sizeof(*sound_info));
		ZeroMemory(&MciParam, sizeof(MciParam));
		_splitpath_s(path, NULL, 0, NULL, 0, NULL, 0, input_ext, sizeof(input_ext));
		_strlwr_s(input_ext, strlen(input_ext) + 1);
		mci_command = MCI_OPEN_TYPE | MCI_OPEN_ELEMENT;
		if (strstr(midi_ext, input_ext)) {
			sound_info->device_type = MCI_DEVTYPE_SEQUENCER;
			lstrcpynA(sound_info->path, path, MAX_PATH);
			MciParam.open_param.lpstrDeviceType = (LPCSTR)MCI_DEVTYPE_SEQUENCER;
			MciParam.open_param.lpstrElementName = TEXT(sound_info->path);
			mci_command |= MCI_OPEN_TYPE_ID;
		}
		else if (strstr(wave_ext, input_ext)) {
			sound_info->device_type = MCI_DEVTYPE_WAVEFORM_AUDIO;
			lstrcpynA(sound_info->path, path, MAX_PATH);
			MciParam.wave_param.lpstrDeviceType = (LPCSTR)MCI_DEVTYPE_WAVEFORM_AUDIO;
			MciParam.wave_param.lpstrElementName = TEXT(sound_info->path);
			mci_command |= MCI_OPEN_TYPE_ID;
			// MciParam.wave_param.dwBufferSeconds	= 60;
			// mci_command |= MCI_WAVE_OPEN_BUFFER;
		}
		else if (strstr(mp3_ext, input_ext)) {
			sound_info->device_type = MCI_DEVTYPE_DIGITAL_VIDEO;
			lstrcpynA(sound_info->path, path, MAX_PATH);
			MciParam.open_param.lpstrDeviceType = "MPEGVideo";
			MciParam.open_param.lpstrElementName = TEXT(sound_info->path);
		}
		else {
			free(sound_info);
			return 0;
		}
		mci_error = mciSendCommandA(0, MCI_OPEN, mci_command, (DWORD_PTR)&MciParam);
		if (mci_error != 0) {
			free(sound_info);
			mciGetErrorString(mci_error, error_str, sizeof(error_str) / sizeof(TCHAR));
			MessageBox(NULL, error_str, NULL, MB_ICONWARNING);
			return 0;
		}
		sound_info->device_id = MciParam.open_param.wDeviceID;
		return (int *)sound_info;
	}

	/**
	 * @brief	サウンド ファイルを閉じる
	 *
	 * @param	sound_id [入力] サウンド ハンドル
	 */
	void MciCloseSound(int* sound_id)
	{
		MciSoundInfo* sound_info;

		if (!sound_id) {
			return;
		}
		sound_info = (MciSoundInfo *)sound_id;
		if (sound_info->device_id) {
			mciSendCommand(sound_info->device_id, MCI_CLOSE, 0, 0);
			sound_info->device_id = 0;
		}
		free(sound_info);
	}

	/**
	 * @brief	サウンドを再生する
	 *
	 * @param	sound_id [入力] サウンド ハンドル
	 * @param	repeat [入力] ループ有無
	 */
	void MciPlaySound(int* sound_id, int repeat)
	{
		MciSoundInfo* sound_info;
		DWORD_PTR mci_command;
		MCI_PLAY_PARMS play_param;

		if (!sound_id) {
			return;
		}
		sound_info = (MciSoundInfo *)sound_id;
		if (!sound_info->device_id) {
			return;
		}
		sound_info->repeat = repeat;
		ZeroMemory(&play_param, sizeof(play_param));
		mci_command = 0;
		if (repeat) {
			switch (sound_info->device_type) {
			case MCI_DEVTYPE_DIGITAL_VIDEO:
				mci_command |= (MCI_FROM | MCI_DGV_PLAY_REPEAT);
				play_param.dwFrom = 0;
				break;
			case MCI_DEVTYPE_SEQUENCER:
			case MCI_DEVTYPE_WAVEFORM_AUDIO:
				break;
			default:
				break;
			}
		}
		mciSendCommand(sound_info->device_id, MCI_SEEK, MCI_SEEK_TO_START, 0);
		mciSendCommand(sound_info->device_id, MCI_PLAY, mci_command, (DWORD_PTR)&play_param);
	}

	/**
	 * @brief	サウンド再生を停止する
	 *
	 * @param	sound_id [入力] サウンド ハンドル
	 */
	void MciStopSound(int* sound_id)
	{
		MciSoundInfo* sound_info;

		if (!sound_id) {
			return;
		}
		sound_info = (MciSoundInfo *)sound_id;
		if (!sound_info->device_id) {
			return;
		}
		sound_info->repeat = 0;
		mciSendCommand(sound_info->device_id, MCI_STOP, MCI_WAIT, 0);
		mciSendCommand(sound_info->device_id, MCI_SEEK, MCI_SEEK_TO_START, 0);
	}

	/**
	 * @brief	サウンド再生状態の取得
	 *
	 * @param	sound_id [入力] サウンド ハンドル
	 *
	 * @return	再生中ならば 0 以外を返す
	 */
	int MciCheckSound(int* sound_id)
	{
		MciSoundInfo* sound_info;
		MCI_STATUS_PARMS status_param;

		if (!sound_id) {
			return 0;
		}
		sound_info = (MciSoundInfo *)sound_id;
		if (!sound_info->device_id) {
			return 0;
		}
		ZeroMemory(&status_param, sizeof(status_param));
		status_param.dwItem = MCI_STATUS_MODE;
		if (mciSendCommand(sound_info->device_id, MCI_STATUS, MCI_STATUS_ITEM, (DWORD_PTR)&status_param)) {
			return 0;
		}
		return status_param.dwReturn == MCI_MODE_PLAY;
	}

	/**
	 * @brief	ループ再生の強制更新
	 *
	 * @param	sound_id [入力] サウンド ハンドル
	 *
	 * @note
	 *	サウンドが停止したら同じサウンドを再生する
	 *	更新時は音量設定が標準値に戻るので再設定を行う必要がある
	 */
	void MciUpdateSound(int* sound_id)
	{
		MciSoundInfo* sound_info;
		MCI_STATUS_PARMS status_param;

		if (!sound_id) {
			return;
		}
		sound_info = (MciSoundInfo *)sound_id;
		if (!sound_info->device_id || !sound_info->repeat) {
			return;
		}

		switch (sound_info->device_type) {
		case MCI_DEVTYPE_DIGITAL_VIDEO:
			break;
		case MCI_DEVTYPE_SEQUENCER:
		case MCI_DEVTYPE_WAVEFORM_AUDIO:
			ZeroMemory(&status_param, sizeof(status_param));
			status_param.dwItem = MCI_STATUS_MODE;
			if (!mciSendCommand(sound_info->device_id, MCI_STATUS, MCI_STATUS_ITEM, (DWORD_PTR)&status_param)) {
				if (status_param.dwReturn == MCI_MODE_NOT_READY) {
				}
				else if (status_param.dwReturn == MCI_MODE_STOP) {
					if (sound_info->repeat > 0) {
						mciSendCommand(sound_info->device_id, MCI_SEEK, MCI_SEEK_TO_START, 0);
						mciSendCommand(sound_info->device_id, MCI_PLAY, 0, 0);
					}
				}
			}
			break;
		default:
			break;
		}
	}


	/**
	 * @brief	再生音量を設定する
	 *
	 * @param	sound_id [入力] サウンド ハンドル
	 * @param	percent [入力] 音量 (0 〜 100)
	 */
	void MciSetVolume(int* sound_id, int percent)
	{
		MciSoundInfo *sound_info;
		MCI_DGV_SETAUDIO_PARMS	audio_param;
		DWORD volume;

		if (!sound_id) {
			return;
		}
		sound_info = (MciSoundInfo *)sound_id;
		if (!sound_info->device_id) {
			return;
		}
		switch (sound_info->device_type) {
		case MCI_DEVTYPE_DIGITAL_VIDEO:
			ZeroMemory(&audio_param, sizeof(audio_param));
			audio_param.dwItem = MCI_DGV_SETAUDIO_VOLUME;
			audio_param.dwValue = percent * 10;
			mciSendCommand(sound_info->device_id, MCI_SETAUDIO, MCI_DGV_SETAUDIO_ITEM | MCI_DGV_SETAUDIO_VALUE, (DWORD_PTR)&audio_param);
			break;
		case MCI_DEVTYPE_SEQUENCER:
			volume = 0x0ffff * percent / 100;
			midiOutSetVolume(0, (DWORD)MAKELONG(volume, volume));
			break;
		case MCI_DEVTYPE_WAVEFORM_AUDIO:
			volume = 0x0ffff * percent / 100;
			waveOutSetVolume(0, (DWORD)MAKELONG(volume, volume));
			break;
		default:
			break;
		}
	}

#ifdef INFO_EX
	//ウィンドウサイズを固定する
	void FixWin(void)
	{
		//ウィンドウサイズ変更禁止
		HWND hCon = GetConsoleWindow();
		LONG lStyle = GetWindowLong(hCon, GWL_STYLE);
		lStyle &= ~(WS_THICKFRAME | WS_HSCROLL | WS_VSCROLL);	//'~'ビットごとの反転(１の補数)
		lStyle = SetWindowLong(hCon, GWL_STYLE, lStyle);
		//SetWindowPos(hCon, NULL, 0, 0, frmb.width + 20, frmb.height, SWP_NOSIZE | SWP_NOZORDER);
	}
	/*
		パレットデータ（16色）を設定する
		const COLORREF* p_cr16: 転送元パレットデータ「１６色」へのポインタ
		unsigned int start:開始インデックス
		unsigned int count:転送数
	*/
	void SetConsolePaletteRange(const COLORREF* p_cr16, unsigned int start, unsigned int count)
	{
		if (start >= NUM_PALETTE) {
			//最大値を超えていたら何もしない
			return;
		}
		//転送インデックス最大値＋１を求める
		int max = (start + count);
		if (max > NUM_PALETTE) {
			//最大値未満にする
			max = NUM_PALETTE;
		}
		//使用する構造体のサイズを設定する
		screen_buffer_info_ex.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
		//開始位置から終了位置まで色情報をコピーする
		for (int n = start; n < max; n++) {
			screen_buffer_info_ex.ColorTable[n] = p_cr16[n];
		}
		//ウィンドウサイズが違っているかもしれないので補正しておく
		screen_buffer_info_ex.srWindow.Right = screen_buffer_info_ex.dwMaximumWindowSize.X;
		screen_buffer_info_ex.srWindow.Bottom = screen_buffer_info_ex.dwMaximumWindowSize.Y;
		//ディスプレイハンドル毎にパレット設定する
		if (display_handle[0]) {
			//ディスプレイバッファ（ハンドル）＃０のパレット情報を設定する
			SetConsoleScreenBufferInfoEx(display_handle[0], &screen_buffer_info_ex);
		}
		if (display_handle[1]) {
			//ディスプレイバッファ（ハンドル）＃１のパレット情報を設定する
			SetConsoleScreenBufferInfoEx(display_handle[1], &screen_buffer_info_ex);
		}
	}
	//
	void GetConsolePaletteRange(COLORREF* p_cr16, unsigned int start, unsigned int count)
	{
		if (start >= NUM_PALETTE) {
			return;
		}
		int max = (start + count);
		if (max > NUM_PALETTE) {
			max = NUM_PALETTE;
		}
		//
		screen_buffer_info_ex.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
		if (display_handle[swap_flg]) {
			GetConsoleScreenBufferInfoEx(display_handle[swap_flg], &screen_buffer_info_ex);
			for (int n = start; n < max; n++) {
				p_cr16[n] = screen_buffer_info_ex.ColorTable[n];
			}
		}
	}
	//パレットに１６色を設定する
	void SetConsolePalette(void) {
		SetConsolePaletteRange(g_ConsoleColorTable, 0, NUM_PALETTE);
	}
	//パレットから１６色を取得する
	void GetConsolePalette(void) {
		GetConsolePaletteRange(g_ConsoleColorTable, 0, NUM_PALETTE);
	}
	//RGBQUAD:{B,G,B,0}型をCOLORREF:0x00BBGGRR型に変換
	COLORREF RGBQtoCREF(RGBQUAD rgb) {
		return (rgb.rgbRed & 0x0000FF) | ((rgb.rgbGreen << 8) & 0x00FF00) | ((rgb.rgbBlue << 16) & 0xFF0000);
	}
	//COLORREF:0x00BBGGRR型をRGBQUAD:{B,G,B,0}型に変換
	RGBQUAD CREFtoRGBQ(COLORREF ref) {
		RGBQUAD rgb = { (ref & 0x00FF0000) >> 16/*Blue*/,(ref & 0x0000FF00) >> 8/*Green*/,(ref & 0x000000FF)/*Red*/,0x00/*Reserved*/ };
		return rgb;
	}
	//パレット配列指定数分コピー：RGBQUAD型配列からCOLORREF型配列へコピー
	void CopyPalette_N(const RGBQUAD* sourcePAL, int start_num, int num_palette) {
		for (int n = 0; n < num_palette; n++) {
			g_ConsoleColorTable[n] = RGBQtoCREF(sourcePAL[n]);
		}
	}
	//パレット配列指定数分コピー：RGBQUAD型配列からCOLORREF型配列へコピー
	void CopyPaletteQ2R_N(COLORREF* destPAL, const RGBQUAD* sourcePAL, int start_num, int num_palette) {
		for (int n = 0; n < num_palette; n++) {
			destPAL[n] = RGBQtoCREF(sourcePAL[n]);
		}
	}
	//パレット配列指定数分コピー：COLORREF型配列からRGBQUAD型配列へコピー
	void CopyPaletteR2Q_N(RGBQUAD* destPAL, const COLORREF* sourcePAL, int start_num, int num_palette) {
		for (int n = 0; n < num_palette; n++) {
			destPAL[n] = CREFtoRGBQ(sourcePAL[n]);
		}
	}
	//パレット配列全てコピー：RGBQUAD型配列からConsoleColorTable配列へコピー
	void CopyPalette(const RGBQUAD* sourcePAL) {
		if (sourcePAL == NULL) {
			return;
		}
		CopyPalette_N(sourcePAL, 0, NUM_PALETTE);
	}
	//パレット配列全てコピー：RGBQUAD型配列からCOLORREF型配列へコピー
	void CopyPaletteQ2R(COLORREF* destPAL, const RGBQUAD* sourcePAL) {
		if (sourcePAL == NULL) {
			return;
		}
		if (destPAL == NULL) {
			return;
		}
		CopyPaletteQ2R_N(destPAL, sourcePAL, 0, NUM_PALETTE);
	}
	//パレット配列全てコピー：COLORREF型配列からRGBQUAD型配列へコピー
	void CopyPaletteR2Q(RGBQUAD* destPAL, const COLORREF* sourcePAL) {
		if (sourcePAL == NULL) {
			return;
		}
		if (destPAL == NULL) {
			return;
		}
		CopyPaletteR2Q_N(destPAL, sourcePAL, 0, NUM_PALETTE);
	}
#endif // INFO_EX

#ifdef __cplusplus
}
#endif

/**
 * @copyright (c) 2018-2020 HAL Osaka College of Technology & Design (Ihara, H.)
 */
