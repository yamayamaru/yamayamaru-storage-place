#!/bin/bash

gcc -O2 -c -o picojpeg.o picojpeg.c
g++ -O2 -c -o JPEGDecoder.o JPEGDecoder.cpp
