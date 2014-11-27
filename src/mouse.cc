
static const char *const rcsid
= "@(#)$Id: mouse.cc,v 1.1 1995/11/23 04:30:18 jaw Exp jaw $";

// Optimization: -O2

#include <mritool.h>
#include <window.h>
#include <string.h>

#include <jl_proto.h>
#include <defproto.h>

extern Obj buttons_sym;

char *button_labels(int i){
	Obj lbl;
	static char buff[64];
	int n;
	
	if(! buttons_sym){
		buttons_sym = maksym("mri:buttons");
		internal_gc_protect = Fcons( buttons_sym, internal_gc_protect);
	}
	lbl = getvalue( buttons_sym);

	if( NDEFINEDP(lbl) || NCONSP(lbl))
		return "";
	lbl = Fnth( lbl, MAKINT(i));
	if(! STRINGP(lbl))
		return "";

	for(n=0; n<CLENGTH(lbl) && n<64; n++)
		buff[n] = CCHARS(lbl)[n];
	buff[n] = 0;
	
	return buff;
}


void XView::footer(const char *s){
	xv_set(main_frame,FRAME_LEFT_FOOTER,s,0);
	XFlush(dpy);
}

#if 0
extern "C" void  mouse_controller(Canvas can, 
	Event *event){
	// mouse was pressed in canvas

	if(me(can).c_image)me(can).c_image->mouse_op(me(can), event);
	return;
}


extern "C" void alt_mouse_selected(Xv_opaque menu,Xv_opaque item){
	// mouse menu item was selected

	int i;
	enum Mouse_Mode v = M_MINUS_ONE;
	char *string;
	XView &m = me(menu);
	
	string = (char *)xv_get(item,MENU_STRING);
	for(i=0;i<M_LAST_ONE;i++){
		if(!strcmp(string,alt_mouse_menu_strings[i].string)){
			v = alt_mouse_menu_strings[i].value;
			break;
		}
	}
	if(v==M_DRAW){
		char txt[256];
		// sprintf(txt, "Draw: %s", config.button_labels[m.c_image->selected]);
		sprintf(txt, "Draw: %s", button_labels(m.c_image->selected));
		m.footer(txt);
	}else
		m.footer(string);
	if(v != M_MINUS_ONE) G.mouse_mode = v;
	else G.mouse_mode = M_NOP;
	if(m.c_image)m.c_image->mouse_op(m, 0);	// no event

	if(G.mouse_mode == M_NOP) m.footer("None");
}

void Image::mouse_op(XView& w, Event *event){
	// this is where the action will take place
	// from mouse mode and event, figure out
	// what to do, and do so
	// if event == 0 then we got here from menu

	static int		// some needed state
		who_down=-1;
	//	these now saved in I::
	//	start_mx=-1, start_my=-1,
	//	last_mx=-1, last_my=-1;

	int eid = event?event_id(event):0;
	int x=0,y=0,i,sm=event?event->ie_shiftmask:0,state;
	char txt[256];
	
	if (event){
		x = event_x(event);  /* get coords. */
		y = event_y(event);
	}
	if ((x>512)||(y>512)||(x<0)||(y<0)) return; // out of bounds
	state = ((sm&SHIFTMASK)?1:0)
		|(((sm&CTRLMASK)?1:0)<<1)
		|(((sm&META_SHIFT_MASK)?1:0)<<2);

	if (eid == MS_RIGHT){
		// in any mode right button will popup menu
		menu_show(w.mouse_menu,w.canvas,event,0);
		return;
	}
	Point p(x,y), las(last_mx, last_my);
	
	switch(G.mouse_mode){

	  case M_DRAW:
		// draw contour
		if (
		    !event 
		    || event_is_up(event) 
		    || !(
			 (eid==MS_LEFT)
			 ||(eid==LOC_DRAG)
		    )
		) return;
		S(IFB_DRAWN_ON);	// invalidate any snake laying around
		if ((last_mx==-1)||(who_down != LEFT_ONE)){  
			/* first point selected? */
			start_mx=last_mx = x; 
			start_my=last_my = y;
			who_down = LEFT_ONE;
			w.put_dot(p);
			sprintf(txt, "Draw: %s", button_labels(selected));
			// sprintf(txt, "Draw: %s", config.button_labels[selected]);
			w.footer(txt);
			return;
		}
		/* else draw line */
		who_down = LEFT_ONE;
		w.put_line(las, p);
		last_mx = x;
		last_my = y;
		return;
		
	case M_D_BLUE:
		// erase contour
		// there is nothing blue here, the name is historic
		if (
		    !event 
		    || event_is_up(event) 
		    || !(
			 (eid==MS_LEFT)
			 ||(eid==LOC_DRAG)
		    )
		) return;
		S(IFB_DRAWN_ON);	// invalidate any snake laying around
		
		w.clear_spot(p);
		who_down = MIDD_ONE;
		last_mx = last_my = -1;	// dont connect the line
		return;
		
	  case M_D_GREEN:
		// draw ROI
		if(event && event_is_up(event)) return;
		if( (eid==MS_LEFT) || (eid==LOC_DRAG) ); else return;
		if(w.c_series) w.c_series->draw_roi(w, x,y,eid);
		who_down = LEFT_ONE;
		return;

	  case M_VISINE:
		// visine();
		// seems to be a problem with this
		// I know not why...
		who_down = LEFT_ONE;
		return;

	  case M_FIDDLE:
		// adjust W/L
		who_down = RIGHT_ONE;
		w.adjust_wl(y-256, x-128);
		return;

	  case M_NXT_SNK:
		close_contour(w);
		who_down = RIGHT_ONE;
		last_mx = last_my = -1;
		selected %= num_of_snakes;
		sprintf(txt, "Draw: %s", button_labels(selected));
		// sprintf(txt, "Draw: %s", config.button_labels[selected]);
		w.footer(txt);
		G.mouse_mode = M_DRAW;
		return;

	  case M_SEL_LOI:
		if (
		    !event 
		    || event_is_up(event) 
		    || !(
			 (eid==MS_LEFT)
			 ||(eid==LOC_DRAG)
		    )
		) return;
		G.mouse_mode = M_SEL_LOI;
		who_down = RIGHT_ONE;
		snake[selected].loi = segm[int(y/2)*256 + x/2];
		snake[selected].ploi = Point(x,y);
		sprintf(txt, "LOI of %s: (%d,%d)=%d",
			button_labels(selected), 
			// config.button_labels[selected],
			x, y, int(segm[int(y/2)*256 + x/2])
		);
		w.footer(txt);
		break;
		
	  default:
		who_down = -1;

	    }

}

#endif






