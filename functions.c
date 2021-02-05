#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "constants.h"
#include "functions.h"

void clean(char **knowenWords, char *keyString, char *inputfileText, FILE *input, FILE *output, FILE *knowenWordsFile)
{
  free(knowenWords);
  free(keyString);
  free(inputfileText);
  fclose(input);
  fclose(output);
  fclose(knowenWordsFile);
}

int hex2int(char h)
{
  h = toupper(h); // if h is a lowercase letter then convert it to uppercase

  if (h >= '0' && h <= '9')
    return h - '0';
  else if (h >= 'A' && h <= 'F')
    return h - 'A' + 10;
  else
  {
    fprintf(stderr, "keyString should contain hexa digits\n");
    exit(1);
  }
  return 0;
}

int processKey(char *keyString)
{
  int n = strlen(keyString);
  if (n % 2 || n / 2 > KEY_SIZE)
  {
    fprintf(stderr, "keyString must have even number of bytes. Number of bytes should not exceed %d\n", KEY_SIZE);
    exit(1);
  }

  for (int i = 0; i < n; i += 2)
  {
    keyBytes[i / 2] = (hex2int(keyString[i]) << 4) | hex2int(keyString[i + 1]);
  }
  return n / 2;
}

void encode(int numBytesInKey)
{
  int i, c;

  for (i = 0; (c = getchar()) != EOF;)
  {
    putchar(c ^ keyBytes[i++]);
    if (i >= numBytesInKey)
      i = 0;
  }
}

char *encodeToString(int numBytesInKey, FILE *fp)
{
  int i, j, c;
  int startSize = ALLOCATION_SIZE;
  char *encodedString = (char *)malloc(sizeof(char) * startSize);

  for (i = 0, j = 0; (c = fgetc(fp)) != EOF;)
  {
    if (j >= startSize)
    {
      startSize += ALLOCATION_SIZE;
      encodedString = (char *)realloc(encodedString, sizeof(char) * startSize);
    }
    encodedString[j] = c ^ keyBytes[i];
    i++;
    if (i >= numBytesInKey)
      i = 0;
    j++;
  }

  return encodedString;
}

char *inputString(FILE *fp, size_t allocated_size)
{
  char *string;
  int ch;
  size_t input_length = 0;
  string = (char *)realloc(NULL, sizeof(char) * allocated_size);
  if (!string)
    return string;
  while (EOF != (ch = fgetc(fp)))
  {
    if (ch == EOF)
      break;
    string[input_length] = ch;
    input_length += 1;
    if (input_length == allocated_size)
    {
      string = (char *)realloc(string, sizeof(char) * (allocated_size += ALLOCATION_SIZE));
      if (!string)
        return string;
    }
  }
  return (char *)realloc(string, sizeof(char) * (input_length));
}

char *createKey(unsigned int keyInt, int givenLen)
{
  int keyIntLen = floor(log10(keyInt)) + 1;
  char *keyString = (char *)malloc(givenLen * sizeof(char));
  int keyStrLen = sprintf(keyString, "%x", keyInt);
  int i;

  if (keyStrLen <= givenLen && keyStrLen % 2 == 1)
  {
    char *temp = (char *)malloc(givenLen * sizeof(char));
    strcpy(temp, "0");
    for (i = givenLen - keyStrLen; i > 1; i--)
    {
      strcat(temp, "0");
    }
    strcat(temp, keyString);
    keyString = strdup(temp);
    free(temp);
  }
  return keyString;
}

char **splitStringByDelimiter(int inputLen, char *inputStr, char *delim, int *counter)
{
  int i = 0;
  char **decodedSplitArray = (char **)malloc(inputLen * sizeof(char *));

  decodedSplitArray[i] = strtok_r(inputStr, delim, &inputStr);
  while (decodedSplitArray[i] != NULL)
  {
    if (i >= inputLen)
    {
      decodedSplitArray = (char **)realloc(decodedSplitArray, sizeof(char *) * (inputLen += ALLOCATION_SIZE));
    }
    decodedSplitArray[++i] = strtok_r(NULL, delim, &inputStr);
  }

  *counter = i;
  return decodedSplitArray;
}