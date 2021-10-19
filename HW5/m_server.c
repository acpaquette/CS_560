#include "m_server.h"
#include <netinet/in.h>
#include <stdint.h>

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
    // int thread_id = 0;
    while (TRUE) {
        // accept connection to client
        if ((client_socket = accept(server_socket, NULL, NULL)) == -1) {
            perror("Error accepting client\n");
        } else {
			printf("client %d connected\n", client_socket);
            pthread_t thread = 0;
            if (pthread_create(&thread, NULL, &handle_client, (void*) client_socket) != 0) {
				perror("Error creating thread");
      	    }

            pthread_detach(thread);
        }
    }
}


/************************************************************************
 * handle client
 ************************************************************************/

void* handle_client(void* socket_addr) {
	int client_socket = ((int) socket_addr);
	char input;
	int keep_going = TRUE;
	uint32_t buff;

	int read_count = recv(client_socket, &buff, sizeof(buff), MSG_WAITALL);

	if (read_count <= 0){
		fprintf(stderr, "Invalid read from client with error: %i\n", read_count);

		if (close(client_socket) == -1) {
			perror("Error closing socket");
		} else {
			printf("Closed socket to client, exit\n");
		}
		return NULL;
	}

	int recved_int = ntohl(buff);
	int a = recved_int;
	int iter_count = 0;

	while (a != 1) {
		switch (a % 2){
		case 0:
			a/=2;
			break;
		case 1:
			a*=3;
			a++;
			break;
		}

		iter_count++;
	}

	buff = htonl(iter_count);

	// send result back to client
	write(client_socket, &buff, sizeof(buff));

	usleep(500000);
	printf("%d: %d -------> %d\n", client_socket, recved_int, iter_count);
	// cleanup
	if (close(client_socket) == -1) {
		perror("Error closing socket");
		exit(EXIT_FAILURE);
	}

	return NULL;
}
