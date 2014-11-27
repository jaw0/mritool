

#ifdef RCSID
static const char *const rcsid
	= "@(#)$Id: popups.cc,v 1.1 1995/11/23 04:30:22 jaw Exp jaw $";
#endif
	
#include <mritool.h>
#include <window.h>
#include <fullname.h>

#include <jl_proto.h>
#include <defproto.h>
	
extern "C" {
#	include <xview/text.h>
#	include <fcntl.h>
#	include <xview/selection.h>
#	include <xview/sel_svc.h>
#	include <xview/sel_attrs.h>
#	include <X11/X.h>
#	include <xview/defaults.h>
#	include <xview/tty.h>	
};

u_short go_but_bits[] = {
#       include "go.icon"
};
u_short snk_but_bits[] = {
#       include "snake.icon"
};

static char *ttyargv[] = { "cat", 0 };


extern "C" void interp_from_panel(Xv_opaque it){
	XView& m = me(it);
	
	Fset(maksym_c("*current-file*"), makstr_c("input from user"));
	m.interpret( (char*)xv_get(m.interp_txt_i, PANEL_VALUE, 0));
	xv_set(m.interp_txt_i, PANEL_VALUE, "", 0);
	Fset(maksym_c("*current-file*"), makstr_c("mritool internals"));
	
}

void XView::create_popups(){
	Xv_opaque ppanel, img;

	offl_msg_popup = xv_create(main_frame,FRAME_CMD,
		XV_KEY_DATA, KEY_THIS, (Xv_opaque)(void*)this,
		FRAME_SHOW_RESIZE_CORNER,FALSE,
		XV_LABEL, "Notice",
		XV_HEIGHT,80,
		XV_WIDTH,400,
		0
	);
	ppanel = xv_get(offl_msg_popup, FRAME_CMD_PANEL);
	xv_set(ppanel, PANEL_LAYOUT, PANEL_VERTICAL,0);
	xv_create(ppanel, PANEL_MESSAGE,
		XV_KEY_DATA, KEY_THIS, (Xv_opaque)(void*)this,
		PANEL_LABEL_STRING,
		"Offline segmentation complete",
		0
	);
	xv_create(ppanel, PANEL_MESSAGE,
		XV_KEY_DATA, KEY_THIS, (Xv_opaque)(void*)this,
		PANEL_LABEL_STRING,
		"Hold on while I process the new data",
		0
	);
	segm_popup = xv_create(main_frame, FRAME_CMD,
		XV_LABEL, "Segment Options",
		XV_KEY_DATA, KEY_THIS, (Xv_opaque)(void*)this,
		FRAME_SHOW_RESIZE_CORNER, FALSE,
		PANEL_LAYOUT, PANEL_VERTICAL,
		XV_X, CINT(getvalue(maksym_c("mri:sgm_x"))),
		XV_Y, CINT(getvalue(maksym_c("mri:sgm_y"))),
	        XV_HEIGHT, 150,
		XV_WIDTH, 200,
                0
	);
	ppanel = xv_get(segm_popup, FRAME_CMD_PANEL, 0);
/*
	segm_what_i = xv_create(ppanel,PANEL_CHOICE,
		XV_KEY_DATA, KEY_THIS, (Xv_opaque)(void*)this,
		PANEL_LABEL_STRING, "What? ",
		PANEL_CHOICE_STRINGS, 
			"Image", "Series", 0, 
		PANEL_VALUE, 0,
		0
	);
*/	
/*
	segm_grnd_i = xv_create(ppanel, PANEL_CHOICE,
		PANEL_LABEL_STRING, "How? ",
		XV_KEY_DATA, KEY_THIS, (Xv_opaque)(void*)this,
		PANEL_CHOICE_STRINGS,
			"Online", "Offline", 0, 
		PANEL_VALUE, 0,
		0		
	);
*/
	
	segm_nlev_i = xv_create(ppanel, PANEL_NUMERIC_TEXT,
		PANEL_LABEL_STRING, "Levels: ",
		XV_KEY_DATA, KEY_THIS, (Xv_opaque)(void*)this,
	        PANEL_VALUE, CINT(getvalue(maksym("mri:nlev"))),
		PANEL_MIN_VALUE, 1, 
		PANEL_MAX_VALUE, 64,
		0
	);
	segm_alpha_i = xv_create(ppanel, PANEL_NUMERIC_TEXT,
		PANEL_LABEL_STRING, "Alpha: ",
		XV_KEY_DATA, KEY_THIS, (Xv_opaque)(void*)this,
	        PANEL_VALUE, CINT(getvalue(maksym("mri:alpha"))),
		PANEL_MIN_VALUE, 0, 
		PANEL_MAX_VALUE, 1024*1024,
		0
	);		 
	
	img = xv_create(0, SERVER_IMAGE, 
		  XV_WIDTH, 64,
		  XV_HEIGHT, 64,
		  XV_KEY_DATA, KEY_THIS, (Xv_opaque)(void*)this,
		  SERVER_IMAGE_BITS, go_but_bits,
		  0
        );
	xv_create(ppanel, PANEL_BUTTON,
		  XV_KEY_DATA, KEY_THIS, (Xv_opaque)(void*)this,
		  PANEL_LABEL_IMAGE, img,
	          PANEL_NOTIFY_PROC, ::segment,
		  0
        );
	snak_popup = xv_create(main_frame, FRAME_CMD,
			       XV_LABEL, "Snake Options",
			       FRAME_SHOW_RESIZE_CORNER, FALSE,
			       XV_KEY_DATA, KEY_THIS, (Xv_opaque)(void*)this,
			       XV_HEIGHT, 160,
			       XV_WIDTH, 500,
			       XV_X, CINT(getvalue(maksym_c("mri:snk_x"))),
			       XV_Y, CINT(getvalue(maksym_c("mri:snk_y"))),
			       0
	);
	ppanel = xv_get(snak_popup, FRAME_CMD_PANEL, 0);
	snak_what_i = xv_create(ppanel,PANEL_CHOICE,
				PANEL_LABEL_STRING, "What? ",
				XV_KEY_DATA, KEY_THIS, (Xv_opaque)(void*)this,
				PANEL_CHOICE_STRINGS, 
				"Image", "Series", 0, 
				PANEL_VALUE, 0,
				0
	);
	snak_from_i = xv_create(ppanel, PANEL_CHOICE,
				PANEL_LABEL_STRING, "From: ",
				XV_KEY_DATA, KEY_THIS, (Xv_opaque)(void*)this,
				PANEL_CHOICE_STRINGS,
				"Drawn", "Prev. slice", "Next Slice", 0,
				PANEL_VALUE, 0,
				0
	);
	snak_theta_i = xv_create(ppanel, PANEL_NUMERIC_TEXT,
				 PANEL_MAX_VALUE,10000,
				 XV_KEY_DATA, KEY_THIS, (Xv_opaque)(void*)this,
				 PANEL_LABEL_STRING, "Elasticity",
				 PANEL_VALUE, CINT(getvalue(maksym("mri:elastic"))),
				 0
	);
	snak_rho_i = xv_create(ppanel, PANEL_NUMERIC_TEXT,
			       PANEL_MAX_VALUE,10000,
			       XV_KEY_DATA, KEY_THIS, (Xv_opaque)(void*)this,
			       PANEL_LABEL_STRING, "Atractivity",
			       PANEL_VALUE, CINT(getvalue(maksym("mri:attractive"))),
			       0
	);
	snak_gamma_i = xv_create(ppanel, PANEL_NUMERIC_TEXT,
				 PANEL_MAX_VALUE,10000,
				 XV_KEY_DATA, KEY_THIS, (Xv_opaque)(void*)this,
				 PANEL_LABEL_STRING, "Stiffness",
				 PANEL_VALUE, CINT(getvalue(maksym("mri:stiffness"))),
				 0
	);
	snak_delta_i = xv_create(ppanel, PANEL_NUMERIC_TEXT,
				 PANEL_MAX_VALUE,10000,
				 XV_KEY_DATA, KEY_THIS, (Xv_opaque)(void*)this,
				 PANEL_LABEL_STRING, "3D constraint",
				 PANEL_VALUE, CINT(getvalue(maksym("mri:3dconstraint"))),
				 0
	);
	
	snak_nloi_i = xv_create(ppanel, PANEL_NUMERIC_TEXT,
				PANEL_MAX_VALUE,10000,
				XV_KEY_DATA, KEY_THIS, (Xv_opaque)(void*)this,
				PANEL_LABEL_STRING, "Simlarity",
				PANEL_VALUE, CINT(getvalue(maksym("mri:similarity"))),
				0
	);
	
	img = xv_create(0, SERVER_IMAGE, 
		  XV_WIDTH, 64,
		  XV_HEIGHT, 64,
		  XV_KEY_DATA, KEY_THIS, (Xv_opaque)(void*)this,
		  SERVER_IMAGE_BITS, snk_but_bits,
		  0
        );
	xv_create(ppanel, PANEL_BUTTON,
		  PANEL_LABEL_IMAGE, img,
		  XV_KEY_DATA, KEY_THIS, (Xv_opaque)(void*)this,
		  PANEL_NOTIFY_PROC, ::snake,
		  0
	);	  

	snak_bounds_popup = xv_create(main_frame, FRAME_CMD,
				      XV_LABEL, "Series Bounds",
				      XV_KEY_DATA, KEY_THIS, (Xv_opaque)(void*)this,
				      FRAME_SHOW_RESIZE_CORNER, FALSE,
				      XV_HEIGHT, 50,
				      XV_WIDTH, 250,
				      XV_X, CINT(getvalue(maksym_c("mri:rng_x"))),
				      XV_Y, CINT(getvalue(maksym_c("mri:rng_y"))),
				      0);
	ppanel = xv_get(snak_bounds_popup, FRAME_CMD_PANEL, 0);
	snak_bounds_l_i = xv_create(ppanel, PANEL_NUMERIC_TEXT,
				    XV_KEY_DATA, KEY_THIS, (Xv_opaque)(void*)this,
				    PANEL_MAX_VALUE,100,
				    PANEL_LABEL_STRING, "Lower Bound",
				    PANEL_VALUE, 0,
				    0);
	snak_bounds_h_i = xv_create(ppanel, PANEL_NUMERIC_TEXT,
				    XV_KEY_DATA, KEY_THIS, (Xv_opaque)(void*)this,
				    PANEL_MAX_VALUE,100,
				    PANEL_LABEL_STRING, "Upper Bound",
				    PANEL_VALUE, 100,
				    0);
				    
				
	load_popup = xv_create(main_frame,FRAME_CMD,
		XV_LABEL,"Load Window",
		XV_KEY_DATA, KEY_THIS, (Xv_opaque)(void*)this,
		FRAME_SHOW_RESIZE_CORNER,FALSE,
		XV_HEIGHT,80,
		XV_WIDTH,400,
		0);
/*
	ppanel = xv_get(load_popup, FRAME_CMD_PANEL);
	button = xv_create(ppanel,PANEL_BUTTON,
		PANEL_LABEL_STRING,"Load",
		XV_KEY_DATA, KEY_THIS, (Xv_opaque)(void*)this,
		PANEL_NOTIFY_PROC,load_proc,
		XV_SHOW,TRUE,
		XV_X,180,
		XV_Y,50,
		0);
	load_filename = xv_create(ppanel,PANEL_TEXT,
		PANEL_VALUE_STORED_LENGTH,MAXNAMELEN,
		PANEL_VALUE_DISPLAY_LENGTH,30,
		XV_KEY_DATA, KEY_THIS, (Xv_opaque)(void*)this,
		XV_SHOW,TRUE,
		PANEL_LABEL_STRING,"Filename >:",
		XV_X,10,
		XV_Y,15,
		PANEL_VALUE,filename,
		0);
*/
	
/*	interp_popup = xv_create(main_frame, FRAME_CMD,
				 FRAME_SHOW_RESIZE_CORNER, TRUE,
				 XV_KEY_DATA, KEY_THIS, (Xv_opaque)(void*)this,
				 XV_HEIGHT, 30,
				 XV_WIDTH, 680,
				 XV_LABEL, "Interpreter",
				 0);

	ppanel = xv_get(interp_popup, FRAME_CMD_PANEL);
	interp_txt_i = xv_create(ppanel, PANEL_TEXT,
				 PANEL_VALUE_STORED_LENGTH,4096,
				 PANEL_LABEL_STRING, "? ",
				 XV_KEY_DATA, KEY_THIS, (Xv_opaque)(void*)this,
				 XV_SHOW, TRUE,
				 PANEL_NOTIFY_PROC, interp_from_panel,
				 PANEL_NOTIFY_LEVEL, PANEL_SPECIFIED,
				 PANEL_NOTIFY_STRING, "\r\n",
				 0);
 */				 
	
	/*xv_create(ppanel, PANEL_BUTTON,
		  PANEL_LABEL_STRING, "DoIt",
		  XV_KEY_DATA, KEY_THIS, (Xv_opaque)(void*)this,
		  PANEL_NOTIFY_PROC, interp_from_panel,
		  0);
	*/
	save_popup = xv_create(main_frame,FRAME_CMD,
		XV_LABEL,"Save Window",
		FRAME_SHOW_RESIZE_CORNER,FALSE,
		XV_KEY_DATA, KEY_THIS, (Xv_opaque)(void*)this,
		XV_HEIGHT,80,
		XV_WIDTH,400,
		0);

/*
	ppanel = xv_get(save_popup, FRAME_CMD_PANEL);
	button = xv_create(panel,PANEL_BUTTON,
		PANEL_LABEL_STRING,"Save",
		PANEL_NOTIFY_PROC,save_image,
		XV_KEY_DATA, KEY_THIS, (Xv_opaque)(void*)this,
		XV_SHOW,TRUE,
		XV_X,180,
		XV_Y,50,
		0);
	save_filename = xv_create(ppanel,PANEL_TEXT,
		PANEL_VALUE_STORED_LENGTH,MAXNAMELEN,
		PANEL_VALUE_DISPLAY_LENGTH,30,
		XV_SHOW,TRUE,
		PANEL_LABEL_STRING,"Filename >:",
		XV_X,10,
		XV_Y,15,
		PANEL_VALUE,filename,
		0);
*/

	data_popup = xv_create(main_frame,FRAME_CMD,
		XV_LABEL,"Data Window",
		XV_KEY_DATA, KEY_THIS, (Xv_opaque)(void*)this,
		FRAME_SHOW_RESIZE_CORNER,FALSE,
		XV_HEIGHT,80,
		XV_WIDTH,200,
		0);
/*
	ppanel = xv_get(data_popup, FRAME_CMD_PANEL);
	xv_create(ppanel,PANEL_MESSAGE,
		PANEL_LABEL_STRING,"Total selected: ",
		XV_SHOW,TRUE,
		XV_X,10,
		XV_Y,10,
		0);
	total_msg = xv_create(ppanel,PANEL_MESSAGE,
		XV_X,120,
		XV_Y,10,
		XV_SHOW,TRUE,
		0);
*/
	histo_popup = xv_create(main_frame,FRAME_CMD,
		XV_LABEL,"Histogram Window",
		XV_SHOW,FALSE,
		XV_HEIGHT,128,
		XV_WIDTH,256,
		XV_X, CINT(getvalue(maksym_c("mri:hst_x"))),
		XV_Y, CINT(getvalue(maksym_c("mri:hst_y"))),
		FRAME_SHOW_RESIZE_CORNER,FALSE,
		0);
	xv_destroy(xv_get(histo_popup,FRAME_CMD_PANEL));
	histo_canvas = xv_create(histo_popup,CANVAS,
		CANVAS_AUTO_SHRINK, FALSE,
		CANVAS_FIXED_IMAGE, FALSE,
		CANVAS_RETAINED, TRUE,
		CANVAS_WIDTH,256,
		CANVAS_HEIGHT,128,
		WIN_DYNAMIC_VISUAL,TRUE,
		WIN_X, 0,
		CANVAS_REPAINT_PROC,refresh_histo,
		0);
	xv_set(xv_get(histo_canvas,CANVAS_NTH_PAINT_WINDOW,0),
		CANVAS_REPAINT_PROC,refresh_view,
		0);
	histo_paint = (Xv_opaque)canvas_pixwin(histo_canvas);
	histo_xid = (Drawable)xv_get(histo_paint, XV_XID,0);
	
	help_popup = xv_create(main_frame,FRAME_CMD,
		XV_LABEL,"Help Window",
		XV_HEIGHT, 700,
		XV_WIDTH,  810,
		FRAME_SHOW_RESIZE_CORNER,TRUE,
		0);
	xv_destroy(xv_get(help_popup,FRAME_CMD_PANEL));
	xv_set(xv_create(help_popup,TEXTSW,
			TEXTSW_DISABLE_CD,TRUE,
			TEXTSW_DISABLE_LOAD,TRUE,
			TEXTSW_READ_ONLY,TRUE,
			TEXTSW_FIRST,   0,
               	 	TEXTSW_INSERTION_POINT, 0,
			0),
		TEXTSW_FILE, fullname(CCHARS(getvalue(maksym("mri:helpfile")))),
		TEXTSW_DISABLE_CD,TRUE,
		TEXTSW_DISABLE_LOAD,TRUE,
		TEXTSW_READ_ONLY,TRUE,
		0);
	
	lic_popup = xv_create(main_frame,FRAME_CMD,
		XV_LABEL,"MRITOOL LICENSE",
		XV_HEIGHT, 600,
		XV_WIDTH,  530,
		FRAME_SHOW_RESIZE_CORNER,TRUE,
		0);
	xv_destroy(xv_get(lic_popup,FRAME_CMD_PANEL));
	xv_set(xv_create(lic_popup,TEXTSW,
			TEXTSW_DISABLE_CD,TRUE,
			TEXTSW_DISABLE_LOAD,TRUE,
			TEXTSW_READ_ONLY,TRUE,
			TEXTSW_FIRST,   0,
               	 	TEXTSW_INSERTION_POINT, 0,
			0),
		TEXTSW_FILE, fullname(CCHARS(getvalue(maksym("mri:license")))),
		TEXTSW_DISABLE_CD,TRUE,
		TEXTSW_DISABLE_LOAD,TRUE,
		TEXTSW_READ_ONLY,TRUE,
		0);
	

	
//	tty_popup = xv_create(main_frame, FRAME_CMD,
//			      XV_LABEL, "Excessive Verbosity",
//			      FRAME_SHOW_RESIZE_CORNER, TRUE,
//			      XV_HEIGHT, 500,
//			      XV_WIDTH,  400,
//			      0);
//	xv_destroy(xv_get(tty_popup, FRAME_CMD_PANEL));
//	ttysw = xv_create(tty_popup, TTY,
//			  TTY_QUIT_ON_CHILD_DEATH, FALSE,
//			  TTY_ARGV, ttyargv,
//			  // TTY_ARGV, TTY_ARGV_DO_NOT_FORK,
//			  0);
//	dup2( (int)xv_get(ttysw, TTY_TTY_FD), 0);
//	dup2( (int)xv_get(ttysw, TTY_TTY_FD), 1);
//	dup2( (int)xv_get(ttysw, TTY_TTY_FD), 2);

	
/*
	thr_popup = xv_create(main_frame,FRAME_CMD,
		XV_LABEL,"Thresholding Values",
		XV_HEIGHT,40,
		XV_WIDTH,200,
		FRAME_SHOW_RESIZE_CORNER,FALSE,
		0);
	panel = xv_get(thr_popup,FRAME_CMD_PANEL);
	xv_create(panel,PANEL_MESSAGE,
		PANEL_LABEL_STRING,"W: ",
		0);
	thr_w_text = xv_create(panel,PANEL_MESSAGE,
		PANEL_LABEL_STRING,"0000",
		0);
	xv_create(panel,PANEL_MESSAGE,
		PANEL_LABEL_STRING,"L: ",
		0);
	thr_l_text = xv_create(panel,PANEL_MESSAGE,
		PANEL_LABEL_STRING,"0000",
		0);
	
*/

}




