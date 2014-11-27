
/*
    Copyright (c) 1994 Jeff Weisberg

    see the file "License"
*/

#ifdef RCSID
static const char *const rcsid
= "@(#)$Id: error.c,v 1.1 1995/11/23 04:30:45 jaw Exp jaw $";
#endif

#include <jlisp.h>
#include <stdio.h>

extern Obj envcurr;

EXTERN_C
Obj jlerror(char* fnc, Obj a, char* huh){
	Obj errsym;
	Obj hndlr, ehs=maksym_c("error-handler");

	/* 1st look for a user defined jlerror handler */
	hndlr = getvalue(ehs);
	if( Fprocp( hndlr )!= IC_FALSE){
		/* hand it our args */
		funcall_3("error", ehs, makstr(fnc), a, makstr(huh));
		/* ought not return ... */
	}

	/* next try throwing an 'error */
	Fthrow(maksym_c("error"), IC_TRUE );

	/* try throwing back to the repl */
	Fthrow(maksym_c("repl"), IC_TRUE);

	/* hopefully by this point something has been taken
	   as if we return, we will likely segv... */
	
	/* lastly, just return */
	return IC_UNSPEC;
}
