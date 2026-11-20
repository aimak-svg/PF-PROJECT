#!/bin/bash
gcc main.c -o arena_duel.exe -lraylib -lopengl32 -lgdi32 -lwinmm

if [ $? -eq 0 ]; then
    ./arena_duel.exe
fi