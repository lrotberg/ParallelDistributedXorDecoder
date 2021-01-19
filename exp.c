#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

enum SIZES
{
  BYTE = 8,
  EXTEND_SIZE = 32,
  SMALL = 255,
  START_SIZE = 512,
  MEDIUM = 65535,
  LARGE = 16777215,
  X_LARGE = 4294967295
};

char *decimalNumToBinaryString(unsigned int n);
void divideStringtoSubStrings(unsigned char **keyParts, unsigned char *cipher, int keyLen);
char *readStringFromFile(FILE *fp, size_t allocated_size);
char **splitStringBySpaces(int inputLen, char *inputStr, FILE *outputFile);

int main(int argc, char *argv[])
{
  int i, j;
  FILE *input = fopen(argv[1], "rb"), *output = stdout, *outputFile = fopen("text2.txt", "w");
  char *inputStr = readStringFromFile(input, START_SIZE);
  char dev = *argv[2];
  int inputLen = strlen(inputStr);
  unsigned int cryptedStr[inputLen];
  // char **wordsArr = splitStringBySpaces(inputLen,inputStr,outputFile);

  if (dev == '1')
  {
    fprintf(output, "\n%d", inputLen);
  }

  unsigned int key = 0b10011001;
  // unsigned char *cipher = decimalNumToBinaryString(key);
  // unsigned int keyLen = strlen(cipher) / BYTE;
  // unsigned char *keyParts[keyLen];
  // divideStringtoSubStrings(keyParts, cipher, keyLen);

  fprintf(output, "\n");
  for (i = 0, j = 0; i < inputLen; i++, j++)
  {
    // if (j == keyLen) {
    //   j = 0;
    //}
    // fprintf(output, "%s ", (int)keyParts[0]);
    cryptedStr[i] = (inputStr[i] ^ key) + '0';
    // cryptedStr[i] = (cryptedStr[i] - '0') ^ 0b10011001;
    fprintf(output, "%c", cryptedStr[i]);
  }
  fprintf(output, "\n");

  // if (dev == '1')
  // {
  //   fprintf(output, "\n%d\n", keyLen);

  //   for(i = 0 ; i < keyLen ; i++)
  //   {
  //     fprintf(output, "%s ", keyParts[i]);
  //   }
  //   fprintf(output, "\n");
  // }

  // for (i = 0; i < keyLen; i++)
  // {
  //   free(keyParts[i]);
  // }

  // free(wordsArr);
  fclose(input);
  fclose(output);
  return 0;
}

char *decimalNumToBinaryString(unsigned int n)
{
  int c, t;
  int size;
  char *p;

  if (n <= SMALL)
    size = 8;
  else if (n <= MEDIUM)
    size = 16;
  else if (n <= LARGE)
    size = 24;
  else
    size = 32;

  t = 0;
  p = (char *)malloc(size + 1);

  if (p == NULL)
    exit(EXIT_FAILURE);

  for (c = (size - 1); c >= 0; c--)
    *(p + t++) = ((n >> c) & 1) ? 1 + '0' : 0 + '0';
  *(p + t) = '\0';

  return p;
}

void divideStringtoSubStrings(unsigned char **keyParts, unsigned char *cipher, int keyLen)
{
  int i;
  for (i = 0; i < keyLen; i++)
  {
    keyParts[i] = strndup(cipher + (i * BYTE), BYTE);
  }
}

char *readStringFromFile(FILE *fp, size_t allocated_size)
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
      string = realloc(string, sizeof(char) * (allocated_size += EXTEND_SIZE));
      if (!string)
        return string;
    }
  }
  return realloc(string, sizeof(char) * (input_length));
}

char **splitStringBySpaces(int inputLen, char *inputStr, FILE *outputFile)
{
  int i = 0;
  char **wordsArr = malloc(inputLen * sizeof(char *));

  wordsArr[i] = strtok_r(inputStr, " ", &inputStr);
  while (wordsArr[i] != NULL)
  {
    fprintf(outputFile, "%s\n", wordsArr[i]);
    wordsArr[++i] = strtok_r(NULL, " ", &inputStr);
  }

  return wordsArr;
}
