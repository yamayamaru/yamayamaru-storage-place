#include "yama_ili9341_library.h"



int32_t _init_width, _init_height;
int32_t _width, _height;

struct gpiod_chip *gpiod_chip01;
struct gpiod_line *gpiod_dc_lineout;
struct gpiod_line *gpiod_reset_lineout;
struct gpiod_line *gpiod_cs_lineout;
const char *appname = "spitest01";
int gpiod_value01;
int  spidev_fd01 = 0;

uint16_t *canvas16_buffer;
int32_t canvas16_rotation = 0;
int32_t canvas16_width;
int32_t canvas16_height;
int32_t canvas16_init_width;
int32_t canvas16_init_height;







const char *spi_device01;
void spi_init(const char *spi_device, const char *gpiod_device, int spi_cs, int spi_dc, int spi_reset) {


    // GPIOデバイスを開く
    if ((gpiod_chip01=gpiod_chip_open(gpiod_device)) == NULL) {
        perror("gpiod_chip_open");
        exit(-1);
    }

    // DCのピンのハンドラを取得する
    // ピンはhttps://mangopi.cc/_media/mq-pro-sch-v12.pdfで確認できます。
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
    // ピンはhttps://mangopi.cc/_media/mq-pro-sch-v12.pdfで確認できます。
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
    // ピンはhttps://mangopi.cc/_media/mq-pro-sch-v12.pdfで確認できます。
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
    delay_ili9341(100);
    gpiod_set_out(gpiod_reset_lineout, 0);
    delay_ili9341(200);
    gpiod_set_out(gpiod_reset_lineout, 1);
    delay_ili9341(200);


    // SPIのオープン
    spi_device01 = spi_device;
    if((spi_open() < 0)) {
        perror("spi_open");
        gpiod_close();
        exit(-1);
    }

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
        void gpiod_chip_close(struct gpiod_chip *chip);
        gpiod_chip_close(gpiod_chip01);
        return ;
    }
}

void gpiod_set_out(struct gpiod_line *pin, int num) {
    if ((gpiod_value01=gpiod_line_set_value(pin, num)) == -1) {
        perror("gpiod_line_set_value");
    }
}

int spi_open() {

    if ((spidev_fd01 = open(spi_device01, O_RDWR)) < 0) {
        perror("spi_open open");
        return spidev_fd01;
    }
    return spidev_fd01;
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

void ili9341_init() {
    _width = ILI9341_WIDTH;
    _height = ILI9341_HEIGHT;
    _init_width = ILI9341_WIDTH;
    _init_height = ILI9341_HEIGHT;

    delay_ili9341(150);

    spi_begin();
    writecommand(0xEF);
    writedata(0x03);
    writedata(0x80);
    writedata(0x02);
    spi_write_buffer_flush();

    writecommand(0xCF);  
    writedata(0x00); 
    writedata(0XC1); 
    writedata(0X30); 
    spi_write_buffer_flush();

    writecommand(0xED);  
    writedata(0x64); 
    writedata(0x03); 
    writedata(0X12); 
    writedata(0X81); 
    spi_write_buffer_flush();

    writecommand(0xE8);  
    writedata(0x85); 
    writedata(0x00); 
    writedata(0x78); 
    spi_write_buffer_flush();

    writecommand(0xCB);  
    writedata(0x39); 
    writedata(0x2C); 
    writedata(0x00); 
    writedata(0x34); 
    writedata(0x02); 
    spi_write_buffer_flush();

    writecommand(0xF7);  
    writedata(0x20); 
    spi_write_buffer_flush();

    writecommand(0xEA);  
    writedata(0x00); 
    writedata(0x00); 
    spi_write_buffer_flush();

    writecommand(ILI9341_PWCTR1);    //Power control 
    writedata(0x23);   //VRH[5:0] 
    spi_write_buffer_flush();

    writecommand(ILI9341_PWCTR2);    //Power control 
    writedata(0x10);   //SAP[2:0];BT[3:0] 
    spi_write_buffer_flush();

    writecommand(ILI9341_VMCTR1);    //VCM control 
    writedata(0x3e); //¶O±E¶Eμ÷?U
    writedata(0x28); 
    spi_write_buffer_flush();

    writecommand(ILI9341_VMCTR2);    //VCM control2 
    writedata(0x86);  //--
    spi_write_buffer_flush();

    writecommand(ILI9341_MADCTL);    // Memory Access Control 
    writedata(0x48);
    spi_write_buffer_flush();

    writecommand(ILI9341_PIXFMT);
    writedata(0x55); 
    spi_write_buffer_flush();

    writecommand(ILI9341_FRMCTR1);
    writedata(0x00);  
    writedata(0x18); 
    spi_write_buffer_flush();

    writecommand(ILI9341_DFUNCTR);    // Display Function Control 
    writedata(0x08); 
    writedata(0x82);
    writedata(0x27);  
    spi_write_buffer_flush();

    writecommand(0xF2);    // 3Gamma Function Disable 
    writedata(0x00); 
    spi_write_buffer_flush();

    writecommand(ILI9341_GAMMASET);    //Gamma curve selected 
    writedata(0x01); 
    spi_write_buffer_flush();

    writecommand(ILI9341_GMCTRP1);    //Set Gamma 
    writedata(0x0F); 
    writedata(0x31); 
    writedata(0x2B); 
    writedata(0x0C); 
    writedata(0x0E); 
    writedata(0x08); 
    writedata(0x4E); 
    writedata(0xF1); 
    writedata(0x37); 
    writedata(0x07); 
    writedata(0x10); 
    writedata(0x03); 
    writedata(0x0E); 
    writedata(0x09); 
    writedata(0x00); 
    spi_write_buffer_flush();

    writecommand(ILI9341_GMCTRN1);    //Set Gamma 
    writedata(0x00); 
    writedata(0x0E); 
    writedata(0x14); 
    writedata(0x03); 
    writedata(0x11); 
    writedata(0x07); 
    writedata(0x31); 
    writedata(0xC1); 
    writedata(0x48); 
    writedata(0x08); 
    writedata(0x0F); 
    writedata(0x0C); 
    writedata(0x31); 
    writedata(0x36); 
    writedata(0x0F); 
    spi_write_buffer_flush();

    writecommand(ILI9341_SLPOUT);    //Exit Sleep 
    spi_end();
    delay_ili9341(120); 
    spi_begin();
    writecommand(ILI9341_DISPON);    //Display on 
    delay_ili9341(150);
    spi_end();

    set_rotation_ILI9341(0);
}

void set_rotation_ILI9341(int mode) {
    int rotation;

    spi_begin();
    rotation = mode % 8;

    writecommand(ILI9341_MADCTL);
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
    spi_write(ILI9341_CASET);
    gpio_dc_off();    
    spi_write_buffer((uint8_t)(x0 >> 8));
    spi_write_buffer((uint8_t)x0);
    spi_write_buffer((uint8_t)(x1 >> 8));
    spi_write_buffer((uint8_t)x1);
    spi_write_buffer_flush();

    gpio_dc_on();
    spi_write(ILI9341_PASET);
    gpio_dc_off();
    spi_write_buffer((uint8_t)(y0 >> 8));
    spi_write_buffer((uint8_t)y0);    
    spi_write_buffer((uint8_t)(y1 >> 8));
    spi_write_buffer((uint8_t)y1);
    spi_write_buffer_flush();
 
    gpio_dc_on();
    spi_write(ILI9341_RAMWR);
    gpio_dc_off();
}

void drawPixel(int32_t x, int32_t y, uint32_t color)
{
    if((x < 0) ||(x >= _width) || (y < 0) || (y >= _height)) return;

    spi_begin();
    setAddrWindow(x, y, x + 1, y + 1);

    spi_write_buffer((uint8_t)(color >> 8));
    spi_write_buffer((uint8_t)color);
    spi_write_buffer_flush();
    spi_end();
}

void fill_color16(uint16_t color, uint32_t len)
{
    for(uint32_t i = 0; i < len; i++) {
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
        fill_color16((uint16_t)color, w * h);
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

void test_spi_swap_int32_t(int32_t a, int32_t b) {
    int32_t t;
    t = a;
    a = b;
    b = t;
}

void drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint32_t color) {
  int32_t steep = test_spi_abs(y1 - y0) > test_spi_abs(x1 - x0);
  if (steep) {
    test_spi_swap_int32_t(x0, y0);
    test_spi_swap_int32_t(x1, y1);
  }

  if (x0 > x1) {
    test_spi_swap_int32_t(x0, x1);
    test_spi_swap_int32_t(y0, y1);
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
    fill_color16((uint16_t)color,h);
    spi_write_buffer_flush();
    spi_end();
}

void drawFastHLine(int32_t x, int32_t y, int32_t w, uint32_t color) {

    // Rudimentary clipping
    if((x >= _width) || (y >= _height)) return;
    if((x+w-1) >= _width)  w = _width-x;

    spi_begin();
    setAddrWindow(x, y, x+w-1, y);
    fill_color16((uint16_t)color, w);
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

/* bitmapデータは16bitのrgb565のリトルエンディアン形式です */
void drawRGBBitmap(int32_t x, int32_t y, const uint8_t bitmap[], int32_t w, int32_t h) {
    int32_t i, j;
    const uint16_t *p = (uint16_t *)bitmap;
    uint16_t pixel01;
 
    spi_begin();
    setAddrWindow(x, y, x + w - 1, y + h - 1);
    for(j=0; j < h; j++){
        for(i = 0; i < w; i++) {
        pixel01 = *p++;
        spi_write_buffer((uint8_t)(pixel01 >> 8));
        spi_write_buffer((uint8_t)(pixel01 & 0xff));
        }
    }
    spi_write_buffer_flush();
    spi_end();
}


/* bitmapデータは16bitのrgb565のリトルエンディアン形式です */
/* maskデータは1bitモノクロのビットマップデータです(MSBから開始します) */
void drawRGBBitmap_mask(int32_t x, int32_t y, const uint8_t *_bitmap,
                                 const uint8_t *mask, int32_t w, int32_t h) {
  uint16_t *bitmap = (uint16_t *)_bitmap;
  int32_t bw = (w + 7) / 8; 
  uint8_t byte = 0;
  for (int32_t j = 0; j < h; j++, y++) {
    for (int32_t i = 0; i < w; i++) {
      if (i & 7)
        byte <<= 1;
      else
        byte = mask[j * bw + i / 8];
      if (byte & 0x80) {
        drawPixel(x + i, y, bitmap[j * w + i]);
      }
    }
  }
}

uint16_t color565(uint32_t colr, uint32_t colg, uint32_t colb) {
    uint16_t color;
    color = (uint16_t)(((colr & 0xff) >> 3) << 11) | (((colg & 0xff) >> 2) << 5) | ((colb & 0xff) >> 3);
    return color;
}

struct timespec delay_nano_sleep_param01_req_ili9341, delay_nano_sleep_param01_rem_ili9341;

void delay_ili9341(long milli_sec) {
    int nanosleep_ret;
    delay_nano_sleep_param01_req_ili9341.tv_sec = milli_sec / 1000;
    delay_nano_sleep_param01_req_ili9341.tv_nsec = (milli_sec % 1000) * 1000 * 1000;
    nanosleep_ret = nanosleep(&delay_nano_sleep_param01_req_ili9341, &delay_nano_sleep_param01_rem_ili9341);
    while (nanosleep_ret == -1) {
        delay_nano_sleep_param01_req_ili9341.tv_sec = delay_nano_sleep_param01_rem_ili9341.tv_sec;
        delay_nano_sleep_param01_req_ili9341.tv_nsec = delay_nano_sleep_param01_rem_ili9341.tv_nsec;
        nanosleep_ret = nanosleep(&delay_nano_sleep_param01_req_ili9341, &delay_nano_sleep_param01_rem_ili9341);
    }
}

#define SPI_WRITE_BUFFER_SIZE 512
#define SPI_WRITE_BUFFER_NUM  400

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
    if (buffer_pos01 > SPI_WRITE_BUFFER_NUM) {
        spi_write_buffer_spi_write(spi_write_buffer_data01, spi_read_buffer_data01, buffer_pos01);
        buffer_pos01 = 0;
    }
}

void spi_write_buffer_flush() {
    spi_write_buffer_spi_write(spi_write_buffer_data01, spi_read_buffer_data01, buffer_pos01);
    buffer_pos01 = 0;
}









int canvas16_begin(int32_t w, int32_t h) {
  int ret = 0;
  uint32_t bytes = w * h * 2;
  canvas16_init_width = w;
  canvas16_init_height = h;
  canvas16_width = w;
  canvas16_height = h;
  if ((canvas16_buffer = (uint16_t *)malloc(bytes)) != NULL) {
      memset(canvas16_buffer, 0, bytes);
      ret = 0;
  } else {
      ret = -1;
  }

  return ret;
}

void canvas16_end() {
  if (canvas16_buffer != NULL)
    free(canvas16_buffer);
}

void canvas16_set_rotation(int32_t x) {
  canvas16_rotation = (x & 3);
  switch (canvas16_rotation) {
  case 0:
  case 2:
    canvas16_width = canvas16_init_width;
    canvas16_height = canvas16_init_height;
    break;
  case 1:
  case 3:
    canvas16_width = canvas16_init_height;
    canvas16_height = canvas16_init_width;
    break;
  }
}

void canvas16_drawPixel(int32_t x, int32_t y, uint32_t color) {
  if (canvas16_buffer) {
    if ((x < 0) || (y < 0) || (x >= canvas16_width) || (y >= canvas16_height))
      return;

    int32_t t;
    switch (canvas16_rotation) {
    case 1:
      t = x;
      x = canvas16_init_width - 1 - y;
      y = t;
      break;
    case 2:
      x = canvas16_init_width - 1 - x;
      y = canvas16_init_height - 1 - y;
      break;
    case 3:
      t = x;
      x = y;
      y = canvas16_init_height - 1 - t;
      break;
    }

    canvas16_buffer[x + y * canvas16_init_width] = color;
  }
}

void canvas16_drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint32_t color) {
  int32_t steep = test_spi_abs(y1 - y0) > test_spi_abs(x1 - x0);
  if (steep) {
    test_spi_swap_int32_t(x0, y0);
    test_spi_swap_int32_t(x1, y1);
  }

  if (x0 > x1) {
    test_spi_swap_int32_t(x0, x1);
    test_spi_swap_int32_t(y0, y1);
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
      canvas16_drawPixel(y0, x0, color);
    } else {
      canvas16_drawPixel(x0, y0, color);
    }
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}

void canvas16_drawFastVLine(int32_t x, int32_t y, int32_t h, uint32_t color) {

    // Rudimentary clipping
    if((x >= canvas16_width) || (y >= canvas16_height)) return;

    if((y+h-1) >= canvas16_height) h = canvas16_height-y;

    for (int yy = y; yy < (y + h); yy++) {
        canvas16_drawPixel(x, yy, color);
    }
}

void canvas16_drawFastHLine(int32_t x, int32_t y, int32_t w, uint32_t color) {

    // Rudimentary clipping
    if((x >= canvas16_width) || (y >= canvas16_height)) return;
    if((x+w-1) >= canvas16_width)  w = canvas16_width-x;

    for (int xx = x; xx < (x + w); xx++) {
        canvas16_drawPixel(xx, y, color);
    }
}

void canvas16_fillCircleHelper(int32_t x0, int32_t y0, int32_t r,
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
        canvas16_drawFastVLine(x0 + x, y0 - y, 2 * y + delta, color);
      if (corners & 2)
        canvas16_drawFastVLine(x0 - x, y0 - y, 2 * y + delta, color);
    }
    if (y != py) {
      if (corners & 1)
        canvas16_drawFastVLine(x0 + py, y0 - px, 2 * px + delta, color);
      if (corners & 2)
        canvas16_drawFastVLine(x0 - py, y0 - px, 2 * px + delta, color);
      py = y;
    }
    px = x;
  }
}

void canvas16_fillCircle(int32_t x0, int32_t y0, int32_t r, uint32_t color) {
    canvas16_drawFastVLine(x0, y0 - r, 2 * r + 1, color);
    canvas16_fillCircleHelper(x0, y0, r, 3, 0, color);
}

void canvas16_drawRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color) {
  canvas16_drawFastHLine(x, y, w, color);
  canvas16_drawFastHLine(x, y + h - 1, w, color);
  canvas16_drawFastVLine(x, y, h, color);
  canvas16_drawFastVLine(x + w - 1, y, h, color);
}

void canvas16_fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color)
{
    if(w > 0 && h > 0){
        if((x >= canvas16_width) || (y >= canvas16_height)) return;
        if((x + w - 1) >= canvas16_width)  w = canvas16_width  - x;
        if((y + h - 1) >= canvas16_height) h = canvas16_height - y;

    for (int j = y; j < (y + h); j++) {
            for (int i = x; i < (x + w); i++) {
                canvas16_drawPixel(i, j, color);
        }
    }
    }
}

/* bitmapデータは16bitのrgb565のリトルエンディアン形式です */
void canvas16_drawRGBBitmap(int32_t x, int32_t y, const uint8_t bitmap[], int32_t w, int32_t h) {
    int32_t i, j;
    const uint16_t *p = (uint16_t *)bitmap;
    uint16_t pixel01;

    for(j=0; j < h; j++){
        for(i = 0; i < w; i++) {
            canvas16_drawPixel(x + i, y + j, *p++);
        }
    }
}

/* bitmapデータは16bitのrgb565のリトルエンディアン形式です */
/* maskデータは1bitモノクロのビットマップデータです(MSBから開始します) */
void canvas16_drawRGBBitmap_mask(int32_t x, int32_t y, const uint8_t *_bitmap,
                                 const uint8_t *mask, int32_t w, int32_t h) {
  uint16_t *bitmap = (uint16_t *)_bitmap;
  int32_t bw = (w + 7) / 8;
  uint8_t byte = 0;
  for (int32_t j = 0; j < h; j++) {
    for (int32_t i = 0; i < w; i++) {
      if (i & 7)
        byte <<= 1;
      else
        byte = mask[j * bw + i / 8];
      if (byte & 0x80) {
        canvas16_drawPixel(x + i, y + j, bitmap[j * w + i]);
      }
    }
  }
}

void canvas16_update(int32_t x, int32_t y) {
    drawRGBBitmap(x, y, (uint8_t *)canvas16_buffer, canvas16_init_width, canvas16_init_height);
}





#define MAX_TEXT_SIZE01 32768
#define STR_BUFF01_LEN01 32768
bool _back_ground_fill_flag = false;
int32_t freetype_fg_color = ILI9341_WHITE;
int32_t freetype_bg_color = ILI9341_BLACK;
int32_t freetype_font_size01 = 16 * 34;
int32_t freetype_font_height01 = 34;
int32_t freetype_font_line_height01 = 34;
int32_t freetype_font_space01 = 16;
int32_t _freetype_x = 0;
int32_t _freetype_y = 0;
char freetype_print_str_buff01[STR_BUFF01_LEN01];

int init_freetype(FT_Library *ft_library01) {
    int ret = 0;
    if ((ret = FT_Init_FreeType(ft_library01 )) != 0) {
        perror("FT_Init_FreeType ft_library01");
        return ret;
    }

    return ret;
}

int set_font(FT_Library *ft_library01, FT_Face *ft_face01, const char *font_path) {

    int ret = 0;
    if ((ret = FT_New_Face(*ft_library01, font_path, 0, ft_face01)) != 0) {
        perror("FT_New_Face ft_library01 ft_face01");
        return ret;
    }

    if ((ret = FT_Set_Char_Size(*ft_face01, 0,
                      16 * 34,    // 文字の幅 * 高さ
                      300, 300)   // X方向のdpi、Y方向のdpi
                      ) != 0) {
        perror("FT_Set_Char_Size ft_face01");
        return ret;
    }
    int32_t size = 16;
    freetype_font_size01 = (size / 2) * size;
    freetype_font_height01 = size;
    freetype_font_line_height01 = size;
    int32_t get_font_ch_char_w(FT_Face *ft_face01, const char *text);
    freetype_font_space01 = get_font_ch_char_w(ft_face01, u8"a");

    return ret;
}

int set_font_size(FT_Face *ft_face01, int32_t size, int32_t x_dpi, int32_t y_dpi) {
    int ret = 0;

    if ((ret = FT_Set_Char_Size(*ft_face01, 0,
                      ((size - 2) / 2) * size,    // 文字の幅 * 高さ
                      x_dpi, y_dpi)   // X方向のdpi、Y方向のdpi
                      ) != 0) {
        perror("FT_Set_Char_Size ft_face01");
        return ret;
    }
    freetype_font_size01 = (size / 2) * size;
    freetype_font_height01 = size;
    freetype_font_line_height01 = size;
    int32_t get_font_ch_char_w(FT_Face *ft_face01, const char *text);
    freetype_font_space01 = get_font_ch_char_w(ft_face01, u8"a");

    return ret;
}

int32_t utf8_byte_count(char ch) {

    uint32_t a = (uint32_t)ch;
    if (0 <= a && a < 0x80) {
        return 1;
    } else if (0xc2 <= a && a < 0xe0) {
        return 2;
    } else if (0xe0 <= a && a < 0xf0) {
        return 3;
    } else if (0xf0 <= a && a < 0xf8) {
        return 4;
    }

    return 0;
}

std::u32string utf8_to_utf32(const char *text, int32_t buf_size) {

    int i;
    uint32_t a, byte_count01;
    std::u32string str1 = U"";
    for (i = 0; i < buf_size; i++) {
        if (text[i] == '\0') break;
        byte_count01 = utf8_byte_count(text[i]);
        switch (byte_count01) {
            case 1:
                str1.push_back((char32_t)text[i]);
                break;
            case 2:
                a = (((uint32_t)text[i] & 0x1f) << 6);
                a |= ((uint32_t)text[++i] & 0x3f);
                str1.push_back((char32_t)a);
                break;
            case 3:
                a = ((uint32_t)text[i] & 0x0f) << 12;
                a |= ((uint32_t)text[++i] & 0x3f) << 6;
                a |= ((uint32_t)text[++i] & 0x3f);
                str1.push_back((char32_t)a);
                break;
            case 4:
                a = ((uint32_t)text[i] & 0x07) << 18;
                a |= ((uint32_t)text[++i] & 0x3f) << 12;
                a |= ((uint32_t)text[++i] & 0x3f) << 6;
                a |= ((uint32_t)text[++i] & 0x3f);
                str1.push_back((char32_t)a);
            default:
        break;
        }
    }

    return str1;
}

void set_font_back_ground_fill(bool flag) {
    _back_ground_fill_flag = flag;
}

void set_font_color(int32_t fg, int32_t bg) {
    freetype_fg_color = fg;
    if (bg != -1) {
        freetype_bg_color = bg;
    }
}

int32_t drawChars(FT_Face *ft_face01, int32_t char_x, int32_t char_y, const char *text, int32_t buf_size) {

    int ret = 0;
    int32_t str_w = 0;
    std::u32string  u32_string01 = utf8_to_utf32(text, buf_size);

    for (int k = 0; k < u32_string01.size(); k++) {
        if ((u32_string01[k] == 0x0000000a) || (u32_string01[k] == 0x00000020) || (u32_string01[k] == 0x000000a0) || (u32_string01[k] == 0x00003000) ) {
            if (u32_string01[k] == 0x00003000) {
                str_w += freetype_font_space01 * 2;
            } else if ((u32_string01[k] == 0x00000020) || (u32_string01[k] == 0x000000a0)){
                str_w += freetype_font_space01;
            }
        } else {
            FT_UInt glyph_char_index01 = FT_Get_Char_Index(*ft_face01, u32_string01[k]);

            if((ret = FT_Load_Glyph(*ft_face01, glyph_char_index01, FT_LOAD_DEFAULT )) != 0) {
                perror("FT_Load_Glyph ft_face01 glyph_char_index01 FT_LOAD_DEFAULT");
            }
            FT_Render_Glyph((*ft_face01)->glyph, FT_RENDER_MODE_NORMAL );// 1文字レンダリング

            int index;
            for (int j = 0; j < (*ft_face01)->glyph->bitmap.rows; j++) {   // 文字のbitmapの描画
                for (int i = 0; i < (*ft_face01)->glyph->bitmap.width; i++) {
                    index = j * (*ft_face01)->glyph->bitmap.width + i;
                    if ((*ft_face01)->glyph->bitmap.buffer[index]) {
                        drawPixel(char_x + i + str_w,
                                  char_y - (*ft_face01)->glyph->bitmap_top + j, freetype_fg_color);
                    } else {
                        if (_back_ground_fill_flag) 
                            drawPixel(char_x + (*ft_face01)->glyph->bitmap_left + i + str_w,
                                  char_y - (*ft_face01)->glyph->bitmap_top + j, freetype_bg_color);
                    }
                }
            }
            str_w += (*ft_face01)->glyph->bitmap.width + (*ft_face01)->glyph->bitmap_left;
        }
    }
    return (char_x + str_w);
}

int32_t canvas16_drawChars(FT_Face *ft_face01, int32_t char_x, int32_t char_y, const char *text, int32_t buf_size) {

    int ret = 0;
    int32_t str_w = 0;
    std::u32string  u32_string01 = utf8_to_utf32(text, buf_size);

    for (int k = 0; k < u32_string01.size(); k++) {
        if ((u32_string01[k] == 0x0000000a) || (u32_string01[k] == 0x00000020) || (u32_string01[k] == 0x000000a0) || (u32_string01[k] == 0x00003000) ) {
            if (u32_string01[k] == 0x00003000) {
                str_w += freetype_font_space01 * 2;
            } else if ((u32_string01[k] == 0x00000020) || (u32_string01[k] == 0x000000a0)){
                str_w += freetype_font_space01;
            }
        } else {
            FT_UInt glyph_char_index01 = FT_Get_Char_Index(*ft_face01, u32_string01[k]);

            if((ret = FT_Load_Glyph(*ft_face01, glyph_char_index01, FT_LOAD_DEFAULT )) != 0) {
                perror("FT_Load_Glyph ft_face01 glyph_char_index01 FT_LOAD_DEFAULT");
            }
            FT_Render_Glyph((*ft_face01)->glyph, FT_RENDER_MODE_NORMAL );// 1文字レンダリング

            int index;
            for (int j = 0; j < (*ft_face01)->glyph->bitmap.rows; j++) {   // 文字のbitmapの描画
                for (int i = 0; i < (*ft_face01)->glyph->bitmap.width; i++) {
                    index = j * (*ft_face01)->glyph->bitmap.width + i;
                    if ((*ft_face01)->glyph->bitmap.buffer[index]) {
                        canvas16_drawPixel(char_x + i + str_w,
                                  char_y - (*ft_face01)->glyph->bitmap_top + j, freetype_fg_color);
                    } else {
                        if (_back_ground_fill_flag) 
                       canvas16_drawPixel(char_x + (*ft_face01)->glyph->bitmap_left + i + str_w,
                  char_y - (*ft_face01)->glyph->bitmap_top + j, freetype_bg_color);
                    }
                }
            }
        str_w += (*ft_face01)->glyph->bitmap.width + (*ft_face01)->glyph->bitmap_left;
        }
    }
    return (char_x + str_w);
}

int32_t freetype_next_line(int32_t cury, int32_t char_h) {
    int32_t res;
    res = cury + char_h;
    return res;
}

void set_font_line_height(int32_t char_h) {
    freetype_font_line_height01 = char_h;
}

int freetype_substr01(const char *source, char *dest, int buffsize, int start, int length) {
    char *sp, *ep, size;
    size = buffsize - 1;
    int32_t source_len01 = strnlen(source,  MAX_TEXT_SIZE01);

    sp = (char *)source + start;
    ep = (char *)source + start + length;
    if (sp > (source + source_len01)) {
        dest[0] = '\0';
        return -1;
    }
    if (ep > (source + source_len01)) {
        ep = ((char *)source + source_len01);
        length = ep - sp;
    }
    if (length > size) {
        length = size;
    }
    if (length < 0) {
        dest[0] = '\0';
        return -1;
    }
    memcpy(dest, sp, length);
    dest[length] = '\0';
    return 0;
}

int32_t get_font_ch_char_w(FT_Face *ft_face01, const char *text) {
    int32_t width = 0;
    std::u32string u32_string01 = utf8_to_utf32(text, 9);
    if ((u32_string01[0] == 0x0000000a) || (u32_string01[0] == 0x00000020) || (u32_string01[0] == 0x000000a0) || (u32_string01[0] == 0x00003000) ) {
        if (u32_string01[0] == 0x00003000) {
            width = freetype_font_space01 * 2;
        } else if ((u32_string01[0] == 0x00000020) || (u32_string01[0] == 0x000000a0)){
            width = freetype_font_space01;
        } else if(u32_string01[0] == 0x0000000a) {
            width = 0;
        }
    } else {
        int ret;
        FT_UInt glyph_char_index01 = FT_Get_Char_Index(*ft_face01, u32_string01[0]);
        if((ret = FT_Load_Glyph(*ft_face01, glyph_char_index01, FT_LOAD_DEFAULT )) != 0) {
            perror("FT_Load_Glyph ft_face01 glyph_char_index01 FT_LOAD_DEFAULT");
        }
        FT_Render_Glyph((*ft_face01)->glyph, FT_RENDER_MODE_NORMAL );
        width = (*ft_face01)->glyph->bitmap.width + (*ft_face01)->glyph->bitmap_left;
    }
    return width;
}

void set_font_pos(int32_t x, int32_t y) {
    _freetype_x = x;
    _freetype_y = y;
}

void print(FT_Face *ft_face01, const char *text) { // does character wrap, compatible with stream output

    int32_t curx, cury, char_w, char_h, width1, written;
    int32_t str_len01;

    curx = _freetype_x;
    cury = _freetype_y;
    char_h = freetype_font_line_height01;
    width1 = 0;
    written = 0;

    str_len01 = strnlen(text, MAX_TEXT_SIZE01);
    if (str_len01 > STR_BUFF01_LEN01) return ;

    int32_t pos_next = 0;
    int32_t ch_byte_count = 0;
    for (int32_t pos = 0; pos < str_len01; pos = pos_next) {
        char ch[9];
        ch_byte_count = utf8_byte_count(text[pos]);
        for (int i = 0; i < ch_byte_count; i++) {
            ch[i] = text[pos_next++];
        }
        ch[ch_byte_count] = '\0';
        if ((ch[0] == '\n') && (ch_byte_count == 1)) {
            if (pos > 0) {
                freetype_substr01(text, freetype_print_str_buff01, STR_BUFF01_LEN01, written, pos - written);
                drawChars(ft_face01, curx, cury, freetype_print_str_buff01, STR_BUFF01_LEN01);
            }
            curx = 0;
            written = pos_next;
            width1 = 0;
            cury = freetype_next_line(cury, char_h);
        } else {
            char_w = get_font_ch_char_w(ft_face01, ch);
            if ((curx + width1 + char_w) == _width) {
                freetype_substr01(text, freetype_print_str_buff01, STR_BUFF01_LEN01, written, pos_next - written);
                drawChars(ft_face01, curx, cury, freetype_print_str_buff01, STR_BUFF01_LEN01);
                curx = 0;
                written = pos_next;
                width1 = 0;
                cury = freetype_next_line(cury, char_h);
            
            } else if ((curx + width1 + char_w) > _width) {
                freetype_substr01(text, freetype_print_str_buff01, STR_BUFF01_LEN01, written, pos - written);
                drawChars(ft_face01, curx, cury, freetype_print_str_buff01, STR_BUFF01_LEN01);
                curx = 0;
                width1 = 0;
                cury = freetype_next_line(cury, char_h);
                freetype_substr01(text, freetype_print_str_buff01, STR_BUFF01_LEN01, pos, pos_next - pos);
                drawChars(ft_face01, curx, cury, freetype_print_str_buff01, STR_BUFF01_LEN01);
                curx += char_w;
                width1 += char_w;
                written = pos_next;
            } else {
                width1 += char_w;
            }
        }
    }
    if (written < str_len01) {
        freetype_substr01(text, freetype_print_str_buff01, STR_BUFF01_LEN01, written, str_len01 - written);
        curx = drawChars(ft_face01, curx, cury, freetype_print_str_buff01, STR_BUFF01_LEN01);
    }
    _freetype_x = curx;
    _freetype_y = cury;
}

void canvas16_print(FT_Face *ft_face01, const char *text) { // does character wrap, compatible with stream output

    int32_t curx, cury, char_w, char_h, width1, written;
    int32_t str_len01;

    curx = _freetype_x;
    cury = _freetype_y;
    char_h = freetype_font_line_height01;
    width1 = 0;
    written = 0;

    str_len01 = strnlen(text, MAX_TEXT_SIZE01);
    if (str_len01 > STR_BUFF01_LEN01) return ;

    int32_t pos_next = 0;
    int32_t ch_byte_count = 0;
    for (int32_t pos = 0; pos < str_len01; pos = pos_next) {
        char ch[9];
        ch_byte_count = utf8_byte_count(text[pos]);
        for (int i = 0; i < ch_byte_count; i++) {
            ch[i] = text[pos_next++];
        }
        ch[ch_byte_count] = '\0';
        if ((ch[0] == '\n') && (ch_byte_count == 1)) {
            if (pos > 0) {
                freetype_substr01(text, freetype_print_str_buff01, STR_BUFF01_LEN01, written, pos - written);
                canvas16_drawChars(ft_face01, curx, cury, freetype_print_str_buff01, STR_BUFF01_LEN01);
            }
            curx = 0;
            written = pos_next;
            width1 = 0;
            cury = freetype_next_line(cury, char_h);
        } else {
            char_w = get_font_ch_char_w(ft_face01, ch);
            if ((curx + width1 + char_w) == canvas16_width) {
                freetype_substr01(text, freetype_print_str_buff01, STR_BUFF01_LEN01, written, pos_next - written);
                canvas16_drawChars(ft_face01, curx, cury, freetype_print_str_buff01, STR_BUFF01_LEN01);
                curx = 0;
                written = pos_next;
                width1 = 0;
                cury = freetype_next_line(cury, char_h);
            
            } else if ((curx + width1 + char_w) > canvas16_width) {
                freetype_substr01(text, freetype_print_str_buff01, STR_BUFF01_LEN01, written, pos - written);
                canvas16_drawChars(ft_face01, curx, cury, freetype_print_str_buff01, STR_BUFF01_LEN01);
                curx = 0;
                width1 = 0;
                cury = freetype_next_line(cury, char_h);
                freetype_substr01(text, freetype_print_str_buff01, STR_BUFF01_LEN01, pos, pos_next - pos);
                canvas16_drawChars(ft_face01, curx, cury, freetype_print_str_buff01, STR_BUFF01_LEN01);
                curx += char_w;
                width1 += char_w;
                written = pos_next;
            } else {
                width1 += char_w;
            }
        }
    }
    if (written < str_len01) {
        freetype_substr01(text, freetype_print_str_buff01, STR_BUFF01_LEN01, written, str_len01 - written);
        curx = canvas16_drawChars(ft_face01, curx, cury, freetype_print_str_buff01, STR_BUFF01_LEN01);
    }
    _freetype_x = curx;
    _freetype_y = cury;
}

int32_t get_font_word_w(FT_Face *ft_face01, std::string word) {
    int32_t word_width = 0;
    int32_t ch_byte_count02 = 0;
    for (int i = 0; i < word.size(); i += ch_byte_count02) {
        ch_byte_count02 = utf8_byte_count(word[i]);
        word_width += get_font_ch_char_w(ft_face01, word.substr(i, ch_byte_count02).c_str());
    }
    return word_width;
}

int32_t freetype_check_leading_spaces(std::string str1) {
    int i;
    for (i = 0; i < str1.size(); i++) {
        if ((char)str1[i] != ' ') break;
    }

    return i;
}

std::string freetype_spaces(int32_t number) {
    std::string spaces01 = "";
    for (int i = 0; i < number; i++) {
        spaces01.push_back(' ');
    }

    return spaces01;
}

void word_print(FT_Face *ft_face01, std::string text) {

    int32_t curx, cury, char_h, char_w;
    std::stringstream text_s{text};
    std::string lines;

    curx = _freetype_x;
    cury = _freetype_y;
    char_h = freetype_font_line_height01;
    char_w = freetype_font_height01 * 2;      // 文字の横幅の最大値
    while (std::getline(text_s, lines, '\n')) {

        int32_t spaces_num = freetype_check_leading_spaces(lines);
        std::stringstream lines_s{lines};
        std::string buf;
        std::vector<std::string> words;
        while (std::getline(lines_s, buf, ' ')) {
            words.push_back(buf);
        }

        if (spaces_num > 0) {
            words[0].insert(0, freetype_spaces(spaces_num));
        }
        for (int i = 0; i < words.size(); i++) {
            std::string word = words[i];
            int32_t word_width01 = get_font_word_w(ft_face01, word);
            if ((curx + word_width01) > _width) {
                if (curx != _freetype_x) cury = freetype_next_line(cury, char_h);
                curx = _freetype_x;
                while ((curx + get_font_word_w(ft_face01, word)) > _width) {
                    int32_t ch_byte_count = 0;
                    int32_t ch_count01 = 0;
                    int32_t ch_count01_next = 0;
                    int32_t word_width01 = 0;
                    int32_t word_width01_next = 0;
                    char ch01[9];
                    int32_t ch_copy_byte_count = 0;
                    bool over_flag01 = false;
                    for (ch_count01_next = 0; ch_count01_next < word.size();) {
                        ch_byte_count = utf8_byte_count((char)word[ch_count01_next]);
                        ch_count01 = ch_count01_next;
                        ch_count01_next += ch_byte_count;
                        word_width01 = word_width01_next;
                        for (ch_copy_byte_count = 0; ch_copy_byte_count < ch_byte_count; ch_copy_byte_count++) {
                            ch01[ch_copy_byte_count] = (char)word[ch_count01_next + ch_copy_byte_count];
                        }
                        ch01[ch_copy_byte_count] = '\0';
                        word_width01_next += get_font_ch_char_w(ft_face01, ch01);
                        if ((curx + word_width01_next) > _width) {
                            over_flag01 = true;
                            break;
                        }  
                    }
                    if (over_flag01) {
                        drawChars(ft_face01, curx, cury,
                                    (word.substr(0, ch_count01).c_str()), ch_count01);
                        word = word.substr(ch_count01, word.size() - ch_count01);
                        curx = _freetype_x;
                        cury = freetype_next_line(cury, char_h);
                    } else {
                        if ((curx + word_width01_next + freetype_font_space01) <= _width) {
                            curx = drawChars(ft_face01, curx, cury, (word + ' ').c_str(), word.size() + 1);
                        } else {
                            drawChars(ft_face01, curx, cury, word.c_str(), word.size());
                            curx = _freetype_x;
                            cury = freetype_next_line(cury, char_h);
                        }
                        break;
                    }
                }
            }
            if (word.length() > 0) {
                if ((curx + word_width01 +  + freetype_font_space01) <= _width) {
                    curx = drawChars(ft_face01, curx, cury, (word + ' ').c_str(), word.size() + 1);
                } else {
                    drawChars(ft_face01, curx, cury, word.c_str(), word.size());
                    curx = _freetype_x;
                    cury = freetype_next_line(cury, char_h);
                }
            }
        }
        curx = _freetype_x;
        cury = freetype_next_line(cury, char_h);
    }
    _freetype_y = cury;
}

void canvas16_word_print(FT_Face *ft_face01, std::string text) {

    int32_t curx, cury, char_h, char_w;
    std::stringstream text_s{text};
    std::string lines;

    curx = _freetype_x;
    cury = _freetype_y;
    char_h = freetype_font_line_height01;
    char_w = freetype_font_height01 * 2;      // 文字の横幅の最大値
    while (std::getline(text_s, lines, '\n')) {

        int32_t spaces_num = freetype_check_leading_spaces(lines);
        std::stringstream lines_s{lines};
        std::string buf;
        std::vector<std::string> words;
        while (std::getline(lines_s, buf, ' ')) {
            words.push_back(buf);
        }

        if (spaces_num > 0) {
            words[0].insert(0, freetype_spaces(spaces_num));
        }
        for (int i = 0; i < words.size(); i++) {
            std::string word = words[i];
            int32_t word_width01 = get_font_word_w(ft_face01, word);
            if ((curx + word_width01) > canvas16_width) {
                if (curx != _freetype_x) cury = freetype_next_line(cury, char_h);
                curx = _freetype_x;
                while ((curx + get_font_word_w(ft_face01, word)) > canvas16_width) {
                    int32_t ch_byte_count = 0;
                    int32_t ch_count01 = 0;
                    int32_t ch_count01_next = 0;
                    int32_t word_width01 = 0;
                    int32_t word_width01_next = 0;
                    char ch01[9];
                    int32_t ch_copy_byte_count = 0;
                    bool over_flag01 = false;
                    for (ch_count01_next = 0; ch_count01_next < word.size();) {
                        ch_byte_count = utf8_byte_count((char)word[ch_count01_next]);
                        ch_count01 = ch_count01_next;
                        ch_count01_next += ch_byte_count;
                        word_width01 = word_width01_next;
                        for (ch_copy_byte_count = 0; ch_copy_byte_count < ch_byte_count; ch_copy_byte_count++) {
                            ch01[ch_copy_byte_count] = (char)word[ch_count01_next + ch_copy_byte_count];
                        }
                        ch01[ch_copy_byte_count] = '\0';
                        word_width01_next += get_font_ch_char_w(ft_face01, ch01);
                        if ((curx + word_width01_next) > canvas16_width) {
                            over_flag01 = true;
                            break;
                        }  
                    }
                    if (over_flag01) {
                        canvas16_drawChars(ft_face01, curx, cury,
                                    (word.substr(0, ch_count01).c_str()), ch_count01);
                        word = word.substr(ch_count01, word.size() - ch_count01);
                        curx = _freetype_x;
                        cury = freetype_next_line(cury, char_h);
                    } else {
                        if ((curx + word_width01_next + freetype_font_space01) <= canvas16_width) {
                            curx = canvas16_drawChars(ft_face01, curx, cury, (word + ' ').c_str(), word.size() + 1);
                        } else {
                            canvas16_drawChars(ft_face01, curx, cury, word.c_str(), word.size());
                            curx = _freetype_x;
                            cury = freetype_next_line(cury, char_h);
                        }
                        break;
                    }
                }
            }
            if (word.length() > 0) {
                if ((curx + word_width01 +  + freetype_font_space01) <= canvas16_width) {
                    curx = canvas16_drawChars(ft_face01, curx, cury, (word + ' ').c_str(), word.size() + 1);
                } else {
                    canvas16_drawChars(ft_face01, curx, cury, word.c_str(), word.size());
                    curx = _freetype_x;
                    cury = freetype_next_line(cury, char_h);
                }
            }
        }
        curx = _freetype_x;
        cury = freetype_next_line(cury, char_h);
    }
    _freetype_y = cury;
}
