#include <stdio.h>
#include <stdlib.h>

/**
 * Meant to by leaky. Used to test the tests.
 */
int main(int argc, char** argv) {
  printf("--- leaky ---\n");
  char* bytes = malloc(16);
  // We are intentionally not calling free on the bytes
  // allocated above.
  return 0;
}
