#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
// #include <assert.h>
#include <string.h>
#include <math.h>

#define MAX_KEY_SIZE 8

char keyBytes[MAX_KEY_SIZE];

int processKey(char *key);
int countDigits(unsigned int num);
char *createKey(unsigned int keyInt);

int main(int argc, char **argv)
{
  // if (argc != 2) {
  //      fprintf(stderr, "usage: %s <key>\nkey should be specified with hexa digits\n", argv[0]);
  //      exit(1);
  // }
  unsigned int keyInt = 0x1002;
  char *key = createKey(keyInt);
  // fprintf(stderr, "\n%s\n", key);

  int nbytes; // number of bytes in key
  nbytes = processKey(key);

  int c;

  //  encode the input
  for (int i = 0; (c = getchar()) != EOF;)
  {
    putchar(c ^ keyBytes[i]); // ^  is the xor operator
    i++;
    if (i >= nbytes)
      i = 0;
  }

  free(key);

  return 0;
} // main

// char *createKey(unsigned int keyInt)
// {
//   int keyIntLen = floor(log10(keyInt)) + 1;
//   char *key = (char *)malloc(keyIntLen * sizeof(char));
//   int keyStrLen = sprintf(key, "%x", keyInt);
//   if (keyStrLen <= 8 && keyStrLen % 2 == 1)
//   {
//     char *temp = (char *)malloc(keyStrLen * sizeof(char));
//     strcpy(temp, "0");
//     strcat(temp, key);
//     // key = (char *)realloc(key, (keyStrLen + 1));
//     key = strdup(temp);
//     free(temp);
//   }
//   return key;
// }
int countDigits(unsigned int num)
{
  int count = 0x0;

  if (num == 0x0)
  {
    return 0x1;
  }

  while (num)
  {
    num /= 0x10;
    count++;
  }
  return count;
}

char *createKey(unsigned int keyInt)
{
  int keyIntLen = countDigits(keyInt);
  char *keyString = (char *)calloc(keyIntLen + 1, sizeof(char));

  if (keyIntLen % 2 == 1)
  {
    sprintf(keyString, "0%x", keyInt);
  }
  else
  {
    sprintf(keyString, "%x", keyInt);
  }
  // fprintf(stderr, "keyIntLen %d keyString %s\n", keyIntLen, keyString);
  return keyString;
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
  if (n % 2 || n / 2 > MAX_KEY_SIZE)
  {
    fprintf(stderr, "key must have even number of bytes. Number of bytes should not exceed %d\n", MAX_KEY_SIZE);
    exit(1);
  }

  for (int i = 0; i < n; i += 2)
  {
    keyBytes[i / 2] = (hex2int(key[i]) << 4) | hex2int(key[i + 1]);
  }
  return n / 2;
}
