
/*
    Copyright (c) 1994 Jeff Weisberg

    see the file "License"
*/

#ifdef RCSID
static const char *const rcsid
= "@(#)$Id: math.c,v 1.1 1995/11/23 04:30:43 jaw Exp jaw $";
#endif


#include <jlisp.h>
#include <math.h>

extern Obj makfloat(float);
extern Obj makdbl(double);
extern Obj big_plus_minus(Obj, Obj, int), big_multiply(Obj, Obj), big_divide(Obj, Obj, int);
extern Obj big_logical(Obj, Obj, int, char*), big_shl(Obj, Obj, char*), big_shr(Obj, Obj, char*);
extern big_log(Obj, Obj);
extern Obj cdouble_to_big(double);
extern IntType big_to_cint(Obj);
extern int big_compare(Obj, Obj);
extern double big_to_cdouble(Obj);
Obj cint_to_big(IntType);
Obj Fto_bignum(Obj);


double dblof(Obj a){

	if(ICHARP(a))  return CCHAR(a);
	if(INUMP(a))   return CINT(a);
	if(FLOATP(a))  return CFLOAT(a);
	if(DOUBLEP(a)) return CDOUBLE(a);
#ifndef NO_BIGNUMS	
	if(BIGNUMP(a)) return big_to_cdouble(a);
#endif
	return 0.0;
}
IntType intof(Obj a){

	if(ICHARP(a))  return CCHAR(a);
	if(INUMP(a))   return CINT(a);
	if(FLOATP(a))  return CFLOAT(a);
	if(DOUBLEP(a)) return CDOUBLE(a);
#ifndef NO_BIGNUMS
	if(BIGNUMP(a)) return big_to_cint(a);
#endif
	return 0;
}
float fltof(Obj a){

	if(ICHARP(a))  return CCHAR(a);
	if(INUMP(a))   return CINT(a);
	if(FLOATP(a))  return CFLOAT(a);
	if(DOUBLEP(a)) return CDOUBLE(a);
#ifndef NO_BIGNUMS
	if(BIGNUMP(a)) return big_to_cdouble(a);
#endif
	return 0;
}

Obj simplemath(Obj a, Obj b, int op, char *name){
	int ta = TYPEOFX(a), tb = TYPEOFX(b);
	int mt;
	IntType tv, th, tl;
	float tf;
	double td;
	Obj bn, foo, bar;
	mt = MAX(ta, tb);

	/* this depends on TYPEOFX (int) < (bignum) < (float) < (double)
	   [see jlisp.h]

	   and is contaguous: int -> bignum -> float -> double
	*/
	
	switch( mt ){
	  case TPVF_IMMED:
		switch (op ){
		  case '=':
			return intof(a) == intof(b) ? IC_TRUE : IC_FALSE;
		  case '<':
			return intof(a) < intof(b) ? IC_TRUE : IC_FALSE;
		  case '>':
			return intof(a) > intof(b) ? IC_TRUE : IC_FALSE;
		  case 'g':
			return intof(a) >= intof(b) ? IC_TRUE : IC_FALSE;
		  case 'l':
			return intof(a) <= intof(b) ? IC_TRUE : IC_FALSE;
		  case '+':
			tv = intof(a) + intof(b);
			break;
		  case '-':
			tv = intof(a) - intof(b);
			break;
		  case '*':
			tv = (th=intof(a)) * (tl=intof(b));
#if 0
			/* ov? convert -> bignum  (a guess) */
			if( ((th>>16) * (tl>>16)) & 0xFFFF0000U )
				return big_multiply( cint_to_big(th), cint_to_big(tl));
#endif			
			break;
		  case '%':
			if( (tv=intof(b)) == 0)
				goto divide_by_zero;
			tv = intof(a) % tv;
			break;
		  case '/':
			if( (tv=intof(b)) == 0)
				goto divide_by_zero;
			tv = intof(a) / tv;
			break;
		}
#if 0
		/* check to see if we ought convert ->bignum */
		if( tv & 0xE0000000U){
			return cint_to_bignum( tv );
		}
#endif
		return MAKINT(tv);

	  case TPV_FLOAT:
		switch(op){
		  case '=':
			return fltof(a) == fltof(b) ? IC_TRUE : IC_FALSE;
		  case '<':
			return fltof(a) < fltof(b) ? IC_TRUE : IC_FALSE;
		  case '>':
			return fltof(a) > fltof(b) ? IC_TRUE : IC_FALSE;
		  case 'g':
			return fltof(a) >= fltof(b) ? IC_TRUE : IC_FALSE;
		  case 'l':
			return fltof(a) <= fltof(b) ? IC_TRUE : IC_FALSE;
		  case '+':
			tf = fltof(a) + fltof(b);
			break;
		  case '-':
			tf = fltof(a) - fltof(b);
			break;
		  case '*':
			tf = fltof(a) * fltof(b);
			break;
		  case '%':
			if( (tf=fltof(b)) == 0.0)
				goto divide_by_zero;
#ifndef NO_BIGNUMS			
			return big_divide( Fto_bignum(a), Fto_bignum(b), op);
#else
			tf = intof(a) % intof(b);
#endif			
			break;
		  case '/':
			if( (tf=fltof(b)) == 0.0)
				goto divide_by_zero;
			tf = fltof(a) / tf;
			break;
		}
		return makfloat( tf );
		
	  case TPV_DOUBLE:
		switch (op){
		  case '=':
			return dblof(a) == dblof(b) ? IC_TRUE : IC_FALSE;
		  case '<':
			return dblof(a) < dblof(b) ? IC_TRUE : IC_FALSE;
		  case '>':
			return dblof(a) > dblof(b) ? IC_TRUE : IC_FALSE;
		  case 'g':
			return dblof(a) >= dblof(b) ? IC_TRUE : IC_FALSE;
		  case 'l':
			return dblof(a) <= dblof(b) ? IC_TRUE : IC_FALSE;
		  case '+':
			td = dblof(a) + dblof(b);
			break;
		  case '-':
			td = dblof(a) - dblof(b);
			break;
		  case '*':
			td = dblof(a) * dblof(b);
			break;
		  case '%':
			if( (td=dblof(b)) == 0.0)
				goto divide_by_zero;
#ifndef NO_BIGNUMS			
			return big_divide( Fto_bignum(a), Fto_bignum(b), op);
#else
			tf = intof(a) % intof(b);
#endif	
			break;
		  case '/':
			if( (td=dblof(b)) == 0.0)
				goto divide_by_zero;
			td = dblof(a) / td;
			break;
		}
		return makdbl( td );

#ifndef NO_BIGNUMS		
	  case TPV_BIGNUM:
		switch (op){
		  case '=':
		  case '<':
		  case '>':
		  case 'g':
		  case 'l':
			if( ta == tb)
				tv = big_compare(a, b);
			else if( ta < tb)
				tv = big_compare(cint_to_big(CINT(a)), b);
			else
				tv = big_compare(a, cint_to_big(CINT(b)));
			switch( op ){
			  case '=':
				return tv==0 ? IC_TRUE : IC_FALSE;
			  case '>':
				return tv==1 ? IC_TRUE : IC_FALSE;
			  case '<':
				return tv==-1? IC_TRUE : IC_FALSE;
			  case 'g':
				return tv!=-1? IC_TRUE : IC_FALSE;
			  case 'l':
				return tv!=1 ? IC_TRUE : IC_FALSE;
			}
			
		  case '+':
			if( ta == tb)
				bn = big_plus_minus(a, b, 0);
			else if( ta < tb)
				bn = big_plus_minus(cint_to_big(CINT(a)), b, 0);
			else
				bn = big_plus_minus(a, cint_to_big(CINT(b)), 0);
			break;
		  case '-':
			if( ta == tb)
				bn = big_plus_minus(a, b, 1);
			else if( ta < tb)
				bn = big_plus_minus(cint_to_big(CINT(a)), b, 1);
			else
				bn = big_plus_minus(a, cint_to_big(CINT(b)), 1);
			break;
		  case '*':
			if( ta == tb)
				bn = big_multiply(a, b);
			else if( ta < tb)
				bn = big_multiply(cint_to_big(CINT(a)), b);	
			else
				bn = big_multiply(a, cint_to_big(CINT(b)));
			break;
		  case '/':
		  case '%':
			if( ta == tb ){
				bn = big_divide(a, b, op);
			}else if( ta < tb)
				bn = big_divide(cint_to_big(CINT(a)), b, op);	
			else
				bn = big_divide(a, cint_to_big(CINT(b)), op);
			break;
		}
		return bn;
#endif
	  case TPV_COMPLEX:

	  default:
		return JLERROR(name, Fcons(a, b), "WTA: numberp");
	}
	
  divide_by_zero:
	return JLERROR(name, b, "divide by zero");
}

DEFUN("+", Fplus, Splus, 2,2,1,0, "(+ n m) addition",
      (Obj a, Obj b))
{
	return simplemath(a,b, '+', Splus.name);
}

DEFUN("-", Fminus, Sminus, 2,2,1,0, "(- n m) subtraction",
      (Obj a, Obj b))
{
	return simplemath(a,b, '-', Sminus.name);
}

DEFUN("*", Ftimes, Stimes, 2,2,1,0, "(* n m) multiplication",
      (Obj a, Obj b))
{
	return simplemath(a,b, '*', Stimes.name);
}

DEFUN("/", Fdivide, Sdivide, 2,2,1,0, "(/ n m) division",
      (Obj a, Obj b))
{
	return simplemath(a,b, '/', Sdivide.name);
}

DEFUN("%", Fmod, Smod, 2,2,1,0,
      "(% n1 n2) n1 modulo n2",
      (Obj a, Obj b))
{
	return simplemath(a, b, '%', Smod.name);
}

DEFUN("=", Fmequal, Smequal, 2,2,1,0,
      "(= n1 n2) Are two numbers equal?",
      (Obj a, Obj b))
{
	return simplemath(a, b, '=', Smequal.name);
}
DEFUN(">", Fgreater, Sgreater, 2,2,1,0,
      "(> n1 n2) Is n1 greater than n2?",
      (Obj a, Obj b))
{
	return simplemath(a, b, '>', Sgreater.name);
}
DEFUN("<", Fless, Sless, 2,2,1,0,
      "(< n1, n2) Is n1 less than n2?",
      (Obj a, Obj b))
{
	return simplemath(a, b, '<', Sless.name);
}
DEFUN(">=", Fgrequal, Sgrequal, 2,2,1,0,
      "(>= n1 n2) Is n1 greater or equal to n2?",
      (Obj a, Obj b))
{
	return simplemath(a,b,'g',Sgrequal.name);
}
DEFUN("<=", Flsequal, Slsequal, 2,2,1,0,
      "(<= n1 n2) Is n1 less or equal than n2?",
      (Obj a, Obj b))
{
	return simplemath(a,b,'l',Slsequal.name);
}

Obj logicalmath(Obj a, Obj b, int op, char *name){
	/* bignum contagious */
	int ta = MIN( TYPEOFX(a), TPV_BIGNUM),
		tb = MIN( TYPEOFX(b), TPV_BIGNUM);
	int mt = MAX( ta, tb);
	Obj foo, bar;
	
	switch ( mt ){
	  case TPVF_IMMED:
		switch ( op ){
		  case '<':
			return MAKINT( intof(a) << intof(b) );
		  case '>':
			return MAKINT( intof(a) >> intof(b) );
		  case '&':
			return MAKINT( intof(a) & intof(b) );
		  case '|':
			return MAKINT( intof(a) | intof(b) );
		  case '^':
			return MAKINT( intof(a) ^ intof(b) );
		}
#ifndef NO_BIGNUMS		
	  case TPV_BIGNUM:	
		if(! BIGNUMP(a)) foo = Fto_bignum(a);
		else foo = a;

		if(! BIGNUMP(b)) bar = Fto_bignum(b);
		else bar = b;

		switch ( op ){
		  case '<':
			return big_shl(foo, bar, name);
		  case '>':
			return big_shr(foo, bar, name);
		  default:
			return big_logical(foo, bar, op, name);
		}
#endif		
	}
}

DEFUN("<<", Fshl, Sshl, 2,2,1,0, "(<< n m) shift left)",
      (Obj a, Obj b))
{
	return logicalmath(a, b, '<', Sshl.name);
}

DEFUN(">>", Fshr, Sshr, 2,2,1,0, "(>> n m) shift right)",
      (Obj a, Obj b))
{
	return logicalmath(a, b, '>', Sshr.name);
}

DEFUN("&", Fland, Sland, 2,2,1,0, "(& n m) bitwise and",
      (Obj a, Obj b))
{
	return logicalmath(a, b, '&', Sland.name);
}

DEFUN("|", Flor, Slor, 2,2,1,0, "(| n m) bitwise or",
      (Obj a, Obj b))
{
	return logicalmath(a, b, '|', Slor.name);
}

DEFUN("^", Flxor, Slxor, 2,2,1,0, "(^ n m) bitwise xor",
      (Obj a, Obj b))
{
	return logicalmath(a, b, '^', Slxor.name);
}

DEFUN("~", Flnot, Slnot, 1,1,1,0, "(~ n) bitwise not",
      (Obj a))
{
	if(! BIGNUMP(a))
		return MAKINT( ~ intof(a));
	return logicalmath(a, MAKINT(-1), '^', Slnot.name);
}

#if 0
DEFUN("real", Freal, Sreal, 1,1,1,0, "(real c) return the real part of a complex number",
	(Obj a))
{

	if(COMPLEXP(a)) return makdbl( CCOMPLEX(a).re );
	if(INUMP(a) || FLOATP(a) || DOUBLEP(a)) return a;

	return JLERROR(Sreal.name, a, "WTA: numberp");
}

DEFUN("imag", Fimag, Simag, 1,1,1,0, "(imag c) return the imaginary part of a complex number",
	(Obj a))
{
	
	if(COMPLEXP(a)) return makdbl( CCOMPLEX(a).im );
	if(INUMP(a) || FLOATP(a) || DOUBLEP(a)) return makdbl( (double)0.0);

	return JLERROR(Simag.name, a, "WTA: numberp");
}
#endif

DEFUN("atan2", Fatan2, Satan2, 2,2,1,0,
      "(atan2 n1 n2)  the arctan of n2/n1",
      (Obj a, Obj b))
{
	return makdbl( atan2( dblof(a), dblof(b)));
}

DEFUN("pow", Fpow, Spow, 2,2,1,0,
      "(pow n1 n2) take n1 to the n2 power",
      (Obj a, Obj b))
{
	return makdbl( pow( dblof(a), dblof(b)));
}

Obj trancend(Obj a, double (*fnc)(double), char* fnm){
	double d;

	if(INUMP(a)||FLOATP(a)||DOUBLEP(a)||COMPLEXP(a)||BIGNUMP(a))
		d = dblof(a);
	else
		return JLERROR(fnm, a, "WTA: numberp");

	return makdbl( fnc(d) );
}

DEFUN("exp", Fexp, Sexp, 1,2,1,0,
      "(exp  d [base]) take e [or a specified base] raised to the d power",
      (Obj a, Obj b))
{
	double e, p=0.0;
	
	if( 1 || ! BIGNUMP(a)){
		e = dblof(a);
		if( BOUNDP(b))
			p = dblof(b);
		if( p==0.0)
			return makdbl( exp( e ));
		else 
			return makdbl( pow(p, e));	/* = exp( e * log(p)) */
	}
				
	/* how to handle bignums? */
	return IC_UNDEF;
}

#ifndef MINIMAL
#ifndef INFERIOR_LIBM
DEFUN("expm1", Fexpm1, Sexpm1, 1,1,1,0, "find e**x - 1",
      (Obj a))
{
	return trancend(a, expm1, Sexpm1.name);
}
#endif
#endif

DEFUN("log", Flog, Slog, 1,2,1,0, "(log x [base]) take the natural [or base] logarithm",
      (Obj a, Obj b))
{
	double e,l=0.0;
	
	if(! BIGNUMP(a)){
		e = dblof(a);
		if( BOUNDP(b))
			l = dblof(b);
		if( l==0.0)
			return makdbl( log( e ));
		else
			return makdbl( log( e) / log( l ));
	}

#ifndef NO_BIGNUMS	
	return big_log( a, b );
#else
	return IC_FALSE;	/* cant happen */
#endif	
}

#ifndef MINIMAL
#ifndef INFERIOR_LIBM
DEFUN("log1p", Flog1p, Slog1p, 1,1,1,0, "take the natural log of x+1",
      (Obj a))
{
	return trancend(a, log1p, Slog1p.name);
}
#endif
DEFUN("sin", Fsin, Ssin, 1,1,1,0, "take the sin",
      (Obj a))
{
	return trancend(a, sin, Ssin.name);
}

DEFUN("cos", Fcos, Scos, 1,1,1,0, "take the cosine",
      (Obj a))
{
	return trancend(a, cos, Scos.name);
}

DEFUN("tan", Ftan, Stan, 1,1,1,0, "take the tangent",
      (Obj a))
{
	return trancend(a, tan, Stan.name);
}

DEFUN("asin", Fasin, Sasin, 1,1,1,0, "take the arcsin",
      (Obj a))
{
	return trancend(a, asin, Sasin.name);
}

DEFUN("acos", Facos, Sacos, 1,1,1,0, "take the arccos",
      (Obj a))
{
	return trancend(a, acos, Sacos.name);
}
DEFUN("atan", Fatan, Satan, 1,1,1,0, "take the arctan",
      (Obj a))
{
	return trancend(a, atan, Satan.name /* uh oh... */);
}
DEFUN("sqrt", Fsqrt, Ssqrt, 1,1,1,0, "take the square root",
      (Obj a))
{
	return trancend(a, sqrt, Ssqrt.name);
}
DEFUN("sinh", Fsinh, Ssinh, 1,1,1,0, "take the hyperbolic sin",
      (Obj a))
{
	return trancend(a, sinh, Ssinh.name);
}
DEFUN("cosh", Fcosh, Scosh, 1,1,1,0, "take the hyperbolic cosine",
      (Obj a))
{
	return trancend(a, cosh, Scosh.name);
}
DEFUN("tanh", Ftanh, Stanh, 1,1,1,0, "take the hyperbolic tangent",
      (Obj a))
{
	return trancend(a, tanh, Stanh.name);
}
DEFUN("asinh", Fasinh, Sasinh, 1,1,1,0, "take the inverse hyperbolic sin",
      (Obj a))
{
	return trancend(a, asinh, Sasinh.name);
}
DEFUN("acosh", Facosh, Sacosh, 1,1,1,0, "take the inverse hyperbolic cosine",
      (Obj a))
{
	return trancend(a, acosh, Sacosh.name);
}
DEFUN("atanh", Fatanh, Satanh, 1,1,1,0, "take the inverse hyperbolic tangent",
      (Obj a))
{
	return trancend(a, atanh, Satanh.name);
}
#ifndef INFERIOR_LIBM
DEFUN("cbrt", Fcbrt, Scbrt, 1,1,1,0, "take the cube root",
      (Obj a))
{
	return trancend(a, cbrt, Scbrt.name);
}
#endif
#endif /* MINIMAL */

DEFUN("floor", Ffloor, Sfloor, 1,1,1,0, "take the floor",
      (Obj a))
{
	return MAKINT( (int)CDOUBLE(trancend(a, floor, Sfloor.name)) );
}
DEFUN("ceil", Fceil, Sceil, 1,1,1,0, "take the ceiling",
      (Obj a))
{
	return MAKINT( (int)CDOUBLE(trancend(a, ceil, Sceil.name)) );
}
DEFUN("abs", Fabs, Sabs, 1,1,1,0, "take the absolute value",
      (Obj a))
{
	return trancend(a, fabs, Sabs.name);
}

DEFUN("random", Frandom, Srandom, 0,0,1,0,
      "(random) return a random number",
      ())
{
#ifndef INFERIOR_LIBM
	return MAKINT(random() & 0xFFFFFFF);
#else
	return MAKINT(rand() & 0xFFFFFFF);
#endif	
}

DEFUN("->int", Fto_int, Sto_int, 1,1,1,0,
      "(->int n) convert to integer",
	(Obj n))
{
	return MAKINT( intof(n));
}

DEFUN("->float", Fto_float, Sto_float, 1,1,1,0,
      "(->float n) convert to single precision float",
	(Obj n))
{
	return makfloat( fltof(n));
}

DEFUN("->double", Fto_double, Sto_double, 1,1,1,0,
      "(->double n) convert to double precision float",
	(Obj n))
{
	return makdbl( dblof(n));
}

DEFUN("->bignum", Fto_bignum, Sto_bignum, 1,1,1,0,
      "(->bignum n) convert to a bignum",
      (Obj n))
{
	int i;
#ifndef NO_BIGNUMS
	if( BIGNUMP(n)) return n;
	if( INUMP(n))   return cint_to_big( CINT(n));
	if( FLOATP(n))  return cdouble_to_big( CFLOAT(n));
	if( DOUBLEP(n)) return cdouble_to_big( CDOUBLE(n));

	return cint_to_big(0);
#else
	return IC_UNDEF;
#endif
}


