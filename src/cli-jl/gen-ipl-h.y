
%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DARG(n, t, c)	arg[argn].cnt = n;\
			arg[argn].test = t;\
			arg[argn].cnvrt = c;\
	  		arg[argn].dflp = 0;\
			arg[argn].optp = 0;\
			arg[argn].tainer = 0

#define DRGX(n, t, c, p)	DARG(n, t, c);	\
				arg[argn].tainer = p
		
struct argprm {		/* description of an arg */
	char *test,		/* predicate */
		*cnvrt,		/* conversion */
		*tainer;	/* conatiner of muliple arg parts (point, complex,...) */
	float dflt;		/* default */
	int optp,		/* optional ? */
		dflp;		/* can it have a default ? */
	int cnt;		/* # parts to arg */
};

int argn=0;
int rtype;
int operp = 0;

char fncname[1024];
char title[1024];
char text[1024];
char docstr[1024];
float value;

int txtlen;
int nestlen;
struct argprm arg[16];	/* XXX -- Hard Coded Limit!!!!! */

char *retfnc;

extern int yydebug;

#define YYDEBUG 1

%}

%token T_V, T_I, T_F, T_S, T_B, T_P, T_BR, T_BA, T_C
%token T_NAME, T_OPTR, T_TITLE, T_NEW, T_VALUE, T_STRING
%token T_SEMI, T_EOL, T_EQUAL, T_COMMA, T_OPEN, T_CLOSE

/* ToDo:
   complex
   optional params
   : optional int
 */

%%

Prog	:	Head  Cmds
	;
Head	:	Header
	|	Eoln Head
	;
Header	:	T_TITLE	T_NAME	{
					strcpy(title, text);
					spit();
				}
	;

Cmds	:	Cmds Cmd
	|	Cmd
	;

Cmd	:	Line Eoln	{ argn = 0; }
	|	Eoln		{ argn = 0; }
	;

Eoln	:	T_SEMI
	|	T_EOL
	;

Void	:	T_V
	|	/* empty */
	;

Ret	: 	T_I		{ retfnc = "MAKINT";   }	
	|	T_F		{ retfnc = "makfloat"; }
	|	T_S		{ retfnc = "makstr";   }
	|	T_B		{ retfnc = "MAKBOOL";  }
	|	T_BR		{ retfnc = "MAKIMG_A"; }
	|	T_P		{ retfnc = "makpoint"; }
	|	T_C		{ retfnc = "makcmplx"; }
	;

Rtype	:	Void		{ rtype = 0; }
	|	T_BA		{ rtype = 0; }	/* has same effect as void */
	|	Ret		{ rtype = 1; }
	|	T_NEW		{ rtype = 2; }
	;

ArgTyp	:	T_I		{ DARG(1, "NUMBERP", "intof"    ); }
	|	T_F		{ DARG(1, "NUMBERP", "fltof"    ); }
	|	T_S		{ DARG(1, "STRINGP", "CCHARS"   ); }
	|	T_BR		{ DARG(1, "IMAGEP",  "CIMAGE_A" ); }	/* interchangable -- */
	|	T_BA		{ DARG(1, "IMAGEP",  "CIMAGE_A" ); }	/* nothing takes a Bimage* */
	|	T_P		{ DRGX(2, "NUMBERP", "intof", "Point"   ); }
	|	T_C		{ DRGX(2, "NUMBERP", "fltof", "complex" ); }
	;

Arg	:	ArgTyp			{ argn++; }
	| 	ArgTyp T_NAME T_EQUAL T_VALUE
					{ arg[argn].dflp = 1;
					  arg[argn++].dflt = value;
					}
	;

Func	:	T_NAME		{ operp = 0; strcpy(fncname, text); }
	|	T_OPTR T_NAME	{ operp = 1; strcpy(fncname, text); }
	;

Doc	:	T_STRING	{ strcpy(docstr, text); }
	|	/* empty */	{ docstr[0] = 0; }

Argl	:	Argl T_COMMA Arg
	|	Arg
	;

Args	:	Void
	|	Argl
	;

Line	:	Rtype Func T_OPEN Args T_CLOSE	Doc	{ spew(argn, rtype, operp); }
	;

%%

#include "gen-ipl-h-lex.c"


int spew(int n, int r, int o){
	/* spew forth our c-code */

	int i, j, nc=0;
	char name[256], buf[64];
	static int tag = 0;
	int mna=1, mxa=1;

	/* create a name */
	if(operp)
		sprintf(name, "op_%d", tag ++);
	else
		sprintf(name, "%s", fncname);

	/* count total args */
	for(i=0; i<n; i++){
		mxa += arg[i].cnt;
		if( arg[i].optp || arg[i].dflp )
			;
		else
			mna += arg[i].cnt;
	}

	/* spit header */
	printf("DEFUN(\"img:%s\", Fimg_%s, Simg_%s, %d,%d,1,0,\n", fncname, name, name, mna, mxa);

	/* spit doc string */
	printf("\t\"(img:%s img", fncname);
	for(i=0; i<n; i++){
		printf(" ");
		if( arg[i].dflp )
			printf("[");
		sprintf(buf, "%s", arg[i].test);
		buf[ strlen(buf) - 1 ] = 0;		/* remove P */
		if( arg[i].cnt > 1)
			printf("%s", arg[i].tainer);
		else
			printf("%s", buf);
		if( arg[i].dflp )
			printf("=%f]", arg[i].dflt);
	}
	printf(") %s\",\n\t(Obj img",
		docstr[0] ? docstr : "image processing function");

	/* spit arg list */
	for(i=0; i<n; i++){
		for(j=0; j<arg[i].cnt; j++){
			printf(", Obj arg%d_%d", i, j);
		}
	}
	printf("))\n{\n");

	/* test img type */
	printf(
"	if(! IMAGEP(img))\n\
		return jlerror(Simg_%s.name, img, \"WTA: imagep\");\n",
	name);

	/* type tests */
	for(i=0; i<n; i++){
		for(j=0; j<arg[i].cnt; j++){
			if( arg[i].dflp ){
				printf(
"	if( NBOUNDP(arg%d_%d))\n\
		arg%d_%d = makfloat(%f);\n",
				i,j, i,j, arg[i].dflt);
			}

			printf(
"	if(! %s(arg%d_%d))\n\
		return jlerror(Simg_%s.name, arg%d_%d, \"WTA: %s\");\n",
			arg[i].test, i, j, name, i, j, arg[i].test);
		}
	}


	/* call fnc, return what? */
	switch( r ){
	  case 0:
		printf("\t");
		printf("CIMAGE( img )->%s%s(", operp?"operator":"", fncname);
		break;
	  case 1:
		printf("\treturn %s( ", retfnc);
		printf("CIMAGE( img )->%s%s(", operp?"operator":"", fncname);
		break;
	  case 2:
		printf("\tBimage *foo = CIMAGE(img)->clone();\n");
		printf("\tfoo->%s%s( CIMAGE_A(img)", operp?"operator":"", fncname);
		if( n ) printf(",");
		break;
	}


	/* spit fnc params */
	for(i=0; i<n; i++){
		if( arg[i].cnt != 1)
			printf(" %s(", arg[i].tainer);
		for(j=0; j<arg[i].cnt; j++){
			printf(" %s( arg%d_%d )", arg[i].cnvrt, i, j);
			if(j!=arg[i].cnt-1)
				printf(",");
		}
		if( arg[i].cnt != 1)
			printf(")");
		if(i!=n-1)
			printf(",");
	}

	switch( r ){
	  case 0:
		printf(");\n\treturn img;\n}\n\n");
		break;
	  case 1:
		printf("));\n}\n\n");
		break;
	  case 2:
		printf(");\n\treturn makimage(*foo);\n}\n\n");
		break;
	}
}

int yywrap(){
	exit(0);
}

void yyerror(s)
	char *s; {

	fprintf(stderr, "Error: %s at line %d\n", s, yylineno);
}

spit(){
	/* spit out the header */

	printf(
"\n\
// Copyright (c) 1994 Jeff Weisberg\n\
\n\
// This file is automagically generated -- Do NOT edit this directly!\n\
// Derived from: %s\n\
\n\
// Optimization: Full\n\
\n\
#define MAKBOOL(x)	((x)?IC_TRUE:IC_FALSE)\n\
#define CIMAGE_A(x)	(*CIMAGE(x))\n\
#define MAKIMG_A(x)	makimage(*(x))\n\
#define NUMBERP(x)	(INUMP(x) || FLOATP(x) || DOUBLEP(x) || BIGNUMP(x))\n\
\n\
#include <jlisp.h>\n\
#include <defproto.h>\n\
\n\
#include <mritool.h>\n\
#include <solid.h>\n\
#include <mrilisp.h>\n\
\n\
\n\
static Obj makcmplx(complex c){\n\
	return Fcons( makfloat( real(c)), makfloat( imag(c))); \n\
}\n\
static Obj makpoint(Point p){\n\
	return Fcons( MAKINT( p.x), MAKINT( p.y));\n\
}\n\
\n",
title);

}

main(){
	yyparse();
}



	
	
