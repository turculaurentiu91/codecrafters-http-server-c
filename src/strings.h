#include <stddef.h>

#ifndef STRINGS_H
#define STRINGS_H

typedef struct {
  char *head;
  size_t length;
  size_t capacity;
} string_t;

typedef struct {
  char *head;
  size_t length;
} string_slice_t;

typedef struct {
  string_slice_t *head;
  size_t length;
  size_t capacity;
} string_slice_list_t;

string_t string_new(size_t capacity);

/**
 * Create a new string from a c string by copying
 * the data.
 *
 * @param size_t length - must include the null at the end of
 * the provided c string
 */
string_t string_new_from_cstr(char *c_str, size_t length);

string_slice_t string_slice(string_t *string, size_t start, size_t length);
string_slice_t string_slice_Slice(string_slice_t *string_slice, size_t start,
                                  size_t length);

void string_free(string_t *string);

short unsigned int string_compare(string_t *a, string_t *b);
short unsigned int string_compare_slice(string_t *a, string_slice_t *b);
short unsigned int string_compare_cstr(string_t *a, const char *b);
short unsigned int string_slice_compare(string_slice_t *a, string_slice_t *b);
short unsigned int string_slice_compare_cstr(string_slice_t *a, const char *b);

string_slice_list_t string_split(string_t *string, const char *split_on);
string_slice_list_t string_slice_split(string_slice_t *string,
                                       const char *split_on);

string_slice_list_t string_slice_list_new(size_t capacity);
void string_slice_list_append(string_slice_list_t *list, string_slice_t slice);

#endif
