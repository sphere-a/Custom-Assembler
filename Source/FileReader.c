#include "../Headers/FileReader.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define SPACE 8192
#define WORD 0

int GetFileLength(FILE* File) {
	if (File == NULL) { return -1; }
	int Length = 0;
	while (getc(File) != EOF) { ++Length; }
	rewind(File);
	return Length;
}

int GetWordCount(char* Buffer, int Length) {
	if (Buffer == NULL) { return -1; }

	int i = 0;
	while (Buffer[i] == ';') { while (i < Length && Buffer[i] != '\n') { i++; } i++; }

	int State = isspace(Buffer[i]);
	int Prev = State;
	i++;

	int WordCount = 0;
	for (; i < Length; ++i) {
		State = isspace(Buffer[i]);
		if (Buffer[i] == ';') { while (i < Length && Buffer[i] != '\n') { i++; } State = SPACE; }
		else { if (State == SPACE && Prev == WORD) { WordCount++; } }
		Prev = State;
	}

	if (State == WORD) { WordCount++; }
	return WordCount;
}


char** ReadFile(const char* FileName, int* ResultWordCount) {
	FILE* File = fopen(FileName, "r");
	int Length = GetFileLength(File);
	char* Buffer = (char*)malloc(Length);
	if (File == NULL || Buffer == NULL) { return NULL; }

	{ int i = 0; int c = getc(File); while (c != EOF) { Buffer[i] = c; i++; c = getc(File); } }
	if (fclose(File)) { return NULL; }

	int WordCount = GetWordCount(Buffer, Length);
	if (WordCount == -1) { return NULL; }
	char** Tokens = (char**)malloc(WordCount * sizeof(char*));
	if (Tokens == NULL) { return NULL; }


	int i = 0;
	while (Buffer[i] == ';') { while (i < Length && Buffer[i] != '\n') { i++; } i++; }

	int State = isspace(Buffer[i]);
	int Prev = State;
	i++;

	int WordIndex = 0;
	int WordStart = 0;
	int WordEnd;

	for (; i < Length; ++i) {
		State = isspace(Buffer[i]);

		if (Buffer[i] == ';') { while (i < Length && Buffer[i] != '\n') { i++; } State = SPACE; }
		else {
			if (State == WORD && Prev == SPACE) { WordStart = i; }
			if ((State == SPACE) && Prev == WORD) {
				WordEnd = i;
				Tokens[WordIndex] = (char*)malloc(WordEnd - WordStart + 1);
				if (Tokens[WordIndex] == NULL) { return NULL; }
				memcpy(Tokens[WordIndex], &(Buffer[WordStart]), WordEnd - WordStart);
				Tokens[WordIndex][WordEnd - WordStart] = '\0';
				WordIndex++;
			}
		}

		Prev = State;
	}

	if (State == WORD) {
		WordEnd = Length;
		Tokens[WordIndex] = (char*)malloc(WordEnd - WordStart + 1);
		if (Tokens[WordIndex] == NULL) { return NULL; }
		memcpy(Tokens[WordIndex], &(Buffer[WordStart]), WordEnd - WordStart);
		Tokens[WordIndex][WordEnd - WordStart] = '\0';
	}

	*ResultWordCount = WordCount;
	return Tokens;
}

int DestroyFile(char** Tokens, int WordCount) {
	if (Tokens == NULL) { return 1; }
	for (int i = 0; i < WordCount; ++i) { free((void*)(Tokens[i])); }
	return 0;
}
