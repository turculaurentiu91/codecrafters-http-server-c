#include <errno.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "strings.h"

int main() {
  // Disable output buffering
  setbuf(stdout, NULL);
  setbuf(stderr, NULL);

  // You can use print statements as follows for debugging, they'll be visible
  // when running tests.
  printf("Logs from your program will appear here!\n");

  int server_fd;
  struct sockaddr_in client_addr;
  unsigned int client_addr_len;

  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd == -1) {
    printf("Socket creation failed: %s...\n", strerror(errno));
    return 1;
  }

  // Since the tester restarts your program quite often, setting SO_REUSEADDR
  // ensures that we don't run into 'Address already in use' errors
  int reuse = 1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) <
      0) {
    printf("SO_REUSEADDR failed: %s \n", strerror(errno));
    return 1;
  }

  struct sockaddr_in serv_addr = {
      .sin_family = AF_INET,
      .sin_port = htons(4221),
      .sin_addr = {htonl(INADDR_ANY)},
  };

  if (bind(server_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) != 0) {
    printf("Bind failed: %s \n", strerror(errno));
    return 1;
  }

  int connection_backlog = 5;
  if (listen(server_fd, connection_backlog) != 0) {
    printf("Listen failed: %s \n", strerror(errno));
    return 1;
  }

  for (;;) {
    printf("Waiting for a client to connect...\n");
    client_addr_len = sizeof(client_addr);

    int client_fd =
        accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
    if (client_fd == -1) {
      printf("There was an error accepting client connection");
      return 1;
    }
    printf("Client connected\n");

    char msg_200[19] = "HTTP/1.1 200 OK\r\n\r\n";
    char msg_404[31] = "HTTP/1.1 404 Not Found\r\n\r\n";
    char msg_400[33] = "HTTP/1.1 400 Bad Request\r\n\r\n";

    string_t request_str = string_new(32 * 1024);
    size_t read_data = read(client_fd, request_str.head, request_str.capacity);
    if (read_data == -1) {
      printf("Could not read from the client soket");
      close(client_fd);
      continue;
    }

    request_str.length = read_data;
    string_slice_list_t request_lines = string_split(&request_str, "\r\n");
    string_slice_t request_line = request_lines.head[0];
    string_slice_list_t request_values = string_slice_split(&request_line, " ");

    size_t written;
    if (request_values.length < 2) {
      written = write(client_fd, &msg_400, 33);
      if (written == -1) {
        printf("Could not write to the client socket");
        close(client_fd);
        continue;
      }
    }

    string_slice_t path = request_values.head[1];
    string_slice_t path_start;
    if (path.length > 6) {
      path_start = string_slice_slice(&path, 0, 6);
    }

    if (string_slice_compare_cstr(&path, "/")) {
      written = write(client_fd, &msg_200, 19);
    } else if (path.length > 6 &&
               string_slice_compare_cstr(&path_start, "/echo/") > 0) {
      string_slice_t echo_val = string_slice_slice(&path, 6, path.length - 6);
      char res[1024 * 32];
      size_t res_size =
          snprintf(res, sizeof(res),
                   "HTTP/1.1 200 OK\r\nContent-Type: "
                   "text/plain\r\nContent-Length: %ld\r\n\r\n%.*s",
                   echo_val.length, (int)echo_val.length, echo_val.head);

      written = write(client_fd, res, res_size);
    } else {
      written = write(client_fd, &msg_404, 31);
    }

    if (written == -1) {
      printf("Could not write to the client socket");
      close(client_fd);
      continue;
    }

    close(client_fd);
  }

  close(server_fd);

  return 0;
}
