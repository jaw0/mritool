
#ifndef _segment_h
#define _segment_h

#define SEGMFILE_MAGIC  ":jwMRs02"

struct SegmFile {
	char magic[10];
	int xdim, ydim;
	int nlevs;
	boolean offline;
	int pid;
};

#if 0
extern void pullsec(image a,image& b,int hcent,int vcent);
extern void edge(image& a);
extern void max_eng(image roi,image& snake,float tho,float ro);
extern void max_eng(image roi,image& snake,float tho,float ro, image p, float d);
extern void close(image snakey,image& boy);
extern void draw (image& a,int b,int c,int d,int e);
extern void dirode (image& cont);
extern void sand (image& a);
extern void reader (image a,image& snak);
extern void impose (image snake,image& boy);
extern void stimpose (image snake,image& boy);
extern void musigest(image& a,int *clust,int k);
extern void musigest(image& a,int *clust,int k, float *m, image &p, int usd);
extern void kmeans(int *clust,image& emg,int nline,int npixl,int k,int max_iter);
extern void thresh(image& roi,int nlev);
extern void minit(float *m, int n, image a);
#endif


#endif

