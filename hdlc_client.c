#include "hdlc_client.h"

void* send_sabm(void* args) {
	int wait;
	HDLCSocket* data = (HDLCSocket*)args;
	struct timeval  now;
	struct timespec limit;
	
	pthread_mutex_lock(&mutex);
	while (1) {
		gettimeofday(&now, NULL);
		limit.tv_nsec  = now.tv_usec*1000 + TIMEOUT_MS*1000000;
		limit.tv_sec   = now.tv_sec + limit.tv_nsec/1000000000;
		limit.tv_nsec %= 1000000000;
		
		len_send = build_sabm(buf_send);
		sendto(data->sock, buf_send, len_send, 0, (struct sockaddr*)&data->sock_addr, data->sock_len);
		wait = pthread_cond_timedwait(&received, &mutex, &limit);
		if (wait != ETIMEDOUT) {
			pthread_mutex_unlock(&mutex);
			pthread_exit(NULL);
		}
	}
}

void* recv_ua(void* args) {
	pthread_exit(NULL);
}

void run_client(HDLCSocket* data) {
	pthread_t send, recv;
	
	pthread_create(&send, NULL, &send_sabm, (void*)data);
	pthread_create(&recv, NULL, &recv_ua,   (void*)data);
	pthread_join(send, NULL);
	pthread_join(recv, NULL);
}
