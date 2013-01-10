/*********************************************************************************
The code in this file attempts to simulate the hungry savages problem in a thread
safe manner through the use of an arbitrator. The arbitrator gives the cook and savages
an equal opportunity to perform their actions. It gives the client an opportunity to
take from the pot and it gives the cook a chance to refill the pot. Only one person can
perform a given action at a time. This program also tests the LTL assertion that 
the size of the pot is always greater than 0 and less than 4.
***********************************************************************************/
#define M 4
short pot;
#define p (pot>0 && pot<M)
bit req[4];
bit gnt[4];

never  {    /* ![]p */
T0_init:
        if
        :: (! ((p))) -> goto accept_all
        :: (1) -> goto T0_init
        fi;
accept_all:
        skip
}


proctype round_robin_arbiter()
{
	short i;

	do
	::
		if
		:: (i > 3) -> i = 0
		:: else -> skip /*Avoids blocking when i <= 3*/
		fi;

		if
		:: (req[i] == 1) ->
			gnt[i] = 1;
			(gnt[i] == 0); /*blocking statement until true*/
			req[i] = 0;
		:: else ->skip; 	/*avoid blocking "if" when req[i] == 0*/
		fi;
		
		i++;
	od
}

//Example client code
proctype client (byte id)
{
	do
	:: (req[id] == 0) ->
		req[id] = 1;
		(gnt[id]==1);
		/*Use the resource here */
		pot--;
		printf("Client %d took from the pot.\n",id);
		gnt[id] = 0;
	od
}

proctype cook(byte id)
{
	do
	:: (req[id] == 0) ->
		assert(pot>=0);
		req[id]=1;
		(gnt[id]==1);
		pot=M;
		gnt[id]=0;
	od
}

init
{
	run round_robin_arbiter();
	run cook(0);
	run client(1);
	run client(2);
	run client(3);
}
