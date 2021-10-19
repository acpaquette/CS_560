/************************************************************************
 * libraries
 ************************************************************************/
// should always be there ...
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// socket/bind/listen/accept
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

// read/write/close
#include <sys/uio.h>
#include <sys/types.h>
#include <unistd.h>


/************************************************************************
 * preprocessor directives
 ************************************************************************/
#define SERVER_ADDR "time.nist.gov" // loopback ip address
#define PORT 13              // port the server will listen on

// I cannot let go of the old-fashioned way :) - for readability ...
#define FALSE false
#define TRUE !false
