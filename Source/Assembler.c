#include "../Headers/FileReader.h"
#include "../Headers/InstructionDecoder.h"
#include "../Headers/FlagHandler.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv) {
	char* InputFile;
	char* OutputFile;
	char* LanguageFile;
	unsigned int OutputSize;
	unsigned int Direction;

	switch (HandleFlags(argc, argv, &OutputSize, &InputFile, &OutputFile, &LanguageFile, &Direction)) {
		case 1:
			return 1;
		case 2:
			return 0;
	}

	int WordCount;
	int LangWordCount;
	
	char** ASMLang = LoadASMLang(LanguageFile, &LangWordCount);
	if (ASMLang == NULL) { fprintf(stderr, "\x1b[91mERROR:\x1b[0m Language file was not found or could not be read.\n"); return 1; }

	char** Tokens = ReadFile(InputFile, &WordCount);
	if (Tokens == NULL) { fprintf(stderr, "\x1b[91mERROR:\x1b[0m Input file was not found or could not be read.\n"); return 1; }

	Byte* Output = DecodeInstructions(ASMLang, LangWordCount, Tokens, WordCount, OutputSize, Direction);
	if (Output == NULL) { fprintf(stderr, "\x1b[91mERROR:\x1b[0m Could not generate output data.\n"); return 1; }
	
	FILE* File = fopen(OutputFile, "wb");
	if (File == NULL) { fprintf(stderr, "\x1b[91mERROR:\x1b[0m Could not open output file.\n"); return 1; }

	fwrite(Output, 1, OutputSize, File);
	fclose(File);

	DestroyFile(ASMLang, LangWordCount);
	DestroyFile(Tokens, WordCount);
	free((void*)ASMLang);
	free((void*)Tokens);

	return 0;
}