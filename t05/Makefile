# Makefile for the bitmap printer program

FLAGS= -Wall -Werror -g -fsanitize=address

all : bitmap_printer

bitmap_printer : bitmap_printer.o bitmap.o
	gcc ${FLAGS} -o $@ $^

bitmap_printer.o : bitmap.h
bitmap.o : bitmap.h

%.o : %.c
	gcc ${FLAGS} -c $<

clean :
	-rm *.o bitmap_printer
