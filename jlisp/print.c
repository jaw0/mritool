
/*
    Copyright (c) 1994 Jeff Weisberg

    see the file "License"
*/

#ifdef RCSID
static const char *const rcsid
= "@(#)$Id: print.c,v 1.1 1995/11/23 04:30:42 jaw Exp jaw $";
#endif

/* $Id: print.c,v 1.1 1995/11/23 04:30:42 jaw Exp jaw $ */


#include <jlisp.h>
#include <stdio.h>
#include <math.h>

extern Obj_Vtbl jlisp_vtbl[];
void writestr(Obj port, char* s);
void writechar(Obj port, int c);
void printnum(Obj port, int val, int bacse, int len, int zc);

extern Obj sym_oradix, sym_stdout;


/* how:
       0	std. print form (no quotes)
       1	in a form that can be read back
*/


void prnobj(Obj a, Obj stream, int how){
	int typ = TYPEOFX(a);
	int (*printfnc)(Obj,Obj,int);
	int i;
	char *foo;
	Obj radix;
	int base;
	double val;
	
	switch( typ ){
	  case TPVF_IMMED:
		if( INUMP( a )){
			/* int */
			radix = getvalue( sym_oradix);
			if(DEFINEDP(radix)&& INUMP(radix))
				base = CINT(radix);
			else
				base = 10;
			printnum(stream, CINT(a), base, 0,0);
		} else if( ICHARP( a )){
			/* char */
			foo = 0;
			if(how){
				writechar(stream, '?');
				switch( CCHAR(a)){
				  case '\n': foo = "\\n"; break;
				  case '\r': foo = "\\r"; break;
				  case '\b': foo = "\\b"; break;
				  case ' ' : foo = "\\s"; break;
				  case '\t': foo = "\\t"; break;	
				  case '\f': foo = "\\f"; break;
				  case 0x1B: foo = "\\e"; break;
				  default: foo = 0;
				}
			}
			if( foo)
				writestr(stream, foo);
			else
				writechar(stream, CCHAR(a));
		} else if( ICONSTP( a )){
			/* const sym */
			switch( a ){

			  case IC_NIL:
				writestr(stream, "()");
				break;

			  case IC_TRUE:
				writestr(stream, "#t");
				break;

			  case IC_FALSE:
				writestr(stream, "#f");
				break;

			  case IC_UNDEF:
				writestr(stream, "#<undefined>");
				break;

			  case IC_UNSPEC:
				writestr(stream, "#<unspecified>");
				break;

			  case IC_EOF:
				writestr(stream, "#<EOF>");
				break;

			  default:
				writestr(stream, "#<IC_0x");
				printnum(stream, a, 16,0,0);
				writestr(stream, "?>");
				break;
			}
		} else {
			writestr(stream, "#<IMM_0x");
			printnum(stream, a, 16,0,0);
			writestr(stream, "?>");
		}
		break;

	  case TPV_SYMBOL:
		writestr(stream, CCHARS(a));
		break;

	  case TPV_SYM_BOX:
		if(how)
			writestr(stream, CSYM_BOX(a)->name);
		else{
			writestr(stream, "#<[");
			writestr(stream, CSYM_BOX(a)->name);
			writestr(stream, "]>");
		}
		break;

	  default:
		printfnc = jlisp_vtbl[ typ ].print;

		if( !printfnc || ! printfnc(a, stream, how) ){
			writestr(stream, "#<_");
			printnum(stream, typ, 10,0,0);
			writestr(stream, "_0x");
			printnum(stream, a, 16, 0,0);
			writestr(stream, ">");
		}
		break;
	}
}

int prn_func_macr(Obj a, Obj stream, char* which){
	
	writestr(stream, "(");
	writestr(stream, which);
	writestr(stream, " ");
	prnobj( CADR(a), stream, 1);	/* the args */
	writestr(stream, " ");
	
	a = CDDR(a);
	while( NNULLP( a )){
		if( NCONSP( a )){
			writestr(stream, " . ");
			prnobj(a, stream, 1);
			break;
		}
		writestr(stream, " ");
		prnobj( CAR(a), stream, 1);
		a = CDR( a );
	}	
	writestr(stream, ")");
	return 1;
}

int prnfunc(Obj a, Obj stream, int how){

	if( how) return prn_func_macr(a, stream, "lambda");
	else writestr(stream, "#<function>");
	return 1;
}
int prnmacr(Obj a, Obj stream, int how){

	if( how) return prn_func_macr(a, stream, "macro");
	else writestr(stream, "#<macro>");
	return 1;
}

int prnccode(Obj a, Obj stream, int how){

	writestr(stream, "#<builtin-function:");
	writestr(stream, CCDECL(a)->name);
	writestr(stream, ">");
	return 1;
}

int prnstr(Obj a, Obj stream, int how){
	int i;
	
	if(how) writestr(stream, "\"");
	for(i=0; i< CLENGTH(a); i++)
		writechar(stream, CCHARS(a)[i]);
	if(how) writestr(stream, "\"");
	return 1;
}

int prnvect(Obj a, Obj stream, int how){
	int i;
	
	writestr(stream, "#(");
	if( CLENGTH(a)) prnobj( CVECTOR(a)[0], stream, how);
	for(i=1; i< CLENGTH(a); i++){
		writestr(stream, " ");
		prnobj( CVECTOR(a)[i], stream, how);
	}
	writestr(stream, ")");
		
	return 1;
}

int prncmplx(Obj a, Obj stream, int how){
	return 0;
}
	
int prncons(Obj a, Obj stream, int how){
	FILE *fp = CFILEPTR( stream );
	
	writestr(stream, "(");
	prnobj(CAR(a), stream, how);
	a = CDR(a);
	while( NNULLP( a )){
		if( NCONSP( a )){
			writestr(stream, " . ");
			prnobj(a, stream, how);
			break;
		}
		writestr(stream, " ");
		prnobj( CAR(a), stream, how );
		a = CDR( a );
	}
	writestr(stream, ")");
	return 1;
}

DEFUN("display", Fdisplay, Sdisplay, 1, 2, 1,0,
      "(display obj [port]) Display the object",
      (Obj a, Obj stream))
{

	if( NBOUNDP( stream )) stream = getvalue(sym_stdout);

	if( NULLP(stream)) return IC_UNSPEC;
	
	if( ! WPORTP( stream )){
		return JLERROR("display", stream, "WTA: outputportp");
	}

	prnobj(a, stream, 0);
	return IC_UNSPEC;
}

DEFUN("write", Fwrite, Swrite, 1, 2, 1,0,
      "(write obj [port]) Display the object in read form",
      (Obj a, Obj stream))
{

	if( NBOUNDP( stream )) stream = getvalue(sym_stdout);

	if( NULLP(stream)) return IC_UNSPEC;

	if( ! WPORTP( stream )){
		return JLERROR("write", stream, "WTA: outputportp");
	}

	prnobj(a, stream, 1);
	return IC_UNSPEC;
}


void printnum(Obj port, int val, int base, int len, int zc){
	int c;
	int vv=1;
	int foo;
	if(!zc) zc = '0';
	
	if(val<0){
		val = -val;
		writechar(port, '-');
	}
	if(!val && !len){
		writechar(port, '0');
		return;
	}
	if(len) vv = pow(base, len);
	else{
		foo = val;
		while (foo >= base){
			foo /= base;
			vv *= base;
		}
	}
	
	while(vv){

		c = val / vv;
		if(!c)
			writechar(port, zc);
		else{
			if(c>=0 && c<=9) c+= '0';
			else c += 'A' - 0xA;
			writechar(port, c);
			zc = '0';
		}
		val %= vv;
		vv /= base;
	}
}


void prnfldbl(double val, int len, Obj stream, int how){
	float vv, vl;
	int vvl;
	int ip, fp;
	Obj radix;
	int base;

#ifndef INFERIOR_LIBM	
	if(isinf(val)){
		writestr(stream, "Infinity");
		return;
	}
	if(isnan(val)){
		writestr(stream, "NaN");
		return;
	}
#endif	
	if(val==0.0){
		writestr(stream, "0.0");
		return;
	}
	if(val<0){
		val = -val;
		writechar(stream, '-');
	}
	radix = getvalue( sym_oradix);
	if(DEFINEDP(radix)&& INUMP(radix))
		base = CINT(radix);
	else
		base = 10;
	
	if(!how){
		vv = floor(log(val)/log(base));
		if( vv>=-3 && vv<=4 )
			vv = 1;
		else{
			vvl = vv;
			vv = pow(base, vv);
		}
	}else{
		vv = 1;
	}

	/* into: ip.fp */

	vl = pow(base, len);
	val /= vv;
	val *= vl;
	val = rint(val);

	fp = (int)val % (int)vl;
	ip = (val - fp) / vl;

	
	printnum(stream, ip, base, 0, 0);
	writechar(stream, '.');
	printnum(stream, fp, base, len-1, 0);

	if(vv!=1){
		writechar(stream, '$');
		printnum(stream, vvl, base, 0,0);
	}
}
	
int prnflt(Obj a, Obj stream, int how){
	prnfldbl( CFLOAT(a), 4, stream, how);
	return 1;
}

int prndbl(Obj a, Obj stream, int how){
	prnfldbl( CDOUBLE(a), 8, stream, how);
	return 1;
}

int prnenvec(Obj a, Obj stream, int how){

	writestr(stream, "#<ENV_0x");
	printnum(stream, a, 16, 0,0);
	writestr(stream, ">");
	return 1;
}

