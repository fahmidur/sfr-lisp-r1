#include <stdio.h>
#include <stdlib.h>
#include "nassert.h"
#include "Atom.h"

int main(int argc, char** argv) {
  printf("--- Atom test ---\n");
  
  Atom_system_init();

  Atom* atom1 = Atom_new("hello");
  Atom* atom2 = Atom_new("hello");

  nassert(atom1 == atom2);

  Atom_system_done();

  nassert_report();
  return nassert_exit_code();
}
