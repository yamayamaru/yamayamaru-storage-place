/******************************************************************************
 *																			  *
 *  EUPHONY LIBRARY SAMPLE PROGRAM											  *
 *																			  *
 *        TITLE    : EUPHONY FILE 演奏										  *
 *																			  *
 *        使用方法 : ｺﾏﾝﾄﾞﾓｰﾄﾞ上から	 				   					  *
 *					   ＞run386 euptest <eup_filename.eup>					  *
 *					 例＞run386 euptest openinig2.eup						  *
 *																			  *
 *					 [機能] 固定ﾀｲﾏﾓｰﾄﾞを使用して EUPﾌｧｲﾙを演奏します		  *
 *																			  *
 *        注意     : run386 実行時 -nocrt ｵﾌﾟｼｮﾝを指定すると表示画面が残り、  *
 *                   ｶｰｿﾙが表示されません                                     *
 *																			  *
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "..\include\snd.h"

int buff_size;
int buff_sw;
int fixed_mode;
char *buff_top[2];
char *buff_btm[2];
char status_save;
void (*int_ent)();
void (*sub_ent)();

int tempo,signature;
int eof;
FILE *fp;

#define BIOS_WORK_SIZE (16384)

void main(argc,argv)
int argc;
char *argv[];
{
	extern void vsync_int();
	extern void vsync_sub();
	extern char key_check();

	char *buff;
	char *work;
	void (*timer_a_sav)();
	void (*timer_sub_sav)();

	char filename[64];
	int i;
	int meas;
	int mem_size;

	if( argc > 3){
		printf("parameter error !! (shell <-m?> <-f>'filepath')\n");
		printf(" -m? = memory size (64KB per 1)  -f = Fixed timer mdoe\n");
		exit(0);
	}

/* set default values */
	fixed_mode = 0;
	mem_size = 4;

/* read option */
	while(--argc){
	    if (*argv[argc]++ == '-'){
			switch (*argv[argc]++){
				case 'f':
				case 'F':
					fixed_mode = 1;
				break;

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

/* allocate memory */
	if((work = malloc(BIOS_WORK_SIZE+mem_size)) == NULL){
		printf("Memory allocation error !!\n");
		exit(0);
	}
	buff = work+BIOS_WORK_SIZE;

/* initialize sound bios */
	if(SND_init(work)){
		printf("Can't initialize Sound Bios !!\n");
		free(work);
		exit(0);
	}
	SND_elevol_mute(0x0f);
	SND_elevol_set(0,127,127);

/* check filename */
	for(i = 0;filename[i] != '\0';i++);
	while((filename[i] != '.') && (i >= 0)) i--;
	if(!(!strcmp(&filename[i+1],"eup")) && !(!strcmp(&filename[i+1],"EUP")))
	{
		printf("file name -> '?????.eup' !!\n");
		SND_end();
		free(work);
		exit(0);
	}

/* intialize EUP driver */
	if(driver_init(buff,mem_size,work)){
		printf("Can't initialize euphony driver !!");
		SND_end();
		free(work);
		exit(0);
	}
	SND_rs_midi_init();

	if(fixed_mode){
		timer_a_sav = SND_int_timer_a_get();
		SND_int_timer_a_set(vsync_int);
		timer_sub_sav = SND_int_timer_sub_get();
		SND_int_timer_sub_set(vsync_sub);
		SND_fm_timer_a_set(1,1024-925);
	}

/* read file */
	if(read_file(filename)){
		printf("can't read %s !\n",filename);
		driver_end();
		SND_rs_midi_end();
		SND_end();
		free(work);
		exit(0);
	}
	
/* play start */	
	SND_eup_play_start(buff_top[0],buff_size*2,signature);
	meas = 0;

/* idol loop */
	while(SND_eup_stat_flag() && (key_check() == 0)){
		if(file_check()){
			printf("some error !! at file_check\n");
			SND_eup_play_stop();
			if(eof == 0) fclose(fp);
			driver_end();
			SND_end();
			exit(0);
		}
		i = SND_eup_stat_meas();
		if(meas != i) meas = i;
	}

/* play stop */
	SND_eup_play_stop();

/*driver end */
	if(eof == 0) fclose(fp);
	for(i=0;i<1000;i++);
	if(fixed_mode)
	{
		SND_fm_timer_a_set(0,0);
		SND_int_timer_a_set(timer_a_sav);
		SND_int_timer_sub_set(timer_sub_sav);
	}
	SND_rs_midi_end();
	driver_end();
	SND_end();
	free(work);

	exit(0);
}

/*------------------------------------------------*/
void vsync_int()
{
	SND_eup_fixed_int(1000000/60);
}
void vsync_sub()
{
	SND_eup_fixed_sub();
}
/*------------------------------------------------*/
int driver_init(buffer,size)
char *buffer;
int size;
{
	if((buff_size = (size/12)*6) < 64)
		return(2);

	buff_top[0] = buffer;
	buff_btm[0] = buff_top[0] + buff_size -6;
	buff_top[1] = buff_top[0] + buff_size;
	buff_btm[1] = buff_top[1] + buff_size -6;

	if(fixed_mode)
		SND_eup_fixed_init(&int_ent,&sub_ent);
	else
		SND_eup_init();
	return(0);
}
/*------------------------------------------------*/
int driver_end()
{
	if(fixed_mode)
		SND_eup_fixed_end();
	else
		SND_eup_end();
	return(0);
}

/*------------------------------------------------*/
int read_file(path)
char *path;
{
	int i,j,size;
	char buffer[128];
	char bankpath[128];
	char fm_bankname[8];
	char pcm_bankname[8];

	eof = 1;
	if((fp = fopen(path,"rb")) == NULL)
		return(1);

	fseek(fp,852,SEEK_SET);

	if(fread(buffer,1,32,fp) < 32){
		fclose(fp);
		return(2);
	}
	for(i=0 ; i<32 ;i++)
		SND_eup_mute_set(i,(int)buffer[i]);

	if(fread(buffer,1,32,fp) < 32){
		fclose(fp);
		return(2);
	}
	for(i=0 ; i<32 ;i++)
		SND_eup_port_set(i,(int)buffer[i]);

	if(fread(buffer,1,32,fp) < 32){
		fclose(fp);
		return(2);
	}
	for(i=0 ; i<32 ;i++)
		SND_eup_midi_ch_set(i,(int)buffer[i]);

	if(fread(buffer,1,32,fp) < 32){
		fclose(fp);
		return(2);
	}
	for(i=0 ; i<32 ;i++)
		SND_eup_bias_set(i,(int)buffer[i]);

	if(fread(buffer,1,32,fp) < 32){
		fclose(fp);
		return(2);
	}
	for(i=0 ; i<32 ;i++)
		SND_eup_transpose_set(i,(int)buffer[i]);
/*
channel assign
*/
	fseek(fp,1748,SEEK_SET);
	if(fread(buffer,1,6,fp) < 6){
		fclose(fp);
		return(2);
	}
	for(i = 0;i < 6;i++)
		SND_midi_ch_assign(i,(int)buffer[i]);

	if(fread(buffer,1,8,fp) < 8){
		fclose(fp);
		return(2);
	}
	for(i = 0;i < 8;i++)
		SND_midi_ch_assign(i+64,(int)buffer[i]);
/*
bank load
*/
	SND_pcm_mode_set(0);
	
	for(i = 0;path[i] != '\0';i++)
		bankpath[i] = path[i];
	while((path[i] != '\\') && (path[i] != ':') && (i >= 0))i--;
	i++;

	if(fread(&bankpath[i],1,8,fp) < 8){
		fclose(fp);
		return(2);
	}

	if(bankpath[i] != '\0'){
		for(j= 0;bankpath[i+j] != '\0';j++);
		strcpy(&bankpath[i+j],".fmb");
		SND_fm_bank_load(bankpath,fm_bankname);
	}

	if(fread(&bankpath[i],1,8,fp) < 8){
		fclose(fp);
		return(2);
	}
	if(bankpath[i] != '\0'){
		for(j= 0;bankpath[i+j] != '\0';j++);
		strcpy(&bankpath[i+j],".pmb");
		SND_pcm_bank_load(bankpath,pcm_bankname);
	}

	fseek(fp,2048,SEEK_SET);

	if(fread(&size,4,1,fp) < 1){
		fclose(fp);
		return(2);
	}

	if(fread(buffer,1,2,fp) < 2){
		fclose(fp);
		return(2);
	}
	signature = (int)buffer[0];
	tempo = (int)buffer[1];

	eof = 0;
	status_save = 0xfe;
	buff_sw = 1;
	load_sub();
	load_sub();
	SND_eup_tempo_set(tempo);
	return(0);

}


/*------------------------------------------------*/
int file_check()
{
	char *ptr;

		if(eof != 0) return(0);
		ptr = SND_eup_stat_ptr();

		if(((buff_sw == 0) && (ptr <= buff_btm[0]))
				||((buff_sw == 1) && (ptr >= buff_top[1])))
			return(load_sub());
		else
			return(0);
			
}	
/*------------------------------------------------*/
int load_sub()
{
	char *ptr;
	char status;
	int sw;

	if(!buff_sw){
		buff_sw = 1;
		sw = 0;
	}
	else{
		buff_sw = 0;
		sw = 1;
	}
	ptr = buff_top[buff_sw];
	while((!eof) && (ptr <= buff_btm[buff_sw])){
		if(fread(ptr,1,6,fp) < 6){
			fclose(fp);
			return(2);
		}
		else if(*ptr == 0xfe){
			eof = 1;
			fclose(fp);
		}
		ptr = ptr + 6;
	}

	status = status_save;
	if(!eof){
		status_save = *buff_btm[buff_sw];
		*buff_btm[buff_sw] = 0xfd;
	}
	*buff_btm[sw] =  status;
	SND_eup_play_restart();
	return(0);
}
