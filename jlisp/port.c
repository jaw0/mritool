
/*
    Copyright (c) 1994 Jeff Weisberg

    see the file "License"
*/

#ifdef RCSID
static const char *const rcsid
= "@(#)$Id: port.c,v 1.1 1995/11/23 04:30:44 jaw Exp jaw $";
#endif


/* I/O ports */

#include <jlisp.h>
#include <stdio.h>
#include <setjmp.h>
#include <fcntl.h>

#ifndef NO_UNIX
#	include <unistd.h>
#endif

typedef struct {
	void (*mark)(Obj);
	int (*free)(Obj);
	int (*print)(Obj,Obj,int);
	Obj (*equal)(Obj, Obj);
	int (*getc)(Obj);
	void (*ungetc)(Obj,int);
	void (*flush)(Obj);
	void (*putc)(Obj,int);
	void (*seek)(Obj, int);
	Obj (*tell)(Obj);
} PortDesc;

extern Obj sym_eof;

extern void mark0(Obj), markcdr(Obj);
extern int free0(Obj);
extern Obj eqcdr(Obj,Obj);

void funreadc(Obj,int);
int  freadc(Obj), freefile(Obj), freepipe(Obj);
int strreadc(Obj);
void strunreadc(Obj,int);
void flflush(Obj), flputc(Obj,int), strputc(Obj,int);
void flseek(Obj,int), strseek(Obj,int);
Obj fltell(Obj), strtell(Obj);

EXTERN_C Obj Fclose(Obj);
EXTERN_C Obj Fread(Obj);

extern int freesock(Obj);
extern int sckgetc(Obj);
extern void sckputc(Obj, int);

PortDesc pdesc[] = {
#ifndef NO_FILES	
	{mark0, freefile, 0, eqcdr, freadc, funreadc, flflush, flputc, flseek, fltell },	/* std file */
#else
	{0,0,0,0,0,0,0,0},									/* no files */
#endif	
#ifndef NO_SOCKETS	
	{mark0, freepipe, 0, eqcdr, freadc, funreadc, flflush, flputc, flseek, fltell },	/* pipe */
#else
	{0,0,0,0,0,0,0,0},									/* no sockets => no pipes */
#endif	
	{markcdr, free0,  0, eqcdr, strreadc, strunreadc, 0, strputc, strseek, strtell },	/* string */
#ifndef NO_SOCKETS	
	{mark0, freesock, 0, eqcdr, sckgetc, 0, 0, sckputc, 0, 0 },				/*  socket */
#else
	{0,0,0,0,0,0,0,0},									/* no sockets */
#endif	
	{0,0,0,0,0,0,0,0}
};


/* entries to vtbl table in jlisp.c */
void markport(Obj p){
	void (*fnc)(Obj);
	int t = SUBPORT(p);

	fnc = pdesc[t].mark;

	if(fnc) fnc(p);
	else mark0(p);
}

int freeport(Obj p){
	int (*fnc)(Obj);
	int t = SUBPORT(p);

	fnc = pdesc[t].free;

	if(fnc) return fnc(p);
	else return free0(p);
}

int prnport(Obj p, Obj s, int h){
	int (*fnc)(Obj,Obj,int);
	int t = SUBPORT(p);

	fnc = pdesc[t].print;

	if(fnc) return fnc(p, s, h);
	else return 0;
}

Obj eqport(Obj a, Obj b){
	Obj (*fnc)(Obj,Obj);
	int t = SUBPORT(a);

	fnc = pdesc[t].equal;

	if(fnc) return fnc(a, b);
	else return eqcdr(a, b);
}

#ifndef NO_FILES
/* entries for port desc table (top of this file) */
int freefile(Obj a){

	fclose( CFILEPTR( a ));
	return 1;
}
#endif
#ifndef NO_SOCKETS
int freepipe(Obj a){

	pclose( CFILEPTR( a ));
	return 1;
}
#endif

#ifndef NO_FILES
int freadc(Obj p){
	return fgetc( CFILEPTR(p) );
}

void funreadc(Obj p, int c){
	ungetc(c, CFILEPTR(p));
}

void flflush(Obj p){
	fflush( CFILEPTR(p));
}

void flputc(Obj p, int c){
	fputc(c, CFILEPTR(p));
}

void flseek(Obj p, int i){
	fseek(CFILEPTR(p), i, SEEK_SET);
}

Obj fltell(Obj p){
	return MAKINT( ftell(CFILEPTR(p)));
}
#endif /* NO_FILES */

/* string port code is now in string.c */

#ifndef NO_FILES
Obj openport(Obj a, char *mode, int m, char*fnc){
	FILE*fp;
	int p=0;

	if(! STRINGP(a))
		return JLERROR(fnc, a, "WTA: filename expected");

#ifndef NO_SOCKETS
	/* NO_SOCKETS => no pipes */
	if( CCHARS(a)[0] == '|'){
		p = 1;
		fp = popen( CCHARS(a)+1, mode);
	}else
#endif		
		fp = fopen( CCHARS(a), mode);

	if( !fp)
		return IC_NIL;
	return makport( fp, m + 4*p);	/* 4*p => subtype==pipe */
}


DEFUN("open:read", Fopenread,Sopenread, 1,1,1,0,
      "(open:read filename) Open a file for reading",
      (Obj a))
{
	return openport(a, "r", READABLE, Sopenread.name);
}

DEFUN("open:write",Fopenwrite,Sopenwrite,1,1,1,0,
      "(open:write filename) Open a file for writing",
      (Obj a))
{
	return openport(a, "w", WRITABLE, Sopenwrite.name);
}

DEFUN("open:read/write", Fopenrw, Sopenrw, 1,1,1,0,
      "(open:read/write filename) Open a file for reading and writing",
      (Obj a))
{
	return openport(a, "r+", READABLE|WRITABLE, Sopenrw.name);
}

DEFUN("open:append",Fopenappend,Sopenappend,1,1,1,0,
      "(open:append filename) Open a file for appending",
      (Obj a))
{
	return openport(a, "a", WRITABLE, Sopenappend.name);
}
#endif /* NO_FILES */

DEFUN("open:string", Fopen_str, Sopen_str, 1,1,1,0,
      "(open:string string) Open a string as an io port",
      (Obj str))
{
	Obj p = newcell(), foo;
	int sigs;

	if(! STRINGP(str))
		return JLERROR(Sopen_str.name, str, "WTA: stringp");

	foo = Fcons( MAKINT(0), str);
	DISABLE( sigs );
	CAR(p) = MAKETYPE( TPV_IOPORT ) | ((8+READABLE+WRITABLE) <<12);
	CDR(p) = foo;
	RENABLE( sigs );
	
	return p;
}

#ifndef NO_FILES
DEFUN("load", Fload, Sload, 1,1,1,0,
      "(load filename) load a lisp file",
      (Obj file))
{
	/* this is used only for the initial init file
	which then redefines load to a much more useful
	function (with more JLERROR handling...) */
	FILE *fp;
	Obj foo;
	
	if( STRINGP(file)){
		Fdefine(maksym_c("*current-file*"), file, IC_UNSPEC);
		file = Fopenread(file);
	}
	if( NULLP( file )) return IC_FALSE;

	if( ! RPORTP(file))
		return IC_FALSE;
		/* return JLERROR("load",file,"WTA: filename or input port p");  */

	fp = CFILEPTR( file );

	while( !feof( fp )){
		foo = Fread(file);
		/* Fdisplay( foo, IC_UNSPEC ); */
		Feval( foo );
	}
	Fclose( file );
	return IC_TRUE;
}
#endif

int readchar(Obj port){
	int (*fnc)(Obj);
	int t = SUBPORT(port);

	if( CAR(port) & 0x80000000){
		CAR(port) &= ~0x80000000;
		return (CAR(port) >> 23) & 0xFF;
	}
	fnc = pdesc[t].getc;

	if(fnc) return fnc(port);
	else return EOF;
}

void unreadchar(Obj port, int c){
	void (*fnc)(Obj,int);
	int t = SUBPORT(port);

	fnc = pdesc[t].ungetc;

	if(fnc) fnc(port, c);
	else{
		/* save it in CAR */
		CAR(port) &= ~0xFF800000;
		CAR(port) |= (c|0x100) << 23;
	}

}

void writechar(Obj port, int c){
	void (*fnc)(Obj,int);
	int t = SUBPORT(port);

	fnc = pdesc[t].putc;

	if(fnc) fnc(port, c);
}

void writestr(Obj port, char* s){
	void (*fnc)(Obj,int);
	int t = SUBPORT(port);

	if(! (fnc=pdesc[t].putc))
		return;
	while( *s)
		fnc(port, *s++);
}

DEFUN("getc", Fgetc, Sgetc, 0,1,1,0,
      "(getc port) Read a character from the specified port",
      (Obj p))
{
	int c;
	
	if( NBOUNDP(p)) p = stdin_port;
	if( NULLP(p)){
		Fthrow(sym_eof, IC_TRUE);
		return IC_EOF;
	}
	if(! RPORTP(p))
		return JLERROR("getc",p, "WTA: input port p");
	c = readchar(p);
	if(c==EOF){
		Fthrow(sym_eof, IC_TRUE);
		return IC_EOF;
	}
	return MAKCHAR( c );
}

DEFUN("ungetc", Fungetc, Sungetc, 1,2,1,0,
      "(ungetc char [port]) un-get a character from the specified port",
      (Obj c, Obj p))
{

	if( NBOUNDP(p)) p = stdin_port;
	if(! RPORTP(p))
		return JLERROR("ungetc",p, "WTA: input port p");
	if(! ICHARP(c)) c = MAKCHAR(0);
	unreadchar(p, CCHAR( c ));
	return c;
}

DEFUN("putc", Fputc, Sputc, 1,2,1,0,
      "(putc char [port]) Write a character to the specified port",
      (Obj c, Obj p))
{
	if( NBOUNDP(p)) p = stdout_port;
	if(! WPORTP(p))
		return JLERROR("putc",p, "WTA: output port p");

	if(! ICHARP(c))
		return JLERROR(Sputc.name, c, "WTA: charp");
	
	writechar(p, CCHAR(c));
	return IC_UNSPEC;
}


DEFUN("flush", Fflush, Sflush, 0,1,1,0,
      "(flush port) flush the buffer associated with port",
      (Obj port))
{
	void (*fnc)(Obj);
	int t;

	if( NBOUNDP(port)) port = stdin_port;
	if(! IOPORTP(port))
		return JLERROR("flush", port, "WTA: ioportp");

	t = SUBPORT(port);
	fnc = pdesc[t].flush;
	if(fnc) fnc(port);
	return IC_UNSPEC;
}

/* this ought use bignum offset */
DEFUN("seek", Fseek, Sseek, 2,2,1,0,
      "(seek port offset) move file postion",
      (Obj p, Obj  o))
{
	void (*fnc)(Obj, int);
	int t;
	
	if(! IOPORTP(p))
		return JLERROR("seek", p, "WTA: ioportp");

	if(! INUMP(o))
		return JLERROR("seek", o, "WTA: integerp");
	
	t = SUBPORT(p);
	fnc = pdesc[t].seek;
	if(fnc) fnc(p, CINT(o));

	return IC_UNSPEC;
}

DEFUN("tell", Ftell, Stell, 1,1,1,0,
      "(tell port) return the current file postion",
      (Obj p))
{
	Obj (*fnc)(Obj);
	int t;

	if(! IOPORTP(p))
		return JLERROR("seek", p, "WTA: ioportp");

	t = SUBPORT(p);
	fnc = pdesc[t].tell;
	if(fnc) return fnc(p);

	return IC_UNSPEC;
}

DEFUN("close", Fclose, Sclose, 1,1,1,0,
      "(close port) closes the port",
      (Obj p))
{
	int (*fnc)(Obj);
	int t;

	if(! IOPORTP(p))
		return JLERROR("seek", p, "WTA: ioportp");

	t = SUBPORT(p);
	fnc = pdesc[t].free;
	if(fnc){
		fnc(p);
		/* make sure it is no longer used */
		CAR(p) = CDR(p) = IC_NIL;
	}
	return IC_UNSPEC;
}
