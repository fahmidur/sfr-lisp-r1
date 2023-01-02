#ifndef __ATOM
#define __ATOM

#define ATOM_BUCKET_SIZE 2048;

typedef struct Atom Atom;
struct Atom {
  size_t buf_size;
  char* buf;
}

Atom* Atom_new(char* str);
void Atom_del(char* str);

#endif
