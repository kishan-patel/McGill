/**********************************************************************/
/*The purpose of this program is to create a process which accepts as */
/*an argument two input parameters - month and year. It then validates*/
/*the arguments passed. If the month and year are both valid, the	*/
/*process prints to the console, the number of days that are present	*/
/*for the given month in the given year.					*/
/**********************************************************************/

proctype CalCheck(int month, year)
{
	if 
	:: (month<1) ->
		printf ("The month cannot be less than 1.\n");
	:: (month>12) ->
		printf ("The month cannot be greater than 12.\n");
	:: (year<1) ->
		printf ("The year cannot be less than 0.\n");
	:: (year>9999) ->
		printf ("The year cannot be more than 4 digits.\n");
	:: else ->
		if
		:: (month!=2) -> //The month is not the second month.
			if
			::(month==1||month==3||month==5||month==7||month==8||month==10||month==12)->	
				printf ("The month %d in %d has 31 days.\n",month,year);
			::else->
				printf ("The month %d in %d has 30 days.\n",month,year);		
			fi
		:: else -> 
			if	//The next set of conditions check to see if the year is a leap year.
			::(year%400==0) ->
				printf ("The month %d in %d has 29 days.\n",month,year);
			::(year%100==0) ->
				printf ("The month %d in %d has 28 days.\n",month,year);
			::(year%4==0) ->
				printf ("The month %d in %d has 29 days.\n",month,year);
			::else ->
				printf ("The month %d in %d has 28 days.\n",month,year);	
			fi			
		fi
	fi
	
}

init
{	
	//These set of test cases test the program to see whether they
	//handle invalid input correctly.
	run CalCheck(0,2000);
	 (_nr_pr == 1)->
                printf ("Init has ID: %d\n",_pid);
	run CalCheck(1,99999);
	 (_nr_pr == 1)->
                printf ("Init has ID: %d\n",_pid);
	run CalCheck(0,99999);
	 (_nr_pr == 1)->
                printf ("Init has ID: %d\n",_pid);

	//These set of test cases are normal cases (no leap year) which
	//test whether the correct number of days are calculated for
	//each of the twelve months in the year.
	run CalCheck(1,2000);
	 (_nr_pr == 1)->
                printf ("Init has ID: %d\n",_pid);
	run CalCheck(2,2000);
	 (_nr_pr == 1)->
                printf ("Init has ID: %d\n",_pid);
	run CalCheck(3,2000);
	 (_nr_pr == 1)->
                printf ("Init has ID: %d\n",_pid);
	run CalCheck(4,2000);
	 (_nr_pr == 1)->
                printf ("Init has ID: %d\n",_pid);
	run CalCheck(5,2000);
	 (_nr_pr == 1)->
                printf ("Init has ID: %d\n",_pid);
	run CalCheck(6,2000);
	 (_nr_pr == 1)->
                printf ("Init has ID: %d\n",_pid);
	run CalCheck(7,2000);
	 (_nr_pr == 1)->
                printf ("Init has ID: %d\n",_pid);
	run CalCheck(8,2000);
	 (_nr_pr == 1)->
                printf ("Init has ID: %d\n",_pid);
	run CalCheck(9,2000);
	 (_nr_pr == 1)->
                printf ("Init has ID: %d\n",_pid);
	run CalCheck(10,2000);
	 (_nr_pr == 1)->
                printf ("Init has ID: %d\n",_pid);
	run CalCheck(11,2000);
	 (_nr_pr == 1)->
                printf ("Init has ID: %d\n",_pid);
	run CalCheck(12,2000);
	 (_nr_pr == 1)->
                printf ("Init has ID: %d\n",_pid);

	//These set of tests check if the number of days
	//in February are calculated properly depending 
	//on the year.
       	run CalCheck(2,2011);
	 (_nr_pr == 1)->
                printf ("Init has ID: %d\n",_pid);
       	run CalCheck(2,2012);
	 (_nr_pr == 1)->
                printf ("Init has ID: %d\n",_pid);
	run CalCheck(2,2100);
	 (_nr_pr == 1)->
                printf ("Init has ID: %d\n",_pid);
	run CalCheck(2,4000);
	 (_nr_pr == 1)->
                printf ("Init has ID: %d\n",_pid);

}
