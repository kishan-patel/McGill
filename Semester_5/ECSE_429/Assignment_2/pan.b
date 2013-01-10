	switch (t->back) {
	default: Uerror("bad return move");
	case  0: goto R999; /* nothing to undo */

		 /* PROC :init: */

	case 3: /* STATE 1 */
		;
		;
		delproc(0, now._nr_pr-1);
		;
		goto R999;

	case 4: /* STATE 2 */
		;
		;
		delproc(0, now._nr_pr-1);
		;
		goto R999;

	case 5: /* STATE 3 */
		;
		;
		delproc(0, now._nr_pr-1);
		;
		goto R999;

	case 6: /* STATE 4 */
		;
		;
		delproc(0, now._nr_pr-1);
		;
		goto R999;

	case 7: /* STATE 5 */
		;
		;
		delproc(0, now._nr_pr-1);
		;
		goto R999;

	case 8: /* STATE 6 */
		;
		p_restor(II);
		;
		;
		goto R999;

		 /* PROC cook */
;
		;
		;
		;
		
	case 11: /* STATE 3 */
		;
		now.req[ Index(((P3 *)this)->id, 4) ] = trpt->bup.oval;
		;
		goto R999;
;
		;
		
	case 13: /* STATE 5 */
		;
		now.pot = trpt->bup.oval;
		;
		goto R999;

	case 14: /* STATE 6 */
		;
		now.gnt[ Index(((P3 *)this)->id, 4) ] = trpt->bup.oval;
		;
		goto R999;

	case 15: /* STATE 10 */
		;
		p_restor(II);
		;
		;
		goto R999;

		 /* PROC client */
;
		;
		
	case 17: /* STATE 2 */
		;
		now.req[ Index(((P2 *)this)->id, 4) ] = trpt->bup.oval;
		;
		goto R999;
;
		;
		
	case 19: /* STATE 4 */
		;
		now.pot = trpt->bup.oval;
		;
		goto R999;

	case 20: /* STATE 5 */
		;
		now.gnt[ Index(((P2 *)this)->id, 4) ] = trpt->bup.oval;
		;
		goto R999;

	case 21: /* STATE 9 */
		;
		p_restor(II);
		;
		;
		goto R999;

		 /* PROC round_robin_arbiter */

	case 22: /* STATE 2 */
		;
		((P1 *)this)->i = trpt->bup.ovals[1];
	/* 0 */	((P1 *)this)->i = trpt->bup.ovals[0];
		;
		;
		ungrab_ints(trpt->bup.ovals, 2);
		goto R999;
;
		;
		;
		;
		
	case 25: /* STATE 9 */
		;
		now.gnt[ Index(((P1 *)this)->i, 4) ] = trpt->bup.oval;
		;
		goto R999;
;
		;
		
	case 27: /* STATE 11 */
		;
		now.req[ Index(((P1 *)this)->i, 4) ] = trpt->bup.oval;
		;
		goto R999;

	case 28: /* STATE 16 */
		;
		((P1 *)this)->i = trpt->bup.oval;
		;
		goto R999;

	case 29: /* STATE 16 */
		;
		((P1 *)this)->i = trpt->bup.oval;
		;
		goto R999;

	case 30: /* STATE 20 */
		;
		p_restor(II);
		;
		;
		goto R999;

		 /* CLAIM never_0 */
;
		;
		
	case 32: /* STATE 8 */
		;
		p_restor(II);
		;
		;
		goto R999;
	}

