#include "../Headers/InstructionDecoder.h"

#include "../Headers/FileReader.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define GET_ASSEMBLER_INSTRUCTIONS(WordCount) (char*[]){ "ORG", "DB", "DW" }; (WordCount) = 3;


char** LoadASMLang(const char* FileName, int* ResultWordCount) {
	int i = 0; while (FileName[i] != '\0' && FileName[i] != '.') { i++; }
	if (strcmp(&(FileName[i]), ".lng")) { return NULL; }

	char** ASMLang = ReadFile(FileName, ResultWordCount);
	if (ASMLang == NULL) { return NULL; }

	if (*ResultWordCount % 3 != 0) { return NULL; }
	return ASMLang;
}

unsigned int DecodeNumber(const char* Number) {
	if (Number == NULL) { return -1; }
	
	unsigned int Result;
	char* CleanedNumber = (char*)malloc(strlen(Number));
	if (CleanedNumber == NULL) { return -1; }

	strncpy(CleanedNumber, Number, strlen(Number));
	CleanedNumber[strlen(Number) - 1] = '\0';

	switch (Number[strlen(Number) - 1]) {
		case 'h': {
			Result = (unsigned int)strtol(CleanedNumber, NULL, 16);
			break;
		} case 'b': {
			Result = (unsigned int)strtol(CleanedNumber, NULL, 2);
			break;
		} default: {
			Result = (unsigned int)strtol(Number, NULL, 10);
			break;
		}
	}

	free((void*)CleanedNumber);
	return Result;
}

int InFile(const char* Word, char** File, int WordCount, int Jumps) {
	for (int i = 0; i < WordCount; i += Jumps) {
		if (strcmp(Word, File[i]) == 0) { return i; }
	}
	return -1;
}

unsigned int DecodeNumberOrLabel(const char* Number, char** Labels, int* LabelValues, int LabelCount) {
	if (Number == NULL || Labels == NULL || LabelValues == NULL) { return 0; }

	int LabelIndex = InFile(Number, Labels, LabelCount, 1);
	if (LabelIndex == -1) { return DecodeNumber(Number); }
	return (unsigned int)(LabelValues[LabelIndex]);
}


int GetLabelCount(char** Tokens, int ProgramWordCount, char** ASMLang, int LangWordCount) {
	int AssemblerWordCount;
	char** AssemblerInstructions = GET_ASSEMBLER_INSTRUCTIONS(AssemblerWordCount);

	if (AssemblerInstructions == NULL || Tokens == NULL) { return -1; }

	int LabelCount = 0;

	for (int i = 0; i < ProgramWordCount; ++i) {
		int TokenIndex = InFile(Tokens[i], ASMLang, LangWordCount, 3);

		if (InFile(Tokens[i], AssemblerInstructions, AssemblerWordCount, 1) != -1) { i++; }
		else if (TokenIndex != -1) { if (DecodeNumber(ASMLang[TokenIndex + 1]) > 0) { i++; } }
		else if (Tokens[i][0] == ':' && Tokens[i][strlen(Tokens[i]) - 1] == ':') { LabelCount++; }
		else { return -1; }
	}

	return LabelCount;
}

int GetLabels(char** Tokens, int ProgramWordCount, char** ASMLang, int LangWordCount, char*** Labels, unsigned int** LabelValues, unsigned int OutputFileSize) {
	if (Tokens == NULL || ASMLang == NULL || Labels == NULL || LabelValues == NULL) { return -1; }
	int LabelCount = GetLabelCount(Tokens, ProgramWordCount, ASMLang, LangWordCount);
	int AddedLabel = 0;
	if (LabelCount == -1) { return -1; }

	*Labels = (char**)malloc(LabelCount * sizeof(char*));
	*LabelValues = (int*)malloc(LabelCount * sizeof(int));
	if (*Labels == NULL || *LabelValues == NULL) { return -1; }

	int AssemblerWordCount;
	char** AssemblerInstructions = GET_ASSEMBLER_INSTRUCTIONS(AssemblerWordCount);

	if (AssemblerInstructions == NULL || Tokens == NULL) { return -1; }

	int WriteLocation = 0;

	for (int i = 0; i < ProgramWordCount; ++i) {
		int TokenIndex = InFile(Tokens[i], ASMLang, LangWordCount, 3);

		switch (InFile(Tokens[i], AssemblerInstructions, AssemblerWordCount, 1)) {
			case 0: { WriteLocation = DecodeNumber(Tokens[i + 1]); i++; break;	}
			case 1: { i++; WriteLocation++;	break; }
			case 2: { i++; WriteLocation += 2; break; }
			case -1: { goto NOT_FOUND; }
		}
		goto FOUND;

		NOT_FOUND:
		if (TokenIndex != -1) {
			WriteLocation++;
			switch (DecodeNumber(ASMLang[TokenIndex + 1])) {
				case 1: { i++; WriteLocation++; }
				case 2: { i++; WriteLocation += 2; }
			}
		} else if (Tokens[i][0] == ':' && Tokens[i][strlen(Tokens[i]) - 1] == ':') {
			(*Labels)[AddedLabel] = (char*)malloc(strlen(Tokens[i]) + 1);
			strcpy((*Labels)[AddedLabel], Tokens[i]);
			(*LabelValues)[AddedLabel] = WriteLocation;
			AddedLabel++;
		} else {
			return -1;
		}

		FOUND:
		if (WriteLocation > OutputFileSize) { return -1; }
	}

	return LabelCount;
}


Byte* DecodeInstructions(char** ASMLang, int LangWordCount, char** Tokens, int ProgramWordCount, unsigned int OutputFileSize, unsigned int Direction) {
	int AssemblerWordCount;
	char** AssemblerInstructions = GET_ASSEMBLER_INSTRUCTIONS(AssemblerWordCount);

	Byte* Output = (Byte*)malloc(OutputFileSize);
	memset((void*)Output, 0, OutputFileSize);
	int WriteLocation = 0;

	char** Labels;
	unsigned int* LabelValues;

	int LabelCount = GetLabels(Tokens, ProgramWordCount, ASMLang, LangWordCount, &Labels, &LabelValues, OutputFileSize);
	if (LabelCount == -1) { return NULL; }

	if (AssemblerInstructions == NULL || ASMLang == NULL || Tokens == NULL || Output == NULL) { return NULL; }

	for (int i = 0; i < ProgramWordCount; ++i) {
		switch (InFile(Tokens[i], AssemblerInstructions, AssemblerWordCount, 1)) {
			case 0: {
				WriteLocation = DecodeNumberOrLabel(Tokens[i + 1], Labels, LabelValues, LabelCount);
				i++;
				break;
			} case 1: {
				Output[WriteLocation] = (Byte)DecodeNumberOrLabel(Tokens[i + 1], Labels, LabelValues, LabelCount);
				i++; WriteLocation++;
				break;
			} case 2: {
				Word NumberWord = (Word)DecodeNumberOrLabel(Tokens[i + 1], Labels, LabelValues, LabelCount);
				Byte* Number = (Byte*)(&NumberWord);
				Output[WriteLocation] = Number[sizeof(Word) - (2 - Direction)];
				Output[WriteLocation + 1] = Number[sizeof(Word) - (1 + Direction)];
				i++; WriteLocation += 2;
				break;
			} case -1: {
				goto NOT_FOUND;
			}
		}
		goto FOUND;

		NOT_FOUND:
		int TokenIndex = InFile(Tokens[i], ASMLang, LangWordCount, 3);
		if (TokenIndex != -1) {
			Output[WriteLocation] = (Byte)DecodeNumber(ASMLang[TokenIndex + 2]);
			WriteLocation++;
			switch (DecodeNumber(ASMLang[TokenIndex + 1])) {
				case 1: {
					Output[WriteLocation] = (Byte)DecodeNumberOrLabel(Tokens[i + 1], Labels, LabelValues, LabelCount);
					i++; WriteLocation++;
					break;
				} case 2: {
					Word NumberWord = (Word)DecodeNumberOrLabel(Tokens[i + 1], Labels, LabelValues, LabelCount);
					Byte* Number = (Byte*)(&NumberWord);
					Output[WriteLocation] = Number[sizeof(Word) - (2 - Direction)];
					Output[WriteLocation + 1] = Number[sizeof(Word) - (1 + Direction)];
					i++; WriteLocation += 2;
					break;
				}
			}
		} else if (Tokens[i][0] == ':' && Tokens[i][strlen(Tokens[i]) - 1] == ':') {
			goto FOUND;
		} else { return NULL; }

		FOUND:
		if (WriteLocation > OutputFileSize) { return NULL; }
	}

	DestroyFile(Labels, LabelCount);
	free((void*)Labels);
	free((void*)LabelValues);
	return Output;
}