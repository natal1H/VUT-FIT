cmake_minimum_required(VERSION 3.12)

#Nazov projektu
project(IFJ-project_VUTFIT C)

#Prepinace
add_compile_options(-Wall -pedantic -lm)

#Standard
set(CMAKE_C_STANDARD 99)

#Prirad vsetky subory ktore sa nachadzaju v zlozke projektu
file(GLOB IFJ-project_VUTFIT_SOURCES
        "*.h"
        "*.c"
        )

#Vytvor spustitelny subor
add_executable(IFJ-project_VUTFIT ${IFJ-project_VUTFIT_SOURCES})
