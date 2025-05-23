#ifndef _SFR_LIST_H
#define _SFR_LIST_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "Error.h"
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

enum ListIterPos {
  LIST_ITER_POS_NIL, // In limbo.
  LIST_ITER_POS_BEG, // Before the 'head' element.
  LIST_ITER_POS_INN, // Within collection of elements.
  LIST_ITER_POS_END, // After the 'tail' element.
};

typedef struct ListIter ListIter;
struct ListIter {
  List* list;
  enum ListIterPos at_pos;
  ListNode* cnode;
};

ListIter*   ListIter_new(List* list);
void        ListIter_del(ListIter* self);
ListIter*   ListIter_next(ListIter* self);
//ListIter* ListIter_prev(ListIter* self);
char        ListIter_at_beg(ListIter* self);
char        ListIter_at_end(ListIter* self);
ListIter*   ListIter_head(ListIter* self);
ListIter*   ListIter_tail(ListIter* self);
Object*     ListIter_get_val(ListIter* self);
/* char        ListIter_del_val(ListIter* self); */

ListNode* ListNode_new(Object* data);
void ListNode_unlink(ListNode* self);
void ListNode_del(ListNode* self);


List*     List_new();
void      List_del(List* self);
void      List_print(List* self);
char      List_zero(List* self);
List*     List_clone(List* self);
int       List_cmp(List* self, List* other);

size_t    List_size(List* self);
ssize_t   List_len(List* self);
ssize_t   List_push(List* self, Object* data);
Object*   List_head(List* self);
Object*   List_tail(List* self);
Object*   List_pop(List* self);
ssize_t   List_unshift(List* self, Object* data);
Object*   List_shift(List* self);
ssize_t   List_rem(List* self, Object* item);
/* Object*   List_first(List* self); */
/* Object*   List_last(List* self); */

/**
 * Get the nth element of the list.
 * zero element. 
 * AKA the nth function in Lisp.
 */
Object*   List_at(List* self, size_t idx);

#endif
