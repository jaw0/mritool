

// Copyright (c) 1993 Jeff Weisberg
// University of Rochester
//      Dept. of Electrical Engineering
//      Dept. of Neurology, C.E.P.
//
// see the file "License"
 
#ifndef _window_h
#define _window_h

#ifdef RCSID
static const char *const rcsid_window_h
= "@(#)$Id: window.h,v 1.1 1995/11/23 04:30:03 jaw Exp jaw $";
#endif
	
#include <X11/Xlib.h>
#include <X11/X.h>
#include <userp.h>

class Image;
class Series;
class Study;
class Patient;
class ifstream;
class XView;

struct Button_Labels {
	Xv_opaque button_i, text_i;
};

class UserI : public UserP {
	// abstract class
	
  public:
	// void 	*dlhand;	// dynamic variable lookup
	
	UserI();
	~UserI(){}

	void do_image(void);
	virtual void main_loop() = 0;
	
	virtual void thinking(void)  const {}
	virtual void thinking(char*) const {}
	virtual void done_thinking() const {}
        virtual void show_loi(void)  const;
	
	virtual void error(const char* a=0, const char* b=0, const char* c=0, const char* d=0,
			   const char* e=0, const char* f=0, const char* g=0) const;
	
	virtual void put_dot(Point, int, Bimage* img=0);
	virtual void put_line(Point, Point, int, Bimage* img=0);
	virtual void clear_spot(Point, Uimage* img=0);
	virtual void set_contour(Bimage&){}
	virtual void visine(Bimage* img=0) = 0;
	virtual void refresh(Uimage&, int) {}

	virtual void interpret(char*);

	virtual XView* get_xv(void){
		// a kludge for lisp interp
		return 0;
	}
};

class XView : public UserI {
  private:
	char cmsname[20];
	static Xv_opaque base_frame;
  public:
	int am_child;
	Button_Labels	*mbuttons;
	
	Xv_opaque	main_frame, canvas, paint, cms,
			data_popup, total_msg, 
			histo_popup, histo_canvas, histo_paint,
			meas_popup, patient_popup, 
			patient_list, study_popup, 
			study_list, series_popup, 
			series_list, image_popup, 
			image_list, mouse_menu, 
			help_popup, panel, lic_popup,
			load_popup, save_popup,
			segm_popup, segm_what_i, segm_grnd_i,
			segm_nlev_i, segm_alpha_i,
			snak_popup, snak_from_i, snak_theta_i,
			snak_rho_i, snak_gamma_i, snak_what_i,
		        snak_wlev_i, snak_delta_i, snak_nloi_i,
			snak_bounds_popup, snak_bounds_h_i, snak_bounds_l_i,
		        tty_popup, ttysw, interp_popup, interp_txt_i, 
			offl_msg_popup;
	u_char		histo[256];
	Display		*dpy;
	Drawable	canvas_xid, histo_xid;
	XImage		*ximage, *xdrawing, *xhisto;
	Pixmap		pdrawing, pdrawing2;
	GC		gc_img, gc_draw, gc_erase, gc_histo;
	Event		*event;
	
	Xv_singlecolor	map[256];	// colormap

	XView();
	~XView() {}
	void create_base();
	void create_panel();
	void create_canvas();
	void create_popups();
	void create_meas_popup();
	void create_patient_popup();
	void init_cms();
	void init_icon();
	void init_cursor();
	void adjust_wl(int x, int y);
	void footer(const char*);
	
	virtual void main_loop();
	virtual void thinking(void)  const;
	virtual void thinking(char*) const;
	virtual void done_thinking() const;
	virtual void error(const char* a=0, const char* b=0, const char* c=0, const char* d=0,
		   const char* e=0, const char* f=0, const char* g=0) const;
	virtual void put_dot(Point, int, Bimage* img=0);
	virtual void put_line(Point, Point, int, Bimage* img=0);
	virtual void clear_spot(Point, Uimage* img=0);
	virtual void set_contour(Bimage&);

	virtual void visine(Bimage* img=0);
	virtual void refresh(Uimage&, int);
	
	virtual void interpret(char*);

	virtual XView* get_xv(void){
		return this;
	}
};

class NoWin : public UserI {

  private:
	Uimage drawing;
	
  public:
	NoWin();
	~NoWin(){}

	virtual void main_loop();

	virtual void visine(Bimage* img=0);
	virtual void thinking(void)   const;
	virtual void thinking(char *) const;

};

extern UserI* UI;

#define KEY_THIS	0x0f715a30
#define KEY_BUTN	0x0f715a31
#define KEY_INTRP	0x0f715a32b
#define SCREENX	(512)
#define SCREENY	(512+25+20)
#define CANV_W	(512)
#define CANV_H	(512)
#define BUTT_WIDTH	60
#define OUTSIDE_ROI	15
#define MAXINLIST	200
#define DX	85
#define DY	50
#define X0	10
#define Y0	10
#define X1	10
#define Y1	30


inline XView& me(Xv_opaque it){
	void* v = (void*)xv_get(it, XV_KEY_DATA, KEY_THIS);
	return *(XView*)v;
}


#endif /* _window_h */

