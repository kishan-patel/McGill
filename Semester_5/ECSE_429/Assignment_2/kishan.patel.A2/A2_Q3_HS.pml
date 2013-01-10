//cook-savage problem
#define M 4

short pot;

active proctype cook()
{
	do
	::	//No do-guard, always true
		if
		:: (pot < 0) -> assert (false)
		:: (pot == 0) -> pot = M
		:: else -> skip
		fi
	od
}

active [3] proctype savage()
{
	do
	:: //No do-guard, always true
		if
		:: (pot > 0) ->
		    pot--;
		    printf("%d got food\n",_pid)
		:: else -> skip
		fi
	od
}
		
