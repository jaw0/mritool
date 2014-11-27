
#ifndef _fbimage_h
#define _fbimage_h
 
#ifdef RCSID
static const char *const rcsid_fbimage_h
= "@(#)$Id: fbimage.h,v 1.1 1995/11/23 04:31:13 jaw Exp jaw $";
#endif
 
#include <images.h>

class Framebuffer : public Uimage_fixed {
  private:
	int fb;
	int mmapsize;
	u_char *fb_base;
  public:
	Framebuffer(const char* devname=0);
	Framebuffer(const Framebuffer&);
	~Framebuffer();

	void install_cmap(void) const;
	void get_cmap(void);
	void blank(void) const;
	void unblank(void) const;
	void set_cursor_pos(Point);	// these will confuse the window system
	Point get_cursor_pos(void) const;
	void set_cursor_shape(const Bimage&);
	
};

#endif /* !_fbimage_h */

