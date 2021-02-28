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
  unsigned int keyInt = MIN_VALUE;
  char *keyString;
  int numBytesInKey;
  char *inputfileText, *decodedText, **knowenWords, **decodedSplitArray;
  int knowenWordsCounter, decodedWordsCounter, cmpRes, givenLen, maxNum;
  int i, j, k, c;
  int cond = 0, comSize, procRank, startIndex, endIndex, partSize;
  time_t start, end;
  MPI_Status status;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &comSize);
  MPI_Comm_rank(MPI_COMM_WORLD, &procRank);

  start = time(NULL);

  if (procRank == 0)
  {
    // * open crypted file
    maxNum = determineMaxNum(argv[1], &givenLen);
    // fprintf(stderr, "\ngivenLen -> %d || maxNum 0x%x\n", givenLen, maxNum);
    partSize = maxNum / comSize;

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
    // openFiles(argv[2], argv[3], argc, input, knowenWordsFile);

    // * get number of words for words array dynamic memory allocation
    fscanf(knowenWordsFile, "%d", &knowenWordsCounter);

    // * allocate knowen words array and each of it's words
    inputfileText = inputString(knowenWordsFile, ALLOCATION_SIZE);
    knowenWords = splitStringByDelimiter(ALLOCATION_SIZE, inputfileText, "\n", &decodedWordsCounter);
  }

  startIndex = partSize * procRank;
  endIndex = maxNum - ((comSize - procRank - 1) * partSize);

#pragma omp parallel for collapse(3) private(i, j)
  for (k = startIndex; k < endIndex; k++)
  // while (keyInt <= maxNum)
  {
    // fprintf(stderr, "\nBefore creation, givenLen -> %d\n", givenLen);
    keyString = createKey(keyInt, 2 * givenLen);
    // fprintf(stderr, "\nkeyString ----> 0x%s\n", keyString);
    numBytesInKey = processKey(keyString);
    // fprintf(stderr, "numBytesInKey ---> %d\n", numBytesInKey);

    // * encode the text to a string
    decodedText = encodeToString(numBytesInKey, input);
    // fprintf(stderr, "decodedText ----> %s\n", decodedText);

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
    if (keyInt == 0xFFFF)
      keyInt = 0x01000000;
    else
      keyInt++;
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
  return 0;

  MPI_Finalize();
} // * main