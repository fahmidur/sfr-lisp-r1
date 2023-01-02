/**
 * Nano-Assert.
 * @author Syed Reza
 *
 * A ultra small unit testing utility.
 */


#ifndef NASSERT_H
#define NASSERT_H

#include <stdio.h>

static int general_count = 0;
static int success_count = 0;
static int failure_count = 0;

#define nassert(expr) \
  do { \
    printf("NASSERT. F=%s L=%05d. || %s\n", __FILE__, __LINE__, #expr); \
    general_count++; \
    if((expr)) { \
      printf("\033[0;32m"); \
      printf("NASSERT. F=%s L=%05d. || R=SUCCESS\n", __FILE__, __LINE__); \
      success_count++; \
    } else { \
      printf("\033[0;31m"); \
      printf("NASSERT. F=%s L=%05d. || R=FAILURE\n", __FILE__, __LINE__); \
      failure_count++; \
    } \
    printf("\033[0m"); \
    printf("--\n"); \
  } while(0); 

#define nassert_eq(a, b) \
  do { \
    printf("NASSERT_EQ. F=%s L=%05d. || %s == %s\n", __FILE__, __LINE__, #a, #b); \
    general_count++; \
    if(a == b) { \
      printf("\033[0;32m"); \
      printf("NASSERT_EQ. F=%s L=%05d. || R=SUCCESS\n", __FILE__, __LINE__); \
      success_count++; \
    } else { \
      printf("\033[0;31m"); \
      printf("NASSERT_EQ. F=%s L=%05d. || R=FAILURE\n", __FILE__, __LINE__); \
      failure_count++; \
    } \
    printf("\033[0m"); \
    printf("--\n"); \
  } while(0);

void nassert_report() {
  printf("=======.\n");
  printf("NASSERT. SUCCESS_COUNT = %5d/%d\n", success_count, general_count);
  printf("NASSERT. FAILURE_COUNT = %5d/%d\n", failure_count, general_count);
  printf("NASSERT. ---%s---\n", failure_count == 0 ? "SUCCESS" : "FAILURE");
  printf("=======.\n");
}

int nassert_exit_code() {
  return failure_count > 0 ? 1 : 0;
}

#endif
