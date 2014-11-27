
// Copyright (c) 1993 Jeff Weisberg
// University of Rochester
//	Dept. of Electrical Engineering
//	Dept. of Neurology, C.E.P.
//
// see the file "License"

#ifdef RCSID
static const char *const rcsid
	= "@(#)$Id: analyze.cc,v 1.1 1995/11/23 04:30:13 jaw Exp jaw $";
#endif

#include <mritool.h>
#include <window.h>
#include <solid.h>

// Optimization: Full

#define loop(i,j,k) for(i=j;i<k;i++)	
#define SURFACE	2

int vol_fill(u_char***, int, int, int);

void Series::analyze(UserI& ui, int obj, int l, int h){

	l = l<1?1:l;
	h = h>num_of_images?num_of_images:h;

	if(obj > images[0]->num_of_snakes){
		ui.error("Invalid Object to analyze");
		return;
	}
	
	int x,y,z;
	int j;
        int facepix,xface=0, yface=0, zface=0;
	Point bbi=512000, bbf=0;
	int hgt, wdt, dpt = h-l+1;

	ui.thinking("analyzing objects");
	
	loop(j, l, h+1){
		// just in  case
		// make sure data is loaded
		find(j)->doit();
		Uimage& u = find(j)->drawing2;

		// find bb
		for(y=0;y<u.height;y++)for(x=0;x<u.width;x++){
			if(u(x,y)==obj+1){
				bbi.x = MIN(bbi.x, x);
				bbi.y = MIN(bbi.y, y);
				bbf.x = MAX(bbf.x, x);
				bbf.y = MAX(bbf.y, y);
			}
		}
	}
	hgt = bbf.y - bbi.y;
	wdt = bbf.x - bbi.x;
	bbi.z = 0;
	
	// allocate volume
	Solid solid(wdt+2, hgt+2, dpt+2);
        loop(z,0,dpt+2)loop(y,0,hgt+2)loop(x,0,wdt+2)
		solid[z].put(x,y, 0);
	
	//solid.offset = bbi - 1;
	//loop(z, 0, dpt)
	//	solid[z].offset = bbi - 1;
	solid.offset.z = - 1;
	
	// copy data
	ui.thinking("creating volume");
	loop(z, 0, dpt){
		Fimage &dimg = find(z+l)->drawing2;
		
		loop(y, bbi.y, bbf.y) loop(x, bbi.x, bbf.x){
			int pv = dimg(x-bbi.x + 1, y-bbi.y + 1)==(obj+1)? 255:0;
			solid[z].put(x-bbi.x + 1, y-bbi.y + 1, pv );
		}
	}

	solid.offset = -1;
	
	Vector &com = stats[obj].com;
	Matrix &m   = stats[obj].I;
	Matrix &eV  = stats[obj].eV;
	Vector &ev  = stats[obj].ev;
	float  &sf  = stats[obj].sf;
	float  &sp  = stats[obj].sp;
	float  &sa  = stats[obj].sa;
	float  &vol = stats[obj].vol;

	float pix_z_dim = find(l)->thickness;
	float pix_x_dim = find(l)->fov / find(l)->x_pix_dim / 2.0;
	float pix_y_dim = find(l)->fov / find(l)->x_pix_dim / 2.0;
	
	if(  solid.fill() || 1 ){

		ui.thinking("calculating metrics");
		sa = sf = sp = vol = 0;
		com = 0;
			
		loop(z,0,dpt)loop(y,0,hgt)loop(x,0,wdt){

			if( solid(x,y,z) ){
				facepix=0;
				com[0] += x;
				com[1] += y;
				com[2] += z;
				vol++;

				if(x && solid(x-1, y, z) ){
					facepix = 1;
					sf++;
					xface++;
				}
				if(x-wdt+1 && solid(x+1, y, z) ){
					facepix = 1;
					sf++;
					xface++;
				}
				if(y && solid(x, y-1, z) ){
					facepix = 1;
					sf++;
					yface++;
				}
				if(y-hgt+1 && solid(x, y+1, z) ){
					facepix = 1;
					sf++;
					yface++;
				}
				if(z && solid(x, y, z-1) ){
					facepix = 1;
					sf++;
					zface++;
				}
				if(z-dpt+1 && solid(x, y, z+1) ){
					facepix = 1;
					sf++;
					zface++;
				}
					
				sp +=facepix;
				if(facepix)
					solid[z].put(x,y, SURFACE);
			
			}
		}
		if( vol == 0.0 ){
			com = 0;
			sa = sf = sp = 0;
			ui.done_thinking();
			return;
		}
		
		com /= vol;
		vol *= pix_x_dim *pix_y_dim * pix_z_dim;
		sa = xface * pix_y_dim * pix_z_dim
		   + yface * pix_x_dim * pix_z_dim
		   + zface * pix_x_dim * pix_y_dim;


	// reference:
	//      "Normalization and Shape Recognition of Three-Dimensional Objects by 3D Moments"
	//      J.M. Galvez, M. Canton
	//      Pattern Recocgnition Vol. 26 No. 5  (1993) pp. 667-81

		float ax,ay,az, glark;
		Vector qux(3);

		ui.thinking("calculating I");
		loop(z,0,dpt)loop(y,0,hgt)loop(x,0,wdt){

				// Build the inertia matrix

			ax = x-com[0];
			ay = y-com[1];
			az = z-com[2];

			ax *= pix_x_dim;	// scale
			ay *= pix_y_dim;
			az *= pix_z_dim;
				
			m[0][1] -= ax * ay;             // -ixy = u 110
			m[0][2] -= ax * az;             // -ixz = u 101
			m[1][2] -= ay * az;             // -iyz = u 011
			
			m[0][0] += ay*ay + az*az;       // ixx = u 020 + u 002
			m[1][1] += ax*ax + az*az;       // iyy = u 200 + u 002
			m[2][2] += ax*ax + ay*ay;       // izz = u 200 + u 020
			
		}
		m[1][0] = m[0][1];      // it is symetric
		m[2][0] = m[0][2];
		m[2][1] = m[1][2];

		m.eigen(eV, ev);
		com[0] += bbi.x; com[1]+= bbi.y; com[2] += bbi.z;
		
		ui.done_thinking();
		
	}//if(fill)
}

