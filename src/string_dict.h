#ifndef STRING_DICT_H
#define STRING_DICT_H

#include "strings.h"

struct node_ {
  string_slice_t *key;
  string_slice_t *value;
  struct node_ *next_collision;
};

typedef struct node_ node_t;

typedef struct {
  node_t *dict;
  node_t *collisions;
  size_t dict_size;
  size_t dict_capacity;
  size_t collision_size;
  size_t collision_capacity;
} dict_t;

dict_t string_dict_new(size_t initial_capacity);
void string_dict_push(dict_t *dict, string_slice_t *key, string_slice_t *value);
string_slice_t *string_dict_get(dict_t *dict, string_slice_t *key);
void string_dict_free(dict_t *dict);

#endif
