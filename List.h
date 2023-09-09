#ifndef _SFR_LIST_H
#define _SFR_LIST_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "Object.h"

typedef struct ListNode ListNode;
struct ListNode {
  ListNode* next;
  ListNode* prev;
  Object*   data;
};

typedef struct List List;
struct List {
  size_t size;
  ListNode* head;
  ListNode* tail;
};

ListNode* ListNode_new(Object* data);
void ListNode_unlink(ListNode* self);
void ListNode_del(ListNode* self);


List*     List_new();
void      List_del(List* self);
void      List_print(List* self);
List*     List_clone(List* self);
int       List_cmp(List* self, List* other);

size_t    List_size(List* self);
size_t    List_len(List* self);
size_t    List_push(List* self, Object* data);
Object*   List_pop(List* self);
size_t    List_unshift(List* self, Object* data);
Object*   List_shift(List* self);

/**
 * Get the nth element of the list.
 * zero element. 
 * AKA the nth function in Lisp.
 */
Object*   List_at(List* self, size_t idx);

#endif
