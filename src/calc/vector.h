#pragma once

#include <calc/assert.h>
#include <stdlib.h>

typedef struct {
  int capacity;
  int index;
  void **items;
} vector;

vector NewVector();
void FreeVector(vector *vec);
void ResizeVector(vector *vec, int size);
void PushVector(vector *vec, void *item);
void PopVector(vector *vec);
void *GetVector(vector *vec, int index);
void *BackVector(vector *vec);
int EmptyVector(vector *vec);
int Size(vector *vec);
