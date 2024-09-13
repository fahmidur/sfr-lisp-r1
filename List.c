#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "List.h"
#include "Object.h"

ListNode* ListNode_new(Object* data) {
  ListNode* self = calloc(1, sizeof(ListNode));
  self->data = data;
  self->next = NULL;
  self->prev = NULL;
  return self;
}

void ListNode_unlink(ListNode* self) {
  ListNode* prev = self->prev;
  ListNode* next = self->next;
  if(prev != NULL) {
    prev->next = next;
    self->prev = NULL;
  }
  if(next != NULL) {
    next->prev = prev;
    self->next = NULL;
  }
}

int ListNode_cmp(ListNode* a, ListNode* b) {
  assert(a != NULL);
  assert(b != NULL);
  return Object_cmp(a->data, b->data);
}

void ListNode_del(ListNode* self) {
  ListNode_unlink(self);
  if(Object_system_delete_recurse()) {
    self->data = Object_rc_decr(self->data);
  } else {
    self->data = NULL;
  }
  free(self);
}

List* List_new() {
  List* self = calloc(1, sizeof(List));
  self->head = NULL;
  self->tail = NULL;
  self->size = 0;
  return self;
}

char List_zero(List* self) {
  /* assert(self != NULL); */
  if(self == NULL) {
    ErrorSystem_set(1, "List_zero. self is NULL");
    return 0;
  }
  while(List_len(self) > 0) {
    Object_reject(List_pop(self));
  }
  return 1;
}

size_t List_size(List* self) {
  return self->size;
}

ssize_t List_len(List* self) {
  return self->size;
}

ssize_t List_push(List* self, Object* obj) {
  assert(self != NULL);
  assert(obj != NULL);
  assert(obj->impl != self);
  // TODO: We cannot rely on assert for
  // error checking. How do we signal to the caller that this failed?
  // TODO: we have to check that a Container incoming Object
  // does not contain any reference to self.
  ListNode* listnode = ListNode_new(obj);
  if(self->size == 0) {
    self->head = listnode;
    self->tail = listnode;
  } 
  else {
    self->tail->next = listnode;
    listnode->prev = self->tail;
    self->tail = listnode;
  }
  /*Object_rc_incr(obj);*/
  Object_accept(obj);
  self->size++;
  return self->size;
}

Object* List_pop(List* self) {
  printf("List_pop(%p).\n", self);
  if(self->size == 0) {
    return Object_new_null();
  }
  ListNode* node;
  ListNode* new_tail;
  Object* ret;
  if(self->size == 1) {
    assert(self->tail == self->head);
    node = self->tail;
    self->head = NULL;
    self->tail = NULL;
  }
  else {
    node = self->tail;
    new_tail = node->prev;
    new_tail->next = NULL; node->prev = NULL;
    self->tail = new_tail;
    assert(node->next == NULL);
  }
  assert(node != NULL);
  // detach node
  node->prev = NULL; node->next = NULL;
  self->size--;
  if(Object_system_delete_recurse()) {
    ret = Object_return(node->data);
  } else {
    ret = NULL;
  }
  ListNode_del(node);
  printf("List_pop(%p). size=%zu\n", self, self->size);
  return ret;
}

ssize_t List_unshift(List* self, Object* obj) {
  assert(self != NULL);
  assert(obj != NULL);
  assert(obj->impl != self);
  ListNode* listnode = ListNode_new(obj);
  if(self->size == 0) {
    self->head = listnode;
    self->tail = listnode;
  }
  else {
    self->head->prev = listnode;
    listnode->next = self->head;
    self->head = listnode;
  }
  Object_rc_incr(obj);
  self->size++;
  return self->size;
}

Object* List_shift(List* self) {
  printf("List_shift(%p).\n", self);
  if(self->size == 0) {
    return NULL;
  }
  ListNode* node;
  ListNode* new_head;
  Object* ret;
  if(self->size == 1) {
    assert(self->head == self->tail);
    node = self->head;
    self->head = NULL;
    self->tail = NULL;
  }
  else {
    node = self->head;
    new_head = node->next;
    new_head->prev = NULL; node->next = NULL;
    self->head = new_head;
    assert(node->prev == NULL);
  }
  assert(node != NULL);
  // detach node
  node->prev = NULL; node->next = NULL;
  self->size--;
  ret = Object_return(node->data);
  ListNode_del(node);
  return ret;
}

Object* List_at(List* self, size_t idx) {
  if(self->size == 0) {
    return NULL;
  }
  if(idx >= self->size) {
    // out of bounds.
    return NULL;
  }
  ListNode* iter = self->head;
  size_t i = 0;
  while(iter != NULL) {
    if(i == idx) {
      return Object_return(iter->data);
    }
    iter = iter->next;
    i++;
  }
  // TODO we iterate from the head forward
  // or the tail backward depending on whether
  // idx is closer to the head or the tail. 
  return NULL;
}

void List_del(List* self) {
  printf("{ List_del(%p) {\n", self);
  Object* tmp;

  while(self->size > 0) {
    tmp = List_pop(self);
    // tmp is now damaged
    printf("List_del(%p). dbg. 001 tmp=%p | Object_reject(tmp)\n", self, tmp);
    // below is unsafe, because tmp may no longer exist
    if(Object_system_delete_recurse()) {
      Object_reject(tmp);
    }
    printf("List_del(%p). dbg. 002\n", self);
  }

  printf("List_del(%p). free(self)\n", self);
  free(self);
  printf("} List_del(%p) }\n", self);
}

void List_print(List* self) {
  ListNode* iter = self->head;
  printf("List(");
  int i = 0;
  while(iter != NULL) {
    if(i > 0) {
      printf(", ");
    }
    Object_print(iter->data);
    iter = iter->next;
    i++;
  }
  printf(")");
}

List* List_clone(List* self) {
  List* clone = List_new();
  ListNode* iter = self->head;
  while(iter != NULL) {
    /*List_push(clone, Object_accept(Object_clone(iter->data)));*/
    List_push(clone, Object_clone(iter->data));
  }
  return clone;
}

int List_cmp(List* self, List* other) {
  if(self->size != other->size) {
    return 1; 
  }
  ListNode* a = self->head;
  ListNode* b = other->head;
  int tcmp;
  while(a != NULL && b != NULL) {
    tcmp = ListNode_cmp(a, b);
    if(tcmp != 0) {
      return tcmp;
    }
    a = a->next;
    b = b->next;
  }
  return 0;
}

// Remove all items based on value
ssize_t List_rem(List* self, Object* item) {
  assert(self != NULL);
  assert(item != NULL);
  Object_accept(item);
  if(self->size == 0) {
    goto _return;
  }
  ListNode* prev = NULL;
  ListNode* next = NULL;
  ListNode* node = self->head;
  while(node != NULL) {
    if(Object_cmp(node->data, item) == 0) {
      next = node->next;
      prev = node->prev;
      // we have a match
      if(node == self->head) {
        // special head position
        self->head = node->next;
      }
      else
      if(node == self->tail) {
        // special tail position
        self->tail = node->prev;
      }
      ListNode_del(node);
      self->size--;
      node = next;
    } 
    else {
      node = node->next;
    }
  }
_return:
  Object_rc_decr(item);
  return 0;
}

ListIter* ListIter_new(List* list) {
  ListIter* self = calloc(1, sizeof(ListIter));
  self->list = list;
  self->at_pos = LIST_ITER_POS_NIL;
  self->cnode = NULL;
  return self;
}

ListIter* ListIter_next(ListIter* self) {
  if(self->at_pos == LIST_ITER_POS_NIL) {
    self->at_pos = LIST_ITER_POS_BEG;
  }
  List* list = self->list;
  if(list->size == 0) {
    self->at_pos = LIST_ITER_POS_END;
  }
  else
  if(self->at_pos == LIST_ITER_POS_BEG) {
    self->cnode = list->head;
    self->at_pos = LIST_ITER_POS_INN;
  }
  else
  if(self->at_pos == LIST_ITER_POS_INN) {
    if(self->cnode->next != NULL) {
      self->cnode = self->cnode->next;
    }
    else {
      self->cnode = NULL;
      self->at_pos = LIST_ITER_POS_END;
    }
  }
  return self;
}

ListIter* ListIter_head(ListIter* self) {
  List* list = self->list;
  if(list->size == 0) {
    return self;
  }
  self->cnode = list->head;
  self->at_pos = LIST_ITER_POS_INN;
  return self;
}

ListIter* ListIter_tail(ListIter* self) {
  List* list = self->list;
  if(list->size == 0) {
    return self;
  }
  self->cnode = list->tail;
  self->at_pos = LIST_ITER_POS_INN;
  return self;
}

Object* ListIter_get_val(ListIter* self) {
  assert(self != NULL);
  assert(self->cnode != NULL);
  return self->cnode->data;
}

void ListIter_del(ListIter* self) {
  if(self == NULL) {
    return;
  }
  free(self);
}

char ListIter_at_beg(ListIter* self) {
  return self->at_pos == LIST_ITER_POS_BEG;
}

char ListIter_at_end(ListIter* self) {
  return self->at_pos == LIST_ITER_POS_END;
}
