PORT=40002
CFLAGS = -DSERVER_PORT=$(PORT) -g -Wall -Werror -fsanitize=address

all: chat_server chat_client

chat_server: chat_server.o socket.o chat_helpers.o
	gcc ${CFLAGS} -o $@ $^

chat_client: chat_client.o socket.o
	gcc ${CFLAGS} -o $@ $^

%.o: %.c
	gcc ${CFLAGS} -c $<

clean:
	rm -f *.o chat_server chat_client
