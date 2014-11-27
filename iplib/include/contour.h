

#ifndef _contour_h
#define _contour_h

 
#ifdef RCSID
static const char *const rcsid_contour_h
= "@(#)$Id: contour.h,v 1.1 1995/11/23 04:31:13 jaw Exp jaw $";
#endif
 
#include <point.h>
#include <images.h>
#include <vector.h>

#define DIST2(a,b)      (SQR(((a).x - (b).x))+ SQR(((a).y - (b).y)))
#define DIST(a,b)       sqrt( DIST2(a,b) )
#define SQR(x)          ((x)*(x))

class Curvelet {
    friend class Contour;
  protected:
        Curvelet *next, *prev;
  public:
	Point pt;
 
        void append (Curvelet*);
        void prepend(Curvelet*);
        void kill(void);        // remove nodes here to end ?
 
        Curvelet& operator=(Point x){pt=x; return *this;}
        Curvelet& operator=(Curvelet& c){
                pt=c.pt;
                next=c.next;prev=c.prev;
                return *this;
        }
 
        Curvelet(Curvelet&);    // no copying
        float dfhls_read(Uimage&, const Bimage&);
        float dfs_read(Uimage&, const Bimage&);
 
  public:
        operator Point() const {return pt;}
        Curvelet(){next=prev=0; pt=0;}
        ~Curvelet(){kill();}
 
        Curvelet* succ(void) const {return next;}
        Curvelet* pred(void) const {return prev;}
	Curvelet* rightof(void) const;
	Curvelet* leftof(void) const;
};


class Contour {

  protected:
	Curvelet *head, *tail;

  public:
	int length;

	Contour(){length=0;head=tail=0;}
	Contour(Contour&);
	~Contour();

	Curvelet* first(void) const {return head;}
	Curvelet* rear(void) const {return tail;}
	void read(const Bimage&);
	float measure(void) const;
	void  bbox(Point&, Point&) const;
};

class Segment : public Vector {
  public:
        float angle;
        enum SegType { Convex, Concave } type;
 
        Segment& operator=(float f){
                Vector::operator=(f); return *this;
        }
 
};

class Shape {
  protected:
        Segment* seg;
 
  public:
        int nsegs;      // no. of segments
	Point *starts, *ends, *mids;

        Shape(){nsegs=0; seg=0; starts=ends=mids=0;}
        Shape(const Shape&);
        ~Shape();
 
        void moosh(const Contour&, int resol=10,float sigma=6);
                // resol = number of divs per segment (=segment.length)
                // default sigma of 6, see paper
 
        Shape& operator=(const Contour& c){moosh(c);return *this;}
 
    friend float cost(const Shape&, const Shape&);
 
};
 


#endif /* !_contour_h */

