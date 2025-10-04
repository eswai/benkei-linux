#pragma once
#define LIST_SIZE 5 // 集合の最大サイズ

typedef struct {
    int elements[LIST_SIZE];
    int size;
} NGList;

void initializeList(NGList *);
int addToList(NGList *, int);
int addToListAt(NGList *, int, int);
int includeList(NGList *, int);
int removeFromList(NGList *, int);
int removeFromListAt(NGList *, int);
void copyList(NGList *, NGList *);

int compareList0(NGList *, int);
int compareList01(NGList *, int, int);
