
#include <images.h>

static const xxPointxx nghbrs[] = {
	// the 8 neighbors of a point in CCw direction
	{0,0}, {0,0}, // skip points 0, 1
	{0,-1}, {-1,-1}, {-1,0}, {-1,1},
	{0,1},  {1,1},  {1,0},  {1,-1}
};


void Bimage::pretty(int c, int maxpass){
	int x,y,i, j, mvd, pass=0;
	int ns, cl, xc, pl;
	int cl2, xc2, cl4, xc4, pl2, pl4;

	Bimage *img = clone();
	Bimage *nimg = clone();


	for(i=0;i<width*height;i++){
		nimg->put(i, 0);
		img->put(i, c?
			 ((int(value(i))==c)?1:0)
			 :(int(value(i))?1:0));
	}

	do{
		mvd = 0;

		for(y=2;y<height-2;y++)for(x=2;x<width-2;x++){
			nimg->put(x,y, img->value(x,y));

			if( int(img->value(x,y))){


				ns = xc = xc2 = xc4 = 0;
				cl  = int( img->value(x+nghbrs[9].x, y+nghbrs[9].y));
				cl2 = int( img->value(x+nghbrs[9].x+nghbrs[2].x, y+nghbrs[9].y+nghbrs[2].y));
				cl4 = int( img->value(x+nghbrs[9].x+nghbrs[4].x, y+nghbrs[9].y+nghbrs[4].y));
				for(i=2;i<10;i++){
					pl  = int( img->value(x+nghbrs[i].x, y+nghbrs[i].y));
					pl2 = int( img->value(x+nghbrs[i].x+nghbrs[2].x, y+nghbrs[i].y+nghbrs[2].y));
					pl4 = int( img->value(x+nghbrs[i].x+nghbrs[4].x, y+nghbrs[i].y+nghbrs[4].y));
					
					ns += pl;		// count nghbrs
					if( (!cl)  && pl  ) xc ++;	// count 0->1 transitions
					cl = pl;
					if( (!cl2) && pl2 ) xc2 ++;	// count 0->1 transitions
					cl2 = pl2;
					if( (!cl4) && pl4 ) xc4 ++;	// count 0->1 transitions
					cl4 = pl4;
					
				}

				if (ns>=3 && ns<=5
				    && xc==1
				    &&(
				       !(
					     int(img->value(x+nghbrs[2].x, y+nghbrs[2].y))
					  && int(img->value(x+nghbrs[4].x, y+nghbrs[4].y))
					  && int(img->value(x+nghbrs[8].x, y+nghbrs[8].y))
				       )
				       ||(
					  xc2 != 1
				       )
				       
				    )
				    &&(
				       !(
					     int(img->value(x+nghbrs[2].x, y+nghbrs[2].y))
					  && int(img->value(x+nghbrs[4].x, y+nghbrs[4].y))
					  && int(img->value(x+nghbrs[6].x, y+nghbrs[6].y))
				       )
				       ||(
					  xc4 != 1
				       )
				       
				    )
				){ 		 // (((this) is) (starting (to) (look (like ('lisp)))))
					// delete pixel
					nimg->put(x,y, 0);
					mvd++;
				}
			}
		}

		for(i=0;i<width*height;i++)
			img->put(i, nimg->value(i));

	}while(mvd && ++pass< maxpass);

	swdat( img );

	delete img;
	delete nimg;
}
