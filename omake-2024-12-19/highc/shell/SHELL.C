/******************************************************************************
 *																			  *
 *  EUPHONY LIBRARY SAMPLE PROGRAM											  *
 *																			  *
 *        TITLE    : EUPHONY FILE 演奏										  *
 *																			  *
 *        使用方法 : ｺﾏﾝﾄﾞﾓｰﾄﾞ上から	 				   					  *
 *					   ＞run386 shell <eup_filename.eup>					  *
 *					 例＞run386 shell openinig2.eup							  *
 *																			  *
 *        注意     : run386 実行時 -nocrt ｵﾌﾟｼｮﾝを指定すると表示画面が残り、  *
 *                   ｶｰｿﾙが表示されません                                     *
 *																			  *
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "..\include\snd.h"

#define BIOS_WORK_SIZE (16384)

void main(argc,argv)
int argc;
char *argv[];
{
	extern int ML_Start();
	extern int ML_End();
	extern int ML_Set();
	extern int ML_Play();
	extern int ML_Stop();
	extern int ML_Check();
	extern int SL_Start();
	extern int SL_Status();
	extern int SL_Load();
	extern int SL_Play();
	extern int SL_Stop();
	extern int SND_eup_stat_flag();
	extern int SND_eup_stat_meas();

	extern char key_check();

	char *buff;
	char *work;

	char filename[64];
	int mode;
	int err;
	int i;
	int meas;
	int mem_size;

	if( argc > 3){
		printf("parameter error !! (shell <-m?> 'filepath')\n");
		printf(" -m? = memory size (64KB per 1)\n");
		exit(0);
	}

	mem_size = 4;
	while(--argc){
		if (*argv[argc]++ == '-'){
			switch (*argv[argc]++){
				case 'm':
				case 'M':
					mem_size = 0;
					while(*argv[argc] != '\0')
						mem_size = mem_size * 10 + ((int)*argv[argc]++ - 0x30);
				break;
			}
		}
		else{
			strcpy(filename,--argv[argc]);
		}
	}
	mem_size = mem_size*65536;

	if((work = malloc(BIOS_WORK_SIZE+mem_size)) == NULL){
		printf("memory allocation error !!\n");
		exit(0);
	}
	buff = work+BIOS_WORK_SIZE;

	if(SND_init(work)){
		printf("bios initialize failed !!\n");
		free(work);
		exit(0);
	}
	SND_elevol_mute(0x03);

	for(i = 0;filename[i] != '\0';i++);
	while((filename[i] != '.') && (i >= 0)) i--;
	if((!strcmp(&filename[i+1],"eup")) || (!strcmp(&filename[i+1],"EUP")))
		mode = 1;
	else{
		if((!strcmp(&filename[i+1],"snd")) || (!strcmp(&filename[i+1],"SND")))
			mode = 0;
		else{
			printf("file name -> '?????.eup' or '?????.snd' !!\n");
			SND_end();
			free(work);
			exit(0);
		}
	}

	if(mode){
		if(ML_Start(buff,mem_size,work)){
			printf("euphony driver initialize failed !!");
			SND_end();
			free(work);
			exit(0);
		}
		err = SND_rs_midi_init();
		if(ML_Set(filename)){
			printf("can't read %s !\n",filename);
			ML_End();
			SND_rs_midi_end();
			SND_end();
			free(work);
			exit(0);
		}
	}
	else{
		if(SL_Start()){
			printf("SL_Start error occured\n");
			SND_end();
			free(work);
			exit(0);
		}
		if(SL_Load(filename,buff,mem_size)){
			printf("can't read %s !\n",filename);
			SND_end();
			free(work);
			exit(0);
		}
	}
	
	
	if(mode){
		err = ML_Play(0);
		meas = 0;
	}
	else{
		err = SL_Play(0,0,127,64,buff);
	}

	if(mode){
		while(SND_eup_stat_flag() && (key_check() == 0)){
			if(ML_Check()){
				printf("some error !! at ML_Check\n");
				ML_Stop();
				ML_End();
				SND_end();
				exit(0);
			}
			i = SND_eup_stat_meas();
			if(meas != i){
				meas = i;
			}
		}
		ML_Stop();
	}
	else{
		while((key_check() == 0) && SL_Status());
		SL_Stop();
	}
	for(i=0;i<1000;i++);
	if(mode){
		err = ML_End();
		SND_rs_midi_end();
	}

	err = SND_end();
	free(work);

	exit(0);
}
