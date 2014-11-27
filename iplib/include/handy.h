

// some handy macros


#ifndef MAX
#       define MAX(x,y) ((x)>(y)?(x):(y))
#endif
#ifndef MIN
#       define MIN(x,y) ((x)<(y)?(x):(y))
#endif
#ifndef ABS
#	define ABS(x)	((x)<0?-(x):(x))
#endif
#ifndef loop
#	define loop(i,j)     for(i=0;i<j;i++)
#endif
#ifndef loopp
#	define loopp(p, a, b)  for(p.y=a.y;p.y<b.y;p.y++)for(p.x=a.x;p.x<b.x;p.x++)
#endif
#ifndef looppp
#	define looppp(p, a, b)	for(p.z=(a).z;p.z<(b).z;p.z++)for(p.y=(a).y;p.y<(b).y;p.y++)for(p.x=(a).x;p.x<(b).x;p.x++)
#endif


