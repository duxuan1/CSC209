FLAGS= -Wall -Werror -fsanitize=address -fsanitize=signed-integer-overflow -g
DEPENDENCIES = closest_helpers.h closest_brute.h closest_serial.h closest_parallel.h

all : closest closest_tests

closest : closest_helpers.o closest_brute.o closest_serial.o closest_parallel.o main.o
	gcc ${FLAGS} -o $@ $^ -lm

closest_tests: closest_helpers.o closest_brute.o closest_serial.o closest_parallel.o closest_tests.o
	gcc ${FLAGS} -o $@ $^ -lm

%.o : %.c ${DEPENDENCIES}
	gcc ${FLAGS} -c $<

clean :
	rm -f *.o closest closest_tests
