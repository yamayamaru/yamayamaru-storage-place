#include <stdio.h>
#include <math.h>
#include <glib.h>
#include <conio.h>


struct m_para {
    XDOUBLE    xcorner;
    XDOUBLE    ycorner;
    XDOUBLE    length;
    int       depth;
} m_pa01[21];

VOID s_pal01();
VOID mandel(xcorner, ycorner, length, xdot, ydot, depth);
int mand(a1, b1, depth1);

int flag01 = 0;

int main(argc, argv)
int   argc;
char  **argv;
{

    XDOUBLE    xcorner, ycorner, length, temp01;
    int       xdot, ydot, depth;
    XDOUBLE    start_time, end_time;
    int       number01;

    s_pal01();


    ginit();
    screen((TINY)8);
    cls();
    boxfill(0,0,255,211, (TINY)0, PSET);


    xdot = 100;
    ydot = 100;


    for (number01 = 0; number01 < 1; number01++){
        xdcpy(&xcorner, &m_pa01[number01].xcorner);         /* xcorner = m_pa01[number01].xcorner; */
        xdcpy(&ycorner, &m_pa01[number01].ycorner);         /* ycorner = m_pa01[number01].ycorner; */
        xdcpy(&length, &m_pa01[number01].length);           /* length  = m_pa01[number01].length; */
        depth   = m_pa01[number01].depth;
        depth = 100;

        mandel(&xcorner, &ycorner, &length, xdot, ydot, depth);
    }

    if(flag01 == 0) while (kbhit() == 0);

    screen((TINY)0);

}

VOID mandel(pxcorner, pycorner, plength, xdot, ydot, depth)
XDOUBLE *pxcorner;
XDOUBLE *pycorner;
XDOUBLE *plength;
int xdot;
int ydot;
int depth;
{
    XDOUBLE xgap, ygap, xcorner, ycorner, length;
    XDOUBLE xx, yy;
    XDOUBLE temp01;
    int    i, j;
    int    col01, colr, colg, colb;

    xdcpy(&xcorner, pxcorner);
    xdcpy(&ycorner, pycorner);
    xdcpy(&length, plength);

    itoxd(&temp01, xdot);            /* xgap = length / xdot; */
    xddiv(&xgap, &length, &temp01);

    itoxd(&temp01, xdot);            /* ygap = length / xdot; */
    xddiv(&ygap, &length, &temp01);

    xdcpy(&xx, &xcorner);             /* xx = xcorner; */
    xdcpy(&yy, &ycorner);             /* yy = ycorner; */
    for(j = 0; j < ydot; j++) {
        if (flag01 != 0) break;
        for(i = 0; i < xdot; i++) {
            flag01 = kbhit();
            if (flag01 != 0) break;
            col01 = (short) mand( &xx, &yy, depth);
            if (col01 == 0){
                colr = 0;
                colg = 0;
                colb = 0;
            } else {
                colr = (col01 % 32) * 8 + 7;
                colg = (col01 % 32) * 8 + 7;
                colb = (col01 % 32) * 4 + 128;
            }
            pset(i, j, (TINY)col01, PSET);


            xdadd(&xx, &xx, &xgap);   /* xx = xx + xgap; */
        }
        xdadd(&yy, &yy, &ygap);    /* yy = yy + ygap; */
        xdcpy(&xx, &xcorner);       /* xx = xcorner; */
    }
}




int mand(aa1, bb1, depth1)
XDOUBLE *aa1;
XDOUBLE *bb1;
int depth1;
{
    int     i1, cmp01;
    XDOUBLE  x1, y1, tmp1, x2, y2, temp01, a1, b1, r1;

    xdcpy(&a1, aa1);  xdcpy(&b1, bb1);

    i1 = 0;
    atoxd(&x1, "0.0");     atoxd(&y1, "0.0");         /* x1 = 0; y1 = 0; */
    xdmul(&x2, &x1, &x1);  xdmul(&y2, &y1, &y1);      /* x2 = x1 * x1;   y2 = y1 * y1; */

    /*     while ((i1 < depth1) && (x2+y2 <=4)) { */
    xdadd(&temp01, &x2, &y2);
    atoxd(&r1, "4.0");
    cmp01 = xdcmp(&temp01, &r1);
    while ((i1 < depth1) && (!(cmp01 > 0))) {
        xdsub(&temp01, &x2, &y2);       /* tmp1 = x2 - y2 + a1; */
        xdadd(&tmp1, &temp01, &a1);
        atoxd(&temp01, "2.0");          /* y1 = 2 * x1 * y1 + b1; */
        xdmul(&temp01, &temp01, &x1);
        xdmul(&temp01, &temp01, &y1);
        xdadd(&y1, &temp01, &b1);
        xdcpy(&x1, &tmp1);              /* x1 = tmp1; */
        xdmul(&x2, &x1, &x1);           /* x2 = x1 * x1; */
        xdmul(&y2, &y1, &y1);           /* y2 = y1 * y1; */
        i1++;

        xdadd(&temp01, &x2, &y2);       /* (x2+y2 <=4) */
        atoxd(&r1, "4.0");
        cmp01 = xdcmp(&temp01, &r1);
    }

    if (i1 == depth1) i1 = 0;
    else            i1++;
    return i1;
}

VOID    s_pal01(){

        /*
        m_pa01[0].xcorner = -2.4;                        m_pa01[00].ycorner = -1.7;
        m_pa01[0].length  =  3.2;                        m_pa01[00].depth   = 1000;
        */
        atoxd(&m_pa01[0].xcorner, "-2.4");
        atoxd(&m_pa01[0].ycorner, "-1.7");
        atoxd(&m_pa01[0].length,  "3.2");
        m_pa01[0].depth = 1000;
        /*
        m_pa01[1].xcorner = -6.1099999999999965e-001;    m_pa01[01].ycorner = 6e-001;
        m_pa01[1].length  =  1.0000000000000001e-001;    m_pa01[01].depth   = 1000;
        */
        atoxd(&m_pa01[1].xcorner, "-6.1099999999999965e-001");
        atoxd(&m_pa01[1].ycorner, "6e-001");
        atoxd(&m_pa01[1].length,  "1.0000000000000001e-001");
        m_pa01[1].depth = 1000;
        /*
        m_pa01[2].xcorner = -1.7878664118448890e+000;    m_pa01[02].ycorner = -4.6994043985999932e-003;
        m_pa01[2].length  =  1.5258789062500001e-006;    m_pa01[02].depth   = 1000;
        */
        atoxd(&m_pa01[2].xcorner, "-1.7878664118448890e+000");
        atoxd(&m_pa01[2].ycorner, "-4.6994043985999932e-003");
        atoxd(&m_pa01[2].length,  "1.5258789062500001e-006");
        m_pa01[2].depth = 1000;
        /*
        m_pa01[3].xcorner =  3.6540625000000060e-001;    m_pa01[03].ycorner =  1.1625000000000031e-001;
        m_pa01[3].length  =  6.2500000000000003e-003;    m_pa01[03].depth   = 1000;
        */
        atoxd(&m_pa01[3].xcorner, "3.6540625000000060e-001");
        atoxd(&m_pa01[3].ycorner, "1.1625000000000031e-001");
        atoxd(&m_pa01[3].length,  "6.2500000000000003e-003");
        m_pa01[3].depth = 1000;
        /*
        m_pa01[4].xcorner = -1.6247249348958287e-001;    m_pa01[04].ycorner = -1.0341339518229167e+000;
        m_pa01[4].length  =  2.4414062500000001e-005;    m_pa01[04].depth   = 1000;
        */
        atoxd(&m_pa01[4].xcorner, "-1.6247249348958287e-001");
        atoxd(&m_pa01[4].ycorner, "-1.0341339518229167e+000");
        atoxd(&m_pa01[4].length,  "2.4414062500000001e-005");
        m_pa01[4].depth = 1000;

/*
        m_pa01[5].xcorner = -7.496979166666662e-001;     m_pa01[5].ycorner = 8.252083333333364e-002;
        m_pa01[5].length  =  3.125000000000000e-003;     m_pa01[5].depth   = 1000;
*/
        atoxd(&m_pa01[5].xcorner, "-7.496979166666662e-001");
        atoxd(&m_pa01[5].ycorner, "8.252083333333364e-002");
        atoxd(&m_pa01[5].length,  "3.125000000000000e-003");
        m_pa01[5].depth = 1000;
/*
        m_pa01[6].xcorner = -1.942551920572915e+000;     m_pa01[6].ycorner = -2.457682291648995e-005;
        m_pa01[6].length  =  4.882812500000000e-005;     m_pa01[6].depth   = 1000;
*/
        atoxd(&m_pa01[6].xcorner, "-1.942551920572915e+000");
        atoxd(&m_pa01[6].ycorner, "-2.457682291648995e-005");
        atoxd(&m_pa01[6].length,  "4.882812500000000e-005");
        m_pa01[6].depth = 1000;
/*
        m_pa01[7].xcorner = -1.7860575510660803e+000;    m_pa01[7].ycorner = 1.7055765787787343e-004;
        m_pa01[7].length  =  6.1035156250000003e-006;    m_pa01[7].depth   = 1000;
*/
        atoxd(&m_pa01[7].xcorner, "-1.7860575510660803e+000");
        atoxd(&m_pa01[7].ycorner, "1.7055765787787343e-004");
        atoxd(&m_pa01[7].length,  "6.1035156250000003e-006");
        m_pa01[7].depth = 1000;
/*
        m_pa01[8].xcorner = -1.7860434722900387e+000;    m_pa01[8].ycorner = 1.6493225097683180e-004;
        m_pa01[8].length  =  1.5258789062500001e-006;    m_pa01[8].depth   = 1000;
*/
        atoxd(&m_pa01[8].xcorner, "-1.7860434722900387e+000");
        atoxd(&m_pa01[8].ycorner, "1.6493225097683180e-004");
        atoxd(&m_pa01[8].length,  "1.5258789062500001e-006");
        m_pa01[8].depth = 1000;
/*
        m_pa01[9].xcorner =  3.0717668660481867e-001;    m_pa01[9].ycorner = 3.1935620625813711e-002;
        m_pa01[9].length  =  9.7656250000000005e-005;    m_pa01[9].depth   = 1000;
*/
        atoxd(&m_pa01[9].xcorner, "3.0717668660481867e-001");
        atoxd(&m_pa01[9].ycorner, "3.1935620625813711e-002");
        atoxd(&m_pa01[9].length,  "9.7656250000000005e-005");
        m_pa01[9].depth = 1000;

/*
        m_pa01[10].xcorner =  -7.513718439737953e-001;    m_pa01[10].ycorner = 2.933079783121612e-002;
        m_pa01[10].length  =   6.103515625000000e-006;    m_pa01[10].depth   = 1000;
*/
        atoxd(&m_pa01[10].xcorner, "-7.513718439737953e-001");
        atoxd(&m_pa01[10].ycorner, "2.933079783121612e-002");
        atoxd(&m_pa01[10].length,  "6.103515625000000e-006");
        m_pa01[10].depth = 1000;
/*
        m_pa01[11].xcorner =  -7.513724441528318e-001;    m_pa01[11].ycorner = 2.932934315999216e-002;
        m_pa01[11].length  =   1.220703125000000e-005;    m_pa01[11].depth   = 1000;
*/
        atoxd(&m_pa01[11].xcorner, "-7.513724441528318e-001");
        atoxd(&m_pa01[11].ycorner, "2.932934315999216e-002");
        atoxd(&m_pa01[11].length,  "1.220703125000000e-005");
        m_pa01[11].depth = 1000;
/*
        m_pa01[12].xcorner =  -7.512120844523107e-001;    m_pa01[12].ycorner = 2.939359283447132e-002;
        m_pa01[12].length  =   2.441406250000000e-005;    m_pa01[12].depth   = 1000;
*/
        atoxd(&m_pa01[12].xcorner, "-7.512120844523107e-001");
        atoxd(&m_pa01[12].ycorner, "2.939359283447132e-002");
        atoxd(&m_pa01[12].length,  "2.441406250000000e-005");
        m_pa01[12].depth = 1000;
/*
        m_pa01[13].xcorner =  -1.255153877258301e+000;    m_pa01[13].ycorner = 3.834366226196277e-001;
        m_pa01[13].length  =   2.441406250000000e-005;    m_pa01[13].depth   = 1000;
*/
        atoxd(&m_pa01[13].xcorner, "-1.255153877258301e+000");
        atoxd(&m_pa01[13].ycorner, "3.834366226196277e-001");
        atoxd(&m_pa01[13].length,  "2.441406250000000e-005");
        m_pa01[13].depth = 1000;
/*
        m_pa01[14].xcorner =  -1.255149571736653e+000;    m_pa01[14].ycorner = 3.834463500976551e-001;
        m_pa01[14].length  =   3.814697265625000e-007;    m_pa01[14].depth   = 1000;
*/
        atoxd(&m_pa01[14].xcorner, "-1.255149571736653e+000");
        atoxd(&m_pa01[14].ycorner, "3.834463500976551e-001");
        atoxd(&m_pa01[14].length,  "3.814697265625000e-007");
        m_pa01[14].depth = 1000;


/*
        m_pa01[15].xcorner =  -1.255150037765502e+000;    m_pa01[15].ycorner = 3.834445966084784e-001;
        m_pa01[15].length  =   3.051757812500000e-006;    m_pa01[15].depth   = 1000;
*/
        atoxd(&m_pa01[15].xcorner, "-1.255150037765502e+000");
        atoxd(&m_pa01[15].ycorner, "3.834445966084784e-001");
        atoxd(&m_pa01[15].length,  "3.051757812500000e-006");
        m_pa01[15].depth = 1000;
/*
        m_pa01[16].xcorner =  -1.255161532719929e+000;    m_pa01[16].ycorner = 3.834325675964341e-001;
        m_pa01[16].length  =   2.441406250000000e-005;    m_pa01[16].depth   = 1000;
*/
        atoxd(&m_pa01[16].xcorner, "-1.255161532719929e+000");
        atoxd(&m_pa01[16].ycorner, "3.834325675964341e-001");
        atoxd(&m_pa01[16].length,  "2.441406250000000e-005");
        m_pa01[16].depth = 1000;
/*
        m_pa01[17].xcorner =  -1.255219516118367e+000;    m_pa01[17].ycorner = 3.833973706563299e-001;
        m_pa01[17].length  =   9.765625000000001e-005;    m_pa01[17].depth   = 1000;
*/
        atoxd(&m_pa01[17].xcorner, "-1.255219516118367e+000");
        atoxd(&m_pa01[17].ycorner, "3.833973706563299e-001");
        atoxd(&m_pa01[17].length,  "9.765625000000001e-005");
        m_pa01[17].depth = 1000;
/*
        m_pa01[18].xcorner =  -1.258991814295450e+000;    m_pa01[18].ycorner = 3.781393953959134e-001;
        m_pa01[18].length  =   6.250000000000000e-003;    m_pa01[18].depth   = 1000;
*/
        atoxd(&m_pa01[18].xcorner, "-1.258991814295450e+000");
        atoxd(&m_pa01[18].ycorner, "3.781393953959134e-001");
        atoxd(&m_pa01[18].length,  "6.250000000000000e-003");
        m_pa01[18].depth = 1000;
/*
        m_pa01[19].xcorner =  -1.255149231592813e+000;    m_pa01[19].ycorner = 3.834458554585760e-001;
        m_pa01[19].length  =   7.629394531250000e-007;    m_pa01[19].depth   = 1000;
*/
        atoxd(&m_pa01[19].xcorner, "-1.255149231592813e+000");
        atoxd(&m_pa01[19].ycorner, "3.834458554585760e-001");
        atoxd(&m_pa01[19].length,  "7.629394531250000e-007");
        m_pa01[19].depth = 1000;

/*
        m_pa01[20].xcorner =  -1.255058807618605e+000;    m_pa01[20].ycorner = 3.834129844820908e-001;
        m_pa01[20].length  =   5.960464477539063e-009;    m_pa01[20].depth   = 1000;
*/
        atoxd(&m_pa01[20].xcorner, "-1.255058807618605e+000");
        atoxd(&m_pa01[20].ycorner, "3.834129844820908e-001");
        atoxd(&m_pa01[20].length,  "5.960464477539063e-009");
        m_pa01[20].depth = 1000;

}
