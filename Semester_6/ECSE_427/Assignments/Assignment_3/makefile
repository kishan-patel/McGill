CC=gcc

all: ./sfs.c ./sfs_api.h ./disk_emu.c ./disk_emu.h ./sfs_test.c
	$(CC) -g -Wall sfs.c disk_emu.c sfs_test.c -DHAVE_PTHREAD_RWLOCK=1 -lslack -o sfs_out 

clean:
	rm sfs_out SFS  testoutput.txt

test: all
	./sfs > testoutput.txt
