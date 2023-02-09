#ifndef __SYMBOL
#define __SYMBOL

#define SYMBOL_BUCKET_SIZE 2048
#define SYMBOLVECTOR_ISIZE 32

void Symbol_system_init();
void Symbol_system_done();
void Symbol_system_print();

typedef struct Symbol Symbol;
struct Symbol {
  size_t str_len;
  char* str;
};

typedef struct SymbolVector SymbolVector;
struct SymbolVector {
  size_t size;
  size_t lidx;
  Symbol** list;
};

SymbolVector* SymbolVector_new();
void SymbolVector_push(SymbolVector* self, Symbol* atom);
void SymbolVector_del(SymbolVector* self);

size_t Symbol_calc_key(char* str);
Symbol* Symbol_find(char* str);
Symbol* Symbol_new(char* str);
void Symbol_del(Symbol* str);

void Symbol_print(Symbol* self);
Symbol* Symbol_clone(Symbol* self);

#endif
