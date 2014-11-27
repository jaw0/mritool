

#ifndef _state_h
#define _state_h

#define STATEFILE_MAGIC  (*((int*)":jwMRt01"))

struct StateFile {
	int magic;
	int pid, study, series, image;
	float x_pix_dim, y_pix_dim, thickness, pixel_size, fov, multby;
	boolean segm;
};







#endif











