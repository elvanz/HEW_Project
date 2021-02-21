//################################################################
//	�w��̃t�H���g���g���Ă��̃t�H���g�Ő������ꂽ������C���[�W���r�b�g�}�b�v�摜�f�[�^�ɕϊ�����B
//	�e�����̓r�b�g�}�b�v�摜�ɕϊ�����BmpChar�\���̂Ɋi�[�����B
//	BmpChar_Create�֐����Ăяo���ƁABmpChar�\���̂��w�肳�ꂽ��������BmpChar�̔z��Ƃ��Đ�������A���̃|�C���^���Ԃ����B
//
//	2021/01/18 : N.Ohmoto
//################################################################
#include "conioex.h"
#include "BmpChar.h"

void ConvBpp1ToB(BmpChar* _pbc, GLYPHMETRICS* _pgm);
void DrawBmpChar(BmpChar* _pbc, GLYPHMETRICS* _pgm, TEXTMETRIC* _ptxm);

//================================================================
//	1bpp�̉摜��8bpp�̉摜�ɕϊ�����B
//arg:
//	BmpChar* _pbc : �r�b�g�}�b�v�����f�[�^�ւ̃|�C���^
//	GLYPHMETRICS* _pgm : �ϊ��������̃O���t���
//return:
//	�Ȃ�
//================================================================
void ConvBpp1ToB(BmpChar* _pbc, GLYPHMETRICS* _pgm)
{
	int w_pix = _pgm->gmBlackBoxX;
	int h_pix = _pgm->gmBlackBoxY;
	int stride = (_pbc->buf_size / _pgm->gmBlackBoxY);
	int stride4 = (w_pix + 0b0011)& (~0b0011);			//�Wbpp�摜�̂S�o�C�g���E�̃o�C�g��
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
	//�Â�1bpp�̃o�b�t�@�͍폜���ĐV���������8bpp�o�b�t�@�ɓ���ւ���B
	free(_pbc->pPix);
	_pbc->pPix = pFontBitmap;
	_pbc->buf_size = bits_size;
}	//ConvBpp1ToB

//================================================================
//	�r�b�g�}�b�v�����P�����̕\���ʒu�𒲐����ăr�b�g�}�b�v����蒼���B
//arg:
//	BitmapChar* _pbc : �r�b�g�}�b�v�����̃|�C���^�B���̃|�C���^���w���r�b�g�}�b�v�����f�[�^�̕\���ʒu�𒲐����ăo�b�t�@����蒼�����B
//	GLYPHMETRICS* _pgm : �ϊ��������̃O���t���
//	TEXTMETRIC* _ptxm : �ϊ����t�H���g�̌v���i�����̐��@�j���
//return:
//	����
//================================================================
void DrawBmpChar(BmpChar* _pbc, GLYPHMETRICS* _pgm, TEXTMETRIC* _ptxm)
{
	int	dest_width = _pgm->gmCellIncX;
	int dest_height = _ptxm->tmHeight;
	int dest_buf_size = dest_width * dest_height;
	char* pDest = (char*)malloc(dest_buf_size);
	ZeroMemory(pDest, dest_buf_size);
	int width = _pgm->gmBlackBoxX;
	int widthBytes = (width + 0b0011)& (~0b0011);	//�����̃o�C�g���͂S�̔{���ɍ��킹��
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
//	�w��̃t�H���g�ŏo�����r�b�g�}�b�v��������쐬���A���̃|�C���^��Ԃ��B
//args:
//	const char* _font_name : �t�H���g��
//	int _font_size : �t�H���g�E�T�C�Y
//	int _bold : �����w��Ftrue�ő���
//	int _ggo : �A���`�E�G�C���A�X���~���w��
//	const wchar_t* _wtext : �ϊ�������������i���C�h�����j
//return:
//	BitmapChar* : �ϊ���̃r�b�g�}�b�v�����̔z��ւ̃|�C���^�B�Ō�̕�����
//================================================================
BmpChar* BmpChar_Create(const char* _font_name, int _font_size, int _bold, int _ggo, const wchar_t* _wtext)
{
	//��]�s��
	MAT2	mat2{ {0,1},{0,0},{0,0},{0,1} };
	//�t�H���g�̐ݒ�`�쐬
	LOGFONT	lf;
	lf.lfHeight = _font_size;
	lf.lfWidth = 0;
	lf.lfEscapement = 0;	//�����P�ʂ̉�]�p�x�����
	lf.lfOrientation = 0;
	if (_bold)
	{
		lf.lfWeight = FW_BOLD;	//�����ݒ�
	}
	else
	{
		lf.lfWeight = FW_NORMAL;
	}
	lf.lfItalic = FALSE;	//�Α�
	lf.lfUnderline = FALSE;
	lf.lfStrikeOut = FALSE;
	lf.lfCharSet = ANSI_CHARSET;
	lf.lfOutPrecision = OUT_TT_PRECIS;	//OUT_DEFAULT_PRECIS
	lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lf.lfQuality = PROOF_QUALITY;
	lf.lfPitchAndFamily = (DEFAULT_PITCH | FF_MODERN);
	lf.lfFaceName[0] = '\0';
	//���̖����R�s�[�i��������������j
	CopyMemory(lf.lfFaceName, _font_name, LF_FACESIZE * sizeof(CHAR));
	//�t�H���g����
	HFONT hFont = CreateFontIndirect(&lf);
	if (hFont == NULL)
	{
		return	NULL;
	}
	// �f�o�C�X�Ƀt�H���g��I������
	HWND hWnd = GetConsoleWindow();
	HDC hdc = GetDC(hWnd);
	HFONT oldFont = (HFONT)SelectObject(hdc, hFont);
	//���������t�H���g�̌v���f�[�^���擾����
	TEXTMETRIC	txm;		//�ϊ������t�H���g�̏�������\����
	GetTextMetrics(hdc, &txm);	//�v���f�[�^���擾
	int aa_level = 2;
	switch (_ggo)
	{
	default:	//�Q�l
	case GGO_BITMAP:		aa_level = 2;		break;	//�Q�l
	case GGO_GRAY2_BITMAP:	aa_level = 5;		break;	//�T�K��
	case GGO_GRAY4_BITMAP:	aa_level = 17;	break;	//�P�V�K��
	case GGO_GRAY8_BITMAP:	aa_level = 65;	break;	//�U�T�K��
	}
	//�w��̃t�H���g�ŏo�����r�b�g�}�b�v�����ŕ�������쐬����B
	GLYPHMETRICS	gm;	//�O���t�ݒ�f�[�^
	UINT code;
	int length = 0;
	//������̕����������߂�B
	while (_wtext[length] != '\0') {
		length++;
	}
	BmpChar*	pBmpChr = (BmpChar*)malloc(sizeof(BmpChar) * (length + 1));
	ZeroMemory(pBmpChr, sizeof(BmpChar) * (length + 1));
	for (int txn = 0; txn < length; txn++) {
		code = (UINT)_wtext[txn];
		//���ꂩ�琶�����镶���r�b�g�}�b�v�f�[�^�̃o�C�g�����擾����B
		int buff_size = GetGlyphOutlineW(hdc, code, _ggo, &gm, 0, NULL, &mat2);
		if (buff_size > 0)
		{
			//�擾�����T�C�Y���̃o�b�t�@���m�ۂ���B
			pBmpChr[txn].pPix = (char*)malloc(buff_size);
			GetGlyphOutlineW(hdc, code, _ggo, &gm, buff_size, pBmpChr[txn].pPix, &mat2);
			if (_ggo == GGO_BITMAP)
			{
				//1bpp�̃r�b�g�}�b�v�͕\�����ɂ����̂łWbpp�ɕϊ�����B
				pBmpChr[txn].buf_size = buff_size;		//�o�b�t�@�T�C�Y
				ConvBpp1ToB(&pBmpChr[txn], &gm);
				buff_size = pBmpChr[txn].buf_size;
			}
			pBmpChr[txn].size.X = gm.gmBlackBoxX;	//���s�N�Z����
			pBmpChr[txn].size.Y = gm.gmBlackBoxY;	//�c�s�N�Z����
			pBmpChr[txn].buf_size = buff_size;		//�o�b�t�@�T�C�Y
			pBmpChr[txn].aaLv = aa_level;			//�A���`�G�C���A�X���~�����x��
			pBmpChr[txn].wch = code;				//�ϊ����̕����R�[�h
			//�����ʒu�𒲐����ăo�b�t�@����蒼���B
			DrawBmpChar(&pBmpChr[txn], &gm, &txm);
		}
	}
	return	pBmpChr;
}	//BmpChar_Create

//
// end of file.
//
