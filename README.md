# Cigma16
A graphical assembler and emulator for the Sigma16 architecture created by Dr. John O'Donnell

The program supports both Sigma16 0.1.7 and 1.4.4 instructions. Minor incompatibilities are expected.

Since it's just a hobby project of mine, the source is released under an MIT license.

Dr. John O'Donnell's home page - http://www.dcs.gla.ac.uk/~jtod/

## How to compile Cigma16 GUI
Open the project in Qt Creator 5.4 and simply compile it

## How to compile Cigma16 CUI
Simply run compile.bat (can be easily adapted into a .sh script) and it produces two executables:

- c16asm - converts the source code into machine code
- c16emu - runs the machine code

If both programs are ran without any arguments, they will provide information on what arguments have to be provided
