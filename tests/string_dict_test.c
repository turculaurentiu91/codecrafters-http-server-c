#include <assert.h>
#include <stdio.h>

#include "../src/string_dict.h"

void test_string_dict_new() {
  dict_t dict = string_dict_new(5);

  assert(dict.dict != NULL);
  assert(dict.collisions != NULL);
  assert(dict.dict_size == 5);
  assert(dict.dict_capacity == 5);
  assert(dict.collision_size == 0);
  assert(dict.collision_capacity == 5);

  string_dict_free(&dict);
}

void test_string_dict_free() {
  dict_t dict = string_dict_new(5);
  string_dict_free(&dict);

  assert(dict.dict == NULL);
  assert(dict.collisions == NULL);
  assert(dict.dict_size == 0);
  assert(dict.dict_capacity == 0);
  assert(dict.collision_size == 0);
  assert(dict.collision_capacity == 0);
}

void test_string_dict_push_and_get() {
  string_t key_str = string_new_from_cstr("key1", 5);
  string_t value_str = string_new_from_cstr("value1", 7);
  string_slice_t key = string_slice(&key_str, 0, key_str.length);
  string_slice_t value = string_slice(&value_str, 0, value_str.length);

  dict_t dict = string_dict_new(5);
  string_dict_push(&dict, &key, &value);

  string_slice_t *retrieved = string_dict_get(&dict, &key);
  assert(retrieved != NULL);
  assert(string_slice_compare(retrieved, &value) > 0);

  string_dict_free(&dict);
  string_free(&key_str);
  string_free(&value_str);
}

void test_string_dict_multiple_entries() {
  string_t key_str1 = string_new_from_cstr("key1", 5);
  string_t value_str1 = string_new_from_cstr("value1", 7);
  string_slice_t key1 = string_slice(&key_str1, 0, key_str1.length);
  string_slice_t value1 = string_slice(&value_str1, 0, value_str1.length);

  string_t key_str2 = string_new_from_cstr("key2", 5);
  string_t value_str2 = string_new_from_cstr("value2", 7);
  string_slice_t key2 = string_slice(&key_str2, 0, key_str2.length);
  string_slice_t value2 = string_slice(&value_str2, 0, value_str2.length);

  dict_t dict = string_dict_new(5);
  string_dict_push(&dict, &key1, &value1);
  string_dict_push(&dict, &key2, &value2);

  string_slice_t *retrieved1 = string_dict_get(&dict, &key1);
  assert(retrieved1 != NULL);
  assert(string_slice_compare(retrieved1, &value1) > 0);

  string_slice_t *retrieved2 = string_dict_get(&dict, &key2);
  assert(retrieved2 != NULL);
  assert(string_slice_compare(retrieved2, &value2) > 0);

  string_dict_free(&dict);
  string_free(&key_str1);
  string_free(&value_str1);
  string_free(&key_str2);
  string_free(&value_str2);
}

void test_string_dict_update_value() {
  string_t key_str = string_new_from_cstr("key1", 5);
  string_t value_str1 = string_new_from_cstr("value1", 7);
  string_t value_str2 = string_new_from_cstr("updated", 8);
  string_slice_t key = string_slice(&key_str, 0, key_str.length);
  string_slice_t value1 = string_slice(&value_str1, 0, value_str1.length);
  string_slice_t value2 = string_slice(&value_str2, 0, value_str2.length);

  dict_t dict = string_dict_new(5);
  string_dict_push(&dict, &key, &value1);
  string_dict_push(&dict, &key, &value2); // Update with new value

  string_slice_t *retrieved = string_dict_get(&dict, &key);
  assert(retrieved != NULL);
  assert(string_slice_compare(retrieved, &value2) > 0);

  string_dict_free(&dict);
  string_free(&key_str);
  string_free(&value_str1);
  string_free(&value_str2);
}

void test_string_dict_get_nonexistent() {
  string_t key_str = string_new_from_cstr("key1", 5);
  string_t nonexistent_str = string_new_from_cstr("nonexistent", 12);
  string_t value_str = string_new_from_cstr("value1", 7);
  string_slice_t key = string_slice(&key_str, 0, key_str.length);
  string_slice_t nonexistent = string_slice(&nonexistent_str, 0, nonexistent_str.length);
  string_slice_t value = string_slice(&value_str, 0, value_str.length);

  dict_t dict = string_dict_new(5);
  string_dict_push(&dict, &key, &value);

  string_slice_t *retrieved = string_dict_get(&dict, &nonexistent);
  assert(retrieved == NULL);

  string_dict_free(&dict);
  string_free(&key_str);
  string_free(&nonexistent_str);
  string_free(&value_str);
}

int main() {
  test_string_dict_new();
  test_string_dict_free();
  test_string_dict_push_and_get();
  test_string_dict_multiple_entries();
  test_string_dict_update_value();
  test_string_dict_get_nonexistent();
  
  printf("All string_dict tests passed!\n");
  return 0;
}
