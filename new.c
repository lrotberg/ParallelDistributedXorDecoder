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
int processKey(char *key);

char *inputString(FILE *fp, size_t size);
char *createKey(unsigned int keyInt);
char *encodeToString(int nbytes, FILE *fp);
char **splitStringBySpaces(int inputLen, char *inputStr);

void encode(int nbytes);
void clean(char **words, int numOfWords, char *key, FILE *input, FILE *output, FILE *wordsFile);

int main(int argc, char **argv)
{
  FILE *input = stdin, *output = stdout, *wordsFile;
  unsigned int keyInt = MIN_VALUE;
  char *key = createKey(keyInt);
  int nbytes = processKey(key); // number of bytes in key
  char *line, *text, **words, **splitArr;
  int numOfWords;
  int i = 0, c;

  // * open crypted file
  input = fopen(argv[1], "r");
  if (!input)
  {
    fprintf(stderr, "Error opening words file\n");
    return 0;
  }

  // * open words file
  if (argc > 2)
    wordsFile = fopen(argv[2], "r");
  else
    wordsFile = fopen("linux_words.txt", "r");
  if (!wordsFile)
  {
    fprintf(stderr, "Error opening file words\n");
    return 0;
  }

  // * get number of words for words array dynamic memory allocation
  fscanf(wordsFile, "%d", &numOfWords);

  // * memory allocation of words array
  words = (char **)malloc(sizeof(char *) * numOfWords);

  // * allocate each word in the words array
  for (i = 0; c = fgetc(wordsFile) != EOF;)
  {
    line = inputString(wordsFile, ALLOCATION_SIZE);
    words[i] = strdup(line);
    fprintf(stderr, "line %d -> %s", i, words[i++]);
    free(line);
  }

  // TODO: Finish this
  // while (1)
  // {
  // * encode the text to a string
  text = encodeToString(nbytes, input);
  // puts(text);

  // * split text string into a string array by 'space' delimiter
  splitArr = splitStringBySpaces(ALLOCATION_SIZE, text);

  free(text);
  // for
  free(splitArr);
  // }

  // * clean all
  clean(words, numOfWords, key, input, output, wordsFile);

  return 0;
} // * main

void clean(char **words, int numOfWords, char *key, FILE *input, FILE *output, FILE *wordsFile)
{
  int i;

  // fprintf(stderr, "\n%s\n", words[0]);

  // for (i = 0; i < numOfWords; i++)
  // {
  //   free(words[i]);
  // }
  free(words);
  free(key);
  fclose(input);
  fclose(output);
  fclose(wordsFile);
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
    fprintf(stderr, "key should contain hexa digits\n");
    exit(1);
  }
  return 0;
}

int processKey(char *key)
{
  int n = strlen(key);
  if (n % 2 || n / 2 > KEY_SIZE)
  {
    fprintf(stderr, "key must have even number of bytes. Number of bytes should not exceed %d\n", KEY_SIZE);
    exit(1);
  }

  for (int i = 0; i < n; i += 2)
  {
    keyBytes[i / 2] = (hex2int(key[i]) << 4) | hex2int(key[i + 1]);
  }
  return n / 2;
}

void encode(int nbytes)
{
  int i, c;

  for (i = 0; (c = getchar()) != EOF;)
  {
    putchar(c ^ keyBytes[i++]);
    if (i >= nbytes)
      i = 0;
  }
}

char *encodeToString(int nbytes, FILE *fp)
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
    if (i >= nbytes)
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
  string = realloc(NULL, sizeof(char) * allocated_size);
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
      string = realloc(string, sizeof(char) * (allocated_size += ALLOCATION_SIZE));
      if (!string)
        return string;
    }
  }
  return realloc(string, sizeof(char) * (input_length));
}

char *createKey(unsigned int keyInt)
{
  int keyIntLen = floor(log10(keyInt)) + 1;
  char *key = (char *)malloc(keyIntLen * sizeof(char));
  int keyStrLen = sprintf(key, "%x", keyInt);
  if (keyStrLen <= 8 && keyStrLen % 2 == 1)
  {
    char *temp = (char *)malloc(keyStrLen * sizeof(char));
    strcpy(temp, "0");
    strcat(temp, key);
    key = strdup(temp);
    free(temp);
  }
  return key;
}

char **splitStringBySpaces(int inputLen, char *inputStr) //, FILE *outputFile
{
  int i = 0;
  char **splitArr = (char **)malloc(inputLen * sizeof(char *));

  splitArr[i] = strtok_r(inputStr, " ", &inputStr);
  while (splitArr[i] != NULL)
  {
    if (i >= inputLen)
    {
      splitArr = (char **)realloc(splitArr, sizeof(char *) * (inputLen += ALLOCATION_SIZE));
    }
    // fprintf(stderr, "%s\n", splitArr[i]);
    splitArr[++i] = strtok_r(NULL, " ", &inputStr);
  }

  return splitArr;
}