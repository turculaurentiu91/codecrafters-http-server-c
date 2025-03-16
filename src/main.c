#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "strings.h"

void process_request(unsigned int client_fd, char *directory_str) {

  char msg_200[19] = "HTTP/1.1 200 OK\r\n\r\n";
  char msg_201[24] = "HTTP/1.1 201 Created\r\n\r\n";
  char msg_404[31] = "HTTP/1.1 404 Not Found\r\n\r\n";
  char msg_400[33] = "HTTP/1.1 400 Bad Request\r\n\r\n";
  char msg_500[34] = "HTTP/1.1 500 Server Error\r\n\r\n";

  string_t request_str = string_new(32 * 1024);
  size_t read_data = read(client_fd, request_str.head, request_str.capacity);
  if (read_data == -1) {
    printf("Could not read from the client soket");
    goto final;
  }

  request_str.length = read_data;
  string_slice_list_t request_lines = string_split(&request_str, "\r\n");
  string_slice_t request_line = request_lines.head[0];
  string_slice_list_t request_values = string_slice_split(&request_line, " ");

  size_t written = 0;
  if (request_values.length < 2) {
    written = write(client_fd, &msg_400, 33);
    if (written == -1) {
      printf("Could not write to the client socket");
      goto final;
    }
  }

  string_slice_t path = request_values.head[1];

  if (string_slice_compare_cstr(&path, "/")) {
    written = write(client_fd, &msg_200, 19);
  } else if (string_slice_compare_cstr(&path, "/user-agent")) {
    for (size_t i = 1; i < request_lines.length - 1; i++) {
      string_slice_t line = request_lines.head[i];
      if (string_slice_starts_with(&line, "User-Agent: ")) {
        string_slice_t user_agent_val =
            string_slice_slice(&line, 12, line.length - 12);

        char res[1024 * 32];
        size_t res_size =
            snprintf(res, sizeof(res),
                     "HTTP/1.1 200 OK\r\nContent-Type: "
                     "text/plain\r\nContent-Length: %ld\r\n\r\n%.*s",
                     user_agent_val.length, (int)user_agent_val.length,
                     user_agent_val.head);

        written = write(client_fd, res, res_size);
      }
    }

    if (written == 0) {
      written = write(client_fd, &msg_400, 33);
    }
  } else if (path.length > 6 && string_slice_starts_with(&path, "/echo/")) {
    string_slice_t echo_val = string_slice_slice(&path, 6, path.length - 6);
    unsigned short int gzip_encoded = 0;

    for (size_t i = 1; i < request_lines.length - 2; i++) {
      string_slice_t header = request_lines.head[i];
      if (string_slice_starts_with(&header, "Accept-Encoding: ")) {
        string_slice_t value =
            string_slice_slice(&header, 17, header.length - 17);

        string_slice_list_t encoding_values = string_slice_split(&value, ", ");
        for (size_t j = 0; j < encoding_values.length; j++) {
          string_slice_t encoding_value = encoding_values.head[j];
          if (string_slice_compare_cstr(&encoding_value, "gzip")) {
            gzip_encoded = 1;
          }
        }
      }
    }
    char *encoding = gzip_encoded ? "Content-Encoding: gzip\r\n" : "";
    char res[1024 * 32];
    size_t res_size = snprintf(
        res, sizeof(res),
        "HTTP/1.1 200 OK\r\nContent-Type: "
        "text/plain\r\nContent-Length: %ld\r\n%s\r\n%.*s",
        echo_val.length, encoding, (int)echo_val.length, echo_val.head);

    written = write(client_fd, res, res_size);
  } else if (string_slice_compare_cstr(&request_values.head[0], "GET") &&
             path.length > 7 && string_slice_starts_with(&path, "/files/")) {
    string_slice_t file_name = string_slice_slice(&path, 7, path.length - 6);
    size_t path_length = strlen(directory_str) + file_name.length + 1;
    char path[path_length];
    snprintf(path, path_length, "%s%.*s", directory_str, (int)file_name.length,
             file_name.head);

    printf("file path is: %s\n", path);
    int file_fd = open(path, O_RDONLY);
    if (file_fd == -1) {
      if (errno == ENOENT) {
        written = write(client_fd, &msg_404, 31);
        goto final;
      }

      written = write(client_fd, &msg_500, 34);
      goto final;
    }

    string_t buffer = string_new(1024 + 32);
    read_data = read(file_fd, buffer.head, buffer.capacity);
    buffer.length = read_data;
    if (read_data == -1) {
      printf("Could not read from the file: %s\n", path);
      written = write(client_fd, &msg_500, 34);
    } else {

      char res[1024];
      size_t res_size =
          snprintf(res, sizeof(res),
                   "HTTP/1.1 200 OK\r\nContent-Type: "
                   "application/octet-stream\r\nContent-Length: %ld\r\n\r\n",
                   buffer.length);

      written = write(client_fd, &res, res_size);
      written += write(client_fd, buffer.head, buffer.length);
    }
    close(file_fd);
  } else if (string_slice_compare_cstr(&request_values.head[0], "POST") &&
             path.length > 7 && string_slice_starts_with(&path, "/files/")) {

    string_slice_t file_name = string_slice_slice(&path, 7, path.length - 6);
    size_t path_length = strlen(directory_str) + file_name.length + 1;
    char path[path_length];
    snprintf(path, path_length, "%s%.*s", directory_str, (int)file_name.length,
             file_name.head);

    printf("file path is: %s\n", path);

    int file_fd = open(path, O_WRONLY | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
    if (file_fd == -1) {
      if (errno == EEXIST) {
        written = write(client_fd, &msg_400, 33);
        goto final;
      }

      written = write(client_fd, &msg_500, 34);
      goto final;
    }

    string_slice_t body_slice = request_lines.head[request_lines.length - 1];
    write(file_fd, body_slice.head, body_slice.length);
    close(file_fd);
    written = write(client_fd, msg_201, 24);

  } else {
    written = write(client_fd, &msg_404, 31);
  }

  if (written == -1) {
    printf("Could not write to the client socket");
  }

final:
  close(client_fd);
  string_free(&request_str);
  return;
}

int main(int argc, char **argv) {
  // Disable output buffering
  setbuf(stdout, NULL);
  setbuf(stderr, NULL);

  char *directory_str = NULL;
  static struct option long_options[] = {
      /*   NAME       ARGUMENT           FLAG  SHORTNAME */
      {"directory", required_argument, NULL, 'd'},
      {"single-thread", no_argument, NULL, 's'},
  };

  int option_index = 0;
  unsigned short int multi_threaded = 1;
  char c;
  while ((c = getopt_long(argc, argv, "d:s", long_options, &option_index)) !=
         -1) {
    switch (c) {
    case 'd':
      directory_str = optarg;
      break;
    case 's':
      multi_threaded = 0;
      break;
    default:
      break;
    }
  }

  if (directory_str == NULL) {
    directory_str = "/tmp/";
  }

  // You can use print statements as follows for debugging, they'll be visible
  // when running tests.
  printf("Logs from your program will appear here!\n");
  printf("directory is: %s\n", directory_str);

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

    if (multi_threaded) {
      switch (fork()) {
      case -1:
        printf("Couldn ot spawn child pricess");
        close(client_fd);
        break;

      case 0:
        close(server_fd);
        process_request(client_fd, directory_str);
        _exit(EXIT_SUCCESS);

      default:
        close(client_fd);
        break;
      }
    } else {
      process_request(client_fd, directory_str);
    }
  }

  close(server_fd);

  return 0;
}
