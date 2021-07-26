#include <calc/vector.h>

vector NewVector() {
    int   capacity = 1;
    int   index    = 0;
    void *items    = malloc(sizeof(void *));
    return (vector){.capacity = capacity, .index = index, .items = items};
}

void FreeVector(vector *vec) {
    free(vec->items);
    free(vec);
}

void ResizeVector(vector *vec, int capacity) {
    void **items = realloc(vec->items, capacity * sizeof(void *));
    if (items) {
        vec->items    = items;
        vec->capacity = capacity;
    }
}

void PushVector(vector *vec, void *item) {
    if (vec->index == vec->capacity) {
        ResizeVector(vec, vec->capacity * 2);
    }
    vec->items[vec->index++] = item;
}

void PopVector(vector *vec) {
    CHECK(vec->index > 0);
    vec->index -= 1;
}

void *GetVector(vector *vec, int index) {
    CHECK(index >= 0 && index <= vec->index);
    return vec->items[index];
}

void *BackVector(vector *vec) { return vec->items[vec->index - 1]; }

int EmptyVector(vector *vec) { return vec->index == 0; }

int Size(vector *vec) { return vec->index; }
