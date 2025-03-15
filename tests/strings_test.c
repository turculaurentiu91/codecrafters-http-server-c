#include <assert.h>
#include <stdio.h>

#include "../src/strings.h"

void test_new_from_cstr() {
  char *c_str = "test";
  string_t string = string_new_from_cstr(c_str, 5);

  assert(c_str != string.head);
  assert(string.length == 4);
  assert(string.capacity == 4);
}

void test_string_free() {
  string_t string = string_new_from_cstr("test", 5);
  string_free(&string);

  assert(string.head == NULL);
  assert(string.capacity == 0);
  assert(string.length == 0);
}

void test_string_compare() {
  string_t a = string_new_from_cstr("test", 4);
  string_t b = string_new_from_cstr("test", 4);
  string_t c = string_new_from_cstr("test not eq", 12);

  short unsigned int eq = string_compare(&a, &b);
  short unsigned int not_eq = string_compare(&a, &c);

  string_free(&b);
  short unsigned int not_eq_freed_1 = string_compare(&a, &b);
  short unsigned int not_eq_freed_2 = string_compare(&b, &c);

  assert(eq > 0);
  assert(not_eq == 0);
  assert(not_eq_freed_1 == 0);
  assert(not_eq_freed_2 == 0);
}

void test_string_slice() {
  string_t string = string_new_from_cstr("test test", 10);
  string_slice_t slice = string_slice(&string, 5, 4);

  assert(slice.length == 4);
  assert(slice.head == string.head + 5);
  assert(slice.head[0] == 't');
  assert(slice.head[1] == 'e');
  assert(slice.head[2] == 's');
  assert(slice.head[3] == 't');
}

void test_string_slice_overflow() {
  string_t string = string_new_from_cstr("test test", 10);
  string_slice_t slice = string_slice(&string, 5, 10);

  assert(slice.length == 4);
  assert(slice.head == string.head + 5);
  assert(slice.head[0] == 't');
  assert(slice.head[1] == 'e');
  assert(slice.head[2] == 's');
  assert(slice.head[3] == 't');
}

void test_string_slice_overflow_start() {
  string_t string = string_new_from_cstr("test test", 10);
  string_slice_t slice = string_slice(&string, 15, 10);

  assert(slice.length == 0);
  assert(slice.head == NULL);
}

void test_string_compare_slice() {
  string_t a = string_new_from_cstr("test test", 10);
  string_slice_t b = string_slice(&a, 0, a.length);
  string_slice_t c = string_slice(&a, 0, 4);

  short unsigned int eq = string_compare_slice(&a, &b);
  short unsigned int not_eq = string_compare_slice(&a, &c);

  string_free(&a);
  short unsigned int not_eq_freed_1 = string_compare_slice(&a, &b);
  short unsigned int not_eq_freed_2 = string_compare_slice(&a, &c);

  assert(eq > 0);
  assert(not_eq == 0);
  assert(not_eq_freed_1 == 0);
  assert(not_eq_freed_2 == 0);
}

void test_string_slice_compare() {
  string_t string = string_new_from_cstr("test test", 10);
  string_slice_t a = string_slice(&string, 0, string.length);
  string_slice_t b = string_slice(&string, 0, string.length);
  string_slice_t c = string_slice(&string, 0, 4);

  short unsigned int eq = string_slice_compare(&a, &b);
  short unsigned int not_eq = string_slice_compare(&a, &c);

  assert(eq > 0);
  assert(not_eq == 0);
}

void test_string_compare_cstr() {
  string_t a = string_new_from_cstr("test", 5);
  const char *b = "test";
  const char *c = "no test";
  const char *d = "ttst";

  unsigned short int eq = string_compare_cstr(&a, b);
  unsigned short int not_eq = string_compare_cstr(&a, c);
  unsigned short int not_eq2 = string_compare_cstr(&a, d);

  assert(eq > 0);
  assert(not_eq == 0);
  assert(not_eq2 == 0);
}

void test_string_slice_compare_cstr() {
  string_t string = string_new_from_cstr("test", 5);
  string_slice_t a = string_slice(&string, 0, a.length);
  const char *b = "test";
  const char *c = "no test";
  const char *d = "ttst";

  unsigned short int eq = string_slice_compare_cstr(&a, b);
  unsigned short int not_eq = string_slice_compare_cstr(&a, c);
  unsigned short int not_eq2 = string_slice_compare_cstr(&a, d);

  assert(eq > 0);
  assert(not_eq == 0);
  assert(not_eq2 == 0);
}

void test_string_slice_list() {
  string_t string = string_new_from_cstr("test test", 10);
  string_slice_t a = string_slice(&string, 0, 4);
  string_slice_t b = string_slice(&string, 5, 4);

  string_slice_list_t list = string_slice_list_new(1);
  string_slice_list_append(&list, a);
  string_slice_list_append(&list, b);

  assert(list.capacity == 2);
  assert(list.length == 2);
  assert(list.head[0].head != list.head[1].head);
  assert(string_slice_compare(&list.head[0], &list.head[1]) > 0);
}

void test_string_split() {
  string_t string = string_new_from_cstr("test: some: string: to: split", 30);
  string_slice_list_t list = string_split(&string, ": ");

  assert(list.length == 5);
  assert(list.head[0].length == 4);
  assert(string_slice_compare_cstr(&list.head[0], "test") > 0);
  assert(list.head[1].length == 4);
  assert(string_slice_compare_cstr(&list.head[1], "some") > 0);
  assert(list.head[2].length == 6);
  assert(string_slice_compare_cstr(&list.head[2], "string") > 0);
  assert(list.head[3].length == 2);
  assert(string_slice_compare_cstr(&list.head[3], "to") > 0);
  assert(list.head[4].length == 5);
  assert(string_slice_compare_cstr(&list.head[4], "split") > 0);
}

int main() {
  test_new_from_cstr();
  test_string_free();
  test_string_compare();
  test_string_slice();
  test_string_slice_overflow();
  test_string_slice_overflow_start();
  test_string_compare_slice();
  test_string_slice_compare();
  test_string_compare_cstr();
  test_string_slice_compare_cstr();
  test_string_slice_list();
  test_string_split();
}
