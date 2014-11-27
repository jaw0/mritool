
// $Id: roi.cc,v 1.1 1995/11/23 04:30:20 jaw Exp jaw $

#include <mritool.h>
#include <window.h>



void Series::draw_roi(XView& w, int x, int y, int eid){
	// draw the roi
        // called from mouse_op (mouse button at (x,y))
	int i = 0;
	
	if( roi[i].present){
	//undraw previous box
		// debug("roi: undraw\n");
		XDrawRectangle(w.dpy, w.canvas_xid, w.gc_draw, 
			       roi[i].xo, roi[i].yo,
			       roi[i].width, roi[i].height);
        }	
   		
	if(eid==MS_LEFT){
		// first point is corner
     		roi[i].xo = x;
		roi[i].yo = y;
		roi[i].width = roi[i].height = 0;
		roi[i].present = 0;

	}else if(eid==LOC_DRAG){
		// next point is a box
	//	if( x<roi[i].xo || y<roi[i].yo ) return;
	
		roi[i].width = x - roi[i].xo;
		roi[i].height = y - roi[i].yo;
		// draw a box
		// debug("roi: draw\n");
		XDrawRectangle(w.dpy, w.canvas_xid, w.gc_draw, 
			      roi[i].xo, roi[i].yo,
			      roi[i].width, roi[i].height);

		roi[i].present = 1;
	} 
}
