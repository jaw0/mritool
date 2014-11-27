
#ifdef RCSID
static const char *const rcsid
= "@(#)$Id: contour.cc,v 1.1 1995/11/23 04:30:58 jaw Exp jaw $";
#endif
 
#include <contour.h>
#include <math.h>
#include <matrix.h>
#include <func/gaussian.h>
#include <func/delta.h>
#include <fstream.h>

#define loopp(p, a, b)  for(p.y=a.y;p.y<b.y;p.y++)for(p.x=a.x;p.x<b.x;p.x++)
#define SIGN(x) ( (x)<0 ? -1 : (x)>0 ? 1 : 0 )

#define FIRSTONE 1

/*
  neighbors are numbered as follows:
 
  ___________________
 |  9| 16|  8| 23| 15|
 |___________________|
 | 17|  1|  0|  7| 22|
 |___________________|
 | 10|  2| **|  6| 14|
 |___________________|
 | 18|  3|  4|  5| 21|
 |___________________|
 | 11| 19| 12| 20| 13|
 |___________________|
 
 */
 
static const xxPointxx nindex[] = {
        // this is the above table
 
        {0, -1}, {-1, -1}, {-1, 0}, {-1, 1},
        {0, 1}, {1, 1}, {1, 0}, {1, -1},
 
        {0, -2}, {-2, -2}, {-2, 0}, {-2, 2},
        {0, 2}, {2, 2}, {2, 0}, {2, -2},
 
        {-1, -2}, {-2, -1}, {-2, 1},{-1, 2},
        {1, 2}, {2, 1}, {2, -1}, {1, -2}
};
static const int heuristic[] = {
        // this is the order we look at our neighbors
        // it's probobly not optimal, but it seems to work well ...
        4, 3, 5, 2, 6, 12, 19, 20, 18, 21, 10, 14, 11, 13, 17, 22, 1, 7, 9, 15, 16, 23
                // do not look at 8, 0
};
static const int h_stops[] = {
        // so as to group the heuristics
        1, 0, 1, 0, 1,  1,  0,  1,  0,  1,  0,  0,  0,  1,  0,  1, 0, 1, 0,  1,  0,  1
};


Shape::~Shape(){
 
        if(seg) delete [] seg;
	if(starts) delete [] starts;
	if(ends) delete [] ends;
	if(mids) delete [] mids;
	
        seg = 0;
	starts = ends = mids = 0;
}
 
void Contour::bbox(Point& from, Point& to) const {
	Curvelet* s=head;
	if(!head){
		to = from = 0;
		return;
	}
	to = from = head->pt;
	while(s->next){
		if(s->pt.x < from.x) from.x = s->pt.x;
		if(s->pt.y < from.y) from.y = s->pt.y;
		if(s->pt.x > to.x) to.x = s->pt.x;
		if(s->pt.y > to.y) to.y = s->pt.y;

		if(s->next == head->prev) break;
		s = s->next;
	}
}
float Contour::measure(void) const{
 
        if(!this) return 0;
        float dist = 0;
        Curvelet* s=head;
 
        while(s->next){
                dist += DIST(s->pt, s->next->pt);
                s = s->next;
                if(s->next == head->prev) break;
        }
        if(head->prev) dist += DIST(head->pt, head->prev->pt);
        return dist;
}

Curvelet* Curvelet::leftof(void) const {
	      // find distinct point to the left of here
 
        if( !prev) return (Curvelet*)this;
 
        if( pt != prev->pt ) return prev;
        if( prev->prev ) return prev->prev;
        return (Curvelet*)this;
}

Curvelet* Curvelet::rightof(void) const {
 
        if( !next) return (Curvelet*)this;
        if( pt != next->pt ) return next;
        if( next->next ) return next->next;
        return (Curvelet*)this;
}


void Curvelet::kill(void){
	Curvelet *k;
	
        if(!this) return;
        if(next){
		k = next;
		next = 0;
		delete k;
        }
	if( prev ) prev->next = 0;
	prev = 0;
	pt = -1;
}
 
void Curvelet::append(Curvelet* c){
        Curvelet* foo;

	if(! c) return;
        c->prev = this;
        if(next){
                foo = c;
                while(foo->next)foo=foo->next;
                foo->next = next;
                next->prev = foo;
        }
        next = c;
}
 
void Curvelet::prepend(Curvelet* c){
        Curvelet* foo;

	if(! c) return;
	
        c->next = this;
	if(prev){
		foo = c;
		while(foo->prev)foo=foo->prev;
                foo->prev = prev;
                next->next = foo;
        }
        prev = c;
}
 
Contour::~Contour(){

	if(! head) return;

	delete head;
	length = 0;
	head = tail = 0;
}

void Bimage::draw(const Contour& c, float v){
	Curvelet *s, *f;
	int i=0;

	for(s=f=c.first(); s && s->succ() && s->succ() != f; s=s->succ(), i++){
		line(s->pt, s->succ()->pt, v);
	}
	if( f->pred())
		line(f->pred()->pt, f->pt, v);
}

	
void Contour::read(const Bimage& initial){

	// convert contour image into vector snake


	Uimage seen(initial.width, initial.height);
	int j;
	float dist=0, cdist;
	Curvelet *tll;
	Point p, fini=initial.offset+Point(initial.width, initial.height);
	
	if(head)head->kill();
	head = 0;
	seen = 0;
        seen.offset = initial.offset;

	loopp(p, initial.offset, fini){
		// search over all points for the snake
		if(int(initial.value(p))){		// tain[p]
			tll = new Curvelet;
			tll->pt = p;
			seen[p] = 1;
			cdist = tll->dfs_read(seen, initial);
			if( cdist > dist){
				dist = cdist;
				head->kill();
				head = tll;
			}else{
				delete tll;
			}
			seen[p] = 0;

			if( dist > 0.0 ) p = fini; // exit loop 
		}
	}	
	// hopefully best path is now gotten

	// find tail
	for(tail=head; tail&&tail->next; tail=tail->next);
	
        // set length
        length = 0;
        for(tll=head; tll; tll=tll->next)length++;

	// orient right-hand
	Point to, from, w;

	bbox(from, to);
	w = to - from;
	w *= .5;
	from -= w;
	to += w;
	from.bound(0,MAX(initial.width, initial.height));
	to.bound(0,MAX(initial.width, initial.height));
	w = to - from;
	
	Uimage qux(w.x, w.y);
	qux = 0;
	qux.offset = from;
	qux.draw(*this, 255);
	qux.fill(from, 255);
	qux.invert();
	qux.draw(*this, 255);

	p = head->next->pt - head->pt;
	for(j=0;j<sizeof(nindex)/sizeof(nindex[0]);j++)
                if (p == nindex[j])break;
 
        j += 2; // pi/2 away
        j %= 8;
        p = nindex[j] + head->pt; // point in front of
 
        if( qux[ p ]){
		// reverse orientation
                Curvelet *bonk;
                // debug("reversing orientation\n");
                for(tll=head; tll; tll=tll->prev){
                        // swap next/prev
                        bonk = tll->next;
                        tll->next = tll->prev;
                        tll->prev = bonk;
                }
                // connect ends
                head->next = tail;
                tail->prev = head;
        }else{
                // connect them (other way)
                head->prev = tail;
                tail->next = head;
        }
}

float Curvelet::dfs_read(Uimage& seen, const Bimage& cont){
	// do a DFS to find snake
	
	float cdist, dist=-1;
	int i,j;
	Curvelet *tll, *head=0;
	Point p;

	for(j=-1;j<2;j++) for(i=-1;i<2;i++){
		// try all 8 directions from here
		// we currently take longest path, if too slow
		// we could try first find
		// yeah, lets go with first...

		p = Point(i,j) + pt;
		
		if(!i && !j) continue;
		if( !INRANGE(0, p.x, seen.width) )continue;
		if( !INRANGE(0, p.y, seen.height) )continue;
		if( int(cont.value(p)) && !seen[p]){
			seen[p] = 1;
			tll = new Curvelet;
			tll->pt = p;
			append(tll);
			cdist = tll->dfhls_read(seen, cont);
			if( cdist > dist){
				dist = cdist;
				head->kill();
				head = tll;
				tll->prev = 0;
				next = 0;
				i = j = 2;		// exit loop after first find
			}else{
				delete tll;
			}
			seen[p] = 0;
		}
	}
	

	append(head);
	return head?
		(dist + DIST2( pt, head->pt))
		: 0.0 ;
}


float Curvelet::dfhls_read(Uimage& seen, const Bimage& cont){
	// do a D-F H-L search to find snake
	
	float cdist, dist=-1;
	int i, j, k;
	Point n, pp;
	Curvelet *tll, *head=0;

	pp = prev->pt - pt;
	pp.bound(-1,1);
	
	for(j=0;j<sizeof(nindex)/sizeof(nindex[0]);j++)
		if (pp == nindex[j])break;
	// now have index of previous in j
	// which is, of course, the (negative of) current direction...

	for(k=0;k<sizeof(heuristic)/sizeof(heuristic[0]);k++){

		i = (j + heuristic[k]) % 8 + (heuristic[k] & ~7);
		n = nindex[i] + pt;
		
		if( int(cont.value(n))
		   && !seen[n]){
			seen[n] = 1;
			tll = new Curvelet;
			tll->pt = n;
			append(tll);		// to end of this chain
			cdist = tll->dfhls_read(seen, cont);
			if( cdist > dist){
				dist = cdist;
				head->kill();
				head = tll;
				tll->prev = 0;
				next = 0;
				if( FIRSTONE || h_stops[i] )break;
			}else{
				delete tll;
			}
			seen[n] = 0;
		}
	}

	append(head);
	return head?
		(dist + DIST2( pt, head->pt))
		: 0.0 ;
}

/*
  reference:
   Heuristic Search Approach to Shape Matching in Image Sequences
   S. Chaudhury, S. Subramanian, G. Parthasarathy
   IEE Proceedings-E Vol. 138, No. 2, March 1991 pp 97-105
*/
 
void Shape::moosh(const Contour& c, int resol, float sigma){
        // turn the curve into a shape representation
        // (as per above paper)
 
        if(seg) delete [] seg;
        nsegs = 0;
        Vector xoft(c.length), yoft(c.length);
        int i=0;
 
        for(Curvelet* cv=c.first(); cv && cv->succ() && cv->succ() != c.first(); cv=cv->succ(), i++ ){
                xoft[i] = cv->pt.x;
                yoft[i] = cv->pt.y;
        }
	xoft[i] = cv->pt.x;
	yoft[i] = cv->pt.y;
	
	// low pass filter
        Vector xlpf(c.length), ylpf(c.length);
 
        xlpf = cconvolve(xoft, Vector(c.length, Gaussian(c.length/2, sigma)));
        ylpf = cconvolve(yoft, Vector(c.length, Gaussian(c.length/2, sigma)));
        xlpf = cconvolve(xlpf, Vector(c.length, Delta(c.length/2)));
        ylpf = cconvolve(ylpf, Vector(c.length, Delta(c.length/2)));
 
#ifdef DEBUG
        xoft.save("xoft.dat");
        yoft.save("yoft.dat");
        xlpf.save("xlpf.dat");
        ylpf.save("ylpf.dat");
#endif
	
        // take derivatives (1st and 2nd)
        Vector xd  = xlpf.d();
        Vector yd  = ylpf.d();
        Vector xdd = xlpf.d(2);
        Vector ydd = ylpf.d(2);

#ifdef DEBUG
        xd.save("xd.dat");
        yd.save("yd.dat");
        xdd.save("xdd.dat");
        ydd.save("ydd.dat");
#endif
	
        Vector k(c.length), zc(c.length), zces;
        // determine curvature
        for(i=0;i<k.length;i++){
                k[i] = (xd[i]*ydd[i] + yd[i]*xdd[i])
                        / pow( xd[i]*xd[i] + yd[i]*yd[i] +.00001, 1.5);
        }
        k = cconvolve(k, Vector(k.length, Gaussian(k.length/2, sigma)));
        k = cconvolve(k, Vector(k.length, Delta(k.length/2)));
 
        // and zero x-ings
        int sig = 0;
        i = k.length;
        while( !sig && i) sig = SIGN( k[--i] );
 
        for(i=0;i<zc.length;i++)
                if( SIGN(k[i]) && SIGN(k[i])!= sig){
                        zc[i] = 1;
                        zces[nsegs++] = i;      // note places of zcs
                        sig = SIGN( k[i] );
                }else
                        zc[i] = 0;
#ifdef  DEBUG
        k.save("k.dat");
        zc.save("zc.dat");
#endif
 

        // build segments
 
        if( nsegs > 2 ){
 
                seg = new Segment [ nsegs ];
		// for reference only...
		starts = new Point [ nsegs ];
		ends = new Point [ nsegs ];
		mids = new Point [ nsegs ];
		
                Point prevmid, zromid;
 
                for(i=0;i<nsegs;i++){
                        seg[i].length = resol;
                        seg[i] = 0;
                }
                for(i=0;i<nsegs;i++){
                        int cs = int(zces[i]);
                        int ce = int(zces[(i+1)%nsegs]);
                        Point cstart;
                        Point cend;
                        Point cmid;

			starts[i] = cstart = Point(int(xlpf[cs]), int(ylpf[cs]));
			ends[i] = cend = Point(int(xlpf[ce]), int(ylpf[ce]));
			mids[i] = cmid = (cstart+cend)/2;
			
                        for(int n=0;n<resol;n++){
                                // find closest before point
                                float dangle = float(n)/resol * M_PI;
 
                                int besti;
                                Point pa;
                                if(ce < cs) ce += k.length;
                                for(int m=cs;m<=ce;m++){
                                        pa = Point(int(xlpf[m%k.length]), int(ylpf[m%k.length]));
                                        float da = dangle - angle(cstart-cmid, pa-cmid);
                                        if(da<0){
                                                besti = m-1;
                                                break;
                                        }
                                        besti = m;
                                }
                                seg[i][n] = DIST(pa, cmid);
                                if(n==resol/2){
                                        // determine con-vexity
                                        float tc = fullangle(cstart-cmid, pa-cmid);
                                        if(tc<0)
                                                seg[i].type = Segment::Concave;
                                        else
                                                seg[i].type = Segment::Convex;
 
                                        // tc==0 -> straight
                                }
                        }

 
                        // find angle btwn. segments
                        if(i){
                                seg[i-1].angle = fullangle(prevmid-cstart, cmid-cstart);
                        }else{
                                zromid = cmid;
                        }
                        if(i==nsegs-1)seg[i].angle = fullangle(cmid-cend, zromid-cend);
                        prevmid = cmid;
                }
 
        }else{
                // take care of degenerate cases
 
 
 
        }
 
}


