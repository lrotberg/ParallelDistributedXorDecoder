#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define KEY_SIZE 8
#define ALLOCATION_SIZE 10
enum
{
  MIN_VALUE = 0x1,
  SM_MAX_VALUE = 0xFF,
  MD_MAX_VALUE = 0xFFFF,
  LG_MAX_VALUE = 0xFFFFFFFF
};
// #define MIN_VALUE 0x01
// #define MAX_VALUE 0xFF

char keyBytes[KEY_SIZE];

int hex2int(char h);
int processKey(char *keyString);

char *inputString(FILE *fp, size_t size);
char *createKey(unsigned int keyInt, int givenLen);
char *encodeToString(int numBytesInKey, FILE *fp);
char **splitStringByDelimiter(int inputLen, char *inputStr, char *delim, int *counter);

void encode(int numBytesInKey);
void clean(char **knowenWords, char *keyString, char *inputfileText, FILE *input, FILE *output, FILE *knowenWordsFile);

int main(int argc, char **argv)
{
  FILE *input = stdin, *output = stdout, *knowenWordsFile;
  unsigned int keyInt = MIN_VALUE;
  char *keyString;
  int numBytesInKey;
  char *inputfileText, *decodedText, **knowenWords, **decodedSplitArray;
  int knowenWordsCounter, decodedWordsCounter, cmpRes, givenLen, maxNum;
  int i, j, c;
  time_t start, end;

  start = time(NULL);

  // * open crypted file
  // givenLen = *argv[1];
  sscanf(argv[1], "%d", &givenLen);
  switch (givenLen)
  {
  case 2:
    maxNum = MD_MAX_VALUE;
    break;
  case 4:
    maxNum = LG_MAX_VALUE;
    break;
  default:
    maxNum = SM_MAX_VALUE;
  }
  // fprintf(stderr, "\ngivenLen -> %d || maxNum 0x%x\n", givenLen, maxNum);

  input = fopen(argv[2], "r");
  if (!input)
  {
    fprintf(stderr, "Error opening words file\n");
    return 0;
  }

  // * open words file
  if (argc > 3)
    knowenWordsFile = fopen(argv[3], "r");
  else
    knowenWordsFile = fopen("linux_words.txt", "r");
  if (!knowenWordsFile)
  {
    fprintf(stderr, "Error opening file words\n");
    return 0;
  }

  // * get number of words for words array dynamic memory allocation
  fscanf(knowenWordsFile, "%d", &knowenWordsCounter);

  // * allocate knowen words array and each of it's words
  inputfileText = inputString(knowenWordsFile, ALLOCATION_SIZE);
  knowenWords = splitStringByDelimiter(ALLOCATION_SIZE, inputfileText, "\n", &decodedWordsCounter);

  while (keyInt <= maxNum)
  {
    fprintf(stderr, "\nBefore creation, givenLen -> %d\n", givenLen);
    keyString = createKey(keyInt, 2 * givenLen);
    fprintf(stderr, "\nkeyString ----> 0x%s\n", keyString);
    numBytesInKey = processKey(keyString);
    fprintf(stderr, "numBytesInKey ---> %d\n", numBytesInKey);

    // * encode the text to a string
    decodedText = encodeToString(numBytesInKey, input);
    fprintf(stderr, "decodedText ----> %s\n", decodedText);

    // * split text string into a string array by 'space' delimiter
    decodedSplitArray = splitStringByDelimiter(ALLOCATION_SIZE, strdup(decodedText), " ", &decodedWordsCounter);
    // fprintf(stderr, "%d", decodedWordsCounter);

    // fprintf(stderr, "%s", decodedText);
    // * match all words of decoded text with each of the knowen words
    for (i = 0; i < decodedWordsCounter; i++)
    {
      for (j = 0; j < knowenWordsCounter; j++)
      {
        if (strlen(knowenWords[j]) > 2)
        {
          cmpRes = strcmp(decodedSplitArray[i], knowenWords[j]);
          if (cmpRes == 0 && strlen(knowenWords[j]) > 2) // * words match
          {
            goto exitLoop;
          }
        }
      }
    }

    // * free current iteration
    free(decodedSplitArray);
    free(decodedText);

    fseek(input, 0, SEEK_SET);
    keyInt++;
  }

exitLoop:
  if (cmpRes == 0)
  {
    fprintf(stderr, "\nSuccsess!\nKey is: 0x%s\nDecoded text is:\n%s\n\n", keyString, decodedText);

    free(decodedSplitArray);
    free(decodedText);
  }
  else
  {
    fprintf(stderr, "\nFailure! No valid key was found\n");
  }

  // * clean all
  clean(knowenWords, keyString, inputfileText, input, output, knowenWordsFile);

  end = time(NULL);
  fprintf(stderr, "Time taken to calculate the key is %.7f seconds\n", difftime(end, start));
  return 0;
} // * main

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
  // if (keyStrLen <= 8 && keyStrLen % 2 == 1)
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
  // fprintf(stderr, "%s", keyString);
  /*switch (givenLen)
  {
  case 2:
    strcat(keyString, keyString);
    break;
  case 4:
    strcat(keyString, keyString);
    strcat(keyString, keyString);
    break;
  default:
    // fprintf(stderr, "key byte length must be 1, 2 or 4! Key is %c byte length\nThe key is reverting to a 1 byte length\n", givenLen);
    break;
  }*/
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