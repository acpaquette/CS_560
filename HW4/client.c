#include "client.h"
#include "server.h"
#include "threadpool.h"

/* ******************************************************* */
/* main()                                                  */
/* ******************************************************* */
int main() {

    // task counter, also serves as argument to 3A+1 algorithm
    int task_counter;

    // create threadpool with THREADS_IN_POOL threads
    threadpool pool = threadpool_create();


    for (task_counter=1; task_counter<=NUMBER_TASKS; task_counter++)
    {
        // in each loop, execute three_a_plus_one_wrapper in a thread from the pool
        threadpool_add_task(pool, task_copy_arguments, three_a_plus_one_wrapper, (void*)&task_counter);
    }

    // lame way to wait for everybody to get done
    // in a network server, this is not needed as the main thread keeps accepting connections
    sleep(2000);

    threadpool_destroy(pool);

    exit(EXIT_SUCCESS);
}


/* ******************************************************* */
/* three_a_plus_one_wrapper()                              */
/* ******************************************************* */
void three_a_plus_one_wrapper(void *number_ptr)
{
    int number = *((int*)number_ptr);

    printf("thread ID %p ----> %d: %d\n", pthread_self(), number, three_a_plus_one_rec(number));

    int client_socket;                  // client side socket
    struct sockaddr_in client_address;  // client socket naming struct

    // create an unnamed socket, and then name it
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    // create addr struct
    client_address.sin_family = AF_INET;
    // printf("Connecting to %s at port %hu", SERVER_ADDR, htons(PORT));
    client_address.sin_addr.s_addr = inet_addr(SERVER_ADDR);
    client_address.sin_port = htons(PORT);

    // connect to server socket
    if (connect(client_socket, (struct sockaddr *)&client_address, sizeof(client_address)) == -1) {
        perror("Error connecting to server!");
        exit(EXIT_FAILURE);
    }

    // make the request to the server
    uint32_t network_number = htonl(number);
    printf("SENDING %i as %i\n", number, network_number);
    write(client_socket, &network_number, sizeof(network_number));

    // get the result
    int read_count = recv(client_socket, &network_number, sizeof(network_number), MSG_WAITALL);
    close(client_socket);

    if (read_count <= 0) {
      fprintf(stderr, "Invalid read from server with error: %i\n", read_count);
    }
    else {
      int result = ntohl(network_number);
      printf("For %i got sequence: %i\n", number, result);
    }
}


/* ******************************************************* */
/* three_a_plus_one() - nonrecursive                       */
/* ******************************************************* */
int three_a_plus_one(int input)
{
    int counter = 0;
    int current = input;

    while (current != 1)
    {
        counter++;
        if (current % 2) {
            current = (current*3) + 1;
        }
        else {
            current >>= 1;
        }
    }
    return counter;
}


/* ******************************************************* */
/* three_a_plus_one_rec() - recursive                          */
/* ******************************************************* */
int three_a_plus_one_rec(int number) {
    int new_number;

    if (number == 1) {
        return 0;
    }

    if (number % 2) {
        new_number = 3 * number + 1;
    } else {
        new_number = number / 2;
    }

    return 1 + three_a_plus_one(new_number);
}


/* ******************************************************* */
/* prepare arguments for thread function                   */
/* ******************************************************* */
void *task_copy_arguments(void *args_in)
{
    void *args_out;

    args_out = malloc(sizeof(int));
    *((int*)args_out) = *((int*)args_in);

    return args_out;
}
