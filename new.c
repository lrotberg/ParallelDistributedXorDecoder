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
  int dictStrLen, decodedWordsCounter, cmpRes, givenLen, maxNum;
  int i, j, c;
  int matchCounter, comSize, procRank, numOfWords;
  int partSize, encodedTextLen;
  double start, end;
  MPI_Status status;

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

  //maxNum = determineMaxNum(argv[1], &givenLen, &partSize);
  partSize = UINT_MAX / comSize;
  if (procRank == comSize - 1)
  {
    partSize += UINT_MAX % comSize;
  }
  startIndex = partSize * procRank;
  endIndex = startIndex + partSize + 1;

  // fprintf(stderr, "\npartSize %d rank %d startIndex 0x%x endIndex 0x%x\n", partSize, procRank, startIndex, endIndex);

  for (keyInt = startIndex; keyInt < endIndex; keyInt++)
  {
    matchCounter = 0;
    keyString = createKey(keyInt);
    // fprintf(stderr, "process %d keyString %s\n", procRank, keyString);
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
        // fprintf(stderr, "dict[%d] = %s\n", j, dict[j]);
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
    // fprintf(stderr, "proc %d before free 1\n", procRank);
    free(decodedSplitArray);
    free(decodedText);
  }

  if (matchCounter >= 2)
  {
    fprintf(stderr, "\nProcess %d - Success!\nKey is: 0x%s\nDecoded text is:\n%s\n\n", procRank, keyString, decodedText);

    // fprintf(stderr, "proc %d before free 2 success\n", procRank);
    free(decodedSplitArray);
    free(decodedText);

    // MPI_Abort(MPI_COMM_WORLD, 0);
  }
  else
  {
    fprintf(stderr, "\nProcess %d - Failure! No valid key was found\n", procRank);
  }

  // * clean all
  // fprintf(stderr, "proc %d before clean\n", procRank);
  clean(dict, keyString, dictStr);

  // end = time(NULL);
  if (procRank == 0)
  {
    fprintf(stderr, "Time taken to calculate the key is %f seconds\n", MPI_Wtime() - start);
  }
  MPI_Finalize();

  return 0;
} // * main