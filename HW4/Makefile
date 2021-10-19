LDFLAGS = -pthread  -lm

client: client.c threadpool.o server.h client.h threadpool.h
	cc -o client threadpool.o client.c


server: server.c server.h
	cc -o server server.c
