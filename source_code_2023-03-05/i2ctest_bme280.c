/*
 * このプログラムはRISC-VのDebianのVisionFive2のI2Cテスト用に作ったものです。
 *
 * 気温、湿度、気圧センサーのBME280を使います。
 *
 * プログラム中で#define I2C_DEVICE "/dev/i2c-0"のようにデバイスを指定してます。
 * BME280のI2Cのデバイスを調べたい場合はi2cdetectを使います。
 * VisionFive2のDebianではi2cdetectで調べられないようなので他のLinuxマシンで調べてください。
 * i2cdetectはaptでi2c-toolsを入れると入ります。
 * 他のi2c関連のツールも入るのでi2c-toolsは入れるといいです。
 *
 * i2cdetectのパラメータに指定する番号はI2Cデバイスの番号になります。
 * /dev/i2c-0なら0を/dev/i2c-1なら1を指定します。
 * 下記のように0x76もしくは0x77が表示されればBME280が認識されています。
 * $ i2cdetect 0
 * WARNING! This program can confuse your I2C bus, cause data loss and worse!
 * I will probe file /dev/i2c-2.
 * I will probe address range 0x08-0x77.
 * Continue? [Y/n] y
 *      0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
 * 00:                         -- -- -- -- -- -- -- --
 * 10: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
 * 20: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
 * 30: -- -- -- -- -- -- -- -- -- -- -- -- 3c -- -- --
 * 40: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
 * 50: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
 * 60: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
 * 70: -- -- -- -- -- -- 76 --
 *
 *
 * コンパイル方法
 *
 * $ gcc -O2 -o i2ctest_bme280 i2ctest_bme280.c
 *
 *
 * 実行方法
 *
 * $ ./i2ctest_bme280
 *
 * (ここで、実行する前に自分が使ってるユーザがi2cグループに所属してるか確認してください。
 * $ cat /etc/group | grep i2c
 * i2cグループに所属してなければ次のようにi2cグループに所属させます。
 * $ sudo gpasswd -a 自分のユーザ名 i2c
 * )
 *
 * 
 * VisionFive2とSSD1306 OLEDの接続はこんな感じです
 *
 * ピンはGPIO57 ピンヘッダの5番ピンが/dev/i2c-1のSCL
 * ピンはGPIO58 ピンヘッダの3番ピンが/dev/i2c-1のSDA
 *
 *
 * (下記のように実行するとピンの利用状況が表示されるようです。)
 * $ sudo cat /sys/kernel/debug/pinctrl/13040000.gpio/pinmux-pins
 * 
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <signal.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>



#define I2C_DEVICE    "/dev/i2c-0"
#define BME280_DEVICE_ADDRESS 0x76

#define MODE_SLEEP 0b00
#define MODE_FORCED 0b01
#define MODE_NORMAL 0b11


int32_t i2c_open(const char *i2c_device);
int32_t i2c_close(int fd);
int i2c_read(int fd, uint8_t i2c_address, uint8_t reg, uint8_t *data, int16_t len);
int i2c_read_byte(int fd, uint8_t i2c_address, uint8_t reg, uint8_t *bytedata);
int i2c_write(int fd, uint8_t i2c_address, uint8_t reg, uint8_t *data, int16_t len);
int i2c_write_byte(int fd, uint8_t i2c_address, uint8_t reg, uint8_t bytedata);
void delay(long milli_sec); 
void sigint_handler(int sig);

int32_t compensate_temp(int32_t adc_T);
uint32_t compensate_pressure(int32_t adc_P);
uint32_t compensate_humidity(int32_t adc_H);
void read_compensation_parameters();
void bme280_read_raw(int32_t *humidity, int32_t *pressure, int32_t *temperature);
int32_t bme280_read_raw_pressure();
int32_t bme280_read_raw_temperature();
int32_t bme280_read_raw_humidity();
int bme280_isMeasuring(void);
void bme280_setMode(uint8_t mode);
void bme280_setStandbyTime(uint8_t timeSetting);
void bme280_setFilter(uint8_t filterSetting);
extern int bme280_i2c_fd01;


int i2c_fd01;



int main() {

    // SIGINTハンドラの設定
    if (signal(SIGINT, sigint_handler) == SIG_ERR) {
        perror("signal SIGINT");
        exit(-1);
    }

    printf("Hello, bme280! Reading raw data from registers via I2C...\n");

    i2c_fd01 = i2c_open(I2C_DEVICE);

    bme280_i2c_fd01 = i2c_fd01;

    // BME280 Reset
    // i2c_write_byte(BME280_DEVICE_ADDRESS, (uint8_t)0xE0, (uint8_t)0xB6);
    i2c_write_byte(bme280_i2c_fd01, BME280_DEVICE_ADDRESS, (uint8_t)0xE0, (uint8_t)0xB6);
    delay(100);

    // See if SPI is working - interrograte the device for its I2C ID number, should be 0x60
    uint8_t id;
    // id = i2c_read_byte(BME280_DEVICE_ADDRESS, (uint8_t)0xD0);
    i2c_read_byte(bme280_i2c_fd01, BME280_DEVICE_ADDRESS, (uint8_t)0xD0, &id);
    printf("Chip ID is 0x%x\n", id);

    read_compensation_parameters();

    // i2c_write_byte(BME280_DEVICE_ADDRESS, (uint8_t)0xF2, (uint8_t)0x1); // Humidity oversampling register - going for x1
    // i2c_write_byte(BME280_DEVICE_ADDRESS, (uint8_t)0xF4, (uint8_t)0x27);// Set rest of oversampling modes and run mode to normal
    i2c_write_byte(bme280_i2c_fd01, BME280_DEVICE_ADDRESS, (uint8_t)0xF2, (uint8_t)0x1); // Humidity oversampling register - going for x1
    i2c_write_byte(bme280_i2c_fd01, BME280_DEVICE_ADDRESS, (uint8_t)0xF4, (uint8_t)0x27);// Set rest of oversampling modes and run mode to normal
    bme280_setStandbyTime(0);
    bme280_setFilter(0);
    bme280_setMode(MODE_SLEEP);

    int32_t humidity, pressure, temperature;

    while (1) {
        bme280_setMode(MODE_FORCED);
        while (!bme280_isMeasuring()) delay(1);  // Wait before the start of measurement
        while ( bme280_isMeasuring()) delay(1);  // Wait during a measurement

        bme280_read_raw(&humidity, &pressure, &temperature);

        // These are the raw numbers from the chip, so we need to run through the
        // compensations to get human understandable numbers
        pressure = compensate_pressure(pressure);
        temperature = compensate_temp(temperature);
        humidity = compensate_humidity(humidity);

        printf("Humidity : %.2f%%\r\n", humidity / 1024.0);
        printf("Pressure : %.2fhPa\r\n", pressure / 100.0);
        printf("Temp     : %.2fC\r\n", temperature / 100.0);
        printf("\n");

        delay(5000);
    }

    return 0;
}

void sigint_handler(int sig) {
    i2c_close(bme280_i2c_fd01);
    exit(1);
}




int bme280_i2c_fd01;

int32_t t_fine;

uint16_t dig_T1;
int16_t dig_T2, dig_T3;
uint16_t dig_P1;
int16_t dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8, dig_P9;
uint8_t dig_H1, dig_H3;
int8_t dig_H6;
int16_t dig_H2, dig_H4, dig_H5;

int32_t compensate_temp(int32_t adc_T) {
    int32_t var1, var2, T;
    var1 = ((((adc_T >> 3) - ((int32_t) dig_T1 << 1))) * ((int32_t) dig_T2)) >> 11;
    var2 = (((((adc_T >> 4) - ((int32_t) dig_T1)) * ((adc_T >> 4) - ((int32_t) dig_T1))) >> 12) * ((int32_t) dig_T3))
            >> 14;

    t_fine = var1 + var2;
    T = (t_fine * 5 + 128) >> 8;
    return T;
}

uint32_t compensate_pressure(int32_t adc_P) {
    int32_t var1, var2;
    uint32_t p;
    var1 = (((int32_t) t_fine) >> 1) - (int32_t) 64000;
    var2 = (((var1 >> 2) * (var1 >> 2)) >> 11) * ((int32_t) dig_P6);
    var2 = var2 + ((var1 * ((int32_t) dig_P5)) << 1);
    var2 = (var2 >> 2) + (((int32_t) dig_P4) << 16);
    var1 = (((dig_P3 * (((var1 >> 2) * (var1 >> 2)) >> 13)) >> 3) + ((((int32_t) dig_P2) * var1) >> 1)) >> 18;
    var1 = ((((32768 + var1)) * ((int32_t) dig_P1)) >> 15);
    if (var1 == 0)
        return 0;

    p = (((uint32_t) (((int32_t) 1048576) - adc_P) - (var2 >> 12))) * 3125;
    if (p < 0x80000000)
        p = (p << 1) / ((uint32_t) var1);
    else
        p = (p / (uint32_t) var1) * 2;

    var1 = (((int32_t) dig_P9) * ((int32_t) (((p >> 3) * (p >> 3)) >> 13))) >> 12;
    var2 = (((int32_t) (p >> 2)) * ((int32_t) dig_P8)) >> 13;
    p = (uint32_t) ((int32_t) p + ((var1 + var2 + dig_P7) >> 4));

    return p;
}

uint32_t compensate_humidity(int32_t adc_H) {
    int32_t v_x1_u32r;
    v_x1_u32r = (t_fine - ((int32_t) 76800));
    v_x1_u32r = (((((adc_H << 14) - (((int32_t) dig_H4) << 20) - (((int32_t) dig_H5) * v_x1_u32r)) +
    ((int32_t) 16384)) >> 15) * (((((((v_x1_u32r * ((int32_t) dig_H6)) >> 10) * (((v_x1_u32r *
    ((int32_t) dig_H3)) >> 11) + ((int32_t) 32768))) >> 10) + ((int32_t) 2097152)) *
    ((int32_t) dig_H2) + 8192) >> 14));
    v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * ((int32_t) dig_H1)) >> 4));
    v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
    v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);

    return (uint32_t) (v_x1_u32r >> 12);
}


/* This function reads the manufacturing assigned compensation parameters from the device */
void read_compensation_parameters() {
    uint8_t buffer[26];

    //i2c_read_buf(BME280_DEVICE_ADDRESS, (uint8_t)0x88, buffer, 24);
    i2c_read(bme280_i2c_fd01, BME280_DEVICE_ADDRESS, (uint8_t)0x88, buffer, 24);

    dig_T1 = buffer[0] | (buffer[1] << 8);
    dig_T2 = buffer[2] | (buffer[3] << 8);
    dig_T3 = buffer[4] | (buffer[5] << 8);

    dig_P1 = buffer[6] | (buffer[7] << 8);
    dig_P2 = buffer[8] | (buffer[9] << 8);
    dig_P3 = buffer[10] | (buffer[11] << 8);
    dig_P4 = buffer[12] | (buffer[13] << 8);
    dig_P5 = buffer[14] | (buffer[15] << 8);
    dig_P6 = buffer[16] | (buffer[17] << 8);
    dig_P7 = buffer[18] | (buffer[19] << 8);
    dig_P8 = buffer[20] | (buffer[21] << 8);
    dig_P9 = buffer[22] | (buffer[23] << 8);

    
    // dig_H1 = i2c_read_byte(BME280_DEVICE_ADDRESS, (uint8_t)0xA1);
    i2c_read_byte(bme280_i2c_fd01, BME280_DEVICE_ADDRESS, (uint8_t)0xA1, &dig_H1);

    //i2c_read_buf(BME280_DEVICE_ADDRESS, (uint8_t)0xE1, buffer, 7);
    i2c_read(bme280_i2c_fd01, BME280_DEVICE_ADDRESS, (uint8_t)0xE1, buffer, 7);

    dig_H2 = buffer[0] | ((int16_t)buffer[1] << 8);
    dig_H3 = (int8_t) buffer[2];
    dig_H4 = (int16_t)buffer[3] << 4 | (buffer[4] & 0xf);
    dig_H5 = ((buffer[4] >> 4) & 0x0f) | ((int16_t)buffer[5] << 4);
    dig_H6 = (int8_t) buffer[6];
}

void bme280_read_raw(int32_t *humidity, int32_t *pressure, int32_t *temperature) {
    uint8_t buffer[8];

    // i2c_read_buf(BME280_DEVICE_ADDRESS, (uint8_t)0xF7, buffer, 8);
    i2c_read(bme280_i2c_fd01, BME280_DEVICE_ADDRESS, (uint8_t)0xF7, buffer, 8);

    *pressure = ((uint32_t) buffer[0] << 12) | ((uint32_t) buffer[1] << 4) | ((uint32_t)buffer[2] >> 4);
    *temperature = ((uint32_t) buffer[3] << 12) | ((uint32_t) buffer[4] << 4) | ((uint32_t)buffer[5] >> 4);
    *humidity = ((uint32_t) buffer[6] << 8) | ((uint32_t)buffer[7]);
}


int32_t bme280_read_raw_pressure() {
    uint8_t buffer[3];
    int32_t pressure;

    // i2c_read_buf(BME280_DEVICE_ADDRESS, (uint8_t)0xF7, buffer, 3);
    i2c_read(bme280_i2c_fd01, BME280_DEVICE_ADDRESS, (uint8_t)0xF7, buffer, 3);

    pressure = ((uint32_t) buffer[0] << 12) | ((uint32_t) buffer[1] << 4) | ((uint32_t)buffer[2] >> 4);
    return pressure;
}

int32_t bme280_read_raw_temperature() {
    uint8_t buffer[3];
    int32_t temperature;

    // i2c_read_buf(BME280_DEVICE_ADDRESS, (uint8_t)0xFA, buffer, 3);
    i2c_read(bme280_i2c_fd01, BME280_DEVICE_ADDRESS, (uint8_t)0xFA, buffer, 3);

    temperature = ((uint32_t) buffer[0] << 12) | ((uint32_t) buffer[1] << 4) | ((uint32_t)buffer[2] >> 4);
    return temperature;
}

int32_t bme280_read_raw_humidity() {
    uint8_t buffer[3];
    int32_t humidity;

    // i2c_read_buf(BME280_DEVICE_ADDRESS, (uint8_t)0xFD, buffer, 2);
    i2c_read(bme280_i2c_fd01, BME280_DEVICE_ADDRESS, (uint8_t)0xFD, buffer, 2);

    humidity = ((uint32_t) buffer[0] << 8) | ((uint32_t)buffer[1]);
    return humidity;
}






int bme280_isMeasuring(void) {
        // uint8_t stat = i2c_read_byte(BME280_DEVICE_ADDRESS, (uint8_t)0xF3);
        uint8_t stat; 
        i2c_read_byte(bme280_i2c_fd01, BME280_DEVICE_ADDRESS, (uint8_t)0xF3, &stat);
        return(stat & (1<<3)); 
}

void bme280_setMode(uint8_t mode) {
        if(mode > 0b11) mode = 0; 
        
        // uint8_t controlData = i2c_read_byte(BME280_DEVICE_ADDRESS, (uint8_t)0xF4);
        uint8_t controlData;
        i2c_read_byte(bme280_i2c_fd01, BME280_DEVICE_ADDRESS, (uint8_t)0xF4, &controlData);
        controlData &= ~( (1<<1) | (1<<0) ); //Clear the mode[1:0] bits
        controlData |= mode; 
        // i2c_write_byte(BME280_DEVICE_ADDRESS, (uint8_t)0xF4, controlData);
        i2c_write_byte(bme280_i2c_fd01, BME280_DEVICE_ADDRESS, (uint8_t)0xF4, controlData);
}

//  0, 0.5ms
//  1, 62.5ms
//  2, 125ms
//  3, 250ms
//  4, 500ms
//  5, 1000ms
//  6, 10ms
//  7, 20ms
void bme280_setStandbyTime(uint8_t timeSetting) {
        if(timeSetting > 0b111) timeSetting = 0; 
        
        // uint8_t controlData = i2c_read_byte(BME280_DEVICE_ADDRESS, (uint8_t)0xF5);
        uint8_t controlData;
        i2c_read_byte(bme280_i2c_fd01, BME280_DEVICE_ADDRESS, (uint8_t)0xF5, &controlData);
        controlData &= ~( (1<<7) | (1<<6) | (1<<5) ); //Clear the 7/6/5 bits
        controlData |= (timeSetting << 5); //Align with bits 7/6/5
        // i2c_write_byte(BME280_DEVICE_ADDRESS, (uint8_t)0xF5, controlData);
        i2c_write_byte(bme280_i2c_fd01, BME280_DEVICE_ADDRESS, (uint8_t)0xF5, controlData);
}

//  0, filter off
//  1, coefficients = 2
//  2, coefficients = 4
//  3, coefficients = 8
//  4, coefficients = 16
void bme280_setFilter(uint8_t filterSetting) {
        if(filterSetting > 0b111) filterSetting = 0; 
        
        // uint8_t controlData = i2c_read_byte(BME280_DEVICE_ADDRESS, (uint8_t)0xF5);
        uint8_t controlData;
        i2c_read_byte(bme280_i2c_fd01, BME280_DEVICE_ADDRESS, (uint8_t)0xF5, &controlData);
        controlData &= ~( (1<<4) | (1<<3) | (1<<2) ); //Clear the 4/3/2 bits
        controlData |= (filterSetting << 2); //Align with bits 4/3/2
        // i2c_write_byte(BME280_DEVICE_ADDRESS, (uint8_t)0xF5, controlData);
        i2c_write_byte(bme280_i2c_fd01, BME280_DEVICE_ADDRESS, (uint8_t)0xF5, controlData);
}


int32_t i2c_open(const char *i2c_device) {

    int fd = open(i2c_device, O_RDWR);
    if (fd == -1) {
        perror("i2c_open");
    }

    return fd;
}

int32_t i2c_close(int fd) {
    close(fd);
}

int i2c_read(int fd, uint8_t i2c_address, uint8_t reg, uint8_t *data, int16_t len) {
    int ret;

    struct i2c_msg i2c_read_msg01[] = {
        {i2c_address, 0, 1, &reg},
        {i2c_address, I2C_M_RD, len, data},
    };

    struct i2c_rdwr_ioctl_data read_ioctl_data = {i2c_read_msg01, 2};

    if ((ret = ioctl(fd, I2C_RDWR, &read_ioctl_data)) < 0) {
        perror("i2c_read ioctl");
        return ret;
    }

    return 0;
}

int i2c_read_byte(int fd, uint8_t i2c_address, uint8_t reg, uint8_t *bytedata) {
    int ret;
    if ((ret = i2c_read(fd, i2c_address, reg, bytedata, 1)) < 0) {
         perror("i2c_read_byte");
         return ret;
    }

    return 0;
}

int i2c_write(int fd, uint8_t i2c_address, uint8_t reg, uint8_t *data, int16_t len) {

    uint8_t *buffer01;
    if ((buffer01 = (uint8_t *)malloc(len + 1)) == NULL) {
        perror("i2c_write malloc");
        return -1;
    }

    buffer01[0] = reg;                    // write用バッファーの最初のバイトにi2cデバイスのレジスタ アドレスを入れる
    memcpy(buffer01 + 1, data, len);      // write用バッファーの2番目以降のバイトに書き込みデータを 格納

    struct i2c_msg i2c_write_msg01 = {i2c_address, 0, len + 1, buffer01};
    struct i2c_rdwr_ioctl_data write_ioctl_data = {&i2c_write_msg01, 1};

    int ret;
    if ((ret = ioctl(fd, I2C_RDWR, &write_ioctl_data)) < 0) {
        perror("i2c_write ioctl");
        return ret;
    }

    free(buffer01);
    return 0;
}

int i2c_write_byte(int fd, uint8_t i2c_address, uint8_t reg, uint8_t bytedata) {
    int ret;
    if ((ret = i2c_write(fd, i2c_address, reg, &bytedata, 1)) < 0) {
         perror("i2c_write_byte");
         return ret;
    }

    return 0;
}

struct timespec delay_nano_sleep_param01_req, delay_nano_sleep_param01_rem;

void delay(long milli_sec) {
    int nanosleep_ret;
    delay_nano_sleep_param01_req.tv_sec = milli_sec / 1000;
    delay_nano_sleep_param01_req.tv_nsec = (milli_sec % 1000) * 1000 * 1000;
    nanosleep_ret = nanosleep(&delay_nano_sleep_param01_req, &delay_nano_sleep_param01_rem);
    while (nanosleep_ret == -1) {
        delay_nano_sleep_param01_req.tv_sec = delay_nano_sleep_param01_rem.tv_sec;
        delay_nano_sleep_param01_req.tv_nsec = delay_nano_sleep_param01_rem.tv_nsec;
        nanosleep_ret = nanosleep(&delay_nano_sleep_param01_req, &delay_nano_sleep_param01_rem);
    }
}