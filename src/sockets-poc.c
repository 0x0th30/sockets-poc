#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PORT 8080
#define MAX_QUEUED_CONNECTIONS 5
#define BUFFER_SIZE 1024

int main() {
  int listening_socket, client_socket;

  struct sockaddr_in address;
  socklen_t addr_len = sizeof(address);
  char message_buffer[BUFFER_SIZE] = {0};

  listening_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (listening_socket == -1) {
    perror("Socket creation failed.");
    exit(EXIT_FAILURE);
  }
  printf("Listening on port %d\n", PORT);

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);

  if (bind(listening_socket, (struct sockaddr *)&address, sizeof(address)) <
      0) {
    perror("Socket bind failed.");
    close(listening_socket);
    exit(EXIT_FAILURE);
  }

  if (listen(listening_socket, MAX_QUEUED_CONNECTIONS) < 0) {
    perror("Socket listening failed.");
    close(listening_socket);
    exit(EXIT_FAILURE);
  }

  client_socket =
      accept(listening_socket, (struct sockaddr *)&address, &addr_len);
  if (client_socket < 0) {
    perror("Socket accept failed.");
    close(client_socket);
    exit(EXIT_FAILURE);
  }
  printf("Accepted connection from new client.\n");

  while (true) {
    memset(message_buffer, 0, BUFFER_SIZE);

    recv(client_socket, message_buffer, BUFFER_SIZE, 0);
    printf("Received new message:\n%s\n", message_buffer);

    char response[BUFFER_SIZE];
    strcpy(response, ">>> ");
    strcat(response, (char *)message_buffer);

    send(client_socket, response, BUFFER_SIZE, 0);
    printf("Sending response:\n%s\n", response);
  }
  return 0;
}
