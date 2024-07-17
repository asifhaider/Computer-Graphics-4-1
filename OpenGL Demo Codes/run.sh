#!/bin/bash
# This script is used to run the OpenGL programs

filename=$1
g++ -o temp $filename -lglut -lGLU -lGL
./temp