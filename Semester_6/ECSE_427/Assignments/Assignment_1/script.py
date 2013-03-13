import os;
import sys;

os.system("gcc -o a.out my-test.c mythreads.c -DHAVE_PTHREAD_RWLOCK=1 -lslack -lrt -lm");
os.system("./a.out");

   
        


