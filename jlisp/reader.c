
/*
    Copyright (c) 1994 Jeff Weisberg

    see the file "License"
*/

#ifdef RCSID
static const char *const rcsid
= "@(#)$Id: reader.c,v 1.1 1995/11/23 04:30:40 jaw Exp jaw $";
#endif

#include <jlisp.h>
#include <stdio.h>
#include <string.h>

extern Obj makfloat(float);
extern Obj str_append(Obj, int, int);
extern Obj cint_to_big(int);

EXTERN_C Obj Fread(Obj);
int readchar(Obj port);
void unreadchar(Obj port, int c);

extern Obj sym_optional, sym_rest, sym_quote;
extern Obj sym_quote, sym_bquote, sym_bq_comma, sym_bq_comma_at;

extern Obj sym_iradix, sym_eof, sym_stdin;

extern Obj envcurr;

int static_scope = 0;	/* needs be restored on throw */

DEFVAR(".lineno", Vlineno, ".lineno the current line number", MAKINT(1))

     
void inc_lineno(){
	/* increment line number */
	VALUE( Vlineno ) += MAKINT(1) - MAKINT(0);
}

void got_eof(){
	Fthrow(sym_eof, IC_TRUE);
}

int vallof(int c, int b){

	if(c>='0' && c<='9') return c - '0';
	if(c>='a' && c<='z') return c - 'a' + 0xa;
	if(c>='A' && c<='Z') return c - 'A' + 0xA;

	return 255;
}

int isvalid(int c, int b){

	return vallof(c, b) < b;
}

void eatcomment(Obj stream){
	/* eat #| comment |#
	   may be nested */
	int c=0;
	
	while( c!='#'){
		while(c!='|'){
			c = readchar(stream);
			switch(c){
			  case '#':
				c = readchar(stream);
				if(c=='|'){
					eatcomment(stream);
					c = readchar(stream);
				}
				break;
			  case EOF:
				got_eof();
				return;
			  case '\n':
				inc_lineno();
			  default:
				break;
			}
		}
		c = readchar(stream);
	}
	return;
}

int special_char(Obj stream){
	/* handle special \escaped characters */
	int c;
	int val=0, base;
	c = readchar(stream);
	switch( c ){
	  case 'a':	c = '\a';	break;	/* yes, I know that this is the ANSI C alert char... */
	  case 'n':	c = '\n';	break;
	  case 'r':	c = '\r';	break;
	  case 'b':	c = '\b';	break;
	  case 't':	c = '\t';	break;
	  case 's':	c = ' ';	break;
	  case 'f':	c = '\f';	break;
	  case 'v':	c = '\v';	break;
	  case 'e':	c = '\033';	break;
	  case '\\':	c = '\\';	break;
	  case '"':	c = '"';	break;
		
	  case '0':
		base = 8;  goto rnum;
	  case 'x':
	  case 'X':
		c = readchar(stream);
		base = 16; goto rnum;
	  case '1': case '2': case '3':
	  case '4': case '5': case '6':
	  case '7': case '8': case '9':
		base = 10; goto rnum;
	  rnum:
		while( isvalid(c, base)){
			val *= base;
			val += vallof(c, base);
			c = readchar(stream);
		}
		unreadchar(stream, c);
		c = val;
		break;

	  case EOF:
		got_eof();
		return EOF;
	  case '\n':
		inc_lineno();
	  default:
		c = c;		break;

	}
	return c;
}

int getc_skipws(Obj stream){
	int c;
	
	while( 1 ){
		c = readchar(stream);
		switch(c){
		  case ';':
			while( c != '\r' && c!= '\n' ) c = readchar(stream);
			/* fall thru' */
		  case '\n':
			inc_lineno();
		  case ' ':
		  case '\t':
		  case '\r':
			continue;
		  case '#':
			c = readchar(stream);
			if(c!='|'){
				unreadchar(stream, c);
				return '#';
			}
			eatcomment(stream);
			continue;
		}
		break;
	}
	return c;
}

Obj readparen( Obj stream, char endparen){
	int c;
	Obj foo;

	c = getc_skipws( stream );
	if( c== endparen ) return IC_NIL;
	unreadchar(stream, c);
	foo = Fread( stream );
	if( SYMBOLP(foo) && !strcmp( CCHARS(foo), ".")){
		/* KLUDGE ALERT */
		/* a lone .
		turn it into an improper list */
		foo = Fread( stream );
		c = getc_skipws( stream );
		if( c!= endparen ) unreadchar(stream, c);
		return foo;
	}
	return Fcons( foo, readparen( stream, endparen ) );
}

DEFUN("read", Fread, Sread, 0, 1, 1,0,
      "(read [port]) read in an expression",
      (Obj stream))
{

	int c;
	char buf[1024];
	int i;
	Obj val, frac, baseo, sym;
	Obj foo, bar;
	int decmp, negp, base;
	Obj radix;
	Obj buffer;
	int prev_scope;
	
	if( NBOUNDP( stream )) stream = getvalue(sym_stdin);
	if( NULLP(stream)){
		Fthrow(sym_eof, IC_EOF);
		return IC_EOF;
	}
	if( !RPORTP( stream )){
		return JLERROR(Sread.name, stream, "wrong type of argument, inputportp");
	}

  restart:
	c = getc_skipws( stream );
	
	switch( c ){
	  case EOF:
		Fthrow(sym_eof, IC_TRUE);
		return IC_EOF;
		
	  case '(':
		val = readparen( stream, ')' );
		goto bottom;

	  case ')':
		return JLERROR(Sread.name, stream, "unexpected ')'");

	  case '{':
		/* begin a pseudo-static scoped body, (some) symbols bound at read,
		   ends with }
		*/
		prev_scope = static_scope;
		static_scope = 1;
		val = readparen(stream, '}');
		static_scope = prev_scope;
		goto bottom;

	  case '"':
		buffer = makstrn("", 0);
		i = 0;
		do {
			c = readchar(stream);
			if(c=='\\'){
				c = special_char(stream);
				str_append(buffer, i++, c);
				c = 0;
				continue;
			}
			if( c==EOF){
				got_eof();
				return IC_EOF;
			}
			if( c=='\n') inc_lineno();
			if( c!= '"')
				str_append(buffer, i++, c);
		}while( c != '"' );

		CCHARS(buffer)[i] = 0;
		val = buffer;
		goto bottom;

	  case '?':
	  rchar:
		c = readchar(stream);
		if( c == '\\' ){
			i = special_char(stream);
		}else
			i = c;
		val = MAKCHAR( i );
		goto bottom;

	  case '#':
		c = getc_skipws( stream );

		switch( c ){
		  case '\\':
			/* handle scheme-like character syntax #\x #\\n would be a newline... */
			goto rchar;
		  case 't':
		  case 'T':
			return IC_TRUE;
		  case 'f':
		  case 'F':
			return IC_FALSE;
		  case '<':
			while ( c != '>') c = getc_skipws(stream);
			return JLERROR(Sread.name, IC_UNSPEC, "unreadable syntax");
		  case '(':
			val = Flist_vect( readparen(stream, ')') );
			goto bottom;
		  case '.':
			/* CL like read time eval */
			val = Feval( Fread( stream ));
			goto bottom;
#		ifndef NO_STRUCTS			
		  case 'S':
			/* create a struct */
			val = Fread(stream);
			if( NCONSP(val))
				JLERROR(Sread.name, val, "invalid struct spec");
			/* create the object */
			foo = Fcall( getvalue(CAR(val)));
			/* set values */
			bar = Fcons(getvalue(maksym_c("method")),
				    Fcons(foo,
					  Fcons(maksym_c("set"),
						CDR(val))));
			Ffuncall( bar);
			val = foo;
			goto bottom;
#		endif

		  case 'M':
			/* object system sugar hack
			   xlat:
			        #M(obj name)	      -> (method obj 'get 'name)
			        #M(obj (fnc args...)) -> (method obj 'fnc args...)
			*/
			val = Fread(stream);
			foo = CADR( val );

			if( NCONSP( foo)){
				val = Fcons( maksym_c("method"),
					    Fcons( CAR(val),
						  Fcons( Fcons( sym_quote, Fcons( maksym_c("get"), IC_NIL)),
							Fcons( Fcons( sym_quote, Fcons( foo, IC_NIL)),
							      IC_NIL))));	
			}else{
				val = Fcons( maksym_c("method"),
					    Fcons( CAR(val), 
						  Fcons( Fcons( sym_quote, Fcons( CAR(foo), IC_NIL)),
							CDR(foo))));
			}
			goto bottom;
			
		  case '_':
			/* XXX - experimental feature -- do NOT actually use this - XXX */
			/* these do not nest */
			c = getc_skipws( stream );
			switch( c ){
			  case 's':
			  case 'S':
				/* _S turns on psuedo-static scope */
				static_scope = 1;
				break;
			  case 'd':
			  case 'D':
				/* _D turns on dynamic scope (the default) */
				static_scope = 0;
				break;
			  case 'X':
			  case 'x':
			  case '*':
				/* turns on pseudo-static on, reads, and turns it back off */
				static_scope = 1;
				val = Fread(stream);
				static_scope = 0;
				goto bottom;
			  case '{':
				/* equiv to #_*( ... ) , but we'll see if the #_{ ... } looks better ...
				now also equiv to { ... }
				*/
				prev_scope = static_scope;
				static_scope = 1;
				val = readparen(stream, '}');
				static_scope = prev_scope;
				goto bottom;
			  default:
				/* ignore ? */
				break;
			}
			goto restart;
			
		  case 'x':
		  case 'X':
			base = 16;	goto rnump;
		  case 'o':
		  case 'O':
			base = 8;	goto rnump;
		  case 'd':
		  case 'D':
			base = 10;	goto rnump;
		  case 'b':
		  case 'B':
			base = 2;
		  rnump:
			c = getc_skipws(stream);
			goto rnum;
		  case '!':
			if( VALUE(Vlineno)==MAKINT(1)){
				/* special script file handling
				   #! is a comment on the 1st line of a file
				*/
				while( c != '\n') c = readchar(stream);
				unreadchar(stream, c);
				val = Fread(stream);
				goto bottom;
			}
			/* else fall thru' */
		  default:
			return JLERROR(Sread.name, IC_UNSPEC, "unreadable syntax");

			/* how about things like #[ ... ], #{ ... },
			   CL like #+ #- #= ##
			*/
		}
		break;
		
	  case '\'':
		val = Fcons(sym_quote, Fcons(Fread( stream ), IC_NIL));
		goto bottom;

	  case '`':
		val = Fcons(sym_bquote, Fcons(Fread( stream ), IC_NIL));
		goto bottom;

	  case ',':
		c = readchar(stream);
		if( c=='@') return Fcons(sym_bq_comma_at, Fcons(Fread( stream ), IC_NIL));
		unreadchar(stream, c);
		val = Fcons(sym_bq_comma, Fcons(Fread( stream ), IC_NIL));
		goto bottom;
		
	  default:
		radix = getvalue( sym_iradix );
		
		if( INUMP(radix))
			base = CINT(radix);
		else
			base = 10;
	  rnum:
		baseo = MAKINT(base);

		i = 0;
		while(1){
			if( c==' ' ) break;
			if( c=='\t') break;
			if( c=='\r') break;
			if( c=='\n') break;
			if( c=='(' ) break;
			if( c==')' ) break;
			if( c=='{' ) break;
			if( c=='}' ) break;
			if( c==';' ) break;
			if( c==',' ) break;
			if( c=='#' ) break;	/* XXX ? ought # be allowed in a symbol name? */
			if( c==EOF ) break;
			if( c=='\\')
				c = special_char(stream);
			buf[i++] = c;
			buf[i] = 0;
			c = readchar(stream);
		}
		unreadchar(stream, c);

		/* handle 2 special cases */
		if(!strcmp(buf, "&rest")) 	return sym_rest;
		if(!strcmp(buf, "&optional")) 	return sym_optional;

		val = MAKINT(0);
		frac= makfloat(1);
		decmp = negp = 0;
		i = 0;
		
		
		if( buf[0]=='-'){
			negp = 1;
			i++;
		}
		while(buf[i]){
			if( !isvalid(buf[i], base) && buf[i]!='.'){
				/* a symbol */
				goto ret_sym;
			}
			if( buf[i]=='.' ){
				if( decmp ) return maksym( buf );
				decmp = 1;
				i++;
				val = Ftimes(val, frac);	/* frac=1.0, causes int->float */
				continue;
			}
			
			if( decmp ){
				if( FLOATP(val) && i > 6)
					/* float -> double */
					val = Fto_double(val);
				frac = Fdivide(frac, baseo);
				val  = Ftimes(val, baseo);
				val  = Fplus(val, MAKINT(vallof( buf[i], base )));
			} else {
				if( INUMP(val) && i > 6)
					/* int -> bignum */
					val = Fto_bignum(val);
				val = Ftimes(val, baseo);
				val = Fplus(val, MAKINT(vallof( buf[i], base )));
			}
			i++;

		}
		if(negp && i==1 || decmp && i==1){
		  ret_sym:
			/* if we someday decide to try using static scoping
			   we'd hook in here */

			if(! static_scope){
				val = maksym( buf );
			}else{
				/* XXX - an attempt at a pseudo-static scope
				   does a read time binding
				*/
				sym = maksym(buf);
				val = Fenvlookup(sym, IC_UNSPEC );
				if(! SYMBOXP( val )){
					val = sym;
				}
			}
		}else{
			if(decmp)
				val = Ftimes(val, frac);
			if(negp)
				val = Fminus(MAKINT(0), val);
		}
	}
  bottom:
	c = readchar(stream);
	if( c == ',' ){
		/* object system sugar hack
		   xlat:
		        obj,name	  -> (method obj 'get 'name)
			obj,(fnc args...) -> (method obj 'fnc args...)
		*/
		foo = Fread(stream);
		if( NCONSP( foo)){
			val = Fcons( maksym_c("method"),
				    Fcons( val,
					  Fcons( Fcons( sym_quote, Fcons( maksym_c("get"), IC_NIL)),
						Fcons( Fcons( sym_quote, Fcons( foo, IC_NIL)),
						      IC_NIL))));	
		}else{
			val = Fcons( maksym_c("method"),
				    Fcons( val, 
					  Fcons( Fcons( sym_quote, Fcons( CAR(foo), IC_NIL)),
						CDR(foo))));
		}
	}else{
		unreadchar(stream, c);
	}

	return val;
}





