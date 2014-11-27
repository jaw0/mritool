

// Copyright (c) 1993 Jeff Weisberg
// University of Rochester
//      Dept. of Electrical Engineering
//      Dept. of Neurology, C.E.P.
//
// see the file "License"

#ifdef RCSID
static const char *const rcsid
= "@(#)$Id: fullname.mri2.cc,v 1.1 1995/11/23 04:30:15 jaw Exp jaw $";
#endif

// Optimization: Full

#include <pwd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>

#include <mritool.h>
#include <window.h>
#include <string.h>

#include <jl_proto.h>
#include <defproto.h>

#define PT( x )	  	((UI && UI->c_image)?( x ):0)

const char* mriname(const char* file){	// NOT reentrant
	char buff[4096];
	const char *p=file;
	char *b=buff;
	
	buff[0] = 0;

	while(*p){

		switch(*p){

		  case '=':
		  case '~':
			if( p==file || p[-1]==' ' || p[-1]=='\t' ){
			    
				if(p[1]=='/' || p[1]==0){
					// =/
					strcat(buff, (char*)getenv("HOME"));
				}else if(p[2]=='/' || p[2]==0 || p[2]==':'){
					// =X/
					char *e=0;
					switch(p[1]){
					  case 'w': e="/data/weisberg";	break;
					  case 's': e="/data/standard";	break;
  					  case 'd': e=CCHARS(getvalue(maksym("mri:debugdir"))); break;
					  case 'S': e=CCHARS(getvalue(maksym("mri:savedir"))); break;
					  case 'G': e=CCHARS(getvalue(maksym("mri:segmdir"))); break;
					  case 'R':
					  case 'P': e=CCHARS(getvalue(maksym("mri:patdir"))); break;
					  /*
					  case 'M': e=CCHARS(getvalue(maksym("mri:srcdir"))); break;
					  case 'L':
						strcat(buff, CCHARS(getvalue(maksym("mri:srcdir"))));
						e = "/lib";
						break;
					  */
					}
					if(e) strcat(buff, e);
					p++;
				}else{
					// =username/
					char name[32];
					int i = 0;
					for(p++;*p!='/' && i<32; i++)
						name[i] = *p++;
					struct passwd *pd = getpwnam(name);
					if(pd)
						strcat(buff, pd->pw_dir);
					p--;
				}
			}else{
				// copy the "="
				*b++ = *p;
				*b = 0;
			}
			break;
		  case ':': {
			  char *d;
			  
			  switch(*++p){
			    case 'r': d = "mri";			break;
			    case 'd': d = "mri/debug";			break;
			    case 'P': d = PT( UI->c_patient->dirname );	break;
			    case 'T': d = PT( UI->c_study->dirname );	break;
			    case 'S': d = PT( UI->c_series->dirname );	break;
			    case 'I': d = PT( UI->c_image->dirname ); 	break;
			  }
			  strcat(buff, "/");
			  strcat(buff, d);
			  break;
		  }

		  case '%': {
			  char numero[32];
			  int n;
			  const char *str=0;
			  static int seqno = 1;
			  // Gotta love these compiler ugrades...

			  switch(*++p){
			    case 'P': n = PT( UI->c_patient->num ); 	break; 
			    case 'T': n = PT( UI->c_study->num );	break; 
			    case 'S': n = PT( UI->c_series->num );	break; 
			    case 'I': n = PT( UI->c_image->num );  	break; 
			    case 'p': n = getpid();			break; 
			    case 'u': n = getuid();			break; 
			    case 's': n = seqno++;			break;
			    case 'V': str = version;			break;
			    case 'v': str = VERSION_SHRT;		break;
			    default:
				  // copy the character directly
				  *numero = *p;
				  numero[1] = 0;
				  str = numero;
				  
			  }
			  if(!str){
				  sprintf(numero, "%d", n);
				  str = numero;
			  }
			  strcat(buff, str);
			  break;
		  }
		  default:
			  *b++ = *p;
			  *b = 0;
		} // eo sw
		while(*b)b++;
		p++;
	} // eo while
	*b = 0;

	return buff;
}

	
	

		
