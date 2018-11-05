#!/bin/bash

nasm -fwin64  libLDCC/Boring-NASM-Code/x86-64/Math/NasmMath64.asm -o NasmMath.o

x86_64-w64-mingw32-gcc-7.3-posix -c libLDCC/GL/DESKTOP/LD_OES2_FALLBACK_GLFW3.c
x86_64-w64-mingw32-gcc-7.3-posix -c libLDCC/LD_C_Chrono.c
x86_64-w64-mingw32-gcc-7.3-posix -c libLDCC/LD_STL.c
x86_64-w64-mingw32-gcc-7.3-posix -c libLDCC/LD_Polygon_Extract_3D.c
x86_64-w64-mingw32-gcc-7.3-posix -c libLDCC/LD_Navi_2D.c
x86_64-w64-mingw32-gcc-7.3-posix -c libLDCC/LD_Math.c
x86_64-w64-mingw32-gcc-7.3-posix -c libLDCC/LD_Collision_2D.c

x86_64-w64-mingw32-gcc-7.3-posix -s main.c LD_Math.o LD_Collision_2D.o LD_OES2_FALLBACK_GLFW3.o LD_C_Chrono.o LD_Navi_2D.o LD_STL.o LD_Polygon_Extract_3D.o NasmMath.o  libglfw3.a -static-libgcc -static-libstdc++ -lopengl32 -lgdi32 -lkernel32 -static -lpthread -o Phobos-Game.exe

mkdir Win
mv Phobos-Game.exe Win/Phobos-Game.exe