#!/bin/bash

#export Z88DK_HOME= #ここにZ88DKのHOMEのパスを指定する
export ZCCCFG=${Z88DK_HOME}/lib/config
export PATH=${Z88DK_HOME}/bin:${PATH}

zcc +msx -DNODELAY -lm -subtype=msxdos mandelc.c msxclib.c msxalib.c -o mandelc.com
