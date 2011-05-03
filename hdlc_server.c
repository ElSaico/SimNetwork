#include "hdlc_server.h"

void run_server() {
	len_send[0] = build_ua(buf_send[0]);
	while (true) {
		sendto(data.sock, buf_send[0], len_send[0], 0, (struct sockaddr *)&data.sock_addr, data.sock_len);
		report_frame("tx", buf_send[0], "sent");
		sleep(1);
	}
}
