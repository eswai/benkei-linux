#include "nglistarray.h"

// 集合を初期化する関数
void initializeListArray(NGListArray *list) { list->size = 0; }

// 要素を集合に追加する関数
int addToListArray(NGListArray *list, NGList *element) {
    if (list->size >= LIST_SIZE) {
        return 0;
    }

    // 集合に要素を追加
    list->elements[list->size++] = *element;
    return 1;
}

int addToListArrayAt(NGListArray *list, NGList *element, int idx) {
    if (list->size >= LIST_SIZE) {
        return 0;
    }
    for (int i = idx; i < list->size; i++) {
        list->elements[i + i] = list->elements[i];
    }
    // 集合に要素を追加
    list->elements[idx] = *element;
    list->size++;

    return 1;
}

// 集合から要素を削除する関数
int removeFromListArrayAt(NGListArray *list, int idx) {
    // 要素を削除して集合を再構築
    if (list->size > 0) {
        for (int i = idx; i < list->size - 1; i++) {
            list->elements[i] = list->elements[i + 1];
        }
        list->size--;
        return 1;
    } else {
        return 0;
    }
}
