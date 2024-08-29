#include <stdio.h>
#include <stdlib.h>
#include "nassert.h"
#include "Symbol.h"
#include "String.h"
#include "Number.h"
#include "Object.h"

int main(int argc, char** argv) {

  Util_heading1(1, "RUNTIME INIT");
  ErrorSystem_init();
  Symbol_system_init();
  Object_system_init();
  Util_heading1(0, "RUNTIME INIT");

  //===========================================================================

  nassert(Symbol_new("Symbol") == SYMBOL_SYMBOL);
  nassert(Symbol_new("Number") == SYMBOL_NUMBER);
  nassert(Symbol_new("String") == SYMBOL_STRING);
  nassert(Symbol_new("Error") == SYMBOL_ERROR);
  nassert(Symbol_new("List") == SYMBOL_LIST);

  //===========================================================================
  Util_heading1(1, "SYMBOL OPERATIONS");

  Object* sym1 = QSYMBOL_NEW1("hammer_and_sickle");
  nassert(Object_type(sym1) == SYMBOL_SYMBOL);

  Util_heading1(0, "SYMBOL OPERATIONS");

  //===========================================================================
  Util_heading1(1, "STRING OPERATIONS");

  int objsize = Object_system_size();
  Object* str1 = Object_new(SYMBOL_STRING, 1, String_new("Hello there 001"));
  Object* str2 = Object_new(SYMBOL_STRING, 1, String_new("Hello there 002"));
  Object* str3 = Object_new(SYMBOL_STRING, 1, String_new("Hello there 003"));
  nassert(objsize - Object_system_size() >= 3); // gained at least 3 objects
  Object_system_gc();
  nassert(objsize - Object_system_size() >= 3);
  Object* tmp1 = Object_new(SYMBOL_STRING, 0, String_new("This too will pass."));
  tmp1->returning = 0; // manually mark it as no longer returning
  nassert(tmp1->rc == 0);
  nassert(objsize - Object_system_size() >= 4);
  objsize = Object_system_size();
  Object_system_gc();
  nassert(objsize - Object_system_size() == 1);

  Object* tmp2 = Object_new(SYMBOL_STRING, 1, String_new("So long and thanks for all the fish."));
  /*printf("BEF. tmp2 = "); Object_print(tmp2); printf("\n");*/
  ObjectUtil_eprintf("BEF. tmp2 = %v\n", tmp2);
  tmp2 = Object_rc_decr(tmp2);
  /*printf("AFT. tmp2 = "); Object_print(tmp2); printf("\n");*/
  ObjectUtil_eprintf("AFT. tmp2 = %v\n", tmp2);

  Object* stralike1 = Object_new(SYMBOL_STRING, 1, String_new("John Doe"));
  Object* stralike2 = Object_new(SYMBOL_STRING, 1, String_new("John Doe"));
  Object* stralike3 = Object_new(SYMBOL_STRING, 1, String_new("John Doo"));

  nassert(Object_cmp(str1, str2) != 0);
  nassert(Object_cmp(stralike1, stralike2) == 0);
  nassert(Object_cmp(stralike2, stralike3) != 0);

  Object* str_hello = Object_new(SYMBOL_STRING, 1, String_new("Hello"));
  Object* str_there = Object_new(SYMBOL_STRING, 1, String_new("There"));
  Object* str_conc1 = Object_accept(Object_bop_add(str_hello, str_there));
  Object* str_conc1_exp = Object_new(SYMBOL_STRING, 1, String_new("HelloThere"));
  nassert(Object_cmp(str_conc1, str_conc1_exp) == 0);

  Object* qstr1 = QSTRING_NEW1("the quick brown fox");
  nassert(Object_type(qstr1) == SYMBOL_STRING);
  /*Object_print(qstr1); printf("\n");*/
  ObjectUtil_eprintf("qstr1 = %v\n", qstr1);

  Object* str_hello2 = QSTRING_NEW1("General");
  Object* str_there2 = QSTRING_NEW1("Kenobi");
  Object* str_conc2_exp = QSTRING_NEW1("GeneralKenobi");
  Object_bop_addx(str_hello2, str_there2);
  nassert(Object_cmp(str_hello2, str_conc2_exp) == 0);

  Object* qstr0 = QSTRING_NEW1("");
  Object* qstr5 = QSTRING_NEW1("a non-empty string");
  ObjectUtil_eprintf("qstr5 = %v\n", qstr5);
  Object_zero(qstr5);
  nassert(Object_cmp(qstr0, qstr5) == 0);

  // Test conversion of String -> Symbol
  Object* qstr6 = QSTRING_NEW1("+");
  ObjectUtil_eprintf("qstr6 = %v\n", qstr6);
  nassert(Object_type(qstr6) == SYMBOL_STRING);
  Object* qstr6_symbol = Object_accept(Object_to_symbol(qstr6));
  nassert(Object_is_returning(qstr6_symbol) == 0);
  ObjectUtil_eprintf("qstr6_symbol = %v\n", qstr6_symbol);
  nassert(Object_type(qstr6_symbol) == SYMBOL_SYMBOL);
  Object* plus_symbol = QSYMBOL_NEW1("+");
  nassert(Object_cmp(plus_symbol, qstr6_symbol) == 0);

  // Test conversion of String -> Number
  // TODO

  Util_heading1(0, "STRING OPERATIONS");

  //===========================================================================

  Util_heading1(1, "NUMBER OPERATIONS");

  Object* num1 = Object_new(SYMBOL_NUMBER, 1, Number_new(3));
  Object* num2 = Object_new(SYMBOL_NUMBER, 1, Number_new(4));

  Object* vnum1 = QNUMBER_NEW1(5);
  Object* vnum2 = QNUMBER_NEW1(7);
  Object* vnumx = QNUMBER_NEW1(12);
  Object_bop_addx(vnum1, vnum2);
  nassert(Object_cmp(vnum1, vnumx) == 0);

  printf("Test Number + Number ...\n");
  Object* res1 = Object_accept(Object_bop_add(num1, num2));
  ObjectUtil_eprintf("%v + %v = %v\n", num1, num2, res1);
  Object* res1_expected = Object_new(SYMBOL_NUMBER, 1, Number_new(7));
  nassert(Object_cmp(res1, res1_expected) == 0);
  /*printf("res1 = "); Object_print(res1); printf("\n");*/
  ObjectUtil_eprintf("res1 = %v\n", res1);

  printf("Test Number - Number ...\n");
  Object* res2 = Object_accept(Object_bop_sub(num1, num2));
  ObjectUtil_eprintf("%v - %v = %v\n", num1, num2, res2);
  Object* res2_expected = Object_new(SYMBOL_NUMBER, 1, Number_new(-1));
  nassert(Object_cmp(res2, res2_expected) == 0);

  printf("Test Number * Number ...\n");
  Object* res3 = Object_accept(Object_bop_mul(num1, num2));
  ObjectUtil_eprintf("%v * %v = %v\n", num1, num2, res3);
  Object* res3_expected = Object_new(SYMBOL_NUMBER, 1, Number_new(12));
  nassert(Object_cmp(res3, res3_expected) == 0);

  printf("Test Number / Number ...\n");
  Object* res4 = Object_accept(Object_bop_div(num1, num2));
  ObjectUtil_eprintf("%v / %v = %v\n", num1, num2, res4);
  Object* res4_expected = Object_new(SYMBOL_NUMBER, 1, Number_new(0.75));
  nassert(Object_cmp(res4, res4_expected) == 0);

  printf("Test String + Number ...\n");
  Object* sum_invalid = Object_accept(Object_bop_add(str1, num2));
  ObjectUtil_eprintf("%v + %v = %v\n", str1, num2, sum_invalid);
  Object_print(sum_invalid); printf("\n");
  nassert(Object_type(sum_invalid) == Symbol_new("Error"));

  Object* qnum0 = QNUMBER_NEW1(0);

  Object* qnum1 = QNUMBER_NEW1(3.14);
  nassert(Object_type(qnum1) == SYMBOL_NUMBER);

  Object* qnum2 = QNUMBER_NEW1(2.718);
  nassert(Object_type(qnum2) == SYMBOL_NUMBER);
  ObjectUtil_eprintf("qnum2 = %v\n", qnum2);
  printf("Object_zero(qnum2) ...\n");
  Object_zero(qnum2);
  ObjectUtil_eprintf("qnum2 = %v\n", qnum2);
  nassert(Object_cmp(qnum2, qnum0) == 0);
  
  Util_heading1(0, "NUMBER OPERATIONS");

  //===========================================================================
  
  Util_heading1(1, "LIST OPERATIONS");

  // List.push
  Object* list1 = Object_new(SYMBOL_LIST, 1, List_new());
  nassert(list1->rc == 1);
  Object* mem1 = Object_new(SYMBOL_NUMBER, 1, Number_new_from_double(5.0));
  Object* mem2 = Object_new(SYMBOL_NUMBER, 1, Number_new_from_double(6.0));
  nassert(mem1->rc == 1);
  nassert(mem2->rc == 1);
  printf("Calling list1.push(mem1) ...\n");
  Object_reject(Object_bop_push(list1, mem1));
  nassert(mem1->rc == 2);
  nassert(list1->rc == 1);
  printf("Calling list1.push(mem2) ...\n");
  Object_reject(Object_bop_push(list1, mem2));
  nassert(mem2->rc == 2);
  nassert(list1->rc == 1);
  printf("list1 = "); Object_print(list1); printf("\n");
  nassert(Object_len(list1) == 2);
  nassert(list1->rc == 1);

  Object* list2 = Object_new(SYMBOL_LIST, 1, List_new());
  nassert(list2->rc == 1);
  printf("Calling list2.push(mem1) ...\n");
  Object_reject(Object_bop_push(list2, mem1));
  nassert(mem1->rc == 3);
  nassert(list2->rc == 1);
  printf("Calling list2.push(mem2) ...\n");
  Object_reject(Object_bop_push(list2, mem2));
  nassert(mem2->rc == 3);
  nassert(list2->rc == 1);
  printf("list2 = "); Object_print(list2); printf("\n");
  printf("--- again with eprintf ---\n");
  ObjectUtil_eprintf("list2 = %v\n", list2);
  ObjectUtil_eprintf("list2, which is %v, is great!\n", list2);
  ObjectUtil_eprintf("list2, which is %v, contains %d elements.\n", list2, Object_len(list2));

  printf("Calling list2.pop() --> popres1 ...\n");
  Object* popres1 = Object_accept(Object_uop_pop(list2));
  nassert(Object_is_error(popres1) == 0);
  nassert(Object_is_null(popres1) == 0);
  ObjectUtil_eprintf("list2 = %v AND popres1 = %v\n", list2, popres1);
  nassert(Object_len(list2) == 1);
  nassert(Object_cmp(popres1, mem2) == 0);
  nassert(Object_cmp(popres1, mem1) != 0);
  nassert(mem2->rc == 3);

  printf("Calling list2.pop() --> popres1 (again) ...\n");
  /*Object_rc_decr(popres1);*/
  /*popres1 = Object_accept(Object_uop_pop(list2));*/
  // Object_assign(...) is better
  Object_assign(&popres1, Object_uop_pop(list2));
  // mem2 is now referenced only by mem2 and list1
  nassert(mem2->rc == 2); 
  // mem1 is now referenced only by mem1, list1, and popres1
  nassert(mem1->rc == 3);
  nassert(!Object_is_error(popres1) && !Object_is_null(popres1));
  nassert(Object_len(list2) == 0);
  nassert(Object_cmp(popres1, mem1) == 0);
  ObjectUtil_eprintf("list2 = %v AND popres1 = %v\n", list2, popres1);

  printf("Calling list2.unshift(mem1) ...\n");
  Object_reject(Object_bop_unshift(list2, mem1));
  ObjectUtil_eprintf("list2 = %v\n", list2);
  // mem1 is now referenced only by mem1, list1, popres1, and list2
  nassert(mem1->rc == 4);
  printf("Calling list2.unshift(mem2) ...\n");
  Object_reject(Object_bop_unshift(list2, mem2));
  ObjectUtil_eprintf("list2 = %v\n", list2);
  Object* mem3 = Object_new(SYMBOL_STRING, 1, String_new("mem3"));
  ObjectUtil_eprintf("mem3 = %v\n", mem3);
  printf("Calling list2.unshift(mem3) ...\n");
  Object_reject(Object_bop_unshift(list2, mem3));
  ObjectUtil_eprintf("list2 = %v\n", list2);
  
  printf("Calling list2.shift() --> shifted1 ...\n");
  Object* shifted1 = Object_accept(Object_uop_shift(list2));
  ObjectUtil_eprintf("shifted1 = %v\n", shifted1);
  ObjectUtil_eprintf("list2 = %v\n", list2);
  nassert(Object_cmp(shifted1, mem3) == 0);

  printf("Calling list.at(1) --> tmp3 ...\n");
  Object* tmp3 = Object_accept(Object_bop_at(list2, 1));
  ObjectUtil_eprintf("tmp3 = %v\n", tmp3);
  nassert(Object_cmp(tmp3, mem1) == 0);

  printf("Creating list3 with QLIST_NEW1() ...\n");
  Object* list3 = QLIST_NEW1();
  ObjectUtil_eprintf("list3 = %v\n", list3);

  printf("Creating list4 with Object_new_list ...\n");
  Object* list4 = Object_new_list(1, 2, QSTRING("qlist-1"), QSTRING("qlist-2"));
  ObjectUtil_eprintf("list4 = %v\n", list4);

  printf("Creating nested list nlist1 ...\n");
  Object* sublist1 = Object_new_list(1, 3, QNUMBER(1), QNUMBER(2), QNUMBER(3));
  Object* nlist1 = Object_new_list(1, 2, QSTRING("nlist1_e1"), QSTRING("nlist_e2"));
  Object_reject(Object_bop_push(nlist1, sublist1));
  Object_reject(Object_bop_push(nlist1, QSTRING("nlist_e4")));
  ObjectUtil_eprintf("nlist1 = %v\n", nlist1);
  nassert(Object_len(nlist1) == 4);

  //keywords: circle, cycle
  printf("Creating circular list ... (cycle, circle)\n");
  Object* list_c_a = Object_new_list(1, 1, QSTRING("l.A"));
  Object* list_c_b = Object_new_list(1, 1, QSTRING("l.B"));
  Object* list_c_c = Object_new_list(1, 1, QSTRING("l.C"));
  Object_bop_push(list_c_a, list_c_b); // A -> B
  Object_bop_push(list_c_b, list_c_c); // B -> C
  Object_bop_push(list_c_c, list_c_a); // C -> A
  nassert(list_c_a->rc == 2);
  nassert(list_c_b->rc == 2);
  nassert(list_c_c->rc == 2);
  ObjectUtil_eprintf("list_c_a(%p) = \n  %v\n", list_c_a, list_c_a);
  printf("list_c_a->visited = %d\n", list_c_a->visited);
  nassert(list_c_a->visited == 0);
  ObjectUtil_eprintf("list_c_b(%p) = \n  %v\n", list_c_b, list_c_b);
  nassert(list_c_b->visited == 0);
  ObjectUtil_eprintf("list_c_c(%p) = \n  %v\n", list_c_c, list_c_c);
  nassert(list_c_c->visited == 0);

  Object_assign(&list_c_a, NULL);
  nassert(Object_is_null(list_c_a));

  Object_assign(&list_c_b, NULL);
  nassert(Object_is_null(list_c_b));

  Object_assign(&list_c_c, NULL);
  nassert(Object_is_null(list_c_c));

  nassert(list_c_a->rc == 1);
  nassert(list_c_b->rc == 1);
  nassert(list_c_c->rc == 1);

  Util_heading1(0, "LIST OPERATIONS");

  //===========================================================================
  
  Util_heading1(1, "HASH OPERATIONS");

  Object* hashop_err = Object_new_null();
  Object* hash1 = QHASH_NEW1();
  ObjectUtil_eprintf("hash1 = %v\n", hash1);
  nassert(hash1->rc == 1);
  Object* str_apple = QSTRING_NEW1("apple");
  Object* str_red = QSTRING_NEW1("red");

  hashop_err = Object_accept(Object_top_hset(hash1, str_apple, str_red));
  ObjectUtil_eprintf("[1] hashop_err = %v | rc=%d\n", hashop_err, hashop_err->rc);
  nassert(!Object_is_error(hashop_err));
  Object_assign(&hashop_err, NULL);
  ObjectUtil_eprintf("[2] hashop_err = %v | rc=%d\n", hashop_err, hashop_err->rc);
  nassert(hashop_err->rc == 1);
  ObjectUtil_eprintf("hash1 = %v\n", hash1);

  // only referred by str_red because keys are always cloned into the Hash.
  nassert(str_apple->rc == 1); 
  nassert(str_red->rc == 2); // referred by str_red, hash1
                             
  nassert(hash1->rc == 1);   // ensure rc of hash1 has not changed
  nassert(Object_len(hash1) == 1);

  Object* str_banana = QSTRING_NEW1("banana");
  Object* str_yellow = QSTRING_NEW1("yellow");

  hashop_err = Object_accept(Object_top_hset(hash1, str_banana, str_yellow));
  ObjectUtil_eprintf("[1] hashop_err = %v | rc=%d\n", hashop_err, hashop_err->rc);
  nassert(!Object_is_error(hashop_err));
  Object_assign(&hashop_err, NULL);
  ObjectUtil_eprintf("[2] hashop_err = %v | rc=%d\n", hashop_err, hashop_err->rc);
  nassert(hashop_err->rc == 1);
  ObjectUtil_eprintf("hash1 = %v\n", hash1);

  nassert(str_banana->rc == 1); // referred by str_banana
  nassert(str_yellow->rc == 2); // referred by str_yellow, hash1
  
  nassert(hash1->rc == 1); // ensure that rc of hash1 has not changed
  nassert(Object_len(hash1) == 2);

  Object* expecting_red = Object_accept(Object_bop_hget(hash1, str_apple));
  nassert(Object_cmp(expecting_red, str_red) ==  0);
  nassert(expecting_red->rc == 3); // referred by red, hash1, expecting_red

  // modifying keys after they have been inserted into hash
  Object_bop_addx_char(str_apple, '2');
  Object* str_apple2 = QSTRING_NEW1("apple2");
  nassert(Object_cmp(str_apple, str_apple2) == 0);

  // show that altering the string 'apple' to 'apple2' does not change 
  // key-value store in the Hash.
  Object* expecting_red2 = Object_accept(Object_bop_hget(hash1, QSTRING_NEW0("apple")));
  nassert(Object_cmp(expecting_red2, str_red) == 0);

  Util_heading1(0, "HASH OPERATIONS");

  //===========================================================================
  Util_heading1(1, "Environment Operations");
  /* Object* env1 = Object_new(SYMBOL_ENVIRONMENT, 1, Environment_new(NULL)); */
  Util_heading1(0, "Environment Operations");

  //===========================================================================
  
  Util_heading1(1, "RUNTIME DONE");
  Object_system_print();
  Object_system_done();
  Symbol_system_done();
  ErrorSystem_done();
  Util_heading1(0, "RUNTIME DONE");

  nassert_report();
  return nassert_exit_code();
}
