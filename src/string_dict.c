#include "string_dict.h"

#include <stdint.h>
#include <stdlib.h>

uint32_t simple_hash(string_slice_t *str) {
  uint32_t hash = 5381; // Initial value, a common starting point

  for (size_t i = 0; i < str->length; i++) {
    int c = str->head[i];
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
  }

  return hash;
}

uint32_t simple_hash_cstr(const char *str) {
  uint32_t hash = 5381; // Initial value, a common starting point
  int c;

  while ((c = *str++)) {
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
  }

  return hash;
}

dict_t string_dict_new(size_t initial_capacity) {
  dict_t dict = {
      .dict = malloc(sizeof(node_t) * initial_capacity),
      .collisions = malloc(sizeof(node_t) * initial_capacity),
      .dict_size = initial_capacity,
      .dict_capacity = initial_capacity,
      .collision_size = 0,
      .collision_capacity = initial_capacity,
  };

  node_t empty = {.key = NULL, .value = NULL, .next_collision = NULL};
  for (size_t i = 0; i < dict.dict_capacity; i++) {
    dict.dict[i] = empty;
  }

  return dict;
};

void string_dict_free(dict_t *dict) {
  free(dict->collisions);
  free(dict->dict);

  dict->dict = NULL;
  dict->dict_size = 0;
  dict->dict_capacity = 0;

  dict->collisions = NULL;
  dict->collision_size = 0;
  dict->collision_capacity = 0;
}

void string_dict_push(dict_t *dict, string_slice_t *key,
                      string_slice_t *value) {
  uint32_t hash = simple_hash(key);
  size_t dict_idx = hash % dict->dict_capacity;

  node_t *dict_node = dict->dict + dict_idx;
  if (dict_node->key == NULL || string_slice_compare(dict_node->key, key) > 0) {
    dict_node->key = key;
    dict_node->value = value;

    return;
  }

  node_t *last_node = dict_node;
  while (last_node->next_collision != NULL) {
    last_node = last_node->next_collision;
    if (string_slice_compare(last_node->key, key) > 0) {
      last_node->value = value;
      return;
    }
  }

  // push it to the collisions, expanding if necessary
  if (dict->collision_size == dict->collision_capacity) {
    dict->collisions = realloc(dict->collisions,
                               sizeof(node_t) * dict->collision_capacity * 2);
    dict->collision_capacity *= 2;
  }

  node_t new_node = {.key = key, .value = value, .next_collision = NULL};
  dict->collisions[dict->collision_size] = new_node;
  last_node->next_collision = dict->collisions + dict->collision_size;
  dict->collision_size++;
}

string_slice_t *string_dict_get(dict_t *dict, string_slice_t *key) {
  uint32_t hash = simple_hash(key);
  size_t dict_idx = hash % dict->dict_capacity;

  node_t *dict_node = dict->dict + dict_idx;
  if (dict_node->key == NULL) {
    return NULL;
  }

  if (string_slice_compare(dict_node->key, key) > 0) {
    return dict_node->value;
  }

  node_t *last_node = dict_node;
  while (last_node->next_collision != NULL) {
    last_node = last_node->next_collision;
    if (string_slice_compare(last_node->key, key) > 0) {
      return last_node->value;
    }
  }

  return NULL;
}

string_slice_t *string_dict_get_cstr(dict_t *dict, const char *key) {
  uint32_t hash = simple_hash_cstr(key);
  size_t dict_idx = hash % dict->dict_capacity;

  node_t *dict_node = dict->dict + dict_idx;
  if (dict_node->key == NULL) {
    return NULL;
  }

  if (string_slice_compare_cstr(dict_node->key, key) > 0) {
    return dict_node->value;
  }

  node_t *last_node = dict_node;
  while (last_node->next_collision != NULL) {
    last_node = last_node->next_collision;
    if (string_slice_compare_cstr(last_node->key, key) > 0) {
      return last_node->value;
    }
  }

  return NULL;
}
