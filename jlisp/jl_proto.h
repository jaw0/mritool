
/*
    Copyright (c) 1994 Jeff Weisberg

    see the file "License"
*/

/*
  $Id: jl_proto.h,v 1.1 1995/11/23 04:30:48 jaw Exp jaw $
*/

#ifndef _jl_proto_h
#define _jl_proto_h

#include <stdlib.h>
#include <stdio.h>
#include <jlconf.h>

EXTERN_C Obj makstr(char* str);
EXTERN_C Obj makstrn(char* str, int n);
EXTERN_C Obj maksym(char *sym);
EXTERN_C Obj makstr_c(char* str);
EXTERN_C Obj makstrn_c(char* str, int n);
EXTERN_C Obj maksym_c(char *sym);
EXTERN_C Obj makvect(int len);
EXTERN_C Obj makfloat(float f);
EXTERN_C Obj makdbl(double d);
EXTERN_C Obj makbign(long l);
EXTERN_C Obj makport( FILE* fp, int rw);
EXTERN_C Obj eqstr(Obj a, Obj b);
EXTERN_C Obj eqcdr(Obj a, Obj b);
EXTERN_C Obj eqdbl(Obj a, Obj b);
EXTERN_C Obj eqvect(Obj a, Obj b);
EXTERN_C Obj eqcons(Obj a, Obj b);

EXTERN_C int str_alloc_size( int );

EXTERN_C Obj newcell(void);
EXTERN_C Obj newbox(int);
EXTERN_C void freecell(Obj);
EXTERN_C void *my_malloc(int size);

EXTERN_C void init_csyms( Defun_Decl* );
EXTERN_C void init_init(void);
EXTERN_C Obj newsymtab(void);
EXTERN_C void pushenv(void);
EXTERN_C void popenv(void);
EXTERN_C Obj getvalue(Obj sym);

EXTERN_C Obj jlerror(char* fnc, Obj a, char* huh);
EXTERN_C float fltof(Obj);
EXTERN_C double dblof(Obj);
EXTERN_C IntType intof(Obj);

EXTERN_C Obj funcall_1(char*, Obj, Obj);
EXTERN_C Obj funcall_2(char*, Obj, Obj, Obj);
EXTERN_C Obj funcall_3(char*, Obj, Obj, Obj, Obj);
EXTERN_C Obj funcall_4(char*, Obj, Obj, Obj, Obj, Obj);

EXTERN_C Obj Fto_int(Obj);
EXTERN_C Obj Fto_float(Obj);
EXTERN_C Obj Fto_double(Obj);
EXTERN_C Obj Fto_bignum(Obj);

EXTERN_C Obj Fdisplay(Obj, Obj);
EXTERN_C Obj Fcall(Obj);
EXTERN_C Obj Feval(Obj);
EXTERN_C Obj Fdefine(Obj, Obj, Obj);
EXTERN_C Obj Fquit(Obj);
EXTERN_C Obj Fcons(Obj, Obj);
EXTERN_C Obj Fthrow(Obj, Obj);
EXTERN_C Obj Finter(Obj, Obj);
EXTERN_C Obj Fenvlookup(Obj, Obj);
EXTERN_C Obj Fprogn(Obj);
EXTERN_C Obj Feq(Obj, Obj);
EXTERN_C Obj Feqv(Obj, Obj);
EXTERN_C Obj Fequal(Obj, Obj);
EXTERN_C Obj Flength(Obj);
EXTERN_C Obj Frunhooks(Obj);
EXTERN_C Obj Fcopylist(Obj);
EXTERN_C Obj Fgc(void);
EXTERN_C Obj Flist_vect(Obj);
EXTERN_C Obj Ftimes(Obj, Obj);
EXTERN_C Obj Fdivide(Obj, Obj);
EXTERN_C Obj Fplus(Obj, Obj);
EXTERN_C Obj Fminus(Obj, Obj);
EXTERN_C Obj Ffuncall(Obj);
EXTERN_C Obj Fcopydeep(Obj);
EXTERN_C Obj Fassq(Obj, Obj);
EXTERN_C Obj Fmemv(Obj, Obj);
EXTERN_C Obj Fmemq(Obj, Obj);
EXTERN_C Obj Fassv(Obj, Obj);
EXTERN_C Obj Fprocp(Obj);

#endif /* !_jl_proto_h */
