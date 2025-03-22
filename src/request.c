#include "request.h"
#include "string_dict.h"

enum request_verb_enum verb_from_slice(string_slice_t *slice) {
  if (string_slice_compare_cstr(slice, "GET")) {
    return R_GET;
  }

  if (string_slice_compare_cstr(slice, "POST")) {
    return R_POST;
  }

  if (string_slice_compare_cstr(slice, "GET")) {
    return R_PUT;
  }

  if (string_slice_compare_cstr(slice, "PATCH")) {
    return R_PATCH;
  }

  if (string_slice_compare_cstr(slice, "DELETE")) {
    return R_DELETE;
  }

  if (string_slice_compare_cstr(slice, "OPTIONS")) {
    return R_OPTIONS;
  }

  return R_UNKNOWN;
}

unsigned short int request_parse_from_string(string_t *request_str,
                                             request_t *out) {

  string_slice_list_t request_lines = string_split(request_str, "\r\n");
  string_slice_t request_line = request_lines.head[0];
  string_slice_list_t request_values = string_slice_split(&request_line, " ");

  if (request_values.length < 2) {
    return 0;
  }

  enum request_verb_enum verb = verb_from_slice(&request_values.head[0]);
  if (verb == R_UNKNOWN) {
    return 0;
  }

  out->verb = verb;
  out->path = request_values.head[1];

  out->headers = string_dict_new(50);

  for (size_t i = 1; i < request_lines.length - 2; i++) {
    request_line = request_lines.head[i];
    string_slice_list_t parts = string_slice_split(&request_line, ": ");

    if (parts.length != 2) {
      continue;
    }

    string_slice_t *key = parts.head;
    string_slice_t *val = parts.head + 1;

    string_dict_push(&out->headers, key, val);
  }

  string_slice_t *user_agent =
      string_dict_get_cstr(&out->headers, "User-Agent");
  if (user_agent == NULL) {
    goto error_and_cleanup;
  }

  out->user_agent = *user_agent;

  string_slice_t *accept_encodings =
      string_dict_get_cstr(&out->headers, "Accept-Encoding");

  if (accept_encodings == NULL) {
    out->accepted_encodings = string_slice_list_new(0);
  } else {
    out->accepted_encodings = string_slice_split(accept_encodings, ", ");
  }

  out->body = request_lines.head[request_lines.length - 1];

  return 1;

error_and_cleanup:
  string_dict_free(&out->headers);
  return 0;
}
