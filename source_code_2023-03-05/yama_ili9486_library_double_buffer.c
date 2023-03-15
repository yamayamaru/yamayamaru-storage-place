#include "yama_ili9486_library_double_buffer.h"


int32_t _init_width, _init_height;
int32_t _width, _height;

struct gpiod_chip *gpiod_chip01;
struct gpiod_line *gpiod_dc_lineout;
struct gpiod_line *gpiod_reset_lineout;
struct gpiod_line *gpiod_cs_lineout;
const char *appname = "spitest01";
int gpiod_value01;


int  spidev_fd01 = 0;


uint8_t *canvas24_buffer = NULL;
int32_t canvas24_rotation = 0;
int32_t canvas24_width = 320;
int32_t canvas24_height = 480;
int32_t canvas24_init_width = 320;
int32_t canvas24_init_height = 480;

char *spi_device01;
void spi_init(char *spi_device, char *gpiod_device, int spi_cs, int spi_dc, int spi_reset) {
    // GPIOデバイスを開く
    if ((gpiod_chip01=gpiod_chip_open(gpiod_device)) == NULL) {
        perror("gpiod_chip_open");
        exit(-1);
    }

    // DCのピンのハンドラを取得する
    // ピンはhttps://doc-en.rvspace.org/VisionFive2/PDF/VisionFive2_QSG.pdfで確認できます。
    if ((gpiod_dc_lineout=gpiod_chip_get_line(gpiod_chip01, spi_dc)) == NULL) {
        perror("gpiod_chip_get_line");
        gpiod_close();
        exit(-1);
    }

    // DCを出力モードに設定する
    if (gpiod_line_request_output(gpiod_dc_lineout, appname, 0) != 0) {
        perror("gpiod_line_request_output");
        gpiod_close();
        exit(-1);
    }

    // DCの値を1に設定する
    gpiod_set_out(gpiod_dc_lineout, 1);

    // CSのピンのハンドラを取得する
    // ピンはhttps://doc-en.rvspace.org/VisionFive2/PDF/VisionFive2_QSG.pdfで確認できます。
    if ((gpiod_cs_lineout=gpiod_chip_get_line(gpiod_chip01, spi_cs)) == NULL) {
        perror("gpiod_chip_get_line");
        gpiod_close();
        exit(-1);
    }

    // CSを出力モードに設定する
    if (gpiod_line_request_output(gpiod_cs_lineout, appname, 0) != 0) {
        perror("gpiod_line_request_output");
        gpiod_close();
        exit(-1);
    }

    // CSの値を1に設定する
    gpiod_set_out(gpiod_cs_lineout, 1);

    // RESETのピンのハンドラを取得する
    // ピンはhttps://doc-en.rvspace.org/VisionFive2/PDF/VisionFive2_QSG.pdfで確認できます。
    if ((gpiod_reset_lineout=gpiod_chip_get_line(gpiod_chip01, spi_reset)) == NULL) {
        perror("gpiod_chip_get_line");
        gpiod_close();
        exit(-1);
    }

    // RESETを出力モードに設定する
    if (gpiod_line_request_output(gpiod_reset_lineout, appname, 0) != 0) {
        perror("gpiod_line_request_output");
        gpiod_close();
        exit(-1);
    }

    // RESETの値を1に設定する
    gpiod_set_out(gpiod_reset_lineout, 1);
    delay_ili9486(100);
    gpiod_set_out(gpiod_reset_lineout, 0);
    delay_ili9486(200);
    gpiod_set_out(gpiod_reset_lineout, 1);
    delay_ili9486(200);

    spi_device01 = spi_device;
    spi_open();

    int ret;
    uint32_t mode;
    uint8_t  bits;
    uint32_t speed;

    mode = SPI_MODE;
    bits = SPI_BITS01;
    speed = SPI_SPEED_HZ01;

    ret = ioctl(spidev_fd01, SPI_IOC_RD_MODE32, &mode);
    if(!ret)
        ret = ioctl(spidev_fd01, SPI_IOC_WR_MODE32, &mode);
    if(!ret)
        ret = ioctl(spidev_fd01, SPI_IOC_RD_BITS_PER_WORD, &bits);
    if(!ret)
        ret = ioctl(spidev_fd01, SPI_IOC_WR_BITS_PER_WORD, &bits);
    if(!ret)
        ret = ioctl(spidev_fd01, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
    if(!ret)
        ret = ioctl(spidev_fd01, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
}


void gpiod_close() {
    if (gpiod_chip01 != 0) {
        gpiod_chip_close(gpiod_chip01);
    }
}

void gpiod_set_out(struct gpiod_line *pin, int num) {
    if ((gpiod_value01=gpiod_line_set_value(pin, num)) == -1) {
        perror("gpiod_line_set_value");
    }
}

void spi_open() {

    spidev_fd01 = open(spi_device01, O_RDWR);

}

void spi_close() {
    if (spidev_fd01 != 0) {
        close(spidev_fd01);
        spidev_fd01 = 0;
    }
}


struct spi_ioc_transfer transfer01;

void spi_write(uint8_t spi_send_byte) {

    uint8_t spi_read_byte = 0;
    int ret;

    transfer01.tx_buf        = (unsigned long)(&spi_send_byte);
    transfer01.rx_buf        = (unsigned long)(&spi_read_byte);
    transfer01.len           = 1;
    transfer01.delay_usecs   = SPI_DELAY_USECS01;
    transfer01.speed_hz      = SPI_SPEED_HZ01;
    transfer01.bits_per_word = SPI_BITS01;
    transfer01.cs_change     = 0;

    ret = ioctl(spidev_fd01, SPI_IOC_MESSAGE(1), &transfer01);
    if (ret < 1) perror("ioctl SPI_IOC_MESSAGE");
}

uint8_t spi_read(uint8_t spi_send_byte) {

    uint8_t spi_read_byte = 0;
    int     ret;

    transfer01.tx_buf        = (unsigned long)(&spi_send_byte);
    transfer01.rx_buf        = (unsigned long)(&spi_read_byte);
    transfer01.len           = 1;
    transfer01.delay_usecs   = SPI_DELAY_USECS01;
    transfer01.speed_hz      = SPI_SPEED_HZ01;
    transfer01.bits_per_word = SPI_BITS01;
    transfer01.cs_change     = 0;

    ret = ioctl(spidev_fd01, SPI_IOC_MESSAGE(1), &transfer01);
    if (ret < 1) perror("ioctl SPI_IOC_MESSAGE");

    return (spi_read_byte &  0xff);
}


void ili9486_init() {
    _width = ILI9486_WIDTH;
    _height = ILI9486_HEIGHT;
    _init_width = ILI9486_WIDTH;
    _init_height = ILI9486_HEIGHT;

    spi_begin();

    writecommand(0x11);
    delay_ili9486(120);

    writecommand(0x3A);
    writedata(0x66);                      // 18 bit colour interface
    spi_write_buffer_flush();

    writecommand(0xC2);
    writedata(0x44);
    spi_write_buffer_flush();

    writecommand(0xC5);
    writedata(0x00);
    writedata(0x00);
    writedata(0x00);
    writedata(0x00);
    spi_write_buffer_flush();

    writecommand(0xE0);
    writedata(0x0F);
    writedata(0x1F);
    writedata(0x1C);
    writedata(0x0C);
    writedata(0x0F);
    writedata(0x08);
    writedata(0x48);
    writedata(0x98);
    writedata(0x37);
    writedata(0x0A);
    writedata(0x13);
    writedata(0x04);
    writedata(0x11);
    writedata(0x0D);
    writedata(0x00);
    spi_write_buffer_flush();

    writecommand(0xE1);
    writedata(0x0F);
    writedata(0x32);
    writedata(0x2E);
    writedata(0x0B);
    writedata(0x0D);
    writedata(0x05);
    writedata(0x47);
    writedata(0x75);
    writedata(0x37);
    writedata(0x06);
    writedata(0x10);
    writedata(0x03);
    writedata(0x24);
    writedata(0x20);
    writedata(0x00);
    spi_write_buffer_flush();

    writecommand(ILI9486_INVOFF);
//    writecommand(ILI9486_INVON);

    writecommand(0x36);
    writedata(0x48);
    spi_write_buffer_flush();

    writecommand(0x29);                     // display on
    delay_ili9486(150);
    spi_end();

    set_rotation_ILI9486(0);
}

void set_rotation_ILI9486(int mode) {
    int rotation;

    spi_begin();
    rotation = mode % 8;

    writecommand(ILI9486_MADCTL);
    switch (rotation) {
      case 0:
        writedata(MADCTL_MX | MADCTL_BGR);
        _width  = _init_width;
        _height = _init_height;
        break;
      case 1:
        writedata(MADCTL_MV | MADCTL_BGR);
        _width  = _init_height;
        _height = _init_width;
        break;
      case 2:
        writedata(MADCTL_MY | MADCTL_BGR);
        _width  = _init_width;
        _height = _init_height;
        break;
      case 3:
        writedata(MADCTL_MX | MADCTL_MY | MADCTL_MV | MADCTL_BGR);
        _width  = _init_height;
        _height = _init_width;
        break;

      case 4:
        writedata(MADCTL_MX | MADCTL_MY | MADCTL_BGR);
        _width  = _init_width;
        _height = _init_height;
        break;
      case 5:
        writedata(MADCTL_MV | MADCTL_MX | MADCTL_BGR);
        _width  = _init_height;
        _height = _init_width;
        break;
      case 6:
        writedata(MADCTL_BGR);
        _width  = _init_width;
        _height = _init_height;
        break;
      case 7:
        writedata(MADCTL_MY | MADCTL_MV | MADCTL_BGR);
        _width  = _init_height;
        _height = _init_width;
        break;

    }
    spi_write_buffer_flush();
    spi_end();
}

void setAddrWindow(int16_t x0, int16_t y0, int16_t x1, int16_t y1)
{
    gpio_dc_on();
    spi_write(ILI9486_CASET);
    gpio_dc_off();    
    spi_write_buffer((uint8_t)(x0 >> 8));
    spi_write_buffer((uint8_t)x0);
    spi_write_buffer((uint8_t)(x1 >> 8));
    spi_write_buffer((uint8_t)x1);
    spi_write_buffer_flush();

    gpio_dc_on();
    spi_write(ILI9486_PASET);
    gpio_dc_off();
    spi_write_buffer((uint8_t)(y0 >> 8));
    spi_write_buffer((uint8_t)y0);    
    spi_write_buffer((uint8_t)(y1 >> 8));
    spi_write_buffer((uint8_t)y1);
    spi_write_buffer_flush();
 
    gpio_dc_on();
    spi_write(ILI9486_RAMWR);
    gpio_dc_off();
}

void drawPixel(int32_t x, int32_t y, uint32_t color)
{
    if((x < 0) ||(x >= _width) || (y < 0) || (y >= _height)) return;

    spi_begin();
    setAddrWindow(x, y, x + 1, y + 1);

    spi_write_buffer((uint8_t)(color >> 16));
    spi_write_buffer((uint8_t)(color >> 8));
    spi_write_buffer((uint8_t)color);
    spi_write_buffer_flush();
    spi_end();
}

void fill_color24(uint32_t color, uint32_t len)
{
    for(uint32_t i = 0; i < len; i++) {
        spi_write_buffer((uint8_t)(color >> 16));
        spi_write_buffer((uint8_t)(color >> 8));
        spi_write_buffer((uint8_t)color);
    }
}

void fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color)
{
    if(w > 0 && h > 0){
        if((x >= _width) || (y >= _height)) return;
        if((x + w - 1) >= _width)  w = _width  - x;
        if((y + h - 1) >= _height) h = _height - y;

        spi_begin();
        setAddrWindow(x, y, x + w - 1, y + h - 1);
        fill_color24(color, w * h);
        spi_write_buffer_flush();
        spi_end();
    }
}

void drawCircle(int32_t x0, int32_t y0, int32_t r, uint32_t color) {
  int32_t f = 1 - r;
  int32_t ddF_x = 1;
  int32_t ddF_y = -2 * r;
  int32_t x = 0;
  int32_t y = r;

  drawPixel(x0, y0 + r, color);
  drawPixel(x0, y0 - r, color);
  drawPixel(x0 + r, y0, color);
  drawPixel(x0 - r, y0, color);

  while (x < y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    drawPixel(x0 + x, y0 + y, color);
    drawPixel(x0 - x, y0 + y, color);
    drawPixel(x0 + x, y0 - y, color);
    drawPixel(x0 - x, y0 - y, color);
    drawPixel(x0 + y, y0 + x, color);
    drawPixel(x0 - y, y0 + x, color);
    drawPixel(x0 + y, y0 - x, color);
    drawPixel(x0 - y, y0 - x, color);
  }
}

int32_t test_spi_abs(int32_t num) {
    if (num < 0) num = -num;
    return num;
}

void test_spi_swap_int32_t(int32_t *a, int32_t *b) {
    int32_t t;
    t = *a;
    *a = *b;
    *b = t;
}

void drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint32_t color) {
  int32_t steep = test_spi_abs(y1 - y0) > test_spi_abs(x1 - x0);
  if (steep) {
    test_spi_swap_int32_t(&x0, &y0);
    test_spi_swap_int32_t(&x1, &y1);
  }

  if (x0 > x1) {
    test_spi_swap_int32_t(&x0, &x1);
    test_spi_swap_int32_t(&y0, &y1);
  }

  int32_t dx, dy;
  dx = x1 - x0;
  dy = test_spi_abs(y1 - y0);

  int32_t err = dx / 2;
  int32_t ystep;

  if (y0 < y1) {
    ystep = 1;
  } else {
    ystep = -1;
  }

  for (; x0 <= x1; x0++) {
    if (steep) {
      drawPixel(y0, x0, color);
    } else {
      drawPixel(x0, y0, color);
    }
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}

void drawFastVLine(int32_t x, int32_t y, int32_t h, uint32_t color) {

    // Rudimentary clipping
    if((x >= _width) || (y >= _height)) return;

    if((y+h-1) >= _height) h = _height-y;

    spi_begin();
    setAddrWindow(x, y, x, y+h-1);
    fill_color24(color,h);
    spi_write_buffer_flush();
    spi_end();
}

void drawFastHLine(int32_t x, int32_t y, int32_t w, uint32_t color) {

    // Rudimentary clipping
    if((x >= _width) || (y >= _height)) return;
    if((x+w-1) >= _width)  w = _width-x;

    spi_begin();
    setAddrWindow(x, y, x+w-1, y);
    fill_color24(color, w);
    spi_write_buffer_flush();
    spi_end();
}

void fillCircleHelper(int32_t x0, int32_t y0, int32_t r,
                                    uint8_t corners, int32_t delta,
                                    uint32_t color) {

  int32_t f = 1 - r;
  int32_t ddF_x = 1;
  int32_t ddF_y = -2 * r;
  int32_t x = 0;
  int32_t y = r;
  int32_t px = x;
  int32_t py = y;

  delta++; // Avoid some +1's in the loop

  while (x < y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
    // These checks avoid double-drawing certain lines, important
    // for the SSD1306 library which has an INVERT drawing mode.
    if (x < (y + 1)) {
      if (corners & 1)
        drawFastVLine(x0 + x, y0 - y, 2 * y + delta, color);
      if (corners & 2)
        drawFastVLine(x0 - x, y0 - y, 2 * y + delta, color);
    }
    if (y != py) {
      if (corners & 1)
        drawFastVLine(x0 + py, y0 - px, 2 * px + delta, color);
      if (corners & 2)
        drawFastVLine(x0 - py, y0 - px, 2 * px + delta, color);
      py = y;
    }
    px = x;
  }
}

void fillCircle(int32_t x0, int32_t y0, int32_t r, uint32_t color) {
    drawFastVLine(x0, y0 - r, 2 * r + 1, color);
    fillCircleHelper(x0, y0, r, 3, 0, color);
}

void drawRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color) {
  drawFastHLine(x, y, w, color);
  drawFastHLine(x, y + h - 1, w, color);
  drawFastVLine(x, y, h, color);
  drawFastVLine(x + w - 1, y, h, color);
}

/* bitmapデータは各色8bitの24bitRGB形式です */
void drawRGBBitmap(int32_t x, int32_t y, const uint8_t bitmap[], int32_t w, int32_t h) {
    int32_t i, j;
    const uint8_t *p = bitmap;
    uint8_t r, g, b;

    if(w > 0 && h > 0){
        if((x >= _width) || (y >= _height)) return;
        if((x + w - 1) >= _width)  w = _width  - x;
        if((y + h - 1) >= _height) h = _height - y;

        spi_begin();
        setAddrWindow(x, y, x + w - 1, y + h - 1);
        for(j=0; j < h; j++){
            for(i = 0; i < w; i++) {
                r = *p++;
                g = *p++;
                b = *p++;
                spi_write_buffer(r);
                spi_write_buffer(g);
                spi_write_buffer(b);
            }
        }
        spi_write_buffer_flush();
        spi_end();
    }
}


/* bitmapデータは各色8bitの24bitRGB形式です */
/* maskデータは1bitモノクロのビットマップデータです(MSBから開始します) */
void drawRGBBitmap_mask(int32_t x, int32_t y, const uint8_t *_bitmap,
                                 const uint8_t *mask, int32_t w, int32_t h) {
  uint16_t *bitmap = (uint16_t *)_bitmap;
  int32_t bw = (w + 7) / 8;
  uint8_t byte = 0;
  int32_t index = 0;
  uint8_t r, g, b;
  for (int32_t j = 0; j < h; j++, y++) {
    for (int32_t i = 0; i < w; i++) {
      if (i & 7)
        byte <<= 1;
      else
        byte = mask[j * bw + i / 8];
      if (byte & 0x80) {
        index = (j * w + i)  * 3;
        r = bitmap[index];
        g = bitmap[index + 1];
        b = bitmap[index + 2];
        drawPixel(x + i, y, ((uint32_t)r << 16) | ((uint32_t)g) << 8 | ((uint32_t)b));
      }
    }
  }
}

uint32_t color24(uint32_t colr, uint32_t colg, uint32_t colb) {
    uint32_t color;
    color = ((colr & 0xff) << 16) | ((colg & 0xff) << 8) | (colb & 0xff);
    return color;
}

struct timespec delay_nano_sleep_param01_req_ili9486, delay_nano_sleep_param01_rem_ili9486;

void delay_ili9486(long milli_sec) {
    int nanosleep_ret;
    delay_nano_sleep_param01_req_ili9486.tv_sec = milli_sec / 1000;
    delay_nano_sleep_param01_req_ili9486.tv_nsec = (milli_sec % 1000) * 1000 * 1000;
    nanosleep_ret = nanosleep(&delay_nano_sleep_param01_req_ili9486, &delay_nano_sleep_param01_rem_ili9486);
    while (nanosleep_ret == -1) {
        delay_nano_sleep_param01_req_ili9486.tv_sec = delay_nano_sleep_param01_rem_ili9486.tv_sec;
        delay_nano_sleep_param01_req_ili9486.tv_nsec = delay_nano_sleep_param01_rem_ili9486.tv_nsec;
        nanosleep_ret = nanosleep(&delay_nano_sleep_param01_req_ili9486, &delay_nano_sleep_param01_rem_ili9486);
    }
}


#define SPI_WRITE_BUFFER_SIZE 512
#define SPI_WRITE_BUFFER_NUM  500

uint8_t spi_write_buffer_data01[SPI_WRITE_BUFFER_SIZE];
uint8_t spi_read_buffer_data01[SPI_WRITE_BUFFER_SIZE];

int32_t buffer_pos01;

void spi_write_buffer_spi_write(uint8_t *write_buf, uint8_t *read_buf, int32_t size) {

    uint8_t spi_read_byte = 0;
    int ret;

    transfer01.tx_buf        = (unsigned long)(write_buf);
    transfer01.rx_buf        = (unsigned long)(read_buf);
    transfer01.len           = size;
    transfer01.delay_usecs   = SPI_DELAY_USECS01;
    transfer01.speed_hz      = SPI_SPEED_HZ01;
    transfer01.bits_per_word = SPI_BITS01;
    transfer01.cs_change     = 0;

    ret = ioctl(spidev_fd01, SPI_IOC_MESSAGE(1), &transfer01);
    if (ret < 1) perror("ioctl SPI_IOC_MESSAGE");
}
void spi_write_buffer(uint8_t byte) {
    spi_write_buffer_data01[buffer_pos01++] = byte;
    if (buffer_pos01 == SPI_WRITE_BUFFER_NUM) {
        spi_write_buffer_spi_write(spi_write_buffer_data01, spi_read_buffer_data01, buffer_pos01);
        buffer_pos01 = 0;
    }
}

void spi_write_buffer_flush() {
    spi_write_buffer_spi_write(spi_write_buffer_data01, spi_read_buffer_data01, buffer_pos01);
    buffer_pos01 = 0;
}










int canvas24_begin(int32_t w, int32_t h) {
  int ret = 0;
  uint32_t bytes = w * h * 3;
  canvas24_init_width = w;
  canvas24_init_height = h;
  canvas24_width = w;
  canvas24_height = h;
  if ((canvas24_buffer = (uint8_t *)malloc(bytes)) != NULL) {
      memset(canvas24_buffer, 0, bytes);
      ret = 0;
  } else {
      ret = -1;
  }

  return ret;
}

void canvas24_end() {
  if (canvas24_buffer != NULL)
    free(canvas24_buffer);
}

void canvas24_set_rotation(int32_t x) {
  canvas24_rotation = (x & 3);
  switch (canvas24_rotation) {
  case 0:
  case 2:
    canvas24_width = canvas24_init_width;
    canvas24_height = canvas24_init_height;
    break;
  case 1:
  case 3:
    canvas24_width = canvas24_init_height;
    canvas24_height = canvas24_init_width;
    break;
  }
}

void canvas24_drawPixel(int32_t x, int32_t y, uint32_t color) {

  int32_t index;

  if (canvas24_buffer) {
    if ((x < 0) || (y < 0) || (x >= canvas24_width) || (y >= canvas24_height))
      return;

    int32_t t;
    switch (canvas24_rotation) {
    case 1:
      t = x;
      x = canvas24_init_width - 1 - y;
      y = t;
      break;
    case 2:
      x = canvas24_init_width - 1 - x;
      y = canvas24_init_height - 1 - y;
      break;
    case 3:
      t = x;
      x = y;
      y = canvas24_init_height - 1 - t;
      break;
    }

    index = ((x + y * canvas24_init_width) * 3);
    canvas24_buffer[index] = (uint8_t)(color >> 16);
    canvas24_buffer[index + 1] = (uint8_t)(color >> 8);
    canvas24_buffer[index + 2] = (uint8_t)color;
  }
}

void canvas24_drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint32_t color) {
  int32_t steep = test_spi_abs(y1 - y0) > test_spi_abs(x1 - x0);
  if (steep) {
    test_spi_swap_int32_t(&x0, &y0);
    test_spi_swap_int32_t(&x1, &y1);
  }

  if (x0 > x1) {
    test_spi_swap_int32_t(&x0, &x1);
    test_spi_swap_int32_t(&y0, &y1);
  }

  int32_t dx, dy;
  dx = x1 - x0;
  dy = test_spi_abs(y1 - y0);

  int32_t err = dx / 2;
  int32_t ystep;

  if (y0 < y1) {
    ystep = 1;
  } else {
    ystep = -1;
  }

  for (; x0 <= x1; x0++) {
    if (steep) {
      canvas24_drawPixel(y0, x0, color);
    } else {
      canvas24_drawPixel(x0, y0, color);
    }
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}

void canvas24_drawFastVLine(int32_t x, int32_t y, int32_t h, uint32_t color) {

    // Rudimentary clipping
    if((x >= canvas24_width) || (y >= canvas24_height)) return;

    if((y+h-1) >= canvas24_height) h = canvas24_height-y;

    for (int yy = y; yy < (y + h); yy++) {
        canvas24_drawPixel(x, yy, color);
    }
}

void canvas24_drawFastHLine(int32_t x, int32_t y, int32_t w, uint32_t color) {

    // Rudimentary clipping
    if((x >= canvas24_width) || (y >= canvas24_height)) return;
    if((x+w-1) >= canvas24_width)  w = canvas24_width-x;

    for (int xx = x; xx < (x + w); xx++) {
        canvas24_drawPixel(xx, y, color);
    }
}

void canvas24_fillCircleHelper(int32_t x0, int32_t y0, int32_t r,
                                    uint8_t corners, int32_t delta,
                                    uint32_t color) {

  int32_t f = 1 - r;
  int32_t ddF_x = 1;
  int32_t ddF_y = -2 * r;
  int32_t x = 0;
  int32_t y = r;
  int32_t px = x;
  int32_t py = y;

  delta++; // Avoid some +1's in the loop

  while (x < y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
    // These checks avoid double-drawing certain lines, important
    // for the SSD1306 library which has an INVERT drawing mode.
    if (x < (y + 1)) {
      if (corners & 1)
        canvas24_drawFastVLine(x0 + x, y0 - y, 2 * y + delta, color);
      if (corners & 2)
        canvas24_drawFastVLine(x0 - x, y0 - y, 2 * y + delta, color);
    }
    if (y != py) {
      if (corners & 1)
        canvas24_drawFastVLine(x0 + py, y0 - px, 2 * px + delta, color);
      if (corners & 2)
        canvas24_drawFastVLine(x0 - py, y0 - px, 2 * px + delta, color);
      py = y;
    }
    px = x;
  }
}

void canvas24_fillCircle(int32_t x0, int32_t y0, int32_t r, uint32_t color) {
    canvas24_drawFastVLine(x0, y0 - r, 2 * r + 1, color);
    canvas24_fillCircleHelper(x0, y0, r, 3, 0, color);
}

void canvas24_drawRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color) {
  canvas24_drawFastHLine(x, y, w, color);
  canvas24_drawFastHLine(x, y + h - 1, w, color);
  canvas24_drawFastVLine(x, y, h, color);
  canvas24_drawFastVLine(x + w - 1, y, h, color);
}

void canvas24_fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color)
{
    if(w > 0 && h > 0){
        if((x >= canvas24_width) || (y >= canvas24_height)) return;
        if((x + w - 1) >= canvas24_width)  w = canvas24_width  - x;
        if((y + h - 1) >= canvas24_height) h = canvas24_height - y;

    for (int j = y; j < (y + h); j++) {
            for (int i = x; i < (x + w); i++) {
                canvas24_drawPixel(i, j, color);
        }
    }
    }
}

/* bitmapデータは各色8bitの24bitRGB形式です */
void canvas24_drawRGBBitmap(int32_t x, int32_t y, const uint8_t bitmap[], int32_t w, int32_t h) {
    int32_t i, j;
    const uint8_t *p = (uint8_t *)bitmap;
    uint32_t color;
    uint32_t r, g, b;

    for(j=0; j < h; j++){
        for(i = 0; i < w; i++) {
            r = *p++;
            g = *p++;
            b = *p++;
            color = (r << 16) | (g << 8) | b; 
            canvas24_drawPixel(x + i, y + j, color);
        }
    }
}

/* bitmapデータは各色8bitの24bitRGB形式です */
/* maskデータは1bitモノクロのビットマップデータです(MSBから開始します) */
void canvas24_drawRGBBitmap_mask(int32_t x, int32_t y, const uint8_t *_bitmap,
                                 const uint8_t *mask, int32_t w, int32_t h) {
  uint8_t *bitmap = (uint8_t *)_bitmap;
  int32_t bw = (w + 7) / 8;
  uint8_t byte = 0;
  uint32_t color;
  uint32_t r, g, b;
  int32_t index;

  for (int32_t j = 0; j < h; j++) {
    for (int32_t i = 0; i < w; i++) {
      if (i & 7)
        byte <<= 1;
      else
        byte = mask[j * bw + i / 8];
      if (byte & 0x80) {
          index = (j * w + i) * 3;
          r = bitmap[index];
          g = bitmap[index + 1];
          b = bitmap[index + 2];
          color = (r << 16) | (g << 8) | b; 
          canvas24_drawPixel(x + i, y + j, color);
      }
    }
  }
}

void canvas24_update(int32_t x, int32_t y) {
    drawRGBBitmap(x, y, (uint8_t *)canvas24_buffer, canvas24_init_width, canvas24_init_height);
}
