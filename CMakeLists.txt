cmake_minimum_required(VERSION 3.27)
project(SAMPLETOYC C)

set(CMAKE_C_STANDARD 23)

add_library(SAMPLETOYC STATIC library.c
        AudioSynthesis.c
        AudioSynthesis.h
        main.c
        Tests.c
)
