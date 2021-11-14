#include "server.h"

/************************************************************************
 * MAIN
 ************************************************************************/

int main(int argc, char** argv) {
    int server_socket;                 // descriptor of server socket
    struct sockaddr_in server_address; // for naming the server's listening socket
    int client_socket;

    // sent when ,client disconnected
    signal(SIGPIPE, SIG_IGN);

    // create unnamed network socket for server to listen on
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // name the socket (making sure the correct network byte ordering is observed)
    server_address.sin_family      = AF_INET;           // accept IP addresses
    server_address.sin_addr.s_addr = htonl(INADDR_ANY); // accept clients on any interface
    server_address.sin_port        = htons(PORT);       // port to listen on

    // binding unnamed socket to a particular port
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        perror("Error binding socket");
        exit(EXIT_FAILURE);
    }

    // listen for client connections (pending connections get put into a queue)
    if (listen(server_socket, NUM_CONNECTIONS) == -1) {
        perror("Error listening on socket");
        exit(EXIT_FAILURE);
    }

    // server loop
    while (TRUE) {

        // accept connection to client
        if ((client_socket = accept(server_socket, NULL, NULL)) == -1) {
            perror("Error accepting client");
        } else {
            printf("\nAccepted client\n");
      			pthread_t thread = 0;
      			if (pthread_create(&thread, NULL, &handle_client, (void*) &client_socket) != 0) {
      				perror("Error creating thread");
      			}
        }
    }
}

char * read_until_new_line(int client_socket, int size) {
  char *buffer = (char *)malloc(sizeof(char) *  size + 1);

  for (int i = 0; i < size; i++) {
    buffer[i] = '0';
  }
  buffer[size] = '\n';

  char input;
  int keep_going = TRUE;

  int i = 0;
  while (keep_going) {
    // read char from client
    switch (read(client_socket, &input, sizeof(char))) {
      case 0:
        keep_going = FALSE;
        printf("\nEnd of stream, returning ...\n");
        break;
      case -1:
        perror("Error reading from network!\n");
        keep_going = FALSE;
        break;
    }
    buffer[i++] = input;
    if (input == '\n') {
      return buffer;
    }
  }
  return buffer;
}


/************************************************************************
 * handle client
 ************************************************************************/

void* handle_client(void* socket_addr) {
    int client_socket = *((int*) socket_addr);
    int keep_going = TRUE;

    while (keep_going) {
        printf("Waiting for new op...\n");
        char * whole_op = read_until_new_line(client_socket, 21);
        printf("%s", whole_op);

        char op = '\0';
        double num1 = 0.0;
        double num2 = 0.0;

        sscanf(whole_op, "%c %lf %lf", &op, &num1, &num2);

        printf("Recieved op: %c\n", op);
        printf("Translated num1: %.15f\n", num1);
        printf("Translated num2: %.15f\n", num2);

        free(whole_op);

        // check if we terminate
        if (op == 'q') {
          printf("Quiting\n");
          break;
        }

        double res = 0.0;

        switch (op) {
          case '+':
            res =  num1 +  num2;
            break;
          case '-':
            res =  num1 -  num2;
            break;
          case '*':
            res =  num1 *  num2;
            break;
          case '/':
            res =  num1 /  num2;
            break;
          case '^':
            res = pow(num1, num2);
            break;
        }

        printf("Result: %.15f\n", res);

        // send result back to client
        int buffer_size = 100;
        char buf[buffer_size];
        snprintf(buf, buffer_size, "%.15f\n", res);
        int i = 0;
        for (; i < buffer_size; i++) {
          if (buf[i] == '\n') {
            break;
          }
        }
        printf("Sending back: %s", buf);
        write(client_socket, &buf, sizeof(char) * i);
    }

    // cleanup
    if (close(client_socket) == -1) {
        perror("Error closing socket\n");
        exit(EXIT_FAILURE);
    } else {
        printf("Closed socket to client, exit\n");
        exit(EXIT_SUCCESS);
    }
    return NULL;
}
