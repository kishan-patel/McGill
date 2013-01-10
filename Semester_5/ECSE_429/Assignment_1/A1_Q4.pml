/**********************************************************************/
/*The purpose of this program is to communicate between two processes */
/*through the use of a channel. Furthermore, the if-defined 		*/
/*non-deterministic guard will also be used. Prior to transmitting any*/
/*data, the sender initiates a start signal to which the receiver has */
/*to respond with an ack signal. After the handshake has been completed	*/
/*the sender may send data through a transmit_req signal or initiate	*/
/*a shutdown by initiating a shudown signal and waiting for an shutdown*/
/*acknowldegement from the receiver.
/**********************************************************************/

//Global mytype and channels.
mtype={start,ack,transmit_req,shutdown,shutdown_ack};
chan sender_channel = [1] of {mtype};
chan receiver_channel = [1] of {mtype};

proctype sender()
{
	//When an ack signal is received 
	//from the receiver, the handshake is completed.
	sender_channel!start;
	receiver_channel?ack;

	//This loop executes until a shutdown signal is not sent and acknowledged.
	//If a shutdown signal is not sent, it sends a transmit_req signal
	//followed by an arbritray integer between 1-5. 
	do
	::
		if
			::sender_channel!transmit_req;sender_channel!1;
			::sender_channel!transmit_req;sender_channel!2;
			::sender_channel!transmit_req;sender_channel!3;
			::sender_channel!transmit_req;sender_channel!4;
			::sender_channel!transmit_req;sender_channel!5;
			::sender_channel!shutdown;receiver_channel?shutdown_ack;goto done;
		fi;
	od;
	done:
		printf("Sender shutting down.\n");
}


proctype receiver()
{
	//When a start signal is received, the receiver 
	//responds with an ack to complete the handshake.
	sender_channel?start;
	receiver_channel!ack;

	//This while loop executes until the receiver doesn't 
	//receive a shutdown signal from the sender. If it does,
	//it responds with a shutdwon acknowledgement at which 
	//point both process should terminate. If a transmit_req
	//signal is received instead, it waits for a integer 
	//value from the sender.
	do
	::
		int signal;
		sender_channel?signal;
		if
		::(signal == transmit_req)->
			sender_channel?signal;
			printf("Value sent by sender is: %d.\n",signal);
		::(signal  == shutdown)->
			receiver_channel!shutdown_ack;
			goto done;
		fi;
	od;
	done:
		printf("Receiver shutting down.\n");
}

init
{
	run sender();
	run receiver();
	(_nr_pr == 1)->
		printf ("Init has ID: %d.\n",_pid);
}
