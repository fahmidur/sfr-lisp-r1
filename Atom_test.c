#include <stdio.h>
#include <stdlib.h>
#include "nassert.h"
#include "Atom.h"

int main(int argc, char** argv) {
  printf("--- Atom test ---\n");
  
  Atom_system_init();

  Atom* apple1 = Atom_new("apple");
  Atom* apple2 = Atom_new("apple");

  nassert(apple1 == apple2);

  Atom* orange = Atom_new("orange");

  nassert(apple1 != orange);

  Atom_system_done();

  nassert_report();
  return nassert_exit_code();
}
