/***************************************************************/
/*                                                             */
/* RS.H                                                        */
/*                                                             */
/*   Native RS-232C library header file                        */
/*                                                             */
/*            Copyright (c) 1991-1994 FUJITSU LIMITED.         */
/*            All rights reserved.                             */
/*                                                             */
/***************************************************************/

int RSB_Init();
int RSB_End();
int RSB_Open(int port);
int RSB_Close(int port);
int RSB_Setpara(int port, char *paradr);
int RSB_Receive(int port, int *data, unsigned int *status);
int RSB_Read(int port, int *leng);
int RSB_Send(int port, int data, unsigned int *status);
int RSB_Ctrl(int port, int ctrl);
int RSB_Status(int port, unsigned int *status, unsigned int *serial);
int RSB_Break(int port, int btime);
int RSB_Initbuf(int port);
int RSB_Dtr(int port, int sw);

unsigned RSB_Chk();
int RSB_Rdpara(int port, char *paradr);
int RSB_Setint(int port, char *intbuf);
int RSB_Rdint(int port, char *intbuf);
int RSB_Clsxoff(int port);
int RSB_Gettxlen(int port, int *len);
int RSB_Init2(int IOport);

unsigned RSB_Hdchk();
