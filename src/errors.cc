

#ifdef RCSID
static const char *const rcs_id
= "@(#)$Id: errors.cc,v 1.1 1995/11/23 04:30:14 jaw Exp jaw $";
#endif
	
// Optimization: -O2

#include <mritool.h>
#include <window.h>
#include <iostream.h>

void bug(const char *msg){

	if(!UI){
		cerr << "internal error" << msg << "send in bug report" << endl;
	}else{
		UI->error("internal error", msg, "send in bug report" );
	}
	sleep(5);

}

void bug(const char *msga, const char* msgb){

	if(!UI){
		cerr << "internal error" << msga << msgb << "send in bug report" << endl;
	}else{
		UI->error("internal error", msga, msgb, "send in bug report" );
	}
	sleep(5);

}

void warn(const char* msg){
	cerr << "Warning: " << msg << endl;
}

void XView::error(const char *a, const char *b, const char *c, const char *d,
		  const char *e, const char *f, const char *g) const {


	notice_prompt(main_frame, 0,
		      NOTICE_BUTTON_YES, "OK",
		      NOTICE_MESSAGE_STRINGS,
		      "Error:",
		      a, b, c, d, e, f, g,
		      0
		      );
}

void UserI::error(const char *a, const char *b, const char *c, const char *d,
		  const char *e, const char *f, const char *g) const {

	char *x = " ";
			   
	cerr << (a?a:x) << " " << (b?b:x) << " " << (c?c:x) << " "  << (d?d:x) << " " 
		<< (e?e:x) << " "  << (f?f:x) << " "  << (g?g:x) << endl;

}







	
