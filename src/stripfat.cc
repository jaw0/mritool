#include <headerpp.h>
typedef unsigned short u_short;

#define nonz(a) ((a)!=0? (a):.00001)
#define pos(a) ((a)>0? (a):-(a))
#define sqr(a) ((a)*(a))
#define BIG 9.99e20
#define pi 3.14159

void kmeans(int *clust,image& emg,int nline,int npixl,int k,int max_iter);
void kmeans(image &emg, int *clust, int k, float *m);

void stripfat(u_short *signa_data, image &a) {

	int i,nlev;
	float m[10]; // something >5
	int *clusta;

	for(i=0; i<256*256;i++) a[i] = signa_data[i];
	a.quantize(0);
	nlev=5;
        clusta=(int *)calloc(256*256,sizeof(int));
	for(i=0;i<nlev;i++)
		m[i] = i*255/(nlev+1);
	
#ifdef NOTGREATER	
	kmeans (clusta,a,256,256,5,30);
#else	
	kmeans(a, clusta, nlev, m);
#endif
	
	for (i=0;i<a.width*a.height;i++)
		if (clusta[i]==(nlev-1))
			a[i]=0;
	a.quantize(0);

}

