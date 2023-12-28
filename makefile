build:
	rm -f app.exe
	gcc main.c -o app.exe

run:
	./app.exe

valgrind:
	valgrind -s --tool=memcheck --leak-check=yes --track-origins=yes ./app.exe

built_test:
	gcc -o test.exe main.c

run_test: built_test
	./test.exe

clean:
	rm -f app.exe test.exe *.o