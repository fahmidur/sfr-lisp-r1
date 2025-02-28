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
static int __nassert_cmp_res = 0;

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

#define nassert_obj_eq(a, b) \
  do { \
    printf("NASSERT_OBJ_EQ. F=%s L=%05d. || Object_cmp(%s, %s) == 0\n", __FILE__, __LINE__, #a, #b); \
    general_count++; \
    assert(a != NULL); \
    assert(b != NULL); \
    assert(a->rc >= 1); \
    assert(b->rc >= 1); \
    ObjectUtil_eprintf("%s = %v\n%s = %v\n", #a, a, #b, b); \
    __nassert_cmp_res = Object_cmp(a, b); \
    printf("NASSERT_OBJ_EQ. cmp_res = %d\n", __nassert_cmp_res); \
    if(__nassert_cmp_res == 0) { \
      printf("\033[0;32m"); \
      printf("NASSERT_OBJ_EQ. F=%s L=%05d. || R=SUCCESS\n", __FILE__, __LINE__); \
      success_count++; \
    } else { \
      printf("\033[0;31m"); \
      printf("NASSERT_OBJ_EQ. F=%s L=%05d. || R=FAILURE\n", __FILE__, __LINE__); \
      failure_count++; \
    } \
    printf("\033[0m"); \
    printf("--\n"); \
  } while(0);

void nassert_report() {
  printf("=======.\n");

  if(success_count == general_count) { printf("\033[0;32m"); } // GREEN
  printf("NASSERT. SUCCESS_COUNT = %5d/%d\n", success_count, general_count);
  printf("\033[0m"); // CLEAR

  if(failure_count > 0) { printf("\033[0;31m"); } // RED
  printf("NASSERT. FAILURE_COUNT = %5d/%d\n", failure_count, general_count);
  printf("\033[0m"); // CLEAR

  if(failure_count == 0) {
    printf("\033[0;32m"); // GREEN
  } else {
    printf("\033[0;31m"); // RED
  }
  printf("NASSERT. ---%s---\n", failure_count == 0 ? "SUCCESS" : "FAILURE");
  printf("\033[0m"); // CLEAR
  printf("=======.\n");
}

int nassert_exit_code() {
  return failure_count > 0 ? 1 : 0;
}

#endif
