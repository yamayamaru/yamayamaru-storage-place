/******************************************************************************
 *																			  *
 *  EUPHONY LIBRARY SAMPLE PROGRAM											  *
 *																			  *
 *        TITLE    : EUPHONY FILE ‰‰‘t										  *
 *																			  *
 *        ‹@”\     : ‰‰‘tÄÞ×²ÊÞ»ÌÞÙ°ÁÝ										  *
 *																			  *
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "..\include\snd.h"

int buff_size;
int buff_sw;
char *buff_top[2];
char *buff_btm[2];
char status_save;

int tempo,signature;
int eof;
FILE *fp;

/*------------------------------------------------*/
int ML_Start(buffer,size)
char *buffer;
int size;
{
	if((buff_size = (size/12)*6) < 64)
		return(2);

	buff_top[0] = buffer;
	buff_btm[0] = buff_top[0] + buff_size -6;
	buff_top[1] = buff_top[0] + buff_size;
	buff_btm[1] = buff_top[1] + buff_size -6;

	SND_eup_init();
	return(0);
}
/*------------------------------------------------*/
int ML_End()
{
	SND_eup_end();
	return(0);
}

/*------------------------------------------------*/
int ML_Set(path)
char *path;
{
	int i,j,err,size;
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
		err=SND_eup_mute_set(i,(int)buffer[i]);

	if(fread(buffer,1,32,fp) < 32){
		fclose(fp);
		return(2);
	}
	for(i=0 ; i<32 ;i++)
		err=SND_eup_port_set(i,(int)buffer[i]);

	if(fread(buffer,1,32,fp) < 32){
		fclose(fp);
		return(2);
	}
	for(i=0 ; i<32 ;i++)
		err=SND_eup_midi_ch_set(i,(int)buffer[i]);

	if(fread(buffer,1,32,fp) < 32){
		fclose(fp);
		return(2);
	}
	for(i=0 ; i<32 ;i++)
		err=SND_eup_bias_set(i,(int)buffer[i]);

	if(fread(buffer,1,32,fp) < 32){
		fclose(fp);
		return(2);
	}
	for(i=0 ; i<32 ;i++)
		err=SND_eup_transpose_set(i,(int)buffer[i]);
/*
channel assign
*/
	fseek(fp,1748,SEEK_SET);
	if(fread(buffer,1,6,fp) < 6){
		fclose(fp);
		return(2);
	}
	for(i = 0;i < 6;i++)
		err=SND_midi_ch_assign(i,(int)buffer[i]);

	if(fread(buffer,1,8,fp) < 8){
		fclose(fp);
		return(2);
	}
	for(i = 0;i < 8;i++)
		err=SND_midi_ch_assign(i+64,(int)buffer[i]);
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
		bankpath[i+j] = '.';
		bankpath[i+j+1] = 'f';
		bankpath[i+j+2] = 'm';
		bankpath[i+j+3] = 'b';
		bankpath[i+j+4] = '\0';
		SND_fm_bank_load(bankpath,fm_bankname);
	}

	if(fread(&bankpath[i],1,8,fp) < 8){
		fclose(fp);
		return(2);
	}
	if(bankpath[i] != '\0'){
		for(j= 0;bankpath[i+j] != '\0';j++);
		bankpath[i+j] = '.';
		bankpath[i+j+1] = 'p';
		bankpath[i+j+2] = 'm';
		bankpath[i+j+3] = 'b';
		bankpath[i+j+4] = '\0';
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
int ML_Play(mode)
int mode;
{
	SND_eup_loop_set(mode);
	SND_eup_play_start(buff_top[0],buff_size*2,signature);
	return(0);
}

/*------------------------------------------------*/
int ML_Stop()
{
	SND_eup_play_stop();
	if(eof == 0)
		fclose(fp);
	return(0);
}

/*------------------------------------------------*/
int ML_Check()
{
	char *ptr;

		if(eof != 0)
			return(0);
		ptr = SND_eup_stat_ptr();

		if(((buff_sw == 0) && (ptr <= buff_btm[0]))
				||((buff_sw == 1) && (ptr >= buff_top[1])))
			return(load_sub());
		else
			return(0);
			
}	
/*------------------------------------------------*/
int SL_Start()
{
	SND_pcm_sound_delete(-1);
	SND_pcm_mode_set(1);
	return(0);
}

/*------------------------------------------------*/
int SL_Load(path,buffer,size)
char *path;
char buffer[];
int size;
{
	FILE *fp;
	int length;

	if((fp = fopen(path,"rb")) == NULL)
		return(1);
	if(size < 32){
		fclose(fp);
		return(2);
	}
	if(fread(buffer,1,32,fp) < 32){
		fclose(fp);
		return(2);
	}

	length = *(int *)&buffer[12];
	if(size < length+32){
		fclose(fp);
		return(2);
	}
	if(fread(buffer+32,1,length,fp) < length){
		fclose(fp);
		return(2);
	}
	fclose(fp);
	return(0);
}

/*------------------------------------------------*/
int SL_Save(path,rate,buffer)
char *path;
int	rate;
char buffer[];
{
	FILE *fp;
	int length;

	if((fp = fopen(path,"wb")) == NULL)
		return(1);
	if(!rate)
		*(short int*)&buffer[24] = rate*0x62/1000;
	length = *(int *)&buffer[12];
	if(length == 0){
		fclose(fp);
		return(2);
	}
	if(fwrite(buffer,1,length+32,fp) < length+32){
		fclose(fp);
		return(1);
	}
	fclose(fp);
	return(0);
}

/*------------------------------------------------*/
int SL_Play(mode,rate,vol,pan,buff)
int mode;
int rate;
int vol;
int pan;
char buff[];
{
	int org_note;
	int loop;

	if(mode > 127)
		return(3);
	SND_pan_set(71,pan);
	if(rate){
		rate = rate * 0x62 / 1000;
		*(short int*)&buff[24] = rate;
	}
	if(mode==1)
		*(int *)&buff[20] = 0;
	else if(mode == 2){
		loop = *(int *)&buff[12];
		*(int *)&buff[16] = 0;
		*(int *)&buff[20] = loop;
	}

	org_note = buff[28];
	return(SND_pcm_play2(71,org_note,vol,buff));
}

/*------------------------------------------------*/
int SL_Status()
{
	return(SND_pcm_status(71));
}

/*------------------------------------------------*/
int SL_Rec(mode,freq,buff,size)
int mode;
int freq;
char buff[];
int size;
{
	int err;
	if(mode > 127)
		return(3);
	SND_elevol_all_mute(0);
	SND_elevol_set(2,127,127);
	SND_eup_master_int_set(0);
	err = SND_pcm_rec(freq,buff+32,size-32,mode);
	SND_eup_master_int_set(-1);
	SND_elevol_set(2,0,0);
	SND_elevol_mute(0x3f);
	SND_elevol_all_mute(-1);
	if(err)
		return(1); 
	srand(((int)buff[32]+(int)buff[34]*256)*(int)buff[38]*256+size);
	*(int *)&buff[8] = rand()+(buff[33]+buff[35]*256+1);
	*(int *)&buff[12] = size-32;
	*(int *)&buff[16] = 0;
	*(int *)&buff[20] = 0;
	*(short int *)&buff[24] = freq*0x62/1000;
	*(short int *)&buff[26] = 0;
	buff[28] = 60;
	return(0);
}

/*------------------------------------------------*/
int SL_Stop()
{
	SND_pcm_play_stop(71);
	SND_pcm_rec_stop();
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
