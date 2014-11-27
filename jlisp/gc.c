
/*
    Copyright (c) 1994 Jeff Weisberg

    see the file "License"
*/

#ifdef RCSID
static const char *const rcsid
= "@(#)$Id: gc.c,v 1.1 1995/11/23 04:30:42 jaw Exp jaw $";
#endif

#include <jlisp.h>
#include <stdio.h>

extern Obj boxhead, envlist;
extern int cells_since_gc;
extern Obj internal_gc_protect;
extern Obj_Vtbl jlisp_vtbl[];
extern Backtrace *backtrace_list;

extern void gc_sweep_weak(void);
extern void freecell(Obj);

DEFVAR(".gc_stats", V_gc_stats, ".gc_stats  vector desribing the results of the last gc",
       IC_NIL)
     
void gc_unmark(void){
	Obj box = boxhead, lka;
	int i, sz;
	
	while( BOXCELLP( box )){

		/* travel the whole box */
		sz = CBOXSIZE( box );
		for(i=3; i< sz; i++){
			lka = (Obj)(CBOXPTR( box ) + i);
			if( FREECELLP(lka) ) continue;
			if( CONSP(lka) ) CDR( lka ) &= ~1;
			else CAR(lka) &= ~ GCBIT;
		}

		box = CBOXNEXT( box );
	}
}

int gc_sweep(void){
	Obj box = boxhead, lka;
	register int i, t;
	int sz;
	Obj fv;
	int totals[ TPV_LAST ];

	for(i=0; i<TPV_LAST; i++)
		totals[i] = 0;
	
	while( BOXCELLP( box )){

		/* travel the whole box */
		sz = CBOXSIZE( box );
		for(i=3; i< sz; i++){
			lka = (Obj)( CBOXPTR(box) + i);
			if( FREECELLP( lka ) ) continue;
			
			/* look at GC bit -- if clear, free cell */
			/* if set, clear it and continue */
			if( CONSP( lka ) ){
				if( CDR( lka ) & 1 ){
					CDR( lka ) &= ~1;
					continue;
				}
				totals[0] ++;
			}else{
				if( CAR( lka ) & (SDBIT | GCBIT)){
					CAR( lka ) &= ~GCBIT;
					continue;
				}
				t = TYPEOF( lka );
				totals[t] += jlisp_vtbl[ t ].free( lka );
			}

			freecell( lka );
		}
		box = CBOXNEXT( box );
	}
	fv =  makvect( TPV_LAST);
	for(i=0; i<TPV_LAST; i++)
		CVECTOR(fv)[i] = MAKINT( totals[i] );
	
	return fv;
}


void gc_mark(Obj a){

	if( IMMEDP( a )) return;
	
	if( CONSP( a )){
		if( CDR( a ) & 1) return;	/* already marked */
		CDR( a ) |= 1;			/* mark */
		gc_mark( CAR( a ));		/* recurse */
		gc_mark( CDR( a ) & ~1 );
	}else{
		if( CAR(a) & GCBIT) return;	/* already marked */
		CAR(a) |= GCBIT;
		jlisp_vtbl[ TYPEOF( a ) ].mark( a );
	}
}

void gc_mark_stack(Cell** base){
	int len = (int)(((Cell**)&base) - base);
	/* since stacks grow down, len is <0 */
	register int n;
	Cell *p, *btm, *top, *foo;
	Obj box;
	
	for(n=0; n>len; n--){
		/* traipse thru' the stack */
		p = base[n];
		if( CLASS0P((Obj)p) || ((Obj)p & 3) )
			continue;
		for(box=boxhead; BOXCELLP(box); box=CBOXNEXT(box)){
			/* see if it points to a valid cell */
			foo = CBOXPTR(box);
			btm = foo + 3;
			top = foo + CBOXSIZE(box);
			/* is it within box, and properly aligned within it */
			if( p>=btm && p<top && !(((char*)p - (char*)btm)%sizeof(Cell))){
				gc_mark( (Obj)p );
				break;
			}
		}
	}
}

DEFUN("gc", Fgc, Sgc, 0, 0, 1,0, "(gc) Garbage collect",
      ())
{

	extern Obj *stackbase;
	Obj fv;

	cells_since_gc = 0;
	Frunhooks(maksym_c("before-gc-hooks"));
	
	VALUE(V_gc_stats) = IC_NIL;
	
	gc_mark( envlist );
	gc_mark( internal_gc_protect );
	gc_mark_stack( (Cell**)stackbase);
#ifdef STACK_IS_UNALIGNED
	gc_mark_stack( (Cell**)((int)stackbase + 1) );
	gc_mark_stack( (Cell**)((int)stackbase + 2) );
	gc_mark_stack( (Cell**)((int)stackbase + 3) ); 
#endif
#ifndef NO_WEAKS	
	gc_sweep_weak();
#endif	
	VALUE(V_gc_stats) = fv = gc_sweep();
	Frunhooks(maksym_c("after-gc-hooks"));

	return fv;
}





	
	
