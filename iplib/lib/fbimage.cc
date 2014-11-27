
 
#ifdef RCSID
static const char *const rcsid
= "@(#)$Id: fbimage.cc,v 1.1 1995/11/23 04:31:01 jaw Exp jaw $";
#endif

#include <fbimage.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sun/fbio.h>
#include <sundev/cg6reg.h>
#include <sundev/cg4reg.h>
#include <sundev/bw2reg.h>
// #include <pixrect/cg3var.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

// from pixrect/cg3var.h
#if defined(mc68000) || defined(sparc)
#define CG3_MMAP_OFFSET 0x04000000      /* 8K x 8K */
#else
#define CG3_MMAP_OFFSET 0
#endif
 

inline void align(int& x, int a){
        x += (a - x%a)%a;
}

Framebuffer::Framebuffer(const char* devname){
	fbgattr atr;
	int mmapoff, im_off=0, ov_off=0, en_off=0;
	int im_len=0, ov_len=0, en_len=0;

	fb_base = 0;
	mmapsize = 0;
	
	fb = open(devname?devname:"/dev/fb", O_RDWR);
	if(fb==-1){
		perror(devname?devname:"/dev/fb");
		return;
	}
	if(ioctl(fb, FBIOGATTR, &atr)==-1
           && ioctl(fb, FBIOGTYPE, &atr.fbtype)==-1){
                perror("fbio get type");
                return;
        }
	im_len = atr.fbtype.fb_size;

	switch(atr.fbtype.fb_type){
		// how about:
		// cg2, cg8, cg9, cg12 ???
 
          case FBTYPE_SUNFAST_COLOR:    // cg6
                mmapoff = CG6_VBASE;
                im_off  = (int)CG6VA_TO_DFB(0);
		
		en_len = ov_len = 0;	// are none
                en_off = ov_off = 0;   
                break;
 
          case FBTYPE_SUN3COLOR:        // cg3
                // has no include file
		// this is mostly just a guess
		
		mmapoff= CG3_MMAP_OFFSET;
                ov_off = 0;			ov_len = BW2_FBSIZE;
                en_off = ov_off + ov_len;	en_len = BW2_FBSIZE;
                im_off = en_off + en_len;	
                break;
		
          case FBTYPE_SUN4COLOR:        // cg4
                mmapoff= 0;
                ov_off = 0;			ov_len = BW2_FBSIZE;
                en_off = ov_off + ov_len;	en_len = BW2_FBSIZE;
                im_off = en_off + en_len;	
                break;
 
          case FBTYPE_SUN2BW:           // bw2
		// a 1 bit image type is not really suported
		// by the library
		
                // fall thru
          default:                      // other
                mmapoff = 0;
                ov_off = en_off = im_off = 0;
                break;
        }

	// how big a chunk do we need to map?
	mmapsize = im_len + en_len + ov_len;
	align(mmapsize, 8192);
	
        // snarf the fb
        fb_base = (u_char*)mmap(0, mmapsize, PROT_READ|PROT_WRITE, MAP_SHARED, fb, mmapoff);
	if(fb_base==(u_char*)-1){
                perror("mmap");
		return;
        }

	height = atr.fbtype.fb_height;
	width = atr.fbtype.fb_width;
	
	init(fb_base+im_off, mmapsize);
}

Framebuffer::~Framebuffer(){

	munmap((char*)fb_base, mmapsize);
	close(fb);
}

void Framebuffer::blank(void) const{
	int i = FBVIDEO_OFF;

	ioctl(fb, FBIOSVIDEO, &i);
}
void Framebuffer::unblank(void) const{
	int i = FBVIDEO_ON;

	ioctl(fb, FBIOSVIDEO, &i);
}

void Framebuffer::install_cmap(void) const {

	if(!cmap) return;

	fbcmap fc;

	fc.index = 0;
	fc.count = cmap->len;
	fc.red = cmap->r;
	fc.blue = cmap->b;
	fc.green = cmap->g;

	ioctl(fb, FBIOPUTCMAP, &fc);
}

void Framebuffer::get_cmap(void) {
	if(!cmap) return;

	fbcmap fc;
	fc.count = 256;
	fc.index = 0;
	fc.red = cmap->r;
	fc.blue = cmap->b;
	fc.green = cmap->g;
	
	ioctl(fb, FBIOGETCMAP, &fc);
}

Point Framebuffer::get_cursor_pos(void) const {
	fbcurpos fbcp;

	ioctl(fb, FBIOGCURPOS, &fbcp);
	return Point(fbcp.x, fbcp.y, 0);
}

void Framebuffer::set_cursor_pos(Point pt){
	fbcurpos fbcp;
	fbcp.x = pt.x;
	fbcp.y = pt.y;

	ioctl(fb,FBIOSCURPOS, &fbcp);
}
void Framebuffer::set_cursor_shape(const Bimage& a){
	fbcursor fbc;
	int i;

	fbc.hot.x = a.width/2;
	fbc.hot.y = a.height/2;
	fbc.size.x = a.width;
	fbc.size.y = a.height;
	fbc.image = new char [ a.width * a.height /8];
	for(i=0;i<a.width*a.height; i++){
		if(i%8) fbc.image[i] = 0;
		fbc.image[i/8] |= (int(a.value(i))?1:0)<<(i%8); // <<(7-(i%8));
	}
	fbc.mask = fbc.image;
	fbc.enable = 1;
	fbc.set = FB_CUR_SETCUR | FB_CUR_SETHOT | FB_CUR_SETSHAPE;

	ioctl(fb, FBIOSCURSOR, &fbc);
	delete [] fbc.image;
}


		
	


	


	

