#ifndef _SFR_LIST
#define _SFR_LIST

#include <stdio.h>
#include <stdlib.h>

typedef struct ListNode ListNode;
struct ListNode {
  ListNode* next;
  ListNode* prev;
  void* data;
};

typedef struct List List;
struct List {
  size_t size;
  ListNode* head;
  ListNode* tail;
};

List* List_new();
void* List_del(List* self);

size_t List_push(List* self, void* data);
void*  List_pop(List* self);

size_t List_unshift(List* self, void* data);
void*  List_shift(List* self);

#endif
