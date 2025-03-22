#include "../src/request.h"
#include <assert.h>
#include <string.h>

void test_parse_request_empty_str() {
  char *req_cstr = "";
  string_t req_str = string_new_from_cstr(req_cstr, strlen(req_cstr) + 1);
  request_t request;
  unsigned short int result = request_parse_from_string(&req_str, &request);

  assert(result == 0);

  string_free(&req_str);
}

void test_parse_request_invalid_verb() {
  char *req_cstr = "ASD / HTTP/1.1\r\nUser-Agent: test\r\n\r\n";
  string_t req_str = string_new_from_cstr(req_cstr, strlen(req_cstr) + 1);
  request_t request;
  unsigned short int result = request_parse_from_string(&req_str, &request);

  assert(result == 0);

  string_free(&req_str);
}

void test_parse_request_missing_user_agent() {
  char *req_cstr = "GET / HTTP/1.1\r\nX-Some-Header: test\r\n\r\n";
  string_t req_str = string_new_from_cstr(req_cstr, strlen(req_cstr) + 1);
  request_t request;
  unsigned short int result = request_parse_from_string(&req_str, &request);

  assert(result == 1);

  string_free(&req_str);
}

void test_parse_request_get() {
  char *req_cstr =
      "GET / HTTP/1.1\r\nUser-Agent: test\r\nX-Some-Header: test\r\n\r\n";
  string_t req_str = string_new_from_cstr(req_cstr, strlen(req_cstr) + 1);
  request_t request;
  unsigned short int result = request_parse_from_string(&req_str, &request);

  assert(result == 1);
  assert(request.verb == R_GET);
  assert(string_slice_compare_cstr(&request.path, "/") > 0);

  string_slice_t *user_agent =
      string_dict_get_cstr(&request.headers, "User-Agent");
  assert(string_slice_compare_cstr(user_agent, "test") > 0);

  string_slice_t *custom_header =
      string_dict_get_cstr(&request.headers, "X-Some-Header");
  assert(string_slice_compare_cstr(custom_header, "test") > 0);

  assert(request.accepted_encodings.length == 0);

  string_free(&req_str);
}

void test_parse_request_get_encodings() {
  char *req_cstr = "GET / HTTP/1.1\r\nUser-Agent: test\r\nX-Some-Header: "
                   "test\r\nAccept-Encoding: gzip, some-encoding\r\n\r\n";
  string_t req_str = string_new_from_cstr(req_cstr, strlen(req_cstr) + 1);
  request_t request;
  unsigned short int result = request_parse_from_string(&req_str, &request);

  assert(result == 1);
  assert(request.verb == R_GET);
  assert(string_slice_compare_cstr(&request.path, "/") > 0);

  string_slice_t *user_agent =
      string_dict_get_cstr(&request.headers, "User-Agent");
  assert(string_slice_compare_cstr(user_agent, "test") > 0);

  string_slice_t *custom_header =
      string_dict_get_cstr(&request.headers, "X-Some-Header");
  assert(string_slice_compare_cstr(custom_header, "test") > 0);

  assert(request.accepted_encodings.length == 2);
  string_slice_t enc1 = request.accepted_encodings.head[0];
  string_slice_t enc2 = request.accepted_encodings.head[1];

  assert(string_slice_compare_cstr(&enc1, "gzip") > 0);
  assert(string_slice_compare_cstr(&enc2, "some-encoding") > 0);

  string_free(&req_str);
}

void test_parse_request_post() {
  char *req_cstr = "POST /files/test.txt HTTP/1.1\r\n"
                   "User-Agent: test\r\n"
                   "X-Some-Header: test\r\n"
                   "Content-Type: application/octet-stream\r\n"
                   "Content-Length: 4\r\n\r\n"
                   "test";
  string_t req_str = string_new_from_cstr(req_cstr, strlen(req_cstr) + 1);
  request_t request;
  unsigned short int result = request_parse_from_string(&req_str, &request);

  assert(result == 1);
  assert(request.verb == R_POST);
  assert(string_slice_compare_cstr(&request.path, "/files/test.txt") > 0);

  string_slice_t *user_agent =
      string_dict_get_cstr(&request.headers, "User-Agent");
  assert(string_slice_compare_cstr(user_agent, "test") > 0);

  string_slice_t *custom_header =
      string_dict_get_cstr(&request.headers, "X-Some-Header");
  assert(string_slice_compare_cstr(custom_header, "test") > 0);

  string_slice_t *content_type =
      string_dict_get_cstr(&request.headers, "Content-Type");
  assert(string_slice_compare_cstr(content_type, "application/octet-stream") >
         0);

  assert(string_slice_compare_cstr(&request.body, "test") > 0);

  string_free(&req_str);
}

int main() {
  test_parse_request_empty_str();
  test_parse_request_invalid_verb();
  test_parse_request_missing_user_agent();
  test_parse_request_get();
  test_parse_request_get_encodings();
  test_parse_request_post();
}
