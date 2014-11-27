
/*
    Copyright (c) 1994 Jeff Weisberg

    see the file "License"
*/

#ifdef RCSID
static const char *const rcsid
= "@(#)$Id: debug.c,v 1.1 1995/11/23 04:30:46 jaw Exp jaw $";
#endif

#include <jlisp.h>

extern Backtrace *backtrace_list;
extern Obj sym_quote;

DEFVAR(".already-debugging", Valrdy_dbg, 
       ".already-debugging  used internally by the debugger to avoid recursing",
       IC_FALSE)
     
Obj debugger(Obj what, Obj val){
	/*
	when internally called, "what" will be:
	0  entering fnc
	1  leaving fnc
	*/
	Obj dbg, dbs;
	
	if(! FALSEP( VALUE(Valrdy_dbg))) return val;
	
	dbs = maksym("debugger");
	dbg = getvalue(dbs);
	if( DEFINEDP(dbg) && !FALSEP(Fprocp(dbg)) ){
		VALUE(Valrdy_dbg) = IC_TRUE;
		/* we do:
		(debugger what 'val)
		*/
		val = Feval( Fcons(dbs,
			Fcons(what,
			      Fcons(
				    /* we must quote val, else it will
				    be re-evaled -- a bad thing */
				    Fcons(sym_quote, Fcons(val, IC_NIL)),
				    IC_NIL))));
		VALUE(Valrdy_dbg) = IC_FALSE;
	}
	return val;
}

DEFUN("backtrace", Fbacktrc, Sbacktrc, 0,0,1,0,
      "(backtrace) return a list of the lisp calling frames\n"
      "of the form: (fncname fncdefn arglist dbgflag)",
      ())
{
	Obj head=IC_NIL, tail, nl;
	Backtrace *bt;

	for(bt=backtrace_list; bt; bt=bt->next){

		nl = Fcons(bt->fncname,
			Fcons(bt->fncdefn,
			      Fcons(bt->argl,
				    Fcons( MAKINT(bt->dbg_me), IC_NIL))));
		if( NULLP(head)){
			head = tail = Fcons( nl, IC_NIL );
		}else{
			CDR(tail) = Fcons(nl, IC_NIL);
			tail = CDR(tail);
		}
	}
	return head;
}

#ifndef MINIMAL 
/* for taking apart a function, these should be used only for debugging... */
DEFUN("get-param-list", Fgparaml, Sgparaml, 1,1,1,0,
      "(get-param-list function|macro) return the parameter declaration list\n"
      "[see also: get-body-list]",
      (Obj l))
{

	if(! FUNCTIONP(l) && ! MACROP(l))
		return JLERROR("get-param-list", l, "WTA: function or macro p");

	return CADR(l);
}

DEFUN("get-body-list", Fgbodyl, Sgbodyl, 1,1,1,0,
      "(get-body-l function|macro) return the body\n"
      "[see also: get-param-list]",
      (Obj l))
{

	if(! FUNCTIONP(l) && ! MACROP(l))
		return JLERROR("get-param-list", l, "WTA: function or macro p");

	return CDDR(l);
}

#endif /* MINIMAL */

DEFUN("set-debug-backtrace", Fsdbgbt, Ssdbgbt, 1,1,1,0,
      "(set-debug-backtrace depth) set the debug flag on the function\n"
      "depth levels back on the stack frame",
      (Obj d))
{
	int i, dpth, flg=1;
	Backtrace *bt;
	
	if(! INUMP(d) || CINT(d)==0)
		return JLERROR("set-debug-flag", d, "WTA: intp");

	dpth = CINT(d);
	if( dpth<0){
		dpth = -dpth;
		flg = 0;
	}

	bt = backtrace_list;
	for(i=0; i++!= dpth && bt; bt=bt->next);
	if( bt)
		bt->dbg_me = flg;

	return Fcons(bt->fncname,
		Fcons(bt->fncdefn,
		      Fcons(bt->argl,
			    Fcons( MAKINT(bt->dbg_me), IC_NIL))));
}

DEFUN("debug-on-entry", Fdbgentry, Sdbgentry, 1,1,1,0,
      "(debug-on-entry function) enter the debugger when function is called",
      (Obj f))
{

	if(! FUNCTIONP(f) && ! MACROP(f))
		return JLERROR("debug-on-entry", f, "WTA: function or macro p");

	CAR(f) |= 1<<12;

	return f;
}

DEFUN("cancel-debug-on-entry", Fcncldbg, Scncldbg, 1,1,1,0,
      "(cancel-debug-on-entry function) no longer call debugger function when called",
      (Obj f))
{

	if(! FUNCTIONP(f) && ! MACROP(f))
		return JLERROR("debug-on-entry", f, "WTA: function or macro p");

	CAR(f) &= ~(1<<12);

	return f;
}

void btrace(void){

	Fdisplay( Fbacktrc(), stderr_port );
}

