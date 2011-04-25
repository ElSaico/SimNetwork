#include "hdlc_server.h"

void run_server(int sock, struct sockaddr* server, socklen_t len) {
	while (1) {
		//len_recv = recvfrom(sock,buf,1024,0,(struct sockaddr *)&server,&length);
		//len_send = sendto(sock,"Got your message\n",17,0,(struct sockaddr *)&server,length);
	}
}
