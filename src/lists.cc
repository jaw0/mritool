

// Copyright (c) 1993 Jeff Weisberg
// University of Rochester
//      Dept. of Electrical Engineering
//      Dept. of Neurology, C.E.P.
//
// see the file "License"

#ifdef RCSID
static const char *const rcsid
= "@(#)$Id: lists.cc,v 1.1 1995/11/23 04:30:16 jaw Exp jaw $";
#endif

// Optimization: Full

#include <mritool.h>
#include <window.h>

// ought there be resize corners on the lists?
#define L_CORNERS_P	TRUE

extern "C" void do_image_l(Xv_opaque it){
	char buffer[256];
	XView &m = me(it);
	
	// hide popup
	xv_set(m.image_popup, XV_SHOW, FALSE, 0 );
	XFlush( m.dpy );
	
	sprintf(buffer, "(set image %d)", m.c_series->selected);
	m.interpret(buffer);
	
	m.done_thinking();
}
extern "C" void show_patient_popup(Xv_opaque it){
	xv_set(me(it).patient_popup,XV_SHOW,TRUE,0);
}
extern "C" void next_image(Xv_opaque it){
	// me(it).interpret("set image next\n");
	me(it).interpret("(set image next)");
	return;
}
extern "C" void previous_image(Xv_opaque it){
	// me(it).interpret("set image prev\n");
	me(it).interpret("(set image prev)");
	return;
}
extern "C" void select_list(
	Xv_opaque item, 
	char *string, 
	int data, 
	Panel_list_op op,
	Event *event) {

	XView &m = me(item);

	if (!op) return;
	if( item == m.patient_list )	m.p_sel = data;
	if( item == m.study_list )	m.c_patient->selected = data;
	if( item == m.series_list )	m.c_study->selected = data;
	if( item == m.image_list )	m.c_series->selected = data;
}
extern "C" void select_studies(Xv_opaque it){
	me(it).c_patient = G.patients_a[me(it).p_sel];
	me(it).c_patient->menu(me(it));
}
extern "C" void select_series(Xv_opaque it){
	me(it).c_study = me(it).c_patient->studies[me(it).c_patient->selected];
	me(it).c_study->menu(me(it));
}
extern "C" void select_images(Xv_opaque it){	
	me(it).c_series = me(it).c_study->series[me(it).c_study->selected];
	me(it).c_series->menu(me(it));
}
extern "C" void list_images(Xv_opaque it){
	me(it).c_series->menu(me(it));
}
extern "C" void back_study(Xv_opaque it){
	xv_set(me(it).series_popup,XV_SHOW,FALSE,0);
	xv_set(me(it).study_popup, XV_SHOW,TRUE,0);
}
extern "C" void back_series(Xv_opaque it){
	xv_set(me(it).image_popup,XV_SHOW,FALSE,0);
	xv_set(me(it).series_popup, XV_SHOW,TRUE,0);
}
extern "C" void back_patient(Xv_opaque it){
	xv_set(me(it).study_popup,XV_SHOW,FALSE,0);
	xv_set(me(it).patient_popup, XV_SHOW,TRUE,0);
}

void XView::create_patient_popup(){
	Xv_opaque ppanel;
	int i;
	char text[64];

	patient_popup = xv_create(main_frame,FRAME_CMD,
		XV_KEY_DATA, KEY_THIS, (Xv_opaque)(void*)this,
		XV_LABEL,"Patient List",
		FRAME_SHOW_RESIZE_CORNER, L_CORNERS_P,
		FRAME_CMD_PUSHPIN_IN, FALSE,
		XV_X,     CINT( getvalue(maksym_c("mri:ptl_x"))),
		XV_Y,     CINT( getvalue(maksym_c("mri:ptl_y"))),
                XV_WIDTH, CINT( getvalue(maksym_c("mri:ptl_w"))),
		XV_HEIGHT,CINT( getvalue(maksym_c("mri:ptl_h"))),
		0);

	ppanel = xv_get(patient_popup, FRAME_CMD_PANEL);

	xv_create(ppanel,PANEL_BUTTON,
		XV_KEY_DATA, KEY_THIS, (Xv_opaque)(void*)this,
	        XV_X,10,
		PANEL_LABEL_STRING,"Select",
		PANEL_NOTIFY_PROC, select_studies, 
		0);

	patient_list = xv_create(ppanel,PANEL_LIST,
		XV_KEY_DATA, KEY_THIS, (Xv_opaque)(void*)this,
		XV_SHOW,TRUE,
		XV_X,10,
		XV_Y,40,
		PANEL_LIST_DISPLAY_ROWS, CINT( getvalue(maksym_c("mri:ptl_r"))),
		PANEL_LIST_NROWS,500,
		PANEL_CHOOSE_ONE,TRUE,
		PANEL_READ_ONLY,TRUE,
		PANEL_NOTIFY_PROC, select_list,
		0);

	for(i=0;i<G.num_of_patients; i++){
		sprintf(text,"%8.8s  %s", G.patients_a[i]->pid, G.patients_a[i]->name);
		xv_set(patient_list,PANEL_LIST_INSERT,i,
			XV_KEY_DATA, KEY_THIS, (Xv_opaque)(void*)this,
		        PANEL_LIST_STRING,i,text,
			PANEL_LIST_CLIENT_DATA,i,i,
			0);

	}

	study_popup = xv_create(main_frame,FRAME_CMD,
		XV_LABEL,"Study List",
		XV_KEY_DATA, KEY_THIS, (Xv_opaque)(void*)this,
		FRAME_SHOW_RESIZE_CORNER, L_CORNERS_P,
		FRAME_CMD_PUSHPIN_IN,FALSE,
		XV_X,     CINT( getvalue(maksym_c("mri:stl_x"))),
		XV_Y,     CINT( getvalue(maksym_c("mri:stl_y"))),
                XV_WIDTH, CINT( getvalue(maksym_c("mri:stl_w"))),
		XV_HEIGHT,CINT( getvalue(maksym_c("mri:stl_h"))),
		0);

	panel = xv_get(study_popup, FRAME_CMD_PANEL);

	xv_create(panel,PANEL_BUTTON,
		XV_X,10,
		XV_KEY_DATA, KEY_THIS, (Xv_opaque)(void*)this,
		PANEL_LABEL_STRING,"Select",
		PANEL_NOTIFY_PROC, select_series, 
		0);

	xv_create(panel,PANEL_BUTTON,
		XV_SHOW,TRUE,
		XV_KEY_DATA, KEY_THIS, (Xv_opaque)(void*)this,
		PANEL_LABEL_STRING, "Go Back",
		PANEL_NOTIFY_PROC, back_patient,
		0);

	study_list = xv_create(panel,PANEL_LIST,
		XV_SHOW,TRUE,
		XV_KEY_DATA, KEY_THIS, (Xv_opaque)(void*)this,
		XV_X,10,
		XV_Y,30,
		PANEL_LIST_DISPLAY_ROWS, CINT( getvalue(maksym_c("mri:stl_r"))),
		PANEL_LIST_NROWS,200,
		PANEL_CHOOSE_ONE,TRUE,
		PANEL_NOTIFY_PROC, select_list,
		0);

	series_popup = xv_create(main_frame,FRAME_CMD,
		XV_LABEL,"Series List",
		XV_KEY_DATA, KEY_THIS, (Xv_opaque)(void*)this,
		FRAME_SHOW_RESIZE_CORNER, L_CORNERS_P,
		FRAME_CMD_PUSHPIN_IN,FALSE,
		XV_X,     CINT( getvalue(maksym_c("mri:sel_x"))),
		XV_Y,     CINT( getvalue(maksym_c("mri:sel_y"))),
                XV_WIDTH, CINT( getvalue(maksym_c("mri:sel_w"))),
		XV_HEIGHT,CINT( getvalue(maksym_c("mri:sel_h"))),
		0);

	ppanel = xv_get(series_popup, FRAME_CMD_PANEL);
	
	xv_create(ppanel,PANEL_BUTTON,
//		XV_X,10,
		XV_KEY_DATA, KEY_THIS, (Xv_opaque)(void*)this,
		PANEL_LABEL_STRING,"Select",
		PANEL_NOTIFY_PROC, select_images, 
		0);

	xv_create(ppanel,PANEL_BUTTON,
		XV_SHOW,TRUE,
		XV_KEY_DATA, KEY_THIS, (Xv_opaque)(void*)this,
		PANEL_LABEL_STRING, "Go Back",
		PANEL_NOTIFY_PROC, back_study,
		0);

	series_list = xv_create(ppanel,PANEL_LIST,
		XV_SHOW,TRUE,
		XV_X,     10,
		XV_Y,     30,
		XV_KEY_DATA, KEY_THIS, (Xv_opaque)(void*)this,
		PANEL_LIST_DISPLAY_ROWS, CINT( getvalue(maksym_c("mri:sel_r"))),
		PANEL_LIST_NROWS,200,
		PANEL_CHOOSE_ONE,TRUE,
		PANEL_NOTIFY_PROC, select_list,
		0);

	image_popup = xv_create(main_frame,FRAME_CMD,
		XV_LABEL,"Image List",
		FRAME_SHOW_RESIZE_CORNER, L_CORNERS_P,
		XV_KEY_DATA, KEY_THIS, (Xv_opaque)(void*)this,
		FRAME_CMD_PUSHPIN_IN,FALSE,
                XV_X,     CINT( getvalue(maksym_c("mri:iml_x"))),
		XV_Y,     CINT( getvalue(maksym_c("mri:iml_y"))),
                XV_WIDTH, CINT( getvalue(maksym_c("mri:iml_w"))),
		XV_HEIGHT,CINT( getvalue(maksym_c("mri:iml_h"))),
		0);

	ppanel = xv_get(image_popup, FRAME_CMD_PANEL);

	xv_create(ppanel,PANEL_BUTTON,
		XV_X,10,
		XV_KEY_DATA, KEY_THIS, (Xv_opaque)(void*)this,
		PANEL_LABEL_STRING,"Select",
		PANEL_NOTIFY_PROC, ::do_image_l, 
		0);

	xv_create(ppanel,PANEL_BUTTON,
		XV_SHOW,TRUE,
		XV_KEY_DATA, KEY_THIS, (Xv_opaque)(void*)this,
		PANEL_LABEL_STRING, "Go Back",
		PANEL_NOTIFY_PROC, back_series,
//		XV_X,150,
		0);

	image_list = xv_create(ppanel,PANEL_LIST,
		XV_KEY_DATA, KEY_THIS, (Xv_opaque)(void*)this,
		XV_SHOW,TRUE,
		XV_X,10,
		XV_Y,30,
		PANEL_LIST_DISPLAY_ROWS, CINT( getvalue(maksym_c("mri:iml_r"))),
		PANEL_LIST_NROWS,200,
		PANEL_CHOOSE_ONE,TRUE,
		PANEL_NOTIFY_PROC, select_list,
		0);
}

void Patient::menu(XView& w){
	int i, nm = num_of_studies;
	char text[64];

	/*w.c_patient->*/ selected = 0;
	xv_set(w.patient_popup,XV_SHOW,FALSE,0);
	for(i=0;i<MAXINLIST;i++) xv_set(w.study_list,PANEL_LIST_DELETE,0,0);
	if(! studies)
		learn();
	
	for(i=0;i<nm;i++){
		sprintf(text, "%4.4d  %11.11s  %s", studies[i]->num,
			studies[i]->date,
			studies[i]->descr);
		xv_set(w.study_list,PANEL_LIST_INSERT,i,
			PANEL_LIST_STRING,i,text,
			PANEL_LIST_CLIENT_DATA,i,i,
			0);
	}
	sprintf(text,"%s" ,name);
	xv_set(w.study_popup,XV_SHOW,TRUE,XV_LABEL,text,0);
}

void Study::menu(XView& w){
	int i, nm = num_of_series;
	char text[64];
	
	selected = 0;
	xv_set(w.study_popup,XV_SHOW,FALSE,0);
	for(i=0;i<MAXINLIST;i++) xv_set(w.series_list,PANEL_LIST_DELETE,0,0);
	if(! series)
		learn();
	for(i=0;i<nm;i++){
		sprintf(text,"%4.4d  %11.11s  %s", series[i]->num,
			series[i]->date,
			series[i]->descr);
		xv_set(w.series_list,PANEL_LIST_INSERT,i,
			PANEL_LIST_STRING,i,text,
			PANEL_LIST_CLIENT_DATA,i,i,
			0);
	}
	sprintf(text,"%4.4d    %s",
		nm, 
		mine->name);
	xv_set(w.series_popup,XV_SHOW,TRUE,XV_LABEL,text,0);
}

void Series::menu(XView& w){
	int i,nm = num_of_images;
	char text[64];
	Image *im;
	
	selected = 1;
	xv_set(w.series_popup,XV_SHOW,FALSE,0);
	for(i=0;i<MAXINLIST;i++) xv_set(w.image_list,PANEL_LIST_DELETE,0,0);
	if(! images)
		learn();
	for(i=0;i<nm;i++){
		im = images[i];
		// im = find( i + 1 );	// images are generally numbered starting at 1
		
		sprintf(text, "%4.4d  %s", im->num, im->name );
// don't really need both image name and number 'tis rather redundant ...
		xv_set(w.image_list,PANEL_LIST_INSERT,i,
			PANEL_LIST_STRING,i,text,
			PANEL_LIST_CLIENT_DATA,i,im->num,
			PANEL_LIST_SELECT,i,FALSE,
			0);
	}
	sprintf(text,"%4.4d    %4.4d    %s", 
		nm,
		mine->num, 
		mine->mine->name);
	xv_set(w.image_popup,XV_SHOW,TRUE,XV_LABEL,text,0);
}

