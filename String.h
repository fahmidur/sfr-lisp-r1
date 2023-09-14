#ifndef _SFR_STRING_H
#define _SFR_STRING_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Util.h"

#define STRING_RETURN_MEMORY 1
#define String_IBUFSIZE 4
#define String_MBUFSIZE 8

typedef struct String String;
struct String {
  size_t len;
  size_t buf_size;
  char* buf;
};

String* String_new(char* buf);
String* String_clone(String* self);
String* String_add(String* a, String* b);
String* String_addx(String* self, String* other);
String* String_addx_char(String* self, char ch);
ssize_t String_getline(struct String* self, FILE *stream);
void String_zero(String* self);
void String_del(String* self);
void String_chomp(String* self);
size_t String_len(String* self);
int String_cmp(String* self, String* other);
void String_print(String* self);
size_t String_hash(String* self);
char String_charat(String* self, size_t idx);
char String_charat_unsafe(String* self, size_t idx);

// TODO
double String_to_double(String* self);
float  String_to_float(String* self);
long   String_to_long(String* self);
int    String_to_int(String* self);
char   String_to_char(String* self);

#endif
