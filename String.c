#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"

struct String*  String_new(char* str) {
  struct String * ds = malloc(sizeof(struct String));
  ds->buf_strlen = 0;
  ds->buf = malloc(sizeof(char) * String_IBUFSIZE);
  ds->buf_size = String_IBUFSIZE;
  memset(ds->buf, '\0', ds->buf_size);
  return ds;
}

void String_del(struct String** dspp) {
  struct String * dsp = *dspp;
  if(dsp != NULL) {
    if(dsp->buf != NULL) {
      free(dsp->buf);
    }
    free(dsp);
    *dspp = NULL;
  }
}

void String_zero(struct String* self) {
  memset(self->buf, 0, self->buf_size);
  #ifdef STRING_RETURN_MEMORY
    if(self->buf_size > String_MBUFSIZE) {
      self->buf = realloc(self->buf, String_IBUFSIZE);
      self->buf_size = String_IBUFSIZE;
    }
  #endif
}

ssize_t String_getline(struct String * self, FILE *stream) {
  String_zero(self);
  ssize_t ret = getline(&self->buf, &(self->buf_size), stream);
  printf("String_getline. ret=%ld buf_size=%ld\n", ret, self->buf_size);
  return ret;
}

void String_chomp(struct String * self) {
  char ch;
  for(size_t i = self->buf_size-1; i >= 0; i--) {
    ch = self->buf[i];
    if(ch == '\0') { 
      continue;
    }
    if(ch == '\n' || ch == '\r') {
      self->buf[i] = '\0';
    } 
    else {
      break;
    }
  }
}
