

// Copyright (c) 1993 Jeff Weisberg
// University of Rochester
//      Dept. of Electrical Engineering
//      Dept. of Neurology, C.E.P.
//
// see the file "License"
 
#ifndef _cl_patient_h
#define _cl_patient_h

#ifdef RCSID
static const char *const rcsid_clpatient_h
= "@(#)$Id: cl_patient.h,v 1.1 1995/11/23 04:30:00 jaw Exp jaw $";
#endif

#include <images.h>
#include <snake.h>
#include <object.h>
#include <matrix.h>
#include <jlisp.h>

class Series;
class Study;
class Patient;
class XView;
class UserI;

extern Patient* find(int pid);
// extern void list(ostream&);	// list patients
extern Obj  pat_list(void);	// ditto

#define B(b)	(1<<(b))

class Base : public Object {

//  protected:
public:
	u_long  flags;
	boolean	flag;
	Base();
	
  public:
	char	dirname[256];
	char	name[32];
	int	num;
	int	selected;
	virtual Type mytype(void)    const {return Object::BASE;}
	virtual char* identity(void) const {return "Base";}

};

class Image : public Base {
    friend int cmp(Image *a, Image *b);
	friend class Study;
	friend class Series;
	friend class Patient;
    friend void test_code(void);

  public:
	Series	*mine;

//  protected:
public:
	int	window, level;
	int	nlevs;		// segmentation levels
	Fimage	undo;

  public:
	u_short		*dg_data;	// data as read in 256 * 256
	Fimage	     	fimg;
	Uimage		segm;
	Uimage		view;
	Uimage 		drawing2;
	Uimage		drawing;
	Snake		*snake;    	// snaked contour

	int		num_of_snakes;	// how many?

  protected:
	
  public:
	// for mice, should these really be here?
	// int start_mx, start_my;
	// int last_mx,  last_my;

	enum {
		IFB_SEGM_PRESENT,
		IFB_OFFL_PENDING,
		IFB_CONTOUR_PRESENT,
		IFB_VIEW_SEGM,
		IFB_HIDE_ROI,
		IFB_HIDE_CONTOUR,
		IFB_DRAWN_ON
	};			// flags bits

	Image	*next;		// LL
	int	pid;		// pid of child offline segm. job
	
	int x_pix_dim, y_pix_dim;	// data from MRI scan
	float thickness, pixel_size, fov;
	float multby;
	
	void S(int b) { flags |= B(b); }
	void C(int b) { flags &= ~B(b);}
	int  T(int b) { return( flags & B(b) ); }

	Image();
	~Image() {cleanup();}
	virtual Type mytype(void) const {return Object::IMAGE;}
	virtual void cleanup(void);
	virtual void lock(void);
	virtual void unlock(void);
	virtual char* identity(void) const {return "Image";}
    
	void doit();		// setup and call load
	void rdb_doit();	// setup and call load - remote 
	void doit(FILE*);
	int  learn(Obj l=IC_NIL);	// fill in data values
	int rdb_learn(void);
	void rdb_mark(void);
	void setm(Series *m){mine = m;}

	void save_segm();		// save the segm data
	void load_segm();		// load segm data
	void segm_lock(void);		// build offline segm lock file
	int  mkdirhier();	// mkdirs required for saving
	void visine(void);
	int load_cont(char*);	// and load
	
		// mouse ops and suchlike
	void update_view();	// fill the view array from the float | segm array
	void update_undo();	// update undo data
	void refresh(UserI&);		// update the screen from the view array and contour
	// void mouse_op(XView&, Event *e);	//
	// void close_contour(UserI&);	// seal it up

	Obj  info(void) const;
	Obj  lois(void) const;
	
};
class Stats {
  public:
	float vol, sa, sf, sp;
	Vector com, ev;
	Matrix I, eV;
	Stats();

    friend ostream& operator<<(ostream&, Stats&);
	
};

class Series : public Base {
	friend int  Image::learn(Obj);
	friend class Study;
	friend class Image;
	friend class Patient;
	
	friend int  Image::mkdirhier();

    friend int  cmp(Series *a,Series *b);
    friend void do_image();
    friend void test_code(void);

//protected:
public:
	Image **images;
  public:
	int 	num_of_images;
	Study	*mine;
	Series	*next;
	char descr[60], date[16], plane[32], protocol[32];
	int 	num;
	int	pid;		// pid of child offline segm. job
	struct Roi {
		boolean	present;	// is this roi valid?
		int	xo,
			yo,
			width,
			height;
	} *roi;		// the region of interest (is of series)
				// allow several
	int c_roi;		// current roi-oi
	Stats		*stats;

	Series();
	~Series() {cleanup();}	
	virtual Type mytype(void) const {return Object::SERIES;}
	virtual void cleanup(void);
	virtual void lock(void);
	virtual void unlock(void);
	virtual char* identity(void) const {return "Series";}
    
	void menu(XView&);	// popup menu of images
	int  learn(Obj l=IC_NIL);	// fill in data values
	void rdb_mark(void);
	int rdb_learn(void);
	void setm(Study *m){mine=m;}

	void reap(int pid);		// reap child offline proc
	void draw_roi(XView&, int x, int y, int eid);	// draw the ROI
	int mkdirhier(void);
	Image *find(int n) const; 	// find slice n
	void analyze(UserI&,int obj, int l, int h);
	// void list(ostream&) const;
	Obj  list(void) ;
	Obj  info(void) const;
	int  many(void) const { return num_of_images; }

};

class Study : public Base {
    friend int Image::learn(Obj);
	friend class Series;
	friend class Image;
	friend class Patient;
    friend int Image::mkdirhier();
    friend int cmp(Study *a,Study *b);
    friend void select_images(Xv_opaque);
    friend void test_code(void);

//protected:
public:
	int num_of_series;
	Series **series;
public:
	Patient	*mine;
	char descr[60], date[16];
	Study	*next;
	
	Study();

	~Study() {cleanup();}
	virtual Type mytype(void) const {return Object::STUDY;}
	virtual void cleanup(void);
	virtual void lock(void);
	virtual void unlock(void);
	virtual char* identity(void) const {return "Study";}
    
	void menu(XView&);	// popup menu of series
	int  learn(Obj l=IC_NIL);	// fill in data values
	int rdb_learn(void);
	void rdb_mark(void);
	void setm(Patient *m){mine=m;}
	Series* find(int) const;
	// void list(ostream&) const;
	Obj  info(void) const;	
	Obj  list(void) ;
	void info(ostream&)const;
};

class Patient : public Base {
    friend void create_patient_db(void);
	friend class Series;
	friend class Image;
	friend class Study;
    friend int  rdb_create_patient_db(void);
    friend int Image::learn(Obj);
    friend void select_series(Xv_opaque);
    friend void test_code(void);

//protected:
public:
	int num_of_studies;
	Study **studies;
public:
	char pid[16], sex;
	char hospital[32];
	int age, weight;
	Patient	*next;
	
	Patient();

	~Patient() {cleanup();}
	virtual Type mytype(void) const {return Object::PATIENT;}
	virtual void cleanup(void);
	virtual void lock(void);
	virtual void unlock(void);
	virtual char* identity(void) const {return "Patient";}
    
	void menu(XView&);	// popup menu of studies
	int  learn(Obj l=IC_NIL);	// fill in data values
	int rdb_learn(void);
	void rdb_mark(void);
	Study* find(int) const;
	// void list(ostream&) const;
	Obj  info(void) const;
	Obj  list(void) ;
	
};


#endif /* !_cl_patient_h */

