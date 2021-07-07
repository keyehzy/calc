#pragma once 

#include <assert.h>
#define CHECK assert
#define CHECK_NOT_REACHED() assert(0)
#define TODO CHECK_NOT_REACHED
