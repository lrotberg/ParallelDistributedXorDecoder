#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <mpi.h>
#include <omp.h>
#include <limits.h>

#include "constants.h"
#include "functions.h"

int main(int argc, char **argv)
{
  FILE *input = stdin, *knowenWordsFile;
  unsigned int keyInt, startIndex, endIndex;
  char *keyString;
  int numBytesInKey;
  char *dictStr, *decodedText, **dict, **decodedSplitArray;
  char *encodedText;
  int dictStrLen, decodedWordsCounter, cmpRes, maxNum;
  int i, j;
  int matchCounter, comSize, procRank, numOfWords;
  int partSize, encodedTextLen;
  double start;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &comSize);
  MPI_Comm_rank(MPI_COMM_WORLD, &procRank);

  start = MPI_Wtime();

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
    fscanf(knowenWordsFile, "%d", &numOfWords);

    // * allocate knowen words array and each of it's words
    dictStr = inputString(knowenWordsFile, ALLOCATION_SIZE);
    dictStrLen = strlen(dictStr);
    encodedText = inputString(input, ALLOCATION_SIZE);
    encodedTextLen = strlen(encodedText);
    fclose(input);
    fclose(knowenWordsFile);
  }

  MPI_Bcast(&numOfWords, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&dictStrLen, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&encodedTextLen, 1, MPI_INT, 0, MPI_COMM_WORLD);
  if (procRank != 0)
  {
    dictStr = (char *)calloc(dictStrLen + 1, sizeof(char));
    encodedText = (char *)calloc(encodedTextLen + 1, sizeof(char));
  }
  MPI_Bcast(dictStr, dictStrLen, MPI_CHAR, 0, MPI_COMM_WORLD);

  dict = splitStringByDelimiter(ALLOCATION_SIZE, dictStr, "\n", &decodedWordsCounter);

  MPI_Bcast(encodedText, encodedTextLen, MPI_CHAR, 0, MPI_COMM_WORLD);

  maxNum = determineMaxNum(argv[1], &partSize);
  startIndex = partSize * procRank;
  endIndex = startIndex + partSize + 1;
  if (procRank == comSize - 1)
  {
    endIndex = maxNum;
  }

  for (keyInt = startIndex; keyInt < endIndex; keyInt++)
  {
    matchCounter = 0;
    keyString = createKey(keyInt);
    numBytesInKey = processKey(keyString);

    // * encode the text to a string
    decodedText = encodeStr(numBytesInKey, encodedText, encodedTextLen);

    // * split text string into a string array by 'space' delimiter
    decodedSplitArray = splitStringByDelimiter(ALLOCATION_SIZE, strdup(decodedText), " ", &decodedWordsCounter);

// * match all words of decoded text with each of the knowen words
#pragma omp parallel for collapse(2) private(j) num_threads(4)
    for (i = 0; i < decodedWordsCounter; i++)
    {
      for (j = 0; j < numOfWords; j++)
      {
        if (strlen(dict[j]) > 2)
        {
          cmpRes = strcmp(decodedSplitArray[i], dict[j]);
          if (cmpRes == 0) // * words match
          {
            matchCounter++;
          }
        }
      }
    }
    if (matchCounter >= 2)
      break;

    // * free current iteration
    free(decodedSplitArray);
    free(decodedText);
  }

  if (matchCounter >= 2)
  {
    printf("\nProcess %d - Success!\nKey is: 0x%s\nDecoded text is:\n%s\n", procRank, keyString, decodedText);

    free(decodedSplitArray);
    free(decodedText);
  }
  else
  {
    printf("\nProcess %d - Failure! No valid key was found\n", procRank);
  }

  // * clean all
  clean(dict, keyString, dictStr);

  if (procRank == 0)
  {
    fprintf(stderr, "Time taken to calculate the key is %f seconds\n", MPI_Wtime() - start);
  }
  MPI_Finalize();

  return 0;
} // * main