#ifndef __ATOM
#define __ATOM

#define ATOM_BUCKET_SIZE 2048;
#define ATOMVECTOR_ISIZE = 1;

void Atom_system_init();
void Atom_system_done();

typedef struct Atom Atom;
struct Atom {
  size_t str_len;
  char* str;
}

typedef struct AtomVector AtomVector;
struct AtomVector {
  size_t size;
  size_t lidx;
  Atom** list;
}

AtomVector* AtomVector_new();
void AtomVector_push(AtomVector* self, Atom* atom);
void AtomVector_del(AtomVector* self);

size_t Atom_key(char* str);
Atom* Atom_find(char* str);
Atom* Atom_new(char* str);
void Atom_del(char* str);

#endif
