/**********************************************************************/
/*The purpose of this program is to receive as input the month and 	*/
/*year parameters and return the orignal month, year, as well as the	*/
/*number of days for the given month in the year. The process  	*/
/*CalCheck_user uses a channel to pass the month, year, and result.   */
/*The CalCheck process uses the same channel to return the response.	*/
/**********************************************************************/

//Define a global mtype
mtype = {Month,Year,Result};

//Define a global channel
chan global_chan = [2] of {mtype,int};

proctype CalCheck()
{
	int month;
	int year;
	int result;

	//Blocks here until the month is received.
	global_chan?Month,month;

	//Blocks here until the year is received.
	global_chan?Year,year;

	//Blocks here until the result is received.
	global_chan?Result,result;
	
        if
        :: (month<1) ->
        	result=-9999; 
        :: (month>12) ->
                result=-9999;
        :: (year<1) ->
                result=-9999;
        :: (year>9999) ->
                result=-9999;
        :: else ->
                if
                :: (month!=2) ->	//The month is not the second month.
                     if
			::(month==1||month==3||month==5||month==7||month==8||month==10||month==12) ->	
				result=31;
			::else ->
				result=30;		
			fi
                :: else ->	//The next set of conditions check to see if the year is a leap year.
                        if
                        ::(year%400==0) ->
                                result=29;
                        ::(year%100==0) ->
                                result=28;
                        ::(year%4==0) ->
                                result=29;
                        ::else ->
                                result=28;
                        fi
                fi
        fi;

	//Send the month that was received.
	global_chan!Month,month;

	//Send the year that was received.
	global_chan!Year,year;

	//Send the result (no. of days) for the month 
	//and year that was received.
	global_chan!Result,result;
}


proctype CalCheck_user(int month, year)
{
	int monthResp;
       int yearResp;
	int resultResp;

	//Send the month to the CalCheck process.
       global_chan!Month,month;

	//Send the year to the CalCheck process.
	global_chan!Year,year;

	//Send the pid to the CalCheck process.
	global_chan!Result,_pid;
	
	//Block until the month is received.
       global_chan?Month,monthResp;
	
	//Block until the year is received.
	global_chan?Year,yearResp;

	//Block until the number of days are received.
	global_chan?Result,resultResp;

       printf ("The number of days for month %d in the year %d is %d days.\n",monthResp,yearResp,resultResp);
}


init
{	//Some basic tests.
	run CalCheck();
	run CalCheck_user(1,2012);
	(_nr_pr == 1)->
		printf ("Init has ID: %d\n",_pid);

	run CalCheck();
	run CalCheck_user(2,2012);
	(_nr_pr == 1)->
		printf ("Init has ID: %d\n",_pid);

	run CalCheck();
	run CalCheck_user(3,2012);
	(_nr_pr == 1)->
		printf ("Init has ID: %d\n",_pid);

	run CalCheck();
	run CalCheck_user(2,4000);
	(_nr_pr == 1)->
		printf ("Init has ID: %d\n",_pid);

	run CalCheck();
	run CalCheck_user(2,2100);
	(_nr_pr == 1)->
		printf ("Init has ID: %d\n",_pid);
}
