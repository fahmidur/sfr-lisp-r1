/**
 * This is simply the driver program which will
 * use our crude runtime and Lisp library. 
 * @author Syed Reza
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Runtime.h"
#include "Lisp.h"
#include "String.h"
#include "sub/linenoise/linenoise.h"

#ifndef VERSION
#define VERSION "0.0.1"
#endif

#ifndef GIT_SHA
#define GIT_SHA "unknown"
#endif

void print_banner() {
  printf("+----------------------------------------------\n");
  printf("| SFR-LISP \n");
  printf("+----------------------------------------------\n");
  printf("| VERSION: %s\n", VERSION);
  printf("| GIT_SHA: %s\n", GIT_SHA);
  printf("+----------------------------------------------\n");
  printf("\n");
}

void print_usage(const char* progname) {
  print_banner();
  printf("Usage: %s [OPTIONS]\n", progname);
  printf("  -h,--help         Print this help page.\n");
  printf("  -v,--version      Print version.\n");
  printf("\n\n");
}

void repl() {
  Object* obj_inp = QSTRING_NEW1("");
  String* inp = obj_inp->impl;
  ssize_t inp_ret;
  Object* rvalue = NULL;
  char* line = NULL;

  linenoiseHistoryLoad("history.txt"); /* Load the history at startup */
  while( (line=linenoise("> ")) != NULL) {
    /* inp_ret = String_getline(inp, stdin); */
    /* if(inp_ret < 0) { */
    /*   printf("getline. ERROR=%ld\n", inp_ret); */
    /*   break; */
    /* } */
    String_zero(inp);
    String_addx_cstr(inp, line);
    printf("OK. READ 001. %ld chars. buf=|%s| buf_size=%ld\n", inp_ret, inp->buf, inp->buf_size);
    String_chomp(inp);
    printf("OK. READ 002. %ld chars. buf=|%s| buf_size=%ld\n", inp_ret, inp->buf, inp->buf_size);

    linenoiseHistoryAdd(line);
    linenoiseHistorySave("history.txt");

    if(strcmp(inp->buf, ".exit") == 0) {
      printf("goodbye\n");
      break;
    }
    if(strcmp(inp->buf, ".env") == 0) {
      Lisp_printenv();
    }
    ObjectUtil_eprintf("OK. Got input = %v\n", obj_inp);

    if(rvalue == NULL) {
    }
    else
    if(!Object_is_null(rvalue)) {
      Object_assign(&rvalue, NULL);
    }
    rvalue = Object_accept(Lisp_eval_string(obj_inp));
    ObjectUtil_eprintf("rvalue = %v\n", rvalue);
    free(line);
    line = NULL;
  }
  /* String_del(inp); */
  if(line != NULL) {
    free(line);
  }
  Object_assign(&obj_inp, NULL);
}

char file_exists(const char *path) {
  FILE *file = fopen(path, "r");
  if(file) {
    fclose(file);
    return 1;
  }
  return 0;
}

int strcmp_onedash(const char* a, const char* b, const char* c) {
  char* buf = (char*) calloc(strlen(a)+1, sizeof(char));
  size_t buf_pos = 0;
  unsigned int i;
  int res;
  size_t a_len = strlen(a);
  char leading = 1;
  unsigned int leading_dash_count = 0;
  for(i = 0; i < a_len; i++) {
    if(leading && a[i] == '-') { 
      if(leading_dash_count == 0) {
        buf[buf_pos++] = a[i];
      }
      leading_dash_count++;
      continue; 
    }
    leading = 0;
    buf[buf_pos++] = a[i];
  }
  //printf("dbg. buf=%s b=%s c=%s\n", buf, b, c);
  res = (
    strcmp(buf, b) == 0 ||
    strcmp(buf, c) == 0 
  ) ? 0 : -1;
  free(buf);
  return res;
}

int main(int argc, char** argv) {
  int ret = 0;
  Runtime_init();
  Lisp_init();
  if(argc == 2) {
    if(strcmp_onedash(argv[1], "-help", "-h") == 0) {
      print_usage(argv[0]);
      goto _return;
    }
    else
    if(strcmp_onedash(argv[1], "-version", "-v") == 0) {
      printf("%s\n", VERSION);
    }
    else
    if(file_exists(argv[1])) {
      printf("evalfile: %s\n", argv[1]);
      FILE* file = fopen(argv[1], "r");
      if(file == NULL) {
        printf("ERROR: Failed to open file at %s\n", argv[1]);
        goto _return;
      }
      Object* content_obj = QSTRING_NEW1("");
      String* content = content_obj->impl;
      String* fline = String_new("");
      while(String_getline(fline, file) != -1) {
        if(fline->len > 0 && fline->buf[0] == '#') {
          // ignore lines starting with '#'. These are Racket lang-line pragmas
          continue;
        }
        /* printf("%s\n", fline->buf); */
        String_addx(content, fline);
      }
      String_del(fline);
      fclose(file);
      printf("--- { content { ---\n");
      printf("%s\n", content->buf);
      printf("--- } content } ---\n");
      Object* fresult = Object_accept(Lisp_eval_string(content_obj));
      ObjectUtil_eprintf("fresult = %v\n", fresult);
    }
    else {
      printf("ERROR: invalid arg: %s\n", argv[1]);
      ret = 1;
      goto _return;
    }
  }
  else 
  if(argc == 1) {
    // REPL
    print_banner();
    repl();
  }
  else {
    printf("ERROR: invalid args\n");
    ret = 1;
    goto _return;
  }
_return:
  Lisp_done();
  Runtime_done();
  return ret;
}
