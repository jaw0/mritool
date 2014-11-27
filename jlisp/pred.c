
/*
    Copyright (c) 1994 Jeff Weisberg

    see the file "License"
*/

#ifdef RCSID
static const char *const rcsid
= "@(#)$Id: pred.c,v 1.1 1995/11/23 04:30:43 jaw Exp jaw $";
#endif

/* $Id: pred.c,v 1.1 1995/11/23 04:30:43 jaw Exp jaw $ */


#include <jlisp.h>

DEFUN("consp", Fconsp, Sconsp, 1, 1, 1,0,  "Is this a cons cell",
      (Obj a))
{

	return CONSP( a ) ? IC_TRUE : IC_FALSE;
}

DEFUN("intp", Fintp, Sintp, 1,1, 1,0, "An integer?",
      (Obj a))
{
	return INUMP(a) ? IC_TRUE : IC_FALSE;
}

DEFUN("charp", Fcharp, Scharp, 1,1,1,0, "A charcacter?",
      (Obj a))
{
	return ICHARP(a) ? IC_TRUE : IC_FALSE;
}

DEFUN("floatp", Ffloatp, Sfloatp, 1,1,1,0, "A float?",
      (Obj a))
{
	return FLOATP(a) ? IC_TRUE : IC_FALSE;
}

DEFUN("doublep", Fdoublep, Sdoublep, 1,1,1,0, "A double?",
      (Obj a))
{
	return DOUBLEP(a) ? IC_TRUE : IC_FALSE;
}

DEFUN("bignump", Fbignp, Sbignp, 1,1,1,0, "A bignum?",
      (Obj a))
{
	return BIGNUMP(a) ? IC_TRUE : IC_FALSE;
}

DEFUN("stringp", Fstringp, Sstringp, 1,1,1,0, "A string?",
      (Obj a))
{
	return STRINGP(a) ? IC_TRUE : IC_FALSE;
}

DEFUN("vectorp", Fvectorp, Svectorp, 1,1,1,0, "A vector?",
      (Obj a))
{
	return VECTORP(a) ? IC_TRUE : IC_FALSE;
}

DEFUN("structp", Fstructp, Sstructp, 1,1,1,0, "A struct?",
      (Obj a))
{
	return STRUCTP(a) ? IC_TRUE : IC_FALSE;
}

DEFUN("atomp", Fatomp, Satomp, 1,1,1,0, "An atom?",
      (Obj a))
{
	return NCONSP(a) ? IC_TRUE : IC_FALSE;
}

DEFUN("nullp", Fnullp, Snullp, 1,1,1,0, "()?",
      (Obj a))
{
	return NULLP(a) ? IC_TRUE : IC_FALSE;
}

DEFUN("falsep", Ffalsep, Sfalsep, 1,1,1,0, "false?",
      (Obj a))
{
	return FALSEP(a) ? IC_TRUE : IC_FALSE;
}

DEFUN("symbolp", Fsymbolp, Ssymbolp, 1,1,1,0, "A symbol?",
      (Obj a))
{
	return SYMBOLP(a)||SYMBOXP(a) ? IC_TRUE : IC_FALSE;
}

DEFUN("boundp", Fboundp, Sboundp, 1,1,1,0, "a bound symbol?",
      (Obj a))
{

	if( SYMBOLP(a) )
		a = Fenvlookup(a, IC_UNSPEC);
	if( SYMBOXP(a) && BOUNDP( a ))
		return IC_TRUE;
	return a==IC_UNSPEC ? IC_FALSE : IC_TRUE;
}

DEFUN("definedp", Fdefinedp,Sdefinedp, 1,1,1,0, "defined?",
      (Obj a))
{

	if( SYMBOLP(a) )
		a = Fenvlookup(a, IC_UNSPEC);
	if( SYMBOXP(a) && DEFINEDP( a ))
		return IC_TRUE;
	return a==IC_UNDEF ? IC_FALSE : IC_TRUE;
}

DEFUN("zerop", Fzerop, Szerop, 1,1,1,0, "zero?",
      (Obj a))
{
	int i,l;
	
	if( INUMP(a)) return CINT(a)==0 ? IC_TRUE : IC_FALSE;
	if( FLOATP(a))return *(float*)&CDR(a)==0.0 ? IC_TRUE : IC_FALSE;

	if( BIGNUMP(a)){
		l = CLENGTH(a);
		for(i=0; i<l; i++){
			if( CBIGNUM(a)[i] ) return IC_FALSE;
		}
		return IC_TRUE;
	}
	
	return IC_FALSE;
}

DEFUN("inputportp",Finputportp, Sinputportp, 1,1,1,0, "A readable ioport?",
      (Obj a))
{
	return RPORTP(a) ? IC_TRUE : IC_FALSE;
}

DEFUN("outputportp",Foutputportp,Soutputportp,1,1,1,0, "A writable ioport?",
      (Obj a))
{
	return WPORTP(a) ? IC_TRUE : IC_FALSE;
}

DEFUN("procedurep", Fprocp, Sprocp, 1,1,1,0, "A procedure?",
      (Obj a))
{
	return (CCODEP(a) || FUNCTIONP(a) || MACROP(a)) ? IC_TRUE : IC_FALSE;
}

DEFUN("ccodep", Fccodep, Sccodep, 1,1,1,0, "Builtin C code?",
      (Obj a))
{
	return CCODEP(a) ? IC_TRUE : IC_FALSE;
}

DEFUN("functionp", Fcosp, Sclosp, 1,1,1,0, "A function?",
      (Obj a))
{
	return FUNCTIONP(a) ? IC_TRUE : IC_FALSE;
}
DEFUN("macrop", Fmacrop, Smacrop, 1,1,1,0, "A macro?",
      (Obj a))
{
	return MACROP(a) ? IC_TRUE : IC_FALSE;
}
