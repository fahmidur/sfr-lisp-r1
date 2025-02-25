#include <stdio.h>
#include "Object.h"
#include "nassert.h"
#include "Lisp.h"

int main(int argc, char** argv) {

  Runtime_init();
  Lisp_init();

  printf("\n=== === tc0.1 === ===\n");
  // numbers evaluate to themselves
  Object* tc0_1_string = QSTRING_NEW1("2.718");
  Object* tc0_1_value = Object_accept(Lisp_eval_string(tc0_1_string));
  ObjectUtil_eprintf("tc0_1_value = %v\n", tc0_1_value);
  nassert_obj_eq(tc0_1_value, QNUMBER(2.718));

  printf("\n=== === tc1 === ===\n");
  Object* tc1_string = QSTRING_NEW1("(+ 2.718 3.141)");
  Object* tc1_tokens_got = Object_accept(Lisp_tokenize(tc1_string));
  Object* tc1_tokens_exp = Object_new_list(1, 5, QSYMBOL("("), QSYMBOL("+"), QNUMBER(2.718), QNUMBER(3.141), QSYMBOL(")"));
  nassert_obj_eq(tc1_tokens_got, tc1_tokens_exp);
  Object* tc1_parsed = Object_accept(Lisp_parse_tokens(tc1_tokens_got));
  ObjectUtil_eprintf("tc1_parsed = %v\n", tc1_parsed);
  Object* tc1_value = Object_accept(Lisp_eval_sexp(tc1_parsed));
  ObjectUtil_eprintf("tc1_value = %v\n", tc1_value);
  nassert(Object_cmp(tc1_value, QNUMBER(5.859)) == 0);

  ObjectUtil_eprintf("again.\ntc1_value = %v\n", tc1_value);

  printf("\n=== === tc1_mul === ===\n");
  Object* tc1_mul_string = QSTRING_NEW1("(* 2.718 3.141)");
  Object* tc1_mul_tokens_got = Object_accept(Lisp_tokenize(tc1_mul_string));
  Object* tc1_mul_tokens_exp = Object_new_list(1, 5, QSYMBOL("("), QSYMBOL("*"), QNUMBER(2.718), QNUMBER(3.141), QSYMBOL(")"));
  nassert_obj_eq(tc1_mul_tokens_got, tc1_mul_tokens_exp);
  Object* tc1_mul_parsed = Object_accept(Lisp_parse_tokens(tc1_mul_tokens_got));
  ObjectUtil_eprintf("tc1_mul_parsed = %v\n", tc1_mul_parsed);
  Object* tc1_mul_value = Object_accept(Lisp_eval_sexp(tc1_mul_parsed));
  ObjectUtil_eprintf("tc1_mul_value = %v\n", tc1_mul_value);
  nassert(Object_cmp(tc1_mul_value, QNUMBER(8.537238)) == 0);

  printf("\n=== === tc1_2 === ===\n");
  Object* tc1_2_string = QSTRING_NEW1("(+ 2.718 (* 2 3.141))");
  Object* tc1_2_tokens_got = Object_accept(Lisp_tokenize(tc1_2_string));
  Object* tc1_2_tokens_exp = Object_new_list(1, 9, QSYMBOL("("), QSYMBOL("+"), QNUMBER(2.718), QSYMBOL("("), QSYMBOL("*"), QNUMBER(2.0), QNUMBER(3.141), QSYMBOL(")"), QSYMBOL(")"));
  nassert_obj_eq(tc1_2_tokens_got, tc1_2_tokens_exp);
  Object* tc1_2_parsed_got = Object_accept(Lisp_parse_tokens(tc1_2_tokens_got));
  ObjectUtil_eprintf("tc1_2_parsed_got = %v\n", tc1_2_parsed_got);
  Object* tc1_2_parsed_exp = Object_new_list(1, 3, QSYMBOL("+"), QNUMBER(2.718), Object_new_list(1, 3, QSYMBOL("*"), QNUMBER(2.0), QNUMBER(3.141)));
  ObjectUtil_eprintf("tc1_2_parsed_exp = %v\n", tc1_2_parsed_exp);
  Object* tc1_2_val = Object_accept(Lisp_eval_sexp(tc1_2_parsed_got));
  ObjectUtil_eprintf("tc1_2_val = %v\n", tc1_2_val);
  nassert(Object_cmp(tc1_2_val, QNUMBER(9.0)) == 0);

  printf("\n=== === tc2 === ===\n");
  Object* tc2_string = QSTRING_NEW1("92.3");
  Object* tc2_tokens_got = Object_accept(Lisp_tokenize(tc2_string));
  Object* tc2_tokens_exp = Object_new_list(1, 1, QNUMBER(92.3));
  nassert_obj_eq(tc2_tokens_got, tc2_tokens_exp);
  Object* tc2_parsed_got = Object_accept(Lisp_parse_tokens(tc2_tokens_got));
  Object* tc2_parsed_exp = QNUMBER(92.3);
  ObjectUtil_eprintf("tc2_parsed_got = %v\n", tc2_parsed_got);
  nassert_obj_eq(tc2_parsed_got, tc2_parsed_exp);
  Object* tc2_val = Object_accept(Lisp_eval_sexp(tc2_parsed_got));
  nassert_obj_eq(tc2_val, QNUMBER(92.3));

  printf("\n=== === tc3 === ===\n");
  Object* tc3_string = QSTRING_NEW1("(- 123 23)");
  Object* tc3_tokens_got = Object_accept(Lisp_tokenize(tc3_string));
  Object* tc3_tokens_exp = Object_new_list(1, 5, QSYMBOL("("), QSYMBOL("-"), QNUMBER(123), QNUMBER(23), QSYMBOL(")"));
  nassert_obj_eq(tc3_tokens_got, tc3_tokens_exp);
  Object* tc3_parsed_got = Object_accept(Lisp_parse_tokens(tc3_tokens_got));
  ObjectUtil_eprintf("tc3_parsed_got = %v\n", tc3_parsed_got);
  Object* tc3_parsed_exp = Object_new_list(1, 3, QSYMBOL("-"), QNUMBER(123), QNUMBER(23));
  nassert_obj_eq(tc3_parsed_got, tc3_parsed_exp);
  Object* tc3_value = Object_accept(Lisp_eval_sexp(tc3_parsed_got));
  nassert_obj_eq(tc3_value, QNUMBER(100));

  printf("\n=== === tc4 ===  ===\n");
  // testing addition of positive and negative number
  Object* tc4_string = QSTRING_NEW1("(+ 5 -9)");
  Object* tc4_tokens_got = Object_accept(Lisp_tokenize(tc4_string));
  Object* tc4_tokens_exp = Object_new_list(1, 5, QSYMBOL("("), QSYMBOL("+"), QNUMBER(5), QNUMBER(-9), QSYMBOL(")"));
  nassert_obj_eq(tc4_tokens_got, tc4_tokens_exp);
  Object* tc4_parsed_got = Object_accept(Lisp_parse_tokens(tc4_tokens_got));
  Object* tc4_parsed_exp = Object_new_list(1, 3, QSYMBOL("+"), QNUMBER(5), QNUMBER(-9));
  nassert_obj_eq(tc4_parsed_got, tc4_parsed_exp);
  Object* tc4_value = Object_accept(Lisp_eval_sexp(tc4_parsed_got));
  nassert_obj_eq(tc4_value, QNUMBER(-4));

  printf("\n=== === tc5 === ===\n");
  Object* tc5_string = QSTRING_NEW1("(displayln \"hello world\")");
  Object* tc5_tokens_got = Object_accept(Lisp_tokenize(tc5_string));
  Object* tc5_tokens_exp = Object_new_list(1, 4, QSYMBOL("("), QSYMBOL("displayln"), QSTRING("hello world"), QSYMBOL(")"));
  nassert_obj_eq(tc5_tokens_got, tc5_tokens_exp);
  Object* tc5_parsed_got = Object_accept(Lisp_parse_tokens(tc5_tokens_got));
  Object* tc5_parsed_exp = Object_new_list(1, 2, QSYMBOL("displayln"), QSTRING("hello world"));
  nassert_obj_eq(tc5_parsed_got, tc5_parsed_exp);
  ObjectUtil_eprintf("tc5_parsed_got = %v\n", tc5_parsed_got);
  Object* tc5_value = Object_accept(Lisp_eval_sexp(tc5_parsed_got));
  nassert(tc5_value != NULL && Object_is_null(tc5_value));
  nassert(Object_type(tc5_value) == SYMBOL_SYMBOL);
  ObjectUtil_eprintf("tc5_value = %v\n", tc5_value);

  printf("\n=== === tc6 === ===\n");
  Object* tc6_string = QSTRING_NEW1(") invalid1 ()");
  ObjectUtil_eprintf("tc6_string = %v\n", tc6_string);
  Object* tc6_tokens = Object_accept(Lisp_tokenize(tc6_string));
  ObjectUtil_eprintf("tc6_tokens = %v\n", tc6_tokens);
  Object* tc6_parsed = Object_accept(Lisp_parse_string(tc6_string));
  ObjectUtil_eprintf("tc6_string  = %v\n", tc6_string);
  ObjectUtil_eprintf("tc6_parsed = %v\n", tc6_parsed);
  nassert(Object_is_error(tc6_parsed));

  printf("\n=== === bad op test === ===\n");
  Object* tcbadop1_string = QSTRING_NEW1("(+ abc def)");
  Object* tcbadop1_value = Object_accept(Lisp_eval_string(tcbadop1_string));
  ObjectUtil_eprintf("tcbadop1_value = %v\n", tcbadop1_value);
  nassert(Object_type(tcbadop1_value) == SYMBOL_ERROR);

  printf("\n=== === lambda test 1 === ===\n");
  Object* tclam1_str = QSTRING_NEW1("(lambda (x) (+ 0 x))");
  ObjectUtil_eprintf("tclam1_str = %v\n", tclam1_str);
  Object* tclam1_tokens = Object_accept(Lisp_tokenize(tclam1_str));
  Object* tclam1_parsed = Object_accept(Lisp_parse_tokens(tclam1_tokens));
  ObjectUtil_eprintf("tclam1_parsed = %v\n", tclam1_parsed);
  Object* tclam1_value = Lisp_eval_sexp(tclam1_parsed);
  ObjectUtil_eprintf("tclam1_value = %v\n", tclam1_value);
  nassert(Object_type(tclam1_value) == SYMBOL_FUNCTION);

  printf("\n=== === simple rc tests === ===\n");
  Object* num1 = QNUMBER_NEW1(3.456);
  nassert(num1->rc == 1);
  Object* num1_res1 = Object_accept(Lisp_eval_sexp(num1));
  printf("num1->rc = %d\n", num1->rc);
  nassert(num1->rc == 2);
  Object_assign(&num1_res1, NULL);
  nassert(num1->rc == 1);
  Object_assign(&num1, NULL);
  int num1_count = ObjectSystem_count_matching_number(3.456);
  printf("num1_count = %d\n", num1_count);
  nassert(num1_count == 0);

  Object* numstr1 = QSTRING_NEW1("1.987");
  Object* numstr1_val = Object_accept(Lisp_eval_string(numstr1));
  int numstr1_count = ObjectSystem_count_matching_number(1.987);
  printf("numstr1_count = %d\n", numstr1_count);
  nassert(numstr1_count == 1);
  Object_assign(&numstr1_val, NULL);
  numstr1_count = ObjectSystem_count_matching_number(1.987);
  printf("numstr1_count = %d\n", numstr1_count);
  nassert(numstr1_count == 0);

  Object* addstr1 = QSTRING_NEW1("(+ 1.987 2.987)");
  Object* addstr1_res1 = Object_accept(Lisp_eval_string(addstr1));
  int addstr1_e1_count = ObjectSystem_count_matching_number(1.987);
  printf("addstr1_e1_count = %d\n", addstr1_e1_count);
  nassert(addstr1_e1_count == 0);
  int addstr1_e2_count = ObjectSystem_count_matching_number(2.987);
  printf("addstr1_e2_count = %d\n", addstr1_e2_count);
  nassert(addstr1_e2_count == 0);

  int i = 0;
  int old_obj_sys_size;
  int new_obj_sys_size;
  int old_op_rc;
  int new_op_rc;

  // test that repeated eval does not increase object count
  Object* symbol_plus = QSYMBOL_NEW1("+");
  old_op_rc = symbol_plus->rc;
  old_obj_sys_size = Object_system_size();
  printf("old_obj_sys_size = %d\n", old_obj_sys_size);
  for(i = 0; i < 100; i++) {
    Object_reject(Lisp_eval_string(QSTRING("(+ 1 2)")));
  }
  new_obj_sys_size = Object_system_size();
  printf("new_obj_sys_size = %d\n", new_obj_sys_size);
  nassert(new_obj_sys_size == old_obj_sys_size);
  new_op_rc = symbol_plus->rc;
  nassert(old_op_rc == new_op_rc);

  // test that repeated eval does not increase object count
  old_obj_sys_size = Object_system_size();
  printf("old_obj_sys_size = %d\n", old_obj_sys_size);
  for(i = 0; i < 100; i++) {
    Object_reject(Lisp_eval_string(QSTRING("(- 1 2)")));
  }
  new_obj_sys_size = Object_system_size();
  printf("new_obj_sys_size = %d\n", new_obj_sys_size);
  nassert(new_obj_sys_size == old_obj_sys_size);

  // test that repeated eval does not increase object count
  old_obj_sys_size = Object_system_size();
  printf("old_obj_sys_size = %d\n", old_obj_sys_size);
  for(i = 0; i < 100; i++) {
    Object_reject(Lisp_eval_string(QSTRING("(* 1 2)")));
  }
  new_obj_sys_size = Object_system_size();
  printf("new_obj_sys_size = %d\n", new_obj_sys_size);
  nassert(new_obj_sys_size == old_obj_sys_size);

  // test that repeated eval does not increase object count
  old_obj_sys_size = Object_system_size();
  printf("old_obj_sys_size = %d\n", old_obj_sys_size);
  for(i = 0; i < 100; i++) {
    Object_reject(Lisp_eval_string(QSTRING("(/ 1 2)")));
  }
  new_obj_sys_size = Object_system_size();
  printf("new_obj_sys_size = %d\n", new_obj_sys_size);
  nassert(new_obj_sys_size == old_obj_sys_size);

  Object* tvar1 = QSYMBOL("tvar1");
  char* dstr1 = calloc(255, sizeof(char));
  old_obj_sys_size = Object_system_size();
  printf("old_obj_sys_size = %d\n", old_obj_sys_size);
  for(i = 0; i < 100; i++) {
    memset(dstr1, 0, 255);
    sprintf(dstr1, "(define tvar1 %d)", i+1);
    /* printf("dstr1 = |%s|\n", dstr1); */
    Object_reject(Lisp_eval_string(QSTRING(dstr1)));
  }
  free(dstr1); dstr1 = NULL;
  new_obj_sys_size = Object_system_size();
  printf("new_obj_sys_size = %d\n", new_obj_sys_size);
  // we should only gain 1 new object, the last defined integer 100
  nassert(new_obj_sys_size - old_obj_sys_size == 1);

  old_obj_sys_size = Object_system_size();
  printf("old_obj_sys_size = %d\n", old_obj_sys_size);
  char* dstr2 = calloc(255, sizeof(char));
  for(i = 0; i < 100; i++) {
    memset(dstr2, 0, 255);
    sprintf(dstr2, "(quote %f)", (0.987 + i));
    Object_reject(Lisp_eval_string(QSTRING(dstr2)));
  }
  free(dstr2); dstr2 = NULL;
  new_obj_sys_size = Object_system_size();
  printf("new_obj_sys_size = %d\n", new_obj_sys_size);
  // no new objects have been created
  nassert(new_obj_sys_size == old_obj_sys_size);

  Lisp_done();
  Runtime_done();

  //----
  nassert_report();
  return nassert_exit_code();
}
