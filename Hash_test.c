#include <stdio.h>
#include <stdlib.h>
#include "nassert.h"
#include "Symbol.h"
#include "Object.h"
#include "Hash.h"

int main(int argc, char** argv) {
  printf("--- Hash test ---\n");
  Symbol_system_init();
  Object_system_init();

  Object* apple = QSYMBOL_NEW1("apple");
  Object* red = QSTRING_NEW1("red");
  Hash* h1 = Hash_new();
  Hash_set(h1, apple, red);
  nassert(Hash_len(h1) == 1);

  Object* apple_color = NULL;
  apple_color = Object_accept(Hash_get(h1, apple));
  nassert(apple_color == red);
  nassert(Object_cmp(apple_color, red) == 0);

  Hash_rem(h1, apple);
  nassert(Hash_len(h1) == 0);

  Object* banana = QSYMBOL_NEW1("banana");
  Object* yellow = QSTRING_NEW1("yellow");
  Hash_set(h1, banana, yellow);
  nassert(Hash_len(h1) == 1);

  Object* banana_color = NULL;
  banana_color = Object_accept(Hash_get(h1, banana));
  nassert(banana_color == yellow);
  nassert(Object_cmp(banana_color, yellow) == 0);

  Hash_set(h1, apple, red);
  printf("Printing hash h1 ... \n");
  Hash_print(h1);

  Object* green = QSTRING_NEW1("green");
  Hash_set(h1, apple, green);
  printf("print hash h1 again ...\n");
  Hash_print(h1);

  printf("Cloning h1 into h1_clone ...\n");
  Hash* h1_clone = Hash_clone(h1);

  printf("Printing h1_clone ...\n");
  Hash_print(h1_clone);

  Hash* h3 = Hash_new();
  Hash_set(h3, apple, green);
  Hash_set(h3, banana, yellow);
  printf("h3 = "); Hash_print(h3); printf("\n");
  nassert(Hash_len(h3) == 2);
  printf("Hash_zero(h3)\n");
  Hash_zero(h3);
  printf("h3 = "); Hash_print(h3); printf("\n");
  nassert(Hash_len(h3) == 0);

  Object* err1 = Object_accept(Hash_set(h3, apple, apple));
  ObjectUtil_eprintf("err1->rc = %d\n", err1->rc);
  nassert(err1->rc == 1);
  ObjectUtil_eprintf("err1 = %v\n", err1);
  nassert(Object_is_error(err1));

  Object* schrodinger_cat = QSTRING_NEW1("schrodinger_cat");
  nassert(schrodinger_cat->rc == 1);
  schrodinger_cat->cloneable = 0;
  Object* err2 = Object_accept(Hash_set(h3, schrodinger_cat, red));
  ObjectUtil_eprintf("err2 = %v\n", err2);
  nassert(Object_is_error(err2));
  nassert(schrodinger_cat->rc == 1);

  //--- manual cleanup ---
  Hash_del(h1);
  Hash_del(h1_clone);
  Hash_del(h3);

  Object_system_print();
  Object_system_done();
  Symbol_system_done();

  nassert_report();
  return nassert_exit_code();
}
