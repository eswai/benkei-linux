#pragma once
#include "nglist.h"

#define LIST_SIZE 5 // 集合の最大サイズ

typedef struct {
    NGList elements[LIST_SIZE];
    int size;
} NGListArray;

void initializeListArray(NGListArray *);
int addToListArray(NGListArray *, NGList *);
int addToListArrayAt(NGListArray *, NGList *, int);
int includeListArray(NGListArray *, NGList *);
int removeFromListArrayAt(NGListArray *, int);
