# Description

This program will print out a summary of memory leaks and invalid free (such as free after free). When a heap using a C application is invoked, it is linked with my_memcheck.so such that all calls to malloc() and free() are intercepted with your versions of malloc and free.

The heap using application a.out should be run as:
```
$ LD_PRELOAD=$PWD/my_memcheck.so ./a.out
```

The environment variable LD_PRELOAD tells the OS to preload your dynamic library ```my_memcheck.so``` ahead of the default one. So your versions of malloc/free will override the default ones. _You are intercepting the real malloc/free calls of the program a.out._

This program works by creating a linked list that holds metadata about the malloc block and checks whether the block has been freed or not by checking the free call count. The same count can be used to check invalid free such as freeing the same block more than one time.

To try out the starter code, do the following:
```
$ make my_memcheck.so
$ make test1
```

and run test1 using the intercepting library
```
$ LD_PRELOAD=$PWD/my_memcheck.so ./test1
```

# Note
The code was tested on gcc version of Ubuntu 16/gcc different OS/gcc/g++ versions may yield different behavior.
