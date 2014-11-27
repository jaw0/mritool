
#include <jlisp.h>

extern void writestr(Obj port, char* s);
extern void writechar(Obj port, int c);
extern void printnum(Obj port, int val, int bacse, int len, int zc);


DEFUN("format", Fformat, Sformat, 1,1,1,1,
      "(format port format-spec args...) print out the args according to the format-spec",
      (Obj args))
{
	Obj port, spec, arg, argl;
	
	port = CAR(args); args = CDR(args);
	spec = CAR(args); args = CDR(args);

	
	
	
