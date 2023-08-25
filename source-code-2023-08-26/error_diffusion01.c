#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define XDOT 256
#define YDOT 212
#define ERROR01_XDOT XDOT + 1
#define ERROR01_YDOT YDOT + 1

uint8_t bitmap01[XDOT * YDOT * 3];
uint8_t bitmap02[XDOT * YDOT];
struct error01_rgb {
    double r;
    double g;
    double b;
};
struct error01_rgb error01[(XDOT+1) * (YDOT+1)];

uint8_t vr[8] = {0,  36,  73, 109, 146, 182, 219, 255};
uint8_t vg[8] = {0,  36,  73, 109, 146, 182, 219, 255};
uint8_t vb[8] = {0,  36,  73, 109, 146, 182, 219, 255};

uint8_t tr[7] = {18,  54,  91, 127, 164, 200, 237};
uint8_t tg[7] = {18,  54,  91, 127, 164, 200, 237};
uint8_t tb[7] = {18,  54,  91, 127, 164, 200, 237};

void load_bitmap_256x212_24bit();
void save_bitmap_256x212_8bit();
void setup();

int main(int argc, char *argv[]) {

    char *filename, *filename2;
    if (argc != 3) {
        fprintf(stderr, "Usage : %s filename1 filename2\n", argv[0]);
        fprintf(stderr, "    filename1 : 256x212 24bit raw bitmap data\n", argv[0]);
        fprintf(stderr, "    filename2 : 256x212  8bit raw bitmap data\n", argv[0]);
        return 0;
    }
    filename = argv[1];
    filename2 = argv[2];

    load_bitmap_256x212_24bit(filename);
    setup();
    save_bitmap_256x212_8bit(filename2);

    return 0;
}

int abs01(int a) {
    if (a < 0) a = -a;
    return a;
}

int on_board(int x, int y) {
    return (0 <= x && x < XDOT && 0 <= y && y < YDOT);
}

void set_pixel(int x, int y, uint8_t r, uint8_t g, uint8_t b) {
    uint8_t pixel01;
    pixel01 = ((g & 0x7) << 5) | ((r & 0x7) << 2) | ((b & 7) >> 1);
    bitmap02[x + y * XDOT] = pixel01;
}

void setup() {
    int r, g, b, ir, ig, ib;
    uint8_t v;
    uint8_t v1, v2, v3;
    double e;

    for (int i = 0; i < ERROR01_XDOT*ERROR01_YDOT; i++) {
        error01[i].r = 0.0;
        error01[i].g = 0.0;
        error01[i].b = 0.0;
    }

    for (int y = 0; y < YDOT; y++) {
        for (int x = 0; x < XDOT; x++) {
            r = bitmap01[(x + y * XDOT) * 3 + 0] + error01[(x + y * ERROR01_XDOT)].r;
            g = bitmap01[(x + y * XDOT) * 3 + 1] + error01[(x + y * ERROR01_XDOT)].g;
            b = bitmap01[(x + y * XDOT) * 3 + 2] + error01[(x + y * ERROR01_XDOT)].b;

            for (ir = 0; ir < 7; ir++) {
                if(abs01(tr[ir] - r) <= 19) break;
            }
            if (ir >= 8) ir = 7;
            v = vr[ir];
            v1 = ir;

            e = (r - v) / 16.0;

            if (on_board(x + 1, y)) {
                error01[(y * ERROR01_XDOT + (x + 1))].r += e * 7.0;
            }
            if (on_board(x - 1, y + 1)) {
                error01[((y + 1) * ERROR01_XDOT + (x - 1))].r += e * 3.0;
            }
            if (on_board(x, y + 1)) {
                error01[((y + 1) * ERROR01_XDOT + x)].r += e * 5.0;
            }
            if (on_board(x + 1, y + 1)) {
                error01[((y + 1) * ERROR01_XDOT + (x + 1))].r += e;
            }

            for (ig = 0; ig < 7; ig++) {
                if(abs01(tg[ig] - g) <= 19) break;
            }
            if (ig >= 8) ig = 7;
            v = vg[ig];
            v2 = ig;

            e = (g - v) / 16.0;

            if (on_board(x + 1, y)) {
                error01[(y * ERROR01_XDOT + (x + 1))].g += e * 7.0;
            }
            if (on_board(x - 1, y + 1)) {
                error01[((y + 1) * ERROR01_XDOT + (x - 1))].g += e * 3.0;
            }
            if (on_board(x, y + 1)) {
                error01[((y + 1) * ERROR01_XDOT + x)].g += e * 5.0;
            }
            if (on_board(x + 1, y + 1)) {
                error01[((y + 1) * ERROR01_XDOT + (x + 1))].g += e;
            }

            for (ib = 0; ib < 7; ib++) {
                if(abs01(tb[ib] - b) <= 19) break;
            }
            if (ib >= 8) ib = 7;
            v = vb[ib];
            v3 = ib;

            e = (b - v) / 16.0;

            if (on_board(x + 1, y)) {
                error01[(y * ERROR01_XDOT + (x + 1))].b += e * 7.0;
            }
            if (on_board(x - 1, y + 1)) {
                error01[((y + 1) * ERROR01_XDOT + (x - 1))].b += e * 3.0;
            }
            if (on_board(x, y + 1)) {
                error01[((y + 1) * ERROR01_XDOT + x)].b += e * 5.0;
            }
            if (on_board(x + 1, y + 1)) {
                error01[((y + 1) * ERROR01_XDOT + (x + 1))].b += e;
            }

            set_pixel(x, y, v1, v2, v3);
        }
    }
}

void save_bitmap_256x212_8bit(char *filename) {
    FILE *fp01;
    struct bsave01struct {
        uint16_t start;
        uint16_t end;
        uint16_t exec;
    };
    union bsave01union {
        struct bsave01struct bsave;
        uint8_t a[6];
    } bsave01;
    
    bsave01.bsave.start = 0;
    bsave01.bsave.end = 256 * 212 - 1;
    bsave01.bsave.exec = 0;

    if ((fp01 = fopen(filename, "wb")) == NULL) {
        fprintf(stderr, "ファイル書き込みエラー : %s\n", filename);
        return;
    }

    fputc(0xfe, fp01);

    for (int i = 0; i < 6; i++) {
        fputc(bsave01.a[i], fp01);
    }

    for (int i = 0; i < XDOT * YDOT; i++) {
        fputc(bitmap02[i], fp01);
    }
    fclose(fp01);
}

void load_bitmap_256x212_24bit(char *filename) {
    FILE *fp01;

    if ((fp01 = fopen(filename, "rb")) == NULL) {
        fprintf(stderr, "ファイル読み込みエラー : %s\n", filename);
        return;
    }
    for (int i = 0; i < XDOT * YDOT * 3; i++) {
        bitmap01[i] = fgetc(fp01);
    }
    fclose(fp01);
}
