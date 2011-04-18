OBJS = hdlc.o

SRCS = $(OBJS,.o=.c)

all: client server

client: $(OBJS) hdlc_client.o
	gcc -O2 -lc -o hdlc_client $(OBJS) hdlc_client.o

server: $(OBJS) hdlc_server.o
	gcc -O2 -lc -o hdlc_server $(OBJS) hdlc_server.o

.c.o:
	gcc -c -O2 -Wall $<

clean:
	rm -f hdlc_client hdlc_server *.o
