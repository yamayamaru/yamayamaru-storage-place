/*
	Header for sound bios lib
	Copyright (C) FUJITSU LIMITED 1988-1994.
	1989/02/02	by Joe Mizuno
	1990/06/12	by Joe Mizuno
	1990/08/24	by Y.Miyazawa
	1991/03/18	by Y.Miyazawa
	1991/07/22	by Joe Mizuno
	1992/06/04	by Y.Miyazawa
	1992/07/03	by Joe Mizuno
	1992/09/18	by Y.Miyazawa
	1993/09/04	by Y.Miyazawa
	1994/08/23	by Y.Miyazawa
*/

#define SndWorkSize  16384
#define SndEventSave 96

/*
	MIDI output port
*/

#define FUJITSU_MIDI_A  (0x00)
#define FUJITSU_MIDI_B  (0x01)
#define FUJITSU_MIDI_C  (0x02)
#define FUJITSU_MIDI_D  (0x03)
#define FUJITSU_MIDI_E  (0x04)
#define FUJITSU_MIDI_F  (0x05)
#define FUJITSU_MIDI_G  (0x06)
#define FUJITSU_MIDI_H  (0x07)
#define RS_MIDI         (0x10)
#define INTERNAL_VOICE  (0xf0)

/*
	Sound bios lib
*/

int SND_fm_bank_load( void *path,void *bankname );
int SND_pcm_bank_load( void *path,void *bankname );

int SND_init( void *work );
int SND_end();
int SND_key_on( int ch,int note,int vol );
int SND_key_off( int ch );
int SND_pan_set( int ch,int pan );
int SND_inst_change( int ch,int inst );
int SND_inst_write( int ch,int inst,void *buff );
int SND_inst_read( int ch,int inst,void *buff );
int SND_pitch_change( int ch,int pitch );
int SND_volume_change( int ch,int vol );
int SND_key_abort( int ch );
int SND_status( int ch,int cmd,void *buff );
int SND_fm_read_status();
int SND_fm_write_data( int bank,int reg,int data );
int SND_fm_write_save_data( int bank,int reg,int data );
int SND_fm_read_save_data( int bank,int reg,int *data );
int SND_fm_timer_a_set( int sw,int count );
int SND_fm_timer_b_set( int sw,int count );
int SND_fm_timer_a_start();
int SND_fm_timer_b_start();
int SND_fm_lfo_set( int frq );
int SND_pcm_wave_set( void *buff,int wave_addr,int size );
int SND_pcm_wave_read( int wave_addr,void *buff,int size );
int SND_pcm_wave_move( int wave_source,int wave_distnation,int size );
int SND_pcm_wave_write( void *buffer,int wave_addr,int size );
int SND_pcm_mode_set( int ch );
int SND_pcm_sound_set( void *buff );
int SND_pcm_sound_delete( int id );
int SND_pcm_rec( int freq,void *buff,int size,int trg );
int SND_pcm_rec2( int freq,void *buff,int size,int trg );
int SND_pcm_rec3( int freq,void *buff,int size,int trg );
int SND_pcm_rec4( int freq,void *buff,int size,int trg,int sw,int bunshu,void event() );
int SND_pcm_play( int ch,int note,int vol,void *buff );
int SND_pcm_play2( int ch,int note,int vol,void *buff );
int SND_pcm_play_rom( int ch,int note,int vol,int rom_num );
int SND_pcm_play2_rom( int ch,int note,int vol,int rom_num );
int SND_pcm_rec_stop();
int SND_pcm_play_stop( int ch );
int SND_pcm_status( int ch );
int SND_pcm_abort();

int SND_pcm_play_auto( int note,int vol,void *buff );
int SND_pcm_play2_auto( int note,int vol,void *buff );
int SND_pcm_play_rom_auto( int note,int vol,int rom_num );
int SND_pcm_play2_rom_auto( int note,int vol,int rom_num );

int SND_int_timer_a_set( void addr() );
void ( *SND_int_timer_a_get() )();
int SND_int_timer_b_set( void addr() );
void ( *SND_int_timer_b_get() )();
int SND_int_timer_sub_set( void addr() );
void ( *SND_int_timer_sub_get() )();
int SND_int_mouse_sub_set( void addr() );
void ( *SND_int_mouse_sub_get() )();

int SND_joy_in_1( int port,int *status );
int SND_joy_in_2( int port,int *status );
int SND_joy_out( int data,int *status1,int *status2 );
int SND_elevol_set( int num,int l_vol,int r_vol );
int SND_elevol_init();
int SND_elevol_read( int num,int *l_vol,int *r_vol );
int SND_elevol_mute( int sw_bit );
int SND_elevol_led_read();
int SND_elevol_led_set( int bit_data );
int SND_elevol_all_mute( int sw );

int SND_get_elevol_set( int num,int *l_vol,int *r_vol );
int SND_get_elevol_mute( int *sw_bit );
int SND_get_int_status();
int SND_pcm_sound_get( int ID,void *buff );
int SND_pcm_sound_ID_get( void *buff );
int SND_pcm_mode_get();

int SND_pcm_adjust_rate( int rate,void *source_buff,void *buff );

void SND_saveEvent( void *buff );
void SND_restoreEvent( void *buff );

/*
	EUPHONY MIDI DRIVER
*/

int SND_eup_init();
int SND_eup_end();
int SND_eup_play_start( void *buff,int size,int sign );
int SND_eup_play_stop();
int SND_eup_play_pause();
int SND_eup_play_restart();
char *SND_eup_stat_ptr();
int SND_eup_stat_meas();
int SND_eup_sync_out_set( int port );
int SND_eup_stat_clock();
int SND_eup_stat_flag();
int SND_eup_stat_tempo();
int SND_eup_master_int_set( int sw );
int SND_eup_int_set( int sw );
int SND_eup_loop_set( int sw );
int SND_eup_tempo_set( int tempo );
int SND_eup_relative_tempo_get( int *tempo );
int SND_eup_relative_tempo_set( int tempo );
int SND_eup_port_set( int track,int port );
int SND_eup_play_int_get( void (**addr)() );
int SND_eup_play_int_set( void (*addr)() );
int SND_eup_midi_ch_set( int track,int ch );
int SND_eup_mute_set( int track,int sw );
int SND_eup_transpose_set( int track,int degree );
int SND_eup_bias_set( int track,int bias );
int SND_eup_rs_midi_get( void (**addr)() );
int SND_eup_rs_midi_set( void addr() );

int SND_eup_fixed_init( void (**(func))(),void (**(func1))() );
int SND_eup_fixed_end();
int SND_eup_fixed_int( int time );
int SND_eup_fixed_sub();

/*
	MIDI EMURATOR
*/

int SND_midi_ch_assign( int ch,int midi_ch );
int SND_midi_write( int port,void *buff,int size );
int SND_midi_read( int port,char *data );

/*
	RS-MIDI Driver
*/

int SND_rs_midi_init();
int SND_rs_midi_end();

/*
	Test
*/

int  SND_int_polling_sub_set( void addr() );
void ( *SND_int_polling_sub_get() )();
int  SND_int_stack_set( void *para );
int  SND_int_stack_get( void *para );
