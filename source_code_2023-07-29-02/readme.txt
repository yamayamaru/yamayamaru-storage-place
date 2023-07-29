LicheePi4AでSPIでILI9341のLCDを使ってマンデルブロ集合を描くプログラム

LicheePi4AでもSPIが使えます

このプログラムではlibgpiodv2.0が必要です
下記を参照してビルド＆インストールしてください

https://github.com/yamayamaru/yamayamaru-storage-place/tree/main/source_code_2023-07-29-01



LCDの接続は

IO1-3がCS
IO1-4がDC
IO1-5がReset

SPI-CKがSCK
SPI-SIがMOSI
SPI-SOがMISO

SPI-SIがMOSIなので間違えないようにしてください


コンパイル方法は下記になります

gcc -O2 -I//home/sipeed/mylib_local/include -L/home/sipeed/mylib_local/lib -o test_spitest_ili9341_mandel03_02 test_spitest_ili9341_mandel03_02.c -lgpiod



実行するためのスクリプトはtest_spitest_ili9341_mandel03_02-exec.shという名前で作成します(任意の名前でかまいません）

#!/bin/bash


export LD_LIBRARY_PATH=/home/sipeed/mylib_local/lib:$LD_LIBRARY_PATH
export PATH=/home/sipeed/mylib_local/bin:$PATH
sudo chmod o+rw /dev/gpiochip4
sudo chmod o+rw /dev/spidev2.0


./test_spitest_ili9341_mandel03_02




スクリプトに実行権付与

chmod u+x test_spitest_ili9341_mandel03_02-exec.sh




実行
./test_spitest_ili9341_mandel03_02-exec.sh
