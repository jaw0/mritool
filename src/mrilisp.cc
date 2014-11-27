 
// Copyright (c) 1994 Jeff Weisberg
// University of Rochester
//      Dept. of Electrical Engineering
//      Dept. of Neurology, C.E.P.
//
// see the file "License"
 
#ifdef RCSID
static const char *const rcsid
= "@(#)$Id: mrilisp.cc,v 1.1 1995/11/23 04:30:19 jaw Exp jaw $";
#endif

// Optimization: Full

#include <jlisp.h>
#include <jlconf.h>
#include <jl_proto.h>
#include <defproto.h>	

#include <mritool.h>
#include <solid.h>
#include <mrilisp.h>

#define CODE_SECTION	SECTION_LISP

extern Obj *stackbase;
extern Obj_Vtbl jlisp_vtbl[];
extern int initialized;
extern unsigned  Brk;
extern "C" void init_jlisp(int, char**);
 
UserI *ui_jl = 0;

extern "C" Obj eqcdr(Obj, Obj);
extern "C" int freeimage(Obj), freesolid(Obj), freexv(Obj);
extern "C" int prnstrct(Obj, Obj, int);

#undef DEFUN
#define DEFUN(ln, cn, sn, min, max, ep, lp, doc, pr)        \
                extern Defun_Decl sn;
#include "cli-jl/defun.list"

#undef DEFVAR
#define DEFVAR(ln, cn, doc, val)                        \
        extern Obj cn;
#include "cli-jl/defvar.list"

void init_mri_lisp(int argc, char**argv){
	Obj foo;
	char *altinitfile = 0;
	
	/* check for -I initfile */
        if( argc>1 && argv[1][0] == '-' && argv[1][1] == 'I'){
                if( argv[1][2])
                        altinitfile = &(argv[1][2]);
                else if( argc>2)
                        altinitfile = argv[2];
        }

	if(! initialized)
		cerr << "Initializing JLisp sub-system...";

	init_jlisp(argc, argv);
	
	// insert vtbl entries
	jlisp_vtbl[ TPV_IMAGE ].free = (int (*)(Obj))freeimage;
	jlisp_vtbl[ TPV_SOLID ].free = (int (*)(Obj))freesolid;
	jlisp_vtbl[ TPV_XVOPAQUE ].free = (int (*)(Obj))freexv;
	jlisp_vtbl[ TPV_IMAGE ].equal = (Obj (*)(Obj,Obj))eqcdr;
	jlisp_vtbl[ TPV_SOLID ].equal = (Obj (*)(Obj,Obj))eqcdr;
	jlisp_vtbl[ TPV_XVOPAQUE ].equal =(Obj (*)(Obj,Obj)) eqcdr;
	jlisp_vtbl[ TPV_UI    ].equal = (Obj (*)(Obj,Obj))eqcdr;
	jlisp_vtbl[ TPV_IMAGE ].name = "img";
	jlisp_vtbl[ TPV_SOLID ].name = "solid";
	jlisp_vtbl[ TPV_XVOPAQUE ].name = "xv";
	jlisp_vtbl[ TPV_UI    ].name = "ui";
	jlisp_vtbl[ TPV_IMAGE ].print = prnstrct;
	
	if(! initialized){

#undef DEFUN
#define DEFUN(ln, cn, sn, min, max, ep, lp, doc, pr)        \
		init_csyms( &sn );
#include "cli-jl/defun.list"

#undef DEFVAR
#define DEFVAR(ln, cn, doc, val)                        	\
		foo = maksym_c(ln);				\
        	Fdefine(foo, makstr(doc), IC_UNDEF);	     	\
        	cn = Fenvlookup(foo, IC_UNSPEC);	     	\
        	CAR( cn ) |= SDBIT;                             \
        	VALUE( cn ) = val;
#include "cli-jl/defvar.list"

		Fdefine(maksym_c("mritool"), IC_TRUE, IC_UNSPEC);
		
		if(   (!altinitfile || Fload(makstr_c( altinitfile )) == IC_FALSE)
		   && (Fload(makstr_c( QUOTIFY( JLISP_INIT ) )) == IC_FALSE)
		   && (Fload(makstr_c("./lisp/init.jl" )) == IC_FALSE)
                   && (Fload(makstr_c("./init.jl" )) == IC_FALSE) ){
                        fprintf(stderr, "mritool aborting, could not load init file: \"%s\"\n",
                                QUOTIFY( JLISP_INIT ));
                        Fquit(MAKINT(-1));
                }

		cerr << "Done" << endl;
	}
	/* do command line args */
	Fset(maksym_c("*current-file*"), makstr_c("mritool internals"));
	Feval(getvalue(maksym_c("%parse-cmd-line%")));
}

void interpret_lisp(UserI* self, char *text){
	UserI *foo = ui_jl;
	Obj qux;
	
	if( self) UI = ui_jl = self;
	
	debugp( DBG_ECHO )
		cerr << text << endl;

	// (eval (mri:interpret-string TEXT))
	qux = Feval( Fcons(maksym_c("mri:interpret-string"),
		Fcons(makstr(text), IC_NIL)));
	
	fflush(stdout);

	debugp( DBG_RESULT )
		Fdisplay( qux, stderr_port );

	if( foo) UI = ui_jl = foo;
}

Obj makxvopaque(Xv_opaque it){
	Obj foo = newcell();
	int sigs;

	DISABLE( sigs );
        CAR(foo) = MAKETYPE( TPV_XVOPAQUE );
	CDR(foo) = it;
	RENABLE( sigs );

	return foo;
}

Obj makxvn(Xv_opaque it){
	Obj foo = newcell();
	int sigs;

	DISABLE( sigs );
        CAR(foo) = MAKETYPE( TPV_XVOPAQUE ) | (1<<12);
	CDR(foo) = it;
	RENABLE( sigs );

	return foo;
}

Obj makimage(Bimage& im){
	Obj foo = newcell();
	int sigs;

	DISABLE( sigs );
        CAR(foo) = MAKETYPE( TPV_IMAGE );
	CDR(foo) = (Obj)&im;
	RENABLE( sigs );

	return foo;
}

Obj maksolid(Solid& sl){
	Obj foo = newcell();
	int sigs;
		
	DISABLE( sigs );
        CAR(foo) = MAKETYPE( TPV_SOLID );
	CDR(foo) = (Obj)&sl;
	RENABLE( sigs );

	return foo;
}

Obj mak_ui(UserI* ui){
	Obj foo = newcell();
	int sigs;

	DISABLE( sigs );
        CAR(foo) = MAKETYPE( TPV_UI );
	CDR(foo) = (Obj)ui;
	RENABLE( sigs );

	return foo;
}

extern "C" int freexv(Obj x){
	
	if( CAR(x)>>12 )
		return 0;
	xv_destroy( CXVOPAQUE(x));
	return 1;
}
	

extern "C" int freeimage(Obj im){

	delete CIMAGE(im);
	return 1;
}

extern "C" int freesolid(Obj sl){
	
	delete CSOLID(sl);
	return 1;
}

	

	
	
	
