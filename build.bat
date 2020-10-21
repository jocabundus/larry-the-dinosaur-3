set include=..\vendor\x64\include
set lib=..\vendor\x64\lib

if "%1"=="debug" goto :debug

:release
g++ -c entity.cpp
g++ -c winapp.cpp -I %include%
g++ -c ld3.cpp -I %include%
g++ -c ld3main.cpp -I %include%
g++ -c main.cpp
g++ -o ld3.exe entity.o winapp.o ld3.o ld3main.o main.o -L %lib% -l SDL2 -l SDL2_mixer

exit /B 0

:debug
g++ -g -c entity.cpp
g++ -g -c winapp.cpp -I %include%
g++ -g -c ld3.cpp -I %include%
g++ -g -c ld3main.cpp -I %include%
g++ -g -c main.cpp
g++ -g -o ld3.exe entity.o winapp.o ld3.o ld3main.o main.o -L %lib% -l SDL2 -l SDL2_mixer
