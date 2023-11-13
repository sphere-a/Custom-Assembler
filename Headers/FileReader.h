#ifndef FILE_READER_H
#define FILE_READER_H

char** ReadFile(const char* FileName, int* ResultWordCount);
int DestroyFile(char** Tokens, int WordCount);

#endif