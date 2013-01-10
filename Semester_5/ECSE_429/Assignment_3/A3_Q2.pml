//Represents the possible states of the components.
mtype = {busy, creatingConnection, connected,  closingConnection, waitingForTeleDigits, conversing, ringing, dialing, onhook, offHook,  idle, callerMessage, calleeMessage};

//Channer through which Amy and Sheldon will exchange messages.
chan globalChan = [1] of {mtype};

//The states of the four components at a given time. Intially, they're all idle.
mtype amyState = idle;
mtype sheldonState = idle;
mtype switchAState = idle;
mtype switchBState = idle;

//Signals that are exchanged between Amy and Switch A.
bool offHookAmyToA = 0;
bool onHookAmyToA = 0;
bool teleDigitsAmyToA = 0;
bool dialToneAToAmy;
bool ringToneAToAmy;
bool noAnsAToAmy = 0;
bool busyAToAmy = 0;
bool voice = 0;

//Signals that are exchanged between Switch A and Switch B.
bool noAnsBToA = 0;
bool busyBToA = 0;
bool iam = 0;
bool acm = 0;
bool anm = 0;
bool rel = 0;
bool rlc = 0;

//Signals that are exchanged between Switch B and Sheldon.
bool offHookSheldonToB = 0;
bool onHookSheldonToB = 0;
bool ringToneBToSheldon;
bool noAnsSheldonToB = 0;
bool busySheldonToB = 0;
bool busyToneBToSheldon = 0;


proctype amy()
{
   do
   ::
		if
		:: 
		::
			printf("about to make a call\n");
			//The caller has lifted the phone off the hook.
			amyState = offHook;
			offHookAmyToA = 1;
			
			//Wait until a dialtone is received from switch A.
			(dialToneAToAmy == 1);
			
			//reset the dialtone for the next run.
			dialToneAToAmy = 0;
			
			//When the caller receives a dialtone, his/her state changes to /dialing 
			amyState = dialing;
			
			//Upon receiving a dialtone, the caller sends tele digits.
			teleDigitsAmyToA = 1;
			
			//The caller is waiting for a ring tone
			(ringToneAToAmy == 1);
			
			//Reset the ring tone for the next run.
			ringToneAToAmy = 0;
			
			//Upon receving the ring tone the caller enters in the ringing state.
			amyState = ringing;
			
			//At this point one of three things can happen: 
			//1.The caller gets connected, 2.The callee is busy, 3.The callee does not pickup
			(voice == 1 || busyAToAmy == 1 || noAnsAToAmy == 1 );
			
			//Reset the signals
			if
			::(busyAToAmy) ->
				busyAToAmy = 0;
			::(noAnsAToAmy) ->
				noAnsAToAmy = 0;
			::(voice == 1) ->
				voice = 0;
				
				//Amy's now talking to Sheldon so her state changes.
				amyState = conversing;
				
				//Exchanging messages.
				globalChan!callerMessage;
				globalChan?calleeMessage;
				globalChan!callerMessage;
				globalChan?calleeMessage;
				
				//Amy puts the phone back on the hook after a while.
				onHookAmyToA = 1;
			fi;
			
			//Amy goes back to being idle.
			amyState = idle;
			printf("finished one call attempt\n");
		fi;
	od;
}

proctype sheldon()
{
    do
	::
		if
		:: 
			//Case where Sheldon is busy.
			//(ringToneBToSheldon == 1);
			ringToneBToSheldon = 0;
			busySheldonToB = 1;
			goto next;	
		:: 
			//Case where Sheldon anwers.
			(ringToneBToSheldon == 1);
			ringToneBToSheldon = 0;
			shelddonState = ringing;
			
			if
			::noAnsSheldonToB = 1; goto next; //Case where sheldon is unavailable
			::
				//Case where Sheldon picks up.
				offHookSheldonToB = 1;

				//Sheldon lifts the phone.
				sheldonState = conversing;
				
				//Amy and Sheldon exchange messages.
				globalChan?callerMessage;
				globalChan!calleeMessage;
				globalChan?callerMessage;
				globalChan!calleeMessage;
				
				//Sheldon waits for Amy to hang up.
				(busyToneBToSheldon == 1);
				busyToneBToSheldon = 0;
				
				//Sheldon hangs up himself.
				onHookSheldonToB = 1;
			fi;
		fi;
		next:;
		sheldonState = idle;
		
	 od;
}

proctype switchA()
{
    do
    ::
        	(offHookAmyToA == 1);
		offHookAmyToA = 0;
		
		//Send a dialtone signal to the caller.
		dialToneAToAmy = 1;
		
		//Switch A is not in the state where it is attempting to establish a connection.
		switchAState = waitingForTeleDigits;
		
		//Wait until we receive the callee number from the caller.
		(teleDigitsAmyToA == 1);
		
		//Reset the teleDigits signal.
		teleDigitsAmyToA = 0;

		//Switch A is attempting to connect to switch B in order for a connection to be established between
		//the caller and the receiver.
		switchAState = creatingConnection;
		
		//Sending an initial address message.
		iam = 1;
		
		//Waiting for an address complete message.
		(acm == 1);
		
		//Reseting address complete message.
		acm =  0;
		
		//Sends a ring tone to the caller.
		ringToneAToAmy = 1;
		
		//Waits for one of the following signals to be sent by switch B.
		(anm == 1 || noAnsBToA == 1 || busyBToA == 1);
		
		//We have to send the notifications to the caller based on the information we received from B.
		if
			::(noAnsBToA == 1) ->
				noAnsBToA = 0;
				noAnsAToAmy = 1;
			::(busyBToA == 1) ->
				busyBToA = 0;
				busyAToAmy = 1;
			::(anm == 1) ->
				anm = 0;
				switchAState = connected;
				voice = 1;
				
				//At this point, the connection should be established between the caller and the callee. Thus,
				//we wait for Amy to put the phone down befor taking further action.
				(onHookAmyToA == 1);
				onHookAmyToA = 0;
				
				//We are about to close the connection as the caller has decided to put the phone down.
				switchAState = closingConnection;
				
				//Sending a release signal to switch B.
				rel = 1;
				
				//Expecting a release confirmation signal to be sent by switch B.
				(rlc == 1);
				rlc = 0;
		fi;
		
		//Switch A goes back to the idle state until it receives a new off hook signal.
		switchAState = idle;
    od;
}

proctype switchB()
{   
    do
    ::
		//Switch B waits for a initial address message signal.
		(iam == 1);
		iam = 0;
		
		//Swith A and B will work together to establish a connection between Amy and Sheldon.
		switchBState = creatingConnection;

		//Switch B is setting the address complete message signal.
		acm = 1;
		
		//Informs sheldon that there is a call coming in for him
		ringToneBToSheldon = 1;
		
		//Waits for the response from Sheldon. Could be one of either:
		//1.The callee picks up 2.The callee is busy, 3.The callee does not pickup
		(offHookSheldonToB == 1 || noAnsSheldonToB == 1 || busySheldonToB == 1);
		
		//The results that may be returned by Sheldon.
		if
		::(busySheldonToB == 1) ->
			busySheldonToB = 0;
			busyBToA = 1;
		::(noAnsSheldonToB == 1) ->
			noAnsSheldonToB = 0;
			noAnsBToA = 1;
		::(offHookSheldonToB == 1)->
			offHookSheldonToB = 0;
			switchBState = connected;
			
			//Sends the answer message to switch A.
			anm = 1;
			
			//At this point the connection is established so we wait for the release signal from
			//switch A before proceeding further.
			(rel == 1);
			rel = 0;
			
			//Switch B is no longer connected to switch A.
			switchBState = closingConnection;
			
			//Send a busy tone to sheldon.
			busyToneBToSheldon = 1;
			
			//Wait for sheldon to put the phone back on the hook.
			(onHookSheldonToB == 1);
			onHookSheldonToB = 0;
			
			//Send a release confirmation signal to switch A.
			rlc = 1;
		fi;
		
		//Switch B re-enters the idle state.
		switchBState = idle;
    od; 
}

init
{
run amy();
run switchA();
run switchB();
run sheldon();
(_nr_pr == 1);
}
