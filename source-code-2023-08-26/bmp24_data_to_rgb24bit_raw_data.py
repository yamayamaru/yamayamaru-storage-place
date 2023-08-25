### 参考サイト:https://www.tutimogura.com/python-bitmap-read/


import sys

args = sys.argv

### 入出力画像ファイルのオブジェクトを生成 ###
fname = args[1]
fname_save  = args[2]

f  = open(fname,"rb")

### BMPファイルヘッダ ###
bfType         = f.read(2)
bfSize         = f.read(4)
bfReserved1    = f.read(2)
bfReserved2    = f.read(2)
bfOffBitsbfOffBits = f.read(4)

### 情報ヘッダ ###
bcSize         = f.read(4)
bcWidth        = f.read(4)
bcHeight       = f.read(4)
bcPlanes       = f.read(2)
bcBitCount     = f.read(2)
biCompression  = f.read(4)
biSizeImage    = f.read(4)
biXPixPerMeter = f.read(4)
biYPixPerMeter = f.read(4)
biClrUsed      = f.read(4)
biCirImportant = f.read(4)

### 出力ファイルのヘッダ作成 ###
#fw.write(bfType            )
#fw.write(bfSize            )
#fw.write(bfReserved1       )
#fw.write(bfReserved2       )
#fw.write((54).to_bytes(4,"little"))
#fw.write(bcSize            )
#fw.write(bcWidth           )
#fw.write(bcHeight          )
#fw.write(bcPlanes          )
#fw.write(bcBitCount        )
#fw.write(biCompression     )
#fw.write(biSizeImage       )
#fw.write(biXPixPerMeter    )
#fw.write(biYPixPerMeter    )
#fw.write(biClrUsed         )
#fw.write(biCirImportant    )

### 処理に必要そうなデータはデータとして持っておく ###
bfType_str             = bfType.decode()
bfOffBitsbfOffBits_int = int.from_bytes(bfOffBitsbfOffBits, "little")
bcSize_int             = int.from_bytes(bcSize,             "little")
bcWidth_int            = int.from_bytes(bcWidth,            "little")
bcHeight_int           = int.from_bytes(bcHeight,           "little")
bcBitCount_int         = int.from_bytes(bcBitCount,         "little")
biCompression_int      = int.from_bytes(biCompression,      "little")

### 想定する画像フォーマットでない場合は、ここで処理を終了 ###
if (bfType_str!="BM") or \
   (bcSize_int!=40)   or \
   (bcBitCount_int!=24) or \
   (biCompression_int!=0):
  print ("### This file format is not supported! ###")
  print("bfType = {0:}".format(bfType_str))
  print("bcSize = {0:}".format(bcSize_int))
  print("bcBitCount = {0:}".format(bcBitCount_int))
  print("biCompression = {0:}".format(biCompression_int))

  print()
  print()
  print("Windows付属のペイントで一度読み込んでから書き出してください")
  sys.exit()

### 画像サイズ確認(デバッグ用) ###
print ("(Width,Height)=(%d,%d)" % (bcWidth_int,bcHeight_int))

### 画像データ本体へJump。ほどんど不要かも。###
offset = bfOffBitsbfOffBits_int-54
f.read(offset)

# データ本体の抽出
data = bytearray([])

for i in range(bcWidth_int * bcHeight_int * 3):
    data.append(int.from_bytes(f.read(1), 'little'))

f.close()



if bcHeight_int < 0:
    height = -bcHeight_int
    mflag = True
else:
    height = bcHeight_int
    mflag = False

# データが下から上の並びになっているので上から下にする(bcHeightがマイナスの場合は上から下)
data_raw = bytearray([])
for j in range(bcHeight_int):
    for i in range(bcWidth_int):
        if (mflag):
            a = j * bcWidth_int + i
        else :
            a = (height - 1 - j) * bcWidth_int + i

        r = data[a * 3 + 2]
        g = data[a * 3 + 1]
        b = data[a * 3 + 0]

        data_raw.append(r & 0xff)
        data_raw.append(g & 0xff)
        data_raw.append(b & 0xff)


# data_rawにbitmapデータがバイト列で格納されている(24bitRGB(各色8bitずつ、RGBの順番))

f  = open(fname_save,"wb")
f.write(data_raw)
f.close()
