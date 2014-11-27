
/*
    Copyright (c) 1994 Jeff Weisberg

    see the file "License"
*/

#ifdef RCSID
static const char *const rcsid
= "@(#)$Id: symtab.c,v 1.1 1995/11/23 04:30:40 jaw Exp jaw $";
#endif

/*
    code for playing with the symbol tables

    we currently spend the majority of our time here (in
    symlookup, envlookup, and hash)

    it ought be sped up (RSN)
*/

#include <jlisp.h>
#include <string.h>

#define KEEP_STATS		/* ought we keep some stats */
#define DEEP_BINDING		/* lets try using shallow */
/* shallow binding /almost/ works, but of several attempts thus far
      are either too slow
      or  break things I don't want to give up
*/      
       


/* initial hash table size: */
#define FIRST_TABLE_SIZE	1021

/* subsequent table sizes are adjustable by user */
DEFVAR(".hash_table_size", V_table_size,
       ".hash_table_size  size of internal hash tables",
       MAKINT(1021))
#define TABLE_SIZE	(VALUE( V_table_size ))
     
Obj envlist = IC_NIL, envcurr = IC_NIL;
Obj newsymtab(void);
int hash(char*);

/* keep some stats */
int level_stat[1024], max_level=0;
int link_stat[1024],  max_link=0;

/*
   envcurr ::
                 ( catch-alist . symtablist )

   symtablist ::
                 ( symtab ...)

   symtab ::
                 #( symbol-chain... )

*/		 



DEFVAR(".getvalue", Vgetvalue, ".getvalue  getvalue interloper function", IC_UNDEF)
     

EXTERN_C
void pushenv(void){
	int i;
	
	if( NULLP( envcurr )){
		for(i=0;i<1024;i++) level_stat[i] = link_stat[i] = 0;
		envcurr = Fcons( IC_NIL, IC_NIL );
	}

	if( NCONSP( envcurr )){
		JLERROR("pushenv", envcurr, "internal enviornment currupt");
		return;
		/* ought try and correct it */
	}

	/* prepend to symtab list */
	CDR( envcurr ) = Fcons( newsymtab(), CDR( envcurr ));

	/* make sure envlist exists */
	if( NULLP( envlist )){
		envlist = Fcons( envcurr, envlist );
	}
}

EXTERN_C
void popenv(void){
	
	if( NCONSP( envcurr )){
		JLERROR("popenv", envcurr, "enviornment currupt");
		return;
	}

	CDR( envcurr ) = CDDR( envcurr );
}

EXTERN_C
Obj newsymtab(void){
	Obj x=newcell(), bar;
	register int i;
	int sigs;
	int tsize;
	Obj oenv;
	
#ifdef DEEP_BINDING
	/* deep */
	
	if( V_table_size ){
		if( CINT(TABLE_SIZE) < 1)
			TABLE_SIZE = MAKINT(1);
		tsize = CINT(TABLE_SIZE);
	}else{
		tsize = FIRST_TABLE_SIZE;
	}
	
	/* inline makvect -- for speed */
	bar = (Obj)(Obj*)my_malloc(sizeof(Obj)*tsize);
	DISABLE( sigs );
	CAR( x ) = MAKETYPE( TPV_ENVEC ) | (tsize << 12);
	CDR( x ) = bar;

	for(i=0; i<tsize; i++)
		CENVVEC(x)[i] = IC_NIL;
	RENABLE( sigs );

#else
	/* shallow */

	tsize = FIRST_TABLE_SIZE;
	bar = (Obj)(Obj*)my_malloc(sizeof(Obj)*tsize);
	DISABLE(sigs);
	/* inline makvect -- for speed */
	CAR( x ) = MAKETYPE( TPV_ENVEC ) | (tsize << 12);
	CDR( x ) = bar;
	for(i=0; i<tsize; i++)
		CENVVEC(x)[i] = IC_NIL;
	RENABLE( sigs );
	
	oenv = CDR(envcurr);
	if( NNULLP(oenv)){
		oenv = CAR(oenv);
		for(i=0; i<tsize; i++)
			CENVVEC(x)[i] = Fcons(IC_NIL, CENVVEC(oenv)[i]);
		
	}else{
		/* initial table, init to (().()) */
		for(i=0; i< tsize; i++)
			CENVVEC(x)[i] = Fcons(IC_NIL, IC_NIL);
	}
	
#endif	
	return x;
}


DEFUN("symlookup", Fsymlookup, Ssymlookup, 2, 2, 1,0,
      "(envlookup symbol symboltable) Lookup symbol in the symbol table",
      (Obj a, Obj symt))
{

	int hv, i=0;
	char *a_str;
	Obj x;
	
	if( ! SYMBOLP( a ) ){
		return JLERROR(Ssymlookup.name, a, "WTA: symbolp");
	}
	if(! ENVECP( symt )){
		return JLERROR(Ssymlookup.name, symt, "WTA: symtable p");
	}

	a_str = CCHARS( a );
	
	/* hash */
	hv = hash( a_str );
	hv %= CLENGTH( symt );

	x = CENVVEC( symt )[hv];
	
	while( NNULLP( x )){
		/* follow list */

#ifdef KEEP_STATS		
		/* update stats */
		link_stat[i++]++;
		if( i>max_link) max_link = i;
#endif

		if( SYMBOXP(x)){
			if(
			   !strcmp( a_str, CSYM_BOX( x )->name )){
				return x;	/* the symbol-box */
			}
			x = CSYM_BOX( x )->next;
		}else if( CONSP(x)){
			x = CDR(x);
		}else{
			return JLERROR(Ssymlookup.name, symt, "enviornment corrupt");
		}
	}
	return IC_UNDEF;
}

DEFUN("envlookup", Fenvlookup, Senvlookup, 1, 2, 1,0,
      "(envlookup symbol [env]) Lookup symbol in the enviornment",
      (Obj a, Obj env))
{
	Obj  y, symtl, car_symtl;
	int i=0;
	/* returns the symbol-box */
	
	if( ! SYMBOLP( a ) ){
		return JLERROR(Senvlookup.name, a, "WTA: symbolp");
	}
	
	if( NBOUNDP( env ))
		env = envcurr;
	
	/* simplistic type check */
	if( NCONSP( env )){
		return JLERROR(Senvlookup.name, env, "WTA: env p");
	}

	symtl = CDR( env );	/* list of symtabs */

#ifdef DEEP_BINDING
	
	while( NNULLP( symtl ) ){
		car_symtl = CAR( symtl );

#ifdef KEEP_STATS
		/* update stats */
		level_stat[i++]++;
		if( i>max_level) max_level = i;
#endif
		if( !CONSP( symtl ) || !ENVECP( car_symtl ) ){
			return JLERROR(Senvlookup.name, symtl, "enviornment currupt");
		}

		y = Fsymlookup( a, car_symtl );
		if( SYMBOXP( y ) ) return y;

		symtl = CDR( symtl );	/* look at next table */
	}
	return IC_UNDEF;
	
#else
	/* shallow binding */
	return Fsymlookup(a, CAR(symtl));
#endif	
}


DEFUN("inter", Finter, Sinter, 1, 2, 1,0,
      "(inter symbol [symboltable]) Inter a symbol in the symbol table",
      (Obj sym, Obj symtab))
{
	Obj foo, spot;
	int hv;
	int sigs;
	
	if( NBOUNDP( symtab ) )
		symtab = CADR( envcurr );

	/* simplistic type check */
	if( ! ENVECP( symtab ) ){
  		return JLERROR(Sinter.name, symtab, "WTA: symbol table p");
	}

	if( !SYMBOLP(sym) && !SYMBOXP(sym)){
		return JLERROR(Sinter.name, sym, "WTA: symbolp");
	}
		
	
#ifdef DEEP_BINDING
	/* if using deep, do not re-inter in the same symtable,
	   just return the current one
	   if using shallow, we must inter it again
	*/
#if 0
	/* actually it can never hurt to re-inter... */
	foo = Fsymlookup( sym, symtab );
	if( SYMBOXP( foo )){
		/* already in the table */
		return foo;
	}
#endif	
#endif
	
	/* make a sym box */
	foo = newcell();

	DISABLE( sigs );
	CAR( foo ) = MAKETYPE( TPV_SYM_BOX );
	CDR( foo ) = (Obj)(Symbox*)my_malloc(sizeof(Symbox));
	CSYM_BOX( foo )->props = IC_NIL;
	CSYM_BOX( foo )->prev  = IC_NIL;
	CSYM_BOX( foo )->next  = IC_NIL;
	CSYM_BOX( foo )->value = IC_UNSPEC;

	if( SYMBOLP(sym)){
		CSYM_BOX( foo )->name = (char*)my_malloc(1 + strlen(CCHARS( sym )));	/* XXX */
		strcpy( CSYM_BOX(foo)->name, CCHARS( sym ) );				/* precludes \0s in syms */
	}else{
		CSYM_BOX( foo )->name = (char*)my_malloc(1 + strlen(CSYM_BOX(sym)->name));	/* XXX */
		strcpy( CSYM_BOX(foo)->name, CSYM_BOX(sym)->name);			/* precludes \0s in syms */
	}
	RENABLE( sigs );
	
	/* insert it into table */
	hv =  hash( SYMBOLP(sym) ? CCHARS( sym ) : CSYM_BOX(sym)->name );
	hv %= CLENGTH( symtab );

	/* link it into table */
#ifdef DEEP_BINDING
	if( NNULLP( CENVVEC(symtab)[hv] )){
                CSYM_BOX( CENVVEC(symtab)[hv] )->prev = foo;
        }
        CSYM_BOX( foo )->next = CENVVEC( symtab )[hv];
        CENVVEC( symtab )[hv] = foo;
#else	
	spot = CDR( CENVVEC(symtab)[hv]) ;
	if( NNULLP(spot)){
		if( NCONSP(spot)){
			CSYM_BOX( spot )->prev = foo;	
		}else{
			/*CDR(spot) = foo;*/
		}
		CSYM_BOX( foo )->next = spot;
	}
	CDR( CENVVEC(symtab)[hv]) = foo;
#endif
	return foo;
}

int hash(char *p){
	register int hv = 0;

	while(*p) hv = hv<<1 ^ *p++;
	hv = hv<0 ? -hv : hv;
	return hv;
}


EXTERN_C
Obj getvalue(Obj sym){
	/* get the value of a symbol in the current env */
	Obj foo;
	static within = 0;
	
	if( SYMBOLP(sym))
		sym = Fenvlookup(sym, IC_UNSPEC);
	if(! SYMBOXP(sym))
		return IC_UNDEF;

	foo = CSYM_BOX(sym)->value;

#if 0
	/* a feeped creature -- but adds too much performace hit */
	if( !within && Vgetvalue && CLASS1P( VALUE(Vgetvalue) )){
		/* assume it is an interloper function of the form
		   (.getvalue symbox value)
		   and return the value of this instead
		*/
		within = 1;
		foo = funcall_2("#<internal:getvalue>",
				VALUE(Vgetvalue),
				sym,
				foo);
		/* if the above has an error or for some reason
		   causes a non-local exit (throw) "within"
		   will not ever be reset back to 0, ergo
		   we will never get here again.

		   c'est la vie...

		   (it should be maintained in catch throw...)
	        */
		within = 0;
		return foo;
	}
#endif	
	return foo;
}

DEFUN("hashstats", Fhashstats, Shashstats, 0,0,1,0,
      "(hashstats) returns a some statistical info",
      ())
{
	Obj v;
	int i;

	v = makvect(2);

	CVECTOR(v)[0] = makvect(max_level);
	CVECTOR(v)[1] = makvect(max_link);

	for(i=0;i<max_level;i++)
		CVECTOR( CVECTOR(v)[0])[i] = MAKINT(level_stat[i]);
	for(i=0;i<max_link;i++)
		CVECTOR( CVECTOR(v)[1])[i] = MAKINT(link_stat[i]);

	return v;
}


/* these are a bit dangerous to use, but are needed for an efficient autoload... */

DEFUN("current-enviornment", Fcurr_env, Scurr_env, 0,0,1,0,
      "(current-enviornment) for internal use, do not use",
      ())
{
	return CDR( envcurr );	
}

DEFUN("with-current-enviornment", Fwith_curr_env, Swith_curr_env, 1,1,0,1,
      "(with-current-enviornment env body...) for internal use, do not use",
      (Obj l))
{
	/* we change only the symtab list, not the catch list
	   so any newly installed catches will be valid
	   (and will restore the env ...)
        */
	
	Obj env = Feval( CAR(l));
	Obj body=CDR(l);
	Obj real_env = CDR(envcurr);
	Obj result;

	if( !CONSP(env) || !ENVECP(CAR(env)) )
		return JLERROR(Swith_curr_env.name, env, "WTA: env p");
	
	CDR(envcurr) = env;
	result = Fprogn( body );
	CDR(envcurr) = real_env;

	return result;
}

/* and oh why not...*/

DEFUN("symbox-chain-next", Fnext_symbox, Snext_symbox, 1,1,1,0,
      "(symbox-chain-next symbox) for internal use, do not use",
      (Obj s))
{
	if( SYMBOXP(s))
		return CSYM_BOX(s)->next;

	if( CONSP(s))
		return Fnext_symbox( CDR(s));

	return JLERROR(Snext_symbox.name, s, "WTA: symboxp");
}

DEFUN("symbox-chain-prev", Fprev_symbox, Sprev_symbox, 1,1,1,0,
      "(symbox-chain-prev symbox) for internal use, do not use",
      (Obj s))
{
	if(! SYMBOXP(s))
		return JLERROR(Sprev_symbox.name, s, "WTA: symboxp");

	return CSYM_BOX(s)->prev;
}




