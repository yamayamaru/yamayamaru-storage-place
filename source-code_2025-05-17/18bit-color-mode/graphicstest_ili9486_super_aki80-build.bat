rem ���ϐ�SDCC_HOME��SDCC���C���X�g�[�������t�H���_���w�肵�Ă�������
rem �f�t�H���g�ł�C:\Program Files\SDCC�ɃC���X�g�[������܂�
set SDCC_HOME="C:\Program Files\SDCC"


set SDCC_INCLUDE=%SDCC_HOME%\include
set PATH=%SDCC%\bin;%PATH%


del graphicstest_ili9486_super_aki80.ihx
del graphicstest_ili9486_super_aki80.rel
del mycrt0_2_super_aki80.rel

sdcc -mz80 -c -o graphicstest_ili9486_super_aki80.rel graphicstest_ili9486_super_aki80.c
sdasz80 -o mycrt0_2_super_aki80.rel mycrt0_2_super_aki80.asm
sdcc -mz80 --code-loc 0x8000 --data-loc 0 --no-std-crt0 -o graphicstest_ili9486_super_aki80.ihx mycrt0_2_super_aki80.rel  graphicstest_ili9486_super_aki80.rel 

