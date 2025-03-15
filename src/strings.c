#include <stdlib.h>
#include <string.h>

#include "strings.h"

string_t string_new(size_t capacity) {
  string_t string = {.head = malloc(sizeof(char) * capacity),
                     .length = 0,
                     .capacity = capacity};

  return string;
}

string_t string_new_from_cstr(char *c_str, size_t length) {
  // trim down the null character at the end of the c string from the length
  size_t trimmed_length = length - 1;

  string_t string = {.head = malloc(sizeof(char) * (trimmed_length)),
                     .length = trimmed_length,
                     .capacity = trimmed_length};

  memcpy(string.head, c_str, trimmed_length);

  return string;
}

void string_free(string_t *string) {
  free(string->head);
  string->head = NULL;
  string->capacity = 0;
  string->length = 0;
}

string_slice_t string_slice(string_t *string, size_t start, size_t length) {
  string_slice_t slice = {
      .head = NULL,
      .length = 0,
  };

  if (string->head == NULL || string->length == 0) {
    return slice;
  }

  if (start >= string->length) {
    return slice;
  }

  slice.head = string->head + start;
  slice.length = length;
  if (start + length > string->length) {
    slice.length = string->length - start;
  }

  return slice;
}

string_slice_t string_slice_slice(string_slice_t *string_slice, size_t start,
                                  size_t length) {
  string_slice_t new_slice = {
      .head = NULL,
      .length = 0,
  };

  if (string_slice->head == NULL || string_slice->length == 0) {
    return new_slice;
  }

  if (start >= string_slice->length) {
    return new_slice;
  }

  new_slice.head = string_slice->head + start;
  new_slice.length = length;
  if (start + length > string_slice->length) {
    new_slice.length = string_slice->length - start;
  }

  return new_slice;
}

short unsigned int string_compare(string_t *a, string_t *b) {
  if (a->head == NULL && b->head == NULL) {
    return 1;
  }

  if (a->head == NULL || a->length == 0) {
    return 0;
  }

  if (b->head == NULL || b->length == 0) {
    return 0;
  }

  if (a->length != b->length) {
    return 0;
  }

  short unsigned int result = 1;
  for (size_t i = 0; i < a->length; i++) {
    if (a->head[i] != b->head[i]) {
      result = 0;
      break;
    }
  }

  return result;
}

short unsigned int string_compare_slice(string_t *a, string_slice_t *b) {
  if (a->head == NULL && b->head == NULL) {
    return 1;
  }

  if (a->head == NULL || a->length == 0) {
    return 0;
  }

  if (b->head == NULL || b->length == 0) {
    return 0;
  }

  if (a->length != b->length) {
    return 0;
  }

  short unsigned int result = 1;
  for (size_t i = 0; i < a->length; i++) {
    if (a->head[i] != b->head[i]) {
      result = 0;
      break;
    }
  }

  return result;
}

short unsigned int string_slice_compare(string_slice_t *a, string_slice_t *b) {
  if (a->head == NULL && b->head == NULL) {
    return 1;
  }

  if (a->head == NULL || a->length == 0) {
    return 0;
  }

  if (b->head == NULL || b->length == 0) {
    return 0;
  }

  if (a->length != b->length) {
    return 0;
  }

  short unsigned int result = 1;
  for (size_t i = 0; i < a->length; i++) {
    if (a->head[i] != b->head[i]) {
      result = 0;
      break;
    }
  }

  return result;
}

short unsigned int string_compare_cstr(string_t *a, const char *b) {
  size_t b_len = strlen(b);
  if (a->head == NULL) {
    if (a->length == 0 && b_len == 0) {
      return 1;
    } else {
      return 0;
    }
  }

  if (a->length != b_len) {
    return 0;
  }

  short unsigned int result = 1;
  for (size_t i = 0; i < a->length; i++) {
    if (a->head[i] != b[i]) {
      result = 0;
      break;
    }
  }

  return result;
}

short unsigned int string_slice_compare_cstr(string_slice_t *a, const char *b) {
  size_t b_len = strlen(b);
  if (a->head == NULL) {
    if (a->length == 0 && b_len == 0) {
      return 1;
    } else {
      return 0;
    }
  }

  if (a->length != b_len) {
    return 0;
  }

  short unsigned int result = 1;
  for (size_t i = 0; i < a->length; i++) {
    if (a->head[i] != b[i]) {
      result = 0;
      break;
    }
  }

  return result;
}

string_slice_list_t string_slice_list_new(size_t capacity) {
  string_slice_list_t list = {
      .head = malloc(sizeof(string_slice_t) * capacity),
      .length = 0,
      .capacity = capacity,
  };

  return list;
}

void string_slice_list_append(string_slice_list_t *list, string_slice_t slice) {
  if (list->length == list->capacity) {
    size_t new_cap = list->capacity * 2;
    list->head = realloc(list->head, sizeof(string_slice_t) * new_cap);
    list->capacity = new_cap;
  }

  list->head[list->length] = slice;
  list->length++;
}

string_slice_list_t string_split(string_t *string, const char *split_on) {
  string_slice_list_t list = string_slice_list_new(10);
  size_t split_on_len = strlen(split_on);

  size_t last_slice_at = 0;
  for (size_t i = 0; i < string->length; i++) {
    string_slice_t cmp_slice = string_slice(string, i, split_on_len);
    if (string_slice_compare_cstr(&cmp_slice, split_on) > 0) {
      string_slice_t slice =
          string_slice(string, last_slice_at, i - last_slice_at);
      string_slice_list_append(&list, slice);
      last_slice_at = i + split_on_len;
      i = i + split_on_len - 1;
    }
  }

  string_slice_t slice =
      string_slice(string, last_slice_at, string->length - last_slice_at);
  string_slice_list_append(&list, slice);

  return list;
}

string_slice_list_t string_slice_split(string_slice_t *string_slice,
                                       const char *split_on) {
  string_slice_list_t list = string_slice_list_new(10);
  size_t split_on_len = strlen(split_on);

  size_t last_slice_at = 0;
  for (size_t i = 0; i < string_slice->length; i++) {
    string_slice_t cmp_slice =
        string_slice_slice(string_slice, i, split_on_len);
    if (string_slice_compare_cstr(&cmp_slice, split_on) > 0) {
      string_slice_t slice =
          string_slice_slice(string_slice, last_slice_at, i - last_slice_at);
      string_slice_list_append(&list, slice);
      last_slice_at = i + split_on_len;
      i = i + split_on_len - 1;
    }
  }

  string_slice_t slice = string_slice_slice(
      string_slice, last_slice_at, string_slice->length - last_slice_at);
  string_slice_list_append(&list, slice);

  return list;
}
