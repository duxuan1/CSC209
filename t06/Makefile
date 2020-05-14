FLAGS= -Wall -Werror -g -fsanitize=address

all : serialize deserialize test

serialize: serialize.o helpers.o
	gcc ${FLAGS} -o $@ $^

serialize.o : helpers.h
helper.o : helpers.h

%.o	: %.c
	gcc ${FLAGS} -c $<

deserialize: deserialize.o helpers.o
	gcc ${FLAGS} -o $@ $^

deserialize.o : helpers.h
helper.o : helpers.h

%.o : %.c
	gcc ${FLAGS} -c $<

test:
	@./serialize < test.dat | ./deserialize; \
    if [ $$? -eq 0 ]; then \
    	echo "Basic test passed"; \
	else \
		echo "Basic test failed"; \
    fi 

clean :
	-rm *.o serialize deserialize