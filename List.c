#include <stdio.h>
#include <stdlib.h>
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

void ListNode_del(ListNode* self) {
  ListNode_unlink(self);
  // TODO: either delete the data or 
  // decrement the refcount for data.
  free(self);
}

List* List_new() {
  List* self = calloc(1, sizeof(List));
  self->head = NULL;
  self->tail = NULL;
  self->size = 0;
  return self;
}

size_t List_size(List* self) {
  return self->size;
}

size_t List_push(List* self, Object* data) {
  ListNode* datanode = ListNode_new(data);
  if(self->size == 0) {
    self->head = datanode;
    self->tail = datanode;
  } 
  else {
    self->tail->next = datanode;
  }
  return self->size++;
};

Object* List_pop(List* self) {
  if(self->size == 0) {
    return NULL;
  }
  ListNode* node;
  void* ret;
  if(self->size == 1) {
    node = self->tail;
    self->head = NULL;
    self->tail = NULL;
  }
  else {
    node = self->tail;
    self->tail = node->prev;
  }
  node->prev = NULL;
  node->next = NULL;
  self->size--;
  ret = node->data;
  ListNode_del(node);
  return ret;
}

void List_del(List* self) {
  while(self->size > 0) {
    List_pop(self);
  }
  free(self);
}
