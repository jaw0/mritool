
// Copyright (c) 1994 Jeff Weisberg
// University of Rochester
//      Dept. of Electrical Engineering
//      Dept. of Neurology, C.E.P.
//
// see the file "License"
 
 
#ifdef RCSID
static const char *const rcsid
= "@(#)$Id: cli-popup.cc,v 1.1 1995/11/23 04:30:06 jaw Exp jaw $";
#endif
 
// Optimization: None

#include <jlisp.h>
#include <jl_proto.h>
#include <defproto.h>
 
#include <mritool.h>
#include <solid.h>
#include <mrilisp.h>
 
extern UserI *ui_jl;
extern "C" void interp_from_panel(Xv_opaque it);
static int indent_level = 0;

#define CODE_SECTION SECTION_WINSYS

#define SHOW_POPUP( popup )					\
 	XView *foo = ui_jl ? ui_jl->get_xv() : 0;		\
 	if( foo){						\
		xv_set( foo-> popup , XV_SHOW, TRUE, 0);	\
		return IC_TRUE;					\
	}else							\
 		return IC_FALSE;					


extern "C" void button_press_handler(Xv_opaque, Xv_opaque it){
	// entry point from a button push

	Obj fnc = (Obj)xv_get(it, XV_KEY_DATA, KEY_INTRP);
	UserI *foo = ui_jl;
	
	if( &me(it) ) UI = ui_jl = & (me(it));
	
	debugp( DBG_ECHO ){
		cerr << "button press: ";
		Fdisplay(fnc, stderr_port);
		cerr << endl;
	}
	if( fnc)
		if( STRINGP( fnc ))
			me(it).interpret( CCHARS(fnc) );
		else
			Fcall( fnc );
	
	if( foo) UI = ui_jl = foo;
}	

DEFUN("show-measure-popup", Fmeasure_popup, Smeasure_popup, 0,0,1,0,
      "(show-measure-popup) popup measure window",
      ())
{
	SHOW_POPUP( meas_popup );
}

DEFUN("show-segm-popup", Fsegm_popup, Ssegm_popup, 0,0,1,0,
      "(show-segm-popup) popup the segm window",
      ())
{
	SHOW_POPUP( segm_popup );
}

DEFUN("show-snake-popup", Fsnake_popup, Ssnake_popup, 0,0,1,0,
      "(show-snake-popup) popup snake window",
      ())
{
	SHOW_POPUP( snak_popup );
}

DEFUN("show-bounds-popup", Fbounds_popup, Sbounds_popup, 0,0,1,0,
      "(show-bounds-popup) popup vounds window",
      ())
{
	SHOW_POPUP( snak_bounds_popup );
}

DEFUN("show-patient-popup", Fpatient_popup, Spatient_popup, 0,0,1,0,
      "(show-patient-popup) popup patient list",
      ())
{
	SHOW_POPUP( patient_popup );
}

DEFUN("show-image-popup", Fimage_popup, Simage_popup, 0,0,1,0,
      "(show-image-popup) popup image list",
      ())
{
	if(! ui_jl)
		return jlerror(Simage_popup.name, IC_NIL, "no ui");

	ui_jl->c_series->menu(*ui_jl->get_xv());
	return IC_UNSPEC;
}

DEFUN("show-help-popup", Fhelp_popup, Shelp_popup, 0,0,1,0,
      "(show-help-popup) popup the help window",
      ())
{
	SHOW_POPUP( help_popup );
}

DEFUN("show-license-popup", Flic_popup, Slic_popup, 0,0,1,0,
      "(show-license-popup) popup the license window",
      ())
{
	SHOW_POPUP( lic_popup );
}

DEFUN("show-graph-popup", Fgraph_popup, Sgraph_popup, 0,0,1,0,
      "(show-graph-popup) popup the graph window",
      ())
{
	SHOW_POPUP( histo_popup );
}

DEFUN("show-lut", Fshow_lut, Sshow_lut, 0,1,1,0,
      "(show-lut [u]) show the color lookup table",
      (Obj u))
{
	UserI *ui;
	XView *foo;
	
	if( UI_P(u)) ui = CUI(u);
	else ui = ui_jl;

	if(! ui)
		return jlerror(Sshow_lut.name, IC_NIL, "no ui");
	foo = ui->get_xv();

	if(foo)
		view_pal(foo->main_frame);

	return IC_UNSPEC;
}
	

static char* indent_it(){
	static char foo[32];
	int i;

	for(i=0; i<indent_level; i++)
		foo[i] = '\t';
	foo[i] = 0;
	return foo;
}


#if 1

Xv_opaque create_menu_buttons(XView * self, Obj list, Xv_opaque par ){
	Xv_opaque menu = xv_create(par, MENU,
		XV_KEY_DATA, KEY_THIS, (Xv_opaque)(void*)self,
		MENU_RELEASE_IMAGE, 
		0
	);

	while( CONSP(list)){
		Obj l = CAR(list);
		Obj label = CAR(l);
		Obj more = CADR(l);
		Xv_opaque mi;

		if(! STRINGP(label)){
			// skip over it
			list = CDR(list);
			continue;
		}
		
		char *labstr = (char*)malloc(int( CLENGTH(label) + 1));
		strncpy(labstr, CCHARS(label), int(CLENGTH(label)+1) );

		while( SYMBOLP(more) || SYMBOXP(more))
			more = getvalue(more);
		
		if( CONSP(more)){
			// create a pullright
			
			debugp( DBG_VERBOSE )
				cerr << indent_it() << "Pull Right: " << labstr << endl;

			indent_level ++;
			mi = xv_create(menu, MENUITEM,
				XV_KEY_DATA, KEY_THIS, (Xv_opaque)(void*)self,
				MENU_RELEASE_IMAGE,
				MENU_PULLRIGHT_ITEM, labstr, create_menu_buttons(self, more, menu),
				0
			);
			
			indent_level --;
			
		}else if( STRINGP(more) || SYMBOLP(more) || CCODEP(more) || FUNCTIONP(more) || MACROP(more)){
			// create a button
			//char *intstr = (char*)malloc(int( CLENGTH(more)  + 1));
			//strncpy(intstr, CCHARS(more),  int(CLENGTH(more) +1) );

			// internal_gc_protect = Fcons( more, internal_gc_protect );
			PROTECT_SYM( more );
			
			mi = xv_create(menu, MENUITEM,
				XV_KEY_DATA, KEY_THIS, (Xv_opaque)(void*)self,
				MENU_STRING, labstr,
				MENU_NOTIFY_PROC, button_press_handler,
				XV_KEY_DATA, KEY_INTRP, more,
				MENU_RELEASE_IMAGE,
				0
			);
			
			//debugp( DBG_VERBOSE )
			//	cerr << indent_it() << "Button: "
			//	     << labstr << ", " << intstr << endl;
			
		}else{
			// nop button...
			mi = xv_create(menu, MENUITEM,
				XV_KEY_DATA, KEY_THIS, (Xv_opaque)(void*)self,
				MENU_STRING, labstr,
				MENU_NOTIFY_PROC, button_press_handler,
				XV_KEY_DATA, KEY_INTRP, 0,
				MENU_RELEASE_IMAGE,
				0
			);
			
			debugp( DBG_VERBOSE )
				cerr << indent_it() << "Button: "
				     << labstr << endl;

		}
		xv_set(menu, MENU_APPEND_ITEM, mi, 0);
		list = CDR(list);
	}
	return menu;
}


void XView::create_panel(){

	int x=30, y=3, ix=100;

	Obj toprow = getvalue(maksym("mri:top-row"));

	panel = xv_create(main_frame,PANEL,
		XV_X,0,
		XV_Y,0,
		XV_HEIGHT,25,
		0);

	interp_popup = xv_create(main_frame,PANEL,
		XV_KEY_DATA, KEY_THIS, (Xv_opaque)(void*)this,
		XV_X,0,
		XV_HEIGHT,20,
		XV_WIDTH, SCREENX, 
		0);

	interp_txt_i = xv_create(interp_popup, PANEL_TEXT,
		PANEL_VALUE_DISPLAY_LENGTH,55,
		PANEL_VALUE_STORED_LENGTH,4096,
		PANEL_LABEL_STRING, "? ",
		XV_KEY_DATA, KEY_THIS, (Xv_opaque)(void*)this,
		XV_SHOW, TRUE,
		PANEL_NOTIFY_PROC, interp_from_panel,
		PANEL_NOTIFY_LEVEL, PANEL_SPECIFIED,
		PANEL_NOTIFY_STRING, "\r\n",
		0);

	mouse_menu = create_menu_buttons(this, getvalue(maksym_c("mouse:menu")), 0);
#if 0
	
	mouse_menu = xv_create(0,MENU,		/* if these strings are changed they MUST also
						   be changed in mouse.cc */
		XV_KEY_DATA, KEY_THIS, (Xv_opaque)(void*)this,			       
		MENU_STRINGS,"Speed Mode",
		"Draw",
		"Erase",
		"Draw ROI",
		"Adjst W/L",
		"Snake LOI",	       
		0,
		MENU_NOTIFY_PROC, alt_mouse_selected,
		0);
#endif	
	
	while( CONSP(toprow)){
		Obj button = CAR(toprow);
		Obj label = CAR(button);
		Obj more = CADR(button);
		Xv_opaque pb;
		
		if( STRINGP( label )){
			char *labstr = (char*)malloc(int( CLENGTH(label) + 1));
			strncpy(labstr, CCHARS(label), int(CLENGTH(label) + 1));

			debugp( DBG_VERBOSE )
				cerr << "Top Row: " << labstr << endl;
			indent_level ++;

			if( CONSP(more)){
				
				pb = xv_create(panel, PANEL_BUTTON,
					XV_X, x,
					XV_Y, y,
					XV_SHOW, TRUE,
					XV_KEY_DATA, KEY_THIS, (Xv_opaque)(void*)this,
					PANEL_LABEL_STRING, labstr,
					PANEL_ITEM_MENU, create_menu_buttons(this, more, 0),
					0
				);
			}else{
				// nop button...
				xv_create(panel, PANEL_BUTTON,
					XV_X, x,
					XV_Y, y,
					XV_SHOW, TRUE,
					XV_KEY_DATA, KEY_THIS, (Xv_opaque)(void*)this,
					PANEL_LABEL_STRING, labstr,
					0
				);
			}
			
			indent_level --;
			x += ix;
		}
		toprow = CDR(toprow);
	}
}
		   

#endif

#if 0
DEFUN("xv_create", Fxvcreate, Sxvcreate, 1,1,1,1,
      "(xv_create ui parent ...)",
      (Obj l))
{
	Obj u, par;
	int ll = Flength(l);
	
	if(ll < 3)
		return jlerror(Sxvcreate.name, l, "bad");
	u = CAR(l);
	par = CADR(l);

	if(! UIP(u))
		return jlerror(Sxvcreate.name, u, "WTA: ui");
	if(! XVOPAQUEP(par))
		return jlerror(Sxvcreate.name, par, "WTA: xv");

	
	
#endif
