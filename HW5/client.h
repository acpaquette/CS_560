#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#include "s_server.h"

// turn on debugging
#define DBG
#include "dbg.h"

int three_a_plus_one(int input);
void three_a_plus_one_wrapper(void *number_ptr);

#define NUMBER_TASKS 1000
