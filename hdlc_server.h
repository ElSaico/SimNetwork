#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hdlc.h"

void run_server(int sock, struct sockaddr* server, socklen_t len);
