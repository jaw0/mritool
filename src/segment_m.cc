

#ifdef RCSID
static const char *const rcsid
= "@(#)$Id: segment_m.cc,v 1.1 1995/11/23 04:30:23 jaw Exp jaw $";
#endif

// Optimization: Full

#include <mritool.h>
#include <segment.h>
#include <window.h>
#include <fullname.h>
#include <defaults.h>
#include <solid.h>

extern "C" Notify_value segm_wait3_proc(Series *series, int pid);

#define CODE_SECTION	SECTION_SEGM

void UserP::segment(int l, int h, int nlev, float alpha, float minwin){
	int i;
	float beta1, beta2;
	int maxk, maxg;
	
	h = h<c_series->num_of_images ? h : (c_series->num_of_images-1);
	l = l>1 ? l : 1;

	beta1 = 1.0/(1.0 + c_image->thickness/c_image->pixel_size);
	beta2 = 1.0 - beta1;

	maxk = CINT(getvalue(maksym_c("mri:segm:maxiter-k")));
	maxg = CINT(getvalue(maksym_c("mri:segm:maxiter-g")));

	debugp( DBG_STATS )
		cerr << "Using beta = " << beta1 << "," << beta2 << endl;
	
	Fsolid img(c_image->fimg.width, c_image->fimg.height, h-l+1);
	Usolid segm(c_image->fimg.width, c_image->fimg.height, h-l+1);
	Fimage stft(c_image->fimg.width, c_image->fimg.height);

	for(i=l; i<=h; i++){
		Image* ci = c_series->find(i);
		int j;
	       	ci->doit();	// make sure data is loaded
		for(j=0;j<ci->fimg.width* ci->fimg.height; j++)
			//stft[j] = ci->dg_data[j];
			stft[j] = ci->fimg[j];

		debugp( DBG_SAVE_X )
			stft.save("=d/stft-p.ras");
#if 0
		// stripfat
		stft.kmeans(img[i-l], nlev, 30);
		debugp( DBG_SAVE_X )
			img[i-l].save("=d/stft.ras");
		for(j=0; j<stft.width*stft.height;j++){
			if(stft[j]==nlev-1)
				img[i-l].put(j,0);
			else
				img[i-l].put(j, ci->dg_data[j]);
		}
		img[i-l].quantize(255);
		debugp( DBG_SAVE_X )
			img[i-l].save("=d/stftqp.ras");
#else
		img[i-l] = stft;
#endif		
	}
	
	
	
	img.segment(segm, nlev, alpha, beta1, beta2, maxk, maxg, minwin);
	for(i=0;i<segm.depth;i++)
		segm[i].quantize(255);

	for(i=l; i<=h; i++){
		Image *ci = c_image;
		c_image = c_series->find(i);
		c_image->segm = segm[i-l];
		c_image->save_segm();
		c_image->S(Image::IFB_SEGM_PRESENT);
		c_image = ci;
	}
}

extern "C" void segment(Xv_opaque it){
	 // from button
        int what, grnd, alph;
	char buffer[512];
	XView& m = me(it);

	debugp( DBG_THINK )
		m.thinking();
	xv_set(m.segm_popup, XV_SHOW, FALSE, 0);
	XFlush(m.dpy);
	alph = (int)xv_get(m.segm_alpha_i, PANEL_VALUE, 0);

	sprintf(buffer, "(segment $R $N %d)", alph);

	m.interpret(buffer);
	debugp( DBG_THINK )
		m.done_thinking();
}


// extensions to try for segm file
char *ext_tabl[] = { SEGM_FILE_TYPE, "ras", "segm", "ras.gz", "ras.Z" , 0 };
			     
void Image::load_segm(void){
	// load segm data
	// look in a few std places (in order):
	//   .../segm.%I.ras.gz
	//   .../I.###
	//   .../segm.%I.{ras,segm,ras.gz,ras.Z}
	//   .../I.###.{ras,segm,ras.gz,ras.Z}
	
	FILE *fp;
	struct SegmFile hdr;

	Fimage foo(256, 256);

	if( chdir(mriname("=G")) ==-1)
		UI->error(__FILE__ ": load segment file, could not access segm dir");
	
	if( segm.load( mriname(SEGM_FILENAME SEGM_FILE_TYPE) ) ){
		;
	}else if ( (fp=fopen(dirname, "r"))){
		// read it in
		fread((char*)&hdr, sizeof(struct SegmFile), 1, fp);
		if (strncmp(hdr.magic, SEGMFILE_MAGIC, 8)){
			UI->error(__FILE__ ": load segment file, bad header");
			fclose(fp);
			return;
		}
		if(hdr.offline){
			// it is offline right now
		        warn(__FILE__ ": load segment file, segm offline (still)");
			S(IFB_OFFL_PENDING);
			pid = hdr.pid;
			fclose(fp);
			return;
		}
		fread((char*)(float*)foo, 256*256, sizeof(float), fp);
		segm = foo;
		// nlevs = hdr.nlevs;			
		fclose(fp);

	}else{
		char buffer[2048];
		int i;

		for(i=0; ext_tabl[i]; i++){
			sprintf(buffer, "%s%s", SEGM_FILENAME, ext_tabl[i]);
			if( segm.load(mriname(buffer))) break;
		}
		if(!ext_tabl[i])
			for(i=0; ext_tabl[i]; i++){
				sprintf(buffer, "%s.%s", dirname, ext_tabl[i]);
				if( segm.load(buffer)) break;
			}
		if(!ext_tabl[i]){
			warn(__FILE__ ": load segment file, could not open");
			return;
		}
		
	}
	nlevs = segm.levels();
	S(IFB_SEGM_PRESENT);
	
}

void Image::save_segm(void){
	// save segm data 
	
	if((chdir(mriname("=G"))==-1)||mkdirhier()){
		// error
		UI->error(
			"             YIKES!!",
			"Could not access segmentation directory",
			"Check `segmdir` line in .mrirc",
			"and accesss permissions on directory"
		);

		return; 
	}
	if( !segm.save( mriname(SEGM_FILENAME SEGM_FILE_TYPE))){
		UI->error(
			"Could not write segmentation file",
			"Data may not be available"
		);
		return;
	}
	chmod (mriname(SEGM_FILENAME SEGM_FILE_TYPE), 0666);

}

void Image::segm_lock(void){
	// create lock file while segm is offline
	struct SegmFile hdr;
	FILE *fp;

	
	if ( 
		(chdir(mriname("=G"))==-1)
		|| mkdirhier()
		|| (!(fp=fopen(dirname, "w")))
	){
		// error
		UI->error(
			"            YIKES!!",
			"Could not lock segmentation file",
			"Data may become corrupted"
		);
		return; 
	}
	
	strncpy(hdr.magic, SEGMFILE_MAGIC, 9);
	hdr.offline = 1;
	hdr.pid = pid;

	fwrite((char*)&hdr, sizeof(struct SegmFile), 1, fp);
	fclose(fp);
	chmod(dirname, 0666);
}
	


extern "C" Notify_value segm_wait3_proc(Series *series, int pid){
	// process sigchild, reap our children

	series->reap(pid);
	return (NOTIFY_DONE);
}


void Series::reap(int pidn){
	int i;

	UI->error(
			"Background Segment job finished",
			"Hold on while I load results..."
	);
	debugp( DBG_VERBOSE )
		cerr << "Reaping" << endl;
	for(i=0;i<num_of_images;i++){
		// check @ image in series

		if( 	images[i]->T(Image::IFB_OFFL_PENDING)
			&& images[i]->pid == pidn
		){
			debugp( DBG_WORK ) cerr << "match " << i << endl;
			// a match. reap it
			images[i]->load_segm();
			images[i]->C(Image::IFB_OFFL_PENDING);
		}
	}
}



