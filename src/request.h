#ifndef REQUEST_H_
#define REQUEST_H_

#include "string_dict.h"
#include "strings.h"

enum request_verb_enum {
  R_GET,
  R_POST,
  R_PUT,
  R_PATCH,
  R_DELETE,
  R_OPTIONS,
  R_UNKNOWN
};

typedef struct {
  enum request_verb_enum verb;
  string_slice_t path;
  dict_t headers;
  string_slice_t body;

  string_slice_t user_agent;
  string_slice_list_t accepted_encodings;
} request_t;

unsigned short int request_parse_from_string(string_t *request_str,
                                             request_t *out);

#endif
