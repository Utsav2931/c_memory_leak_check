my_memcheck.so: my_memcheck.c
	gcc -g -shared -fPIC my_memcheck.c -o my_memcheck.so -ldl
test1:	test1.c
	gcc -g -rdynamic -o test1 test1.c
test2:	test2.c
	gcc -g -rdynamic -o test2 test2.c
test3:	test3.c
	gcc -g -rdynamic -o test3 test3.c
test4:	test4.c
	gcc -g -rdynamic -o test4 test4.c
clean:
	rm -f my_memcheck.so test1 test2 test3 test4
