/*********************************************************************************
The uses assertions to ensure that the number of clients in the museum at a given time
is less than the preset maximum. It ensures this through the use of blocking statements.
Before a given client wants to enter the museum, it must get permission from the manaager to
do so. The manager will only grant permission if the state of the museum is open (the number
of clients in the museum has not reached the maximum capacity). Once the client has entered,
he must let the manager know so that the manager can update the state of the museum and assert
whether the number of clients in the museum does not exceed the max capacity.
***********************************************************************************/
byte n = 0;
mtype = {open, wait, closed};
byte state = open;
bool time_to_close = false;
chan req = [1] of {bool}; /*global channel request*/
chan gnt = [1] of {bool}; /*global channel grant*/
chan done = [1] of {bool}; /*global channel done*/

proctype client()
{
	bool inside = false;
	
	do
	:: //No condition guard, thus always true!
		if
		:: (state == closed) -> break;
		:: (inside) ->
			n--;
			inside = false;
		:: else ->
			if
			:: (state == open) ->
				//Before entering, we send a request signal to enter and wait for a response signal from the manager
				//indicating it's okay to enter.
				//This is an additional check to ensure that the museum is not already full as it helps the manager keep track of
				//each person that enters.
				req!true;gnt?true;
				n++;
				inside = true; /*enter*/
				//The client has entered the museum and the manager should be made aware of this. This
				//is done by sending a signal to the manager.
				done!true;
			:: else -> skip /*wait to enter */
			fi
		fi
	od
}

proctype manager (byte max)
{
	do
	::  assert (n<=max); //The assertion
	//:: assert ((!(state==wait))||((state==wait)&&(n<(max+2))));
	if
		::(time_to_close) ->
			state = closed;
			break;
		:: else ->
			if
			:: (n < max) ->
				state = open; /* more room inside */
				//These statements ensures that the manager keeps track of each person that enters the museum.
				//If these statements were not present then more than one client may enter the museum once it is open.
				//If the museum is open, the manager first expects to receive a request signal from the client. It will then
				//accept the request signal allowing the client to enter. Once the client has entered, the manager expects to
				//receive a done signal. Once this signal is received, the manager can proceed to check and see if the total number
				//of clients in the musuem is less than the maximum and repeat the process.
				req?true;gnt!true;done?true;
			:: else ->
				state = wait; /* too many people inside */
			fi
		fi
	od
}

init
{
	run manager(3);
	run client();
	run client();
	run client();
	run client();
	run client();
}
