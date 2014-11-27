 
// Copyright (c) 1994 Jeff Weisberg
// University of Rochester
//      Dept. of Electrical Engineering
//      Dept. of Neurology, C.E.P.
//
// see the file "License"
 
#ifdef RCSID
static const char *const rcsid
= "@(#)$Id: file.cc,v 1.1 1995/11/23 04:30:15 jaw Exp jaw $";
#endif

// Optimization: Full

#include <mritool.h>
#include <window.h>
#include <fullname.h>

#include <jl_proto.h>
#include <defproto.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <time.h>

#include <signa-consts.h>	// 4.x header defn
#include <gemr_imgf.h>		// 5.x header defn

union ge_xxx_4 {
	char	c[4];
	int	i;
	float	f;
};
union ge_xxx_2 {
	char	c[2];
	short	s;
};


// read the header, handles 4x and 5x headers
int GEMR_Header::read_header(FILE *fp){
	int s, l;
	
	// grab a bit off the front to determine file type
	s = fread(u.raw, 128, 1, fp);
	
	if( u.ge5x.img_magic == IMGF_MAGIC ){
		// 5x format
		tag = 5;

		if( u.ge5x.img_version != 3)
			warn( __FILE__ ": GEMR::read_header(): unrecognized header version");

		l = u.ge5x.img_hdr_length;
		if( l > sizeof(u)){
			cerr << "???" << endl;
		}
		s = fread(u.raw + 128, 1, u.ge5x.img_hdr_length - 128, fp);
		p.im = (void*)u.raw;
		p.su = u.ge5x.img_l_suite  ? (void*) (u.raw + u.ge5x.img_p_suite ) : 0;
		p.ex = u.ge5x.img_l_exam   ? (void*) (u.raw + u.ge5x.img_p_exam  ) : 0;
		p.se = u.ge5x.img_l_series ? (void*) (u.raw + u.ge5x.img_p_series) : 0;
		p.mr = u.ge5x.img_l_image  ? (void*) (u.raw + u.ge5x.img_p_image ) : 0;
		p.up = u.ge5x.img_l_unpack ? (void*) (u.raw + u.ge5x.img_p_unpack) : 0;

	}else{
		// 4x format
		tag = 4;
		s = fread(u.raw + 128, 1, 256*28*sizeof(short) - 128, fp);
	}
	return tag;
}

static void get_xxx4(union ge_xxx_4 *u, char* p){
	u->c[0] = p[0];
	u->c[1] = p[1];
	u->c[2] = p[2];
	u->c[3] = p[3];
}
static void get_xxx2(union ge_xxx_2 *u, char* p){
	u->c[0] = p[0];
	u->c[1] = p[1];
}
static int get_int(char *p){
	union ge_xxx_4 u;
	get_xxx4(&u, p);
	return u.i;
}
static float get_float(char *p){
	union ge_xxx_4 u;
	get_xxx4(&u, p);
	return u.f;
}
static int get_short(char *p){
	union ge_xxx_2 u;
	get_xxx2(&u, p);
	return u.s;
}

int GEMR_Header::get_int_pt_age(void){
	if(tag==4){
		return atoi( u.ge4x.c + STHDR_START+AGE );
	}else{
		return get_short( (char*)p.ex + _EX_patage );
	}
}
	
int GEMR_Header::get_int_pt_sex(void){
	if(tag==4){
		return u.ge4x.c[STHDR_START+SEX];
	}else{
		return get_short( (char*)p.ex + _EX_patsex );
	}
}

int GEMR_Header::get_int_pt_weight(void){
	if(tag==4){
		return atoi( u.ge4x.c + STHDR_START+WEIGHT );
	}else{
		return get_short( (char*)p.ex + _EX_patweight );
	}
}

int GEMR_Header::get_int_mr_xdim(void){
	if(tag==4){
		return u.ge4x.s[(IMHDR_START+IMXPIXDIM)/2];
	}else{
		return get_short( (char*)p.mr + _MR_imatrix_X );
	}
}

int GEMR_Header::get_int_mr_ydim(void){
	if(tag==4){
		return u.ge4x.s[(IMHDR_START+IMXPIXDIM)/2];
	}else{
		return get_short( (char*)p.mr + _MR_imatrix_Y );
	}
}

char * GEMR_Header::get_str_se_protocol(void){
	if(tag==4){
		return u.ge4x.c + SEHDR_START + SERPROTO;
	}else{
		return (char*)p.se + _SE_prtcl;
	}
}

char * GEMR_Header::get_str_se_date(void){
	if(tag==4){
		return u.ge4x.c + SEHDR_START+SERDATE;
	}else{
		int t;
		struct tm *stm;
		static char buf[64];
		
		t = get_int( (char*)p.se + _SE_se_datetime );
		stm = localtime((time_t*) &t);
		strftime(buf, sizeof(buf), "%d-%h-%Y", stm);
		return buf;
	}
}

char * GEMR_Header::get_str_st_date(void){
	if(tag==4){
		return u.ge4x.c + STHDR_START+STDATE;
	}else{
		int t;
		struct tm *stm;
		static char buf[64];
		
		t = get_int( (char*)p.ex + _EX_ex_datetime );
		stm = localtime((time_t*) &t);
		strftime(buf, sizeof(buf), "%d-%h-%Y", stm);
		return buf;
	}
}

char * GEMR_Header::get_str_se_desc(void){
	if(tag==4){
		return u.ge4x.c + SEHDR_START+SERDESCR;
	}else{
		return (char*)p.se + _SE_se_desc;
	}
}
char * GEMR_Header::get_str_st_desc(void){
	if(tag==4){
		return u.ge4x.c + STHDR_START+STDESCR;
	}else{
		return (char*)p.ex + _EX_ex_desc;
	}
}
	
char * GEMR_Header::get_str_pt_name(void){
	if(tag==4){
		return u.ge4x.c + NAME_START+STHDR_START;
	}else{
		return (char*)p.ex + _EX_patname;
	}
}
char * GEMR_Header::get_str_hosp(void){
	if(tag==4){
		return u.ge4x.c + HOSP+STHDR_START;
	}else{
		return (char*)p.ex + _EX_hospname;
	}
}
float GEMR_Header::get_float_mr_thick(void){
	if(tag==4){
		return dg2sun( (int)u.ge4x.s[(IMHDR_START+IMTHICK)/2], 
			       (int)u.ge4x.s[(IMHDR_START+IMTHICK)/2+1] );
	}else{
		return get_float( (char*)p.mr + _MR_slthick );
	}
}

float GEMR_Header::get_float_mr_fov(void){
	if(tag==4){
		return dg2sun( (int)u.ge4x.s[(SEHDR_START+SERFOV)/2], 
			       (int)u.ge4x.s[(SEHDR_START+SERFOV)/2+1] );
	}else{
		return get_float( (char*)p.mr + _MR_dfov );
	}
}
float GEMR_Header::get_float_mr_psize(void){
	if(tag==4){
		return dg2sun( (int)u.ge4x.s[(IMHDR_START+IMPIX)/2], 
			       (int)u.ge4x.s[(IMHDR_START+IMPIX)/2+1] );
	}else{
		return get_float( (char*)p.mr + _MR_pixsize_X );
	}
}

void Image::doit(void){
	// allocate mem and load

	int i, fd, c;
	FILE *fp;
	int pipep=0;

	if(! int(fimg)){
		// need to load in an image
		char *cmd;
		
		chdir(mriname("=R"));
		c = dirname[strlen(dirname)-1];
		switch(c){
		case 'Z':
			cmd = "uncompress -c 2>/dev/null";
			pipep = 1;
			break;
		case 'z':
			cmd = "gunzip -c 2>/dev/null";
			pipep = 1;
			break;

		case 'B':	// remote file has .RDB
			rdb_doit();
			return;
		}

		if(pipep){
			char buf[8192];
			sprintf(buf, "%s %s", cmd, dirname);
			fp = popen(buf, "r");
		}else{
			fp = fopen(dirname, "r");
		}
		if( !fp ){
			// error
			return;
		}

		doit( fp );
		
		if(pipep)
			pclose(fp);
		else
			fclose(fp);
	}
}

void Image::doit(FILE*fp){
	struct GEMR_Header hdr;

	hdr.read_header(fp);

	x_pix_dim = hdr.get_int_mr_xdim();
	y_pix_dim = hdr.get_int_mr_ydim();
	thickness = hdr.get_float_mr_thick();
	pixel_size = hdr.get_float_mr_psize();
	fov = hdr.get_float_mr_fov();

	// is there anything else we'd like to get from the header?
		
		
	if(!dg_data)
		delete [] dg_data;
	dg_data = new u_short [ x_pix_dim * y_pix_dim ];

	// load data
	if(hdr.tag == 5){
		// 5x format

		int x,y, off=0;
		int xdim=hdr.u.ge5x.img_width;
		int ydim=hdr.u.ge5x.img_height;
		int start, end, bot;
		int upi=0;
		u_char c;
		u_short s;
		int prev;

		// skip blank lines at top
		for(y=0; y< hdr.u.ge5x.img_top_offset; y++){
			for(x=0;x<xdim; x++)
				dg_data[off++] = hdr.u.ge5x.img_bgshade;
		}
		bot = ydim-hdr.u.ge5x.img_bot_offset;
			
		for( ; y<bot; y++){

			if(   hdr.u.ge5x.img_compress == IC_PACKED
			      || hdr.u.ge5x.img_compress == IC_COMPACK){
				start = get_short( (char*)hdr.p.up + upi);
				end   = start + get_short( (char*)hdr.p.up + upi + 2);
				upi += 4;
			}else{
				start = 0;
				end = xdim;
			}
				// pad start of row if packed
			for(x=0; x<start; x++) dg_data[off++] = hdr.u.ge5x.img_bgshade;

			if(   hdr.u.ge5x.img_compress == IC_COMPRESSED
			      || hdr.u.ge5x.img_compress == IC_COMPACK){
				while( x++ < end){
					fread((char*)&c, 1,1, fp);
					if(! (c&0x80)){
				// 7 bit delta
						if( c&0x40) c |= 0x80; 	// sign extend;
						prev += (signed char)c;
					}else if( c&0x40 ){
				// actual value
						fread((char*)&s, 2,1, fp);
						prev = s;
					}else{
				// 14 bit delta
						if( c&0x20 ) c |= 0xC0;	// sign extend
						else c &= ~0xC0;
						s = c << 8;
						fread((char*)&s+1, 1,1, fp);
						prev += (signed short)s;
					}
				/// ???
					if( prev < hdr.u.ge5x.img_undflow)
						prev = hdr.u.ge5x.img_undflow;
					if( prev > hdr.u.ge5x.img_ovrflow)
						prev = hdr.u.ge5x.img_ovrflow;
						
					dg_data[off++] = prev;
				}

			}else{
				while( x++ < end){
					fread((char*)&s, 2,1, fp);
					dg_data[off++] = s;
				}
			}
				// pad end of line if packed
			for( ; x<xdim; x++) dg_data[off++] = hdr.u.ge5x.img_bgshade;
		}

		// skip blank lines at bottom
		for( ; y<ydim; y++){
			for(x=0;x<xdim; x++)
				dg_data[off++] = hdr.u.ge5x.img_bgshade;
		}

	}else{
		// 4x format
		fread ((char*)dg_data, sizeof(short),256*256,fp);	// slurp it in
	}
		
	multby = fov * fov / ( x_pix_dim * y_pix_dim * 4) * thickness;

		
	fimg.width = x_pix_dim;
	fimg.height = y_pix_dim;
	fimg.resize();

	// stripfat(dg_data, fimg);	// map data
	// put stripfat in with segm'ing

	// following is taken from image_map in old version
	// (which came from the nice folks at GE
	// (mrconsole frames.c:copy_12_bits()))

		
	int minp=65536, maxp=0, min_pixel=0, max_pixel=255,j;
	int pixel, offset, width;
	u_short *from=dg_data;
	float *to = fimg;
	int i;

	for(i=0;i<x_pix_dim*y_pix_dim;i++){
		pixel = (0xFFFF & dg_data[i]);
		if (pixel > 0 && pixel < minp) minp = pixel;
		if (pixel > maxp) maxp = pixel;
	}

cerr << "Min: " << minp << " Max: " << maxp << endl;
	
	offset = minp;
	width = maxp - minp;
	if (width == 0) width = 1;
	for (j = 0; j < y_pix_dim; j++) {
		for (i = 0; i < x_pix_dim; i++) {
			pixel = (0xFFFF & *(from++));
			pixel = ((pixel - offset) << 8) / width;
			if (pixel < min_pixel) {
				pixel = min_pixel;
			}
			if (pixel > max_pixel) {
				pixel = max_pixel;
			}
			*(to++) =  pixel;
		}
	}

#ifdef DO_HISTOEQ

	float hist[256*256], cum[256*256];

		
	for(i=0;i<256*256;i++) hist[i]=0;
	for(i=0;i<256*256;i++) hist[(int)fimg[i]]++;

	for(i=0;i<256*256;i++) hist[i] /= (64*1024 / 256);
	cum[0] = hist[0];
		
	for(i=1;i<256*256;i++) cum[i]=cum[i-1]+hist[i];
		
	for(i=0;i<256*256;i++){
		j = cum[(int)fimg[i]];
		j = (j<0)?0:(j>255)?255:j;
		fimg[i] = j;
	}
#endif
	segm.resize();
	drawing.resize();
	drawing2.resize();
	undo.resize();
	view.resize();
	
	load_segm();
	update_undo();	
}


int Image::load_cont(char *fname){
	
	chdir(mriname("=S"));	// default dir
	if(drawing2.load(mriname(fname))){
		int i;
		drawing.resize();
		for(i=0;i<drawing2.width*drawing2.height;i++){
			drawing[i] = drawing2[i]?0x80:0;
		}
		drawing.dilate( CINT(getvalue(maksym("mri:line-width"))) );
		C(IFB_DRAWN_ON);
		S(IFB_CONTOUR_PRESENT);
		if(this == UI->c_image) UI->set_contour(drawing);
		return 1;
	}else
		return 0;	
}
	
extern "C" void save_series_conts(Xv_opaque it){

	// me(it).interpret("save contour $R");
	me(it).interpret("(save contour 0 1000)");
}

extern "C" void save_image_cont(Xv_opaque it){

	// me(it).interpret("save contour");
	me(it).interpret("(save contour)");
}

