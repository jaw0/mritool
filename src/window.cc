

#ifdef RCSID
static const char *const rcsid
	= "@(#)$Id: window.cc,v 1.1 1995/11/23 04:30:26 jaw Exp jaw $";
#endif

// Optimization: None

#include <mritool.h>
#include <window.h>
#include <fullname.h>
#include <jl_proto.h>
#include <X11/X.h>
#include <X11/Xlib.h>

u_short icon_bits[] = {
#include "head.icon"
};
u_short icon_m_bits[] = {
#include "head.icon"
};

Xv_opaque XView::base_frame = 0;
extern "C" void interp_from_panel(Xv_opaque it);

static Xv_singlecolor const control_colors[] = {
	// some nice colors, to prevent flashing
	{ 0xff, 0xff, 0xff },
	{ 0, 0, 0 },
	{ 0xcc, 0xcc, 0xcc },
	{ 0xb7, 0xb7, 0xb7 },
	{ 0x66, 0x66, 0x66 },
	{ 0xf4, 0xf4, 0xf4 },
	{ 0xb7, 0xc1, 0xe5 },
	{ 0xe6, 0xe6, 0xff },
	{ 0xe6, 0xff, 0xff },
	{ 0xe6, 0xff, 0xe6 },
	{ 0xff, 0xe6, 0xff },
	{ 0xff, 0xe6, 0xe6 },
	{ 0xf0, 0xf0, 0xfa },
	{ 0xff, 0xff, 0xff },
	{ 0, 0, 0 },
	{ 0, 0, 0 }

};

int frame_done(Xv_opaque fr){
	cerr << fr << endl;
	xv_destroy( fr );
	return 1;
}

XView::XView(){
	XGCValues values;
	char dummy[1024*1024];
	GC	gc;
	static int nchild = 0;
	
	if(! base_frame)
		am_child = 0;
	else{
		XView &xv = me( base_frame );
		*this = xv;		// copy parent frame
		am_child = 1;
	}
	event = 0;
	create_base();
	create_panel();
	create_canvas();
	init_cms();
	init_cursor();
	create_meas_popup();	
	create_patient_popup();
	create_popups();
	if(! am_child){
		init_icon();
		xv_set(main_frame, 
		       XV_X, CINT( getvalue(maksym_c("mri:base_x"))),
		       XV_Y, CINT( getvalue(maksym_c("mri:base_y"))),
		       FRAME_DONE_PROC, quit,
		       0
	       );
	}else{
		xv_set(main_frame, XV_SHOW, TRUE,
		       FRAME_CMD_PUSHPIN_IN,TRUE,
		       FRAME_DONE_PROC, xv_destroy, //frame_done,
		       XV_X, CINT( getvalue(maksym_c("mri:child_x"))) + nchild * CINT( getvalue(maksym_c("mri:child_dx"))),
		       XV_Y, CINT( getvalue(maksym_c("mri:child_y"))) + nchild * CINT( getvalue(maksym_c("mri:child_dy"))),
		       0
		);
		nchild ++;
	}
	
	// init for graphics
	values.foreground = CINT(getvalue(maksym("mri:line-value")));
	values.line_width = CINT(getvalue(maksym("mri:line-width")));
	values.line_style = CINT(getvalue(maksym("mri:line-style")));
	values.line_style = values.line_style==1? LineSolid
		: values.line_style==2 ? LineOnOffDash
		: values.line_style==3 ? LineDoubleDash
		: LineSolid;
	
	gc_img = XCreateGC(dpy, canvas_xid, 
		GCForeground|GCLineWidth|GCLineStyle, 
		&values);
	
	values.function = CINT(getvalue(maksym("mri:line-GXfnc")));
	gc_draw = XCreateGC(dpy, canvas_xid, 
		GCFunction|GCForeground|GCLineWidth|GCLineStyle, 
		&values);
	
	values.foreground = 0;	// erase
	values.background = 0;
	gc_erase = XCreateGC(dpy, canvas_xid, 
		GCForeground|GCBackground, 
		&values);
	
	values.line_width = 1;
	
	ximage  =  XCreateImage(dpy, (Visual*)xv_get(canvas,XV_VISUAL,0),
		8,ZPixmap, 0, dummy, /*512, 512*/ 1024, 1024, 8,0);
	xdrawing = XCreateImage(dpy, (Visual*)xv_get(canvas,XV_VISUAL,0),
		8,ZPixmap, 0, dummy, 1024,1024,8,0);
	pdrawing = XCreatePixmap(dpy, canvas_xid, 1024, 1024, 8);

	// clear it out
	values.foreground = 0;
	values.background = 0;
	gc = XCreateGC(dpy, pdrawing, GCForeground|GCBackground, &values), 
		XFillRectangle(dpy, pdrawing, gc, 0, 0, 1024,1024);
	XFreeGC(dpy, gc);
	
	// notify_interpose_destroy_func(main_frame, 
	// 	(Notify_func)main_destroy_interposer);

	if(! base_frame ) base_frame = main_frame;
	ui = this;

}

void XView::create_base(){
	char buffer[1024];
	FILE *fp;
	char *i;

	if (fp=fopen(fullname("~weisberg/.mrimsg"),"r")){
		/* get right footer text from file */
		fread(buffer,1023,1,fp);
		fclose(fp);
		for(i=buffer;*i;i++)
			if( *i == '\n' || *i == '\r' ) *i=*(i+1)=0;
	}else 
		*buffer = 0;
	// parent to create is 0 (instead of base) so as to allow frames
	// to be placed in separate virtual desktops, as a side effect,
	// the program will not quit until all of the frames are dismissed
	// (ie. clicking quit on the base is not sufficient)
	main_frame = xv_create(/*base_frame*/0, am_child?FRAME_CMD:FRAME,
		XV_WIDTH, SCREENX,
		XV_HEIGHT, SCREENY,
		FRAME_SHOW_RESIZE_CORNER,FALSE,
		WIN_DYNAMIC_VISUAL,TRUE,
		XV_LABEL,version,
		FRAME_SHOW_FOOTER,TRUE,
		FRAME_RIGHT_FOOTER,buffer,
	        XV_KEY_DATA, KEY_THIS, (Xv_opaque)(void*)this,
		0);


	dpy = (Display *)xv_get(main_frame, XV_DISPLAY,0);

}
/************************************************************************/
void XView::create_canvas()
{

	canvas = xv_create(main_frame,CANVAS,
		CANVAS_AUTO_SHRINK, FALSE,
		CANVAS_FIXED_IMAGE, FALSE,
		CANVAS_RETAINED, TRUE,
			      // canvas could be made larger, but it gets slooooow....
		CANVAS_WIDTH, 1024, //CANV_W,
		CANVAS_HEIGHT,1024, //CANV_H,
	        WIN_BELOW, interp_popup,
		WIN_DYNAMIC_VISUAL,TRUE,
		WIN_X, 0,
		WIN_EVENT_PROC, mouse_controller,
		XV_KEY_DATA, KEY_THIS, (Xv_opaque)(void*)this,
		CANVAS_REPAINT_PROC,refresh_view,
	//	WIN_CMS, cms, //
		0);


	xv_set(xv_get(canvas,CANVAS_NTH_PAINT_WINDOW,0),
		WIN_CONSUME_EVENT,LOC_DRAG,
		WIN_EVENT_PROC,mouse_controller,
		XV_KEY_DATA, KEY_THIS, (Xv_opaque)(void*)this,
	        CANVAS_REPAINT_PROC,refresh_view,
	//	WIN_CMS, cms, //
		0);
	paint = (Xv_opaque)canvas_pixwin(canvas);
	canvas_xid = (Drawable)xv_get(paint, XV_XID,0);
	
}

/***********************************************************/

/* create_panel moved into lisp */

/********************************************************/

void XView::init_icon(){
	Server_image icon_image,mask_image;
	Icon icon;

	icon_image = (Server_image)xv_create(NULL,SERVER_IMAGE,
		XV_WIDTH,64,
		XV_HEIGHT,64,
		SERVER_IMAGE_BITS,icon_bits,
		0);

	mask_image = (Server_image)xv_create(NULL,SERVER_IMAGE,
		XV_WIDTH,64,
		XV_HEIGHT,64,
		SERVER_IMAGE_BITS,icon_m_bits,
		0);

	icon = (Icon)xv_create(main_frame,ICON,
		ICON_IMAGE,icon_image,
		ICON_MASK_IMAGE,mask_image, /* blend in bkgrnd */
		0);
	
	xv_set(main_frame,FRAME_ICON,icon,0);
}

/************************************************************************/

//  cmap looks like:
// vvvv xxxxxxxxxxxx GGGGGGGGGGGGGGGGGGGG xxxxxxxxxxxxxxxx RRRRRRRRRRRRRRRR
// |    |            |                    |                |               |
// 0   16           64                   128              192             256


extern int current_level, current_window;
void XView::init_cms(){
	int  i;
	int a,b,v;
	float r;
	unsigned long *cls;
	XColor xcls[64];
	
	int window = CINT(getvalue(maksym("mri:window")));
	int level  = CINT(getvalue(maksym("mri:level")));

	current_level = level;
	current_window = window;
	
	a = level-window/2;
	b = level+window/2;
	a = (a>255)?255:(a<0)?0:a;
	b = (b>255)?255:(b<0)?0:b;
	r = 255.999/(b-a);
	
	i=OUTSIDE_ROI;
	map[i].red=map[i].green=map[i].blue=0;

	// really need a way to get the bottom colors....
	//for(i=0; i<16; i++){
	//	map[i] = control_colors[i];
	//}

	// cls = (unsigned long *)xv_get(0, WIN_X_COLOR_INDICES, 0);
	// cls = (unsigned long*)xv_get( xv_get(0, WIN_CMS, 0), CMS_COLORS, 0);
	// return 0

	for(i=0;i<64;i++){
		xcls[i].pixel = i;
		xcls[i].flags = DoRed | DoGreen | DoBlue;
	}
	XQueryColors(dpy, DefaultColormap(dpy,0), xcls, 64);
	for(i=0;i<64;i++){
		map[i].red = xcls[i].red >> 8;
		map[i].blue= xcls[i].blue >> 8;
		map[i].green = xcls[i].green >> 8;
	}	
		
	for(i=64;i<128;i++){
		// gray
		v = ((i-64)*(255.0/63)-a)*r;
		map[i].blue=map[i].green=map[i].red
			= (u_char) (v<0)?0:(v>255)?255:v;
		
	}

	for(i=192;i<256;i++){
		// red
		map[i].red = (256-i)*2+127;
		map[i].blue =  ((256-i)*(255.0/127));
		map[i].green = ((256-i)*(255.0/127));
	} 

	if(1|| !am_child){
		cms = xv_create(0,CMS,
				CMS_SIZE, 256,
				CMS_COLOR_COUNT, 256,
				CMS_COLORS, map,
				CMS_TYPE, XV_DYNAMIC_CMS,
				0);

		xv_set(paint,  WIN_CMS, cms, 0);
	}else{
		xv_set(paint,  WIN_CMS, me(base_frame).cms, 0);
	}
}



/********************************************************/


void XView::init_cursor(){

	/* code stolen from openwindows example code */

    Xv_Cursor      cursor;
    Server_image   image;
    Pixmap         pixmap;
    Display        *ddpy = (Display *)xv_get(main_frame, XV_DISPLAY);
    GC             gc;
    XGCValues      gcvalues;
    int		crad, ccorn;
    

    crad  = CINT(getvalue(maksym("mri:cursor-size")));
    ccorn = 8 - crad;
    
    image = (Server_image)xv_create(XV_NULL, SERVER_IMAGE,
        XV_WIDTH,       16,
        XV_HEIGHT,      16,
        NULL);
    pixmap = (Pixmap)xv_get(image, XV_XID);
    /* Create GC with reversed foreground and background colors to
     * clear pixmap first.  Use 1 and 0 because pixmap is 1-bit deep.
     */
    gcvalues.foreground = 0;
    gcvalues.background = 1;
    gc = XCreateGC(ddpy, pixmap, GCForeground|GCBackground, &gcvalues);
    XFillRectangle(ddpy, pixmap, gc, 0, 0, 16, 16);
    /*
     * Reset foreground and background values for XDrawArc() routines.
     */
    gcvalues.foreground = 1;
    gcvalues.background = 0;
    XChangeGC(ddpy, gc, GCForeground | GCBackground, &gcvalues);
    XDrawArc(ddpy, pixmap, gc, ccorn, ccorn, 2*crad+1, 2*crad+1, 0, 360 * 64); 
    XDrawPoint(ddpy, pixmap, gc, 8,8);

    /* Create cursor and assign it to the window (parameter) */
    cursor = xv_create(XV_NULL, CURSOR,
        CURSOR_IMAGE,   image,
        CURSOR_XHOT,    7,
        CURSOR_YHOT,    7,
	CURSOR_OP,	PIX_SRC^PIX_DST,
        NULL);
    xv_set((Xv_opaque)paint, WIN_CURSOR, cursor, NULL);

    /* free the GC -- the cursor and the image must not be freed. */
    XFreeGC(ddpy, gc);
}

//***************************************************************************/

void XView::main_loop(){
	xv_main_loop(base_frame);
}


void XView::thinking(void) const {
	thinking("Thinking");
}

static int think_lev = 0;
void XView::thinking(char *msg) const {

// 	think_lev ++;
	cerr << msg << endl;
	xv_set(main_frame,
		XV_LABEL, msg,
		FRAME_BUSY,TRUE,
		0);
	XFlush( dpy );
}

void XView::done_thinking() const {
	char buffer[256];

//	if( --think_lev)
//     	        return;
	
	xv_set(main_frame,
		XV_LABEL,version,
		FRAME_BUSY,FALSE,
		0);
	XFlush( dpy );

	// set label on top
	
	((XView*)this)->interpret("(mri:set-header)");

	XFlush( dpy );

}



