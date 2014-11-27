# include "stdio.h"
# define U(x) x
# define NLSTATE yyprevious=YYNEWLINE
# define BEGIN yybgin = yysvec + 1 +
# define INITIAL 0
# define YYLERR yysvec
# define YYSTATE (yyestate-yysvec-1)
# define YYOPTIM 1
# define YYLMAX BUFSIZ
# define output(c) putc(c,yyout)
# define input() (((yytchar=yysptr>yysbuf?U(*--yysptr):getc(yyin))==10?(yylineno++,yytchar):yytchar)==EOF?0:yytchar)
# define unput(c) {yytchar= (c);if(yytchar=='\n')yylineno--;*yysptr++=yytchar;}
# define yymore() (yymorfg=1)
# define ECHO fprintf(yyout, "%s",yytext)
# define REJECT { nstr = yyreject(); goto yyfussy;}
int yyleng; extern char yytext[];
int yymorfg;
extern char *yysptr, yysbuf[];
int yytchar;
FILE *yyin = {stdin}, *yyout = {stdout};
extern int yylineno;
struct yysvf { 
	struct yywork *yystoff;
	struct yysvf *yyother;
	int *yystops;};
struct yysvf *yyestate;
extern struct yysvf yysvec[], *yybgin;
# define yS 2
# define yR 4
# define YYNEWLINE 10
yylex(){
int nstr; extern int yyprevious;
       BEGIN   yS;
while((nstr = yylook()) >= 0)
yyfussy: switch(nstr){
case 0:
if(yywrap()) return(0); break;
case 1:
	case 2:
	;
break;
case 3:
		;
break;
case 4:
	;
break;
case 5:
	{ return T_TITLE; }
break;
case 6:
	{ return T_V; }
break;
case 7:
	case 8:
	{ return T_F; }
break;
case 9:
	case 10:
	{ return T_I; }
break;
case 11:
	{ return T_B; }
break;
case 12:
	case 13:
	{ return T_S; }
break;
case 14:
	case 15:
	{ return T_P; }
break;
case 16:
	case 17:
	case 18:
	{ return T_BR; }
break;
case 19:
	{ return T_BA; }
break;
case 20:
	case 21:
	{ return T_C; }
break;
case 22:
	{ return T_NEW; }
break;
case 23:
	{ return T_OPTR; }
break;
case 24:
		{ return T_COMMA; }
break;
case 25:
		{ return T_SEMI; }
break;
case 26:
		{ return T_EQUAL; }
break;
case 27:
		{ return T_OPEN; }
break;
case 28:
		{ return T_CLOSE; }
break;
case 29:
{
				value = atof( yytext );
				return T_VALUE;
			}
break;
case 30:
{
				strcpy(text, yytext);
				return T_NAME;
			}
break;
case 31:
	{
				yydebug = 1;
				return T_EOL;
			}
break;
case 32:
         { BEGIN yR; txtlen = 0; }
break;
case 33:
        { text[txtlen++] = yytext[1]; }
break;
case 34:
         { BEGIN yS; text[txtlen] = 0; return T_STRING; }
break;
case 35:
          { text[txtlen++] = *yytext; }
break;
case -1:
break;
default:
fprintf(yyout,"bad switch yylook %d",nstr);
} return(0); }
/* end of yylex */


 
int yyvstop[] = {
0,

30,
0,

30,
0,

30,
0,

30,
0,

30,
0,

30,
0,

30,
0,

3,
0,

32,
0,

27,
0,

28,
0,

24,
0,

30,
0,

30,
0,

30,
0,

29,
30,
0,

25,
0,

26,
30,
0,

30,
0,

30,
0,

30,
0,

30,
0,

30,
0,

30,
0,

30,
0,

30,
0,

30,
0,

30,
0,

30,
0,

30,
0,

30,
0,

30,
0,

35,
0,

34,
35,
0,

35,
0,

29,
30,
0,

30,
0,

30,
-1,
0,

29,
30,
0,

30,
0,

30,
0,

30,
0,

30,
0,

30,
0,

30,
0,

30,
0,

30,
0,

30,
0,

14,
30,
0,

30,
0,

30,
0,

30,
0,

30,
0,

30,
0,

30,
0,

30,
0,

30,
0,

33,
0,

30,
0,

-1,
0,

1,
0,

30,
0,

30,
0,

30,
0,

30,
0,

30,
0,

7,
30,
0,

30,
0,

16,
30,
0,

10,
30,
0,

22,
30,
0,

30,
0,

12,
30,
0,

30,
0,

30,
0,

30,
0,

30,
0,

30,
0,

30,
0,

30,
0,

2,
30,
0,

30,
0,

11,
30,
0,

30,
0,

30,
0,

30,
0,

30,
0,

30,
0,

30,
0,

6,
30,
0,

30,
0,

30,
0,

30,
0,

30,
0,

30,
0,

2,
0,

30,
0,

20,
30,
0,

30,
0,

8,
30,
0,

17,
30,
0,

15,
30,
0,

5,
30,
0,

13,
30,
0,

4,
30,
0,

30,
0,

30,
0,

30,
-31,
0,

30,
0,

30,
0,

30,
0,

9,
30,
0,

31,
0,

19,
30,
0,

18,
30,
0,

21,
30,
0,

30,
0,

23,
30,
0,
0};
# define YYTYPE char
struct yywork { YYTYPE verify, advance; } yycrank[] = {
0,0,	0,0,	3,7,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	3,8,	3,8,	
83,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
6,0,	60,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	3,9,	
0,0,	0,0,	0,0,	0,0,	
0,0,	3,10,	3,11,	0,0,	
83,60,	3,12,	3,13,	3,14,	
3,15,	3,16,	6,33,	6,33,	
0,0,	60,60,	6,33,	0,0,	
61,61,	0,0,	60,83,	0,0,	
3,17,	0,0,	3,18,	0,0,	
0,0,	0,0,	0,0,	3,19,	
3,20,	6,33,	0,0,	3,21,	
0,0,	0,0,	3,22,	0,0,	
0,0,	0,0,	0,0,	3,23,	
0,0,	3,24,	0,0,	0,0,	
3,25,	3,26,	0,0,	3,27,	
4,10,	4,11,	0,0,	0,0,	
4,12,	4,13,	4,14,	4,15,	
0,0,	5,33,	0,0,	3,28,	
3,29,	0,0,	6,35,	61,61,	
0,0,	5,33,	5,0,	4,17,	
0,0,	4,18,	0,0,	0,0,	
3,30,	0,0,	0,0,	0,0,	
4,32,	0,0,	3,31,	0,0,	
0,0,	61,61,	61,61,	0,0,	
0,0,	61,61,	0,0,	0,0,	
61,61,	0,0,	5,34,	0,0,	
0,0,	61,61,	0,0,	61,61,	
5,33,	5,33,	61,61,	61,61,	
5,33,	61,61,	0,0,	0,0,	
5,33,	0,0,	4,28,	4,29,	
0,0,	0,0,	0,0,	0,0,	
7,7,	0,0,	0,0,	5,33,	
0,0,	0,0,	0,0,	4,30,	
7,0,	7,0,	5,33,	5,33,	
7,0,	4,31,	5,33,	0,0,	
0,0,	5,33,	0,0,	0,0,	
0,0,	0,0,	5,33,	0,0,	
5,33,	13,0,	13,0,	5,33,	
5,33,	13,0,	5,33,	7,0,	
0,0,	7,0,	0,0,	0,0,	
5,35,	0,0,	0,0,	7,0,	
7,0,	0,0,	0,0,	7,0,	
0,0,	0,0,	0,0,	7,7,	
13,0,	0,0,	13,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
13,0,	13,0,	7,0,	0,0,	
13,0,	0,0,	13,14,	0,0,	
13,16,	7,7,	7,7,	14,0,	
14,0,	7,7,	0,0,	14,0,	
7,7,	0,0,	0,0,	13,0,	
0,0,	7,7,	0,0,	7,7,	
15,0,	15,0,	7,7,	7,7,	
15,0,	7,7,	0,0,	0,0,	
0,0,	0,0,	14,0,	0,0,	
14,0,	0,0,	0,0,	0,0,	
16,0,	16,0,	14,0,	14,0,	
16,0,	0,0,	14,0,	15,0,	
0,0,	15,0,	14,36,	0,0,	
0,0,	0,0,	0,0,	15,0,	
15,0,	15,37,	0,0,	15,0,	
0,0,	14,0,	15,38,	16,0,	
0,0,	16,0,	18,0,	18,0,	
0,0,	0,0,	18,0,	16,0,	
16,0,	0,0,	15,0,	16,0,	
0,0,	16,39,	0,0,	16,16,	
19,0,	19,0,	0,0,	0,0,	
19,0,	0,0,	0,0,	0,0,	
0,0,	18,0,	16,0,	18,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	18,0,	18,0,	0,0,	
0,0,	18,0,	0,0,	19,0,	
0,0,	19,0,	20,0,	20,0,	
0,0,	0,0,	20,0,	19,0,	
19,0,	0,0,	0,0,	19,0,	
18,0,	21,0,	21,0,	0,0,	
0,0,	21,0,	22,0,	22,0,	
0,0,	0,0,	22,0,	0,0,	
0,0,	20,0,	19,0,	20,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	20,0,	20,0,	0,0,	
21,0,	20,0,	21,0,	0,0,	
0,0,	22,0,	0,0,	22,0,	
21,0,	21,0,	0,0,	0,0,	
21,0,	22,0,	22,0,	0,0,	
20,0,	22,0,	0,0,	0,0,	
23,0,	23,0,	0,0,	0,0,	
23,0,	24,0,	24,0,	21,0,	
0,0,	24,0,	0,0,	0,0,	
22,0,	0,0,	0,0,	0,0,	
19,40,	0,0,	0,0,	0,0,	
0,0,	0,0,	19,41,	23,0,	
0,0,	23,0,	0,0,	0,0,	
24,0,	0,0,	24,0,	23,0,	
23,0,	0,0,	0,0,	23,0,	
24,0,	24,0,	0,0,	0,0,	
24,0,	25,0,	25,0,	0,0,	
0,0,	25,0,	20,42,	0,0,	
20,43,	0,0,	23,0,	0,0,	
0,0,	0,0,	0,0,	24,0,	
21,44,	0,0,	0,0,	0,0,	
0,0,	0,0,	22,45,	22,46,	
25,0,	0,0,	25,0,	0,0,	
0,0,	0,0,	26,0,	26,0,	
25,0,	25,0,	26,0,	0,0,	
25,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	27,0,	
27,0,	0,0,	0,0,	27,0,	
0,0,	0,0,	0,0,	25,0,	
23,47,	26,0,	0,0,	26,0,	
28,0,	28,0,	0,0,	0,0,	
28,0,	26,0,	26,0,	0,0,	
0,0,	26,0,	27,0,	24,48,	
27,0,	29,0,	29,0,	0,0,	
24,49,	29,0,	27,0,	27,0,	
0,0,	0,0,	27,0,	28,0,	
26,0,	28,0,	30,0,	30,0,	
0,0,	0,0,	30,0,	28,0,	
28,0,	0,0,	0,0,	28,0,	
29,0,	27,0,	29,0,	0,0,	
0,0,	0,0,	31,0,	31,0,	
29,0,	29,0,	31,0,	0,0,	
29,0,	30,0,	28,0,	30,0,	
25,50,	0,0,	0,0,	0,0,	
0,0,	30,0,	30,0,	0,0,	
0,0,	30,0,	0,0,	29,0,	
0,0,	31,0,	0,0,	31,0,	
0,0,	0,0,	26,51,	0,0,	
35,58,	31,0,	31,0,	0,0,	
30,0,	31,0,	0,0,	0,0,	
35,58,	35,0,	0,0,	0,0,	
0,0,	0,0,	32,0,	32,0,	
0,0,	27,52,	32,0,	0,0,	
31,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
36,0,	36,0,	28,41,	0,0,	
36,0,	35,58,	0,0,	0,0,	
29,53,	32,0,	0,0,	32,0,	
0,0,	29,42,	0,0,	29,54,	
0,0,	32,0,	32,0,	35,58,	
0,0,	32,0,	0,0,	36,0,	
0,0,	36,0,	0,0,	0,0,	
31,56,	30,55,	0,0,	36,0,	
36,0,	0,0,	37,37,	36,0,	
32,0,	35,58,	35,58,	36,36,	
0,0,	35,58,	37,59,	37,0,	
35,58,	0,0,	32,57,	0,0,	
0,0,	35,58,	36,0,	35,58,	
0,0,	0,0,	35,58,	35,58,	
0,0,	35,58,	0,0,	0,0,	
38,38,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	37,59,	
38,61,	38,62,	0,0,	0,0,	
0,0,	39,0,	39,0,	37,60,	
0,0,	39,0,	0,0,	0,0,	
0,0,	37,37,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	38,61,	0,0,	0,0,	
39,0,	0,0,	39,0,	37,37,	
37,37,	0,0,	0,0,	37,37,	
39,0,	39,0,	37,37,	38,38,	
39,0,	0,0,	0,0,	37,37,	
39,39,	37,37,	0,0,	0,0,	
37,37,	37,37,	0,0,	37,37,	
0,0,	0,0,	0,0,	39,0,	
0,0,	38,38,	38,38,	40,0,	
40,0,	38,38,	0,0,	40,0,	
38,38,	0,0,	0,0,	0,0,	
0,0,	38,38,	0,0,	38,38,	
41,0,	41,0,	38,38,	38,38,	
41,0,	38,38,	0,0,	0,0,	
0,0,	0,0,	40,0,	0,0,	
40,0,	42,0,	42,0,	0,0,	
0,0,	42,0,	40,0,	40,0,	
0,0,	0,0,	40,0,	41,0,	
0,0,	41,0,	43,0,	43,0,	
0,0,	0,0,	43,0,	41,0,	
41,0,	0,0,	0,0,	41,0,	
42,0,	40,0,	42,0,	44,0,	
44,0,	0,0,	0,0,	44,0,	
42,0,	42,0,	0,0,	0,0,	
42,0,	43,0,	41,0,	43,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	43,0,	43,0,	0,0,	
0,0,	43,0,	44,0,	42,0,	
44,0,	45,0,	45,0,	0,0,	
0,0,	45,0,	44,0,	44,0,	
0,0,	0,0,	44,0,	0,0,	
43,0,	0,0,	46,0,	46,0,	
0,0,	0,0,	46,0,	0,0,	
0,0,	0,0,	0,0,	40,63,	
45,0,	44,0,	45,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
45,0,	45,0,	0,0,	0,0,	
45,0,	46,0,	41,64,	46,0,	
47,0,	47,0,	0,0,	0,0,	
47,0,	46,0,	46,0,	0,0,	
0,0,	46,0,	0,0,	45,0,	
42,65,	48,0,	48,0,	0,0,	
0,0,	48,0,	0,0,	0,0,	
0,0,	0,0,	43,66,	47,0,	
46,0,	47,0,	0,0,	0,0,	
0,0,	49,0,	49,0,	47,0,	
47,0,	49,0,	0,0,	47,0,	
48,0,	44,67,	48,0,	0,0,	
0,0,	0,0,	44,68,	0,0,	
48,0,	48,0,	0,0,	0,0,	
48,0,	45,69,	47,0,	0,0,	
49,0,	0,0,	49,0,	45,70,	
0,0,	0,0,	0,0,	0,0,	
49,0,	49,0,	0,0,	48,0,	
49,0,	50,0,	50,0,	0,0,	
0,0,	50,0,	51,0,	51,0,	
0,0,	0,0,	51,0,	0,0,	
0,0,	0,0,	0,0,	49,0,	
0,0,	46,71,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
50,0,	0,0,	50,0,	0,0,	
0,0,	51,0,	0,0,	51,0,	
50,0,	50,0,	0,0,	0,0,	
50,0,	51,0,	51,0,	52,0,	
52,0,	51,0,	0,0,	52,0,	
0,0,	48,73,	47,72,	0,0,	
0,0,	0,0,	0,0,	50,0,	
0,0,	53,0,	53,0,	0,0,	
51,0,	53,0,	0,0,	0,0,	
0,0,	0,0,	52,0,	0,0,	
52,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	52,0,	52,0,	
0,0,	0,0,	52,0,	0,0,	
53,0,	0,0,	53,0,	54,0,	
54,0,	0,0,	0,0,	54,0,	
53,0,	53,0,	0,0,	0,0,	
53,0,	52,0,	0,0,	0,0,	
55,0,	55,0,	0,0,	0,0,	
55,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	54,0,	53,0,	
54,0,	0,0,	50,74,	0,0,	
56,0,	56,0,	54,0,	54,0,	
56,0,	51,75,	54,0,	55,0,	
0,0,	55,0,	0,0,	0,0,	
0,0,	57,0,	57,0,	55,0,	
55,0,	57,0,	0,0,	55,0,	
0,0,	54,0,	0,0,	56,0,	
0,0,	56,0,	0,0,	52,76,	
59,59,	0,0,	0,0,	56,0,	
56,0,	53,77,	55,0,	56,0,	
57,0,	59,0,	57,0,	0,0,	
0,0,	0,0,	63,0,	63,0,	
57,0,	57,0,	63,0,	0,0,	
57,0,	0,0,	56,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
64,0,	64,0,	0,0,	0,0,	
64,0,	0,0,	0,0,	57,0,	
0,0,	63,0,	0,0,	63,0,	
0,0,	59,82,	57,81,	54,66,	
54,78,	63,0,	63,0,	59,59,	
55,79,	63,0,	0,0,	64,0,	
0,0,	64,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	64,0,	
64,0,	0,0,	56,80,	64,0,	
63,0,	59,59,	59,59,	0,0,	
0,0,	59,59,	0,0,	0,0,	
59,59,	0,0,	57,7,	0,0,	
0,0,	59,59,	64,0,	59,59,	
65,0,	65,0,	59,59,	59,59,	
65,0,	59,59,	0,0,	0,0,	
0,0,	0,0,	0,0,	66,0,	
66,0,	0,0,	0,0,	66,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	63,84,	65,0,	
0,0,	65,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	65,0,	
65,0,	0,0,	66,0,	65,0,	
66,0,	67,0,	67,0,	0,0,	
0,0,	67,0,	66,0,	66,0,	
0,0,	0,0,	66,0,	64,85,	
68,0,	68,0,	65,0,	0,0,	
68,0,	69,0,	69,0,	0,0,	
0,0,	69,0,	0,0,	0,0,	
67,0,	66,0,	67,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
67,0,	67,0,	0,0,	68,0,	
67,0,	68,0,	0,0,	0,0,	
69,0,	0,0,	69,0,	68,0,	
68,0,	0,0,	0,0,	68,0,	
69,0,	69,0,	0,0,	67,0,	
69,0,	0,0,	0,0,	0,0,	
70,0,	70,0,	0,0,	0,0,	
70,0,	0,0,	68,0,	65,86,	
0,0,	0,0,	0,0,	69,0,	
0,0,	71,0,	71,0,	0,0,	
0,0,	71,0,	0,0,	0,0,	
0,0,	0,0,	66,87,	70,0,	
0,0,	70,0,	72,0,	72,0,	
0,0,	0,0,	72,0,	70,0,	
70,0,	67,88,	0,0,	70,0,	
71,0,	0,0,	71,0,	73,0,	
73,0,	0,0,	0,0,	73,0,	
71,0,	71,0,	0,0,	0,0,	
71,0,	72,0,	70,0,	72,0,	
74,0,	74,0,	0,0,	69,89,	
74,0,	72,0,	72,0,	0,0,	
0,0,	72,0,	73,0,	71,0,	
73,0,	75,0,	75,0,	0,0,	
0,0,	75,0,	73,0,	73,0,	
0,0,	0,0,	73,0,	74,0,	
72,0,	74,0,	76,0,	76,0,	
0,0,	0,0,	76,0,	74,0,	
74,0,	0,0,	0,0,	74,0,	
75,0,	73,0,	75,0,	77,0,	
77,0,	0,0,	0,0,	77,0,	
75,0,	75,0,	0,0,	0,0,	
75,0,	76,0,	74,0,	76,0,	
78,0,	78,0,	0,0,	0,0,	
78,0,	76,0,	76,0,	0,0,	
0,0,	76,0,	77,0,	75,0,	
77,0,	0,0,	0,0,	0,0,	
79,0,	79,0,	77,0,	77,0,	
79,0,	0,0,	77,0,	78,0,	
76,0,	78,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	78,0,	
78,0,	0,0,	0,0,	78,0,	
73,90,	77,0,	0,0,	79,0,	
0,0,	79,0,	0,0,	0,0,	
0,0,	80,0,	80,0,	79,0,	
79,0,	80,0,	78,0,	79,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	81,0,	81,0,	
75,91,	0,0,	81,0,	0,0,	
82,59,	76,92,	79,0,	0,0,	
80,0,	0,0,	80,0,	0,0,	
0,0,	82,0,	0,0,	0,0,	
80,0,	80,0,	0,0,	0,0,	
80,0,	81,0,	0,0,	81,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	81,0,	81,0,	0,0,	
77,93,	81,0,	0,0,	80,0,	
0,0,	0,0,	0,0,	0,0,	
84,0,	84,0,	0,0,	0,0,	
84,0,	82,82,	78,94,	0,0,	
81,0,	0,0,	82,98,	82,59,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	79,95,	0,0,	84,0,	
0,0,	84,0,	0,0,	0,0,	
0,0,	82,59,	82,59,	84,0,	
84,0,	82,59,	81,97,	84,0,	
82,59,	0,0,	85,0,	85,0,	
80,96,	82,59,	85,0,	82,59,	
0,0,	0,0,	82,59,	82,59,	
0,0,	82,59,	84,0,	86,0,	
86,0,	0,0,	0,0,	86,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	85,0,	0,0,	85,0,	
87,0,	87,0,	0,0,	0,0,	
87,0,	85,0,	85,0,	0,0,	
0,0,	85,0,	86,0,	0,0,	
86,0,	88,0,	88,0,	0,0,	
0,0,	88,0,	86,0,	86,0,	
0,0,	0,0,	86,0,	87,0,	
85,0,	87,0,	0,0,	0,0,	
0,0,	0,0,	84,99,	87,0,	
87,0,	0,0,	0,0,	87,0,	
88,0,	86,0,	88,0,	89,0,	
89,0,	0,0,	0,0,	89,0,	
88,0,	88,0,	0,0,	0,0,	
88,0,	0,0,	87,0,	0,0,	
90,0,	90,0,	0,0,	0,0,	
90,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	89,0,	88,0,	
89,0,	91,0,	91,0,	0,0,	
0,0,	91,0,	89,0,	89,0,	
0,0,	0,0,	89,0,	90,0,	
0,0,	90,0,	92,0,	92,0,	
0,0,	0,0,	92,0,	90,0,	
90,0,	0,0,	0,0,	90,0,	
91,0,	89,0,	91,0,	93,0,	
93,0,	0,0,	0,0,	93,0,	
91,0,	91,0,	86,100,	87,101,	
91,0,	92,0,	90,0,	92,0,	
94,0,	94,0,	0,0,	0,0,	
94,0,	92,0,	92,0,	0,0,	
0,0,	92,0,	93,0,	91,0,	
93,0,	95,0,	95,0,	0,0,	
88,102,	95,0,	93,0,	93,0,	
93,106,	0,0,	93,0,	94,0,	
92,0,	94,0,	0,0,	89,103,	
0,0,	96,0,	96,0,	94,0,	
94,0,	96,0,	0,0,	94,0,	
95,0,	93,0,	95,0,	0,0,	
0,0,	0,0,	97,0,	97,0,	
95,0,	95,0,	97,0,	98,59,	
95,0,	0,0,	94,0,	0,0,	
96,0,	91,105,	96,0,	90,104,	
98,0,	0,0,	99,0,	99,0,	
96,0,	96,0,	99,0,	95,0,	
96,0,	97,0,	0,0,	97,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	97,0,	97,0,	0,0,	
0,0,	97,0,	0,0,	96,0,	
0,0,	99,0,	0,0,	99,0,	
0,0,	0,0,	0,0,	0,0,	
98,82,	99,0,	99,0,	0,0,	
97,0,	99,0,	98,59,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	95,108,	100,0,	100,0,	
97,110,	0,0,	100,0,	94,107,	
99,0,	0,0,	0,0,	0,0,	
98,59,	98,59,	0,0,	0,0,	
98,59,	96,109,	0,0,	98,59,	
0,0,	0,0,	0,0,	0,0,	
98,59,	100,0,	98,59,	100,0,	
0,0,	98,59,	98,59,	0,0,	
98,59,	100,0,	100,0,	101,0,	
101,0,	100,0,	0,0,	101,0,	
102,0,	102,0,	0,0,	0,0,	
102,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	99,111,	0,0,	
100,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	101,0,	0,0,	
101,0,	0,0,	0,0,	102,0,	
0,0,	102,0,	101,0,	101,0,	
0,0,	0,0,	101,0,	102,0,	
102,0,	103,0,	103,0,	102,0,	
0,0,	103,0,	104,0,	104,0,	
0,0,	0,0,	104,0,	0,0,	
0,0,	101,0,	0,0,	0,0,	
0,0,	0,0,	102,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
103,0,	0,0,	103,0,	0,0,	
0,0,	104,0,	0,0,	104,0,	
103,0,	103,0,	0,0,	0,0,	
103,0,	104,0,	104,0,	105,0,	
105,0,	104,0,	0,0,	105,0,	
106,0,	106,0,	0,0,	0,0,	
106,0,	0,0,	0,0,	103,0,	
0,0,	0,0,	0,0,	101,112,	
104,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	105,0,	0,0,	
105,0,	0,0,	0,0,	106,0,	
0,0,	106,0,	105,0,	105,0,	
0,0,	0,0,	105,0,	106,0,	
106,0,	107,0,	107,0,	106,0,	
0,0,	107,0,	108,0,	108,0,	
0,0,	0,0,	108,0,	0,0,	
0,0,	105,0,	0,0,	0,0,	
0,0,	0,0,	106,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
107,0,	0,0,	107,0,	0,0,	
0,0,	108,0,	0,0,	108,0,	
107,0,	107,0,	0,0,	0,0,	
107,0,	108,0,	108,0,	109,0,	
109,0,	108,0,	0,0,	109,0,	
110,0,	110,115,	0,0,	0,0,	
110,0,	0,0,	0,0,	107,0,	
0,0,	0,0,	0,0,	0,0,	
108,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	109,0,	0,0,	
109,0,	0,0,	0,0,	110,0,	
0,0,	110,0,	109,0,	109,0,	
0,0,	0,0,	109,0,	110,0,	
110,0,	111,0,	111,0,	110,0,	
0,0,	111,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	109,0,	0,0,	112,0,	
112,0,	0,0,	110,0,	112,0,	
0,0,	0,0,	0,0,	0,0,	
111,0,	0,0,	111,0,	0,0,	
0,0,	0,0,	111,116,	0,0,	
111,0,	111,0,	111,117,	0,0,	
111,0,	108,113,	112,0,	0,0,	
112,0,	113,0,	113,0,	0,0,	
0,0,	113,0,	112,0,	112,0,	
0,0,	0,0,	112,0,	111,0,	
114,0,	114,0,	0,0,	0,0,	
114,0,	116,0,	116,0,	0,0,	
0,0,	116,0,	0,0,	0,0,	
113,0,	112,0,	113,0,	0,0,	
109,114,	0,0,	0,0,	0,0,	
113,0,	113,0,	0,0,	114,0,	
113,0,	114,0,	0,0,	0,0,	
116,0,	0,0,	116,0,	114,0,	
114,0,	0,0,	0,0,	114,0,	
116,0,	116,0,	0,0,	113,0,	
116,0,	0,0,	0,0,	0,0,	
117,0,	117,0,	0,0,	0,0,	
117,0,	0,0,	114,0,	0,0,	
0,0,	0,0,	0,0,	116,0,	
0,0,	118,0,	118,0,	0,0,	
0,0,	118,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	117,0,	
0,0,	117,0,	0,0,	0,0,	
0,0,	0,0,	112,118,	117,0,	
117,0,	119,0,	119,0,	117,0,	
118,0,	119,0,	118,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
118,0,	118,0,	0,0,	113,119,	
118,0,	0,0,	117,0,	0,0,	
0,0,	120,0,	120,0,	0,0,	
119,0,	120,0,	119,0,	0,0,	
0,0,	0,0,	0,0,	118,0,	
119,0,	119,0,	0,0,	0,0,	
119,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
120,0,	0,0,	120,0,	0,0,	
0,0,	0,0,	0,0,	119,0,	
120,0,	120,0,	0,0,	0,0,	
120,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	120,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	119,120,	0,0,	
0,0};
struct yysvf yysvec[] = {
0,	0,	0,
yycrank+0,	0,		yyvstop+1,
yycrank+0,	0,		yyvstop+3,
yycrank+-1,	0,		yyvstop+5,
yycrank+-48,	yysvec+3,	yyvstop+7,
yycrank+-96,	0,		yyvstop+9,
yycrank+-10,	yysvec+5,	yyvstop+11,
yycrank+-151,	0,		yyvstop+13,
yycrank+0,	0,		yyvstop+15,
yycrank+0,	0,		yyvstop+17,
yycrank+0,	0,		yyvstop+19,
yycrank+0,	0,		yyvstop+21,
yycrank+0,	0,		yyvstop+23,
yycrank+-168,	yysvec+7,	yyvstop+25,
yycrank+-210,	yysvec+7,	yyvstop+27,
yycrank+-223,	yysvec+7,	yyvstop+29,
yycrank+-239,	yysvec+7,	yyvstop+31,
yycrank+0,	0,		yyvstop+34,
yycrank+-265,	yysvec+7,	yyvstop+36,
yycrank+-279,	yysvec+7,	yyvstop+39,
yycrank+-305,	yysvec+7,	yyvstop+41,
yycrank+-316,	yysvec+7,	yyvstop+43,
yycrank+-321,	yysvec+7,	yyvstop+45,
yycrank+-359,	yysvec+7,	yyvstop+47,
yycrank+-364,	yysvec+7,	yyvstop+49,
yycrank+-400,	yysvec+7,	yyvstop+51,
yycrank+-429,	yysvec+7,	yyvstop+53,
yycrank+-442,	yysvec+7,	yyvstop+55,
yycrank+-455,	yysvec+7,	yyvstop+57,
yycrank+-468,	yysvec+7,	yyvstop+59,
yycrank+-481,	yysvec+7,	yyvstop+61,
yycrank+-497,	yysvec+7,	yyvstop+63,
yycrank+-541,	yysvec+7,	yyvstop+65,
yycrank+0,	0,		yyvstop+67,
yycrank+0,	0,		yyvstop+69,
yycrank+-535,	0,		yyvstop+72,
yycrank+-555,	yysvec+7,	yyvstop+74,
yycrank+-597,	0,		yyvstop+77,
yycrank+-623,	0,		yyvstop+79,
yycrank+-628,	yysvec+7,	yyvstop+82,
yycrank+-682,	yysvec+7,	yyvstop+85,
yycrank+-695,	yysvec+7,	yyvstop+87,
yycrank+-708,	yysvec+7,	yyvstop+89,
yycrank+-721,	yysvec+7,	yyvstop+91,
yycrank+-734,	yysvec+7,	yyvstop+93,
yycrank+-760,	yysvec+7,	yyvstop+95,
yycrank+-773,	yysvec+7,	yyvstop+97,
yycrank+-799,	yysvec+7,	yyvstop+99,
yycrank+-812,	yysvec+7,	yyvstop+101,
yycrank+-828,	yysvec+7,	yyvstop+103,
yycrank+-864,	yysvec+7,	yyvstop+106,
yycrank+-869,	yysvec+7,	yyvstop+108,
yycrank+-902,	yysvec+7,	yyvstop+110,
yycrank+-916,	yysvec+7,	yyvstop+112,
yycrank+-942,	yysvec+7,	yyvstop+114,
yycrank+-955,	yysvec+7,	yyvstop+116,
yycrank+-971,	yysvec+7,	yyvstop+118,
yycrank+-984,	yysvec+7,	yyvstop+120,
yycrank+0,	0,		yyvstop+122,
yycrank+-1007,	yysvec+37,	0,	
yycrank+-11,	yysvec+37,	yyvstop+124,
yycrank+-55,	yysvec+38,	yyvstop+126,
yycrank+0,	0,		yyvstop+128,
yycrank+-1013,	yysvec+7,	yyvstop+130,
yycrank+-1027,	yysvec+7,	yyvstop+132,
yycrank+-1079,	yysvec+7,	yyvstop+134,
yycrank+-1090,	yysvec+7,	yyvstop+136,
yycrank+-1116,	yysvec+7,	yyvstop+138,
yycrank+-1127,	yysvec+7,	yyvstop+140,
yycrank+-1132,	yysvec+7,	yyvstop+143,
yycrank+-1171,	yysvec+7,	yyvstop+145,
yycrank+-1184,	yysvec+7,	yyvstop+148,
yycrank+-1197,	yysvec+7,	yyvstop+151,
yycrank+-1210,	yysvec+7,	yyvstop+154,
yycrank+-1223,	yysvec+7,	yyvstop+156,
yycrank+-1236,	yysvec+7,	yyvstop+159,
yycrank+-1249,	yysvec+7,	yyvstop+161,
yycrank+-1262,	yysvec+7,	yyvstop+163,
yycrank+-1275,	yysvec+7,	yyvstop+165,
yycrank+-1291,	yysvec+7,	yyvstop+167,
yycrank+-1320,	yysvec+7,	yyvstop+169,
yycrank+-1333,	yysvec+7,	yyvstop+171,
yycrank+-1347,	yysvec+37,	0,	
yycrank+-2,	yysvec+37,	yyvstop+173,
yycrank+-1375,	yysvec+7,	yyvstop+176,
yycrank+-1413,	yysvec+7,	yyvstop+178,
yycrank+-1426,	yysvec+7,	yyvstop+181,
yycrank+-1439,	yysvec+7,	yyvstop+183,
yycrank+-1452,	yysvec+7,	yyvstop+185,
yycrank+-1478,	yysvec+7,	yyvstop+187,
yycrank+-1491,	yysvec+7,	yyvstop+189,
yycrank+-1504,	yysvec+7,	yyvstop+191,
yycrank+-1517,	yysvec+7,	yyvstop+193,
yycrank+-1530,	yysvec+7,	yyvstop+196,
yycrank+-1543,	yysvec+7,	yyvstop+198,
yycrank+-1556,	yysvec+7,	yyvstop+200,
yycrank+-1572,	yysvec+7,	yyvstop+202,
yycrank+-1585,	yysvec+7,	yyvstop+204,
yycrank+-1598,	yysvec+37,	yyvstop+206,
yycrank+-1601,	yysvec+7,	yyvstop+208,
yycrank+-1645,	yysvec+7,	yyvstop+210,
yycrank+-1678,	yysvec+7,	yyvstop+213,
yycrank+-1683,	yysvec+7,	yyvstop+215,
yycrank+-1716,	yysvec+7,	yyvstop+218,
yycrank+-1721,	yysvec+7,	yyvstop+221,
yycrank+-1754,	yysvec+7,	yyvstop+224,
yycrank+-1759,	yysvec+7,	yyvstop+227,
yycrank+-1792,	yysvec+7,	yyvstop+230,
yycrank+-1797,	yysvec+7,	yyvstop+233,
yycrank+-1830,	yysvec+7,	yyvstop+235,
yycrank+-1835,	yysvec+7,	yyvstop+237,
yycrank+-1868,	yysvec+7,	yyvstop+240,
yycrank+-1882,	yysvec+7,	yyvstop+242,
yycrank+-1908,	yysvec+7,	yyvstop+244,
yycrank+-1919,	yysvec+7,	yyvstop+246,
yycrank+0,	0,		yyvstop+249,
yycrank+-1924,	yysvec+7,	yyvstop+251,
yycrank+-1963,	yysvec+7,	yyvstop+254,
yycrank+-1976,	yysvec+7,	yyvstop+257,
yycrank+-1996,	yysvec+7,	yyvstop+260,
yycrank+-2016,	yysvec+7,	yyvstop+262,
0,	0,	0};
struct yywork *yytop = yycrank+2110;
struct yysvf *yybgin = yysvec+1;
char yymatch[] = {
00  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,011 ,012 ,01  ,01  ,011 ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
011 ,01  ,'"' ,01  ,01  ,01  ,01  ,01  ,
'"' ,'"' ,01  ,01  ,'"' ,01  ,01  ,01  ,
'0' ,'0' ,'0' ,'0' ,'0' ,'0' ,'0' ,'0' ,
'0' ,'0' ,01  ,'"' ,01  ,01  ,01  ,01  ,
01  ,01  ,'B' ,'C' ,01  ,01  ,'F' ,01  ,
01  ,'I' ,01  ,01  ,01  ,01  ,'N' ,01  ,
'P' ,01  ,01  ,'S' ,'T' ,01  ,'V' ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,'B' ,'C' ,01  ,01  ,'F' ,01  ,
01  ,'I' ,01  ,01  ,01  ,01  ,'N' ,01  ,
'P' ,01  ,01  ,'S' ,'T' ,01  ,'V' ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
0};
char yyextra[] = {
0,1,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,1,
0,0,0,0,0,0,0,0,
0};
#ifndef lint
static	char ncform_sccsid[] = "@(#)ncform 1.6 88/02/08 SMI"; /* from S5R2 1.2 */
#endif

int yylineno =1;
# define YYU(x) x
# define NLSTATE yyprevious=YYNEWLINE
char yytext[YYLMAX];
struct yysvf *yylstate [YYLMAX], **yylsp, **yyolsp;
char yysbuf[YYLMAX];
char *yysptr = yysbuf;
int *yyfnd;
extern struct yysvf *yyestate;
int yyprevious = YYNEWLINE;
yylook(){
	register struct yysvf *yystate, **lsp;
	register struct yywork *yyt;
	struct yysvf *yyz;
	int yych, yyfirst;
	struct yywork *yyr;
# ifdef LEXDEBUG
	int debug;
# endif
	char *yylastch;
	/* start off machines */
# ifdef LEXDEBUG
	debug = 0;
# endif
	yyfirst=1;
	if (!yymorfg)
		yylastch = yytext;
	else {
		yymorfg=0;
		yylastch = yytext+yyleng;
		}
	for(;;){
		lsp = yylstate;
		yyestate = yystate = yybgin;
		if (yyprevious==YYNEWLINE) yystate++;
		for (;;){
# ifdef LEXDEBUG
			if(debug)fprintf(yyout,"state %d\n",yystate-yysvec-1);
# endif
			yyt = yystate->yystoff;
			if(yyt == yycrank && !yyfirst){  /* may not be any transitions */
				yyz = yystate->yyother;
				if(yyz == 0)break;
				if(yyz->yystoff == yycrank)break;
				}
			*yylastch++ = yych = input();
			yyfirst=0;
		tryagain:
# ifdef LEXDEBUG
			if(debug){
				fprintf(yyout,"char ");
				allprint(yych);
				putchar('\n');
				}
# endif
			yyr = yyt;
			if ( (int)yyt > (int)yycrank){
				yyt = yyr + yych;
				if (yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transitions */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					goto contin;
					}
				}
# ifdef YYOPTIM
			else if((int)yyt < (int)yycrank) {		/* r < yycrank */
				yyt = yyr = yycrank+(yycrank-yyt);
# ifdef LEXDEBUG
				if(debug)fprintf(yyout,"compressed state\n");
# endif
				yyt = yyt + yych;
				if(yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transitions */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					goto contin;
					}
				yyt = yyr + YYU(yymatch[yych]);
# ifdef LEXDEBUG
				if(debug){
					fprintf(yyout,"try fall back character ");
					allprint(YYU(yymatch[yych]));
					putchar('\n');
					}
# endif
				if(yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transition */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					goto contin;
					}
				}
			if ((yystate = yystate->yyother) && (yyt= yystate->yystoff) != yycrank){
# ifdef LEXDEBUG
				if(debug)fprintf(yyout,"fall back to state %d\n",yystate-yysvec-1);
# endif
				goto tryagain;
				}
# endif
			else
				{unput(*--yylastch);break;}
		contin:
# ifdef LEXDEBUG
			if(debug){
				fprintf(yyout,"state %d char ",yystate-yysvec-1);
				allprint(yych);
				putchar('\n');
				}
# endif
			;
			}
# ifdef LEXDEBUG
		if(debug){
			fprintf(yyout,"stopped at %d with ",*(lsp-1)-yysvec-1);
			allprint(yych);
			putchar('\n');
			}
# endif
		while (lsp-- > yylstate){
			*yylastch-- = 0;
			if (*lsp != 0 && (yyfnd= (*lsp)->yystops) && *yyfnd > 0){
				yyolsp = lsp;
				if(yyextra[*yyfnd]){		/* must backup */
					while(yyback((*lsp)->yystops,-*yyfnd) != 1 && lsp > yylstate){
						lsp--;
						unput(*yylastch--);
						}
					}
				yyprevious = YYU(*yylastch);
				yylsp = lsp;
				yyleng = yylastch-yytext+1;
				yytext[yyleng] = 0;
# ifdef LEXDEBUG
				if(debug){
					fprintf(yyout,"\nmatch ");
					sprint(yytext);
					fprintf(yyout," action %d\n",*yyfnd);
					}
# endif
				return(*yyfnd++);
				}
			unput(*yylastch);
			}
		if (yytext[0] == 0  /* && feof(yyin) */)
			{
			yysptr=yysbuf;
			return(0);
			}
		yyprevious = yytext[0] = input();
		if (yyprevious>0)
			output(yyprevious);
		yylastch=yytext;
# ifdef LEXDEBUG
		if(debug)putchar('\n');
# endif
		}
	}
yyback(p, m)
	int *p;
{
if (p==0) return(0);
while (*p)
	{
	if (*p++ == m)
		return(1);
	}
return(0);
}
	/* the following are only used in the lex library */
yyinput(){
	return(input());
	}
yyoutput(c)
  int c; {
	output(c);
	}
yyunput(c)
   int c; {
	unput(c);
	}
