#pragma once

#define KEY_SIZE 8
#define ALLOCATION_SIZE 10

enum
{
  MIN_VALUE = 0x1,
  SM_MAX_VALUE = 0xFF,
  MD_MAX_VALUE = 0xFFFF,
  LG_MAX_VALUE = 0xFFFFFFFF
};

char keyBytes[KEY_SIZE];