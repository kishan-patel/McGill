#Kishan Patel - 260376121
#ECSE 427 PA3
#Compiler gcc

all: ./sfs.c ./sfs_api.h ./disk_emu.c ./disk_emu.h ./sfs_test.c
	gcc -g  -Wall sfs.c disk_emu.c sfs_test.c -DHAVE_PTHREAD_RWLOCK=1 -lslack -o sfs_out 

