PORT=40001
CFLAGS = -DSERVER_PORT=$(PORT) -g -Wall -Werror -fsanitize=address

all: readserver randclient bufserver

readserver: readserver.o socket.o
	gcc ${CFLAGS} -o $@ $^

randclient: randclient.o socket.o
	gcc ${CFLAGS} -o $@ $^

bufserver: bufserver.o socket.o
	gcc ${CFLAGS} -o $@ $^

%.o: %.c socket.h
	gcc ${CFLAGS} -c $<

clean:
	rm *.o readserver randclient bufserver
