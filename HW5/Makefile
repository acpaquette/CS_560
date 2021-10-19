LDFLAGS = -pthread  -lm
CCFLAGS = -g
CFLAGS = -g

all: client s_server m_server t_server

client: threadpool.o dbg.h client.h

s_server: s_server.h

m_server: m_server.h

t_server: t_server.h threadpool.o

clean:
	rm -f m_server s_server t_server client *.o

threadpool.o: threadpool.c threadpool.h
