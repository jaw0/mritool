

#include <mritool.h>


extern "C" void mode_vw_segm(Xv_opaque it){
	
	me(it).c_image->S(Image::IFB_VIEW_SEGM);
	update_view(it);
	refresh_view(it);
}


extern "C" void mode_vw_img(Xv_opaque it){

	me(it).c_image->C(Image::IFB_VIEW_SEGM);
	update_view(it);
	refresh_view(it);
}

extern "C" void mode_hide_cntr(Xv_opaque it){

	me(it).c_image->S(Image::IFB_HIDE_CONTOUR);
	refresh_view(it);
}


extern "C" void mode_show_cntr(Xv_opaque it){

	me(it).c_image->C(Image::IFB_HIDE_CONTOUR);
	refresh_view(it);
}

