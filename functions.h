#pragma once

int hex2int(char h);
int processKey(char *keyString);
int determineMaxNum(const char *fileName, int *givenLen);

char *inputString(FILE *fp, size_t size);
char *createKey(unsigned int keyInt, int givenLen);
char *encodeToString(int numBytesInKey, FILE *fp);
char **splitStringByDelimiter(int inputLen, char *inputStr, char *delim, int *counter);

void encode(int numBytesInKey);
void clean(char **knowenWords, char *keyString, char *inputfileText, FILE *input, FILE *output, FILE *knowenWordsFile);
void openFiles(const char *inputFileName, const char *knowenWordsFileName, const int argc, FILE *input, FILE *knowenWordsFile);