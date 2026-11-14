ubuntu 24.04でのclang 18用newlib-i386のビルドと自作プログラムのビルド方法

$ sudo apt install build-essential clang lld-18

newlib-i386のビルド手順

$ wget ftp://sourceware.org/pub/newlib/newlib-4.4.0.20231231.tar.gz

$ tar -xvf newlib-4.4.0.20231231.tar.gz
$ mkdir newlib-build
$ cd newlib-build


$ ../newlib-4.4.0.20231231/configure --target=i386-none-elf --disable-newlib-supplied-syscalls --disable-libgloss \
    CC_FOR_TARGET="clang -march=i386 -target i386-none-elf -m32 -m80387 -ffreestanding -Wno-error=implicit-function-declaration" \
    GCC_FOR_TARGET="clang -march=i386 -target i386-none-elf -m32 -m80387 -ffreestanding -Wno-error=implicit-function-declaration" \
    AS_FOR_TARGET="llvm-as-18" \
    AR_FOR_TARGET="llvm-ar-18" \
    LD_FOR_TARGET="lld -m elf_i386" \
    RANLIB_FOR_TARGET="llvm-ranlib-18" \
    NM_FOR_TARGET="llvm-nm-18" \
    STRIP_FOR_TARGET="llvm-strip-18" \
    CXX="clang++-18 -march=i386 -target i386-none-elf -m32 -m80387 -ffreestanding -Wno-error=implicit-function-declaration"


make

ビルドに成功するとnewlib-build/i386-none-elf/newlibにlibc.a、libm.a、libg.aが作成されます





binutilsのビルド(ldやobjcopyでa.out-i386形式を使えるようにbinutilsを自前でビルドする必要があります)

$ sudo apt install build-essential bison flex

$ wget https://ftp.gnu.org/gnu/binutils/binutils-2.43.1.tar.gz

$ tar zxvf binutils-2.43.1.tar.gz
$ cd binutils-2.43.1

$ ./configure --enable-targets=all --prefix=/home/ubuntu/2.43.1

$ make
$ make install

ここで--prefix=/home/ubuntu/2.43.1の部分は自分でインストールしたい場所を指定してください





プログラムのコンパイル方法
(アセンブラは標準のasを使います)
newlibのコンパイルでできたlibc.a、libm.a、libg.aをカレントディレクトリにコピーしておきます

ここで自作プログラムはtest01.cとしておきます

コンパイル
$ clang -O2 -m32 -march=i386 -m80387 -fno-pic -fno-pie -ffreestanding -I./newlib-4.4.0.20231231/newlib/libc/include -c test01.c -o test01.o
$ clang -O2 -m32 -march=i386 -m80387 -fno-pic -fno-pie -ffreestanding -I./newlib-4.4.0.20231231/newlib/libc/include -c newlib_stub.c -o stub.o
$ as --32  -o startup.o startup.s

ここで-I./newlib-4.4.0.20231231/newlib/libc/includeはインクルードファイルの指定をしてます。
newlib-4.4.0.20231231の場所によって変わるので自分の環境に合わせてください



リンク(自分でビルドしたbinutilsのldを使う)
$ /home/ubuntu/2.43.1/usr/bin/ld -N -m elf_i386 -static -nostdlib  -T aout.ld \
    -o test01.elf \
    startup.o stub.o \
    test01.o \
    libc.a libm.a \
    -Map test01.map


test01.binをa.out-i386形式に変換(自分でビルドしたbinutilsのobjcopyを使う)
$ /home/ubuntu/2.43.1usr/bin/objcopy -S -O a.out-i386 test01.elf test01.bin

逆アセンブルファイルの生成(自分でビルドしたbinutilsのobjdumpを使う)
$ /home/ubuntu/2.43.1usr/bin/objdump -b a.out-i386 -d test01.elf > test01.elf.txt



このtest01.binをFM TOWNSエミュレータ津軽に持っていき、TOWNS-gccのGENEXP3.EXPを実行します
GENEXP3.EXPのみ使えればいいのでTOWNS-gccを使えるようにインストールする必要はありません

run386 -nocrt f:\tgcc\usr\bin\genexp3 test01.bin test01.exp

(f:\tgcc\usr\binはgenexp3.expへのパスです。ご自分の環境に合わせて変更してください)

test01.cはTOWNS OSのコマンドラインから実行するために作られたものなので
TOWNSメニューから実行したい場合はegbフォルダにあるhat05.cを試してください


TOWNS-gcc
https://anikun.kutami.jp/towns-gcc/environment.php
GENEXP3.EXPは下記URLのLZHファイルの中のBINBIN.LZHの中にGENEXP3.EXPがあります
https://anikun.kutami.jp/towns-gcc/gcc/tgp245_1.lzh

注意

FM TOWNSエミュレータ津軽での実行にあたってclangで作成したプログラムはFPUをオンにしないと実行できません。
また、FM TOWNS用のMSDOS Ver3.1とTOWNS OSからunzのところのmkosromで生成した
FMT_DOS.ROMで津軽用の互換ROMのFMT_DOS.ROMを上書きしないと実行できないかもしれません

FM TOWNS用のMSDOS Ver3.1とTOWNS OSからunzのところのmkosromで生成したFMT_DOS.ROMを使うとHDDイメージからしか起動できないので
別途TownsシステムソフトウェアのディスクからTOSNS OSをHDDイメージにインストールする必要があります
なのでTownsシステムソフトウェアが必要になります



これらのプログラムの使用に当たってはご自分の責任において使用してください
これらのプログラムで発生したいかなる損害、データの消失、金銭等の責任は一切負いません。
