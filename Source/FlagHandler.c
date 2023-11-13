#include "../Headers/InstructionDecoder.h"

#include <stdio.h>
#include <string.h>


char* HelpMessage = "\n\
Custom Assmbler or 'CASM', by Amir Siekierski,\n\
is a programme used for compilling assembly code to raw machine language.\n\
Its speciallity is that custom assembly language files are provided to the programme,\n\
thus, fulfilling the needs of programmers working on legacy platforms\n\
and for programmers poinearing new ones.\n\
\n\
Usage:\n\
  casm [options] <file>\n\
\n\
Compile file to raw binary file.\n\
\n\
Options:\n\
  -o, --output      	Set output file name.\n\
  -s, --size        	Set output file size in bytes.\n\
  -l, --language    	Provide a .lng file containing an assembly language.\n\
  -l, --help        	Display this help message.\n\
  -e, --little-endian	Set direction of word numbers to little endian.\n\
  -E, --big-endian		Set direction of word number to big endian.\n\n";

int HandleFlags(int argc, char** argv, unsigned int* OutputFileSize, char** InputFileName, char** OutputFileName, char** LanguageFileName, unsigned int* Direction) {
	*OutputFileSize = 0x10000;
	*InputFileName = NULL;
	*OutputFileName = "Program.bin";
	*LanguageFileName = "ASM.lng";
	*Direction = LITTLE_ENDIAN;

	for (int i = 1; i < argc; ++i) {
		if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0) {
			if (i + 1 >= argc) { fprintf(stderr, "\x1b[91mERROR:\x1b[0m No arguements for -o, --output flag provided.\n"); return 1; }
			*OutputFileName = argv[i + 1];
		} else if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--size") == 0) {
			if (i + 1 >= argc) { fprintf(stderr, "\x1b[91mERROR:\x1b[0m No arguements for -s, --size flag provided.\n"); return 1; }
			*OutputFileSize = DecodeNumber(argv[i + 1]);
		} else if (strcmp(argv[i], "-l") == 0 || strcmp(argv[i], "--language") == 0) {
			if (i + 1 >= argc) { fprintf(stderr, "\x1b[91mERROR:\x1b[0m No arguements for -l, --language flag provided.\n"); return 1; }
			*LanguageFileName = argv[i + 1];
		} else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
			printf("%s", HelpMessage);
			return 2;
		} else if (strcmp(argv[i], "-e") == 0 || strcmp(argv[i], "--little-endian") == 0) {
			*Direction = LITTLE_ENDIAN;
		} else if (strcmp(argv[i], "-E") == 0 || strcmp(argv[i], "--big-endian") == 0) {
			*Direction = BIG_ENDIAN;
		} else {
			*InputFileName = argv[i];
			continue;
		}
		i++;
	}

	if (*InputFileName == NULL) { fprintf(stderr, "\x1b[91mERROR:\x1b[0m No input file provided.\n"); return 1; }
	return 0;
}
