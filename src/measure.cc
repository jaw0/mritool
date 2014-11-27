

#ifdef RCSID
static const char *const rcsid
	= "@(#)$Id: measure.cc,v 1.1 1995/11/23 04:30:16 jaw Exp jaw $";
#endif

#include <mritool.h>
#include <window.h>

#include <jl_proto.h>
#include <defproto.h>

extern "C" void meas_button_push(Xv_opaque, Event*);
extern char *button_labels(int i);
Obj buttons_sym = 0;

void XView::create_meas_popup(void){
	
        int i,across,down,x,y, nbuttons;
        Xv_opaque panel;
	Obj foo;
	
        // across = config.across;
	foo = getvalue(maksym("mri:across"));
	if(! INUMP(foo))
		return;
	across = CINT(foo);

	if(! buttons_sym){
		buttons_sym = maksym("mri:buttons");
		internal_gc_protect = Fcons( buttons_sym, internal_gc_protect);
	}
	foo = getvalue(buttons_sym);
	if( NCONSP(foo))
		return;
	nbuttons = CINT( Flength(foo));	
	
	down = nbuttons/across + ((nbuttons%across)?1:0);
 
	meas_popup= xv_create(main_frame,FRAME_CMD,
                XV_KEY_DATA, KEY_THIS, (Xv_opaque)(void*)this,
		XV_LABEL,"Selection Window",
                FRAME_SHOW_RESIZE_CORNER,FALSE,
                FRAME_CMD_PUSHPIN_IN,TRUE,
                XV_WIDTH,across*DX+2*X0,
                XV_HEIGHT,down*DY/2+Y0,
		XV_X, CINT(getvalue(maksym_c("mri:slt_x"))),
		XV_Y, CINT(getvalue(maksym_c("mri:slt_y"))),
                0);
 
        panel = xv_get(meas_popup, FRAME_CMD_PANEL);

	mbuttons = new Button_Labels [ CINT( Flength(foo)) ];
	
        for(i=0;i<nbuttons;i++){
                y=i/across;
                x=i%across;
		
                mbuttons[i].button_i = xv_create(panel,PANEL_BUTTON,
			XV_KEY_DATA, KEY_THIS, (Xv_opaque)(void*)this,
			XV_KEY_DATA, KEY_BUTN, i, 
			PANEL_LABEL_STRING, CCHARS( Fnth(foo, MAKINT(i))), 
                        PANEL_NOTIFY_PROC, meas_button_push,
                        PANEL_LABEL_WIDTH,BUTT_WIDTH,
                        PANEL_FONT,FONT_FAMILY_DEFAULT_FIXEDWIDTH,
                        XV_SHOW,TRUE,
                        XV_X, x*DX+X0,
                        XV_Y, y*DY/2+Y0,
                        0);
		
        }
}

extern "C" void meas_button_push(Xv_opaque button, Event *event){
	int bn;
	XView& m = me(button);

	bn = (int)xv_get(button, XV_KEY_DATA, KEY_BUTN);
	
	// G.mouse_mode = M_NXT_SNK;		
	m.c_image->selected = bn;
	Fset(maksym_c("mouse:selected"), MAKINT(bn));
	Fset(maksym_c("mouse:mode"),
	     Fcons(sym_quote, Fcons( maksym_c("mouse:NXTSNK"), IC_NIL)));
	Ffuncall( Fcons(maksym("mouse:handler"),
		Fcons(makvect(8), IC_NIL)));
	
}




	
