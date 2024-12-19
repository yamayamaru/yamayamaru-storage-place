/*
 *  ��ʐݒ�
 *
 */

#include "..\include\egb.h"
extern CRTC(), palette();

/*  �l�r�|�c�n�r��ʐݒ� */

void screenterm(char *egbwork)
{
    EGB_init(egbwork, 1536);
    EGB_resolution(egbwork, 0, 1);
    EGB_resolution(egbwork, 1, 1);
    EGB_displayPage(egbwork, 1, 3);
    CRTC(0x18, 0x80);
    palette(egbwork, 7, 0xbf, 0xbf, 0xbf);
    return;
}

/* �n�C�X�L������ʐݒ� */

void NormalHigh(char *egbwork)
{

    EGB_init(egbwork, 1536);
    EGB_resolution(egbwork, 0, 5);
    EGB_resolution(egbwork, 1, 5);

    EGB_writePage(egbwork, 1);     /* �X�v���C�g��� */
    EGB_color(egbwork, 1, 0x8000);
    EGB_clearScreen(egbwork);
    EGB_displayStart(egbwork, 2, 2, 2);
    EGB_displayStart(egbwork, 3, 256, 240);

    EGB_writePage(egbwork, 0);     /* �b�f��� */
    EGB_displayStart(egbwork, 2, 2, 2);
    EGB_displayStart(egbwork, 3, 256, 240);
    EGB_displayPage(egbwork, 1, 3);

    return;

}

/* ���[�X�L������ʐݒ� */

void NormalLow(char *egbwork)
{

    EGB_init(egbwork, 1536);
    EGB_resolution(egbwork, 0, 8);
    EGB_resolution(egbwork, 1, 8);

    EGB_writePage(egbwork, 1);    /* �X�v���C�g��� */
    EGB_color(egbwork, 1, 0x8000);
    EGB_clearScreen(egbwork);
    EGB_displayStart(egbwork, 2, 4, 1);
    EGB_displayStart(egbwork, 3, 256, 240);

    EGB_writePage(egbwork, 0);    /* �b�f��� */
    EGB_displayStart(egbwork, 2, 4, 1);
    EGB_displayPage(egbwork, 1, 3);
    EGB_displayStart(egbwork, 3, 256, 240);

    return;;

}

/* �C���^�[���[�X��ʐݒ� */

void NormalVideo(char *egbwork)
{

    EGB_init(egbwork, 1536);
    EGB_resolution(egbwork, 0, 7);
    EGB_resolution(egbwork, 1, 7);

    EGB_writePage(egbwork, 1);    /* �X�v���C�g��� */
    EGB_color(egbwork, 1, 0x8000);
    EGB_clearScreen(egbwork);
    EGB_displayStart(egbwork, 2, 4, 1);
    EGB_displayStart(egbwork, 3, 256, 240);

    EGB_writePage(egbwork, 0);    /* �b�f��� */
    EGB_displayStart(egbwork, 2, 4, 1);
    EGB_displayPage(egbwork, 1, 3);
    EGB_displayStart(egbwork, 3, 256, 240);

    return;

}

/* �n�C�X�L����������ʐݒ� */

void WideHigh(char *egbwork)
{

    EGB_init(egbwork, 1536);
    EGB_resolution(egbwork, 0, 5);
    EGB_resolution(egbwork, 1, 5);

    EGB_writePage(egbwork, 1);     /* �X�v���C�g��� */
    EGB_color(egbwork, 1, 0x8000);
    EGB_clearScreen(egbwork);
    EGB_displayStart(egbwork, 2, 2, 2);
    EGB_displayStart(egbwork, 3, 256, 240);

    EGB_writePage(egbwork, 0);     /* �b�f��� */
    EGB_displayStart(egbwork, 2, 2, 2);
    EGB_displayStart(egbwork, 3, 256, 240);
    EGB_displayPage(egbwork, 1, 3);

    CRTC(0, 80);;                       /* hsw1 */
    CRTC(1, 590);                       /* hsw2 */
    CRTC(4, 669);                       /* hst */
    CRTC(29, 3);                        /* er1 */
    CRTC(9, 130);                       /* hds0 */ /* �b�f�`��������ɂ��� */
    CRTC(18, 130);                      /* haj0 */
    CRTC(10, 642);                      /* hde0 */

    CRTC(11, 130);                      /* hds1 */ /* �X�v���C�g�������ɂ��� */
    CRTC(22, 130);                      /* haj1 */
    CRTC(12, 642);                      /* hde1 */

    return;

}

/* ���[�X�L����������ʐݒ� */

void WideLow(char *egbwork)
{

    EGB_init(egbwork, 1536);
    EGB_resolution(egbwork, 0, 8);
    EGB_resolution(egbwork, 1, 8);

    EGB_writePage(egbwork, 1);    /* �X�v���C�g��� */
    EGB_color(egbwork, 1, 0x8000);
    EGB_clearScreen(egbwork);
    EGB_displayStart(egbwork, 2, 5, 1);
    EGB_displayStart(egbwork, 3, 256, 240);

    EGB_writePage(egbwork, 0);     /* �b�f��� */
    EGB_displayStart(egbwork, 2, 5, 1);
    EGB_displayPage(egbwork, 1, 3);
    EGB_displayStart(egbwork, 3, 256, 240);

    return;

}

/* �C���^�[���[�X������ʐݒ� */

void WideVideo(char *egbwork)
{

    EGB_init(egbwork, 1536);
    EGB_resolution(egbwork, 0, 7);
    EGB_resolution(egbwork, 1, 7);

    EGB_writePage(egbwork, 1);    /* �X�v���C�g��� */
    EGB_color(egbwork, 1, 0x8000);
    EGB_clearScreen(egbwork);
    EGB_displayStart(egbwork, 2, 5, 1);
    EGB_displayStart(egbwork, 3, 256, 240);

    EGB_writePage(egbwork, 0);     /* �b�f��� */
    EGB_displayStart(egbwork, 2, 5, 1);
    EGB_displayPage(egbwork, 1, 3);
    EGB_displayStart(egbwork, 3, 256, 240);

    return;
}
