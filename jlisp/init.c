
/*
    Copyright (c) 1994 Jeff Weisberg

    see the file "License"
*/

#ifdef RCSID
static const char *const rcsid
= "@(#)$Id: init.c,v 1.1 1995/11/23 04:30:41 jaw Exp jaw $";
#endif

#include <jlisp.h>
#include <stdio.h>

/* so we can use these without needed to look them up */
Obj stdin_port, stdout_port, stderr_port;
Obj sym_stdout, sym_stdin;
Obj sym_optional, sym_rest;
Obj sym_quote, sym_bquote, sym_bq_comma, sym_bq_comma_at;
Obj sym_iradix, sym_oradix;
Obj sym_docstring;
Obj sym_eof;
Obj sym_eval_function, sym_eval_macro;


Obj *stackbase;
void init_init(void), init_jlisp(int, char**);
EXTERN_C Obj Fread(Obj);
EXTERN_C Obj Fload(Obj);

extern void pushenv(void);
extern Obj envcurr, envlist, internal_gc_protect;
extern Obj lineno, freehint;
extern unsigned  etext, edata, end;

int initialized = 0;
unsigned int Brk = 0;

#undef DEFUN
#define DEFUN(ln, cn, sn, min, max, ep, lp, doc, pr)	\
	extern Defun_Decl sn;
#include <defun.list>
#undef DEFUN

#undef DEFVAR
#define DEFVAR(ln, cn, doc, val)			\
	extern Obj cn;
#include <defvar.list>		
#undef DEFVAR	

void init_csyms( Defun_Decl *decl ){
	
	Obj csym, cbox;
	Obj ccode = newcell();
	int sigs;
	
	DISABLE( sigs );
	CAR( ccode ) = MAKETYPE( TPV_C_CODE ) | SDBIT;
	CDR( ccode ) = (Obj)decl;
	RENABLE( sigs );
	
	csym = maksym_c( decl->name );
	Fdefine( csym,
		decl->doc ? makstr_c(decl->doc) : IC_UNSPEC,
		ccode);
}

#ifndef NOT_STAND_ALONE
int main(int argc, char**argv){
	char *altinitfile = 0;

	/* check for -I initfile */
	if( argc>1 && argv[1][0] == '-' && argv[1][1] == 'I'){
		if( argv[1][2])
			altinitfile = &(argv[1][2]);
		else if( argc>2)
			altinitfile = argv[2];
	}
	
	init_jlisp(argc, argv);

	if(! initialized){
#ifndef NO_FILES		
		fprintf(stderr, "Loading...");
		
		if(   (!altinitfile || Fload(makstr_c( altinitfile )) == IC_FALSE)
		   && (Fload(makstr_c( QUOTIFY( JLISP_INIT ) )) == IC_FALSE)
		   && (Fload(makstr_c("./lisp/init.jl" )) == IC_FALSE)
		   && (Fload(makstr_c("./init.jl" )) == IC_FALSE) ){
			fprintf(stderr, "jlisp aborting, could not load init file: \"%s\"\n",
				QUOTIFY( JLISP_INIT ));
			Fquit(MAKINT(-1));
		}
		fprintf(stderr, "Done.\n");
#else
		/* do what ? */
#endif		
	}

	/* initialize signal handlers */
#ifndef NO_SIGNALS	
	Fcall(maksym_c("%initialize-signal-handlers%"));
#endif	
	/* do command line args */
	Feval(getvalue(maksym_c("%parse-cmd-line%")));
	/* enter repl */
	Feval(getvalue(maksym_c("%repl%")));
	Fquit(MAKINT(0));

	/* not reached, but it makes the compiler happier */
	return 1;	

}
#endif

void init_jlisp(int argc, char**argv){
	Obj box;
	int i;
	
	stackbase = (Obj*) &argc;

#ifndef NOT_UNIX	
	if( Brk) brk( Brk);	/* make sure break is right after dump */

	tzset();		/* in case it was cached */
#endif	
	
	if( !initialized){
		pushenv();
		init_init();
	}

	/* inter the command line args */
        box = makvect( argc );
        for(i=0; i<argc; i++){
                CVECTOR(box)[i] = makstr( argv[i] );
        }
	Fdefine( maksym_c("*argv*"),
		box,
		makstr_c( "*argv* vector of command line arguments"));
	
}

void init_init(void){
	Obj box, foo;
	int i;
	
	sym_docstring = maksym_c(".docstring");
	Fdefine( sym_docstring, sym_docstring, IC_UNSPEC);

#ifndef NO_FILES
	/* setup stdio ports */
	stdin_port =  makport( stdin, READABLE);
	sym_stdin = maksym_c("*stdin_port*");
	Fdefine( sym_stdin,
		makstr_c("standard input port"),
		stdin_port);
	
	stdout_port = makport( stdout, WRITABLE);
	sym_stdout = maksym_c("*stdout_port*");
	Fdefine( sym_stdout,
		makstr_c("standard output port"),
		stdout_port);
	
	stderr_port = makport( stderr, WRITABLE);
	Fdefine( maksym_c("*stderr_port*"),
		makstr_c("standard error port"),
		stderr_port);
#else
	stdin_port =  IC_NIL;
	sym_stdin = maksym_c("*stdin_port*");
	Fdefine( sym_stdin,
		makstr_c("standard input port"),
		stdin_port);
	
	stdout_port = IC_NIL;
	sym_stdout = maksym_c("*stdout_port*");
	Fdefine( sym_stdout,
		makstr_c("standard output port"),
		stdout_port);
	
	stderr_port = IC_NIL;
	Fdefine( maksym_c("*stderr_port*"),
		makstr_c("standard error port"),
		stderr_port);
#endif	
	/* and some other useful symbols */
	sym_rest = maksym_c("&rest");
	Fdefine( sym_rest,
		makstr_c("&rest signal the rest of the arguments"),
		sym_rest);

	sym_optional = maksym_c("&optional");
	Fdefine( sym_optional,
		makstr_c("&optional signal optional arguments"),
		sym_optional);

	sym_iradix = maksym_c("%input-radix%");
	Fdefine( sym_iradix,
		MAKINT(10),
		makstr_c( "%input-radix% the radix used for numbers on input"));

	sym_oradix = maksym_c("%output-radix%");
	Fdefine( sym_oradix,
		MAKINT(10),
		makstr_c( "%output-radix% the radix used for numbers on output"));

#undef DEFUN
#define DEFUN(ln, cn, sn, min, max, ep, lp, doc, pr)	\
		init_csyms( &sn );
#include <defun.list>

#undef DEFVAR

#ifndef NO_DOCSTRINGS       
#define DEFVAR(ln, cn, doc, val)			\
	foo = maksym_c(ln);				\
	Fdefine(foo, makstr_c(doc), IC_UNDEF);		\
	cn = Fenvlookup(foo, IC_UNSPEC);	    	\
	CAR( cn ) |= SDBIT;				\
	VALUE( cn ) = val;
#else
#define DEFVAR(ln, cn, doc, val)			\
	foo = maksym_c(ln);				\
	Fdefine(foo, IC_UNSPEC, IC_UNDEF);		\
	cn = Fenvlookup(foo, IC_UNSPEC);	    	\
	CAR( cn ) |= SDBIT;				\
	VALUE( cn ) = val;
#endif	
	
#include <defvar.list>		
		

	sym_eof = maksym_c("eof");
	sym_eval_function = Fenvlookup(maksym_c("#<:internal:eval-function>"), IC_UNSPEC);
	sym_eval_macro = Fenvlookup(maksym_c("#<:internal:eval-macro>"), IC_UNSPEC);
	sym_quote = Fenvlookup( maksym_c("quote"), IC_UNSPEC);
	sym_bquote = Fenvlookup( maksym_c("backquote"), IC_UNSPEC);
	sym_bq_comma = Fenvlookup( maksym_c("unquote"), IC_UNSPEC);
	sym_bq_comma_at = Fenvlookup( maksym_c("unquote-splice"), IC_UNSPEC);

	PROTECT_SYM( sym_iradix );
	PROTECT_SYM( sym_oradix );
	PROTECT_SYM( sym_eof );
	PROTECT_SYM( sym_stdout );
	PROTECT_SYM( sym_stdin );

	box = IC_NIL;
	box = Fcons( maksym_c("eval"), box);
	box = Fcons( maksym_c("macros"), box);
	box = Fcons( maksym_c("floats"), box);
#ifndef NO_BIGNUMS	
	box = Fcons( maksym_c("bignum"), box);
#endif
#ifndef NO_STRUCTS
	box = Fcons( maksym_c("struct"), box);
#endif
	box = Fcons( maksym_c("ports"), box);
	box = Fcons( maksym_c("string-ports"), box);
#ifndef NO_SOCKETS	
	box = Fcons( maksym_c("sockets"), box);
#endif
#ifndef NO_SIGNALS	
	box = Fcons( maksym_c("signals"), box);
#endif
#ifndef NO_REGEXPS	
	box = Fcons( maksym_c("regexps"), box);
#endif
#ifndef NO_UNEXEC	
	box = Fcons( maksym_c("unexec"), box);
#endif	
#ifndef NO_WEAKS
	box = Fcons( maksym_c("weaks"), box);
#endif
#ifndef NO_UNIX	
	box = Fcons( maksym_c("unix"), box);
#endif
#ifndef NO_DOCSTRINGS
	box = Fcons( maksym_c("docstrings"), box);
#endif
#ifndef INFERIOR_UNIX
	box = Fcons( maksym_c("real-unix"), box);
#endif
#ifndef INFERIOR_LIBM
	box = Fcons( maksym_c("real-math"), box);
#endif
	
#ifdef MINIMAL
	/* not really a feature, but... */
	box = Fcons( maksym_c("minimal"), box);
#else
#	ifdef EMBEDDED
		box = Fcons( maksym_c("embedded"), box);
#	else
		box = Fcons( maksym_c("kitchen-sink"), box);
#	endif
#endif
	
	box = Fcons(sym_quote, Fcons(box, IC_NIL));
	foo = Fdefine( maksym_c("*features*"), box,
		makstr_c("*features* list of features supported"));
	
}

void Fxx_dummy_xx(Obj a){

	Fprocp(a);
}


	
	

	
	
