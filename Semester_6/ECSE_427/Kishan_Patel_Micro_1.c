#include<stdio.h>
#include<string.h>
#include<unistd.h>

main()
{
	//Variable declarations.
	int pid = getpid();
	int parentPid = getppid();
	char pathToFile[30];
	FILE *file;
	char lineInFile[500];
	char name[50];
	char value[50];
	
	//Print the PID of the process and the PID of the parent of the 
	//given process.
	printf("PID is: %d\n",pid);
	printf("Parent PID is: %d\n",parentPid);
	
	//Parse the status file associated with the given PID.
	sprintf(pathToFile,"/proc/%d/status",pid);
	file = fopen(pathToFile,"r");
	if(file != NULL)
	{
		while(fgets(lineInFile,500,file)!=NULL)
		{
			if(strstr(lineInFile,"Name:")!=NULL)
			{
				sscanf(lineInFile,"%s%s",name,value);
				printf("%s%s\n",name,value);
			}
			else if(strstr(lineInFile,"State:")!=NULL)
			{
				sscanf(lineInFile,"%s %[^\n]",name,value);
				printf("%s%s\n",name,value);
			}
			else if(strstr(lineInFile,"VmStk:")!=NULL)
			{
				sscanf(lineInFile,"%s %[^\n]",name,value);
				printf("%s%s\n",name,value);
			}
			else if(strstr(lineInFile,"voluntary_ctxt_switches:")!=NULL)
			{
				sscanf(lineInFile,"%s %s",name,value);
				printf("%s%s\n",name,value);
			}
			else if(strstr(lineInFile,"nonvoluntary_ctxt_switches:")!=NULL)
			{
				sscanf(lineInFile,"%s %s",name,value);
				printf("%s%s\n",name,value);
			}
		}
		
		fclose(file);
	}
	else
	{
		perror("Error opening file");
	}
	return 0;
}