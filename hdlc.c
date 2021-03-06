/** 
 **
 **  u16 represents an unsigned 16-bit number. Adjust the typedef for
 **  your hardware. 
 **/

#include <assert.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "hdlc.h"

#define PPPINITFCS16    0xffff  /* Initial FCS value    */
#define PPPGOODFCS16    0xf0b8  /* Good final FCS value */ 
#define DATA_SIZE       254   
    
typedef unsigned short u16;

/** FCS lookup table as calculated by the table generator.r. **/
  
static u16 fcstab[256] = {
   0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
   0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
   0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
   0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
   0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
   0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
   0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
   0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
   0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
   0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
   0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
   0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
   0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
   0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
   0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
   0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
   0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
   0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
   0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
   0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,

   0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
   0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
   0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
   0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
   0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
   0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
   0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
   0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
   0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
   0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
   0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
   0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
   };


/**
 **  Calculate a new fcs given the current fcs and the new data.
 **/

u16 pppfcs16(u16 fcs, unsigned char *cp,int len)
{
   assert(sizeof (u16) == 2);
   assert(((u16) -1) > 0);
   while (len--)
       fcs = (fcs >> 8) ^ fcstab[(fcs ^ *cp++) & 0xff];

   return (fcs);
}

int byte_stuff(uint8_t* data, int data_len) {
	int i, new_len = data_len;
	for (i = 1; i < new_len-1; ++i) {
		if ((data[i] == FRAME_FLAG) || (data[i] == ESCAPE_CHAR)) {
			data[i] ^= ESCAPE_CONST;
			memmove(data+i+1, data+i, new_len-i);
			data[i] = ESCAPE_CHAR;
			new_len++;
		}
	}
	return new_len;
}

int byte_unstuff(uint8_t* data, int data_len) {
	int i, new_len = data_len;
	for (i = 1; i < new_len-1; ++i) {
		if (data[i] == ESCAPE_CHAR) {
			memmove(data+i, data+i+1, new_len-i-1);
			data[i] ^= ESCAPE_CONST;
			new_len--;
		}
	}
	return new_len;
}

int build_frame(uint8_t* buffer, uint8_t control, const uint8_t* data, int data_len) {
	uint8_t* ptbuf = buffer;
	
	HDLCHeader head;
	head.init_flag = FRAME_FLAG;
	head.address = FRAME_BCAST;
	head.control = control;
	memcpy(ptbuf, &head, 3);
	
	ptbuf += 3;
	memcpy(ptbuf, data, data_len);
	if ((data_len > 0) && (rand() % 100 < 10))
		ptbuf[rand() % data_len] ^= 0xFF;
	
	ptbuf += data_len;
	HDLCFooter foot;
	foot.fcs = pppfcs16(PPPINITFCS16, buffer+1, data_len+2); // endereço + controle + dados
	foot.fcs ^= 0xFFFF;
	foot.end_flag = FRAME_FLAG;
	memcpy(ptbuf, &foot, 3);
	
	return byte_stuff(buffer, data_len+6);
}

int build_I(uint8_t* buffer, int n, const uint8_t* data, int data_len) {
	return build_frame(buffer, I_FRAME | (n << 4), data, data_len);
}

int build_sabm(uint8_t* buffer) {
	return build_frame(buffer, U_SABM, (const uint8_t*)"", 0);
}

int build_ua(uint8_t* buffer) {
	return build_frame(buffer, U_UA, (const uint8_t*)"", 0);
}

int build_disc(uint8_t* buffer) {
	return build_frame(buffer, U_DISC, (const uint8_t*)"", 0);
}

int build_rr(uint8_t* buffer, int n, bool pf) {
	uint8_t control = S_FRAME | S_RR | n;
	if (pf)
		control |= POLLING_FINAL;
	return build_frame(buffer, control, (const uint8_t*)"", 0);
}

int build_rnr(uint8_t* buffer, int n) {
	return build_frame(buffer, S_FRAME | S_RNR | n, (const uint8_t*)"", 0);
}

uint8_t frame_type(uint8_t* frame) {
	HDLCHeader* head = (HDLCHeader*)frame;
	switch (head->control) {
		case U_SABM:
		case U_UA:
		case U_DISC:
			return head->control;
		default: // precisa expandir para cobrir RR/RNR
			return head->control & NOT_I ? head->control & S_TYPE : I_FRAME;
	}
}

uint8_t frame_seq(uint8_t* frame) {
	if (frame_type(frame) == I_FRAME) {
		HDLCHeader* head = (HDLCHeader*)frame;
		return head->control & NS_RECEIVER;
	} else {
		return WINDOW_SIZE;
	}
}

int recv_frame(uint8_t* control, uint8_t* info) {
	len_recv = recvfrom(data.sock, buf_recv, MAX_BUFFER, 0, (struct sockaddr *)&data.sock_addr, &data.sock_len);
	if (len_recv < 0)
		return -1;
	
	len_recv = byte_unstuff(buf_recv, len_recv);
	HDLCHeader* head = (HDLCHeader*)buf_recv;
	HDLCFooter* foot = (HDLCFooter*)(buf_recv+len_recv-3); // que foi? gosto de viver perigosamente
	if ((head->init_flag != FRAME_FLAG) && (head->address != FRAME_BCAST) && (foot->end_flag != FRAME_FLAG)) {
		report_frame("rx", buf_recv, "malformed frame");
		return -1;
	}
	
	if (pppfcs16(PPPINITFCS16, buf_recv+1, len_recv-2) != PPPGOODFCS16) {
		report_frame("rx", buf_recv, "CRC mismatch");
		return -1;
	}
	
	*control = head->control;
	memcpy(info, buf_recv+3, len_recv-6);
	return len_recv-6;
}

void report_frame(const char* who, uint8_t* frame, const char* status) {
	pthread_mutex_lock(&log_lock);
	switch(frame_type(frame)) {
		case U_SABM:
			printf("(%s) SABM [%s]\n", who, status); break;
		case U_UA:
			printf("(%s) UA [%s]\n", who, status); break;
		case U_DISC:
			printf("(%s) DISC [%s]\n", who, status); break;
		case S_TYPE:
			printf("(%s) S [%s]\n", who, status); break; // TODO
		case I_FRAME:
			printf("(%s) I %d [%s]\n", who, frame_seq(frame), status); break;
		default:
			printf("(%s) ??? [unknown]\n", who); break;
	}
	pthread_mutex_unlock(&log_lock);
}
