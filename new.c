#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

#define KEY_SIZE 8
#define ALLOCATION_SIZE 10
#define MIN_VALUE 0x01
#define MAX_VALUE 0xFF

char keyBytes[KEY_SIZE];

int hex2int(char h);
int processKey(char *keyString);

char *inputString(FILE *fp, size_t size);
char *createKey(unsigned int keyInt);
char *encodeToString(int numBytesInKey, FILE *fp);
char **splitStringByDelimiter(int inputLen, char *inputStr, char *delim, int *counter);

void encode(int numBytesInKey);
void clean(char **knowenWords, int knowenWordsCounter, char *keyString, FILE *input, FILE *output, FILE *knowenWordsFile);

int main(int argc, char **argv)
{
  FILE *input = stdin, *output = stdout, *knowenWordsFile;
  unsigned int keyInt = MIN_VALUE;
  char *keyString;   // = createKey(keyInt);
  int numBytesInKey; // = processKey(keyString); // number of bytes in keyString
  char *inputfileText, *decodedText, **knowenWords, **decodedSplitArray;
  int knowenWordsCounter, decodedWordsCounter, cmpRes;
  int i, j, c;

  // * open crypted file
  input = fopen(argv[1], "r");
  if (!input)
  {
    fprintf(stderr, "Error opening words file\n");
    return 0;
  }

  // * open words file
  if (argc > 2)
    knowenWordsFile = fopen(argv[2], "r");
  else
    knowenWordsFile = fopen("linux_words.txt", "r");
  if (!knowenWordsFile)
  {
    fprintf(stderr, "Error opening file words\n");
    return 0;
  }

  // * get number of words for words array dynamic memory allocation
  fscanf(knowenWordsFile, "%d", &knowenWordsCounter);

  // * memory allocation of words array
  knowenWords = (char **)malloc(sizeof(char *) * knowenWordsCounter);

  // * allocate each word in the words array
  inputfileText = inputString(knowenWordsFile, ALLOCATION_SIZE);
  knowenWords = splitStringByDelimiter(ALLOCATION_SIZE, inputfileText, "\n", &decodedWordsCounter);

  // TODO: Finish this
  while (keyInt <= MAX_VALUE)
  {
    keyString = createKey(keyInt);
    numBytesInKey = processKey(keyString);

    // * encode the text to a string
    decodedText = encodeToString(numBytesInKey, input);

    // * split text string into a string array by 'space' delimiter
    decodedSplitArray = splitStringByDelimiter(ALLOCATION_SIZE, decodedText, " ", &decodedWordsCounter);
    // fprintf(stderr, "%d", decodedWordsCounter);

    // TODO: Add the loop
    // *
    for (i = 0; i < decodedWordsCounter; i++)
    {
      for (j = 0; j < knowenWordsCounter; j++)
      {
        cmpRes = strcmp(decodedSplitArray[i], knowenWords[j]);
        if (cmpRes == 0) // * strings match
        {
          goto exitLoop;
        }
      }
    }

    // * free current iteration
    free(decodedText);
    for (i = 0; i < decodedWordsCounter; i++)
    {
      free(decodedSplitArray[i]);
    }
    free(decodedSplitArray);

    keyInt++;
  }

exitLoop:
  if (cmpRes == 0)
  {
    puts("Succsess! Key is:");
    puts(keyString);
    puts("Decoded text is:");
    puts(decodedText);

    free(decodedText);
    for (i = 0; i < decodedWordsCounter; i++)
    {
      free(decodedSplitArray[i]);
    }
    free(decodedSplitArray);
  }

  free(inputfileText);

  // * clean all
  clean(knowenWords, knowenWordsCounter, keyString, input, output, knowenWordsFile);

  return 0;
} // * main

void clean(char **knowenWords, int knowenWordsCounter, char *keyString, FILE *input, FILE *output, FILE *knowenWordsFile)
{
  int i;

  for (i = 0; i < knowenWordsCounter; i++)
  {
    free(knowenWords[i]);
  }
  free(knowenWords);
  free(keyString);
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
      encodedString = (char *)realloc(encodedString, sizeof(char) * (startSize += ALLOCATION_SIZE));
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
  // *input_length = 0;
  string = (char *)realloc(NULL, sizeof(char) * allocated_size);
  if (!string)
    return string;
  while (EOF != (ch = fgetc(fp)))
  {
    if (ch == EOF)
      break;
    // if (ch == *"\n")
    // {
    //   fseek(fp, -1, SEEK_CUR);
    //   break;
    // }
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

char *createKey(unsigned int keyInt)
{
  int keyIntLen = floor(log10(keyInt)) + 1;
  char *keyString = (char *)malloc(keyIntLen * sizeof(char));
  int keyStrLen = sprintf(keyString, "%x", keyInt);
  if (keyStrLen <= 8 && keyStrLen % 2 == 1)
  {
    char *temp = (char *)malloc(keyStrLen * sizeof(char));
    strcpy(temp, "0");
    strcat(temp, keyString);
    keyString = strdup(temp);
    free(temp);
  }
  return keyString;
}

char **splitStringByDelimiter(int inputLen, char *inputStr, char *delim, int *counter) //, FILE *outputFile
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
    // fprintf(stderr, "%s\n", decodedSplitArray[i]);
    decodedSplitArray[++i] = strtok_r(NULL, delim, &inputStr);
  }

  *counter = i;
  return decodedSplitArray;
}