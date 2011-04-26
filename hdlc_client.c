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
		report_frame("tx", U_SABM, "ok");
		wait = pthread_cond_timedwait(&received, &mutex, &limit);
		if (wait == 0) {
			pthread_mutex_unlock(&mutex);
			pthread_exit(NULL);
		}
		report_frame("tx", U_SABM, "timeout");
	}
}

void* recv_ua(void* args) {
	HDLCSocket* data = (HDLCSocket*)args;
	uint8_t control, info[MAX_BUFFER/2];
	while (1) {
		pthread_mutex_lock(&mutex);
		len_recv = recvfrom(data->sock, buf_recv, MAX_BUFFER, 0, (struct sockaddr *)&data->sock_addr, &data->sock_len);
		len_recv = unpack_frame(&control, info, buf_recv, len_recv);
		pthread_mutex_unlock(&mutex);
		if (len_recv >= 0) {
			report_frame("rx", control, "OK");
			if (frame_type(control) == U_SABM) {
				pthread_cond_signal(&received);
				pthread_exit(NULL);
			}
		}
	}
}

void run_client(HDLCSocket* data) {
	pthread_t send, recv;
	
	pthread_create(&send, NULL, &send_sabm, (void*)data);
	pthread_create(&recv, NULL, &recv_ua,   (void*)data);
	pthread_join(send, NULL);
	pthread_join(recv, NULL);
}
