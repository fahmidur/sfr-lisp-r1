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
  Object* tc0_1_exp = QNUMBER_NEW1(2.718);
  ObjectUtil_eprintf("tc0_1_value = %v\n", tc0_1_value);
  nassert(Object_system_rtcount() == 0);
  printf("rtcount 001 = %lu\n", Object_system_rtcount());
  nassert_obj_eq(tc0_1_value, tc0_1_exp);
  printf("rtcount 002 = %lu\n", Object_system_rtcount());
  nassert(Object_system_rtcount() == 0);

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
  nassert(Object_system_rtcount() == 0);

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

  nassert(Object_system_rtcount() == 0);

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

  nassert(Object_system_rtcount() == 0);

  printf("\n=== === tc2 === ===\n");
  Object* tc2_string = QSTRING_NEW1("92.3");
  Object* tc2_tokens_got = Object_accept(Lisp_tokenize(tc2_string));
  Object* tc2_tokens_exp = Object_new_list(1, 1, QNUMBER(92.3));
  nassert_obj_eq(tc2_tokens_got, tc2_tokens_exp);
  Object* tc2_parsed_got = Object_accept(Lisp_parse_tokens(tc2_tokens_got));
  Object* tc2_parsed_exp = QNUMBER_NEW1(92.3);
  ObjectUtil_eprintf("tc2_parsed_got = %v\n", tc2_parsed_got);
  nassert_obj_eq(tc2_parsed_got, tc2_parsed_exp);
  Object* tc2_val = Object_accept(Lisp_eval_sexp(tc2_parsed_got));
  nassert_obj_eq(tc2_val, tc2_parsed_exp);

  nassert(Object_system_rtcount() == 0);

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
  Object* tc3_exp = QNUMBER_NEW1(100);
  nassert_obj_eq(tc3_value, tc3_exp);

  nassert(Object_system_rtcount() == 0);

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
  Object* tc4_exp = QNUMBER_NEW1(-4);
  nassert_obj_eq(tc4_value, tc4_exp);

  nassert(Object_system_rtcount() == 0);

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

  nassert(Object_system_rtcount() == 0);

  printf("\n=== === tc6 === ===\n");
  Object* tc6_string = QSTRING_NEW1(") invalid1 ()");
  ObjectUtil_eprintf("tc6_string = %v\n", tc6_string);
  Object* tc6_tokens = Object_accept(Lisp_tokenize(tc6_string));
  ObjectUtil_eprintf("tc6_tokens = %v\n", tc6_tokens);
  Object* tc6_parsed = Object_accept(Lisp_parse_string(tc6_string));
  ObjectUtil_eprintf("tc6_string  = %v\n", tc6_string);
  ObjectUtil_eprintf("tc6_parsed = %v\n", tc6_parsed);
  nassert(Object_is_error(tc6_parsed));

  nassert(Object_system_rtcount() == 0);

  printf("\n=== === bad op test === ===\n");
  Object* tcbadop1_string = QSTRING_NEW1("(+ abc def)");
  Object* tcbadop1_value = Object_accept(Lisp_eval_string(tcbadop1_string));
  ObjectUtil_eprintf("tcbadop1_value = %v\n", tcbadop1_value);
  nassert(Object_type(tcbadop1_value) == SYMBOL_ERROR);

  nassert(Object_system_rtcount() == 0);

  printf("\n=== === lambda test 1 === ===\n");
  Object* tclam1_str = QSTRING_NEW1("(lambda (x) (+ 0 x))");
  ObjectUtil_eprintf("tclam1_str = %v\n", tclam1_str);
  Object* tclam1_tokens = Object_accept(Lisp_tokenize(tclam1_str));
  Object* tclam1_parsed = Object_accept(Lisp_parse_tokens(tclam1_tokens));
  ObjectUtil_eprintf("tclam1_parsed = %v\n", tclam1_parsed);
  Object* tclam1_value = Object_accept(Lisp_eval_sexp(tclam1_parsed));
  ObjectUtil_eprintf("tclam1_value = %v\n", tclam1_value);
  nassert(Object_type(tclam1_value) == SYMBOL_FUNCTION);

  nassert(Object_system_rtcount() == 0);

  /* Object_system_gc(); */

  printf("\n=== === simple rc tests === ===\n");
  nassert(ObjectSystem_count_matching_number(3.456) == 0);
  Object* num1 = QNUMBER_NEW1(3.456);
  nassert(num1->rc == 1);
  ObjectUtil_eprintf("num1 = %v | p=%p\n", num1, num1);
  Object* num1_res1 = Object_accept(Lisp_eval_sexp(num1));
  nassert(num1_res1 != NULL);
  nassert(num1 == num1_res1);
  nassert(Object_cmp(num1_res1, num1) == 0);
  ObjectUtil_eprintf("num1_res = %v | p=%p\n", num1_res1, num1_res1);
  printf("num1->rc = %d\n", num1->rc);
  nassert(num1->rc == 2); // this fails, debug here
  Object_assign(&num1_res1, NULL);
  printf("num1->rc = %d\n", num1->rc);
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
    // printf("dstr1 = |%s|\n", dstr1); 
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

  nassert(Object_system_rtcount() == 0);

  printf("\n=== === lambda env leak test 2 === ===\n");
  Object* tclam2_line1 = QSTRING_NEW1("(define make_account (lambda (balance) (lambda (amt) (begin (set! balance (+ balance amt)) balance ))))");
  Object_reject(Lisp_eval_string(tclam2_line1));
  Object* tclam2_line2 = QSTRING_NEW1("(define acc (make_account 100))");
  Object_reject(Lisp_eval_string(tclam2_line2));
  int environment_count_bef = ObjectSystem_count_type(SYMBOL_ENVIRONMENT);
  printf("environment_count_bef = %d\n", environment_count_bef);
  Object* tclam2_line3 = QSTRING_NEW1("(acc 1)");
  for(i = 0; i < 5; i++) {
    printf("adding 1 to account balance with (acc 1) ...\n");
    Object_reject(Lisp_eval_string(tclam2_line3));
  }
  int environment_count_aft = ObjectSystem_count_type(SYMBOL_ENVIRONMENT);
  printf("environment_count_aft = %d\n", environment_count_aft);
  nassert(environment_count_bef == environment_count_aft);

  //generic binary op test
  const char* binary_ops[9] = {"<", ">", "<=", ">=", "+", "-", "/", "*", "list"};
  char binop_str[256];
  size_t binop_obj_count = 0;
  for(i = 0; i < 9; i++) {
    memset(binop_str, 0, sizeof(binop_str));
    sprintf(binop_str, "(%s 1 2)", binary_ops[i]);
    binop_obj_count = Object_system_size();
    for(int j = 0; j < 10; j++) {
      Object* binop_obj = QSTRING_NEW1(binop_str);
      // ObjectUtil_eprintf("%2d | binop_obj = %v\n", j, binop_obj);
      Object* binop_res = Object_accept(Lisp_eval_string(binop_obj));
      /* ObjectUtil_eprintf("%2d | binop_res = %v\n", j, binop_res); */
      Object_assign(&binop_obj, NULL);
      Object_assign(&binop_res, NULL);
    }
    nassert(Object_system_size() == binop_obj_count);
  }

  Object* load_statement = QSTRING_NEW1("(load \"test/sample/lambda_001.lsp\")");
  int load_env_count = ObjectSystem_count_type(SYMBOL_ENVIRONMENT);
  printf("load_env_count = %d\n", load_env_count);
  for(i = 0; i < 4; i++) {
    Object_reject(Lisp_eval_string(load_statement));
  }
  // we only gain one Environment from evaluating the last lambda_001.lsp
  // we do not gain any new objects from evaluating the same file multiple times.
  int load_env_count_aft = ObjectSystem_count_type(SYMBOL_ENVIRONMENT);
  printf("load_env_count_aft = %d\n", load_env_count_aft);
  nassert(load_env_count_aft == (load_env_count+1));
  Object_assign(&load_statement, NULL);

  Object* qlist1 = Object_accept(Lisp_eval_string(QSTRING("(list 1 2 3)")));
  nassert(Object_type(qlist1) == SYMBOL_LIST);
  nassert(Object_len(qlist1) == 3);
  nassert(Object_cmp(Object_bop_at(qlist1, 0), QNUMBER(1)) == 0);
  nassert(Object_cmp(Object_bop_at(qlist1, 1), QNUMBER(2)) == 0);
  nassert(Object_cmp(Object_bop_at(qlist1, 2), QNUMBER(3)) == 0);
  Object_assign(&qlist1, NULL);

  Object* car_res1 = Object_accept(Lisp_eval_string(QSTRING("(car (list 1 2 3))")));
  nassert(Object_cmp(car_res1, QNUMBER(1)) == 0);

  Object* cdr_res1 = Object_accept(Lisp_eval_string(QSTRING("(cdr (list 1 2 3))")));
  nassert(Object_type(cdr_res1) == SYMBOL_LIST);
  nassert(Object_len(cdr_res1) == 2);
  nassert(Object_cmp(Object_bop_at(cdr_res1, 0), QNUMBER(2)) == 0);
  nassert(Object_cmp(Object_bop_at(cdr_res1, 1), QNUMBER(3)) == 0);

  Object* last_res1 = Object_accept(Lisp_eval_string(QSTRING("(last (list 1 2 3))")));
  nassert(Object_cmp(last_res1, QNUMBER(3)) == 0);

_shutdown:
  Lisp_done();
  Runtime_done();

  //----
  nassert_report();
  return nassert_exit_code();
}
