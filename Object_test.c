#include <stdio.h>
#include <stdlib.h>
#include "nassert.h"
#include "Symbol.h"
#include "String.h"
#include "Number.h"
#include "Object.h"

Object* fn_println(Function* fn, Object* env, Object* argv) {
  assert(fn != NULL);
  assert(argv != NULL);
  if(Object_is_null(argv)) {
    printf("\n");
    return NULL;
  }
  assert(Object_type(argv) == SYMBOL_LIST);
  /* printf("fn_println. Called\n"); */
  /* ObjectUtil_eprintf("fn_println. argv=%v\n", argv); */
  /* ObjectUtil_eprintf("fn_println. len(argv)=%d\n", Object_len(argv)); */
  int argv_len = Object_len(argv);
  Object* tmp;
  Symbol* tmp_type;
  int i = 0;
  ListIter* argv_iter = ListIter_new(argv->impl);
  ListIter_next(argv_iter);
  ListIter_next(argv_iter);
  while(!ListIter_at_end(argv_iter)) {
    tmp = ListIter_get_val(argv_iter);
    /* ObjectUtil_eprintf("debug. tmp=%v\n", tmp); */
    tmp_type = Object_type(tmp);
    if(i > 0) {
      printf(" ");
    }
    if(tmp_type == SYMBOL_STRING) {
      printf("%s", (char*)(((String*)tmp->impl)->buf));
    } 
    else
    if(tmp_type == SYMBOL_NUMBER) {
      printf("%f", (((Number*)tmp->impl)->rep));
    }
    else {
      Object_print(tmp);
    }
    ListIter_next(argv_iter);
    i++;
  }
  ListIter_del(argv_iter);
  printf("\n");
  return NULL;
}

Object* fn_add(Function* fn, Object* env, Object* argv) {
  // This Function was defined with named-params, and so we should be able to
  // 'a' and 'b' from the environment.
  /* printf("F=%s L=%d. rtcount = %zu\n", __FILE__, __LINE__, Object_system_rtcount()); */
  Object* a = Object_accept(Object_bop_hget(env, QSYMBOL("a")));
  Object* b = Object_accept(Object_bop_hget(env, QSYMBOL("b")));
  ObjectUtil_eprintf("fn_add. got a = %v\n", a);
  ObjectUtil_eprintf("fn_add. got b = %v\n", b);
  Object* ret = Object_accept(Object_bop_add(a, b));
  Object_rc_decr(a);
  Object_rc_decr(b);
  return ret;
}

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

  Object* sym2 = QSYMBOL_NEW1("+");
  nassert(Object_type(sym2) == SYMBOL_SYMBOL);

  Util_heading1(0, "SYMBOL OPERATIONS");

  //===========================================================================
  Util_heading1(1, "STRING OPERATIONS");

  size_t objsize = Object_system_size();
  printf("old object_system_size() = %ld\n", objsize);
  Object* str1 = Object_new(SYMBOL_STRING, 1, String_new("Hello there 001"));
  Object* str2 = Object_new(SYMBOL_STRING, 1, String_new("Hello there 002"));
  Object* str3 = Object_new(SYMBOL_STRING, 1, String_new("Hello there 003"));
  printf("new object_system_size() = %ld\n", Object_system_size());
  // we gain 3 more objects
  nassert(Object_system_size() - objsize == 3);
  Object_system_gc();
  // GC does not delete any of these objects
  nassert(Object_system_size() - objsize  == 3);
  Object* tmp1 = Object_new(SYMBOL_STRING, 0, String_new("This too will pass."));
  tmp1->returning = 0; // manually mark it as no longer returning
  nassert(tmp1->rc == 0);
  // one more object gained
  nassert(Object_system_size() - objsize == 4);
  objsize = Object_system_size();
  Object_system_gc();
  // this tmp1 object is deleted
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
  nassert(res1->rc == 1);
  ObjectUtil_eprintf("%v + %v = %v\n", num1, num2, res1);
  Object* res1_expected = Object_new(SYMBOL_NUMBER, 1, Number_new(7));
  nassert(Object_cmp(res1, res1_expected) == 0);
  /*printf("res1 = "); Object_print(res1); printf("\n");*/
  ObjectUtil_eprintf("res1 = %v\n", res1);
  nassert(num1->rc == 1);
  nassert(num2->rc == 1);

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
  Util_heading1(1, "BOOLEAN OPERATIONS");
  Object* null_obj1 = Object_new_null();
  Object* truthy_obj1 = QSYMBOL_NEW1("True");
  nassert(Object_is_truthy(truthy_obj1));
  nassert(!Object_is_truthy(null_obj1));
  Util_heading1(0, "BOOLEAN OPERATIONS");

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
  nassert(Object_len(list1) == 2);
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
  nassert(list_c_a->returning == 0);
  nassert(list_c_b->returning == 0);
  nassert(list_c_c->returning == 0);
  Object_reject(Object_bop_push(list_c_a, list_c_b)); // A -> B
  Object_reject(Object_bop_push(list_c_b, list_c_c)); // B -> C
  Object_reject(Object_bop_push(list_c_c, list_c_a)); // C -> A
  nassert(list_c_a->rc == 2);
  nassert(list_c_b->rc == 2);
  nassert(list_c_c->rc == 2);
  nassert(list_c_a->returning == 0);
  nassert(list_c_b->returning == 0);
  nassert(list_c_c->returning == 0);
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

  int clist_begsize = Object_system_size();
  printf("clist_begsize = %d\n", clist_begsize);
  Object_system_gc();
  int clist_endsize = Object_system_size();
  printf("clist_endsize = %d\n", clist_endsize);
  // test that the total number of objects has gone down by 6 after GC
  // 3 for each of the lists, 3 for each Object<String> in each list.
  nassert(clist_begsize - clist_endsize == 6);

  // test head, tail, rest
  Object* list5 = Object_new_list(1, 3, QNUMBER(5.1), QNUMBER(5.2), QNUMBER(5.3));
  ObjectUtil_eprintf("list5 = %v\n", list5);
  nassert(Object_len(list5) == 3);
  Object* list5_e1 = Object_accept(Object_uop_head(list5));
  nassert(Object_cmp(list5_e1, QNUMBER(5.1)) == 0);
  Object* list5_en = Object_accept(Object_uop_tail(list5));
  nassert(Object_cmp(list5_en, QNUMBER(5.3)) == 0);
  Object* list5_r1 = Object_accept(Object_uop_rest(list5));
  ObjectUtil_eprintf("list5_r1 = %v\n", list5_r1);
  Object* list5_r1_exp = Object_new_list(1, 2, QNUMBER(5.2), QNUMBER(5.3));
  nassert(Object_cmp(list5_r1, list5_r1_exp) == 0);

  // test simple deletion of elements
  Object* list6 = Object_new_list(1, 3, QNUMBER(6.1), QNUMBER(6.2), QNUMBER(6.3));
  int obj_begsize = Object_system_size();
  Object_assign(&list6, NULL);
  int obj_endsize = Object_system_size();
  printf("list6. obj_begsize = %d\n", obj_begsize);
  printf("list6. obj_endsize = %d\n", obj_endsize);
  // we lose 4 objects, one for the list, 3 for the members
  nassert(obj_begsize - obj_endsize == 4);
  

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

  nassert(str_apple->rc == 1);
  Object* expecting_red = Object_accept(Object_bop_hget(hash1, str_apple));
  nassert(Object_cmp(expecting_red, str_red) ==  0);
  nassert(expecting_red->rc == 3); // referred by str_red, hash1, expecting_red
  nassert(str_red->rc == 3);
  nassert(str_apple->rc == 1); // the rc of str_apple is left alone
  Object_assign(&expecting_red, NULL);
  nassert(str_red->rc == 2);

  // modifying keys after they have been inserted into hash
  Object_bop_addx_char(str_apple, '2');
  Object* str_apple2 = QSTRING_NEW1("apple2");
  nassert(Object_cmp(str_apple, str_apple2) == 0);

  // show that altering the string 'apple' to 'apple2' does not change 
  // key-value store in the Hash.
  Object* expecting_red2 = Object_accept(Object_bop_hget(hash1, QSTRING_NEW0("apple")));
  nassert(Object_cmp(expecting_red2, str_red) == 0);

  nassert(ObjectSystem_count_matching_number(5.987) == 0);
  Object* hash2 = QHASH_NEW1();
  Object* key1 = QSYMBOL_NEW1("key1");
  Object* val1 = QNUMBER_NEW1(5.987);
  Object* val2 = QNUMBER_NEW1(6.987);
  nassert(key1->rc == 1);
  Object_reject(Object_top_hset(hash2, key1, val1));
  nassert(ObjectSystem_count_matching_number(5.987) == 1);
  nassert(ObjectSystem_count_matching_number(6.987) == 1);
  nassert(val1->rc == 2);
  Object_assign(&val1, NULL);
  nassert(Object_is_null(val1));
  nassert(ObjectSystem_count_matching_number(5.987) == 1);
  Object_reject(Object_top_hset(hash2, key1, val2));
  nassert(ObjectSystem_count_matching_number(5.987) == 0);
  nassert(ObjectSystem_count_matching_number(6.987) == 1);
  nassert(val2->rc == 2);

  Util_heading1(0, "HASH OPERATIONS");

  //keywords: circle, circular
  printf("Creating circular hashes ...\n");
  Object* str_other = QSTRING_NEW1("other");
  Object* hash_c_a = QHASH_NEW1();
  Object* hash_c_b = QHASH_NEW1();
  Object_top_hset(hash_c_a, str_other, hash_c_b);
  Object_top_hset(hash_c_b, str_other, hash_c_a);
  nassert(hash_c_a->rc == 2);
  nassert(hash_c_b->rc == 2);
  Object_assign(&hash_c_a, NULL);
  Object_assign(&hash_c_b, NULL);
  nassert(hash_c_a->rc == 1);
  nassert(hash_c_b->rc == 1);

  int chash_begsize = Object_system_size();
  Object_system_gc();
  int chash_endsize = Object_system_size();
  nassert(chash_begsize - chash_endsize == 2);

  //===========================================================================
  Util_heading1(1, "Environment Operations");
  Object* env1 = Object_new(SYMBOL_ENVIRONMENT, 1, Environment_new());
  Object_top_hset(env1, str_apple, str_red);
  int str_apple_rc = str_apple->rc;
  int str_red_rc = str_red->rc;
  printf("bef. str_apple_rc = %d\n", str_apple_rc);
  printf("bef. str_red_rc = %d\n", str_red_rc);
  nassert(Object_len(env1) == 1);
  nassert(
      Object_cmp(
        Object_bop_hget(env1, str_apple),
        str_red
      ) == 0
  )
  printf("aft. str_apple.rc = %d\n", str_apple->rc);
  printf("aft. str_red.rc = %d\n", str_red->rc);
  // the rc for tor the key str_apple is left alone.
  nassert(str_apple_rc == str_apple->rc);
  // the rc for the val str_red is left alone
  nassert(str_red_rc == str_red->rc);
  Util_heading1(0, "Environment Operations");

  Object* env2 = Object_new(SYMBOL_ENVIRONMENT, 1, Environment_new());
  Object* env2_key1 = QSYMBOL_NEW1("key1");
  Object* env2_val1 = QNUMBER_NEW1(11.987);
  Object* env2_val2 = QNUMBER_NEW1(12.987);
  nassert(env2_val1->rc == 1);
  nassert(ObjectSystem_count_matching_number(11.987) == 1);
  nassert(ObjectSystem_count_matching_number(12.987) == 1);
  Object_reject(Object_top_hset(env2, env2_key1, env2_val1));
  nassert(Object_len(env2) == 1);
  nassert(env2_val1->rc == 2);
  Object_assign(&env2_val1, NULL);
  nassert(Object_is_null(env2_val1));
  nassert(ObjectSystem_count_matching_number(11.987) == 1);
  Object_reject(Object_top_hset(env2, env2_key1, env2_val2));
  nassert(env2_val2->rc == 2);
  nassert(ObjectSystem_count_matching_number(11.987) == 0);

  //===========================================================================
  
  Util_heading1(1, "Function Operations");

  Object* fn1 = Object_new(SYMBOL_FUNCTION, 1, Function_new(
    QSYMBOL("println"),  // name
    NULL,                // env
    fn_println,          // impl
    -1, 
    NULL, 
    NULL
  ));
  nassert(fn1 != NULL);
  nassert(Object_type(fn1) == SYMBOL_FUNCTION);

  Object_reject(
    Object_bop_call(fn1, Object_new_list(0, 2, QSTRING("The value of pi is "), QNUMBER(3.14)))
  );

  Object* fn2 = Object_new(
    SYMBOL_FUNCTION, 
    1, 
    Function_new(QSYMBOL("add"), NULL, fn_add, 2, Object_new_list(1, 2, QSYMBOL("a"), QSYMBOL("b")), NULL)
  );
  nassert(fn2 != NULL);
  nassert(Object_type(fn2) == SYMBOL_FUNCTION);

  Object* fnres1 = Object_accept(Object_bop_call(fn2, Object_new_list(1, 2, QNUMBER(1.2), QNUMBER(1.3))));
  ObjectUtil_eprintf("fnres1 = %v\n", fnres1);
  nassert(Object_cmp(fnres1, QNUMBER(2.5)) == 0);

  Util_heading1(0, "Function Operations");

  //===========================================================================

  Util_heading1(1, "Extra GC Tests 1");
  objsize = Object_system_size();
  Object* gc1_num1 = QNUMBER_NEW1(19.39);
  Object* gc1_num2 = QNUMBER_NEW1(19.40);
  Object* gc1_num3 = QNUMBER_NEW1(19.41);
  // we gain these 3 number objects.
  nassert(Object_system_size() - objsize == 3);
  nassert(ObjectSystem_count_matching_number(19.39) == 1);
  nassert(ObjectSystem_count_matching_number(19.40) == 1);
  nassert(ObjectSystem_count_matching_number(19.41) == 1);
  Object_system_gc();
  // these numbers are NOT deleted.
  nassert(Object_system_size() - objsize == 3);
  nassert(ObjectSystem_count_matching_number(19.39) == 1);
  nassert(ObjectSystem_count_matching_number(19.40) == 1);
  nassert(ObjectSystem_count_matching_number(19.41) == 1);
  Object_assign(&gc1_num1, NULL);
  nassert(Object_is_null(gc1_num1));
  nassert(ObjectSystem_count_matching_number(19.39) == 0);
  nassert(ObjectSystem_count_matching_number(19.40) == 1);
  nassert(ObjectSystem_count_matching_number(19.41) == 1);
  Object_assign(&gc1_num2, NULL);
  nassert(Object_is_null(gc1_num2));
  nassert(ObjectSystem_count_matching_number(19.39) == 0);
  nassert(ObjectSystem_count_matching_number(19.40) == 0);
  nassert(ObjectSystem_count_matching_number(19.41) == 1);
  Object_assign(&gc1_num3, NULL);
  nassert(Object_is_null(gc1_num3));
  nassert(ObjectSystem_count_matching_number(19.39) == 0);
  nassert(ObjectSystem_count_matching_number(19.40) == 0);
  nassert(ObjectSystem_count_matching_number(19.41) == 0);

  Util_heading1(0, "Extra GC Tests 1");
  
  nassert(Object_system_rtcount() == 0);

_shutdown:
  Util_heading1(1, "RUNTIME DONE");
  Object_system_print();
  Object_system_done();
  Symbol_system_done();
  ErrorSystem_done();
  Util_heading1(0, "RUNTIME DONE");

  nassert_report();
  return nassert_exit_code();
}
