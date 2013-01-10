proctype CalCheck(int month , year)
{
if
:: ( month > 12)->
	printf("ERROR:The month is greater then 12\n");
:: ( month < 1)->
	printf("ERROR: The month is less then 1\n");
:: (year > 9999)->
	printf("ERROR: The year is greater then 4 digits\n");
::(year < 1)->
	printf("ERROR: The value of a year is less then 1\n");
::else->
	if
	::( month != 2)->
		if
		::(month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12)->
			printf("Month: %d of Year: %d has 31 Days\n",month,year);
		::else->
			printf("Month: %d  of Year: %d has 30 Days\n",month,year);
		fi
	::else->
		if
		::(year%400 ==0)->
			printf("Month: %d of Year: %d  has 29 Days\n",month,year);
		::(year%100 ==0)->
			printf("Month: %d of Year: %d has 28 Days\n",month,year);
		::(year%4 ==0)->
			printf("Month: %d of Year: %d has 29 Days\n",month,year);
		::else->
			printf("Month: %d of Year: %d has 28 Days\n",month,year);
		fi
	fi 
fi
}

init
{
	//Check leap Year
	printf("Lets Check Some Cases:\n");
	printf("lets Check Leap years\n");
	printf("1\)Feb on 1990 has 28 days:\n");
	run CalCheck(02,1990);
	(_nr_pr == 1)-> 
	     printf("2\)Feb on 2004 has 29 days:\n"); 

	run CalCheck(02,2004);

	(_nr_pr == 1)-> 
             printf("3\)Check all 31 day months:\n");	

	run CalCheck(01,2000);
	(_nr_pr == 1)->;
	run CalCheck(03,2001);
	(_nr_pr == 1)->;
	run CalCheck(05,2002);
	(_nr_pr == 1)->;
	run CalCheck(07,2003);
	(_nr_pr == 1)->;
	run CalCheck(08,2004);
	(_nr_pr == 1)->;
	run CalCheck(10,2005);
	(_nr_pr == 1)->;
	run CalCheck(12,2006);



	(_nr_pr == 1)->
             printf("4\)Check all 30 day months:\n");

	run CalCheck(04,1996);
	(_nr_pr == 1)->;
	run CalCheck(06,1997);
	(_nr_pr == 1)->;
	run CalCheck(09,1998);
	(_nr_pr == 1)->;
	run CalCheck(11,1999);

	(_nr_pr == 1)->
             printf("Now we will check the 4 cases that can break the conditions\n");

	//Now Check cases That Break
	run CalCheck(13,1523);
	(_nr_pr == 1)->;
	run CalCheck(00,1834);
	(_nr_pr == 1)->;
	run CalCheck(05,10000);
	(_nr_pr == 1)->;
	run CalCheck(05,0);
	
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
