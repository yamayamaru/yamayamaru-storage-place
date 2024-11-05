/*
 *    �O�D�T�h�b�g�X�N���[���T���v��
 *
 *    ���jsetscrn.obj, gra.obj, io.obj�������N���Ă�������
 */

#include <stdio.h>
#include <fmcfrb.h>              /* KYBD */
#include <egb.h>

extern void CRTC(), screenterm(), EGB_singleLine();
void leftHalfScroll(), waitVsync();

void main()
{
    unsigned int    ky, ene;
    int             i, c;
    char            egbwork[1536];

    struct {
        short int    x, y, num;
        char         moji[31];
    } title = { 0, 16, 31, "�O�D�T�h�b�g�X�N���[���T���v��" };

    KYB_init();
    KYB_clic(1);

    EGB_init(egbwork, 1536);
    EGB_resolution(egbwork, 0, 10);
    EGB_resolution(egbwork, 1, 10);
    EGB_displayPage(egbwork, 1, 3);

    EGB_writePage(egbwork, 1);
    EGB_color(egbwork, 1, 0x8000);
    EGB_color(egbwork, 0, 0x7fff);
    EGB_clearScreen(egbwork);
    EGB_displayStart(egbwork, 1, 0, 0);
    EGB_displayStart(egbwork, 2, 2, 2);
    EGB_displayStart(egbwork, 3, 320, 240);
    EGB_sjisString(egbwork, (char *)&title);

    for (i = 0; i < 320; i=i + 4) {
        EGB_singleLine(egbwork, i, 120, i, 239);
    }

    EGB_writePage(egbwork, 0);
    EGB_clearScreen(egbwork);
    EGB_displayStart(egbwork, 1, 0, 0);
    EGB_displayStart(egbwork, 2, 2, 2);
    EGB_displayStart(egbwork, 3, 320, 240);

    c = 0;
    for (i = 0; i < 320; i++) {
        EGB_color(egbwork, 0, c);
        EGB_singleLine(egbwork, i, 0, i, 239);
        c = c + 25;
    }

    ky = 0xffff;
    while (ky != 13) {        /* ret�ŏI�� */
        ky = 0xffff;
        leftHalfScroll(egbwork);
        while (ky == 0xffff) ky = KYB_read(1, &ene);
    }

    screenterm(egbwork);
}


/*
 *    �O�D�T�X�N���[�����[�`��
 *
 */

#define fa0   17
#define haj0  18

void leftHalfScroll(char *egbwork)
{
    static int cnt = 0;

    /* fa0�̏����l�i��ʍ���j */
    static int fa0Data = 0;

    /* haj0�̏����l�i���̒l�͉�ʃ��[�h�ɂ���ĈقȂ�B�Ԗ{�Q�Ɓj */
    static int haj0Data = 0x8a;


    if (cnt == 0 || cnt == 4) {
        waitVsync(egbwork);
        CRTC(fa0, fa0Data);
        CRTC(haj0, haj0Data);
        fa0Data++;
        if (fa0Data ==256) fa0Data = 0;
             /* fa0Data > 255����Y������+1dot�����̂Ŗ߂� */
        cnt = 1;
    } else {
        waitVsync(egbwork);
        CRTC(haj0, haj0Data - cnt);
        cnt++;
    }
}

void waitVsync(char *egbwork)
{
    int    par = 0;
    EGB_palette(egbwork, 1, (char *)&par);
}
