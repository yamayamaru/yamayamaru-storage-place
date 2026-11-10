/* realmode bios call test */

#include <stdio.h>
#include "newlib_stub.h"
#include "egb.h"


int main(int argc, char *argv[])
{
    unsigned int segment01;
    if (allocate_msdos_memory_block(1024, &segment01) == 0) {
        union REGS regs;
        struct SREGS sregs;

        printf("allocate_msdos_memory_block success!  segment01 = 0x%08x\r\n", segment01);

        regs.h.ah = 0x01;
        regs.x.di = 0;
        sregs.ds = segment01;
        intreal(0x96, &regs, &regs, &sregs);

        if (regs.h.ah == 0) {
            int year, mon, day;
            int hour, min, sec, h;
            unsigned int addr;

            addr = (segment01 << 4) + 0;
            year = peekw_far(0x0034, addr + 0);
            mon  = peekb_far(0x0034, addr + 2);
            day  = peekb_far(0x0034, addr + 3);
            hour = peekb_far(0x0034, addr + 5);
            min  = peekb_far(0x0034, addr + 6);
            sec  = peekb_far(0x0034, addr + 7);
            h  = peekb_far(0x0034, addr + 8);

            printf("int0x96, ah=0x01 success!\r\n");
            printf("%d/%02d/%02d %02d:%02d:%02d.%02d\r\n", year, mon, day, hour, min, sec, h);

        } else {
            printf("int0x96, ah=0x01 failed!\r\n");
        }

        free_msdos_memory_block(segment01);
    } else {
        printf("allocate_msdos_memory_block failed!\r\n");
    }

    return 0;
}
