#!/bin/bash

cd JPEGDecoder_for_linux 
gcc -O2 -c -o picojpeg.o picojpeg.c
g++ -O2 -c -o JPEGDecoder.o JPEGDecoder.cpp
cd ..

g++ -O2 -c -o TFT000001_100.o TFT000001_100.cc
g++ -O2 -o framebuf_draw_mjpeg02 framebuf_draw_mjpeg02.cc TFT000001_100.o JPEGDecoder_for_linux/JPEGDecoder.o  JPEGDecoder_for_linux/picojpeg.o

