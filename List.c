#include <stdio.h>
#include <stdlib.h>
#include "Object.h"
#include "List.h"

ListNode* ListNode_new(Object* data) {
  ListNode* self = malloc(sizeof(ListNode));
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
  Object_del(self->data);

}

List* List_new() {
  List* self = calloc(1, sizeof(List));
  self->head = NULL;
  self->tail = NULL;
  self->size = 0;
  return self;
}

size_t List_push(List* self, Object* data) {
  ListNode* datanode = ListNode_new(data);
};
