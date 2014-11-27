
 
// Copyright (c) 1993 Jeff Weisberg
// University of Rochester
//      Dept. of Electrical Engineering
//      Dept. of Neurology, C.E.P.
//
// see the file "License"

#ifdef RCSID
static const char *const rcsid
="@(#)$Id: misc.cc,v 1.1 1995/11/23 04:30:17 jaw Exp jaw $";
#endif
	
#include <mritool.h>
#include <window.h>
#include <string.h>

extern "C" void quit(Xv_opaque it){
	
	// xv_destroy(me(it).main_frame);
	me(it).interpret("(quit)");
	// Not Reached
}
void null_op(void){}
void spin(){
	static int i=0;
	static char xx_foo[] = "-\\|/";

	fprintf(stderr,"\b%c",xx_foo[i]);
	i++;
	i = (i>=4)?0:i;
	// usleep(50);
}

extern "C" int main_destroy_interposer(Xv_opaque frame){
	
	static int recur = 0;

	if (recur) return 0;
	recur = 1;
	quit(frame);
}

