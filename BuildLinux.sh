#!/bin/bash

mkdir tmp

ld  -r -b binary Shaders/DeferredLighting/VS_Gbuffer.glsl -o tmp/Gbuffer_VS.o
ld  -r -b binary Shaders/DeferredLighting/FS_Gbuffer.glsl -o tmp/Gbuffer_FS.o

gcc -g -c libLDCC/GL/OES2/LD_OES2_GLFW3.c   -o tmp/LD_OES2_GLFW3.o
gcc -g -c libLDCC/LD_C_Chrono.c		    -o tmp/LD_C_Chrono.o
gcc -g -c libLDCC/LD_STL.c		    -o tmp/LD_STL.o
gcc -g -c libLDCC/LD_Polygon_Extract_3D.c   -o tmp/LD_Polygon_Extract_3D.o
gcc -g -c libLDCC/LD_Navi_2D.c		    -o tmp/LD_Navi_2D.o
gcc -g -c libLDCC/LD_Math.c		    -o tmp/LD_Math.o
gcc -g -c libLDCC/LD_Collision_2D.c	    -o tmp/LD_Collision_2D.o

gcc -g -c main.c			    -o tmp/main.o

#nasm -f ELF64 libLDCC/Boring-NASM-Code/x86-64/Math/NasmMath64.asm -o NasmMath.o
nasm -f ELF64 -i libLDCC/Boring-NASM-Code/x86-64/Math/ libLDCC/Boring-NASM-Code/x86-64/Math/NasmGeometry64.asm -o tmp/NasmGeometry.o

cd tmp

gcc -g main.o NasmGeometry.o Gbuffer_VS.o LD_Math.o LD_Collision_2D.o LD_OES2_GLFW3.o LD_C_Chrono.o LD_Navi_2D.o LD_STL.o LD_Polygon_Extract_3D.o Gbuffer_FS.o  -lluajit-5.1 -lm -lglfw -lGL -lpthread -no-pie -o Phobos-Game

mv Phobos-Game ../Phobos-Game

#The final executable will depend on GLFW3 shared object for it was dynamically linked
