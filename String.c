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

/**
 * Convert this string to a long.
 */
long String_to_long(String* self) {
  char* endptr;
  // TODO: infer base based on format of string
  long ret = strtol(self->buf, &endptr, 10);
  if(*endptr != '\0') {
    return 0;
  }
  return ret;
}

/**
 * Convert this string to a long with a given base.
 */
long String_to_long_b(String* self, int base) {
  char* endptr;
  // TODO: infer base based on format of string
  long ret = strtol(self->buf, &endptr, base);
  if(*endptr != '\0') {
    return 0;
  }
  return ret;
}

int String_to_int(String* self) {
  return (int) String_to_long(self);
}

/**
 * Convert this string to double using strtod.
 */
double String_to_double(String* self) {
  char* endptr;
  double ret = strtod(self->buf, &endptr);
  if(*endptr != '\0') {
    return 0;
  }
  return ret;
}

/**
 * Convert this string to a float.
 */
float String_to_float(String* self) {
  char* endptr;
  double ret = strtof(self->buf, &endptr);
  if(*endptr != '\0') {
    return 0;
  }
  return ret;
}

/**
 * Convert string to char.
 * It only makes sense to do this to strings of length 1.
 */
char String_to_char(String* self) {
  return String_charat(self, 0);
}

char String_charat(String* self, size_t idx) {
  if(idx < 0 || idx >= self->len) {
    return '\0'; 
  }
  return self->buf[idx];
}

char String_charat_unsafe(String* self, size_t idx) {
  return self->buf[idx];
}

size_t String_hash(String* self) {
  return Util_cstr_hash_djb2(self->buf);
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

/**
 * Add two strings a and b, returning a new String
 */
String* String_add(String* a, String* b) {
  String* clone = calloc(1, sizeof(String));
  clone->len = a->len + b->len;
  clone->buf_size = a->buf_size + b->buf_size;
  clone->buf = calloc(clone->buf_size, sizeof(char));
  int i;
  int alen = a->len;
  int blen = b->len;
  for(i = 0; i < alen; i++) {
    clone->buf[i] = a->buf[i];
  }
  for(i = 0; i < blen; i++) {
    clone->buf[alen+i] = b->buf[i];
  }
  return clone;
}

/**
 * Add a string to this string, modifying this string.
 */
String* String_addx(String* self, String* other) {
  size_t new_len = self->len + other->len;
  size_t new_buf_size = 2 * new_len;
  self->buf = realloc(self->buf, new_buf_size);
  if(self->buf == NULL) {
    return NULL;
  }
  self->buf_size = new_buf_size;
  size_t i;
  size_t self_len = self->len;
  for(i = self_len; i < new_buf_size; i++) {
    self->buf[i] = '\0';
  }
  size_t other_len = other->len;
  for(i = 0; i < other_len; i++) {
    self->buf[i+self_len] = other->buf[i];
  }
  self->buf[new_buf_size - 1] = '\0';
  self->len = new_len;
  return self;
}

/**
 * Add a single character to the end of this string.
 */
String* String_addx_char(String* self, char ch) {
  size_t new_len = self->len + 1;
  size_t new_buf_size = 2 * new_len;
  self->buf = realloc(self->buf, new_buf_size);
  if(self->buf == NULL) {
    return NULL;
  }
  self->buf_size = new_buf_size;
  size_t i;
  size_t self_len = self->len;
  for(i = self_len; i < new_buf_size; i++) {
    self->buf[i] = '\0';
  }
  self->buf[new_len - 1] = ch;
  self->buf[new_buf_size - 1] = '\0';
  self->len = new_len;
  return self;
}

void String_del(String* self) {
  if(self == NULL) {
    return;
  }
  printf("String_del(%p). buf=%s\n", self, self->buf);
  if(self->buf != NULL) {
    free(self->buf);
    self->buf = NULL;
  }
  free(self);
}

/**
 * Reset this string to the zero state --
 * the empty string.
 */
void String_zero(String* self) {
  memset(self->buf, 0, self->buf_size);
  self->len = 0;
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
