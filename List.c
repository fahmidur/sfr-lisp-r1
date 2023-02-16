#include <stdio.h>
#include <stdlib.h>
#include "List.h"

ListNode* ListNode_new(void* data) {
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
  free(self);
}

List* List_new() {
  List* self = calloc(1, sizeof(List));
  self->head = NULL;
  self->tail = NULL;
  self->size = 0;
  return self;
}

size_t List_push(List* self, void* data) {
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

void List_del(List* self) {
  ListNode* iter = self->head;
  while(iter != NULL) {
    ListNode_del(iter);
    iter = iter->next;
  }
  self->head = NULL;
  self->tail = NULL;
  free(self);
}
