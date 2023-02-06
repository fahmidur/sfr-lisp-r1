#include <stdio.h>
#include <stdlib.h>

/**
 * Meant to by leaky. Used to test the tests.
 */
int main(int argc, char** argv) {
  printf("--- leaky ---\n");
  malloc(16);
  return 0;
}
