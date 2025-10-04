#include "nglist.h"

// 集合を初期化する関数
void initializeList(NGList *list) { list->size = 0; }

// 要素を集合に追加する関数
int addToList(NGList *list, int element) {
    if (list->size >= LIST_SIZE) {
        return 0;
    }

    // 集合に要素を追加
    list->elements[list->size++] = element;
    return 1;
}

int addToListAt(NGList *list, int element, int idx) {
    if (list->size >= LIST_SIZE) {
        return 0;
    }
    for (int i = idx; i < list->size; i++) {
        list->elements[i + i] = list->elements[i];
    }
    // 集合に要素を追加
    list->elements[idx] = element;
    list->size++;

    return 1;
}

int includeList(NGList *list, int element) {
    // 要素のインデックスを見つける
    for (int i = 0; i < list->size; i++) {
        if (list->elements[i] == element) {
            return i;
            break;
        }
    }

    return -1;
}

// 集合から要素を削除する関数
int removeFromList(NGList *list, int element) {
    int foundIndex = includeList(list, element);

    if (foundIndex == -1) {
        return 0;
    }

    // 要素を削除して集合を再構築
    for (int i = foundIndex; i < list->size - 1; i++) {
        list->elements[i] = list->elements[i + 1];
    }
    list->size--;
    return 1;
}

void copyList(NGList *a, NGList *b) {
    initializeList(b);
    for (int i = 0; i < a->size; i++) {
        addToList(b, a->elements[i]);
    }
}

// 集合から要素を削除する関数
int removeFromListAt(NGList *list, int idx) {
    // 要素を削除して集合を再構築
    for (int i = idx; i < list->size - 1; i++) {
        list->elements[i] = list->elements[i + 1];
    }
    list->size--;
    return 1;
}

int compareList0(NGList *list, int a) {
    if (list->elements[0] == a)
        return 1;
    else
        return 0;
}

int compareList01(NGList *list, int a, int b) {
    if ((list->elements[0] == a && list->elements[1] == b) ||
        (list->elements[0] == b && list->elements[1] == a))
        return 1;
    else
        return 0;
}
