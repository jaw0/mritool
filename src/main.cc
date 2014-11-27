

#ifdef RCSID
static const char *const rcsid
= "@(#)$Id: main.cc,v 1.1 1995/11/23 04:30:16 jaw Exp jaw $";
#endif

// Optimization: None

#include <mritool.h>
#include <window.h>
#include <string.h>
#include <jl_proto.h>
#include <defproto.h>
#include <solid.h>
#include <mrilisp.h>
#include <fstream.h>
#include <unistd.h>
#include <new.h>

#include <rdb_client.h>

extern "C" {
	char* getenv(const char*);
};

static int bkgrnd = 1;
struct Globals G;
extern void init_mri_lisp(int, char**);
UserI *UI;
boolean debug_flags[ N_SECTIONS ][ N_FIELDS ];

extern void out_of_memory(void);

#define CODE_SECTION	SECTION_INIT

const char *ptables[][3] = {
	// default ptable entries
		{ "gz",   "gzip -c",     "gunzip -c" },
		{ "Z",    "compress -c", "uncompress -c" },
		{ "segm", "rastosegm",   "segmtoras" },
		{ 0,0,0 }
};

int main(int argc, char **argv){
        char envbuf[4096], nbuf[1024];
        char *toprs = 0, *toint = 0;
	int c = argc-1;
	int nw = 0;
	Uimage foo(1,1);			// dummy so as to create static_ptable
	{ ofstream of("/dev/null"); }		// dummy to get filebuf::openprot set right

	foo.ptable->addlist(ptables);		// set ptable to above array
	
	sprintf(envbuf, "PATH=%s:%s",		// prepend stuff to path
		QUOTIFY( SEARCHPATH ),	   	// so we can find our helper programs
		getenv("PATH")
		);
	putenv(envbuf);

	set_new_handler( out_of_memory );
	

	if( argv[0][0] == '.'){
		// if started as ./prog change to full pathname
		// so we can find the exec. after chdir()ing
		getcwd(nbuf, sizeof(nbuf));
		strcpy(nbuf+strlen(nbuf), argv[0]+1);
		argv[0] = nbuf;
	}
		// init. the lisp sub-system
	init_mri_lisp(argc, argv);	

	
 		// load patient database (and requested hooks)
	if(  getvalue(maksym_c("mri:build-pt-db"))!=IC_FALSE){
		Frunhooks(maksym_c("before-patient-db-hooks"));

		// first to connecting to rdb server
		// if not able, use local data
		if(! rdb_create_patient_db())
			create_patient_db();
		Frunhooks(maksym_c("after-patient-db-hooks"));
	}
	
		// initialize the  user-interface
	debugp( DBG_VERBOSE )
		cerr << "initializing the user interface" << endl;
	if( getvalue(maksym_c("mri:windows")) == IC_TRUE)
		xv_init(XV_INIT_ARGC_PTR_ARGV,&argc,argv,0);

	UI = CUI( Fcall(maksym_c("mri:init-frame")));

		// enter the main processing loop
	debugp( DBG_VERBOSE )
		cerr << "Entering main loop" << endl;
	Frunhooks(maksym_c("before-main-loop-hooks"));
	UI->main_loop();				

	Fquit(IC_UNSPEC);
	exit(0);	// ought not be reached
}

