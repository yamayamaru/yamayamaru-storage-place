//
//    FM TOWNS�G�~�����[�^�̒Ìy��TCP/IP�ɔ�΂���RS232C�f�[�^���󂯂�RS232C�ɗ����v���O����
//
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <process.h>
#include <synchapi.h>

#pragma comment(lib,"ws2_32.lib")

int source_port;

HANDLE hComPort;                               // COM�|�[�g�̃n���h��������ϐ�
TCHAR *tchar01;                                // COM�|�[�g�̃|�[�g���̕����������ϐ�

int  dstSocket01, srcSocket01;

volatile int endflag01 = 0;
volatile int execflag01 = 0;

struct param01 {
    HANDLE  hComPort;
    volatile int *pendflag01;
    volatile int *pexecflag01;
    int dstSocket;
};

int serial01(){
    // 1.COM�|�[�g���J��
    hComPort = CreateFile(                     //�t�@�C���Ƃ��ă|�[�g���J��
             tchar01,                          // �|�[�g�����w���o�b�t�@�ւ̃|�C���^:COM??���J��
             GENERIC_READ | GENERIC_WRITE,     // �A�N�Z�X���[�h:�ǂݏ�����������
             0,                                //�|�[�g�̋��L���@���w��:�I�u�W�F�N�g�͋��L���Ȃ�
             NULL,                             //�Z�L�����e�B����:�n���h�����q�v���Z�X�֌p�����Ȃ�
             OPEN_EXISTING,                    //�|�[�g���J�������w��:�����̃|�[�g���J��
             0,                                //�|�[�g�̑������w��:����  �񓯊��ɂ������Ƃ���FILE_FLAG_OVERLAPPED
             NULL                              // �e���v���[�g�t�@�C���ւ̃n���h��:NULL�ł���
     );
    if (hComPort == INVALID_HANDLE_VALUE){               //�|�[�g�̎擾�Ɏ��s
        fprintf(stderr, "�w��COM�|�[�g���J���܂���.\n\r");
        CloseHandle(hComPort);                              //�|�[�g�����
        return 0;
    }
    else{
        fprintf(stderr, "COM�|�[�g�͐���ɊJ���܂���.\n\r");
    }
    // �|�[�g���t�@�C���Ƃ݂Ȃ�CreateFile()�֐���p���ĊJ���܂��B
    // ���s�����INVALID_HANDLE_VALUE��Ԃ��܂��B
    // �|�[�g�̑����͔񓯊��iFILE_FLAG_OVERLAPPED�j�ɂ������������炵���ł�����肭�����Ȃ������̂œ����ʐM�ɂ��Ă���܂��B
    // 2.����M�o�b�t�@�̐ݒ�
    // SetupComm()�֐���p���đ���M�o�b�t�@�̐ݒ�����܂��B
    int check;                                  //�G���[�`�F�b�N�p�̕ϐ�
    check = SetupComm(
          hComPort,                             //COM�|�[�g�̃n���h��
          1024,                                 //��M�o�b�t�@�T�C�Y:1024byte
          1024                                  //���M�o�b�t�@:1024byte
     );
    if (check == FALSE){
        fprintf(stderr, "����M�o�b�t�@�̐ݒ肪�ł��܂���.\r\n");
        CloseHandle(hComPort);
        return 0;
    }
    else{
        fprintf(stderr, "����M�o�b�t�@�̐ݒ肪�������܂���.\r\n");
    }


    // 3.����M�o�b�t�@�̏�����
    // PurgeComm()�֐���p���ďo���͂��ׂẴo�b�t�@���N���A���܂��B
    check = PurgeComm(
          hComPort,                                                       // COM�|�[�g�̃n���h��
          PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR   // �o���̓o�b�t�@�����ׂăN���A
    );
    if (check == FALSE){
        fprintf(stderr, "����M�o�b�t�@�̏��������ł��܂���.\r\n");
        CloseHandle(hComPort);
        return 0;
    } else{
         fprintf(stderr, "����M�o�b�t�@�̏��������������܂���.\r\n");
    }

    // 4.COM�|�[�g�\�����̏�����
    // DCB�\���̂ō\������ݒ肵�܂�
    // �{�[���[�g��f�[�^�T�C�Y�A�p���e�B�r�b�g�A�X�g�b�v�r�b�g�Ȃǂ̓}�C�R�����Ƃ��킹�Ă��������B
    // �������������SetCommState()�֐��ōĐݒ���s���܂�
    DCB dcb;                                   // �\�������L�^����\���̂̐���
    GetCommState(hComPort, &dcb);              // ���݂̐ݒ�l��ǂݍ���
    dcb.DCBlength = sizeof(DCB);               // DCB�̃T�C�Y
    dcb.BaudRate = 38400;                      // �{�[���[�g:38400bps
    dcb.ByteSize = 8;                          // �f�[�^�T�C�Y:8bit
    dcb.fBinary = TRUE;                        // �o�C�i�����[�h:�ʏ�TRUE
    dcb.fParity = NOPARITY;                    // �p���e�B�r�b�g:�p���e�B�r�b�g�Ȃ�
    dcb.StopBits = ONESTOPBIT;                 // �X�g�b�v�r�b�g:1bit
   
    dcb.fOutxCtsFlow = TRUE;                   // CTS�t���[����:�t���[����Ȃ�
    dcb.fOutxDsrFlow = FALSE;                  // DSR�n�[�h�E�F�A�t���[����F�g�p���Ȃ�
    dcb.fDtrControl = DTR_CONTROL_DISABLE;     // DTR�L��/����:DTR����
    dcb.fRtsControl = RTS_CONTROL_ENABLE;      // RTS�t���[����:RTS����Ȃ�
   // dcb.fOutxCtsFlow = TRUE;                 // CTS�t���[����:�t���[���䂠��
   // dcb.fOutxDsrFlow = TRUE;                 // DSR�n�[�h�E�F�A�t���[����F�g�p����
   // dcb.fDtrControl = DTR_CONTROL_ENABLE;    // DTR�L��/����:DTR�L��
   // dcb.fRtsControl = RTS_CONTROL_ENABLE;    // RTS�t���[����:RTS���䂠��
   
    dcb.fOutX = FALSE;                         // ���M��XON/XOFF����̗L��:�Ȃ�
    dcb.fInX = FALSE;                          // ��M��XON/XOFF����̗L��:�Ȃ�
    dcb.fTXContinueOnXoff = TRUE;              // ��M�o�b�t�@�[���t��XOFF��M��̌p�����M��:���M��
    dcb.XonLim = 512;                          // XON��������܂łɊi�[�ł���ŏ��o�C�g��:512
    dcb.XoffLim = 512;                         // XOFF��������܂łɊi�[�ł���ŏ��o�C�g��:512
    dcb.XonChar = 0x11;                        // ���M��XON���� ( ���M�F�r�W�B���� ) �̎w��:XON�����Ƃ���11H ( �f�o�C�X����P�FDC1 )
    dcb.XoffChar = 0x13;                       // XOFF�����i���M�s�F�r�W�[�ʍ��j�̎w��:XOFF�����Ƃ���13H ( �f�o�C�X����3�FDC3 )
   
    dcb.fNull = FALSE;                         // NULL�o�C�g�̔j��:�j�����Ȃ�
    dcb.fAbortOnError = TRUE;                  // �G���[���̓ǂݏ�������I��:�I������
    dcb.fErrorChar = FALSE;                    // �p���e�B�G���[�������̃L�����N�^�iErrorChar�j�u��:�Ȃ�
    dcb.ErrorChar = 0x00;                      // �p���e�B�G���[�������̒u���L�����N�^
    dcb.EofChar = 0x03;                        // �f�[�^�I���ʒm�L�����N�^:��ʂ�0x03(ETX)���悭�g���܂��B
    dcb.EvtChar = 0x02;                        // �C�x���g�ʒm�L�����N�^:��ʂ�0x02(STX)���悭�g���܂�
   
    check = SetCommState(hComPort, &dcb);      // �ݒ�l�̏�������
    if (check == FALSE){//�G���[�`�F�b�N
        fprintf(stderr, "COM�|�[�g�\�����̕ύX�Ɏ��s���܂���.\r\n");
        CloseHandle(hComPort);
        return 0;
    } else{
         fprintf(stderr, "COM�|�[�g�\������ύX���܂���.\r\n");
    }


    // 5.�^�C���A�E�g���Ԃ̐ݒ�
    // �\�����̐ݒ�Ǝ����菇�ł�
    // COMMTIMEOUTS�\���̂��g���AGetCommTimeouts()�֐��Ŋ�{�����擾���A
    // �ύX�����̂�SetCommTimeouts()�֐��ŏ��������Ă��܂�
    // �e�l�͂悭�m��܂��񂪂���Ȋ����������ł��B

    COMMTIMEOUTS TimeOut;                            // COMMTIMEOUTS�\���̂̕ϐ���錾
    GetCommTimeouts(hComPort, &TimeOut);             // �^�C���A�E�g�̐ݒ��Ԃ��擾
   
    //(��M�g�[�^���^�C���A�E�g) = ReadTotalTimeoutMultiplier �~ (��M�\��o�C�g��) + ReadTotalTimeoutConstant
    TimeOut.ReadTotalTimeoutMultiplier = 0;          // �Ǎ��̂P����������̎���:�^�C���A�E�g�Ȃ�
    TimeOut.ReadTotalTimeoutConstant = 1;         // �Ǎ��G���[���o�p�̃^�C���A�E�g����

    //(���M�g�[�^���^�C���A�E�g) = WriteTotalTimeoutMultiplier �~(���M�\��o�C�g��) + WriteTotalTimeoutConstant
    TimeOut.WriteTotalTimeoutMultiplier = 0;         // �������݂P����������̑҂�����:�^�C���A�E�g�Ȃ�
    TimeOut.WriteTotalTimeoutConstant = 1000;        // �������݃G���[���o�p�̃^�C���A�E�g����
   
    check = SetCommTimeouts(hComPort, &TimeOut);     // �^�C���A�E�g�ݒ�̏�������

    if (check == FALSE){//�G���[�`�F�b�N
        fprintf(stderr, "�^�C���A�E�g�̐ݒ�Ɏ��s���܂���.\r\n");
        CloseHandle(hComPort);
        return 0;
    }
    else{
         fprintf(stderr, "�^�C���A�E�g�̐ݒ�ɐ������܂���.\r\n");
    }


    // 6.���M
    // WriteFile()�֐���p���đ��M
    // ���M����f�[�^�T�C�Y��NULL�����̕�+1����̂�Y��Ȃ�

    int  port;
    int  dstSocket, srcSocket;
    struct sockaddr_in dstAddr, srcAddr;
    int  dstAddrSize;

    // �e��p�����[�^
    char buffer[1024];

    // Windows �̏ꍇ
    WSADATA data;
    WSAStartup(MAKEWORD(2,0), &data);


    port = source_port;
    // sockaddr_in �\���̂̃Z�b�g
    memset(&srcAddr, 0, sizeof(srcAddr));
    srcAddr.sin_port = htons(port);
    srcAddr.sin_family = AF_INET;
    srcAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    dstAddrSize = sizeof(dstAddr);


    // �\�P�b�g�̐���
    srcSocket = socket(AF_INET, SOCK_STREAM, 0);
    srcSocket01 = srcSocket;
    // �\�P�b�g�̃o�C���h
    bind(srcSocket, (struct sockaddr *) &srcAddr, sizeof(srcAddr));

    // �ڑ��̋���
    listen(srcSocket, 10);

    //�ڑ�
    endflag01 = 0;
    while(1){   //���[�v�ŉ񂷂��Ƃɂ���ĉ��x�ł��N���C�A���g����Ȃ����Ƃ��ł���
        unsigned int _stdcall ConsoleToSerial(void *data);
        HANDLE   hThread01 = NULL;
        struct   param01 *p01;

        printf("�|�[�g:%d�Őڑ���҂��Ă��܂�\n�N���C�A���g�v���O�����𓮂����Ă�������\n", port);
        dstSocket = accept(srcSocket, (struct sockaddr *) &dstAddr, &dstAddrSize);
        dstSocket01 = dstSocket;
        if (dstSocket > 0) {
            printf("%s ����ڑ����󂯂܂��� socket = %d\n", inet_ntoa(dstAddr.sin_addr), dstSocket);

            execflag01 = 1;
            p01 = (struct param01 *) malloc(sizeof(struct param01));
            p01->hComPort = hComPort;
            p01->pendflag01 = &endflag01;
            p01->pexecflag01 = &execflag01;
            p01->dstSocket = dstSocket;
            hThread01 = (HANDLE)_beginthreadex(NULL, 0, &ConsoleToSerial, (void *)p01, 0, NULL);
        }

        while ((execflag01 != 0) && (endflag01 == 0)) {
            Sleep((DWORD)10);
        };

        if (dstSocket01 > 0) closesocket(dstSocket01);

        if (hThread01 != NULL) {
            // �X���b�h�I���҂�
            WaitForSingleObject(hThread01, INFINITE);
            // �X���b�h�폜
            CloseHandle(hThread01);
            hThread01 = NULL;
        }
        if (endflag01 != 0) break;
    }

    if (srcSocket01 > 0) closesocket(srcSocket01);

    WSACleanup();

    return 0;
}

unsigned int _stdcall ConsoleToSerial(void *data) {

    char buffer[512], buffer2[512];
    char crlfbuf[3] = { 0x0d, 0x0a, 0x00 };
    int SendSize;
    DWORD writeSize, readSize;
    HANDLE hComPort;
    int dstSocket;
    char *bufpos;
    struct param01 *p01;
    int numrcv, numrcv2;
    int rcv_data_size, rcv_data_size2;
    int count, count1, count2, count21;
    int i;

    struct timeval t_val = {0, 0};    // 1/1000�b
    int select_ret;
    fd_set fds_r, readfds;
    fd_set fds_w, writefds;

    count = 0;
    count1 = 0;
    count2 = 0;
    count21 = 0;
    rcv_data_size = 0;
    rcv_data_size2 = 0;
    p01 = (struct param01 *)data;
    hComPort = p01->hComPort;
    dstSocket = p01->dstSocket;
    *(p01->pexecflag01) = 1;

    FD_ZERO(&readfds);
    FD_SET(dstSocket, &readfds);

    while(1) {
        int write_num = 0;
        int i_num01;

        if (*(p01->pendflag01) != 0) {
            break;
        }
        // socket�����1�����ǂݍ���(select���g����1������M�������̂�recv����)
        memcpy(&fds_r, &readfds, sizeof(fd_set));
        select_ret = select(0, &fds_r, NULL, NULL, &t_val);
        if (select_ret == SOCKET_ERROR) break;
        if (select_ret != 0) {
            numrcv = recv(dstSocket, buffer, 6, 0);
            if (numrcv == 0 || numrcv == -1) {
                goto goto_label01;
            }
            if (numrcv > 0) {
                write_num = 0;
                rcv_data_size += numrcv;
                bufpos = buffer;
                SendSize = 1;
                for (i_num01 = 0; i_num01 < 10; i_num01++) {
                    WriteFile(hComPort, bufpos + write_num, numrcv - write_num, &writeSize, NULL);
                    write_num += writeSize;
                    if ((write_num - numrcv) >= 0) break;
                }
                FlushFileBuffers(hComPort);
                if (count >= 100) {
                    count = 0;
                    count1++;
                    printf("send data = %d\n", rcv_data_size);
                }
                count++;
            }
        }
        if (*(p01->pendflag01) != 0) {
            break;
        }

        // serial�����1�����ǂݍ���
        ReadFile(hComPort, buffer2, 512, &readSize, 0); // �V���A���|�[�g�ɑ΂���ǂݍ���
        numrcv2 = readSize;
        if (numrcv2 > 0) {
            rcv_data_size2 += numrcv2;
            send(dstSocket, buffer2, numrcv2, 0);
            if (count2 >= 100) {
                count2 = 0;
                count21++;
                printf("serial recv data = %d\n", rcv_data_size2);
            }
            count2++;
        }
    }
    goto_label01:
    printf("�ڑ����I�����܂���\n\n");
    *(p01->pexecflag01) = 0;
    free(p01);
    _endthreadex(0);
    return 0;
}


BOOL WINAPI CtrlHandler(DWORD fdwCtrlType);

int main(int argc, char **argv)
{

    char *char01;
  
  
    if (argc != 3) {
        fprintf(stderr, "Usage: %s  com_port_number port_number\n", argv[0]);
        exit(0);
    }
    source_port = atoi(argv[2]);
  
    char01 = argv[1];
  
    if (!strcmp(char01, "1")) {
        tchar01 = _T("COM1");
    } else if (!strcmp(char01, "2")) {
        tchar01 = _T("COM2");
    } else if (!strcmp(char01, "3")) {
        tchar01 = _T("COM3");
    } else if (!strcmp(char01, "4")) {
        tchar01 = _T("COM4");
    } else if (!strcmp(char01, "5")) {
        tchar01 = _T("COM5");
    } else if (!strcmp(char01, "6")) {
        tchar01 = _T("COM6");
    } else if (!strcmp(char01, "7")) {
        tchar01 = _T("COM7");
    } else if (!strcmp(char01, "8")) {
        tchar01 = _T("COM8");
    } else if (!strcmp(char01, "9")) {
        tchar01 = _T("COM9");
    } else if (!strcmp(char01, "10")) {
        tchar01 = _T("\\\\.\\COM10");
    } else if (!strcmp(char01, "11")) {
        tchar01 = _T("\\\\.\\COM11");
    } else if (!strcmp(char01, "12")) {
        tchar01 = _T("\\\\.\\COM12");
    } else if (!strcmp(char01, "13")) {
        tchar01 = _T("\\\\.\\COM13");
    } else if (!strcmp(char01, "14")) {
        tchar01 = _T("\\\\.\\COM14");
    } else if (!strcmp(char01, "15")) {
        tchar01 = _T("\\\\.\\COM15");
    } else if (!strcmp(char01, "16")) {
        tchar01 = _T("\\\\.\\COM16");
    } else if (!strcmp(char01, "17")) {
        tchar01 = _T("\\\\.\\COM17");
    } else if (!strcmp(char01, "18")) {
        tchar01 = _T("\\\\.\\COM18");
    } else if (!strcmp(char01, "19")) {
        tchar01 = _T("\\\\.\\COM19");
    } else if (!strcmp(char01, "20")) {
        tchar01 = _T("\\\\.\\COM20");
    } else if (!strcmp(char01, "21")) {
        tchar01 = _T("\\\\.\\COM21");
    } else if (!strcmp(char01, "22")) {
        tchar01 = _T("\\\\.\\COM22");
    } else if (!strcmp(char01, "23")) {
        tchar01 = _T("\\\\.\\COM23");
    } else if (!strcmp(char01, "24")) {
        tchar01 = _T("\\\\.\\COM24");
    } else if (!strcmp(char01, "25")) {
        tchar01 = _T("\\\\.\\COM25");
    } else if (!strcmp(char01, "26")) {
        tchar01 = _T("\\\\.\\COM26");
    } else if (!strcmp(char01, "27")) {
        tchar01 = _T("\\\\.\\COM27");
    } else if (!strcmp(char01, "28")) {
        tchar01 = _T("\\\\.\\COM28");
    } else if (!strcmp(char01, "29")) {
        tchar01 = _T("\\\\.\\COM29");
    } else if (!strcmp(char01, "30")) {
        tchar01 = _T("\\\\.\\COM30");
    } else if (!strcmp(char01, "31")) {
        tchar01 = _T("\\\\.\\COM31");
    } else if (!strcmp(char01, "32")) {
        tchar01 = _T("\\\\.\\COM32");
    } else if (!strcmp(char01, "33")) {
        tchar01 = _T("\\\\.\\COM33");
    } else if (!strcmp(char01, "34")) {
        tchar01 = _T("\\\\.\\COM34");
    } else if (!strcmp(char01, "35")) {
        tchar01 = _T("\\\\.\\COM35");
    } else if (!strcmp(char01, "36")) {
        tchar01 = _T("\\\\.\\COM36");
    } else if (!strcmp(char01, "37")) {
        tchar01 = _T("\\\\.\\COM37");
    } else if (!strcmp(char01, "38")) {
        tchar01 = _T("\\\\.\\COM38");
    } else if (!strcmp(char01, "39")) {
        tchar01 = _T("\\\\.\\COM39");
    } else {
        fprintf(stderr, "com_port_number���s���ł�\n");
        exit(0);
    }
  
    SetConsoleCtrlHandler(CtrlHandler, TRUE);
  
    serial01();

    CloseHandle(hComPort); 
    exit(0);
}

BOOL WINAPI CtrlHandler(DWORD fdwCtrlType) {
    switch (fdwCtrlType)
    {
        // Handle the CTRL-C signal.
    case CTRL_C_EVENT:
        CloseHandle(hComPort); 
        closesocket(dstSocket01);
        dstSocket01 = -1;
        closesocket(srcSocket01);
        srcSocket01 = -1;
        endflag01 = 1;

        return TRUE;

        // CTRL-CLOSE: confirm that the user wants to exit.
    case CTRL_CLOSE_EVENT:
        CloseHandle(hComPort); 
        closesocket(dstSocket01);
        dstSocket01 = -1;
        closesocket(srcSocket01);
        srcSocket01 = -1;
        endflag01 = 1;

        return TRUE;

        // Pass other signals to the next handler.
    case CTRL_BREAK_EVENT:
        CloseHandle(hComPort); 
        closesocket(dstSocket01);
        dstSocket01 = -1;
        closesocket(srcSocket01);
        srcSocket01 = -1;
        endflag01 = 1;

        return TRUE;

    case CTRL_LOGOFF_EVENT:
        CloseHandle(hComPort); 
        closesocket(dstSocket01);
        dstSocket01 = -1;
        closesocket(srcSocket01);
        srcSocket01 = -1;
        endflag01 = 1;

        return TRUE;

    case CTRL_SHUTDOWN_EVENT:
        CloseHandle(hComPort); 
        closesocket(dstSocket01);
        dstSocket01 = -1;
        closesocket(srcSocket01);
        srcSocket01 = -1;
        endflag01 = 1;

        return TRUE;

    default:
        return TRUE;
    }
}
