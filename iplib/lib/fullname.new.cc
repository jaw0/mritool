
// the *new* fullname package...
// $Id: fullname.new.cc,v 1.1 1995/11/23 04:31:03 jaw Exp jaw $

#include <pwd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>

#include <String.h>
#include <Integer.h>

#ifdef MRITOOL
#	include <mritool.h>
#	include <config.h>
#	include <window.h>
#endif

/*
  of form =X:Y:Z/foo%pbar
          =X/
	  =X
	  =/
  
*/

#define II	"/home/images"
#define x(a, b)	name.gsub(Regex(a), b)

const char* fullname(const char* file){
	static int seqno = 0;
	String name(file);
	String foo("/");

	name.gsub(Regex("^~"), "=");

	// need to add ~user here
	if( name.contains(Regex("^=[^:/]+/"))){
		String user = name;
		user.gsub(Regex("^="), "");
		user.gsub(Regex("/.*"), "");
		name.gsub(user, String(getpwnam(user)->pw_dir));
	}
	
	x( ":r", "/mri");
	x( ":d", "/mri/debug");
	x( ":g", "/gray.test.ras");
	x( ":c", "/color.test.ras");
	x( ":m", "/misc");
	x( ":h", "/mri/hippo_test_1");
	
	x( "^=w", II "3/weisberg");
	x( "^=s", II "/standard");
	x( "^=c", II "/cchen");
	x( "^=r", II "/riek");
	x( "^=b", II "/berg");
	
	  // x( "^=S", "/usr/ceas/common/raster/SunRas");
	  // x( "^=R", "/usr/ceas/common/raster");
	x( "^=G", "/usr/ceas/common/raster/GIF");
	x( "^=k", "/home/knuth/weisberg/src/tool/masks");
	x( "^=i", "/home/orion/weisberg/images");

	x( "%p", Itoa( getpid() ));
	x( "%u", Itoa( getuid() ));
	x( "%s", Itoa( seqno++, 16));

#ifdef MRITOOL
	if(UI && UI->c_image){
		x( "%P", Itoa( UI->c_patient->num));
		x( "%T", Itoa( UI->c_study->num));
		x( "%S", Itoa( UI->c_series->num));
		x( "%I", Itoa( UI->c_image->num));
	
		x( "^=S", config.savedir);
		x( "^=R", config.rawdir );
		x( "^=P", config.rawdir );

		x( ":P", "/:P");
		x( ":T", "/:T");
		x( ":S", "/:S");
		x( ":I", "/:I");
		x( ":P", UI->c_patient->dirname );
		x( ":T", UI->c_study->dirname );
		x( ":S", UI->c_series->dirname );
		x( ":I", UI->c_image->dirname );
	}
#endif	

	x( "^=", (char*)getenv("HOME"));
	
	return (const char*) name;
}

#ifdef EXPNAME

main(int argc, char** argv){

	if( argc != 2){
		cout << fullname("~") << flush;
	}else{
		cout << fullname( argv[1] ) << flush;
	}
	exit(0);

}

#endif


	

	
