

// Copyright (c) 1993 Jeff Weisberg
// University of Rochester
//      Dept. of Electrical Engineering
//      Dept. of Neurology, C.E.P.
//
// see the file "License"

#ifdef RCSID
static const char *const rcsid
= "@(#)$Id: interp.cc,v 1.1 1995/11/23 04:30:16 jaw Exp jaw $";
#endif

// Optimization: -O2

#include <mritool.h>
#include <window.h>

#include <jl_proto.h>
#include <defproto.h>

// char *yy_toparse=0;
extern void interpret_lisp(UserI*, char*);


char prev_cmd[4096];
void UserI::interpret(char *text){

	if( text[0] == '('){
		strcpy(prev_cmd, text);
		interpret_lisp(this, text);

	}else if( text[0] == '!'){
		if( text[1] == '!')
			interpret_lisp(this, prev_cmd);
	}
	// ...
	
}

/*
  variables interpolated in XView::interpret string:

  [LHRABGDSFNabgds*]

      $L	low end of range
      $H	high end of range
      $R	the full range: "$L  $H"
      $A	snake param elasticity
      $a	default from config
      $B	snake param attractivity
      $b	default from config
      $G	snake param stiffness
      $g	default from config
      $D	snake param 3D constraint
      $d	default from config
      $S	snake param sameness
      $s	default from config
      $F	snake param from contour
      $N	segm param number of levels
      
      $*	$a $b $g $d $s

*/

void XView::interpret(char *text){
	static char buffer[4096];
	char *s=text,*d=buffer;

	// thinking();
	while(*s){
		if(*s!='$') *d++ = *s++;
		else{
			switch(*++s){

			  case 'L':	// range lo
				sprintf(d, " %d ", (int)xv_get(snak_bounds_l_i, PANEL_VALUE, 0));
				break;
			  case 'H':	// range hi
				sprintf(d, " %d ", (int)xv_get(snak_bounds_h_i, PANEL_VALUE, 0));
				break;
			  case 'R':	// range lo to hi
				sprintf(d, " %d  %d ",
					(int)xv_get(snak_bounds_l_i, PANEL_VALUE, 0),
					(int)xv_get(snak_bounds_h_i, PANEL_VALUE, 0));
				break;
			  case 'A':	// sanke elastic
				sprintf(d, " %d ", (int)xv_get(snak_theta_i, PANEL_VALUE, 0));
				break;
			  case 'B':	// snake attacrtive
				sprintf(d, " %d ", (int)xv_get(snak_rho_i, PANEL_VALUE, 0));
				break;
			  case 'G':	// snake stiff
				sprintf(d, " %d ", (int)xv_get(snak_gamma_i, PANEL_VALUE, 0));
				break;
			  case 'D':	// snake 3d
				sprintf(d, " %d ", (int)xv_get(snak_delta_i, PANEL_VALUE, 0));
				break;
			  case 'S':	// snake simil
				sprintf(d, " %d ", (int)xv_get(snak_nloi_i, PANEL_VALUE, 0));
				break;
			  case 'a':
				sprintf(d, " %s ", "mri:elastic");
				break;
			  case 'b':
				sprintf(d, " %s ", "mri:attractive");
				break;
			  case 'g':
				sprintf(d, " %s ", "mri:stiffness");
				break;
			  case 'd':
				sprintf(d, " %s ", "mri:3dconstraint");
				break;
			  case 's':
				sprintf(d, " %s ", "mri:similarity");
				break;
			  case '*':
				sprintf(d, " %s %s %s %s %s ",
					"mri:elastic",
					"mri:attractive",
					"mri:stiffness",
					"mri:3dconstraint",
					"mri:similarity"
				);
				
				break;
			  case 'F': {	// snake from
				  int f = (int)xv_get(snak_from_i, PANEL_VALUE, 0);
				  sprintf(d, " %d ", f);
				  
				  /* sprintf(d, " %s ",
					  f==0 ? "drawn"
					  : f==1 ? "prev"
					  : f==2 ? "next"
					  : "*ERROR*"
				  ); */
				  
				  break;
			  }
			  case 'N':	// segment n-levels
				sprintf(d, " %d ", (int)xv_get(segm_nlev_i, PANEL_VALUE, 0));
				break;

			}
			s++;
			while(*++d);
		} // else
		*d = 0;
	} // while
	*d = 0;
	UserI::interpret(buffer);
	//done_thinking();
}


	
