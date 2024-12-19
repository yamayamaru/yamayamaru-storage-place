/***************************************************************/
/*                                                             */
/* MIDIMAN.H                                                   */
/*            COPYRIGHT (C) 1994 FUJITSU LIMITED               */
/*                                                             */
/***************************************************************/

#define	NOERR			0
#define	NOROOM			-1
#define	NODATA			-2
#define	PARAERR			-3
#define	NODEVICE		-10
#define	NOTSUPPORT		-11
#define	OVERRUN			-12
#define	DOSERR			-13
#define	ALREADYOPEN		-14

#define	FLTNOTEOFF			0x01
#define	FLTNOTEON			0x02
#define	FLTKEYPRESS			0x04
#define	FLTCTRL				0x08
#define	FLTPROG				0x10
#define	FLTCHPRESS			0x20
#define	FLTPITCH			0x40
#define	FLTEXC				0x80
#define	FLTTEMPO			0x100
#define	FLTMETA				0x200

#define SMPTE24		(0)
#define SMPTE25		(1)
#define SMPTE30DF	(2)
#define SMPTE30		(3)
#define SMPTEREAL	(4)

#define INTERNALSYNC	(0)
#define MIDISYNC	(1)
#define SMPTESYNC	(2)

#define NORMALPLAY	(0)
#define ENDLESSPLAY	(1)
#define LOOPPLAY	(2)

#define PLAYFLAG	(1)
#define PAUSEFLAG	(2)
#define RECFLAG		(4)

#define	DEFSIGN		(0x04022408)

typedef	unsigned int		SIGN;
typedef	int					TIME;
typedef	unsigned char		SEQ;
typedef	unsigned char		TRACK;
typedef	unsigned char		PORT;
typedef	unsigned char		MIDICH;
typedef	unsigned char		FLAG;

typedef struct
{
	short int	as_status;
	short int	as_chmap;
	TRACK		as_track;
	char		as_velo;
	char		as_oct;
	char		as_ctrlmap;
} ASSIGNFILTER;

typedef	struct
{
	TRACK	trk_number;		/*track number		*/
	FLAG	trk_mute;		/*play or mute		*/
	int		trk_filter;		/*play filter bits	*/
	PORT	trk_port;		/*output port		*/
	MIDICH	trk_midich;		/*output midi ch	*/
	char	trk_notemap;	/*note replace map	*/
	char	trk_ctrlmap;	/*ctrl replace map	*/
	char	trk_progmap;	/*prog replace map	*/
	char	trk_bias;		/*velocity shift	*/
	char	trk_oct;		/*transpose shift	*/
	char	trk_vol;		/*ctrl volume shift	*/
	char	trk_exp;		/*ctrl expression shift */
	char	trk_resv1;		/*reserve		*/
}TRACKWORK;

typedef	struct
{
	int		off_time;
	FLAG	off_flag;
	TRACK	off_track;
	PORT	off_port;
	MIDICH	off_ch;
	char	off_note;
	char	off_velo;
}NOTEOFFTABLE;

typedef struct
{
	int				mc_interval;
	int				mc_maxnote;
	int				mc_maxtrack;
	unsigned char	*mc_sbios;
	NOTEOFFTABLE	*mc_noteoff;
	TRACKWORK		*mc_trackwork;
	char			*mc_filter[16];
	unsigned char	*mc_event;
	int				mc_eventsize;
	unsigned char	*mc_fmtout;
	int				mc_fmtoutsize;
	unsigned char	*mc_fmtin;
	int				mc_fmtinsize;
	unsigned char	*mc_smpu;
	int	 			mc_smpusize;
	unsigned char	*mc_stackadr;
	int				mc_stacksize;
} MIDIMANCTRL;

typedef struct
{
	void			(*(fc_idletask))();
	short int		fc_idleseg;
	void			(*(fc_rectask))();
	short int		fc_recseg;
	void			(*(fc_eventtask))();
	short int		fc_eventseg;
	short int		fc_dataseg;
}FUNCCTRL;

typedef struct
{
	void			(*(cfc_idletask))();
	void			(*(cfc_rectask))(PORT,FLAG,TRACK,unsigned char *);
	void			(*(cfc_eventtask))(TRACK,PORT,unsigned char *);
}CFUNCCTRL;

typedef struct
{
	void			(*(rc_output))();
	short int		rc_outputseg;
	void			(*(rc_input))();
	short int		rc_inputseg;
	short int		rc_dataseg;
}RSCTRL;

typedef struct
{
	int	rt_lo;
	int	rt_hi;
} REALTIME;

typedef	struct
{
	int			p_meas;
	int			p_beat;
	int			p_tick;
	TIME		p_clock;
	REALTIME	p_time;
	SIGN		p_sign;
} PLACE;

typedef struct
{
	int		tc_hr;
	int		tc_min;
	int		tc_sec;
	int		tc_fr;
	int		tc_bit;
	int		tc_type;
} SMPTE;

/*	location structure	*/
typedef	struct{
	unsigned char	*ptr;
	TIME			time;
	int				remain;
	int				count;
} LOCATION;

typedef	struct{

	unsigned char	metro_mode;
	PORT			metro_port;
	MIDICH			metro_midich;
	char			metro_hinote;
	char			metro_hivelo;
	char			metro_lownote;
	char			metro_lowvelo;
	char			metro_resv;
	TIME			metro_duration;
} METRONOME;


extern	int TMM_openMidiMan(MIDIMANCTRL *);
extern	int TMM_closeMidiMan();
extern	int TMM_getMidiManCtrl(MIDIMANCTRL *);
extern	int TMM_initRsMidi(RSCTRL *rs_ctrl);
extern	int TMM_termRsMidi();
extern	int TMM_setUserFunc(FUNCCTRL *func_ctrl);
extern	int TMM_getUserFunc(int,FUNCCTRL *);
extern	int TMM_resetUserFunc(int);
extern	int TMM_outputMidiEvent(PORT,unsigned char *);
extern	int TMM_playStart(char,FLAG,unsigned char *,int,PLACE *);
extern	int TMM_playStop();
extern	int TMM_playPause();
extern	int TMM_playRestart();
extern	int TMM_setPlayMode(char);
extern	int TMM_getPlayPlace(int *,unsigned char **,PLACE *);
extern	int TMM_setTempo(char,int);
extern	int TMM_getTempo(char,int *);
extern	int TMM_setRelativeTempo(int);
extern	int TMM_getRelativeTempo(int *);
extern	int TMM_setEupTempoOffset(int);
extern	int TMM_getEupTempoOffset(int *);
extern	int TMM_stepModeEntry(int);
extern	int TMM_setSyncIn(char,PORT,char,int,int);
extern	int TMM_setSmpteOffset(REALTIME *);
extern	int TMM_setSmpteLag(int,int);
extern	int TMM_setSmpuTime(REALTIME *);
extern	int TMM_realtimeToSmpte(SMPTE *,REALTIME *);
extern	int TMM_smpteToRealtime(REALTIME *,SMPTE *);
extern	int TMM_setRemoteMode(char);
extern	int TMM_setSyncOut(char,PORT);
extern	int TMM_setMetronome(METRONOME *);
extern	int TMM_setAssignMap(PORT,char);
extern	int TMM_getAssignMap(PORT,char *);
extern	int TMM_setAssignFilter(char,ASSIGNFILTER *);
extern	int TMM_getAssignFilter(char,ASSIGNFILTER *);
extern	int TMM_setOutputPortMap(PORT,PORT);
extern	int TMM_getOutputPortMap(PORT,PORT *);
extern	int TMM_setInputPortMap(PORT,PORT);
extern	int TMM_getInputPortMap(PORT,PORT *);
extern	int TMM_initInternalVoice();
extern	int TMM_sendInternalVoice(unsigned char);
extern	int TMM_setInternalChannel(unsigned char,MIDICH);
extern	int TMM_getInternalChannel(unsigned char,MIDICH *);
extern	int TMM_setInternalVolume(char);
extern	int TMM_getInternalVolume(char *);
extern	int TMM_setCFunc(CFUNCCTRL *);
extern	unsigned int TMM_readLength(unsigned char **);
extern	void TMM_writeLength(unsigned char **,unsigned int);
