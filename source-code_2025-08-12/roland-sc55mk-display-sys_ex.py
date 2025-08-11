# テキストからSC-55mk2 ディスプレイ表示 SysEx生成プログラム

import sys

a01 = '        **      '
a02 = '       ***      '
a03 = '      * **      '
a04 = '     *  **      '
a05 = '        **      '
a06 = '        **      '
a07 = '        **      '
a08 = '        **      '
a09 = '        **      '
a10 = '        **      '
a11 = '        **      '
a12 = '        **      '
a13 = '        **      '
a14 = '        **      '
a15 = '     ********   '
a16 = '     ********   '



a01 = '      *****     '
a02 = '      *    *    '
a03 = '     *     *    '
a04 = '     *     *    '
a05 = '     *     *    '
a06 = '     *     *    '
a07 = '          **    '
a08 = '         **     '
a09 = '        **      '
a10 = '       **       '
a11 = '      **        '
a12 = '     **         '
a13 = '     *          '
a14 = '     *          '
a15 = '     *******    '
a16 = '     *******    '



a01 = '       ***      '
a02 = '      *  **     '
a03 = '     **    *    '
a04 = '     *     *    '
a05 = '     *     *    '
a06 = '          *     '
a07 = '       ***      '
a08 = '       ***      '
a09 = '          *     '
a10 = '          *     '
a11 = '           *    '
a12 = '     *     *    '
a13 = '     *     *    '
a14 = '     **   **    '
a15 = '      *  **     '
a16 = '       ***      '



dat01 = a01+a02+a03+a04+a05+a06+a07+a08
dat01 += a09+a10+a11+a12+a13+a14+a15+a16

width = 16
height = 16
datasize = width * height;


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
        if (dat01[i]!=' '):
            a01 |= 1 << (4-col01)
    if (col01 >= 5 and col01 < 10):
        if (dat01[i]!=' '):
            b01 |= 1 << (9-col01)
    if (col01 >= 10 and col01 < 15):
        if (dat01[i]!=' '):
            c01 |= 1 << (14-col01)
    if (col01 == 15):
        if (dat01[i]!=' '):
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
