OBJS = sim_hdlc.o hdlc.o hdlc_client.o hdlc_server.o

SRCS = $(OBJS,.o=.c)

all: sim_hdlc

sim_hdlc: $(OBJS)
	gcc -O2 -lc -lpthread -o sim_hdlc $(OBJS)

.c.o:
	gcc -c -O2 -Wall $<

clean:
	rm -f sim_hdlc *.o
