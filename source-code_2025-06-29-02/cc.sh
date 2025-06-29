#!/bin/bash

as -o pimil.o pimil.s
gcc -O2 -o pimil pimil_main.c pimil.o
