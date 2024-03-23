void srnd()
{
    extern int RNDSPD[2];

    RNDSPD[1] = *((int *)0xfca2);
}

int sgn(int a) {
    if (a == 0) return 0;
    if (a < 0)  return -1;
    return 1;
}
