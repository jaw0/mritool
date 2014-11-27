

// Copyright (c) 1993 Jeff Weisberg
// University of Rochester
//	Dept. of Electrical Engineering
//	Dept. of Neurology, C.E.P.
//
// see the file "License"

#ifdef RCSID
static const char *const rcsid
= "@(#)$Id: snake.supp.cc,v 1.1 1995/11/23 04:30:25 jaw Exp jaw $";
#endif

#include <mritool.h>
#include <window.h>
#include <snake.h>
#include <math.h>
#include <iostream.h>

#define CODE_SECTION SECTION_SNAKE
#define loopp(p, a, b)	for(p.y=a.y;p.y<b.y;p.y++)for(p.x=a.x;p.x<b.x;p.x++)

extern const float sqrt_tabl[100];
const float sqrt_tabl[100] = {
	// lookup table of sqr roots
	// we only need low precision result of small numbers
	// so calling sqrt() would be overkill (and much slower)
	
	0,      1.0000, 1.4142, 1.7320, 2.0000, 2.2360, 2.4494,
	2.6457, 2.8284, 3.0000, 3.1622, 3.3166, 3.4641, 3.6055,
	3.7416, 3.8729, 4.0000, 4.1231, 4.2426, 4.3588, 4.4721,
	4.5825, 4.6904, 4.7958, 4.8989, 5.0000, 5.0990, 5.1961,
	5.2915, 5.3851, 5.4772, 5.5677, 5.6568, 5.7445, 5.8309,
	5.9160, 6.0000, 6.0827, 6.1644, 6.2449, 6.3245, 6.4031,
	6.4807, 6.5574, 6.6332, 6.7082, 6.7823, 6.8556, 6.9282,
	7.0000, 7.0710, 7.1414, 7.2111, 7.2801, 7.3484, 7.4161,
	7.4833, 7.5498, 7.6157, 7.6811, 7.7459, 7.8102, 7.8740,
	7.9372, 8.0000, 8.0622, 8.1240, 8.1853, 8.2462, 8.3066,
	8.3666, 8.4261, 8.4852, 8.5440, 8.6023, 8.6602, 8.7177,
	8.7749, 8.8317, 8.8881, 8.9442, 9.0000, 9.0553, 9.1104,
	9.1651, 9.2195, 9.2736, 9.3273, 9.3808, 9.4339, 9.4868,
	9.5393, 9.5916, 9.6436, 9.6953, 9.7467, 9.7979, 9.8488,
	9.8994, 9.9498  // table generated from bc
};

void Snake::lock(void){

	// need to lock all snakelets as well

	Object::lock();

	Snakelet *sn = box.next;
	while(sn && !sn->locked()){
		sn->lock();
		sn = sn->next;
	}
}

void Snake::unlock(void){

	Object::unlock();
	
	Snakelet *sn = box.next;
	while(sn && sn->locked()){
		sn->unlock();
		sn = sn->next;
	}
}

void Snakelet::kill(void){
	Snakelet *k;
	
	if( !this) return;
	if( next ){
		// otherwise we can loop forever if it is a closed curve...
		k = next;
		next = 0;
		delete k;
	}
	if( prev ) prev->next = 0;
	prev = 0;
	pt = -1;
}

void Snakelet::remove(void){

	if( !this) return;
	if( next ) next->prev = prev;
	if( prev ) prev->next = next;

	prev = next = 0;
	delete this;
}

void Snakelet::append(Snakelet* n){
	Snakelet *foo;
	
	if( !n) return;
	n->prev = this;
	if(next){
		foo=n;
		while( foo->next ) foo=foo->next;
		foo->next = next;
		next->prev = foo;
	}
	next = n;
}

void Snakelet::prepend(Snakelet* n){
	Snakelet *foo;
	
	if( !n) return;
	n->next = this;
	if(prev){
		foo=n;
		while( foo->prev ) foo=foo->prev;
		foo->prev = prev;
		prev->next = foo;
	}
	prev = n;
}

void Snake::nbox(Point& to, Point& from, Point& w, float factor) const {
	
	w = bb_f - bb_i;
	from = bb_i - w*factor;
	to   = bb_f + w*factor;
	to.bound(0,512);
	from.bound(0,512);
	w = to - from;
}

float Snakelet::measure(void) const{

	if(!this) return 0;
	float dist = 0;
	Snakelet* s= (Snakelet*)this;

	while(s->next){
		dist += DIST(s->pt, s->next->pt);
		s = s->next;
		if(s->next == prev) break;
	}
	if(prev) dist += DIST(pt, prev->pt);
	return dist;
}
	
void Snake::shed(){
	// get rid of old skin (and memory)

	if(!box.next) return;
	
	delete box.next;
	edg.data.cleanup();
	
	length = 0;
	box.next = box.prev = 0;
}

Snakelet *Snakelet::leftof() const{
	// find distinct point to the left of here

	if( !prev) return (Snakelet*)this;
	
	if( pt != prev->pt ) return prev;
	if( prev->prev ) return prev->prev;
	return (Snakelet*)this;
}

Snakelet *Snakelet::rightof() const{
	// as above, to the right
	
	if( !next) return (Snakelet*)this;
	if( pt != next->pt ) return next;
	if( next->next ) return next->next;
	return (Snakelet*)this;
}

void Snake::inspect(Snakelet* sn){
	Snakelet *nsl;

	if(length<4) return;
	if(!sn) return;
	
	
	if(sn->next && sn->pt == sn->next->pt || sn->prev && sn->pt == sn->prev->pt){
		if(box.next == sn->next) box.next=sn->next->next;
		if(box.prev == sn->next) box.prev=sn->next->prev;
		sn->next->remove();
		////cerr << "removed " << sn->pt << endl;
		length--;
	}else{
#if 0
		if(sn->next && DIST(sn->pt, sn->next->pt) > DISTFAR){
			nsl = new Snakelet;
			nsl->pt = (sn->pt + sn->next->pt) / 2;
			sn->append(nsl);
			////cerr << "inserted " << nsl->pt << endl;
			length++;
		}
#endif		
		if(sn->prev && DIST(sn->pt, sn->prev->pt) > DISTFAR){
			nsl = new Snakelet;
			nsl->pt = (sn->pt + sn->prev->pt) / 2;
			sn->prepend(nsl);
			////cerr << "inserted " << nsl->pt << endl;
			length++;
		}
	}
	if( !sn->isvalid() || !sn->next->isvalid() || ! sn->prev->isvalid() )
		bug(__FILE__ ": snake::inspect",
		    !sn->isvalid() ? "!sn->isvalid"
		    : !sn->next->isvalid() ? "!sn->next->isvalid"
		    : "!sn->prev->isvalid"
		    );
}

ostream& operator<<(ostream& os, Snake &s){

	os << "Length :	" << s.length << endl;
	os << "Ref.Pt.:	" << s.refpt << endl;
	os << "Displ  :	" << s.displ << endl;
	os << "Loi    :	" << s.ploi << " = " << s.loi << endl;
	os << "BBox   :	" << s.bb_i << "::" << s.bb_f << endl;
	return os;
}


extern "C" void snake(Xv_opaque it){
	// from button
	char buffer[512];
	int what;
	XView& m = me(it);
	
	m.thinking();
	
	what  = (int)xv_get(m.snak_what_i,  PANEL_VALUE, 0);

	sprintf(buffer, "(progn (mouse:close-contour) (snake%s $A $B $G $D $S ) (refresh))",
		what ? "-range $R" : "-slice $F"
		);
	
	xv_set(m.snak_popup, XV_SHOW, FALSE, 0);
	xv_set(m.snak_bounds_popup, XV_SHOW, FALSE, 0);
	XFlush(m.dpy);	// get rid of popup
	m.interpret(buffer);
}


void Snake::illiterate(Uimage& draw1, int c) const {

	// convert vector snake back to raster image
	// is the reverse of reader

	Snakelet *s;
	int i=0;

	if( length < 4) return;
	
	for(s=box.next; s && s->next && s->next !=box.next; s=s->next, i++){
		draw1.line(s->pt, s->next->pt, c);
	}
	// assume closure
	if( box.next->prev){
		draw1.line(box.next->pt, box.next->prev->pt, c);
	}
}


void Snake::edgemap(int lev, Uimage& segm){

	if(length < 4)return;
	
	Point to, from, w;
	Point p;

	nbox(to, from, w, .5);
	
	edg.level = lev;
	edg.data.width  = w.x;
	edg.data.height = w.y;
	edg.data.offset = 0;
	edg.data.resize();

	segm.offset = 0;	// make sure

	loopp(p, Point(0), w){
		edg.data[p]
			= (int(segm[ (p + from)/2 ]) // going from 256 -> 512 space
			== lev) ? 255 : 0;					 // punch
	}

	edg.data.offset = from;
	edg.data.median(7);
	edg.data.edge(380);
	edg.data.blur_nl(MIN(w.x, w.y)/2, 16);

	debugp( DBG_SAVE_X )
		edg.data.save("=d/edge.ras");
}

#if 0
void Snake::prune(void){

	Snakelet *sn=box.next, *tail=box.next->prev;

	do{
		Snakelet *lk;
		for(lk=sn->next;lk!=sn;lk=lk->next){
			if(sn->pt==lk->pt){
				// found 2 at same spot
				// something need be done!

				// find  bbs
				Point amin(512), amax(0), w;
				Snakelet *sk;
				for(sk=sn;sk!=lk->next;sk=sk->next){
					if( sk->pt.x < amin.x ) amin.x = sk->pt.x;
					if( sk->pt.y < amin.y ) amin.y = sk->pt.y;
					if( sk->pt.x > amax.x ) amax.x = sk->pt.x;
					if( sk->pt.y > amax.y ) amax.y = sk->pt.y;
				}
				w = amax - amin + 2;
				Uimage a(w.x, w.y);
				a = 0;
				a.offset = amin - 1;
				for(sk=sn;sk!=lk;sk=sk->next)
					a.line(sk->pt, sk->next->pt, 255);
				a.fill(amin-1, 255);

				for(sk=sn;sk!=lk->prev;sk=sk->prev){
					if( sk->pt.x < amin.x ) amin.x = sk->pt.x;
					if( sk->pt.y < amin.y ) amin.y = sk->pt.y;
					if( sk->pt.x > amax.x ) amax.x = sk->pt.x;
					if( sk->pt.y > amax.y ) amax.y = sk->pt.y;
				}
				w = amax - amin + 2;
				Uimage b(w.x, w.y);
				b = 0;
				b.offset = amin -1;
				for(sk=sn;sk!=lk;sk=sk->prev)
					b.line(sk->pt, sk->prev->pt, 255);
				b.fill(amin-1, 255);

				// ok, now find the areas
				int aarea=a.width*a.height, barea=b.width*b.height;
				int i;
				for(i=0;i<a.width*a.height;i++)
					if(a[i]) aarea--;
				for(i=0;i<b.width*b.height;i++)
					if(b[i]) barea--;

				// now remove the smaller one (and both points)
				if(aarea < barea){
					// remove right half
					Snakelet *dn=lk->next;
					for(sk=sn;sk!=dn;sk=sk->next){
						if(box.next == sk) box.next=sk->next;
						if(box.prev == sk) box.prev=sk->prev;
						sk->remove();
						length--;
					}
				}else{
					// remove left half
					Snakelet *dn=lk->prev;
					for(sk=sn;sk!=dn;sk=sk->prev){
						if(box.next == sk) box.next=sk->next;
						if(box.prev == sk) box.prev=sk->prev;
						sk->remove();
						length--;
					}
				}
			}// oe if ==
		}
		sn=sn->next;
	}while(sn!=tail);
}
#else

int Snake::prune(void){
	Point to, from, w, p;
	Snakelet *sn=box.next, *ns, *ls;
	int chd = 0;
	
	nbox(to, from, w, .5);

	Uimage a(w.x, w.y), b(w.x, w.y), c(w.x, w.y);	
	a.offset = c.offset = from;
	a = 0;

	// bb_f = from;	// recalc these
	// bb_i = to;
	
	illiterate(a, 255);
	a.fill(a.offset, 255);
	c = a;
	
	a.invert();		// detect outer 
	a.median(5);
	illiterate(a, 0); 
	a.dilate(5);

	c.median(5);		// and inner squiggliness
	illiterate(c,0);
	c.dilate(5);


	b = a;
	illiterate(b, 128);
	debugp( DBG_SAVE )
		b.save("=d/foo.ras");
	
	do {
		ns = sn->next;
		if( sn->pt.x < bb_i.x)bb_i.x = sn->pt.x;
		if( sn->pt.y < bb_i.y)bb_i.y = sn->pt.y;
		if( sn->pt.x > bb_f.x)bb_f.x = sn->pt.x;
		if( sn->pt.y > bb_f.y)bb_f.y = sn->pt.y;

		if( !a[ sn->pt ] || !c[ sn->pt ] ){
			// squigglyp ?
			// remove it
			if(box.next == sn) box.next=sn->next;
			if(box.prev == sn) box.prev=sn->prev;
			sn->remove();
			length--;
			chd ++;
		}else if( nindexof(sn->pt - sn->rightof()->pt, 8) == nindexof(sn->pt - sn->rightof()->pt, 8)){
			// pointyp ?
			// remove it
			if(box.next == sn) box.next=sn->next;
			if(box.prev == sn) box.prev=sn->prev;
			sn->remove();
			length--;
			chd ++;
			
		}else{
			//  make sure points are unique
			for(ls=ns; ls != sn; ls = ls->next){
				if( ls->pt == sn->pt ){
					// remove it
					if(box.next == sn) box.next=sn->next;
					if(box.prev == sn) box.prev=sn->prev;
					sn->remove();
					length--;
					chd ++;
					break;
				}
			}
		}
				
		sn=ns;
	}while(sn!=box.next);

	return chd;
}

#endif



