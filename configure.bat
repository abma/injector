@echo off
del CMakeCache.txt
set PATH=%PATH%;C:\MinGW32\bin\;%ProgramFiles%\CMake 2.8\bin\;%ProgramFiles%\Ninja;%ProgramFiles%\Git\bin
cmake.exe -G Ninja .
