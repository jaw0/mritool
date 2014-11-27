
#define MAXNAMELEN 160
#include <stdio.h>
#include <rasterfile.h>

#ifndef u_char
typedef unsigned char u_char;
#endif

#ifdef CPLUSPLUS
	char *fullname(char *name);
#else
	char *fullname();
#endif
FILE *ras_open_ghdr();
void ras_read_rgb();
void ras_read_bin();
void ras_read_ucr();
int ras_write_ucr();
u_char **malloc_2d(), ***malloc_3d();
int vff_write_3d();


