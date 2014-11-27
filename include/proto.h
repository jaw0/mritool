

#ifdef RCSID
static const char *const rcsid_prt_h
	= "@(#)$Id: proto.h,v 1.1 1995/11/23 04:30:02 jaw Exp jaw $";
#endif
	
#ifndef _proto_h
#define _proto_h

extern "C" {
	int test_function();

// window interface stuff

int
	load_proc(), 
	do_green(), 
	reset_green(), 
	select_thr(),
	do_undo(), 
	do_histo(), 
	save_image(), 
	extract(),
	save_data(),
	main_destroy_interposer(Xv_opaque f);

void
	analyze(Xv_opaque),
	record_meas(),
	refresh_view(Xv_opaque),
	view_pal(Xv_opaque),
	update_view(Xv_opaque), 
	visine(Xv_opaque),
	mouse_controller(Canvas, Event*),
	alt_mouse_selected(Xv_opaque i, Xv_opaque m),
	quit(Xv_opaque),
	null_op(), 
	show_load_popup(Xv_opaque), 
	show_data_popup(Xv_opaque), 
	show_help_popup(Xv_opaque),
	show_save_popup(Xv_opaque),
	show_meas_popup(Xv_opaque),
	show_segm_popup(Xv_opaque),
	show_snak_popup(Xv_opaque),
	show_bounds_popup(Xv_opaque),
	show_patient_popup(Xv_opaque),
	show_debug_popup(Xv_opaque),
	show_interp_popup(Xv_opaque),
	do_image(Xv_opaque),
	next_image(Xv_opaque),
	previous_image(Xv_opaque),
	select_images(Xv_opaque),
	select_series(Xv_opaque),
	select_list(Xv_opaque, char*,   int,  Panel_list_op, Event*),
	list_images(Xv_opaque),
	segment(Xv_opaque),
	snake(Xv_opaque),
	mode_vw_segm(Xv_opaque),
	mode_vw_img(Xv_opaque),
	mode_hide_cntr(Xv_opaque),
	mode_show_cntr(Xv_opaque),
        save_series_conts(Xv_opaque),
	save_image_cont(Xv_opaque),
	do_adjst(), 
	refresh_histo();

};

extern void
        footer(char *s),
        test_code(void),
	// stripfat(u_short *d,image &a),
	spin(void),
	mouse_op(Xv_opaque),
	set_label(void),
	get_what_patients(void),
	warn(const char*),
	bug(const char *txt),
	bug(const char*, const char*);

extern int 
	our_patient(char *name), 
	q_sort();

extern float dg2sun(int a, int b);
extern const char* mriname(const char*);

#endif /* !_proto_h */

