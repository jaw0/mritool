

// Copyright (c) 1993 Jeff Weisberg
// University of Rochester
//      Dept. of Electrical Engineering
//      Dept. of Neurology, C.E.P.
//
// see the file "License"

#ifndef _snake_h
#define _snake_h

#ifdef RCSID
static const char *const rcsid_sh
= "@(#)$Id: snake.h,v 1.1 1995/11/23 04:30:02 jaw Exp jaw $";
#endif
	
#include <iostream.h>
#include <object.h>
#include <point.h>

#undef SQR
#undef ABS
#undef MIN

#define INRANGE(l,x,h)	( (x)>=(l) ? (x)<(h) ? 1 : 0 : 0 )
#define SQR(x)		( (x) * (x) )
#define MIN(x,y)	( ((x) > (y)) ? (y) : (x) )
#define ABS(x)		( ((x)>0) ? (x) : -(x) )
#define DIST2(a,b)      (SQR(((a).x - (b).x))+ SQR(((a).y - (b).y)))
#define DIST(a,b)	(SQRT( DIST2(a,b) ))
#define SIGN(x)		( ((x)<0)?-1:1 )
#define SQRT(x)		( (x<(sizeof(sqrt_tabl)/sizeof(sqrt_tabl[0])))?sqrt_tabl[int(x)]:sqrt(double(x)) )
#define DISTFAR		4
#define DISTFAR_READ		8

class Edgemap {
  public:
	int level;
	Fimage data; 

	Edgemap() : data(0,0) {level=0;}
};
class Bolus {
  public:
	int alpha, 	// elast
	beta,		// attract
	gamma,		// stiffn
	delta,		// 3d
	nloi;		// sameness
	Edgemap* edge;
	Uimage*  tain;
	Fimage*  pcont;
	Point    orig;
};
	
class Snakelet : private Object {
    friend class Snake;
  private:
	Point	pt;
	Snakelet *next, *prev;
	
	void append (Snakelet*);	// add new one after me	
	void prepend(Snakelet*);	// add new one before me
	void remove(void);		// remove node
	void kill(void);		// remove all from here to end
	Snakelet& operator=(Point x){pt=x; return *this;}

	float penergy(Bolus&) const;	// point E

	Snakelet *leftof (void) const;	// return point on approp. side
	Snakelet *rightof(void) const;
	Snakelet& operator=(const Snakelet& s){ pt = s.pt;
					  prev=s.prev; next=s.next;
					  return *this;
				  }
	Snakelet(Snakelet& s);		// no copying
	float dfhls_read(Uimage&, Uimage&);
	float dfs_read  (Uimage&, Uimage&);
	float measure(void) const;
	
  public:
	Snakelet(){ next=prev=0; }
	~Snakelet(){kill();}
	virtual Type mytype(void) const { return Object::SNAKELET; }
	virtual char* identity(void) const {return "Snakelet";}
	float energy(Bolus&) const;	    	// local E

	
};

	
class Snake : private Object {
    friend class Image;
    friend class UserP;
    friend ostream& operator<<(ostream& os, Snake &s);
	
  private:
	int 	length;			// how big is this critter?
	Point	refpt,			// guess at point of peak value
		displ;			// displacement from pcont
	
        Snakelet box;			// put it in box so they don't get out
	Edgemap  edg;
	Uimage	 tain;			// whats contained in the snake
	
	void inspect (Snakelet*);		// remove, or insert new points
	virtual void lock(void);
	virtual void unlock(void);
	virtual void cleanup(void) { shed(); }
	void edgemap(int lev, Uimage& segm);
	void expand(const Bimage&, const Bimage&, int, float);
	int prune(void);
	
  public:
	int loi;
	Point ploi;
	Point		bb_i, bb_f;		// a bounding box
	
	Snake() : tain(512, 512, 0) {
		length = 0; loi=-1; ploi=0;
		displ=0; refpt=-1;}
	~Snake(){ shed();}
	virtual Type mytype(void) const {return Object::SNAKE; }
	virtual char* identity(void) const {return "Snake";}
	
	void shed(void);						   	// shed off old skin (and memory)
	void reader(Uimage& init, int i);  			      	// convert to vector snake
	void minimate(Bolus);   	// minimize E
	
	void illiterate(Uimage& drawing, int i) const;	// anti-reader
	//void displ_est(Snake &pc, Uimage& segm, int l);
	void displ_guess(const Snake &pc, Uimage& segm, int l);
	void re_est(Uimage& segm, int l);

	void nbox(Point&, Point&, Point&, float) const;
};

extern int nindexof(Point, int);

#endif /* !_snake_h */


