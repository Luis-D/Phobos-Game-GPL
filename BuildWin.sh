#!/bin/bash

mkdir tmp

ld  -r -b binary Shaders/DeferredLighting/VS_Gbuffer.glsl -o tmp/Gbuffer_VS.o
ld  -r -b binary Shaders/DeferredLighting/FS_Gbuffer.glsl -o tmp/Gbuffer_FS.o

nasm -f win64 -i libLDCC/Boring-NASM-Code/x86-64/Math/ libLDCC/Boring-NASM-Code/x86-64/Math/NasmGeometry64.asm -o tmp/NasmGeometry.o

x86_64-w64-mingw32-gcc-7.3-posix -c libLDCC/GL/DESKTOP/LD_OES2_FALLBACK_GLFW3.c 	-o tmp/LD_OES2_FALLBACK_GLFW3.o
x86_64-w64-mingw32-gcc-7.3-posix -c libLDCC/Time/LD_C_Chrono.c						-o tmp/LD_C_Chrono.o
x86_64-w64-mingw32-gcc-7.3-posix -c libLDCC/LD_STL.c		        						-o tmp/LD_STL.o
x86_64-w64-mingw32-gcc-7.3-posix -c libLDCC/LD_Polygon_Extract_3D.c 					-o tmp/LD_Polygon_Extract_3D.o
x86_64-w64-mingw32-gcc-7.3-posix -c libLDCC/LD_Navi_2D.c		    						-o tmp/LD_Navi_2D.o
x86_64-w64-mingw32-gcc-7.3-posix -c libLDCC/LD_Collision_2D.c	    						-o tmp/LD_Collision_2D.o
x86_64-w64-mingw32-gcc-7.3-posix -c libLDCC/Extern/LD_Math.c							-o tmp/LD_Math.o
x86_64-w64-mingw32-gcc-7.3-posix -c main.c			                						-o tmp/main.o

cd tmp

x86_64-w64-mingw32-gcc-7.3-posix -s Gbuffer_FS.o main.o LD_Math.o LD_Collision_2D.o LD_OES2_FALLBACK_GLFW3.o LD_C_Chrono.o LD_Navi_2D.o LD_STL.o LD_Polygon_Extract_3D.o  Gbuffer_VS.o NasmGeometry.o  ../Extern/libglfw3.a ../lua51.dll -static-libgcc -static-libstdc++ -lopengl32 -lgdi32 -lkernel32 -static -lpthread -o Phobos-Game.exe

mv Phobos-Game.exe ../Phobos-Game.exe