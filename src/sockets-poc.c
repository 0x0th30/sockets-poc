#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PORT 8080
#define MAX_QUEUED_CONNECTIONS 5
#define BUFFER_SIZE 1024

void gracefully_quit() {
  printf("Gracefully quitting... Connection closed by client.\n");
  exit(EXIT_SUCCESS);
}

void handle_error(char *err_str, int failing_socket) {
  perror(err_str);
  close(failing_socket);
  exit(EXIT_FAILURE);
}

int main() {
  int listening_socket, client_socket;

  struct sockaddr_in address;
  socklen_t addr_len = sizeof(address);
  char message_buffer[BUFFER_SIZE] = {0};

  listening_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (listening_socket == -1)
    handle_error("Socket creation failed.", listening_socket);
  printf("Listening on port %d\n", PORT);

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);

  if (bind(listening_socket, (struct sockaddr *)&address, sizeof(address)) < 0)
    handle_error("Socket binding failed.", listening_socket);

  if (listen(listening_socket, MAX_QUEUED_CONNECTIONS) < 0)
    handle_error("Socket listening failed", listening_socket);

  client_socket =
      accept(listening_socket, (struct sockaddr *)&address, &addr_len);
  if (client_socket < 0)
    handle_error("Socket accept failed", client_socket);
  printf("Accepted connection from new client.\n");

  while (true) {
    memset(message_buffer, 0, BUFFER_SIZE);

    recv(client_socket, message_buffer, BUFFER_SIZE, 0);
    printf("Received new message:\n%s\n", message_buffer);

    if (message_buffer[0] == '\0')
      gracefully_quit();

    char response[BUFFER_SIZE];
    strcpy(response, ">>> ");
    strcat(response, (char *)message_buffer);

    send(client_socket, response, BUFFER_SIZE, 0);
    printf("Sending response:\n%s\n", response);
  }
  return 0;
}
