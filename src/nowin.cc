

// Copyright (c) 1993 Jeff Weisberg
// University of Rochester
//      Dept. of Electrical Engineering
//      Dept. of Neurology, C.E.P.
//
// see the file "License"

#ifdef RCSID
static const char *const rcsid
= "@(#)$Id: nowin.cc,v 1.1 1995/11/23 04:30:20 jaw Exp jaw $";
#endif

// Optimization: -O2

 
#include <mritool.h>
#include <window.h>


extern Obj buttons_sym;

void UserI::show_loi(void) const {
	int i;

	cout << "\tLOI\tValue" << endl;
	for(i=0;i<c_image->num_of_snakes;i++){
		/* if(c_image && i==c_image->selected)
			cout << "=>"; */
		cout << "\t" << i << "\t" << c_image->snake[i].loi;
		/* if(c_image && i==c_image->selected)
			cout << "\t<="; */
		cout << endl;
	}
}

void NoWin::thinking(void) const {
	thinking("Thinking...");
}

void NoWin::thinking(char *msg) const {
	cerr << msg << endl;
}

NoWin::NoWin(){
	int i;

	drawing.resize();
	drawing = 0;
	ui = this;

}

void NoWin::main_loop(void){

	interpret( "(eval %repl%)" );
	interpret( "(quit)" );
}




