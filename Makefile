compile:
	gcc -o test test.c
run: compile
	./test
clean:
	rm test
