#pragma once
//################################################################
//	�w��̃t�H���g���g���Ă��̃t�H���g�Ő������ꂽ������C���[�W���r�b�g�}�b�v�摜�f�[�^�ɕϊ�����B
//	�e�����̓r�b�g�}�b�v�摜�ɕϊ�����BmpChar�\���̂Ɋi�[�����B
//	BmpChar_Create�֐����Ăяo���ƁABmpChar�\���̂��w�肳�ꂽ��������BmpChar�̔z��Ƃ��Đ�������A���̃|�C���^���Ԃ����B
//
//	2021/01/18 : N.Ohmoto
//################################################################

//�r�b�g�}�b�v�����P�������̃o�b�t�@�p�\����
typedef struct {
	char*	pPix;	//�����̃r�b�g�}�b�v�f�[�^�ւ̃|�C���^
	COORD	size;	//�����̕��ƍ���
	int buf_size;	//�r�b�g�}�b�v�f�[�^�̃T�C�Y�i�o�C�g���j
	int aaLv;		//�~�����i�A���`�G�C���A�X���x���j
	wchar_t	wch;	//�ϊ����̕���
} BmpChar;

BmpChar* BmpChar_Create(const char* _font_name, int _font_size, int _bold, int _ggo, const wchar_t* _wtext);

//
// end of file.
//
