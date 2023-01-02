#ifndef _SFR_STRING_H
#define _SFR_STRING_H

#define STRING_RETURN_MEMORY 1
#define String_IBUFSIZE 4
#define String_MBUFSIZE 8


typedef struct String String;
struct String {
  size_t buf_size;
  char* buf;
  unsigned int buf_strlen;
};

String* String_new(char* buf);
ssize_t String_getline(struct String* self, FILE *stream);
void String_zero(struct String* self);
void String_del(struct String** dspp);
void String_chomp(struct String * self);

#endif
