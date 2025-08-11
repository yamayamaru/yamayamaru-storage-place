# 24bit bitmap画像からSC-55mk2 ディスプレイ表示 SysEx生成プログラム
#
# require Pillow
# python -m pip install Pillow
#

import sys
from PIL import Image
import os




image01 = Image.open(os.getcwd() + '\\test0001.png')





width = image01.size[0]
height = image01.size[1]

datasize = width * height

a = []
b = []
c = []
d = []

for i in range(datasize):
    row01 = i // width
    col01 = i % width
    if (col01 == 0):
        a01 = 0
        b01 = 0
        c01 = 0
        d01 = 0
    if (col01 < 5):
        pix01 = image01.getpixel((col01, row01))
        aa = 0
        for i in pix01:
            aa |= i
        if (aa != 0):
            a01 |= 1 << (4-col01)
    if (col01 >= 5 and col01 < 10):
        pix01 = image01.getpixel((col01, row01))
        aa = 0
        for i in pix01:
            aa |= i
        if (aa != 0):
            b01 |= 1 << (9-col01)
    if (col01 >= 10 and col01 < 15):
        pix01 = image01.getpixel((col01, row01))
        aa = 0
        for i in pix01:
            aa |= i
        if (aa != 0):
            c01 |= 1 << (14-col01)
    if (col01 == 15):
        pix01 = image01.getpixel((col01, row01))
        aa = 0
        for i in pix01:
            aa |= i
        if (aa != 0):
            c01 |= 1 << 4
        a.append(a01)
        b.append(b01)
        c.append(c01)
        d.append(d01)


a += b
a += c
a += d

# 先頭データ出力
sys.stdout.write('F0 41 10 45 12 10 01 00 ')

# データ部分出力
sum01 = 0
for i in range(len(a)):
    sys.stdout.write('{:02x} '.format(a[i]))
    sum01 += a[i]

# チェックサム出力
sys.stdout.write('{:02x} '.format(128 - ((0x11+sum01) & 0x7f)))

# SysEx終了コード
print('F7')
