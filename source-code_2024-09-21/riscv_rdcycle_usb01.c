#pragma GCC optimize ("Os")

#include "pico/stdlib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hardware/structs/systick.h"
#include "hardware/clocks.h"


int main() {
    int mcounteren01, mcountinhibit01;

    stdio_init_all();

    // counter enable
    asm volatile (
        "csrr t0, mcountinhibit\n\t"
        "mv   %[mcountinhibit01], t0\n\t"
        "li   t1, 1\n\t"
        "not  t1, t1\n\t"
        "and  t0, t0, t1\n\t"
        "csrw mcountinhibit, t0\n\t"
        "csrr t0, mcounteren\n\t"
        "mv   %[mcounteren01], t0\n\t"
        "ori  t0, t0, 1\n\t"
        "csrw mcounteren, t0\n\t"
        :[mcounteren01]"=r"(mcounteren01), [mcountinhibit01]"=r"(mcountinhibit01)
        :
        :"t0", "t1"
    );


    while (1) {
        int32_t t001, t002, t;

        asm volatile (
            "rdcycle    %[t001]\n\t"

            "add t0, t0, t1\n\t"
            "add t0, t0, t1\n\t"
            "add t0, t0, t1\n\t"
            "add t0, t0, t1\n\t"
            "add t0, t0, t1\n\t"
            "add t0, t0, t1\n\t"
            "add t0, t0, t1\n\t"
            "add t0, t0, t1\n\t"
            "add t0, t0, t1\n\t"
            "add t0, t0, t1\n\t"

            "rdcycle    %[t002]\n\t"
            :[t001] "=r"(t001), [t002] "=r"(t002)
            :
            :"t0", "t1"
        );
        printf("mcounteren    = 0x%08x\n", mcounteren01);
        printf("mcountinhibit = 0x%08x\n", mcountinhibit01);

        t = t002 - t001;
        printf("t001 = %d\n", t001);
        printf("t002 = %d\n", t002);
        printf("t001 - t002 = %d\n", t);
        printf("\n");
        printf("clock_get_hz(clk_sys) = %d\n", clock_get_hz(clk_sys));
        printf("\n");


        sleep_ms(5000);
    }

    return 0;
}
