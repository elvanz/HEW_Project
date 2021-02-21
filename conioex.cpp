/**
 * @file	conioex.cpp
 * @brief	Win32 �R���\�[�� I/O �g��
 *
 * @date	2020/09/18 2020�N�x����
 * @date	2020/10/09 �s��C��
 * @version	1.08
 *          2020/09/24 �V���O���o�b�t�@�g�p���ɃE�B���h�E�T�C�Y��
 *                     ���f����Ȃ����̍ďC��
 *          2020/10/09 �o�b�t�@�T�C�Y��1�����Ȃ肷���Ă�����̏C��
 * @note
 *  �R���\�[���E�B���h�E�̃v���p�e�B�̐ݒ���ȉ��ɕύX���邱��
 *  �E�u�]���̃R���\�[�����g���v�̃`�F�b�N���O��
 *  �E�u�ȈՕҏW���[�h�v�̃`�F�b�N���O��
 */
#ifdef __cplusplus
extern "C" {
#endif

#include "conioex.h"

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
DWORD	conio_key_map[8] = {0, 0, 0, 0, 0, 0, 0, 0};
COORD	conio_mouse_position = {0, 0};
COORD	screen_buffer_size;
WORD	screen_buffer_attribute = LIGHTGRAY;
CONSOLE_SCREEN_BUFFER_INFO screen_buffer_info;
CONSOLE_FONT_INFOEX font_size = { sizeof font_size };
SMALL_RECT window_size;
int swap_flg = 0;

CHAR_INFO* screen_buffer_16;
char* screen_buffer_full;

/**
 * @brief	�R���\�[�� I/O ������
 *
 * @param	width [����] �R���\�[���E�B���h�E�̉��T�C�Y(1�`)
 * @param	height [����] �R���\�[���E�B���h�E�̏c�T�C�Y(1�`)
 */
void InitConio(int width, int height)
{
	CONSOLE_HISTORY_INFO history_info;
	char *out_buf;

	// �f�B�X�v���C���擾
	display_handle[0] = GetStdHandle(STD_OUTPUT_HANDLE);
	display_handle[1] = NULL;
	input_handle = GetStdHandle(STD_INPUT_HANDLE);

	// �R���\�[���E�B���h�E�̃��[�h�ݒ�
	SetConsoleMode(display_handle[0], CONSOLE_OUTPUT_MODE);
	SetConsoleMode(input_handle, CONSOLE_INPUT_MODE);

	// �R�}���h������ۑ����Ȃ�
	history_info.cbSize = sizeof(CONSOLE_HISTORY_INFO);
	history_info.HistoryBufferSize = 0;
	history_info.NumberOfHistoryBuffers = 0;
	history_info.dwFlags = 0;
	SetConsoleHistoryInfo(&history_info);

	// �E�B���h�E�T�C�Y�ύX
	window_size.Left = 0;
	window_size.Top = 0;
	window_size.Right = width - 1;
	window_size.Bottom = height - 1;
	SetConsoleWindowInfo(display_handle[0], TRUE, &window_size);

	// �o�b�t�@�T�C�Y�ύX
	screen_buffer_size.X = width;
	screen_buffer_size.Y = height;
	SetConsoleScreenBufferSize(display_handle[0], screen_buffer_size);

	// �t�H���g�T�C�Y�ύX
	font_size.dwFontSize.X = DEF_FONTSIZE_X;
	font_size.dwFontSize.Y = DEF_FONTSIZE_Y;
	SetCurrentConsoleFontEx(display_handle[0], TRUE, &font_size);

	// �E�B���h�E�T�C�Y�ύX
	window_size.Left = 0;
	window_size.Top = 0;
	window_size.Right = width - 1;
	window_size.Bottom = height - 1;
	SetConsoleWindowInfo(display_handle[0], TRUE, &window_size);

	// �X�N���[���o�b�t�@�̏����擾
	GetConsoleScreenBufferInfo(display_handle[0], &screen_buffer_info);

	// �X�N���[���o�b�t�@(16�F�p)�𐶐�
	screen_buffer_16 = (CHAR_INFO*)malloc(sizeof(CHAR_INFO) * width * height);
	memset(screen_buffer_16, 0, width * height);

	// �X�N���[���o�b�t�@(�t���J���[�p)�𐶐�
	screen_buffer_full = (char *)malloc(sizeof(char) * (((width * 20) * height) + height));

	// �t���J���[�p�ɃG�X�P�[�v�V�[�P���X�������ݒ�
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
}

/**
 * @brief	�_�u���o�b�t�@������
 *
 * @retval	0	����I��
 * @retval	-1	�o�b�t�@�n���h���擾���s
 */
int InitDoubleBuffer(void)
{
	// �_�u���o�b�t�@�p�̃������[���m��
	display_handle[0] = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	if (display_handle[0] == INVALID_HANDLE_VALUE) {
		printf("�_�u���o�b�t�@1�̃n���h���擾�Ɏ��s���܂���\n");
		return -1;
	}

	display_handle[1] = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	if (display_handle[1] == INVALID_HANDLE_VALUE) {
		printf("�_�u���o�b�t�@2�̃n���h���擾�Ɏ��s���܂���\n");
		return -1;
	}

	// �o�b�t�@���㏑�����[�h��
	SetConsoleMode(display_handle[0], CONSOLE_OUTPUT_MODE);
	SetConsoleMode(display_handle[1], CONSOLE_OUTPUT_MODE);

	// �E�B���h�E�T�C�Y�ύX
	SetConsoleWindowInfo(display_handle[0], TRUE, &window_size);
	SetConsoleWindowInfo(display_handle[1], TRUE, &window_size);

	// �o�b�t�@�T�C�Y�ύX
	SetConsoleScreenBufferSize(display_handle[0], screen_buffer_size);
	SetConsoleScreenBufferSize(display_handle[1], screen_buffer_size);

	// �t�H���g�T�C�Y�ύX
	SetCurrentConsoleFontEx(display_handle[0], TRUE, &font_size);
	SetCurrentConsoleFontEx(display_handle[1], TRUE, &font_size);

	// �E�B���h�E�T�C�Y�ύX
	SetConsoleWindowInfo(display_handle[0], TRUE, &window_size);
	SetConsoleWindowInfo(display_handle[1], TRUE, &window_size);

	return 0;
}

/**
 * @brief	�t�H���g�T�C�Y�ύX
 *
 * @param	width [����] �t�H���g�̉��T�C�Y(1�`)
 * @param	height [����] �t�H���g�̏c�T�C�Y(1�`)
 */
void SetScreenFontSize(int width, int height)
{
	// �t�H���g�T�C�Y�ύX
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
 * @brief	���������̃J�[�\���ʒu���擾
 *
 * @return	���݂̃J�[�\���ʒu��X���W(1�`)
 */
int GetCursorX(void)
{
	return screen_buffer_info.dwCursorPosition.X - screen_buffer_info.srWindow.Left + 1;
}

/**
 * @brief	���������̃J�[�\���ʒu���擾
 *
 * @return	���݂̃J�[�\���ʒu��Y���W(1�`)
 */
int GetCursorY(void)
{
	return screen_buffer_info.dwCursorPosition.Y - screen_buffer_info.srWindow.Top + 1;
}

/**
 * @brief	�J�[�\���ʒu�̈ړ�
 *
 * @param	x [����] X���W(1�`)
 * @param	y [����] Y���W(1�`)
 */
void SetCursorPosition(int x, int y)
{
	COORD lc;

	lc.X = x - 1;
	lc.Y = screen_buffer_info.srWindow.Top + y - 1;

	SetConsoleCursorPosition(display_handle[swap_flg], lc);
}

/**
 * @brief	������̏o��
 *
 * @param	buf [����] �o�͕�����z��̃|�C���^
 * @param	size [����] �o�͕�����
 */
void PrintString(const char* buf, int size)
{
	DWORD write_num;

	WriteConsole(display_handle[swap_flg], buf, size, &write_num, NULL);
}

/**
 * @brief	�摜�̏o��
 *
 * @param	buf [����] RGB�摜�f�[�^�z��̃|�C���^
 * @note
 *  RGB�摜�f�[�^�z��̓X�N���[���̉����~�c���̃o�C�g���ȏ�̔z��Ƃ��A
 *  �z��̒��g��RGB�e1�o�C�g(���v3�o�C�g)��1��f�Ƃ����f�[�^�ɂ���B
 *  �S�Ẳ�f�͘A�����Ă���K�v����B
 *  ��)��80�����~�c25�s�̏ꍇ�A80x25=200�o�C�g�ȏ�̔z���n��
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
			// R�ݒ�
			*(out_buf+0) = 0x30 + (*in_buf / 100);
			*(out_buf+1) = 0x30 + (*in_buf % 100 / 10);
			*(out_buf+2) = 0x30 + (*in_buf % 10);
			in_buf++;
			// G�ݒ�
			*(out_buf+4) = 0x30 + (*in_buf / 100);
			*(out_buf+5) = 0x30 + (*in_buf % 100 / 10);
			*(out_buf+6) = 0x30 + (*in_buf % 10);
			in_buf++;
			// B�ݒ�
			*(out_buf+8) = 0x30 + (*in_buf / 100);
			*(out_buf+9) = 0x30 + (*in_buf % 100 / 10);
			*(out_buf+10) = 0x30 + (*in_buf % 10);
			in_buf++;
			out_buf += 13;
		}
		out_buf++;
	}
	WriteConsole(display_handle[swap_flg], screen_buffer_full, screen_buffer_size.Y * (screen_buffer_size.X * 20) + (screen_buffer_size.Y - 1), &write_num, NULL);
}

/**
 * @brief	�s���܂ŏ���
 */
void ClearLine(void)
{
	DWORD fill_num;

	FillConsoleOutputAttribute(	display_handle[swap_flg], 
								screen_buffer_info.wAttributes,
								screen_buffer_info.srWindow.Right - screen_buffer_info.dwCursorPosition.X + 1,
								screen_buffer_info.dwCursorPosition,
								&fill_num );
	FillConsoleOutputCharacter(	display_handle[swap_flg],
								TEXT(' '),
								screen_buffer_info.srWindow.Right - screen_buffer_info.dwCursorPosition.X + 1,
								screen_buffer_info.dwCursorPosition,
								&fill_num );
}

/**
 * @brief	��ʏ���
 */
void ClearScreen(void)
{
	DWORD fill_num;
	COORD screen_origin = { 0, 0 };

	screen_buffer_attribute = screen_buffer_info.wAttributes;

	FillConsoleOutputAttribute(	display_handle[swap_flg],
								screen_buffer_info.wAttributes,
								screen_buffer_info.dwSize.X * screen_buffer_info.dwSize.Y,
								screen_origin,
								&fill_num );
	FillConsoleOutputCharacter(	display_handle[swap_flg],
								TEXT(' '),
								screen_buffer_info.dwSize.X * screen_buffer_info.dwSize.Y,
								screen_origin,
								&fill_num);
	SetCursorPosition(screen_buffer_info.srWindow.Left + 1, screen_buffer_info.srWindow.Top + 1);
}

/**
 * @brief	�_�u���o�b�t�@���̕`��ʐؑ�
 */
void FlipScreen(void)
{
	SetConsoleActiveScreenBuffer(display_handle[swap_flg]);	// �o�b�t�@�����ւ��\��
	swap_flg = (swap_flg) ? 0 : 1;
}

/**
 * @brief	�����F���P�x��
 */
void SetHighVideoColor(void)
{
	SetConsoleTextAttribute(display_handle[swap_flg], screen_buffer_info.wAttributes |= FOREGROUND_INTENSITY);
}

/**
 * @brief	�����F��P�x��
 */
void SetLowVideoColor(void)
{
	SetConsoleTextAttribute(display_handle[swap_flg], screen_buffer_info.wAttributes &= ~FOREGROUND_INTENSITY);
}

/**
 * @brief	���蕶���F�ݒ�
 */
void SetNormalVideoColor(void)
{
	SetConsoleTextAttribute(display_handle[swap_flg], LIGHTGRAY);
}

/**
 * @brief	�����w�i�F�ݒ�
 *
 * @param	color [����] �����w�i�F 
 * @note
 *  �w�i�F��enum COLORS���Q�Ƃ���
 */
void SetTextBackColor(int color)
{
	screen_buffer_info.wAttributes &= ~0x00f0;
	screen_buffer_info.wAttributes |= ((color & 0x07) << 4);
	SetConsoleTextAttribute(display_handle[swap_flg], screen_buffer_info.wAttributes);
}

/**
 * @brief	�����F�ݒ�
 *
 * @param	color [����] �����F
 * @note
 *  �����F��enum COLORS���Q�Ƃ���
 */
void SetConsoleTextColor(int color)
{
	screen_buffer_info.wAttributes &= ~0x000f;
	screen_buffer_info.wAttributes |= (color & 0x0f);
	SetConsoleTextAttribute(display_handle[swap_flg], screen_buffer_info.wAttributes);
}

/**
 * @brief	�����F�w�i�F�����ݒ�
 *
 * @param	attribute [����] �����w�i���
 * @note
 *  �ȉ��̊e�ݒ�l���r�b�g����OR���Z��p���Ĉ����Ɏw�肷��
 *	  FOREGROUND_BLUE            0x0001 // text color contains blue.
 *	  FOREGROUND_GREEN           0x0002 // text color contains green.
 *	  FOREGROUND_RED             0x0004 // text color contains red.
 *	  FOREGROUND_INTENSITY       0x0008 // text color is intensified.
 *	  BACKGROUND_BLUE            0x0010 // background color contains blue.
 *	  BACKGROUND_GREEN           0x0020 // background color contains green.
 *	  BACKGROUND_RED             0x0040 // background color contains red.
 *	  BACKGROUND_INTENSITY       0x0080 // background color is intensified.
 *	  COMMON_LVB_LEADING_BYTE    0x0100 // Leading Byte of DBCS
 *	  COMMON_LVB_TRAILING_BYTE   0x0200 // Trailing Byte of DBCS
 *	  COMMON_LVB_GRID_HORIZONTAL 0x0400 // DBCS: Grid attribute: top horizontal.
 *	  COMMON_LVB_GRID_LVERTICAL  0x0800 // DBCS: Grid attribute: left vertical.
 *	  COMMON_LVB_GRID_RVERTICAL  0x1000 // DBCS: Grid attribute: right vertical.
 *	  COMMON_LVB_REVERSE_VIDEO   0x4000 // DBCS: Reverse fore/back ground attribute.
 *	  COMMON_LVB_UNDERSCORE      0x8000 // DBCS: Underscore.
 *	  COMMON_LVB_SBCSDBCS        0x0300 // SBCS or DBCS flag.
 */
void SetTextAttribute(int attribute)
{
	SetConsoleTextAttribute(display_handle[swap_flg], attribute);
}

/**
 * @brief	�J�[�\���^�C�v�ݒ�
 *
 * @param	type [����] NOCURSOR �J�[�\���\���Ȃ�
 *                      SOLIDCURSOR (��Ή�)
 *                      NORMALCURSOR �J�[�\���̒ʏ�\��
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
	cursor_info.dwSize		= (type < NORMALCURSOR) ? 100 : size;
	cursor_info.bVisible	= (type != NOCURSOR);
	SetConsoleCursorInfo(display_handle[swap_flg], &cursor_info);
}

/**
 * @brief	���ݍs�ɑ}��
 */
void InsertLine(void)
{
	COORD	lc;
	DWORD	len;
	DWORD	num;
	LPTSTR	psz;
	LPWORD	pw;

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
}

/**
 * @brief	���ݍs�̍폜
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
}

/**
 * @brief	������̈ꊇ�]��
 *
 * @param	buf [����] �X�N���[���o�b�t�@�̃|�C���^
 */
void PrintFrameBuffer(char* buf)
{
	CHAR_INFO *char_info = screen_buffer_16;

	// �摜�`��(��ʊO�ւ̂͂ݏo�������Ȃ�)
	for (int count = 0; count < screen_buffer_size.Y * screen_buffer_size.X; count++) {
		// �w�i�F��4�r�b�g���V�t�g���ăZ�b�g
		char_info->Attributes = (*buf & 0x0F) | ((*buf << 4) & 0xF0);
		char_info++;
		buf++;
	}

	// ��ʂ̈ꊇ�`��
	WriteConsoleOutputA(display_handle[swap_flg], screen_buffer_16, screen_buffer_size, { 0,0 }, &window_size);
}

/**
 * @brief	�ꊇ�]���p�o�b�t�@�N���A
 *
 * @param	buf [����] �X�N���[���o�b�t�@�̃|�C���^
 * @note
 *  �X�N���[���o�b�t�@�̓E�B���h�E�T�C�Y�̉����~�c����
 *  ���v�o�C�g�T�C�Y�ȏ��char�^�z��Ƃ���
 *  �X�N���[���o�b�t�@�̓��e��S��0�ŃN���A����
 */
void ClearFrameBuffer(char* buf)
{
	ZeroMemory(buf, sizeof(char) * screen_buffer_size.X * screen_buffer_size.Y);
}

/**
 * @brief	�R���\�[���E�B���h�E�̃^�C�g���o�[�ɕ\�������e�L�X�g��ݒ�
 *
 * @param	title [����] �E�B���h�E�^�C�g���ɕ\������e�L�X�g
 */
void SetCaption(const char* title)
{
	SetConsoleTitleA(title);
}

/**
 * @brief	�R���\�[���E�B���h�E�̃^�C�g���o�[�ɕ\�������e�L�X�g���擾
 *
 * @param	title [�o��] ���݂̃E�B���h�E�^�C�g���̃e�L�X�g
 * @param	len [����] �E�B���h�E�^�C�g���̕�����
 *
 * @retval	��0	���݂̃E�B���h�E�^�C�g���̕�����
 * @retval	0	�G���[
 */
int GetCaption(char* title, int len)
{
	return GetConsoleTitleA(title, len);
}

/**
 * @brief	�L�[��񃊃Z�b�g
 */
void ResetKeyMap(void)
{
	for (int count = 0; count < 8; count++) {
		conio_key_map[count] = 0;
	}
}

/**
 * @brief	�W���C�p�b�h����
 *
 * @param	port [����] �|�[�g�ԍ�(P*_*)
 *
 * @retval	0		����I��
 * @retval	1�ȏ�	Joystick�̓��͒l
 * @retval	-1		�G���[
 */
int InputJoystick(int port)
{
	JOYINFO	joy_info;
	int id;
	int func;

	// �Q�[���p�b�h����
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
 * @brief	�W���C�p�b�h�ڍד���
 *
 * @param	port [����] �R���g���[���ԍ�
 * @param	port [����] �|�[�g�ԍ�(P*_*)
 *
 * @retval	0		����I��
 * @retval	1�ȏ�	Joystick�̓��͒l
 * @retval	-1		�G���[
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
 * @brief	�W���C�p�b�h����(XInput�Ή�)
 *
 * @param	port [����] �|�[�g�ԍ�(P*_*)
 *
 * @retval	0		����I��
 * @retval	1�ȏ�	Joystick�̓��͒l
 * @retval	-1		�G���[
 *
 * @note
 *	LR�̃X�e�B�b�N�͒��S���獶�E�Ɉړ�����ۂ̃f�b�h�]�[�����p�ӂ���Ă���
 *	�i�f�b�h�]�[���͍��E�Ɉړ������Ƃ݂Ȃ��Ȃ��G���A�̂��Ɓj
 *	 #define XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE  7849
 *	 #define XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE 8689
 */
int InputJoystickX(int id, int port)
{
	XINPUT_STATE controller_state;	// XInput�R���g���[�����

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
		return controller_state.Gamepad.wButtons;
	}
	return 0;
}

/**
 * @brief	�L�[�{�[�h�E�}�E�X����
 *
 * @param	port [����] �|�[�g�ԍ�(P*_*)
 * @return	���͒l
 */
int InputKeyMouse(int port)
{
	DWORD event = 0;
	DWORD read = 0;
	PINPUT_RECORD input_record;
	KEY_EVENT_RECORD* key_event;
	MOUSE_EVENT_RECORD* mouse_event;

	// �L�[�{�[�h�C�x���g�`�F�b�N
	if (GetNumberOfConsoleInputEvents(input_handle, &event) && event) {
		read = 0;
		input_record = (PINPUT_RECORD)_alloca(event * sizeof(INPUT_RECORD));

		if (ReadConsoleInput(input_handle, input_record, event, &read) && read) {
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

	// �}�E�X���W��Ԃ�
	switch (port) {
	case PM_CURX:
		return conio_mouse_position.X + 1;
	case PM_CURY:
		return conio_mouse_position.Y + 1;
	default:
		break;
	}
	// �L�[��Ԃ�Ԃ�
	return (conio_key_map[(port & 0x0FF) >> 5] & (0x01 << (port & 31))) != 0;
}

/**
 * @brief	�T�E���h �t�@�C�����J��
 *
 * @param	path [����] �t�@�C����
 *
 * @retval	��0	�T�E���h �n���h��
 * @retval	0	�G���[
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
		// MciParam.wave_param.dwBufferSeconds  = 60;
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
 * @brief	�T�E���h �t�@�C�������
 *
 * @param	sound_id [����] �T�E���h �n���h��
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
 * @brief	�T�E���h���Đ�����
 *
 * @param	sound_id [����] �T�E���h �n���h��
 * @param	repeat [����] ���[�v�L��
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
 * @brief	�T�E���h�Đ����~����
 *
 * @param	sound_id [����] �T�E���h �n���h��
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
 * @brief	�T�E���h�Đ���Ԃ̎擾
 *
 * @param	sound_id [����] �T�E���h �n���h��
 *
 * @return	�Đ����Ȃ�� 0 �ȊO��Ԃ�
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
 * @brief	���[�v�Đ��̋����X�V
 *
 * @param	sound_id [����] �T�E���h �n���h��
 *
 * @note
 *  �T�E���h����~�����瓯���T�E���h���Đ�����
 *  �X�V���͉��ʐݒ肪�W���l�ɖ߂�̂ōĐݒ���s���K�v������
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
 * @brief	�Đ����ʂ�ݒ肷��
 *
 * @param	sound_id [����] �T�E���h �n���h��
 * @param	percent [����] ���� (0 �` 100)
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

#ifdef __cplusplus
}
#endif

/**
 * @copyright (c) 2018-2020 HAL Osaka College of Technology & Design (Ihara, H.)
 */
