import os;
import sys;

os.system("gcc -o a.out Mini_A1.c");
os.system("echo > Results");

for i in range (1,11):
    f = open('Results','a');
    f.write("Results of Run #"+str(i)+"\n");
    f.write("..................\n");
    f.close();
    os.system("./a.out >> Results");
   
        


