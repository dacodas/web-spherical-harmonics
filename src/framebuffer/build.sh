#!/bin/bash

g++ -ggdb -I LearnOpenGL/configuration -I LearnOpenGL/includes/ LearnOpenGL/src/glad.c LearnOpenGL/src/stb_image.cpp main.cpp -lpng -lglfw -ldl
