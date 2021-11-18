#include "server.h"

typedef struct client_connection {
	int server_socket;
	char buff[CONNECTION_BUFF_LEN];
	ssize_t msg_size;
	struct sockaddr address;
} client_connection;

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
    if ((server_socket = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
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

    // server loop
    while (TRUE) {

        // accept connection to client
		client_connection* conn = (client_connection*)malloc(sizeof(*conn));

		socklen_t addr_size = sizeof(conn->address);
		conn->server_socket = server_socket;
		conn->msg_size = recvfrom(server_socket, conn->buff, CONNECTION_BUFF_LEN, MSG_WAITALL,&conn->address, &addr_size);

		printf("\nAccepted client\n");
		if (conn->buff[0] == 'q') {
			// cleanup
			if (close(server_socket) == -1) {
				perror("Error closing socket\n");
				exit(EXIT_FAILURE);
			} else {
				printf("Closed socket to client, exit\n");
				exit(EXIT_SUCCESS);
			}

		}

		pthread_t thread = 0;
		if (pthread_create(&thread, NULL, &handle_client, (void*) conn) != 0) {
			perror("Error creating thread");
		}
    }
}

char * read_until_new_line(int client_socket, int size) {
  char *buffer = (char *)malloc(sizeof(char) *  size + 1);

  for (int i = 0; i < size; i++) {
    buffer[i] = '\0';
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
    if (input == '\n' || i == size+1) {
      return buffer;
    }
  }
  return buffer;
}


/************************************************************************
 * handle client
 ************************************************************************/

void* handle_client(void* socket_addr) {
    client_connection* client_conn = (client_connection*) socket_addr;
    int keep_going = TRUE;

	char * whole_op = client_conn->buff;
	printf("%s", whole_op);

	char op = '\0';
	double num1 = 0.0;
	double num2 = 0.0;

	int parsed = sscanf(whole_op, "%c %lf %lf", &op, &num1, &num2);
	if (parsed != 3) {
		printf("Bad request\n");
		return NULL;
	}

	printf("Received op: %c\n", op);
	printf("Translated num1: %.15f\n", num1);
	printf("Translated num2: %.15f\n", num2);

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

	socklen_t addr_size = sizeof(client_conn->address);
	sendto(client_conn->server_socket, &buf, sizeof(char) * i, 0,
		   &client_conn->address, addr_size);

	free(client_conn);
    return NULL;
}
