CC = gcc
Flags = -O2
OutputName = casm
Output = -o $(OutputName)
Input = ../Source/*.c
Include = -I ../Headers/
Debugger = gdb
Example = ../Example/

all:
	$(CC) $(Input) $(Include) $(Flags) $(Output)

clear:
	rm $(OutputName)

run:
	./$(OutputName) Program.asm

debug:
	$(CC) $(Input) $(Include) $(Flags) -g $(Output)
	$(Debugger) --args $(OutputName) Program.asm