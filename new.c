#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <mpi.h>
#include <omp.h>
#include "constants.h"
#include "functions.h"

int main(int argc, char **argv)
{
  FILE *input = stdin, *output = stdout, *knowenWordsFile;
  unsigned int keyInt;
  char *keyString;
  int numBytesInKey;
  char *inputfileText, *decodedText, **knowenWords, **decodedSplitArray;
  int knowenWordsCounter, decodedWordsCounter, cmpRes, givenLen, maxNum;
  int i, j, c;
  int cond = 0, comSize, procRank, startIndex, endIndex;
  int partSize;
  time_t start, end;
  MPI_Status status;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &comSize);
  MPI_Comm_rank(MPI_COMM_WORLD, &procRank);

  start = time(NULL);

  if (procRank == 0)
  {
    // * open crypted file
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
    MPI_Bcast();
  }

  maxNum = determineMaxNum(argv[1], &givenLen, &partSize);
  startIndex = partSize * procRank;
  endIndex = startIndex + partSize + 1;
  if (procRank == comSize - 1)
  {
    endIndex = maxNum;
  }
  fprintf(stderr, "\npartSize %d rank %d startIndex 0x%x endIndex 0x%x\n", partSize, procRank, startIndex, endIndex);

#pragma omp parallel for collapse(3) private(i, j)
  for (keyInt = startIndex; keyInt < endIndex; keyInt++)
  {
    keyString = createKey(keyInt, 2 * givenLen);
    numBytesInKey = processKey(keyString);

    // * encode the text to a string
    decodedText = encodeToString(numBytesInKey, input);

    // * split text string into a string array by 'space' delimiter
    decodedSplitArray = splitStringByDelimiter(ALLOCATION_SIZE, strdup(decodedText), " ", &decodedWordsCounter);

    // * match all words of decoded text with each of the knowen words
    for (i = 0; i < decodedWordsCounter; i++)
    {
      for (j = 0; j < knowenWordsCounter; j++)
      {
        if (strlen(knowenWords[j]) > 2)
        {
          cmpRes = strcmp(decodedSplitArray[i], knowenWords[j]);
          if (cmpRes == 0 && strlen(knowenWords[j]) > 2) // * words match and knowen word is longer then 2 chars for safety
          {
            cond = 1;
            break;
          }
        }
      }
      if (cond)
        break;
    }
    if (cond)
      break;

    // * free current iteration
    free(decodedSplitArray);
    free(decodedText);

    // * return pointer to start of the file
    fseek(input, 0, SEEK_SET);
    // if (keyInt == 0xFFFF)
    //   keyInt = 0x01000000;
    // else
    //   keyInt++;
  }

  if (cond)
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
  fprintf(stderr, "Time taken to calculate the key is %.2f seconds\n", difftime(end, start));
  MPI_Finalize();

  return 0;
} // * main