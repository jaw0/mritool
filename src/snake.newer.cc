

// Copyright (c) 1993 Jeff Weisberg
// University of Rochester
//	Dept. of Electrical Engineering
//	Dept. of Neurology, C.E.P.
//
// see the file "License"

#ifdef RCSID
static const char *const rcsid
= "@(#)$Id: snake.newer.cc,v 1.1 1995/11/23 04:30:24 jaw Exp jaw $";
#endif

// Optimization: Full

#include <mritool.h>
#include <segment.h>
#include <window.h>
#include <snake.h>
#include <math.h>
#include <iostream.h>

#include <jl_proto.h>
#include <defproto.h>

#define CODE_SECTION	SECTION_SNAKE
#define loopp(p, a, b)	for(p.y=a.y;p.y<b.y;p.y++)for(p.x=a.x;p.x<b.x;p.x++)

extern void ras_save(char *n, u_char *i, int x, int y);
void pretty_line(Uimage& initial, Uimage& img, int c);
extern const float sqrt_tabl[100];
extern int got_a_signal;

static const Point nghbrs[] = {
	// the 8 neighbors of a point in CCw direction
	0,0, // skip points 0, 1
	Point(0,-1), Point(-1,-1), Point(-1,0), Point(-1,1),
	Point(0,1),  Point(1,1),   Point(1,0),  Point(1,-1)
};


void UserP::snakit(int alpha, int beta, int gamma, int delta, int nloi, 
		   int fr){
	// snake out an image
	// offline processing not yet supported (flag ignored)
	if(!this || !c_image) return;
	
	int i, j=c_image->num, w,h;
	int num = c_image->num;
	Image *islice;
	Bolus bo;
	bo.alpha=alpha; bo.beta=beta; bo.gamma=gamma;
	bo.nloi = nloi;
	
	bo.delta=fr?delta:0;	// no pcont term if using drawn

	debugp( DBG_THINK )
		ui->thinking("Snaking...");
	
	c_image->doit();		// load image & segm if needed
	c_image->lock();		// notify GC
	
	if(!c_image->T(Image::IFB_SEGM_PRESENT)){
#		if 0
		//	mike suggests we just do the segm automatically
#		else		
			ui->error("Segmented Image is not available",
				  "I cannot snake it" );
			return;
#		endif			
	}
	
	
	// we need an initial contour, let's find one!
	// fr ==
	//	 0 -> this slice
	// 	 1 -> previous slice
	// 	 2 -> next slice
	
	j = (fr==1) ? (num-1) : (fr==2) ? (num+1) : num;	
	j %= c_series->num_of_images;	// slice number to use
	islice = c_series->find(j);		// find the slice
	
	if( islice->T(Image::IFB_DRAWN_ON) ){
		// close it up if needed
		islice->S(Image::IFB_CONTOUR_PRESENT);
		// need to close the contour here
		// lisp coded close-contour needed...
		// 	islice->close_contour(*ui);
		
	}else if (!islice->T(Image::IFB_CONTOUR_PRESENT)){
		ui->error("You selected an initial contour",
		 	  "that does not exist");
		return;
	}
	
	islice->lock();
	Uimage& initial = islice->drawing2;
	
	
////// BEGIN SNAKE PROCESSING
	
	for(i=0;i<c_image->num_of_snakes;i++){
		if( 1 ){ 
			// used to check length, do not want to reformat code ...

			if (fr){
				c_image->snake[i].loi  = islice->snake[i].loi;
				if( c_image->snake[i].loi == -1 ) continue;
				debugp( DBG_WORK )
					ui->thinking("estimating displacement...");
				c_image->snake[i].displ_guess(islice->snake[i], c_image->segm, c_image->snake[i].loi);
			}else{
				c_image->snake[i].displ = 0;
				c_image->snake[i].refpt = -1;
				if( c_image->snake[i].loi == -1) continue;
			}

			debugp( DBG_WORK )
				ui->thinking("reading...");
			c_image->snake[i].reader(initial,i+1);
			if(fr)
				// expand no more than 50%
				c_image->snake[i].expand(islice->segm, c_image->segm, c_image->snake[i].loi, .5 );
			
			if( c_image->snake[i].length){
				debugp( DBG_STATS ){
					cerr << "Slice #" << c_image->num <<"	Snake #" << i << endl;
					cerr << c_image->snake[i] ;
					cerr << "" << endl;
				}
			}

			debugp( DBG_WORK )
				ui->thinking("creating edgemap...");
			c_image->snake[i].edgemap(c_image->snake[i].loi, c_image->segm);

			// 4 is arbitrary small number
			if(c_image->snake[i].length < 4) continue;

			Point p;
			Fimage pcont(0,0);
			if(bo.delta){
				
				debugp( DBG_WORK )
					ui->thinking("Generating pcont");
				// generate pcont
				// pcont = blurred initial
				// just do blur in area of BB
				Point to, from, w, qux;
				int xx,yy;
				
				c_image->snake[i].nbox(to, from, w, .5);
				qux = from; 

				pcont.width = w.x;
				pcont.height = w.y;
				pcont.resize();

				for(yy=0;yy<w.y;yy++) for(xx=0;xx<w.x;xx++)
					pcont[yy*w.x + xx]
						= (initial[ (yy+qux.y)*512 + (xx+qux.x) ]==i+1)?255:0;

				pcont.blur_nl(MIN(w.y,w.x)/2, 4.0);
				pcont.offset = from;

				debugp( DBG_SAVE_X )
					pcont.save("=d/pcont.ras");
			}

			debugp( DBG_WORK )
				ui->thinking("creating tain");
			
			loopp(p, Point(0), Point(512)){
				c_image->snake[i].tain[ p ] = c_image->segm[ p/2 ]==c_image->snake[i].loi ? 255 : 0;
			}
			debugp( DBG_SAVE_X )
				c_image->snake[i].tain.save("=d/tain.ras");
			
			debugp( DBG_WORK )
				ui->thinking("minimizing...");
			bo.pcont = &pcont;
 			c_image->snake[i].minimate(bo);
			cerr << c_image->snake[i];

			c_image->snake[i].re_est(c_image->segm, c_image->snake[i].loi);

		}  // if(1)
		if( c_image->snake[i].length){
			debugp( DBG_STATS ){
				cerr << endl;
				cerr << "Slice #" << num <<"	Snake #" << i << endl;
				cerr << c_image->snake[i] ;
				cerr << "" << endl;
			}
		}
		// clear old snake off
		for(j=0; j<c_image->drawing2.width*c_image->drawing2.height; j++){
			if( c_image->drawing2[ j ] == i+1 ) c_image->drawing2[ j ] = 0;
		}
		debugp( DBG_WORK )
			ui->thinking("illiterating...");
		c_image->snake[i].illiterate(c_image->drawing2, i+1);
		
	} // for num of snakes

	for(i=0;i<c_image->num_of_snakes;i++){
		c_image->snake[i].tain.cleanup();
	}
	c_image->drawing = 0;
	for(j=0; j<c_image->drawing2.width*c_image->drawing2.height; j++){
		u_char quux = c_image->drawing2[ j ];
		c_image->drawing[ j ] = quux ? 0x80 : 0;
	}
	
	debugp( DBG_SAVE_X )
		c_image->drawing.save("=d/drawinga.ras");
	
	c_image->drawing.dilate( CINT(getvalue(maksym("mri:line-width"))));

	if(ui->c_image == c_image)
		ui->set_contour(c_image->drawing);
	
	debugp( DBG_SAVE_X )
		c_image->drawing.save("=d/drawingb.ras");
	debugp( DBG_SAVE )
		c_image->drawing2.save("=d/drawing2.ras");
	c_image->C(Image::IFB_DRAWN_ON);	// mark snake as valid
	c_image->S(Image::IFB_CONTOUR_PRESENT);

	debugp( DBG_THINK )
		ui->done_thinking();
	
	islice->unlock();
	c_image->unlock();
}


void Snake::minimate(Bolus bo){
	// minimize energy

	got_a_signal = 0;
	if( length < 4 ) return;
	
	int i, j, k, schg=0, lap=0, chgd=0;
	Point best, here;
	float nrg_b, nrg_c, nrg_h;
	Snakelet *tl;

	int maxiter = CINT(getvalue(maksym("mri:maxiter")));
	int minpercent = CINT(getvalue(maksym("mri:percent")));
	Obj stop_sym = maksym("mri:stop-iterating");
	
	bo.edge = &edg;
	bo.tain = &tain;

	i = 0;
	tl = box.next;
	while(1){
		if(lap > maxiter) break;
		if(float(schg) > float(minpercent*length/100.00)) break;

		if( getvalue(stop_sym)==IC_TRUE){
			Fset(stop_sym, IC_FALSE);
			break;
		}
		if(length>300){
			// chaos protection
			shed();
			break;	
		}
		
		// calc nrg of here and neighbors;

		// here
		bo.orig = tl->pt;
		nrg_b = nrg_h = tl->energy(bo);
		best = here = tl->pt;

		for(j=-1;j<2;j++)for(k=-1;k<2;k++){
			// try 8 neighbors
			if (!j && !k) continue;
			tl->pt = here + Point(j,k);
			tl->pt.bound(0,512);
			nrg_c = tl->energy(bo);
			if( nrg_c < nrg_b){
				nrg_b = nrg_c;
				best = tl->pt;
			}
		}
		
		// which place is best?
		if ( nrg_h != nrg_b ){
			// elsewhere was better
			
			tl->pt = best;
			schg = 0;
			chgd ++;
			inspect(tl);
		}else{
			// current is better
			tl->pt = here;
			schg++;
		}
		i++;
		i %= length;
		if(tl->next)
			tl = tl->next;
		else
			tl= box.next;
		if(!i){
			lap++;
			debugp( DBG_STATS ){
				cerr << lap
				     << ( (lap%10==1)?"st":(lap%10==2)?"nd":(lap%10==3)?"rd":"th" )
				     << " lap, moved " << chgd + prune() << " points (of " << length
				     << ")" << endl;
			}else{
				prune();
			}
			chgd = 0;
		}
	}
}

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

static const Point nindex[] = {
	// this is the above table
	
	Point(0, -1), Point(-1, -1), Point(-1, 0), Point(-1, 1),
	Point(0, 1), Point(1, 1), Point(1, 0), Point(1, -1),
		
	Point(0, -2), Point(-2, -2), Point(-2, 0), Point(-2, 2),
	Point(0, 2), Point(2, 2), Point(2, 0), Point(2, -2),

	Point(-1, -2), Point(-2, -1), Point(-2, 1), Point(-1, 2),
	Point(1, 2), Point(2, 1), Point(2, -1), Point(1, -2)
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

void Snake::reader(Uimage& initial, int c){	
	// convert contour image into vector snake


	Uimage seen(512, 512);
	int i,j;
	float dist=0, cdist;
	Snakelet *tll, *head=0, *tail;
	Point p, fini=initial.offset+Point(initial.width, initial.height);
	
	shed();
	tain.resize();
	seen = 0;

	debugp( DBG_SAVE_X )
		initial.save("=d/initial.ras");
	
	if(c)
		// hidden-option
		pretty_line(initial, tain, c);

	loopp(p, initial.offset, fini){
		// search over all points for the snake
		if(tain[p]){
			tll = new Snakelet;
			tll->pt = p;
			seen[p] = 1;
			cdist = tll->dfs_read(seen, tain);
			if( cdist > dist){
				dist = cdist;
				head->kill();
				head = tll;
			}else{
				delete tll;
			}
			seen[p] = 0;

			if( dist > 100 ) p = fini; // exit loop 
		}
	}	
	// hopefully best path is now gotten

	// find tail
	for(tail=head; tail&&tail->next; tail=tail->next);


	// put snakelets in a box for safe keeping
	box.next = head;
	box.prev = tail;

	// now, lets learn about it
	// how big is he, where is he, ...
	length = 0;
	bb_f = 0;
	bb_i = fini;
	for(tll=head; tll; tll=tll->next){
		length++;
		// adjust by displ
		tll->pt += displ; 	

		if( tll->pt.x < bb_i.x)bb_i.x = tll->pt.x;
		if( tll->pt.y < bb_i.y)bb_i.y = tll->pt.y;
		if( tll->pt.x > bb_f.x)bb_f.x = tll->pt.x;
		if( tll->pt.y > bb_f.y)bb_f.y = tll->pt.y;
	}
	
	if(length < 4) return;
#if 1	
	// orient it in right-hand direction
#if 1	
	Point to, from, w;
	nbox(to, from, w, .5);
	Uimage qux(w.x, w.y);

	qux = 0;
	qux.offset = from;
	illiterate(qux, 255);
	qux.fill(from, 255);
	qux.invert();
	illiterate(qux, 255);

	debugp( DBG_SAVE_X )
		qux.save("=d/pretty.ras");

#else
	tain = 0;
	illiterate(tain, 255);
	tain.fill(ploi, 255);
#endif	
	p = head->next->pt - head->pt;
	// p.bound(-1, 1);
	
	for(j=0;j<sizeof(nindex)/sizeof(nindex[0]);j++)
		if (p == nindex[j])break;
	
	j += 2;	// pi/2 away
	j %= 8;
	p = nindex[j] + head->pt; // point in front of

	if( qux[ p ]){
#else
	if( 0 ){
#endif			
		// reverse orientation
		Snakelet *bonk;
		debugp( DBG_WORK )
			cerr << "reversing orientation\n" << endl;
		
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

float Snakelet::dfs_read(Uimage& seen, Uimage& cont){
	// do a DFS to find snake
	
	float cdist, dist=-1;
	int i,j;
	Snakelet *tll, *head=0;
	Point p;

	for(j=-1;j<2;j++) for(i=-1;i<2;i++){
		// try all 8 directions from here
		// we currently take longest path, if too slow
		// we could try first find
		// yeah, lets go with first...

		p = Point(i,j) + pt;
		
		if(!i && !j) continue;
		if( !INRANGE(0, p.x, 512) )continue;
		if( !INRANGE(0, p.y, 512) )continue;
		if( cont[p] && !seen[p]){
			seen[p] = 1;
			tll = new Snakelet;
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

float Snakelet::dfhls_read(Uimage& seen, Uimage& cont){
	// do a D-F H-L search to find snake
	
	float cdist, dist=-1;
	int i, j, k;
	Point n, pp;
	Snakelet *tll, *head=0;

	pp = prev->pt - pt;
	pp.bound(-1,1);
	
	for(j=0;j<sizeof(nindex)/sizeof(nindex[0]);j++)
		if (pp == nindex[j])break;
	// now have index of previous in j
	// which is, of course, the (negative of) current direction...


	for(k=0;k<sizeof(heuristic)/sizeof(heuristic[0]);k++){

		i = (j + heuristic[k]) % 8 + (heuristic[k] & ~7);
		n = nindex[i] + pt;
		
		if( cont[n]
		   && !seen[n]){
			seen[n] = 1;
			tll = new Snakelet;
			tll->pt = n;
			append(tll);		// to end of this chain
			cdist = tll->dfhls_read(seen, cont);
			if( cdist > dist){
				dist = cdist;
				head->kill();
				head = tll;
				tll->prev = 0;
				next = 0;
			}else{
				delete tll;
			}
			seen[n] = 0;
		}
		if( h_stops[i] && (dist > 1)) break;
	}

	
	append(head);
	return head?
		(dist + DIST2( pt, head->pt))
		: 0.0 ;
}

float Snakelet::energy(Bolus& bo) const {

	// claculate local energy (in the area of x,y)

	float nrg;
	Snakelet *left, *right;
	int count = 0;
	
	left  = leftof ();
	right = rightof();

	// calc n-loi term....
	// this might work (it will be slowwww)
	// or more likely it will just crash and die
	
	if(bo.nloi &&
	   pt!=right->pt && pt!=left->pt && right->pt!=left->pt
	   && !( bo.tain->operator[](pt)
		 && bo.tain->operator[](left->pt)
		 && bo.tain->operator[](right->pt)
		 )
	   ){
		Point bba, bbb, w, p;
		
		bba.x = MIN(left->pt.x, MIN(right->pt.x, pt.x));
		bba.y = MIN(left->pt.y, MIN(right->pt.y, pt.y));
		bbb.x = MAX(left->pt.x, MAX(right->pt.x, pt.x));
		bbb.y = MAX(left->pt.y, MAX(right->pt.y, pt.y));
		w = (bbb - bba) + 4;

		Uimage foo( w.x, w.y);
		foo.offset = bba - 2;
		foo = 0;
		
		foo.line(left->pt, right->pt, 255);
		foo.line(right->pt, pt, 255);
		foo.line(pt, left->pt, 255);

		foo.fill(bba-2, 255);
		
		foo.line(left->pt, right->pt, 0);
		foo.line(right->pt, pt, 0);
		// foo.line(pt, left->pt,  0);
		
		loopp(p, bba, bbb){
			if( bo.tain->operator[]( p )) foo[p] = 255;
			// count "n-loi"
			if( !foo[p]) count++;
		}

		// find sign of this mass
		// use cross product = |a|*|b|*sin(theta)*<right hand>
		// = < 0, 0, ax*by - ay*bx >
		// use sign of z
		// this _ought_ tell us if it is toward or away from the aoi

		bba = right->pt - left->pt;
		bbb = pt - left->pt;

		if( bba.x*bbb.y - bba.y*bbb.x < 0 )
			count = -count;
	}
	
	nrg =     penergy(bo)        
		+ left ->penergy(bo)
		+ right->penergy(bo)
		// + bo.alpha * (bo.alpha ? measure() / 10.0 : 0)
		- bo.beta  * ((bo.beta  && bo.edge->data.within(pt)) ? bo.edge->data[ pt ] : 0)
		- bo.delta * ((bo.delta && bo.pcont->within(pt)) ? bo.pcont->operator[](pt): 0)
		- bo.nloi  * count
		
		;
	
	return nrg;
}


float Snakelet::penergy(Bolus& bo) const {

	if(!bo.gamma)return 0;
	
	// calculate energy term for this point
	// E = distance to neighbors,
	//   value of edgemap,
	//   angle with neighbors (a measure of 2nd order discontin.)
	//   and 3d term
 
	Point here=pt, left=leftof()->pt, right=rightof()->pt;
	Point rright=rightof()->rightof()->pt;
	Point lleft = leftof()->leftof()->pt;

#if 0
	float nrg;
	
	nrg = fullangle( right-here,   left-here )
		- fullangle( rright-right, here-right)
		- fullangle( here-left,    lleft-left);
	nrg = bo.gamma * ABS( nrg );
#else	
	float nrg, c_ang, d_l, d_r;
	int d_l2, d_r2, c2, abc;

	
	d_l2 = DIST2(here, left);
	d_r2 = DIST2(here,right);
	c2   = DIST2(right,left);
	d_l  = SQRT( d_l2 );
	d_r  = SQRT( d_r2 );
	abc  = (d_l*d_r <20000) ? (4 * d_l2 * d_r2) : (20000*20000);
	
	c_ang =	abc?( d_l2 + d_r2 - c2 ) /(float)SQRT(abc) // = cos( theta )
		: -1;
	
	nrg =     bo.gamma * (float)(c_ang + 1) / 2.0	// cos( theta / 2 )
		+ bo.alpha * (float)( d_l + d_r ) * (abc?1.0:0.5)
		;
#endif	
	return nrg;
}

void Snake::displ_guess(const Snake &pc, Uimage& segm, int l){

	if(pc.length <4 || pc.refpt==-1){
		displ = 0;
		refpt = -1;
		return;
	}

	// make an edgemap and find its COM
	Point to, from, w;
	int xx,yy;
	// look in the spot where the previous snake was
#if 1
	pc.nbox(to, from, w, .5);
#else
	// try a smaller region
	pc.nbox(to, from, w, 0);
#endif

	Fimage edgmp(w.x, w.y);
	Point p;
	segm.offset = 0;		// make sure

	// get region from segm
	loopp(p, Point(0), w){
		edgmp[p]
			= (int( segm[ (p + from)/2 ] )
			== l) ? 255 : 0;					 // punch
	}

#if 0
	// use edge of object
	edgmp.edge(380);
#endif

#if 1
	edgmp.blur(MIN(w.x, w.y)/4, 16);
#endif	
	
	// now find COM
	float xt=0, yt=0, tt=0, wt;
	
	for(yy=0;yy<w.y;yy++) for(xx=0;xx<w.x;xx++){
		wt = edgmp[ yy*w.x +xx ];
		if( wt!=0.0 ){
			xt += wt*xx;
			yt += wt*yy;
			tt += wt;
		}
	}
	Point foo;

	refpt = Point(xt, yt) / tt + from;
	if(pc.refpt==0) foo = refpt;
	else foo = pc.refpt;
	displ = refpt - foo;
	ploi  = pc.ploi + displ;

}


void Snake::re_est(Uimage& segm, int l){
	// recalc various params of the snake

	if(length < 4) return;

	int i;
	Snakelet *sn;

	bb_i = 512;
	bb_f = 0;
	
	// find bounding box
	for(i=0, sn=box.next;
	    i<length;
	    i++, sn=sn->next){

		if(sn->pt.x > bb_f.x) bb_f.x = sn->pt.x;
		if(sn->pt.y > bb_f.y) bb_f.y = sn->pt.y;
		if(sn->pt.x < bb_i.x) bb_i.x = sn->pt.x;
		if(sn->pt.y < bb_i.y) bb_i.y = sn->pt.y;
	}

	if(refpt==-1) refpt = 0;	// clear refpt (must not be -1)
	displ_guess(*this, segm, l);
}


void pretty_line(Uimage& initial, Uimage& img, int c){
	int x,y,i, j, mvd, pass=0;
	int ns, cl, xc, pl;
	int cl2, xc2, cl4, xc4, pl2, pl4;
	u_char nimg[512*512];

	// clean up the contour a bit

	for(i=0;i<512*512;i++)img[i]=((nimg[i]=0),((initial[i]==c)?1:0));

	// thinning
#if 1	
	do{
		mvd = 0;

		for(y=2;y<510;y++)for(x=2;x<510;x++){
			nimg[y*512+x] = img[y*512+x];
			if(img[y*512+x]){
				ns = xc = xc2 = xc4 = 0;
				cl = img[(y+nghbrs[9].y)*512+(x+nghbrs[9].x)];
				cl2 = img[(y+nghbrs[9].y+nghbrs[2].y)*512+(x+nghbrs[9].x+nghbrs[2].x)];
				cl4 = img[(y+nghbrs[9].y+nghbrs[4].y)*512+(x+nghbrs[9].x+nghbrs[4].x)];
				for(i=2;i<10;i++){
					pl  = img[(y+nghbrs[i].y)*512+(x+nghbrs[i].x)];
					pl2 = img[(y+nghbrs[i].y+nghbrs[2].y)*512+(x+nghbrs[i].x+nghbrs[2].x)];
					pl4 = img[(y+nghbrs[i].y+nghbrs[4].y)*512+(x+nghbrs[i].x+nghbrs[4].x)];
					
					ns += pl;		// count nghbrs
					if( (!cl)  && pl  ) xc ++;	// count 0->1 transitions
					cl = pl;
					if( (!cl2) && pl2 ) xc2 ++;	// count 0->1 transitions
					cl2 = pl2;
					if( (!cl4) && pl4 ) xc4 ++;	// count 0->1 transitions
					cl4 = pl4;
					
				}
				
				if (ns>=3 && ns<=5
				    && xc==1
				    &&(
				       !(
					     img[(y+nghbrs[2].y)*512+(x+nghbrs[2].x)]
					  && img[(y+nghbrs[4].y)*512+(x+nghbrs[4].x)]
					  && img[(y+nghbrs[8].y)*512+(x+nghbrs[8].x)]
				       )
				       ||(
					  xc2 != 1
				       )
				       
				    )
				    &&(
				       !(
					     img[(y+nghbrs[2].y)*512+(x+nghbrs[2].x)]
					  && img[(y+nghbrs[4].y)*512+(x+nghbrs[4].x)]
					  && img[(y+nghbrs[6].y)*512+(x+nghbrs[6].x)]
				       )
				       ||(
					  xc4 != 1
				       )
				       
				    )
				){ 		 // (((this) is) (starting (to) (look (like ('lisp)))))
					// delete pixel
					nimg[y*512+x] = 0;
					mvd++;
				}
			}
		}
		for(i=0;i<512*512;i++)img[i]=nimg[i];

	}while(mvd && ++pass<10);
#endif
	int rl, ml;
	Point p;
	static const int lpos[4][5] = {
		{8, 9, 2, 3, 4 },
		{4, 5, 6, 7, 8 },
		{6, 7, 8, 9, 2 },
		{2, 3, 4, 5, 6 }
	};
#if 0
	// I keep pruning off the *entire* contour, this is a Bad Thing ...
	
	// pruning
	pass=0;
	do {
		mvd = 0;
		for(y=1;y<511;y++)for(x=1;x<511;x++){
			nimg[y*512+x] = img[y*512+x];
			if( img[y*512+x]){
				ml = 0;
				for(j=0;j<4;j++){
					rl = 0;
					for(i=0;i<5;i++){
						p = nghbrs[ lpos[j][i] ];
						if(!img[(y+p.y)*512+(x+p.x)])
							rl ++;
					}
					if(rl>ml) ml = rl;
				}
				if( ml >= 5){
					nimg[y*512+x] = 0;
					mvd++;
				}
			}
		}
		for(i=0;i<512*512;i++)img[i]=nimg[i];
	}while(mvd && ++pass<10);
#endif


}	

int nindexof(Point p, int res){
	int j;

	if(!res) res = sizeof(nindex)/sizeof(nindex[0]);
	
	for(j=0; j<res; j++)
		if(p==nindex[j]) return j;

	return -1;
}

#if 1
void Snake::expand(const Bimage& isegm, const Bimage& msegm, int loi, float maxexp){
	Snakelet *s;
	Point to, from, w;
	int done = 0, far = 0;
	Uimage foo(512, 512), bar(512, 512);
	
	debugp( DBG_SAVE_X ){
		illiterate(foo, 128);
	}
	nbox(to, from, w, maxexp);
	
	while(!done){
		for(s=box.next; !done && s && s->next && s->next != box.next; s=s->next){
			Point away;
			int j;

			if(s==box.next) away = s->rightof()->pt - s->pt;
			if(s==box.next->prev) away = s->pt - s->leftof()->pt;
			else
				away = s->rightof()->pt - s->leftof()->pt;

			away.bound(-1, 1);
			j = nindexof(away, 8);
			j += 2;	// pi/2 away
			j %= 8 /*sizeof(nindex)/sizeof(nindex[0])*/ ;

			if(   s->pt.x < from.x || s->pt.x > to.x
			   || s->pt.y < from.y || s->pt.y > to.y ){
				done = 1;
				debugp( DBG_WORK )
					cerr << "Expansion done, bounding box hit" << endl;
			}
			if( isegm.nvalue( (s->pt - displ)/2 ) != loi){
				// it looks like an edge point -- move away
				s->pt += nindex[j];
				if( msegm.nvalue( s->pt/2 ) == loi ){
					done = 1;
					debugp( DBG_WORK )
						cerr << "Expansion done, hit region" << endl;
				}
			}
		}
		prune();
		debugp( DBG_SAVE_X ){
			bar = foo;
			illiterate(bar, 255);
			bar.save("=d/expfoo.ras");
		}
	}

	debugp( DBG_SAVE_X ){
		illiterate(foo, 255);
		foo.save("=d/exped.ras");
	}
}
		
#else
void Snake::expand(const Bimage& isegm, const Bimage& msegm, int loi, float maxexp){
	Point to, from, w;
	nbox(to, from, w, maxexp);
	
	Uimage img(w.x+2, w.y+2), umg(w.x+2, w.y+2);
	img.offset = umg.offset = from - Point(1,1);

	umg = 0;
	illiterate(umg, 255);
	umg.fill(from, 255);
	umg.invert();
	
	// do a limited dilation
	int x,y, i,j;
	int done = 0;

	debugp( DBG_SAVE_X )
		isegm.save("=d/isegm.ras");
	debugp( DBG_SAVE_X )
		msegm.save("=d/msegm.ras");
	
	while(!done){
		img = umg;
		debugp( DBG_SAVE )
			img.save("=d/foo.ras");
		
		for(y=from.y+1; y<to.y-1; y++) for(x=from.x+1; x<to.x-1; x++){

			// dilate this point ?
			if( int(img(x,y))==255 && int(isegm.nvalue( (Point(x,y)-displ)/2 )) != loi ) {
				for(j=-1;j<=1;j++)for(i=-1;i<=1;i++){
					Point np(x+i, y+j);
					
					// is point already set ?
					if( ! int(img[np]) ){
						umg[np] = 255;

						// Are we there yet ?
						if(   np.x <= from.x || np.x >= to.x
						   || np.y <= from.y || np.y >= to.y ){
							if(!done)
								debugp( DBG_WORK )
									cerr << "Expansion done, hit bounding box" << endl;
							done = 1;
						}
			
						// have we hit region yet ?
						// 	this test is no good -- we are always there
						// 	this is the problem with trying to  use dilation...
						if( int(msegm.nvalue( np/2 )) == loi ){

							if(!done)
								debugp( DBG_WORK )
									if(!done) cerr << "Expansion done, hit region" << endl;
							done = 1;
						}
					}
				}
			}
		}
		debugp( DBG_SAVE_X ) umg.save("=d/expfoo.ras");
	}

	umg.edge(128);
	debugp( DBG_SAVE ) umg.save("=d/exped.ras");

	Uimage kludge(512, 512);
	kludge = 0;
	kludge.overlay(umg, umg.offset);

	Point td = displ, tr = refpt;
	displ =  0;
	reader(kludge, 255);
	displ = td;
	refpt = tr;
}
#endif








	
