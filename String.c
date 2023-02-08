#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "Util.h"

struct String* String_new(char* istr) {
  struct String* self = malloc(sizeof(struct String));
  size_t ilen = strlen(istr);
  size_t i;
  self->buf_size = Util_sizet_max(String_IBUFSIZE, ilen+1);
  self->buf = calloc(self->buf_size, sizeof(char));
  for(i = 0; i < ilen; i++) {
    self->buf[i] = istr[i];
  }
  self->len = strlen(self->buf);
  return self;
}

void String_del(String* self) {
  if(self == NULL) {
    return;
  }
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
