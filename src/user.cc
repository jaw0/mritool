

// Copyright (c) 1993 Jeff Weisberg
// University of Rochester
//      Dept. of Electrical Engineering
//      Dept. of Neurology, C.E.P.
//
// see the file "License"

#ifdef RCSID
static const char *const rcsid
= "@(#)$Id: user.cc,v 1.1 1995/11/23 04:30:26 jaw Exp jaw $";
#endif

#include <mritool.h>
#include <window.h>
#include <fstream.h>
#include <pstream.h>

extern "C" {
#	include <dlfcn.h>
#	include <stdio.h>	
};


void UserI::do_image(void){
	
	thinking("Loading");
	if(!c_image)
		error("Image not found");
	else{
		c_image->doit();
		c_image->update_view();
		// c_image->update_contour();
		set_contour(c_image->drawing);
		c_image->refresh(*this);
	}
	done_thinking();
}

#if 0
void UserI::parse(char* name){
	
	if(name[0]=='<' || name[0]=='|'){
		char *p;
		for(p=name+1; *p==' ' || *p=='\t'; p++);
		
		ipstream ip( mriname(p));
		parse( ip );
		
	}else{
		ifstream fin(mriname(name));
		if(!fin){
			error("Could not open file");
			return;
		}
		parse(fin);
	}
}

void UserI::parse(istream& fin){
	char buffer[1024];
	int tline = yylineno;

	yylineno = 1;
	while(fin){
		fin.getline(buffer, 1023, '\n');
		if(*buffer){
			if(yyecho) cerr << buffer << endl;
			interpret(buffer);
			*buffer = 0;
		}
		yylineno++;
	}
}

#endif

UserI::UserI(){

	// dlhand = dlopen(0,1);
	c_patient = 0;
	c_study = 0;
	c_series = 0;
	c_image = 0;
	p_sel = 0;
	
}

