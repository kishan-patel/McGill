import os;
import sys;

os.system("rm -f a.out");
os.system("gcc -g -o a.out my-test.c mythreads.c -DHAVE_PTHREAD_RWLOCK=1 -lslack -lrt -lm");
os.system("./a.out");

   
        


