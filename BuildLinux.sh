#!/bin/bash

gcc -g -c libLDCC/GL/OES2/LD_OES2_GLFW3.c
gcc -g -c libLDCC/LD_C_Chrono.c
gcc -g -c libLDCC/LD_STL.c
gcc -g -c libLDCC/LD_Polygon_Extract_3D.c
gcc -g -c libLDCC/LD_Navi_2D.c
gcc -g -c libLDCC/LD_Math.c
gcc -g -c libLDCC/LD_Collision_2D.c

nasm -f ELF64 libLDCC/Boring-NASM-Code/x86-64/Math/NasmMath64.asm -o NasmMath.o

gcc -g main.c LD_Math.o LD_Collision_2D.o LD_OES2_GLFW3.o LD_C_Chrono.o LD_Navi_2D.o LD_STL.o LD_Polygon_Extract_3D.o NasmMath.o -lluajit-5.1 -lm -lglfw -lGL -lpthread -no-pie -o Phobos-Game

#The final executable will depend on GLFW3 shared object for it was dynamically linked
