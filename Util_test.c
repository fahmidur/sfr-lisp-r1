#include <stdio.h>
#include <stdlib.h>
#include "nassert.h"
#include "Util.h"
#include <string.h>

void test_stdout(void (*foo)(FILE*), char* exp) {
  FILE* fp = tmpfile();
  foo(fp);
  fflush(fp);
  size_t fsize = ftell(fp);
  rewind(fp);
  char* buf = malloc(fsize+1);
  fread(buf, 1, fsize, fp);
  fclose(fp);
  size_t buf_len = strlen(buf);
  size_t exp_len = strlen(exp);
  printf("test_stdout. buf = |%s| len=%ld\n", buf, buf_len);
  printf("test_stdout. exp = |%s| len=%ld\n", exp, exp_len);
  nassert(buf_len == exp_len);
  nassert(strcmp(buf, exp) == 0);
  free(buf); buf = NULL;
}

void test_util_indent(FILE* fp) {
  Util_indent(fp, 2);
}

int main(int argc, char** argv) {
  nassert(Util_cstr_hash_djb2("") == 5381);
  nassert(Util_sizet_max(2, 3) == 3);
  nassert(Util_sizet_max(9, 2) == 9);
  nassert(Util_sizet_max(1, 1) == 1);

  test_stdout(test_util_indent, "    ");

  //----
  nassert_report();
  return nassert_exit_code();
}
