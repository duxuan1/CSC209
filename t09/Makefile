TARGETS=time_reads write_test_file

all: $(TARGETS)

%: %.c
	gcc -Wall -Werror -fsanitize=address -g -std=gnu99 -o $@ $^

clean:
	rm -f $(TARGETS)
