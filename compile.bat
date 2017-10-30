@echo off
mkdir obj
mkdir bin
g++ -W -Wall -c -o obj/Cigma16_assembler.o Cigma16_assembler.cpp
g++ -W -Wall -c -o obj/assembler.o assembler.cpp
g++ -W -Wall -c -o obj/assembler017.o assembler017.cpp
g++ -W -Wall -c -o obj/assembler144.o assembler144.cpp
g++ -W -Wall -c -o obj/emulator.o emulator.cpp
g++ -W -Wall -o bin/c16asm obj/*.o
del obj\Cigma16_assembler.o
g++ -W -Wall -c -o obj/Cigma16_emulator.o Cigma16_emulator.cpp
g++ -W -Wall -o bin/c16emu obj/*.o
del obj\*.o
echo Done