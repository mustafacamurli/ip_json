

all: test

test: test.o ip_json.o
	gcc -o $@ $^

test.o: test.c
	gcc -c -g $^
	
ip_json.o: ip_json.c
	gcc -c -g $^
	
clean:
	rm -rf test *.o

.PHONY: all clean