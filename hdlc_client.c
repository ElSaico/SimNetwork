#include "hdlc_client.h"

void* send_frame(void* args) {
	int i = (int)args;
	int wait;
	struct timeval  now;
	struct timespec limit;
	
	pthread_mutex_lock(&network);
	gettimeofday(&now, NULL);
	limit.tv_nsec  = now.tv_usec*1000 + TIMEOUT_MS*1000000;
	limit.tv_sec   = now.tv_sec + limit.tv_nsec/1000000000;
	limit.tv_nsec %= 1000000000;
	
	sendto(data.sock, buf_send[i], len_send[i], 0, (struct sockaddr*)&data.sock_addr, data.sock_len);
	report_frame("tx", buf_send[i], "sent");
	wait = pthread_cond_timedwait(&received[i], &network, &limit);
	pthread_mutex_unlock(&network);
	if (wait != 0) {
		report_frame("tx", buf_send[i], "timeout");
		pthread_mutex_lock(&time_lock);
		timeout = true;
		pthread_mutex_unlock(&time_lock);
	} else {
		if (i < WINDOW_SIZE) { // + ser o que foi acordado pelo RR
			// signal pros anteriores
			// deslizar janela
		}
	}
	return NULL;
}

void* recv_loop(void* args) {
	uint8_t control, info[MAX_BUFFER/2];
	bool disc = false;
	while (!disc) {
		recv_frame(&control, info);
		if (len_recv >= 0) {
			report_frame("rx", buf_recv, "OK");
			pthread_cond_signal(&received[frame_seq(buf_recv)]);
		}
		pthread_mutex_lock(&disc_lock);
		disc = disconnect;
		pthread_mutex_unlock(&disc_lock);
	}
	return NULL;
}

void run_client() {
	bool time;
	pthread_t send[WINDOW_SIZE+1], recv;
	pthread_create(&recv, NULL, &recv_loop, NULL);
	
	len_send[WINDOW_SIZE] = build_sabm(buf_send[WINDOW_SIZE]);
	do {
		pthread_create(&send[0], NULL, &send_frame, (void*)WINDOW_SIZE);
		pthread_join(send[0], NULL);
		pthread_mutex_lock(&time_lock);
		time = timeout;
		pthread_mutex_unlock(&time_lock);
	} while(time);
}
