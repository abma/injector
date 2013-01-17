@echo off
del CMakeCache.txt
set PATH=%PATH%;C:\MinGW32\bin\;C:\Programme\CMake 2.8\bin\;C:\Programme\Ninja
cmake.exe -G Ninja .
