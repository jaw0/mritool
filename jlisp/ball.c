
/*
    Copyright (c) 1994 Jeff Weisberg

    see the file "License"
*/

#ifdef RCSID
static const char *const rcsid
= "@(#)$Id: ball.c,v 1.1 1995/11/23 04:30:44 jaw Exp jaw $";
#endif

#include <jlisp.h>
#include <setjmp.h>

/*
    ball.c? Huh? What does this have to with balls?

    Well, see whats the most natural thing to do with a ball?
    Why to throw them and catch them, of course....
*/

typedef struct Unwind {
	Obj envca, envcd, code;
	struct Unwind *next;
} Unwind;

typedef struct {
	jmp_buf jb;
	Backtrace *bt;
	Obj envca, envcd, envl;
	Unwind *up;
	Obj retval;
	int scope;
} CatchFrame;

Unwind *unwind_list = 0;
extern Obj envcurr, envlist;
extern Backtrace *backtrace_list;
extern int static_scope;

void markctfrm(Obj a){
	/* nop */
}


DEFUN("catch", Fcatch, Scatch, 1,1,0,1,
      "(catch tag body...) setup a catch frame\n"
      "the return value will be the value specified by the throw\n"
      "if the throw did not specify a value, it will default to #t\n"
      "or the result of body if no throw was caught\n"
      "[see also: throw unwind-protect]",
      (Obj args))
{
	Obj tag = CAR(args);
	Obj body= CDR(args);
	Obj acf = newcell();
	CatchFrame cf;
	int sigs;

	DISABLE( sigs );
	cf.envcd = CDR(envcurr);
	cf.envca = CAR(envcurr);
	cf.envl  = envlist;
	cf.bt    = backtrace_list;
	cf.up    = unwind_list;
	cf.scope = static_scope;
	
	CAR(acf) = MAKETYPE( TPV_CATCHFRM );
	CDR(acf) = (Obj)&cf;
	RENABLE( sigs );
	
	if( setjmp( cf.jb )){
		/* caught a throw */

		/* snarf return value */
		if( NBOUNDP(cf.retval))
			cf.retval = IC_TRUE;
		/* as the catch cannot have happened within
		   a critical section, signals must have been
		   enabled at that time, so we make sure they
		   still are (although I don't believe that a
		   throw could happen in a crit sect either)
		   ...
		*/
		RENABLE( 0 );
	}else{

		/* add it to catchframe alist */
		acf = Fcons(Feval(tag), acf);
		CAR(envcurr) = Fcons( acf, CAR(envcurr));

		cf.retval = Fprogn( body );
	}

	/* remove it from alist, and restore state */

	DISABLE( sigs );
	envlist = cf.envl;
	CAR(envcurr) = cf.envca;
	CDR(envcurr) = cf.envcd;
	backtrace_list = cf.bt;
	unwind_list = cf.up;
	static_scope = cf.scope;
	RENABLE( sigs );
	
	return cf.retval;
}

DEFUN("throw", Fthrow, Sthrow, 1,2,1,0,
      "(throw tag [value]) jump out of the matching catch frame\n"
      "if a value is given, it will be the return value from the matching (eqv) catch\n"
      "if there is no matching catch, throw will return #f\n"
      "[see also: catch unwind-protect]",
      (Obj tag, Obj val))
{
	Obj m;
	CatchFrame *cf;
	Unwind *up;
	
	m = Fassv(tag, CAR(envcurr));
	
	if( m == IC_FALSE )
		/* no matching tag */
		return IC_FALSE;
	
	cf = (CatchFrame*)CDDR(m);
	cf->retval = val;
	
	/* handle unwind protects */
	for(up=unwind_list; up!=cf->up; up=up->next){
		CAR(envcurr) = up->envca;
		CDR(envcurr) = up->envcd;
		unwind_list  = up->next;
		Fprogn( up->code );
	}

	longjmp( cf->jb, 1);
}

DEFUN("unwind-protect", Funwindpro, Sunwindpro, 1,1,0,1,
      "(unwind-protect now-form later-forms...)\n"
      "make sure later-forms get eval'ed even if we throw\n"
      "while doing now-form, returns the result of now-form\n"
      "[see-also: catch throw]",
      (Obj argl))
{
	Obj now = CAR(argl);
	Obj later = CDR(argl);
	Obj retval;
	Unwind up;

	up.next  = unwind_list;
	up.envca = CAR(envcurr);
	up.envcd = CDR(envcurr);
	up.code  = later;
	unwind_list = &up;

	retval = Feval( now );

	unwind_list = up.next;
	Fprogn( later );

	return retval;
}


