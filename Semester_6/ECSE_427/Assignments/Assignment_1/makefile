#Kishan Patel - 260376121
#ECSE 427 - Assignment 1
#Compiler: GCC

my-test : my-test.o mythreads.o mythreads.h
	gcc -static -g -o my-test my-test.o mythreads.o -DHAVE_PTHREAD_RWLOCK=1 -lslack -lrt -lm

my-test.o : my-test.c mythreads.h
	gcc -c my-test.c -DHAVE_PTHREAD_RWLOCK=1 -lslack -lrt -lm 

mythreads.o : mythreads.c
	gcc -c mythreads.c -DHAVE_PTHREAD_RWLOCK=1 -lslack -lrt -lm

