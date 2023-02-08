#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "Util.h"

struct String*  String_new(char* istr) {
  struct String* ds = malloc(sizeof(struct String));
  size_t ilen = strlen(istr);
  size_t i;
  ds->buf_size = Util_sizet_max(String_IBUFSIZE, ilen+1);
  ds->buf = calloc(ds->buf_size, sizeof(char));
  /*memset(ds->buf, '\0', ds->buf_size);*/
  for(i = 0; i < ilen; i++) {
    ds->buf[i] = istr[i];
  }
  return ds;
}

void String_del(struct String* self) {
  if(self == NULL) {
    return;
  }
  if(self->buf != NULL) {
    free(self->buf);
  }
  free(self);
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
