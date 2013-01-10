/**********************************************************************/
/*The purpose of this program is to demonstrate the importance of	*/
/*atomic statements. After running the simulation, the last process to*/
/*execute is supposed to print a value of 11 for n on the console. 	*/
/*However, if the program is executed as it is written below, this is */
/*not always guaranteed to occur as there is no synchronization used.  */
/*This means that a given process does not have to wait for the other */
/*process to be done with the shared resource before it uses it. If an*/
/*atomic statement is used enclosing each statement in both processes,*/
/*then, the output of n will be 11 for the final process that executes.*/
/**********************************************************************/

/*Global Variables*/
byte n = 0;

proctype amy()
{
	byte tmp;
	tmp = n + 5;
	n = tmp;
	printf ("Process Amy, n = %d, Amy's PID: %d\n", n, _pid);
	
}

proctype sheldon()
{
	byte tmp;
	tmp = n + 6;
	n = tmp;
	printf ("Process Sheldon, n = %d, Sheldon's PID: %d\n", n, _pid);
	
}

init
{
	run sheldon();
	run amy();
}
