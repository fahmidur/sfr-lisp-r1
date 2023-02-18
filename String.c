#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "Util.h"

String* String_new(char* istr) {
  printf("String_new(%s)\n", istr);
  String* self = calloc(1, sizeof(String));
  size_t ilen = strlen(istr);
  size_t i;
  self->buf_size = Util_sizet_max(String_IBUFSIZE, ilen+1);
  self->buf = calloc(self->buf_size, sizeof(char));
  for(i = 0; i < ilen; i++) {
    self->buf[i] = istr[i];
  }
  self->len = strlen(self->buf);
  printf("String_new. p=%p || buf=%s\n", self, self->buf);
  return self;
}

String* String_clone(String* self) {
  String* clone = calloc(1, sizeof(String));
  clone->len = self->len;
  clone->buf_size = self->buf_size;
  clone->buf = calloc(clone->buf_size, sizeof(char));
  int i;
  int ilen = self->len;
  for(i = 0; i < ilen; i++) {
    clone->buf[i] = self->buf[i];
  }
  return clone;
}

void String_del(String* self) {
  if(self == NULL) {
    return;
  }
  printf("String_del. p=%p || buf=%s\n", self, self->buf);
  if(self->buf != NULL) {
    free(self->buf);
  }
  free(self);
}

void String_zero(String* self) {
  memset(self->buf, 0, self->buf_size);
  #ifdef STRING_RETURN_MEMORY
    if(self->buf_size > String_MBUFSIZE) {
      self->buf = realloc(self->buf, String_IBUFSIZE);
      self->buf_size = String_IBUFSIZE;
    }
  #endif
  String_len(self);
}

ssize_t String_getline(String* self, FILE *stream) {
  String_zero(self);
  ssize_t ret = getline(&self->buf, &(self->buf_size), stream);
  printf("String_getline. ret=%ld buf_size=%ld\n", ret, self->buf_size);
  String_len(self);
  return ret;
}

void String_chomp(String* self) {
  char ch;
  for(size_t i = self->buf_size-1; i >= 0; i--) {
    ch = self->buf[i];
    if(ch == '\0') { 
      continue;
    }
    if(ch == '\n' || ch == '\r') {
      self->buf[i] = '\0';
      break;
    } 
    else {
      break;
    }
  }
  String_len(self);
}

size_t String_len(String* self) {
  self->len = strlen(self->buf);
  return self->len;
}

int String_cmp(String* self, String* other) {
  return strcmp(self->buf, other->buf);
}

void String_print(String* self) {
  printf("String(%s)", self->buf);
}
