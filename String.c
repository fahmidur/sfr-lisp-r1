#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "Util.h"
#include "Error.h"

int     StringIO_state = 0;
char*   StringIO_buf = NULL;
int     StringIO_buf_size = 0;
int     StringIO_len = 0;
char    StringIO_buf_kb13 = 0;

String* String_new(char* istr) {
  /* dbg_printf("String_new(%s)\n", istr); */
  String* self = calloc(1, sizeof(String));
  if(self == NULL) {
    ErrorSystem_set(1, "String_new. calloc failed");
    return NULL;
  }
  size_t ilen = strlen(istr);
  size_t i;
  self->buf_size = Util_sizet_max(String_IBUFSIZE, ilen+1);
  self->buf = calloc(self->buf_size, sizeof(char));
  if(self->buf == NULL) {
    ErrorSystem_set(1, "String_new. calloc failed on self->buf");
    free(self);
    return NULL;
  }
  for(i = 0; i < ilen; i++) {
    self->buf[i] = istr[i];
  }
  self->len = strlen(self->buf);
  /* dbg_printf("String_new. p=%p || buf=%s\n", self, self->buf); */
  return self;
}

/**
 * Convert this string to a long.
 */
long String_to_long(String* self) {
  char* endptr;
  // TODO: infer base based on format of string
  long ret = strtol(self->buf, &endptr, 10);
  /*if(*endptr != '\0') {*/
    /*return 0;*/
  /*}*/
  return ret;
}

/**
 * Convert this string to a long with a given base.
 */
long String_to_long_b(String* self, int base) {
  char* endptr;
  // TODO: infer base based on format of string
  long ret = strtol(self->buf, &endptr, base);
  /*if(*endptr != '\0') {*/
    /*return 0;*/
  /*}*/
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
  /*if(*endptr != '\0') {*/
    /*return 0;*/
  /*}*/
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

String* String_addx_cstr(String* self, char* cstr) {
  ssize_t cstr_len = strlen(cstr);
  int i = 0;
  for(i = 0; i < cstr_len; i++) {
    String_addx_char(self, cstr[i]);
  }
  return self;
}

void String_del(String* self) {
  if(self == NULL) {
    return;
  }
  dbg_printf("String_del(%p). buf=%s\n", self, self->buf);
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
char String_zero(String* self) {
  if(self == NULL) {
    ErrorSystem_set(1, "String_zero. self is null");
    return 0;
  }
  char ret = 1;
  memset(self->buf, 0, self->buf_size);
  self->len = 0;
  #ifdef STRING_RETURN_MEMORY
    if(self->buf_size > String_MBUFSIZE) {
      self->buf = realloc(self->buf, String_IBUFSIZE);
      if(self->buf == NULL) {
        // the reallocation failed
        ret = 0;
        ErrorSystem_set(1, "realloc failed");
      }
      self->buf_size = String_IBUFSIZE;
    }
  #endif
  // recomputes the length
  String_len(self);
  return ret;
}

ssize_t String_getline(String* self, FILE* stream) {
  String_zero(self);
  ssize_t ret = 0;
  /* if(stream == stdin) { */
  /*   #ifdef WASM */
  /*     ret = StringIO_getline(&(self->buf), &(self->buf_size)); */
  /*   #else */
  /*     ret = getline(&(self->buf), &(self->buf_size), stream); */
  /*   #endif */
  /* } */ 
  /* else { */
    ret = getline(&(self->buf), &(self->buf_size), stream);
  /* } */
  /* dbg_printf("String_getline. ret=%ld buf_size=%ld\n", ret, self->buf_size); */
  String_len(self);
  return ret;
}

/**
 * Remove the last newline in the string if there is one
 **/
void String_chomp(String* self) {
  if(String_len(self) == 0) {
    return;
  }
  char ch;
  for(ssize_t i = (self->len-1); i >= 0; i--) {
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

ssize_t String_len(String* self) {
  self->len = strlen(self->buf);
  return self->len;
}

int String_cmp(String* self, String* other) {
  return strcmp(self->buf, other->buf);
}

void String_print_quoted(String* self) {
  Util_vt_set(VT_COLOR_CYAN_FG);
  Util_cstr_print(self->buf);
  Util_vt_set(VT_RESET);
}

void String_print(String* self) {
  /*dbg_printf("String(%s)", self->buf);*/
  printf("String(");
  Util_vt_set(VT_COLOR_CYAN_FG);
  Util_cstr_print(self->buf);
  Util_vt_set(VT_RESET);
  printf(")");
  /*char bchar = 92; // The backslash character*/
  /*char wchar = '"';*/
  /*dbg_printf("String(%c", wchar);*/
  /*size_t str_len = strlen(self->buf);*/
  /*size_t i;*/
  /*char ch;*/
  /*for(i = 0; i < str_len; i++) {*/
    /*ch = self->buf[i];*/
    /*if(ch == wchar) {*/
      /*putc(bchar, stdout);*/
    /*} */
    /*if(ch == bchar) {*/
      /*putc(bchar, stdout);*/
    /*}*/
    /*putc(ch, stdout);*/
  /*}*/
  /*dbg_printf("%c)", wchar);*/
}

char* String_cstr(String* self) {
  return self->buf;
}

int* StringIO_init() {
  StringIO_state = 0;
  StringIO_buf_size = 128;
  StringIO_buf = calloc(StringIO_buf_size, 1);
  /* printf("StringIO_init(). StringIO_buf = %p\n", StringIO_buf); */
  StringIO_len = 0;
  return &StringIO_state;
}

void StringIO_state_set(int new_state) {
  StringIO_state = new_state;
}

int StringIO_push(char ch) {
  if(StringIO_buf_kb13) {
    printf("ERROR: getline buf is full\n");
    return -1;
  }
#ifdef WASM
  printf("%c", ch);
  fflush(stdout);
#endif
  int new_len = StringIO_len+1;
  if(new_len >= StringIO_buf_size) {
    StringIO_buf = realloc(StringIO_buf, 2*StringIO_buf_size);
    if(StringIO_buf == NULL) {
      printf("ERROR: realloc failure in StringIO_push\n");
      fflush(stdout);
      return -1;
    }
    StringIO_buf_size = 2*StringIO_buf_size;
    int i;
    for(i = StringIO_len; i < StringIO_buf_size; i++) {
      StringIO_buf[i] = '\0';
    }
  }
  int ret = 0;
  StringIO_buf[StringIO_len++] = ch;
  if(ch == '\n' || ch == '\r') { // newline
    printf("\n--- got newline ---\n");
    printf("\nbuf=|%s|\n", StringIO_buf);
    fflush(stdout);
    StringIO_buf_kb13 = 1;
    ret = 1;
  } else {
    ret = 0;
  }
  return ret;
}

char StringIO_getline_ready() {
  return StringIO_buf_kb13;
}

void StringIO_set(char* buf, int buf_size) {
  StringIO_buf = buf;
  StringIO_buf_size = buf_size;
  StringIO_len = strlen(StringIO_buf);
}

void StringIO_reset() {
  memset(StringIO_buf, '\0', StringIO_buf_size);
  StringIO_len = 0;
  StringIO_buf_kb13 = 0;
  StringIO_state = 0;
}

/* ssize_t StringIO_getline(char** buf_ptr, size_t* buf_size_ptr) { */
/*   ssize_t ret = 0; */
/*   int i = 0; */
/*   while(1) { */
/*     if(StringIO_getline_ready()) { */
/*       printf("\n --- stringio getline ready ---\n"); fflush(stdout); */
/*       *buf_ptr = realloc(*buf_ptr, StringIO_buf_size); */
/*       *buf_size_ptr = StringIO_buf_size; */
/*       for(i = 0; i < StringIO_buf_size; i++) { */
/*         (*buf_ptr)[i] = StringIO_buf[i]; */
/*       } */
/*       ret = strlen(StringIO_buf); */
/*       StringIO_reset(); */
/*       break; */
/*     } else { */
/*       /1* printf("."); fflush(stdout); *1/ */
/*     } */
/*   } */
/*   return ret; */
/* } */

char* StringIO_get_buf() {
  /* printf("StringIO_get_buf(). StringIO_buf = %p\n", StringIO_buf); */
  return StringIO_buf;
}

int StringIO_get_buf_len() {
  if(StringIO_buf == NULL) {
    printf("StringIO_get_buf_len(). StringIO_buf = %p\n", StringIO_buf);
    printf("ERROR: StringIO_buf is NULL\n");
    fflush(stdout);
    return 0;
  }
  return strlen(StringIO_buf);
}

void StringIO_done() {
  if(StringIO_buf != NULL) {
    free(StringIO_buf);
    StringIO_buf = NULL;
  }
}
