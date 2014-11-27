extern char *malloc(), *realloc();

# line 3 "gen-ipl-h.y"
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

# define T_V 257
# define T_I 258
# define T_F 259
# define T_S 260
# define T_B 261
# define T_P 262
# define T_BR 263
# define T_BA 264
# define T_C 265
# define T_NAME 266
# define T_OPTR 267
# define T_TITLE 268
# define T_NEW 269
# define T_VALUE 270
# define T_STRING 271
# define T_SEMI 272
# define T_EOL 273
# define T_EQUAL 274
# define T_COMMA 275
# define T_OPEN 276
# define T_CLOSE 277
#define yyclearin yychar = -1
#define yyerrok yyerrflag = 0
extern int yychar;
extern int yyerrflag;
#ifndef YYMAXDEPTH
#define YYMAXDEPTH 150
#endif
#ifndef YYSTYPE
#define YYSTYPE int
#endif
YYSTYPE yylval, yyval;
# define YYERRCODE 256

# line 137 "gen-ipl-h.y"


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



	
	
int yyexca[] ={
-1, 1,
	0, -1,
	-2, 0,
-1, 8,
	0, 1,
	-2, 12,
	};
# define YYNPROD 42
# define YYLAST 87
int yyact[]={

    17,    18,    19,    20,    21,    23,    22,    14,    24,    46,
    32,    47,    16,     5,    52,     6,     7,     6,     7,     6,
     7,    17,    39,    40,    41,    50,    44,    42,    43,    45,
    39,    40,    41,    53,    44,    42,    43,    45,    37,    30,
    31,    48,    33,    26,    13,     4,     9,     2,    11,    34,
    36,    49,    25,    29,    11,    27,    28,    38,    12,    15,
    10,     3,     8,     1,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,    35,     0,     0,
     0,     0,     0,     0,     0,     0,    51 };
int yypact[]={

  -255, -1000,  -257, -1000,  -255,  -223, -1000, -1000,  -257, -1000,
  -253, -1000,  -227, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,  -266,
 -1000,  -224,  -236, -1000,  -268, -1000,  -264, -1000,  -225, -1000,
 -1000, -1000, -1000, -1000, -1000, -1000,  -246,  -228,  -260, -1000,
 -1000, -1000,  -237, -1000 };
int yypgo[]={

     0,    63,    47,    62,    61,    45,    46,    60,    44,    59,
    58,    57,    38,    53,    51,    50,    49 };
int yyr1[]={

     0,     1,     2,     2,     4,     3,     3,     6,     6,     5,
     5,     8,     8,     9,     9,     9,     9,     9,     9,     9,
    10,    10,    10,    10,    11,    11,    11,    11,    11,    11,
    11,    12,    12,    13,    13,    14,    14,    15,    15,    16,
    16,     7 };
int yyr2[]={

     0,     4,     2,     4,     5,     4,     2,     5,     3,     2,
     2,     2,     0,     3,     3,     3,     3,     3,     3,     3,
     3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
     3,     3,     9,     3,     5,     3,     1,     6,     2,     2,
     2,    13 };
int yychk[]={

 -1000,    -1,    -2,    -4,    -5,   268,   272,   273,    -3,    -6,
    -7,    -5,   -10,    -8,   264,    -9,   269,   257,   258,   259,
   260,   261,   263,   262,   265,    -2,   266,    -6,    -5,   -13,
   266,   267,   276,   266,   -16,    -8,   -15,   -12,   -11,   258,
   259,   260,   263,   264,   262,   265,   277,   275,   266,   -14,
   271,   -12,   274,   270 };
int yydef[]={

     0,    -2,    12,     2,     0,     0,     9,    10,    -2,     6,
     0,     8,     0,    20,    21,    22,    23,    11,    13,    14,
    15,    16,    17,    18,    19,     3,     4,     5,     7,     0,
    33,     0,    12,    34,     0,    39,    40,    38,    31,    24,
    25,    26,    27,    28,    29,    30,    36,     0,     0,    41,
    35,    37,     0,    32 };
typedef struct { char *t_name; int t_val; } yytoktype;
#ifndef YYDEBUG
#	define YYDEBUG	0	/* don't allow debugging */
#endif

#if YYDEBUG

yytoktype yytoks[] =
{
	"T_V",	257,
	"T_I",	258,
	"T_F",	259,
	"T_S",	260,
	"T_B",	261,
	"T_P",	262,
	"T_BR",	263,
	"T_BA",	264,
	"T_C",	265,
	"T_NAME",	266,
	"T_OPTR",	267,
	"T_TITLE",	268,
	"T_NEW",	269,
	"T_VALUE",	270,
	"T_STRING",	271,
	"T_SEMI",	272,
	"T_EOL",	273,
	"T_EQUAL",	274,
	"T_COMMA",	275,
	"T_OPEN",	276,
	"T_CLOSE",	277,
	"-unknown-",	-1	/* ends search */
};

char * yyreds[] =
{
	"-no such reduction-",
	"Prog : Head Cmds",
	"Head : Header",
	"Head : Eoln Head",
	"Header : T_TITLE T_NAME",
	"Cmds : Cmds Cmd",
	"Cmds : Cmd",
	"Cmd : Line Eoln",
	"Cmd : Eoln",
	"Eoln : T_SEMI",
	"Eoln : T_EOL",
	"Void : T_V",
	"Void : /* empty */",
	"Ret : T_I",
	"Ret : T_F",
	"Ret : T_S",
	"Ret : T_B",
	"Ret : T_BR",
	"Ret : T_P",
	"Ret : T_C",
	"Rtype : Void",
	"Rtype : T_BA",
	"Rtype : Ret",
	"Rtype : T_NEW",
	"ArgTyp : T_I",
	"ArgTyp : T_F",
	"ArgTyp : T_S",
	"ArgTyp : T_BR",
	"ArgTyp : T_BA",
	"ArgTyp : T_P",
	"ArgTyp : T_C",
	"Arg : ArgTyp",
	"Arg : ArgTyp T_NAME T_EQUAL T_VALUE",
	"Func : T_NAME",
	"Func : T_OPTR T_NAME",
	"Doc : T_STRING",
	"Doc : /* empty */",
	"Argl : Argl T_COMMA Arg",
	"Argl : Arg",
	"Args : Void",
	"Args : Argl",
	"Line : Rtype Func T_OPEN Args T_CLOSE Doc",
};
#endif /* YYDEBUG */
#line 1 "/usr/lib/yaccpar"
/*	@(#)yaccpar 1.10 89/04/04 SMI; from S5R3 1.10	*/

/*
** Skeleton parser driver for yacc output
*/

/*
** yacc user known macros and defines
*/
#define YYERROR		goto yyerrlab
#define YYACCEPT	{ free(yys); free(yyv); return(0); }
#define YYABORT		{ free(yys); free(yyv); return(1); }
#define YYBACKUP( newtoken, newvalue )\
{\
	if ( yychar >= 0 || ( yyr2[ yytmp ] >> 1 ) != 1 )\
	{\
		yyerror( "syntax error - cannot backup" );\
		goto yyerrlab;\
	}\
	yychar = newtoken;\
	yystate = *yyps;\
	yylval = newvalue;\
	goto yynewstate;\
}
#define YYRECOVERING()	(!!yyerrflag)
#ifndef YYDEBUG
#	define YYDEBUG	1	/* make debugging available */
#endif

/*
** user known globals
*/
int yydebug;			/* set to 1 to get debugging */

/*
** driver internal defines
*/
#define YYFLAG		(-1000)

/*
** static variables used by the parser
*/
static YYSTYPE *yyv;			/* value stack */
static int *yys;			/* state stack */

static YYSTYPE *yypv;			/* top of value stack */
static int *yyps;			/* top of state stack */

static int yystate;			/* current state */
static int yytmp;			/* extra var (lasts between blocks) */

int yynerrs;			/* number of errors */

int yyerrflag;			/* error recovery flag */
int yychar;			/* current input token number */


/*
** yyparse - return 0 if worked, 1 if syntax error not recovered from
*/
int
yyparse()
{
	register YYSTYPE *yypvt;	/* top of value stack for $vars */
	unsigned yymaxdepth = YYMAXDEPTH;

	/*
	** Initialize externals - yyparse may be called more than once
	*/
	yyv = (YYSTYPE*)malloc(yymaxdepth*sizeof(YYSTYPE));
	yys = (int*)malloc(yymaxdepth*sizeof(int));
	if (!yyv || !yys)
	{
		yyerror( "out of memory" );
		return(1);
	}
	yypv = &yyv[-1];
	yyps = &yys[-1];
	yystate = 0;
	yytmp = 0;
	yynerrs = 0;
	yyerrflag = 0;
	yychar = -1;

	goto yystack;
	{
		register YYSTYPE *yy_pv;	/* top of value stack */
		register int *yy_ps;		/* top of state stack */
		register int yy_state;		/* current state */
		register int  yy_n;		/* internal state number info */

		/*
		** get globals into registers.
		** branch to here only if YYBACKUP was called.
		*/
	yynewstate:
		yy_pv = yypv;
		yy_ps = yyps;
		yy_state = yystate;
		goto yy_newstate;

		/*
		** get globals into registers.
		** either we just started, or we just finished a reduction
		*/
	yystack:
		yy_pv = yypv;
		yy_ps = yyps;
		yy_state = yystate;

		/*
		** top of for (;;) loop while no reductions done
		*/
	yy_stack:
		/*
		** put a state and value onto the stacks
		*/
#if YYDEBUG
		/*
		** if debugging, look up token value in list of value vs.
		** name pairs.  0 and negative (-1) are special values.
		** Note: linear search is used since time is not a real
		** consideration while debugging.
		*/
		if ( yydebug )
		{
			register int yy_i;

			(void)printf( "State %d, token ", yy_state );
			if ( yychar == 0 )
				(void)printf( "end-of-file\n" );
			else if ( yychar < 0 )
				(void)printf( "-none-\n" );
			else
			{
				for ( yy_i = 0; yytoks[yy_i].t_val >= 0;
					yy_i++ )
				{
					if ( yytoks[yy_i].t_val == yychar )
						break;
				}
				(void)printf( "%s\n", yytoks[yy_i].t_name );
			}
		}
#endif /* YYDEBUG */
		if ( ++yy_ps >= &yys[ yymaxdepth ] )	/* room on stack? */
		{
			/*
			** reallocate and recover.  Note that pointers
			** have to be reset, or bad things will happen
			*/
			int yyps_index = (yy_ps - yys);
			int yypv_index = (yy_pv - yyv);
			int yypvt_index = (yypvt - yyv);
			yymaxdepth += YYMAXDEPTH;
			yyv = (YYSTYPE*)realloc((char*)yyv,
				yymaxdepth * sizeof(YYSTYPE));
			yys = (int*)realloc((char*)yys,
				yymaxdepth * sizeof(int));
			if (!yyv || !yys)
			{
				yyerror( "yacc stack overflow" );
				return(1);
			}
			yy_ps = yys + yyps_index;
			yy_pv = yyv + yypv_index;
			yypvt = yyv + yypvt_index;
		}
		*yy_ps = yy_state;
		*++yy_pv = yyval;

		/*
		** we have a new state - find out what to do
		*/
	yy_newstate:
		if ( ( yy_n = yypact[ yy_state ] ) <= YYFLAG )
			goto yydefault;		/* simple state */
#if YYDEBUG
		/*
		** if debugging, need to mark whether new token grabbed
		*/
		yytmp = yychar < 0;
#endif
		if ( ( yychar < 0 ) && ( ( yychar = yylex() ) < 0 ) )
			yychar = 0;		/* reached EOF */
#if YYDEBUG
		if ( yydebug && yytmp )
		{
			register int yy_i;

			(void)printf( "Received token " );
			if ( yychar == 0 )
				(void)printf( "end-of-file\n" );
			else if ( yychar < 0 )
				(void)printf( "-none-\n" );
			else
			{
				for ( yy_i = 0; yytoks[yy_i].t_val >= 0;
					yy_i++ )
				{
					if ( yytoks[yy_i].t_val == yychar )
						break;
				}
				(void)printf( "%s\n", yytoks[yy_i].t_name );
			}
		}
#endif /* YYDEBUG */
		if ( ( ( yy_n += yychar ) < 0 ) || ( yy_n >= YYLAST ) )
			goto yydefault;
		if ( yychk[ yy_n = yyact[ yy_n ] ] == yychar )	/*valid shift*/
		{
			yychar = -1;
			yyval = yylval;
			yy_state = yy_n;
			if ( yyerrflag > 0 )
				yyerrflag--;
			goto yy_stack;
		}

	yydefault:
		if ( ( yy_n = yydef[ yy_state ] ) == -2 )
		{
#if YYDEBUG
			yytmp = yychar < 0;
#endif
			if ( ( yychar < 0 ) && ( ( yychar = yylex() ) < 0 ) )
				yychar = 0;		/* reached EOF */
#if YYDEBUG
			if ( yydebug && yytmp )
			{
				register int yy_i;

				(void)printf( "Received token " );
				if ( yychar == 0 )
					(void)printf( "end-of-file\n" );
				else if ( yychar < 0 )
					(void)printf( "-none-\n" );
				else
				{
					for ( yy_i = 0;
						yytoks[yy_i].t_val >= 0;
						yy_i++ )
					{
						if ( yytoks[yy_i].t_val
							== yychar )
						{
							break;
						}
					}
					(void)printf( "%s\n", yytoks[yy_i].t_name );
				}
			}
#endif /* YYDEBUG */
			/*
			** look through exception table
			*/
			{
				register int *yyxi = yyexca;

				while ( ( *yyxi != -1 ) ||
					( yyxi[1] != yy_state ) )
				{
					yyxi += 2;
				}
				while ( ( *(yyxi += 2) >= 0 ) &&
					( *yyxi != yychar ) )
					;
				if ( ( yy_n = yyxi[1] ) < 0 )
					YYACCEPT;
			}
		}

		/*
		** check for syntax error
		*/
		if ( yy_n == 0 )	/* have an error */
		{
			/* no worry about speed here! */
			switch ( yyerrflag )
			{
			case 0:		/* new error */
				yyerror( "syntax error" );
				goto skip_init;
			yyerrlab:
				/*
				** get globals into registers.
				** we have a user generated syntax type error
				*/
				yy_pv = yypv;
				yy_ps = yyps;
				yy_state = yystate;
				yynerrs++;
			skip_init:
			case 1:
			case 2:		/* incompletely recovered error */
					/* try again... */
				yyerrflag = 3;
				/*
				** find state where "error" is a legal
				** shift action
				*/
				while ( yy_ps >= yys )
				{
					yy_n = yypact[ *yy_ps ] + YYERRCODE;
					if ( yy_n >= 0 && yy_n < YYLAST &&
						yychk[yyact[yy_n]] == YYERRCODE)					{
						/*
						** simulate shift of "error"
						*/
						yy_state = yyact[ yy_n ];
						goto yy_stack;
					}
					/*
					** current state has no shift on
					** "error", pop stack
					*/
#if YYDEBUG
#	define _POP_ "Error recovery pops state %d, uncovers state %d\n"
					if ( yydebug )
						(void)printf( _POP_, *yy_ps,
							yy_ps[-1] );
#	undef _POP_
#endif
					yy_ps--;
					yy_pv--;
				}
				/*
				** there is no state on stack with "error" as
				** a valid shift.  give up.
				*/
				YYABORT;
			case 3:		/* no shift yet; eat a token */
#if YYDEBUG
				/*
				** if debugging, look up token in list of
				** pairs.  0 and negative shouldn't occur,
				** but since timing doesn't matter when
				** debugging, it doesn't hurt to leave the
				** tests here.
				*/
				if ( yydebug )
				{
					register int yy_i;

					(void)printf( "Error recovery discards " );
					if ( yychar == 0 )
						(void)printf( "token end-of-file\n" );
					else if ( yychar < 0 )
						(void)printf( "token -none-\n" );
					else
					{
						for ( yy_i = 0;
							yytoks[yy_i].t_val >= 0;
							yy_i++ )
						{
							if ( yytoks[yy_i].t_val
								== yychar )
							{
								break;
							}
						}
						(void)printf( "token %s\n",
							yytoks[yy_i].t_name );
					}
				}
#endif /* YYDEBUG */
				if ( yychar == 0 )	/* reached EOF. quit */
					YYABORT;
				yychar = -1;
				goto yy_newstate;
			}
		}/* end if ( yy_n == 0 ) */
		/*
		** reduction by production yy_n
		** put stack tops, etc. so things right after switch
		*/
#if YYDEBUG
		/*
		** if debugging, print the string that is the user's
		** specification of the reduction which is just about
		** to be done.
		*/
		if ( yydebug )
			(void)printf( "Reduce by (%d) \"%s\"\n",
				yy_n, yyreds[ yy_n ] );
#endif
		yytmp = yy_n;			/* value to switch over */
		yypvt = yy_pv;			/* $vars top of value stack */
		/*
		** Look in goto table for next state
		** Sorry about using yy_state here as temporary
		** register variable, but why not, if it works...
		** If yyr2[ yy_n ] doesn't have the low order bit
		** set, then there is no action to be done for
		** this reduction.  So, no saving & unsaving of
		** registers done.  The only difference between the
		** code just after the if and the body of the if is
		** the goto yy_stack in the body.  This way the test
		** can be made before the choice of what to do is needed.
		*/
		{
			/* length of production doubled with extra bit */
			register int yy_len = yyr2[ yy_n ];

			if ( !( yy_len & 01 ) )
			{
				yy_len >>= 1;
				yyval = ( yy_pv -= yy_len )[1];	/* $$ = $1 */
				yy_state = yypgo[ yy_n = yyr1[ yy_n ] ] +
					*( yy_ps -= yy_len ) + 1;
				if ( yy_state >= YYLAST ||
					yychk[ yy_state =
					yyact[ yy_state ] ] != -yy_n )
				{
					yy_state = yyact[ yypgo[ yy_n ] ];
				}
				goto yy_stack;
			}
			yy_len >>= 1;
			yyval = ( yy_pv -= yy_len )[1];	/* $$ = $1 */
			yy_state = yypgo[ yy_n = yyr1[ yy_n ] ] +
				*( yy_ps -= yy_len ) + 1;
			if ( yy_state >= YYLAST ||
				yychk[ yy_state = yyact[ yy_state ] ] != -yy_n )
			{
				yy_state = yyact[ yypgo[ yy_n ] ];
			}
		}
					/* save until reenter driver code */
		yystate = yy_state;
		yyps = yy_ps;
		yypv = yy_pv;
	}
	/*
	** code supplied by user is placed in this switch
	*/
	switch( yytmp )
	{
		
case 4:
# line 66 "gen-ipl-h.y"
{
					strcpy(title, text);
					spit();
				} break;
case 7:
# line 76 "gen-ipl-h.y"
{ argn = 0; } break;
case 8:
# line 77 "gen-ipl-h.y"
{ argn = 0; } break;
case 13:
# line 88 "gen-ipl-h.y"
{ retfnc = "MAKINT";   } break;
case 14:
# line 89 "gen-ipl-h.y"
{ retfnc = "makfloat"; } break;
case 15:
# line 90 "gen-ipl-h.y"
{ retfnc = "makstr";   } break;
case 16:
# line 91 "gen-ipl-h.y"
{ retfnc = "MAKBOOL";  } break;
case 17:
# line 92 "gen-ipl-h.y"
{ retfnc = "MAKIMG_A"; } break;
case 18:
# line 93 "gen-ipl-h.y"
{ retfnc = "makpoint"; } break;
case 19:
# line 94 "gen-ipl-h.y"
{ retfnc = "makcmplx"; } break;
case 20:
# line 97 "gen-ipl-h.y"
{ rtype = 0; } break;
case 21:
# line 98 "gen-ipl-h.y"
{ rtype = 0; } break;
case 22:
# line 99 "gen-ipl-h.y"
{ rtype = 1; } break;
case 23:
# line 100 "gen-ipl-h.y"
{ rtype = 2; } break;
case 24:
# line 103 "gen-ipl-h.y"
{ DARG(1, "NUMBERP", "intof"    ); } break;
case 25:
# line 104 "gen-ipl-h.y"
{ DARG(1, "NUMBERP", "fltof"    ); } break;
case 26:
# line 105 "gen-ipl-h.y"
{ DARG(1, "STRINGP", "CCHARS"   ); } break;
case 27:
# line 106 "gen-ipl-h.y"
{ DARG(1, "IMAGEP",  "CIMAGE_A" ); } break;
case 28:
# line 107 "gen-ipl-h.y"
{ DARG(1, "IMAGEP",  "CIMAGE_A" ); } break;
case 29:
# line 108 "gen-ipl-h.y"
{ DRGX(2, "NUMBERP", "intof", "Point"   ); } break;
case 30:
# line 109 "gen-ipl-h.y"
{ DRGX(2, "NUMBERP", "fltof", "complex" ); } break;
case 31:
# line 112 "gen-ipl-h.y"
{ argn++; } break;
case 32:
# line 114 "gen-ipl-h.y"
{ arg[argn].dflp = 1;
					  arg[argn++].dflt = value;
					} break;
case 33:
# line 119 "gen-ipl-h.y"
{ operp = 0; strcpy(fncname, text); } break;
case 34:
# line 120 "gen-ipl-h.y"
{ operp = 1; strcpy(fncname, text); } break;
case 35:
# line 123 "gen-ipl-h.y"
{ strcpy(docstr, text); } break;
case 36:
# line 124 "gen-ipl-h.y"
{ docstr[0] = 0; } break;
case 41:
# line 134 "gen-ipl-h.y"
{ spew(argn, rtype, operp); } break;
	}
	goto yystack;		/* reset registers in driver code */
}
