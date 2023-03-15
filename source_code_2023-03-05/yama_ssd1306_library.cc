#include "yama_ssd1306_library.h"



// i2c読み書き用の関数群

// i2c_error_num_maxで設定した値の回数だけ書き込み及び読み込みで連続してエラーが出た場合に書き込みおよび読み込みを停止します。エラーを放置するとLinux自体がハングするための処置です。
int i2c_error_num_max = 4;
int i2c_error_num = 0;
int32_t i2c_open(const char *i2c_device) {

    int fd = open(i2c_device, O_RDWR);
    if (fd == -1) {
        perror("i2c_open");
    }

    return fd;
}

void i2c_close(int fd) {
    if (fd > 0) {
        close(fd);
    }
}

int i2c_read(int fd, uint8_t i2c_address, uint8_t reg, uint8_t *data, uint16_t len) {
    int ret;

    if (i2c_error_num > i2c_error_num_max) {
        printf("i2c error!!\nfailed %d times in a row\n", i2c_error_num_max);
        return -1;
    }
    
    struct i2c_msg i2c_read_msg01[] = {
        {i2c_address, 0, 1, &reg},
        {i2c_address, I2C_M_RD, len, data},
    };

    struct i2c_rdwr_ioctl_data read_ioctl_data = {i2c_read_msg01, 2};

    if ((ret = ioctl(fd, I2C_RDWR, &read_ioctl_data)) < 0) {
        i2c_error_num++;
        perror("i2c_read ioctl");
        return ret;
    }
    i2c_error_num = 0;

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

int i2c_write(int fd, uint8_t i2c_address, uint8_t reg, uint8_t *data, uint16_t len) {

    uint8_t *buffer01;
    if (i2c_error_num > i2c_error_num_max) {
        printf("i2c error!!\nfailed %d times in a row\n", i2c_error_num_max);
        return -1;
    }

    if ((buffer01 = (uint8_t *)malloc(len + 1)) == NULL) {
        perror("i2c_write malloc");
        return -1;
    }

    buffer01[0] = reg;                    // write用バッファーの最初のバイトにi2cデバイスのレジスタ アドレスを入れる
    memcpy(buffer01 + 1, data, len);      // write用バッファーの2番目以降のバイトに書き込みデータを 格納

    struct i2c_msg i2c_write_msg01 = {i2c_address, 0, (uint16_t)(len + 1), buffer01};
    struct i2c_rdwr_ioctl_data write_ioctl_data = {&i2c_write_msg01, 1};

    int ret;
    if ((ret = ioctl(fd, I2C_RDWR, &write_ioctl_data)) < 0) {
        i2c_error_num++;
        perror("i2c_write ioctl");
        return ret;
    }

    free(buffer01);
    i2c_error_num = 0;

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

int i2c_write_nonreg(int fd, uint8_t i2c_address, uint8_t *data, uint16_t len) {

    if (i2c_error_num > i2c_error_num_max) {
        printf("i2c error!!\nfailed %d times in a row\n", i2c_error_num_max);
        return -1;
    }
    struct i2c_msg i2c_write_msg01 = {i2c_address, 0, len, data};
    struct i2c_rdwr_ioctl_data write_ioctl_data = {&i2c_write_msg01, 1};

    int ret = 0;
    if ((ret = ioctl(fd, I2C_RDWR, &write_ioctl_data)) < 0) {
        i2c_error_num++;
        perror("i2c_write_nonreg ioctl");
        return ret;
    }
    i2c_error_num = 0;

    return 0;
}








// libfreetypeを使ったssd1306で文字表示をするための関数群や変数
#define SSD1306_MAX_TEXT_SIZE01 32767
#define SSD1306_STR_BUFF01_LEN01 32768
bool ssd1306_back_ground_fill_flag = false;
int32_t ssd1306_freetype_fg_color = SSD1306_WHITE;
int32_t ssd1306_freetype_bg_color = SSD1306_BLACK;
int32_t ssd1306_freetype_font_size01 = 8 * 16;
int32_t ssd1306_freetype_font_height01 = 16;
int32_t ssd1306_freetype_font_line_height01 = 16;
int32_t ssd1306_freetype_font_space01 = 8;
int32_t ssd1306_freetype_x = 0;
int32_t ssd1306_freetype_y = 0;
char ssd1306_freetype_print_str_buff01[SSD1306_STR_BUFF01_LEN01];

int init_ssd1306_freetype(FT_Library *ft_library01) {
    int ret = 0;
    if ((ret = FT_Init_FreeType(ft_library01 )) != 0) {
        perror("FT_Init_FreeType ft_library01");
        return ret;
    }

    return ret;
}

int ssd1306_set_font(FT_Library *ft_library01, FT_Face *ft_face01, const char *font_path) {

    int ret = 0;
    if ((ret = FT_New_Face(*ft_library01, font_path, 0, ft_face01)) != 0) {
        perror("FT_New_Face ft_library01 ft_face01");
        return ret;
    }

    if ((ret = FT_Set_Char_Size(*ft_face01, 0,
                      8 * 16,    // 文字の幅 * 高さ
                      300, 300)   // X方向のdpi、Y方向のdpi
                      ) != 0) {
        perror("FT_Set_Char_Size ft_face01");
        return ret;
    }
    int32_t size = 16;
    ssd1306_freetype_font_size01 = (size / 2) * size;
    ssd1306_freetype_font_height01 = size;
    ssd1306_freetype_font_line_height01 = size;
    int32_t ssd1306_get_font_ch_char_w(FT_Face *ft_face01, const char *text);
    ssd1306_freetype_font_space01 = ssd1306_get_font_ch_char_w(ft_face01, u8"a");

    return ret;
}

int ssd1306_set_font_size(FT_Face *ft_face01, int32_t size, int32_t x_dpi, int32_t y_dpi) {
    int ret = 0;

    if ((ret = FT_Set_Char_Size(*ft_face01, 0,
                      ((size - 2) / 2) * size,    // 文字の幅 * 高さ
                      x_dpi, y_dpi)   // X方向のdpi、Y方向のdpi
                      ) != 0) {
        perror("FT_Set_Char_Size ft_face01");
        return ret;
    }
    ssd1306_freetype_font_size01 = (size / 2) * size;
    ssd1306_freetype_font_height01 = size;
    ssd1306_freetype_font_line_height01 = size;
    int32_t ssd1306_get_font_ch_char_w(FT_Face *ft_face01, const char *text);
    ssd1306_freetype_font_space01 = ssd1306_get_font_ch_char_w(ft_face01, u8"a");

    return ret;
}

int32_t ssd1306_utf8_byte_count(char ch) {

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

std::u32string ssd1306_utf8_to_utf32(const char *text, int32_t buf_size) {

    int i;
    uint32_t a, byte_count01;
    std::u32string str1 = U"";
    for (i = 0; i < buf_size; i++) {
        if (text[i] == '\0') break;
        byte_count01 = ssd1306_utf8_byte_count(text[i]);
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

void ssd1306_set_font_back_ground_fill(bool flag) {
    ssd1306_back_ground_fill_flag = flag;
}

void ssd1306_set_font_color(int32_t fg, int32_t bg) {
    ssd1306_freetype_fg_color = fg;
    if (bg != -1) {
        ssd1306_freetype_bg_color = bg;
    }
}

int32_t ssd1306_drawChars(FT_Face *ft_face01, int32_t char_x, int32_t char_y, const char *text, int32_t buf_size) {

    int ret = 0;
    int32_t str_w = 0;
    std::u32string  u32_string01 = ssd1306_utf8_to_utf32(text, buf_size);

    for (int k = 0; k < u32_string01.size(); k++) {
        if ((u32_string01[k] == 0x0000000a) || (u32_string01[k] == 0x00000020) || (u32_string01[k] == 0x000000a0) || (u32_string01[k] == 0x00003000) ) {
            if (u32_string01[k] == 0x00003000) {
                str_w += ssd1306_freetype_font_space01 * 2;
            } else if ((u32_string01[k] == 0x00000020) || (u32_string01[k] == 0x000000a0)){
                str_w += ssd1306_freetype_font_space01;
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
                        ssd1306_drawPixel(char_x + i + str_w,
                                  char_y - (*ft_face01)->glyph->bitmap_top + j, ssd1306_freetype_fg_color);
                    } else {
                        if (ssd1306_back_ground_fill_flag) 
                       ssd1306_drawPixel(char_x + (*ft_face01)->glyph->bitmap_left + i + str_w,
                  char_y - (*ft_face01)->glyph->bitmap_top + j, ssd1306_freetype_bg_color);
                    }
                }
            }
        str_w += (*ft_face01)->glyph->bitmap.width + (*ft_face01)->glyph->bitmap_left;
        }
    }
    return (char_x + str_w);
}

int32_t ssd1306_freetype_next_line(int32_t cury, int32_t char_h) {
    int32_t res;
    res = cury + char_h;
    return res;
}

void ssd1306_set_font_line_height(int32_t char_h) {
    ssd1306_freetype_font_line_height01 = char_h;
}

int ssd1306_freetype_substr01(const char *source, char *dest, int buffsize, int start, int length) {
    char *sp, *ep, size;
    size = buffsize - 1;
    int32_t source_len01 = strnlen(source,  SSD1306_MAX_TEXT_SIZE01);

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

int32_t ssd1306_get_font_ch_char_w(FT_Face *ft_face01, const char *text) {
    int32_t width = 0;
    std::u32string u32_string01 = ssd1306_utf8_to_utf32(text, 9);
    if ((u32_string01[0] == 0x0000000a) || (u32_string01[0] == 0x00000020) || (u32_string01[0] == 0x000000a0) || (u32_string01[0] == 0x00003000) ) {
        if (u32_string01[0] == 0x00003000) {
            width = ssd1306_freetype_font_space01 * 2;
        } else if ((u32_string01[0] == 0x00000020) || (u32_string01[0] == 0x000000a0)){
            width = ssd1306_freetype_font_space01;
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

void ssd1306_set_font_pos(int32_t x, int32_t y) {
    ssd1306_freetype_x = x;
    ssd1306_freetype_y = y;
}

void ssd1306_print(FT_Face *ft_face01, const char *text) { // does character wrap, compatible with stream output

    int32_t curx, cury, char_w, char_h, width1, written;
    int32_t str_len01;

    curx = ssd1306_freetype_x;
    cury = ssd1306_freetype_y;
    char_h = ssd1306_freetype_font_line_height01;
    width1 = 0;
    written = 0;

    str_len01 = strnlen(text, SSD1306_MAX_TEXT_SIZE01);
    if (str_len01 > SSD1306_STR_BUFF01_LEN01) return ;

    int32_t pos_next = 0;
    int32_t ch_byte_count = 0;
    for (int32_t pos = 0; pos < str_len01; pos = pos_next) {
        char ch[9];
        ch_byte_count = ssd1306_utf8_byte_count(text[pos]);
        for (int i = 0; i < ch_byte_count; i++) {
            ch[i] = text[pos_next++];
        }
        ch[ch_byte_count] = '\0';
        if ((ch[0] == '\n') && (ch_byte_count == 1)) {
            if (pos > 0) {
                ssd1306_freetype_substr01(text, ssd1306_freetype_print_str_buff01, SSD1306_STR_BUFF01_LEN01, written, pos - written);
                ssd1306_drawChars(ft_face01, curx, cury, ssd1306_freetype_print_str_buff01, SSD1306_STR_BUFF01_LEN01);
            }
            curx = 0;
            written = pos_next;
            width1 = 0;
            cury = ssd1306_freetype_next_line(cury, char_h);
        } else {
            char_w = ssd1306_get_font_ch_char_w(ft_face01, ch);
            if ((curx + width1 + char_w) == ssd1306_width) {
                ssd1306_freetype_substr01(text, ssd1306_freetype_print_str_buff01, SSD1306_STR_BUFF01_LEN01, written, pos_next - written);
                ssd1306_drawChars(ft_face01, curx, cury, ssd1306_freetype_print_str_buff01, SSD1306_STR_BUFF01_LEN01);
                curx = 0;
                written = pos_next;
                width1 = 0;
                cury = ssd1306_freetype_next_line(cury, char_h);
            
            } else if ((curx + width1 + char_w) > ssd1306_width) {
                ssd1306_freetype_substr01(text, ssd1306_freetype_print_str_buff01, SSD1306_STR_BUFF01_LEN01, written, pos - written);
                ssd1306_drawChars(ft_face01, curx, cury, ssd1306_freetype_print_str_buff01, SSD1306_STR_BUFF01_LEN01);
                curx = 0;
                width1 = 0;
                cury = ssd1306_freetype_next_line(cury, char_h);
                ssd1306_freetype_substr01(text, ssd1306_freetype_print_str_buff01, SSD1306_STR_BUFF01_LEN01, pos, pos_next - pos);
                ssd1306_drawChars(ft_face01, curx, cury, ssd1306_freetype_print_str_buff01, SSD1306_STR_BUFF01_LEN01);
                curx += char_w;
                width1 += char_w;
                written = pos_next;
            } else {
                width1 += char_w;
            }
        }
    }
    if (written < str_len01) {
        ssd1306_freetype_substr01(text, ssd1306_freetype_print_str_buff01, SSD1306_STR_BUFF01_LEN01, written, str_len01 - written);
        curx = ssd1306_drawChars(ft_face01, curx, cury, ssd1306_freetype_print_str_buff01, SSD1306_STR_BUFF01_LEN01);
    }
    ssd1306_freetype_x = curx;
    ssd1306_freetype_y = cury;
}

int32_t get_font_word_w(FT_Face *ft_face01, std::string word) {
    int32_t word_width = 0;
    int32_t ch_byte_count02 = 0;
    for (int i = 0; i < word.size(); i += ch_byte_count02) {
        ch_byte_count02 = ssd1306_utf8_byte_count(word[i]);
        word_width += ssd1306_get_font_ch_char_w(ft_face01, word.substr(i, ch_byte_count02).c_str());
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
    std::string spaces01 = u8"";
    for (int i = 0; i < number; i++) {
        spaces01.push_back(' ');
    }

    return spaces01;
}

void ssd1306_word_print(FT_Face *ft_face01, std::string text) {

    int32_t curx, cury, char_h, char_w;
    std::stringstream text_s{text};
    std::string lines;

    curx = ssd1306_freetype_x;
    cury = ssd1306_freetype_y;
    char_h = ssd1306_freetype_font_line_height01;
    char_w = ssd1306_freetype_font_height01 * 2;      // 文字の横幅の最大値
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
            if ((curx + word_width01) > ssd1306_width) {
                if (curx != ssd1306_freetype_x) cury = ssd1306_freetype_next_line(cury, char_h);
                curx = ssd1306_freetype_x;
                while ((curx + get_font_word_w(ft_face01, word)) > ssd1306_width) {
                    int32_t ch_byte_count = 0;
                    int32_t ch_count01 = 0;
                    int32_t ch_count01_next = 0;
                    int32_t word_width01 = 0;
                    int32_t word_width01_next = 0;
                    char ch01[9];
                    int32_t ch_copy_byte_count = 0;
                    bool over_flag01 = false;
                    for (ch_count01_next = 0; ch_count01_next < word.size();) {
                        ch_byte_count = ssd1306_utf8_byte_count((char)word[ch_count01_next]);
                        ch_count01 = ch_count01_next;
                        ch_count01_next += ch_byte_count;
                        word_width01 = word_width01_next;
                        for (ch_copy_byte_count = 0; ch_copy_byte_count < ch_byte_count; ch_copy_byte_count++) {
                            ch01[ch_copy_byte_count] = (char)word[ch_count01_next + ch_copy_byte_count];
                        }
                        ch01[ch_copy_byte_count] = '\0';
                        word_width01_next += ssd1306_get_font_ch_char_w(ft_face01, ch01);
                        if ((curx + word_width01_next) > ssd1306_width) {
                            over_flag01 = true;
                            break;
                        }  
                    }
                    if (over_flag01) {
                        ssd1306_drawChars(ft_face01, curx, cury,
                                    (word.substr(0, ch_count01).c_str()), ch_count01);
                        word = word.substr(ch_count01, word.size() - ch_count01);
                        curx = ssd1306_freetype_x;
                        cury = ssd1306_freetype_next_line(cury, char_h);
                    } else {
                        if ((curx + word_width01_next + ssd1306_freetype_font_space01) <= ssd1306_width) {
                            curx = ssd1306_drawChars(ft_face01, curx, cury, (word + ' ').c_str(), word.size() + 1);
                        } else {
                            ssd1306_drawChars(ft_face01, curx, cury, word.c_str(), word.size());
                            curx = ssd1306_freetype_x;
                            cury = ssd1306_freetype_next_line(cury, char_h);
                        }
                        break;
                    }
                }
            }
            if (word.length() > 0) {
                if ((curx + word_width01 +  + ssd1306_freetype_font_space01) <= ssd1306_width) {
                    curx = ssd1306_drawChars(ft_face01, curx, cury, (word + ' ').c_str(), word.size() + 1);
                } else {
                    ssd1306_drawChars(ft_face01, curx, cury, word.c_str(), word.size());
                    curx = ssd1306_freetype_x;
                    cury = ssd1306_freetype_next_line(cury, char_h);
                }
            }
        }
        curx = ssd1306_freetype_x;
        cury = ssd1306_freetype_next_line(cury, char_h);
    }
    ssd1306_freetype_y = cury;
}









// ssd1306を使うための関数群や変数

// 
// 
//    
//    これ以降のプログラムはAdafruit_SSD1306を元に作られています。
//    https://github.com/adafruit/Adafruit_SSD1306
//
//    Copyright (c) 2012 Adafruit Industries.
//    Released under the BSD License
//    https://github.com/adafruit/Adafruit_SSD1306/blob/master/license.txt
//
// 
//
//                                 https://twitter.com/yama23238
//
//      これらのプログラムの使用に当たってはご自分の責任において使用してください
//      これらのプログラムで発生したいかなる損害、データの消失、金銭等の責任は一切負いません。
//


#define SSD1306_MEMORYMODE 0x20          ///< See datasheet
#define SSD1306_COLUMNADDR 0x21          ///< See datasheet
#define SSD1306_PAGEADDR 0x22            ///< See datasheet
#define SSD1306_SETCONTRAST 0x81         ///< See datasheet
#define SSD1306_CHARGEPUMP 0x8D          ///< See datasheet
#define SSD1306_SEGREMAP 0xA0            ///< See datasheet
#define SSD1306_DISPLAYALLON_RESUME 0xA4 ///< See datasheet
#define SSD1306_DISPLAYALLON 0xA5        ///< Not currently used
#define SSD1306_NORMALDISPLAY 0xA6       ///< See datasheet
#define SSD1306_INVERTDISPLAY 0xA7       ///< See datasheet
#define SSD1306_SETMULTIPLEX 0xA8        ///< See datasheet
#define SSD1306_DISPLAYOFF 0xAE          ///< See datasheet
#define SSD1306_DISPLAYON 0xAF           ///< See datasheet
#define SSD1306_COMSCANINC 0xC0          ///< Not currently used
#define SSD1306_COMSCANDEC 0xC8          ///< See datasheet
#define SSD1306_SETDISPLAYOFFSET 0xD3    ///< See datasheet
#define SSD1306_SETDISPLAYCLOCKDIV 0xD5  ///< See datasheet
#define SSD1306_SETPRECHARGE 0xD9        ///< See datasheet
#define SSD1306_SETCOMPINS 0xDA          ///< See datasheet
#define SSD1306_SETVCOMDETECT 0xDB       ///< See datasheet

#define SSD1306_SETLOWCOLUMN 0x00  ///< Not currently used
#define SSD1306_SETHIGHCOLUMN 0x10 ///< Not currently used
#define SSD1306_SETSTARTLINE 0x40  ///< See datasheet

#define SSD1306_RIGHT_HORIZONTAL_SCROLL 0x26              ///< Init rt scroll
#define SSD1306_LEFT_HORIZONTAL_SCROLL 0x27               ///< Init left scroll
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 0x29 ///< Init diag scroll
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL 0x2A  ///< Init diag scroll
#define SSD1306_DEACTIVATE_SCROLL 0x2E                    ///< Stop scroll
#define SSD1306_ACTIVATE_SCROLL 0x2F                      ///< Start scroll
#define SSD1306_SET_VERTICAL_SCROLL_AREA 0xA3             ///< Set scroll range

#define SSD1306_OLEDWIDTH 128 ///< DEPRECATED: width w/SSD1306_128_64 defined
#define SSD1306_OLEDHEIGHT 64 ///< DEPRECATED: height w/SSD1306_128_64 defined

#define SSD1306_I2C_WIRE_MAX 1280

int        ssd1306_i2c_fd01 = -1;
int32_t    ssd1306_width = 128;
int32_t    ssd1306_height = 64;
int32_t    ssd1306_init_width = 128;
int32_t    ssd1306_init_height = 64;
int32_t    ssd1306_rotation = 0;

uint8_t    *ssd1306_buffer = NULL;
int8_t     ssd1306_i2caddr, ssd1306_vccstate, ssd1306_page_end;
uint8_t    ssd1306_contrast;
uint8_t    ssd1306_i2c_buffer01[SSD1306_I2C_WIRE_MAX + 1];

void ssd1306_i2c_write_byte_non_register(int addr, uint8_t val);
void ssd1306_i2c_write_blocking(int fd, uint8_t i2c_address, uint8_t *data, int16_t len, int flag);

void ssd1306_drawFastHLineInternal(int16_t x, int16_t y, int16_t w, uint16_t color);
void ssd1306_drawFastVLineInternal(int16_t x, int16_t y, int16_t h, uint16_t color);
void ssd1306_command1(uint8_t c);
void ssd1306_commandList(const uint8_t *c, uint8_t n);
inline const uint8_t ssd1306_read_byte_data(const uint8_t *adr) {
      return *adr;
}


// SOME DEFINES AND STATIC VARIABLES USED INTERNALLY -----------------------


#define SSD1306_swap(a, b)                                                     \
  (((a) ^= (b)), ((b) ^= (a)), ((a) ^= (b))) ///< No-temp-var swap operation

#define SSD1306_PROGMEM 

#define SSD1306_SELECT
#define SSD1306_DESELECT
#define SSD1306_MODE_COMMAND
#define SSD1306_MODE_DATA


#define SSD1306_I2C_SETWIRECLOCK ///< Dummy stand-in define
#define SSD1306_I2C_RESWIRECLOCK ///< keeps compiler happy

// The definition of 'transaction' is broadened a bit in the context of
// this library -- referring not just to SPI transactions (if supported
// in the version of the SPI library being used), but also chip select
// (if SPI is being used, whether hardware or soft), and also to the
// beginning and end of I2C transfers (the Wire clock may be sped up before
// issuing data to the display, then restored to the default rate afterward
// so other I2C device types still work).  All of these are encapsulated
// in the TRANSACTION_* macros.

// Check first if Wire, then hardware SPI, then soft SPI:
#define SSD1306_TRANSACTION_START                           \
  if (ssd1306_i2c_fd01 > 0) {                                                                  \
    SSD1306_I2C_SETWIRECLOCK;                                       \
  }

#define SSD1306_TRANSACTION_END                             \
  if (ssd1306_i2c_fd01 > 0) {                                                                  \
    SSD1306_I2C_RESWIRECLOCK;                                       \
  }

// CONSTRUCTORS, DESTRUCTOR ------------------------------------------------

/*!
    @brief  Constructor for I2C-interfaced SSD1306 displays.
    @param  w
            Display width in pixels
    @param  h
            Display height in pixels
    @param  twi
            Pointer to an existing TwoWire instance (e.g. &Wire, the
            microcontroller's primary I2C bus).
    @param  rst_pin
            Reset pin (using Arduino pin numbering), or -1 if not used
            (some displays might be wired to share the microcontroller's
            reset pin).
    @param  clkDuring
            Speed (in Hz) for Wire transmissions in SSD1306 library calls.
            Defaults to 400000 (400 KHz), a known 'safe' value for most
            microcontrollers, and meets the SSD1306 datasheet spec.
            Some systems can operate I2C faster (800 KHz for ESP32, 1 MHz
            for many other 32-bit MCUs), and some (perhaps not all)
            SSD1306's can work with this -- so it's optionally be specified
            here and is not a default behavior. (Ignored if using pre-1.5.7
            Arduino software, which operates I2C at a fixed 100 KHz.)
    @param  clkAfter
            Speed (in Hz) for Wire transmissions following SSD1306 library
            calls. Defaults to 100000 (100 KHz), the default Arduino Wire
            speed. This is done rather than leaving it at the 'during' speed
            because other devices on the I2C bus might not be compatible
            with the faster rate. (Ignored if using pre-1.5.7 Arduino
            software, which operates I2C at a fixed 100 KHz.)
    @return yama_2_GFX_SSD1306 object.
    @note   Call the object's begin() function before use -- buffer
            allocation is performed there!
*/

// LOW-LEVEL UTILS ---------------------------------------------------------

// Issue single command to SSD1306, using I2C or hard/soft SPI as needed.
// Because command calls are often grouped, SPI transaction and selection
// must be started/ended in calling function for efficiency.
// This is a private function, not exposed (see ssd1306_command() instead).
void ssd1306_command1(uint8_t c) {
  if (ssd1306_i2c_fd01 > 0) { // I2C
    uint8_t buf[2];
    buf[0] = 0x00;    // Co = 0, D/C = 0
    buf[1] = c;
    ssd1306_i2c_write_blocking(ssd1306_i2c_fd01, ssd1306_i2caddr, buf, 2, false);
  }
}

// Issue list of commands to SSD1306, same rules as above re: transactions.
// This is a private function, not exposed.
void ssd1306_commandList(const uint8_t *c, uint8_t n) {
  if (ssd1306_i2c_fd01 > 0) { // I2C

    int n1, n2;
    uint8_t *m;
    const uint8_t *c1 = c;
    n1 = n / SSD1306_I2C_WIRE_MAX;
    n2 = n - (n1 * SSD1306_I2C_WIRE_MAX);
    m = ssd1306_i2c_buffer01;
    for (int i = 0; i < n1; i++) {
      *m = 0x00;
      memcpy(m + 1, c1, SSD1306_I2C_WIRE_MAX);
      ssd1306_i2c_write_blocking(ssd1306_i2c_fd01, ssd1306_i2caddr, m, SSD1306_I2C_WIRE_MAX + 1, false);
      c1 += SSD1306_I2C_WIRE_MAX;
    }
    if (n2 > 0) {
      *m = 0x00;
      memcpy(m + 1, c1, n2);
      ssd1306_i2c_write_blocking(ssd1306_i2c_fd01, ssd1306_i2caddr, m, n2 + 1, false);
    }

  }
}

// A public version of ssd1306_command1(), for existing user code that
// might rely on that function. This encapsulates the command transfer
// in a transaction start/end, similar to old library's handling of it.
/*!
    @brief  Issue a single low-level command directly to the SSD1306
            display, bypassing the library.
    @param  c
            Command to issue (0x00 to 0xFF, see datasheet).
    @return None (void).
*/
void ssd1306_command(uint8_t c) {
  SSD1306_TRANSACTION_START
  ssd1306_command1(c);
  SSD1306_TRANSACTION_END
}

// ALLOCATE & INIT DISPLAY -------------------------------------------------

/*!
    @brief  Allocate RAM for image buffer, initialize peripherals and pins.
    @param  vcs
            VCC selection. Pass SSD1306_SWITCHCAPVCC to generate the display
            voltage (step up) from the 3.3V source, or SSD1306_EXTERNALVCC
            otherwise. Most situations with Adafruit SSD1306 breakouts will
            want SSD1306_SWITCHCAPVCC.
    @param  addr
            I2C address of corresponding SSD1306 display (or pass 0 to use
            default of 0x3C for 128x32 display, 0x3D for all others).
            SPI displays (hardware or software) do not use addresses, but
            this argument is still required (pass 0 or any value really,
            it will simply be ignored). Default if unspecified is 0.
    @param  reset
            If true, and if the reset pin passed to the constructor is
            valid, a hard reset will be performed before initializing the
            display. If using multiple SSD1306 displays on the same bus, and
            if they all share the same reset pin, you should only pass true
            on the first display being initialized, false on all others,
            else the already-initialized displays would be reset. Default if
            unspecified is true.
    @param  periphBegin
            If true, and if a hardware peripheral is being used (I2C or SPI,
            but not software SPI), call that peripheral's begin() function,
            else (false) it has already been done in one's sketch code.
            Cases where false might be used include multiple displays or
            other devices sharing a common bus, or situations on some
            platforms where a nonstandard begin() function is available
            (e.g. a TwoWire interface on non-default pins, as can be done
            on the ESP8266 and perhaps others).
    @return true on successful allocation/init, false otherwise.
            Well-behaved code should check the return value before
            proceeding.
    @note   MUST call this function before any drawing or updates!
*/

void ssd1306_close() {

    if (ssd1306_buffer != NULL) {
        free(ssd1306_buffer);
    }
}
int ssd1306_oled_setup(int fd, uint8_t vcs, uint8_t addr) {

  ssd1306_width = 128;
  ssd1306_height = 64;
  ssd1306_init_width = ssd1306_width;
  ssd1306_init_height = ssd1306_height;
  ssd1306_rotation = 0;
  ssd1306_i2c_fd01 = fd;

  if ((!ssd1306_buffer) && !(ssd1306_buffer = (uint8_t *)malloc(ssd1306_init_width * ((ssd1306_init_height + 7) / 8))))
    return false;

  ssd1306_clearDisplay();

  ssd1306_vccstate = vcs;

  // Setup pin directions
  if (ssd1306_i2c_fd01 > 0) { // Using I2C
    // If I2C address is unspecified, use default
    // (0x3C for 32-pixel-tall displays, 0x3D for all others).
    ssd1306_i2caddr = addr ? addr : ((ssd1306_init_height == 32) ? 0x3C : 0x3D);
    // TwoWire begin() function might be already performed by the calling
    // function if it has unusual circumstances (e.g. TWI variants that
    // can accept different SDA/SCL pins, or if two SSD1306 instances
    // with different addresses -- only a single begin() is needed).

  }

  SSD1306_TRANSACTION_START

  // Init sequence
  static const uint8_t SSD1306_PROGMEM init1[] = {SSD1306_DISPLAYOFF,         // 0xAE
                                          SSD1306_SETDISPLAYCLOCKDIV, // 0xD5
                                          0x80, // the suggested ratio 0x80
                                          SSD1306_SETMULTIPLEX}; // 0xA8
  ssd1306_commandList(init1, sizeof(init1));
  ssd1306_command1(ssd1306_init_height - 1);

  static const uint8_t SSD1306_PROGMEM init2[] = {SSD1306_SETDISPLAYOFFSET, // 0xD3
                                          0x0,                      // no offset
                                          SSD1306_SETSTARTLINE | 0x0, // line #0
                                          SSD1306_CHARGEPUMP};        // 0x8D
  ssd1306_commandList(init2, sizeof(init2));

  ssd1306_command1((ssd1306_vccstate == SSD1306_EXTERNALVCC) ? 0x10 : 0x14);

  static const uint8_t SSD1306_PROGMEM init3[] = {SSD1306_MEMORYMODE, // 0x20
                                          0x00, // 0x0 act like ks0108
                                          SSD1306_SEGREMAP | 0x1,
                                          SSD1306_COMSCANDEC};
  ssd1306_commandList(init3, sizeof(init3));

  uint8_t comPins = 0x02;
  ssd1306_contrast = 0x8F;

  if ((ssd1306_init_width == 128) && (ssd1306_init_height == 32)) {
    comPins = 0x02;
    ssd1306_contrast = 0x8F;
  } else if ((ssd1306_init_width == 128) && (ssd1306_init_height == 64)) {
    comPins = 0x12;
    ssd1306_contrast = (ssd1306_vccstate == SSD1306_EXTERNALVCC) ? 0x9F : 0xCF;
  } else if ((ssd1306_init_width == 96) && (ssd1306_init_height == 16)) {
    comPins = 0x2; // ada x12
    ssd1306_contrast = (ssd1306_vccstate == SSD1306_EXTERNALVCC) ? 0x10 : 0xAF;
  } else {
    // Other screen varieties -- TBD
  }

  ssd1306_command1(SSD1306_SETCOMPINS);
  ssd1306_command1(comPins);
  ssd1306_command1(SSD1306_SETCONTRAST);
  ssd1306_command1(ssd1306_contrast);

  ssd1306_command1(SSD1306_SETPRECHARGE); // 0xd9
  ssd1306_command1((ssd1306_vccstate == SSD1306_EXTERNALVCC) ? 0x22 : 0xF1);
  static const uint8_t SSD1306_PROGMEM init5[] = {
      SSD1306_SETVCOMDETECT, // 0xDB
      0x40,
      SSD1306_DISPLAYALLON_RESUME, // 0xA4
      SSD1306_NORMALDISPLAY,       // 0xA6
      SSD1306_DEACTIVATE_SCROLL,
      SSD1306_DISPLAYON}; // Main screen turn on
  ssd1306_commandList(init5, sizeof(init5));

  SSD1306_TRANSACTION_END

  return true; // Success
}

// DRAWING FUNCTIONS -------------------------------------------------------

/*!
    @brief  Set/clear/invert a single pixel. This is also invoked by the
            yama_2_GFX library in generating many higher-level graphics
            primitives.
    @param  x
            Column of display -- 0 at left to (screen width - 1) at right.
    @param  y
            Row of display -- 0 at top to (screen height -1) at bottom.
    @param  color
            Pixel color, one of: SSD1306_BLACK, SSD1306_WHITE or SSD1306_INVERT.
    @return None (void).
    @note   Changes buffer contents only, no immediate effect on display.
            Follow up with a call to ssd1306_display(), or with other graphics
            commands as needed by one's own application.
*/
void ssd1306_drawPixel(int16_t x, int16_t y, uint16_t color) {
  if ((x >= 0) && (x < ssd1306_width) && (y >= 0) && (y < ssd1306_height)) {
    // Pixel is in-bounds. Rotate coordinates if needed.
    switch (ssd1306_rotation) {
    case 1:
      SSD1306_swap(x, y);
      x = ssd1306_init_width - x - 1;
      break;
    case 2:
      x = ssd1306_init_width - x - 1;
      y = ssd1306_init_height - y - 1;
      break;
    case 3:
      SSD1306_swap(x, y);
      y = ssd1306_init_height - y - 1;
      break;
    }
    switch (color) {
    case SSD1306_WHITE:
      ssd1306_buffer[x + (y / 8) * ssd1306_init_width] |= (1 << (y & 7));
      break;
    case SSD1306_BLACK:
      ssd1306_buffer[x + (y / 8) * ssd1306_init_width] &= ~(1 << (y & 7));
      break;
    case SSD1306_INVERSE:
      ssd1306_buffer[x + (y / 8) * ssd1306_init_width] ^= (1 << (y & 7));
      break;
    }
  }
}

/*!
    @brief  Clear contents of display buffer (set all pixels to off).
    @return None (void).
    @note   Changes buffer contents only, no immediate effect on display.
            Follow up with a call to ssd1306_display(), or with other graphics
            commands as needed by one's own application.
*/
void ssd1306_clearDisplay(void) {
  memset(ssd1306_buffer, 0, ssd1306_init_width * ((ssd1306_init_height + 7) / 8));
}

/*!
    @brief  Draw a horizontal line. This is also invoked by the yama_2_GFX
            library in generating many higher-level graphics primitives.
    @param  x
            Leftmost column -- 0 at left to (screen width - 1) at right.
    @param  y
            Row of display -- 0 at top to (screen height -1) at bottom.
    @param  w
            Width of line, in pixels.
    @param  color
            Line color, one of: SSD1306_BLACK, SSD1306_WHITE or SSD1306_INVERT.
    @return None (void).
    @note   Changes buffer contents only, no immediate effect on display.
            Follow up with a call to ssd1306_display(), or with other graphics
            commands as needed by one's own application.
*/
void ssd1306_drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
  bool bSwap = false;
  switch (ssd1306_rotation) {
  case 1:
    // 90 degree rotation, swap x & y for rotation, then invert x
    bSwap = true;
    SSD1306_swap(x, y);
    x = ssd1306_init_width - x - 1;
    break;
  case 2:
    // 180 degree rotation, invert x and y, then shift y around for height.
    x = ssd1306_init_width - x - 1;
    y = ssd1306_init_height - y - 1;
    x -= (w - 1);
    break;
  case 3:
    // 270 degree rotation, swap x & y for rotation,
    // then invert y and adjust y for w (not to become h)
    bSwap = true;
    SSD1306_swap(x, y);
    y = ssd1306_init_height - y - 1;
    y -= (w - 1);
    break;
  }

  if (bSwap)
    ssd1306_drawFastVLineInternal(x, y, w, color);
  else
    ssd1306_drawFastHLineInternal(x, y, w, color);
}

void ssd1306_drawFastHLineInternal(int16_t x, int16_t y, int16_t w,
                                             uint16_t color) {

  if ((y >= 0) && (y < ssd1306_init_height)) { // Y coord in bounds?
    if (x < 0) {                  // Clip left
      w += x;
      x = 0;
    }
    if ((x + w) > ssd1306_init_width) { // Clip right
      w = (ssd1306_init_width - x);
    }
    if (w > 0) { // Proceed only if width is positive
      uint8_t *pBuf = &ssd1306_buffer[(y / 8) * ssd1306_init_width + x], mask = 1 << (y & 7);
      switch (color) {
      case SSD1306_WHITE:
        while (w--) {
          *pBuf++ |= mask;
        };
        break;
      case SSD1306_BLACK:
        mask = ~mask;
        while (w--) {
          *pBuf++ &= mask;
        };
        break;
      case SSD1306_INVERSE:
        while (w--) {
          *pBuf++ ^= mask;
        };
        break;
      }
    }
  }
}

/*!
    @brief  Draw a vertical line. This is also invoked by the yama_2_GFX
            library in generating many higher-level graphics primitives.
    @param  x
            Column of display -- 0 at left to (screen width -1) at right.
    @param  y
            Topmost row -- 0 at top to (screen height - 1) at bottom.
    @param  h
            Height of line, in pixels.
    @param  color
            Line color, one of: SSD1306_BLACK, SSD1306_WHITE or SSD1306_INVERT.
    @return None (void).
    @note   Changes buffer contents only, no immediate effect on display.
            Follow up with a call to ssd1306_display(), or with other graphics
            commands as needed by one's own application.
*/
void ssd1306_drawFastVLine(int16_t x, int16_t y, int16_t h,
                                     uint16_t color) {
  bool bSwap = false;
  switch (ssd1306_rotation) {
  case 1:
    // 90 degree rotation, swap x & y for rotation,
    // then invert x and adjust x for h (now to become w)
    bSwap = true;
    SSD1306_swap(x, y);
    x = ssd1306_init_width - x - 1;
    x -= (h - 1);
    break;
  case 2:
    // 180 degree rotation, invert x and y, then shift y around for height.
    x = ssd1306_init_width - x - 1;
    y = ssd1306_init_height - y - 1;
    y -= (h - 1);
    break;
  case 3:
    // 270 degree rotation, swap x & y for rotation, then invert y
    bSwap = true;
    SSD1306_swap(x, y);
    y = ssd1306_init_height - y - 1;
    break;
  }

  if (bSwap)
    ssd1306_drawFastHLineInternal(x, y, h, color);
  else
    ssd1306_drawFastVLineInternal(x, y, h, color);
}

void ssd1306_drawFastVLineInternal(int16_t x, int16_t __y,
                                             int16_t __h, uint16_t color) {

  if ((x >= 0) && (x < ssd1306_init_width)) { // X coord in bounds?
    if (__y < 0) {               // Clip top
      __h += __y;
      __y = 0;
    }
    if ((__y + __h) > ssd1306_init_height) { // Clip bottom
      __h = (ssd1306_init_height - __y);
    }
    if (__h > 0) { // Proceed only if height is now positive
      // this display doesn't need ints for coordinates,
      // use local byte registers for faster juggling
      uint8_t y = __y, h = __h;
      uint8_t *pBuf = &ssd1306_buffer[(y / 8) * ssd1306_init_width + x];

      // do the first partial byte, if necessary - this requires some masking
      uint8_t mod = (y & 7);
      if (mod) {
        // mask off the high n bits we want to set
        mod = 8 - mod;
        // note - lookup table results in a nearly 10% performance
        // improvement in fill* functions
        // uint8_t mask = ~(0xFF >> mod);
        static const uint8_t SSD1306_PROGMEM premask[8] = {0x00, 0x80, 0xC0, 0xE0,
                                                   0xF0, 0xF8, 0xFC, 0xFE};
        uint8_t mask = ssd1306_read_byte_data(&premask[mod]);
        // adjust the mask if we're not going to reach the end of this byte
        if (h < mod)
          mask &= (0XFF >> (mod - h));

        switch (color) {
        case SSD1306_WHITE:
          *pBuf |= mask;
          break;
        case SSD1306_BLACK:
          *pBuf &= ~mask;
          break;
        case SSD1306_INVERSE:
          *pBuf ^= mask;
          break;
        }
        pBuf += ssd1306_init_width;
      }

      if (h >= mod) { // More to go?
        h -= mod;
        // Write solid bytes while we can - effectively 8 rows at a time
        if (h >= 8) {
          if (color == SSD1306_INVERSE) {
            // separate copy of the code so we don't impact performance of
            // black/white write version with an extra comparison per loop
            do {
              *pBuf ^= 0xFF; // Invert byte
              pBuf += ssd1306_init_width; // Advance pointer 8 rows
              h -= 8;        // Subtract 8 rows from height
            } while (h >= 8);
          } else {
            // store a local value to work with
            uint8_t val = (color != SSD1306_BLACK) ? 255 : 0;
            do {
              *pBuf = val;   // Set byte
              pBuf += ssd1306_init_width; // Advance pointer 8 rows
              h -= 8;        // Subtract 8 rows from height
            } while (h >= 8);
          }
        }

        if (h) { // Do the final partial byte, if necessary
          mod = h & 7;
          // this time we want to mask the low bits of the byte,
          // vs the high bits we did above
          // uint8_t mask = (1 << mod) - 1;
          // note - lookup table results in a nearly 10% performance
          // improvement in fill* functions
          static const uint8_t SSD1306_PROGMEM postmask[8] = {0x00, 0x01, 0x03, 0x07,
                                                      0x0F, 0x1F, 0x3F, 0x7F};
          uint8_t mask = ssd1306_read_byte_data(&postmask[mod]);
          switch (color) {
          case SSD1306_WHITE:
            *pBuf |= mask;
            break;
          case SSD1306_BLACK:
            *pBuf &= ~mask;
            break;
          case SSD1306_INVERSE:
            *pBuf ^= mask;
            break;
          }
        }
      }
    } // endif positive height
  }   // endif x in bounds
}

/*!
    @brief  Return color of a single pixel in display buffer.
    @param  x
            Column of display -- 0 at left to (screen width - 1) at right.
    @param  y
            Row of display -- 0 at top to (screen height -1) at bottom.
    @return true if pixel is set (usually SSD1306_WHITE, unless display invert
   mode is enabled), false if clear (SSD1306_BLACK).
    @note   Reads from buffer contents; may not reflect current contents of
            screen if ssd1306_display() has not been called.
*/
bool ssd1306_getPixel(int16_t x, int16_t y) {
  if ((x >= 0) && (x < ssd1306_width) && (y >= 0) && (y < ssd1306_height)) {
    // Pixel is in-bounds. Rotate coordinates if needed.
    switch (ssd1306_rotation) {
    case 1:
      SSD1306_swap(x, y);
      x = ssd1306_init_width - x - 1;
      break;
    case 2:
      x = ssd1306_init_width - x - 1;
      y = ssd1306_init_height - y - 1;
      break;
    case 3:
      SSD1306_swap(x, y);
      y = ssd1306_init_height - y - 1;
      break;
    }
    return (ssd1306_buffer[x + (y / 8) * ssd1306_init_width] & (1 << (y & 7)));
  }
  return false; // Pixel out of bounds
}

/*!
    @brief  Get base address of display buffer for direct reading or writing.
    @return Pointer to an unsigned 8-bit array, column-major, columns padded
            to full byte boundary if needed.
*/
uint8_t *ssd1306_getBuffer(void) { return ssd1306_buffer; }

// REFRESH DISPLAY ---------------------------------------------------------

/*!
    @brief  Push data currently in RAM to SSD1306 display.
    @return None (void).
    @note   Drawing operations are not visible until this function is
            called. Call after each graphics command, or after a whole set
            of graphics commands, as best needed by one's own application.
*/
void ssd1306_display(void) {
  SSD1306_TRANSACTION_START
  static const uint8_t SSD1306_PROGMEM dlist1[] = {
      SSD1306_PAGEADDR,
      0,                      // Page start address
      0xFF,                   // Page end (not really, but works here)
      SSD1306_COLUMNADDR, 0}; // Column start address
  ssd1306_commandList(dlist1, sizeof(dlist1));
  ssd1306_command1(ssd1306_init_width - 1); // Column end address

  uint16_t count = ssd1306_init_width * ((ssd1306_init_height + 7) / 8);
  uint8_t *ptr = ssd1306_buffer;
  if (ssd1306_i2c_fd01 > 0) { // I2C
    int n1, n2;
    uint8_t *m;
    n1 = count / SSD1306_I2C_WIRE_MAX;
    n2 = count - (n1 * SSD1306_I2C_WIRE_MAX);
    m = ssd1306_i2c_buffer01;
    for (int i = 0; i < n1; i++) {
      *m = 0x40;
      memcpy(m + 1, ptr, SSD1306_I2C_WIRE_MAX);
      ssd1306_i2c_write_blocking(ssd1306_i2c_fd01, ssd1306_i2caddr, m, SSD1306_I2C_WIRE_MAX + 1, false);
      ptr += SSD1306_I2C_WIRE_MAX;
    }
    if (n2 > 0) {
      *m = 0x40;
      memcpy(m + 1, ptr, n2);
      ssd1306_i2c_write_blocking(ssd1306_i2c_fd01, ssd1306_i2caddr, m, n2 + 1, false);
    }

  }
  SSD1306_TRANSACTION_END
}

// SCROLLING FUNCTIONS -----------------------------------------------------

/*!
    @brief  Activate a right-handed scroll for all or part of the display.
    @param  start
            First row.
    @param  stop
            Last row.
    @return None (void).
*/
// To scroll the whole display, run: display.ssd1306_startscrollright(0x00, 0x0F)
void ssd1306_startscrollright(uint8_t start, uint8_t stop) {
  SSD1306_TRANSACTION_START
  static const uint8_t SSD1306_PROGMEM scrollList1a[] = {
      SSD1306_RIGHT_HORIZONTAL_SCROLL, 0X00};
  ssd1306_commandList(scrollList1a, sizeof(scrollList1a));
  ssd1306_command1(start);
  ssd1306_command1(0X00);
  ssd1306_command1(stop);
  static const uint8_t SSD1306_PROGMEM scrollList1b[] = {0X00, 0XFF,
                                                 SSD1306_ACTIVATE_SCROLL};
  ssd1306_commandList(scrollList1b, sizeof(scrollList1b));
  SSD1306_TRANSACTION_END
}

/*!
    @brief  Activate a left-handed scroll for all or part of the display.
    @param  start
            First row.
    @param  stop
            Last row.
    @return None (void).
*/
// To scroll the whole display, run: display.ssd1306_startscrollleft(0x00, 0x0F)
void ssd1306_startscrollleft(uint8_t start, uint8_t stop) {
  SSD1306_TRANSACTION_START
  static const uint8_t SSD1306_PROGMEM scrollList2a[] = {SSD1306_LEFT_HORIZONTAL_SCROLL,
                                                 0X00};
  ssd1306_commandList(scrollList2a, sizeof(scrollList2a));
  ssd1306_command1(start);
  ssd1306_command1(0X00);
  ssd1306_command1(stop);
  static const uint8_t SSD1306_PROGMEM scrollList2b[] = {0X00, 0XFF,
                                                 SSD1306_ACTIVATE_SCROLL};
  ssd1306_commandList(scrollList2b, sizeof(scrollList2b));
  SSD1306_TRANSACTION_END
}

/*!
    @brief  Activate a diagonal scroll for all or part of the display.
    @param  start
            First row.
    @param  stop
            Last row.
    @return None (void).
*/
// display.ssd1306_startscrolldiagright(0x00, 0x0F)
void ssd1306_startscrolldiagright(uint8_t start, uint8_t stop) {
  SSD1306_TRANSACTION_START
  static const uint8_t SSD1306_PROGMEM scrollList3a[] = {
      SSD1306_SET_VERTICAL_SCROLL_AREA, 0X00};
  ssd1306_commandList(scrollList3a, sizeof(scrollList3a));
  ssd1306_command1(ssd1306_init_height);
  static const uint8_t SSD1306_PROGMEM scrollList3b[] = {
      SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL, 0X00};
  ssd1306_commandList(scrollList3b, sizeof(scrollList3b));
  ssd1306_command1(start);
  ssd1306_command1(0X00);
  ssd1306_command1(stop);
  static const uint8_t SSD1306_PROGMEM scrollList3c[] = {0X01, SSD1306_ACTIVATE_SCROLL};
  ssd1306_commandList(scrollList3c, sizeof(scrollList3c));
  SSD1306_TRANSACTION_END
}

/*!
    @brief  Activate alternate diagonal scroll for all or part of the display.
    @param  start
            First row.
    @param  stop
            Last row.
    @return None (void).
*/
// To scroll the whole display, run: display.ssd1306_startscrolldiagleft(0x00, 0x0F)
void ssd1306_startscrolldiagleft(uint8_t start, uint8_t stop) {
  SSD1306_TRANSACTION_START
  static const uint8_t SSD1306_PROGMEM scrollList4a[] = {
      SSD1306_SET_VERTICAL_SCROLL_AREA, 0X00};
  ssd1306_commandList(scrollList4a, sizeof(scrollList4a));
  ssd1306_command1(ssd1306_init_height);
  static const uint8_t SSD1306_PROGMEM scrollList4b[] = {
      SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL, 0X00};
  ssd1306_commandList(scrollList4b, sizeof(scrollList4b));
  ssd1306_command1(start);
  ssd1306_command1(0X00);
  ssd1306_command1(stop);
  static const uint8_t SSD1306_PROGMEM scrollList4c[] = {0X01, SSD1306_ACTIVATE_SCROLL};
  ssd1306_commandList(scrollList4c, sizeof(scrollList4c));
  SSD1306_TRANSACTION_END
}

/*!
    @brief  Cease a previously-begun scrolling action.
    @return None (void).
*/
void ssd1306_stopscroll(void) {
  SSD1306_TRANSACTION_START
  ssd1306_command1(SSD1306_DEACTIVATE_SCROLL);
  SSD1306_TRANSACTION_END
}

// OTHER HARDWARE SETTINGS -------------------------------------------------

/*!
    @brief  Enable or disable display invert mode (white-on-black vs
            black-on-white).
    @param  i
            If true, switch to invert mode (black-on-white), else normal
            mode (white-on-black).
    @return None (void).
    @note   This has an immediate effect on the display, no need to call the
            ssd1306_display() function -- buffer contents are not changed, rather a
            different pixel mode of the display hardware is used. When
            enabled, drawing SSD1306_BLACK (value 0) pixels will actually draw
   white, SSD1306_WHITE (value 1) will draw black.
*/
void ssd1306_invertDisplay(int i) {
  SSD1306_TRANSACTION_START
  ssd1306_command1(i ? SSD1306_INVERTDISPLAY : SSD1306_NORMALDISPLAY);
  SSD1306_TRANSACTION_END
}

/*!
    @brief  Dim the display.
    @param  dim
            true to enable lower brightness mode, false for full brightness.
    @return None (void).
    @note   This has an immediate effect on the display, no need to call the
            ssd1306_display() function -- buffer contents are not changed.
*/
void ssd1306_dim(int dim) {
  // the range of contrast to too small to be really useful
  // it is useful to dim the display
  SSD1306_TRANSACTION_START
  ssd1306_command1(SSD1306_SETCONTRAST);
  ssd1306_command1(dim ? 0 : ssd1306_contrast);
  SSD1306_TRANSACTION_END
}

void ssd1306_i2c_write_byte_non_register(int addr, uint8_t val) {
    ssd1306_i2c_write_blocking(ssd1306_i2c_fd01, addr, &val, 1, false);
}

void ssd1306_setContrast(uint8_t _contrast) {
  ssd1306_contrast = _contrast;
  ssd1306_command1(SSD1306_SETCONTRAST);
  ssd1306_command1(ssd1306_contrast);
}

void ssd1306_i2c_write_blocking(int fd, uint8_t i2c_address, uint8_t *data, int16_t len, int flag) {
    i2c_write_nonreg(fd, i2c_address, data, len);
}

int32_t ssd1306_abs(int32_t num) {
    if (num < 0) num = -num;
    return num;
}

void ssd1306_swap_int32_t(int32_t *a, int32_t *b) {
    int32_t t;
    t = *a;
    *a = *b;
    *b = t;
}

void ssd1306_set_rotation(int32_t x) {
  ssd1306_rotation = (x & 3);
  switch (ssd1306_rotation) {
  case 0:
  case 2:
    ssd1306_width = ssd1306_init_width;
    ssd1306_height = ssd1306_init_height;
    break;
  case 1:
  case 3:
    ssd1306_width = ssd1306_init_height;
    ssd1306_height = ssd1306_init_width;
    break;
  }
}

void ssd1306_drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint32_t color) {
  int32_t steep = ssd1306_abs(y1 - y0) > ssd1306_abs(x1 - x0);
  if (steep) {
    ssd1306_swap_int32_t(&x0, &y0);
    ssd1306_swap_int32_t(&x1, &y1);
  }

  if (x0 > x1) {
    ssd1306_swap_int32_t(&x0, &x1);
    ssd1306_swap_int32_t(&y0, &y1);
  }

  int32_t dx, dy;
  dx = x1 - x0;
  dy = ssd1306_abs(y1 - y0);

  int32_t err = dx / 2;
  int32_t ystep;

  if (y0 < y1) {
    ystep = 1;
  } else {
    ystep = -1;
  }

  for (; x0 <= x1; x0++) {
    if (steep) {
      ssd1306_drawPixel(y0, x0, color);
    } else {
      ssd1306_drawPixel(x0, y0, color);
    }
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}


void ssd1306_fillCircleHelper(int32_t x0, int32_t y0, int32_t r,
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
        ssd1306_drawFastVLine(x0 + x, y0 - y, 2 * y + delta, color);
      if (corners & 2)
        ssd1306_drawFastVLine(x0 - x, y0 - y, 2 * y + delta, color);
    }
    if (y != py) {
      if (corners & 1)
        ssd1306_drawFastVLine(x0 + py, y0 - px, 2 * px + delta, color);
      if (corners & 2)
        ssd1306_drawFastVLine(x0 - py, y0 - px, 2 * px + delta, color);
      py = y;
    }
    px = x;
  }
}

void ssd1306_fillCircle(int32_t x0, int32_t y0, int32_t r, uint32_t color) {
    ssd1306_drawFastVLine(x0, y0 - r, 2 * r + 1, color);
    ssd1306_fillCircleHelper(x0, y0, r, 3, 0, color);
}

void ssd1306_drawRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color) {
  ssd1306_drawFastHLine(x, y, w, color);
  ssd1306_drawFastHLine(x, y + h - 1, w, color);
  ssd1306_drawFastVLine(x, y, h, color);
  ssd1306_drawFastVLine(x + w - 1, y, h, color);
}

void ssd1306_fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color)
{
    if(w > 0 && h > 0){
        if((x >= ssd1306_width) || (y >= ssd1306_height)) return;
        if((x + w - 1) >= ssd1306_width)  w = ssd1306_width  - x;
        if((y + h - 1) >= ssd1306_height) h = ssd1306_height - y;

    for (int j = y; j < (y + h); j++) {
            for (int i = x; i < (x + w); i++) {
                ssd1306_drawPixel(i, j, color);
        }
    }
    }
}

/* bitmapデータは1bitモノクロのビットマップデータ(MSBから開始) */
void ssd1306_drawBitmap(int32_t x, int32_t y, const uint8_t bitmap[], int32_t w, int32_t h) {
    int32_t i, j;
    const uint8_t *p = (uint8_t *)bitmap;
    uint16_t pixel01;

    int32_t index = 0;
    int32_t div   = 0;
    int32_t rem   = 0;
    int32_t color = 0;
    for(j=0; j < h; j++){
        for(i = 0; i < w; i++) {
            index = j * w + i;
            div = index / 8;
            rem = index % 8;
            color = (p[div] & (1 << (7 - rem))) ? SSD1306_WHITE : SSD1306_BLACK;
            ssd1306_drawPixel(x + i, y + j, color);
        }
    }
}

/* bitmapデータは1bitモノクロのビットマップデータ(MSBから開始) */
/* maskデータは1bitモノクロのビットマップデータ(MSBから開始)   */
void ssd1306_drawBitmap_mask(int32_t x, int32_t y, const uint8_t *bitmap,
                                 const uint8_t *mask, int32_t w, int32_t h) {
  int32_t index = 0;
  int32_t div   = 0;
  int32_t rem   = 0;
  int32_t color = 0;
  for (int32_t j = 0; j < h; j++) {
    for (int32_t i = 0; i < w; i++) {
      index = j * w + i;
      div = index / 8;
      rem = index % 8;
      if (mask[div] & (1 << (7 - rem))) {
        color = (bitmap[div] & (1 << (7 - rem))) ? SSD1306_WHITE : SSD1306_BLACK;
        ssd1306_drawPixel(x + i, y + j, color);
      }
    }
  }
}

