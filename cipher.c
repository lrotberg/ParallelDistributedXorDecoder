#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#define START_SIZE 512
#define EXTEND_SIZE 32
#define MAX_KEY_SIZE 4

char *readStringFromFile(FILE *fp, size_t allocated_size, size_t *input_length)
{
  char *string;
  int ch;
  *input_length = 0;
  string = realloc(NULL, sizeof(char) * allocated_size);
  if (!string)
    return string;
  while (EOF != (ch = fgetc(fp)))
  {
    if (ch == EOF)
      break;
    string[*input_length] = ch;
    *input_length += 1;
    if (*input_length == allocated_size)
    {
      string = realloc(string, sizeof(char) * (allocated_size += EXTEND_SIZE));
      if (!string)
        return string;
    }
  }
  return realloc(string, sizeof(char) * (*input_length));
}

void binaryStringToBinary(char *string, size_t num_bytes)
{
  int i, byte;
  unsigned char binary_key[MAX_KEY_SIZE];
  for (byte = 0; byte < num_bytes; byte++)
  {
    binary_key[byte] = 0;
    for (i = 0; i < 8; i++)
    {
      binary_key[byte] = binary_key[byte] << 1;
      binary_key[byte] |= string[byte * 8 + i] == '1' ? 1 : 0;
    }
  }
  memcpy(string, binary_key, num_bytes);
}

void cipher(char *key, size_t key_len, FILE *input, FILE *output)
{
  int i, j = 0;
  size_t input_length;
  char *input_str = readStringFromFile(input, START_SIZE, &input_length);
  char *output_str = malloc(input_length * sizeof(char));
  if (!input_str || !output_str)
  {
    fprintf(stderr, "Error reading string\n");
    exit(0);
  }
  for (i = 0; i < input_length; i++, j++)
  {
    if (j == key_len)
      j = 0;
    output_str[i] = input_str[i] ^ key[j];
  }
  fwrite(output_str, sizeof(char), input_length, output);
  free(output_str);
  free(input_str);
}

void printHelp(char *argv)
{
  fprintf(stdout, "usage: %s KEY KEY_LENGTH [options]...\nEncrypt a file using xor cipher (key length in bytes)\n", argv);
  fprintf(stdout, "    -i, --input             specify input file\n");
  fprintf(stdout, "    -o, --output            specify output file\n");
  fprintf(stdout, "    -b, --binary            read key as binary\n");
}

int main(int argc, char *argv[])
{
  FILE *input, *output;
  int i, j;
  if (argc < 3 || argc > 8)
  {
    printHelp(argv[0]);
    return 0;
  }
  input = stdin;
  output = stdout;
  for (i = 3; i < argc; i++)
  {
    if (strcmp(argv[i], "-input") == 0 || strcmp(argv[i], "-i") == 0)
    {
      i++;
      input = fopen(argv[i], "r");
      if (!input)
      {
        fprintf(stderr, "Error opening file\n");
        return 0;
      }
      continue;
    }
    if (strcmp(argv[i], "-output") == 0 || strcmp(argv[i], "-o") == 0)
    {
      i++;
      output = fopen(argv[i], "w");
      if (!output)
      {
        fprintf(stderr, "Error opening file\n");
        return 0;
      }
      continue;
    }
    if (strcmp(argv[i], "-binary") == 0 || strcmp(argv[i], "-b") == 0)
    {
      binaryStringToBinary(argv[1], atoi(argv[2]));
      continue;
    }
    printHelp(argv[0]);
    return 0;
  }

  cipher(argv[1], atoi(argv[2]), input, output);

  unsigned char text[9] = "no cigar.";
  unsigned int textInt[9] = {0};
  unsigned int cipher = 0b10011001;

  fprintf(output, "\n");
  for (i = 0; i < 9; i++)
  {
    fprintf(output, "%x ", text[i]);
  }

  unsigned char text2[9] = "no cigar.";
  // unsigned int textInt[9];
  const unsigned char *cipher2 = "0000000010011001";
  // unsigned char part1[8];
  // strncpy(part1, cipher2, 8);
  // unsigned char part2[8];
  // strncpy(part2, cipher2 + 8, 8);
  // fprintf(output, "\npart1 := %s, part 2 := %s", part1, part2);
  char *parts[2] = {strndup(cipher2, 8), strndup(cipher2 + 8, 8)};
  // parts[0] = part1;
  // parts[1] = part2;
  fprintf(output, "\nparts[0] => %s, parts[1] => %s\n", parts[0], parts[1]);

  fprintf(output, "\n");
  for (i = 0, j = 0; i < 9; i++, j++)
  {
    if (j == 2)
    {
      j = 0;
    }
    // fprintf(output, "%c %s ", text2[i], parts[j]);
    textInt[i] = text2[i] ^ (int)*parts[j];
    // textInt[i] = textInt[i] ^ (int)*parts[j];
    fprintf(output, "%x ", textInt[i]);
  }

  unsigned char text3[18] = "00000000no cigar.";
  unsigned int textInt2[18] = {0};
  cipher = 0b1111100110011001;

  fprintf(output, "\n");
  for (i = 0; i < 18; i++)
  {
    textInt2[i] = text3[i] ^ cipher;
    fprintf(output, "%x ", textInt2[i]);
  }

  for (i = 0; i < sizeof(parts) / sizeof(parts[0]); i++)
  {
    free(parts[i]);
  }

  fclose(input);
  fclose(output);
  return 0;
}
