#include "time_client.h"

/************************************************************************
 * MAIN
 ************************************************************************/
int main() {
    int client_socket;                  // client side socket
    struct sockaddr_in client_address;  // client socket naming struct
    char c;
    char buffer[1000];                  // Assume a significant buffer to
                                        // recieve data from the server
    printf("Time client\n");

    // create an unnamed socket, and then name it
    client_socket = socket(AF_INET, SOCK_STREAM, 0);

    // create addr struct
    struct hostent * record = gethostbyname(SERVER_ADDR);
  	in_addr_t * address = (in_addr_t * )record->h_addr;
    client_address.sin_family = AF_INET;
    client_address.sin_addr.s_addr = *address;
    client_address.sin_port = htons(PORT);

    // connect to server socket
    if (connect(client_socket, (struct sockaddr *)&client_address, sizeof(client_address)) == -1) {
        perror("Error connecting to server!\n");
        exit(EXIT_FAILURE);
    }

    int i = 0;
    while (TRUE) {
        // get the result
        read(client_socket, &c, sizeof(char));
        buffer[i++] = c;

        // Read up until the *
        if (c == '*') {
          break;
        }
    }

    char output[i-2];
    for (int output_pos = 1; output_pos < i-2; output_pos++) {
      output[output_pos] = buffer[output_pos];
    }

    printf("%s\n", output);

    return EXIT_SUCCESS;
}
