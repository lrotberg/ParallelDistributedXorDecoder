#pragma once

int hex2int(char h);
int processKey(char *keyString);
// int determineMaxNum(const char *fileName, int *givenLen);
int determineMaxNum(const char *fileName, int *partSize);
int countDigits(unsigned int num);

char *inputString(FILE *fp, size_t size);
char *createKey2(unsigned int keyInt, int givenLen);
char *createKey(unsigned int keyInt);
char *encodeToString(int numBytesInKey, FILE *fp);
char *encodeStr(int numBytesInKey, char *encodedStr, int encodedStrLen);
char **splitStringByDelimiter(int inputLen, char *inputStr, char *delim, int *counter);

void encode(int numBytesInKey);
void clean(char **knowenWords, char *keyString, char *inputfileText);
void openFiles(const char *inputFileName, const char *knowenWordsFileName, const int argc, FILE *input, FILE *knowenWordsFile);