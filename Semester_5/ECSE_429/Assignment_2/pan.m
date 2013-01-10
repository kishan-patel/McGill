#define rand	pan_rand
#if defined(HAS_CODE) && defined(VERBOSE)
	#ifdef BFS_PAR
		bfs_printf("Pr: %d Tr: %d\n", II, t->forw);
	#else
		cpu_printf("Pr: %d Tr: %d\n", II, t->forw);
	#endif
#endif
	switch (t->forw) {
	default: Uerror("bad forward move");
	case 0:	/* if without executable clauses */
		continue;
	case 1: /* generic 'goto' or 'skip' */
		IfNotBlocked
		_m = 3; goto P999;
	case 2: /* generic 'else' */
		IfNotBlocked
		if (trpt->o_pm&1) continue;
		_m = 3; goto P999;

		 /* PROC :init: */
	case 3: /* STATE 1 - A2_Q3.pml:77 - [(run round_robin_arbiter())] (0:0:0 - 1) */
		IfNotBlocked
		reached[4][1] = 1;
		if (!(addproc(II, 1, 0)))
			continue;
		_m = 3; goto P999; /* 0 */
	case 4: /* STATE 2 - A2_Q3.pml:78 - [(run cook(0))] (0:0:0 - 1) */
		IfNotBlocked
		reached[4][2] = 1;
		if (!(addproc(II, 3, 0)))
			continue;
		_m = 3; goto P999; /* 0 */
	case 5: /* STATE 3 - A2_Q3.pml:79 - [(run client(1))] (0:0:0 - 1) */
		IfNotBlocked
		reached[4][3] = 1;
		if (!(addproc(II, 2, 1)))
			continue;
		_m = 3; goto P999; /* 0 */
	case 6: /* STATE 4 - A2_Q3.pml:80 - [(run client(2))] (0:0:0 - 1) */
		IfNotBlocked
		reached[4][4] = 1;
		if (!(addproc(II, 2, 2)))
			continue;
		_m = 3; goto P999; /* 0 */
	case 7: /* STATE 5 - A2_Q3.pml:81 - [(run client(3))] (0:0:0 - 1) */
		IfNotBlocked
		reached[4][5] = 1;
		if (!(addproc(II, 2, 3)))
			continue;
		_m = 3; goto P999; /* 0 */
	case 8: /* STATE 6 - A2_Q3.pml:82 - [-end-] (0:0:0 - 1) */
		IfNotBlocked
		reached[4][6] = 1;
		if (!delproc(1, II)) continue;
		_m = 3; goto P999; /* 0 */

		 /* PROC cook */
	case 9: /* STATE 1 - A2_Q3.pml:66 - [((req[id]==0))] (0:0:0 - 1) */
		IfNotBlocked
		reached[3][1] = 1;
		if (!((((int)now.req[ Index(((int)((P3 *)this)->id), 4) ])==0)))
			continue;
		_m = 3; goto P999; /* 0 */
	case 10: /* STATE 2 - A2_Q3.pml:67 - [assert((pot>=0))] (0:0:0 - 1) */
		IfNotBlocked
		reached[3][2] = 1;
		spin_assert((now.pot>=0), "(pot>=0)", II, tt, t);
		_m = 3; goto P999; /* 0 */
	case 11: /* STATE 3 - A2_Q3.pml:68 - [req[id] = 1] (0:0:1 - 1) */
		IfNotBlocked
		reached[3][3] = 1;
		(trpt+1)->bup.oval = ((int)now.req[ Index(((int)((P3 *)this)->id), 4) ]);
		now.req[ Index(((P3 *)this)->id, 4) ] = 1;
#ifdef VAR_RANGES
		logval("req[cook:id]", ((int)now.req[ Index(((int)((P3 *)this)->id), 4) ]));
#endif
		;
		_m = 3; goto P999; /* 0 */
	case 12: /* STATE 4 - A2_Q3.pml:69 - [((gnt[id]==1))] (0:0:0 - 1) */
		IfNotBlocked
		reached[3][4] = 1;
		if (!((((int)now.gnt[ Index(((int)((P3 *)this)->id), 4) ])==1)))
			continue;
		_m = 3; goto P999; /* 0 */
	case 13: /* STATE 5 - A2_Q3.pml:70 - [pot = 4] (0:0:1 - 1) */
		IfNotBlocked
		reached[3][5] = 1;
		(trpt+1)->bup.oval = now.pot;
		now.pot = 4;
#ifdef VAR_RANGES
		logval("pot", now.pot);
#endif
		;
		_m = 3; goto P999; /* 0 */
	case 14: /* STATE 6 - A2_Q3.pml:71 - [gnt[id] = 0] (0:0:1 - 1) */
		IfNotBlocked
		reached[3][6] = 1;
		(trpt+1)->bup.oval = ((int)now.gnt[ Index(((int)((P3 *)this)->id), 4) ]);
		now.gnt[ Index(((P3 *)this)->id, 4) ] = 0;
#ifdef VAR_RANGES
		logval("gnt[cook:id]", ((int)now.gnt[ Index(((int)((P3 *)this)->id), 4) ]));
#endif
		;
		_m = 3; goto P999; /* 0 */
	case 15: /* STATE 10 - A2_Q3.pml:73 - [-end-] (0:0:0 - 1) */
		IfNotBlocked
		reached[3][10] = 1;
		if (!delproc(1, II)) continue;
		_m = 3; goto P999; /* 0 */

		 /* PROC client */
	case 16: /* STATE 1 - A2_Q3.pml:54 - [((req[id]==0))] (0:0:0 - 1) */
		IfNotBlocked
		reached[2][1] = 1;
		if (!((((int)now.req[ Index(((int)((P2 *)this)->id), 4) ])==0)))
			continue;
		_m = 3; goto P999; /* 0 */
	case 17: /* STATE 2 - A2_Q3.pml:55 - [req[id] = 1] (0:0:1 - 1) */
		IfNotBlocked
		reached[2][2] = 1;
		(trpt+1)->bup.oval = ((int)now.req[ Index(((int)((P2 *)this)->id), 4) ]);
		now.req[ Index(((P2 *)this)->id, 4) ] = 1;
#ifdef VAR_RANGES
		logval("req[client:id]", ((int)now.req[ Index(((int)((P2 *)this)->id), 4) ]));
#endif
		;
		_m = 3; goto P999; /* 0 */
	case 18: /* STATE 3 - A2_Q3.pml:56 - [((gnt[id]==1))] (0:0:0 - 1) */
		IfNotBlocked
		reached[2][3] = 1;
		if (!((((int)now.gnt[ Index(((int)((P2 *)this)->id), 4) ])==1)))
			continue;
		_m = 3; goto P999; /* 0 */
	case 19: /* STATE 4 - A2_Q3.pml:58 - [pot = (pot-1)] (0:0:1 - 1) */
		IfNotBlocked
		reached[2][4] = 1;
		(trpt+1)->bup.oval = now.pot;
		now.pot = (now.pot-1);
#ifdef VAR_RANGES
		logval("pot", now.pot);
#endif
		;
		_m = 3; goto P999; /* 0 */
	case 20: /* STATE 5 - A2_Q3.pml:59 - [gnt[id] = 0] (0:0:1 - 1) */
		IfNotBlocked
		reached[2][5] = 1;
		(trpt+1)->bup.oval = ((int)now.gnt[ Index(((int)((P2 *)this)->id), 4) ]);
		now.gnt[ Index(((P2 *)this)->id, 4) ] = 0;
#ifdef VAR_RANGES
		logval("gnt[client:id]", ((int)now.gnt[ Index(((int)((P2 *)this)->id), 4) ]));
#endif
		;
		_m = 3; goto P999; /* 0 */
	case 21: /* STATE 9 - A2_Q3.pml:61 - [-end-] (0:0:0 - 1) */
		IfNotBlocked
		reached[2][9] = 1;
		if (!delproc(1, II)) continue;
		_m = 3; goto P999; /* 0 */

		 /* PROC round_robin_arbiter */
	case 22: /* STATE 1 - A2_Q3.pml:33 - [((i>3))] (14:0:2 - 1) */
		IfNotBlocked
		reached[1][1] = 1;
		if (!((((P1 *)this)->i>3)))
			continue;
		/* dead 1: i */  (trpt+1)->bup.ovals = grab_ints(2);
		(trpt+1)->bup.ovals[0] = ((P1 *)this)->i;
#ifdef HAS_CODE
		if (!readtrail)
#endif
			((P1 *)this)->i = 0;
		/* merge: i = 0(0, 2, 14) */
		reached[1][2] = 1;
		(trpt+1)->bup.ovals[1] = ((P1 *)this)->i;
		((P1 *)this)->i = 0;
#ifdef VAR_RANGES
		logval("round_robin_arbiter:i", ((P1 *)this)->i);
#endif
		;
		/* merge: .(goto)(0, 6, 14) */
		reached[1][6] = 1;
		;
		_m = 3; goto P999; /* 2 */
	case 23: /* STATE 7 - A2_Q3.pml:38 - [((req[i]==1))] (0:0:0 - 1) */
		IfNotBlocked
		reached[1][7] = 1;
		if (!((((int)now.req[ Index(((P1 *)this)->i, 4) ])==1)))
			continue;
		_m = 3; goto P999; /* 0 */
	case 24: /* STATE 8 - A2_Q3.pml:39 - [printf('set to 1. req id: %d.\\n',i)] (0:0:0 - 1) */
		IfNotBlocked
		reached[1][8] = 1;
		Printf("set to 1. req id: %d.\n", ((P1 *)this)->i);
		_m = 3; goto P999; /* 0 */
	case 25: /* STATE 9 - A2_Q3.pml:40 - [gnt[i] = 1] (0:0:1 - 1) */
		IfNotBlocked
		reached[1][9] = 1;
		(trpt+1)->bup.oval = ((int)now.gnt[ Index(((P1 *)this)->i, 4) ]);
		now.gnt[ Index(((P1 *)this)->i, 4) ] = 1;
#ifdef VAR_RANGES
		logval("gnt[round_robin_arbiter:i]", ((int)now.gnt[ Index(((P1 *)this)->i, 4) ]));
#endif
		;
		_m = 3; goto P999; /* 0 */
	case 26: /* STATE 10 - A2_Q3.pml:41 - [((gnt[i]==0))] (0:0:0 - 1) */
		IfNotBlocked
		reached[1][10] = 1;
		if (!((((int)now.gnt[ Index(((P1 *)this)->i, 4) ])==0)))
			continue;
		_m = 3; goto P999; /* 0 */
	case 27: /* STATE 11 - A2_Q3.pml:42 - [req[i] = 0] (0:0:1 - 1) */
		IfNotBlocked
		reached[1][11] = 1;
		(trpt+1)->bup.oval = ((int)now.req[ Index(((P1 *)this)->i, 4) ]);
		now.req[ Index(((P1 *)this)->i, 4) ] = 0;
#ifdef VAR_RANGES
		logval("req[round_robin_arbiter:i]", ((int)now.req[ Index(((P1 *)this)->i, 4) ]));
#endif
		;
		_m = 3; goto P999; /* 0 */
	case 28: /* STATE 13 - A2_Q3.pml:43 - [printf('set to 0. req id: %d\\n',i)] (0:17:1 - 1) */
		IfNotBlocked
		reached[1][13] = 1;
		Printf("set to 0. req id: %d\n", ((P1 *)this)->i);
		/* merge: .(goto)(17, 15, 17) */
		reached[1][15] = 1;
		;
		/* merge: i = (i+1)(17, 16, 17) */
		reached[1][16] = 1;
		(trpt+1)->bup.oval = ((P1 *)this)->i;
		((P1 *)this)->i = (((P1 *)this)->i+1);
#ifdef VAR_RANGES
		logval("round_robin_arbiter:i", ((P1 *)this)->i);
#endif
		;
		/* merge: .(goto)(0, 18, 17) */
		reached[1][18] = 1;
		;
		_m = 3; goto P999; /* 3 */
	case 29: /* STATE 16 - A2_Q3.pml:46 - [i = (i+1)] (0:17:1 - 3) */
		IfNotBlocked
		reached[1][16] = 1;
		(trpt+1)->bup.oval = ((P1 *)this)->i;
		((P1 *)this)->i = (((P1 *)this)->i+1);
#ifdef VAR_RANGES
		logval("round_robin_arbiter:i", ((P1 *)this)->i);
#endif
		;
		/* merge: .(goto)(0, 18, 17) */
		reached[1][18] = 1;
		;
		_m = 3; goto P999; /* 1 */
	case 30: /* STATE 20 - A2_Q3.pml:48 - [-end-] (0:0:0 - 1) */
		IfNotBlocked
		reached[1][20] = 1;
		if (!delproc(1, II)) continue;
		_m = 3; goto P999; /* 0 */

		 /* CLAIM never_0 */
	case 31: /* STATE 1 - A2_Q3.pml:18 - [(!(((pot>0)&&(pot<4))))] (0:0:0 - 1) */
		
#if defined(VERI) && !defined(NP)
#if NCLAIMS>1
		{	static int reported1 = 0;
			if (verbose && !reported1)
			{	int nn = (int) ((Pclaim *)pptr(0))->_n;
				printf("depth %ld: Claim %s (%d), state %d (line %d)\n",
					depth, procname[spin_c_typ[nn]], nn, (int) ((Pclaim *)pptr(0))->_p, src_claim[ (int) ((Pclaim *)pptr(0))->_p ]);
				reported1 = 1;
				fflush(stdout);
		}	}
#else
		{	static int reported1 = 0;
			if (verbose && !reported1)
			{	printf("depth %d: Claim, state %d (line %d)\n",
					(int) depth, (int) ((Pclaim *)pptr(0))->_p, src_claim[ (int) ((Pclaim *)pptr(0))->_p ]);
				reported1 = 1;
				fflush(stdout);
		}	}
#endif
#endif
		reached[0][1] = 1;
		if (!( !(((now.pot>0)&&(now.pot<4)))))
			continue;
		_m = 3; goto P999; /* 0 */
	case 32: /* STATE 8 - A2_Q3.pml:23 - [-end-] (0:0:0 - 1) */
		
#if defined(VERI) && !defined(NP)
#if NCLAIMS>1
		{	static int reported8 = 0;
			if (verbose && !reported8)
			{	int nn = (int) ((Pclaim *)pptr(0))->_n;
				printf("depth %ld: Claim %s (%d), state %d (line %d)\n",
					depth, procname[spin_c_typ[nn]], nn, (int) ((Pclaim *)pptr(0))->_p, src_claim[ (int) ((Pclaim *)pptr(0))->_p ]);
				reported8 = 1;
				fflush(stdout);
		}	}
#else
		{	static int reported8 = 0;
			if (verbose && !reported8)
			{	printf("depth %d: Claim, state %d (line %d)\n",
					(int) depth, (int) ((Pclaim *)pptr(0))->_p, src_claim[ (int) ((Pclaim *)pptr(0))->_p ]);
				reported8 = 1;
				fflush(stdout);
		}	}
#endif
#endif
		reached[0][8] = 1;
		if (!delproc(1, II)) continue;
		_m = 3; goto P999; /* 0 */
	case  _T5:	/* np_ */
		if (!((!(trpt->o_pm&4) && !(trpt->tau&128))))
			continue;
		/* else fall through */
	case  _T2:	/* true */
		_m = 3; goto P999;
#undef rand
	}

