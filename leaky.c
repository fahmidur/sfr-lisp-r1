#include <stdio.h>
#include <stdlib.h>

/**
 * Meant to by leaky. Used to test the tests.
 */
int main(int argc, char** argv) {
  printf("--- { leaky { ---\n");
  size_t nbytes = 16;

  printf("malloc(%d)\n", nbytes);
  char* buf = malloc(nbytes);

  // We are intentionally not calling free on the bytes
  // allocated above.
  printf("--- } leaky } ---\n");
  return 0;
}
