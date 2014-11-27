

#include <mritool.h>
#include <signal.h>
#include <sys/wait.h>

#ifdef RCSID
static const char *const rcsid
= "@($)$Id: signal.cc,v 1.1 1995/11/23 04:30:24 jaw Exp jaw $";
#endif

int got_a_signal = 0;
extern "C" void save_and_die(int, ...);

extern "C" void set_stop_flag(int, ...){
	got_a_signal = 1;
}
extern "C" void reread_config_file(int, ...){
	
}

extern "C" void the_reaper(int, ...){

	cerr << "Reaping" << endl;
	// wait4(-1, 0, WNOHANG, 0);
}
	
void install_sig_handlers(void){
	// xview does not want us to catch
	// ALARM, VTALARM, TERM, CHLD, IO, URG

	// this is now all done in lisp
	
	// signal(SIGCHLD, the_reaper);	
	
}
 

extern "C" void save_and_die(int sig, ...){
	static int recur=0;

	if(recur){
		fprintf(stderr,"ACCCK!! Caught signal (%d) in signal handler! Giving up!\n",sig);
		exit(-1);
	}
	recur = 1;
	fprintf(stderr,"Caught a signal (%d). Going down!\n", sig);
	// ...
	exit(1);
}
