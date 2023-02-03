#ifndef __ATOM
#define __ATOM

#define ATOM_BUCKET_SIZE 2048;

// All Atoms are strings.
typedef struct Atom Atom;
struct Atom {
  size_t str_len;
  char* str;
}

Atom* Atom_find(char* str);
Atom* Atom_new(char* str);
void Atom_del(char* str);

#endif
