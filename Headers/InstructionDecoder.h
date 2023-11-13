#ifndef INSTRUCTION_DECODER_H
#define INSTRUCTION_DECODER_H

#define LITTLE_ENDIAN 0
#define BIG_ENDIAN 1

typedef unsigned char Byte;
typedef unsigned short Word;

char** LoadASMLang(const char* FileName, int* ResultWordCount);
unsigned int DecodeNumber(const char* Number);
Byte* DecodeInstructions(char** ASMLang, int LangWordCount, char** Tokens, int ProgramWordCount, unsigned int OutputFileSize, unsigned int Direction);
#endif