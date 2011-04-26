#ifndef _HDLC_H
#define _HDLC_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include <netinet/in.h>
#include <pthread.h>

#define MAX_BUFFER 512
#define CONN_PORT  64000
#define TIMEOUT_MS 100

#define I_FRAME     0x00
#define NS_SENDER   0x70
#define NS_RECEIVER 0x07
#define NOT_I       0x80

#define S_FRAME 0x80
#define S_TYPE  0x30
#define S_RR    0x00
#define S_RNR   0x10
#define S_REJ   0x20

#define U_FRAME 0xC0
#define U_SABM  0xF4
#define U_DISC  0xC2
#define U_UA    0xCE

#define POLLING_FINAL 0x08

#define FRAME_FLAG  0x7E
#define FRAME_BCAST 0xFF

#define ESCAPE_CHAR  0x7D
#define ESCAPE_CONST 0x20

int len_send, len_recv;
uint8_t buf_send[MAX_BUFFER], buf_recv[MAX_BUFFER];
pthread_mutex_t mutex, log_lock;
pthread_cond_t received;

typedef struct {
	int sock;
	struct sockaddr_in sock_addr;
	socklen_t sock_len;
	FILE* file;
} HDLCSocket;

typedef struct {
	uint8_t init_flag;
	uint8_t address;
	uint8_t control;
} HDLCHeader;

typedef struct {
	uint16_t fcs;
	uint8_t end_flag;
} HDLCFooter;

uint16_t pppfcs16(uint16_t fcs, uint8_t* buffer, int length);

int build_I(uint8_t* buffer, int n, const uint8_t* data, int data_len);
int build_sabm(uint8_t* buffer);
int build_ua(uint8_t* buffer);
int build_disc(uint8_t* buffer);
int build_rr(uint8_t* buffer, int n);
int build_rnr(uint8_t* buffer, int n);

uint8_t frame_type(uint8_t control);
int unpack_frame(uint8_t* control, uint8_t* data, uint8_t* frame, int len);
void report_frame(const char* who, uint8_t control, const char* status);

#endif
