

// segment a 3D set of images
// using k-means with 3D GRF
// as per Mike Changs paper


#include <solid.h>
#include <matrix.h>
#include <handy.h>
#include <math.h>


class Update {
  public:
	Point pt;
	int oldclass, newclass;
	Update *next;
};

static float potential(const Solid& segm, int x, int y, int z, int here, float b1, float b2);
static void calc_means(Vector& means, const Bimage& img, const Bimage& clust);
static void calc_means_var(Solid* means, Solid* var, const Solid&  img, const Solid&  segm, int k, float winmin);
static void calc_pots( Solid* pots,   const Solid& segm, int k, float beta1, float beta2);

/*
  static void update(Update* u, Solid* pots, const Solid& segm, int k, float beta1, float beta2);
*/

void Bimage::kmeans(Bimage& clust, int k, int maxiter){
	if(k==0 || k==1){
		// trivial case
		clust = 0;
		return;
	}
	Vector means(k), nmeans(k), mbnd(k-1);
	int iterno;
	
	// find an initial clustering

#if 0	
	clust = *this;
	clust.quantize(k);
	calc_means(means, *this, clust);

#else
	int min, max, x,y, i;
	int hist[ 64 * 1024 ];
	float r;
	
	min = int( minval());
	max = int( maxval());
	r = (sizeof(hist)/sizeof(hist[0])) / (max - min);

	cerr << "min: " << min << " max: " << max << endl;
	// build histogram
	for(i=0; i<(sizeof(hist)/sizeof(hist[0])); i++) hist[i] = 0;
	
	for(y=0; y<height; y++) for(x=0; x<width; x++){
		hist[ int( (value(x,y) - min) * r ) ] ++;
	}
	// -> cdf
	for(i=1; i<(sizeof(hist)/sizeof(hist[0])); i++)
		hist[i] += hist[i - 1];
	
	// cluster
	for(i=x=0; i<k; i++){

		while( (i+0.5)*width*height/float(k) > hist[x] )
			x++;
		means[i] = x/r + min;
	}
	
#endif
	cout << "initial means" << endl;
	cout << means << endl;

	
	
	// iterate till the means hold still or maxiter exceeded
	for(iterno=0; iterno < maxiter; iterno++){
		
		// recluster to nearest mean
		for(i=0;i<k-1;i++)
			mbnd[i] = (means[i+1] + means[i])/2;
		cout << mbnd << endl;
		for(i=0;i<width*height;i++){
			// find closest mean
			// using a binary search (adapted from BSD bsearch())
			float pixel = value(i);
			int lb=0, lm, at;
			for(lm=k;lm;lm/=2){
				at = lb + lm/2;
				if( ( at>=k-1 || pixel <= mbnd[at])
				    && (!at || pixel >= mbnd[at-1]) )
					break;	// found
				if( at<k-1 && pixel > mbnd[at]){
					lb = at + 1;
					lm--;
				}
			}
			clust.put(i, at);
		}

		calc_means(nmeans, *this, clust);
		cout << "means after iter # " << iterno << endl;
		cout << nmeans << endl;
		if( means == nmeans ) break;
		// I love C++, really I do....
		means = *(Function*)&nmeans;

		clust.save("=d/segmk.ras");
	}
}

static void calc_means(Vector& means, const Bimage& img, const Bimage& clust){
	int i;
	Vector sum(means.length);
	int li=-1, ri=-1;
	int lu, ru;
	float lv, rv;
	
	means = 0;
	sum = 0;

	for(i=0; i<img.width*img.height; i++){
		int n = int( clust.value(i) );
		means[ n ] += img.value(i);
		sum[ n ] ++;
	}

	// we have problem if sum[i] = 0 ...
	
	for(i=0; i<means.length; i++){

		if( sum[i] == 0.0 ){

			// take the weighted mean of the closest valid buckets
			if( ri < i){
				// find right index
				for(ri=i+1; ri< means.length; ri++){
					if( sum[i] != 0.0 )
						break;
				}
				if( ri == means.length) ri --;
			}

			if( li == -1){
				lu = 0;
				lv = 0;
			}else{
				lu = li;
				lv = means[ li ] / sum[ li ];
			}
			if( sum[ ri ] == 0.0){
				ru = means.length;
				rv = (lv + 1) * 2;
			}else{
				ru = ri;
				rv = means[ ri ] / sum[ ri ];
			}

			means[ i ] = ( lv + rv ) * (i - lu);
			sum[ i ]   = ( ru - lu );
		}else{
			li = i;
		}
	}

	means /= sum;
}


void Solid::segment(Solid& segm, int k,
		    float alpha, float beta1, float beta2,
		    int maxkm, int maxit, float minwin){

	int i;
	Vector energy(k);

#if 1
	// initialize with kmeans
	for(i=0;i<depth;i++){
		data[i]->kmeans(segm[i], k, maxkm);
	}
#else
#if 0
	int min, max, x,y,z,j;
	int hist[ 64 * 1024 ];
	Vector mns(k);
	
	// build histogram (using only center slice)
	for(i=0; i<(sizeof(hist)/sizeof(hist[0])); i++) hist[i] = 0;
	
	for(y=0; y<height; y++) for(x=0; x<width; x++){
		hist[ int(data[depth/2]->value(x,y)) ] ++;
	}
	// -> cdf
	for(i=1; i<(sizeof(hist)/sizeof(hist[0])); i++)
		hist[i] += hist[i - 1];
	
	// cluster
	for(i=x=0; i<k; i++){
		while( (i+0.5)*width*height/float(k) > hist[x] )
			x++;
		mns[i] = x;
	}

	// build lut
	for(i=j=0;i<k-1;i++){
		int c = (mns[i] + mns[i+1]) / 2;
		for( ;j<c; j++)
			hist[j] = i;
	}
	for( ;j<64*1024;j++)
		hist[j] = k-1;

	// segm
	for(z=0;z<depth;z++)for(y=0;y<height;y++)for(x=0;x<width;x++){
		segm[z].put(x,y, hist[ int(data[z]->value(x,y)) ]);
	}
#else	
	Vector mns(k), nms(k);
	int cp;
	int x,y,z,j;
	int hist[ 64 * 1024 ];

	data[depth/2]->kmeans(segm[depth/2],k,maxkm);

	mns = 0;
	nms = 0;
	
	for(y=0;y<height;y++)for(x=0;x<width;x++){
		mns[ cp=int(segm[depth/2].value(x,y)) ] += data[depth/2]->value(x,y);
		nms[ cp ] ++;
	}
	mns /= nms;
	cerr << "Means: " << mns << endl;
	
	// build lut
	for(i=j=0;i<k-2;i++){
		int c = (mns[i] + mns[i+1]) / 2;
		for( ;j<c; j++)
			hist[j] = i;
	}
	for( ;j<64*1024;j++)
		hist[j] = k-1;

	// segm
	for(z=0;z<depth;z++)for(y=0;y<height;y++)for(x=0;x<width;x++){
		segm[z].put(x,y, hist[ int(data[z]->value(x,y)) ]);
	}
	
#endif	
#endif	
	
	segm[depth/2].save("=d/segmi.ras");
	
	Solid *pots  = new Solid[k];
	Solid *means = new Solid[k];
	Solid *var   = new Solid[k];

	for(i=0;i<k;i++){
		means[i].finit(width, height, depth);
		pots[i].finit(width, height, depth);
		var[i].finit(width, height, depth);
	}
	
	
	calc_pots(pots, segm, k, beta1, beta2);
	
	for(float winsize=1.0; winsize>=minwin; winsize/=2){
		
		if(winsize!=1.0)
			cerr << "Reducing window size to " << winsize << endl;

		
		for(int iterno=0;iterno<maxit;iterno++){
			int x,y,z;
			int chnged = 0;
			
			// calculate some meanage
			calc_means_var(means, var, *this, segm, k, winsize);

			for(z=0;z<depth;z++)for(y=0;y<height;y++)for(x=0;x<width;x++){

				float pixel = data[z]->nvalue(x, y);
				float oclass= segm[z].nvalue(x, y);
				for(i=0;i<k;i++){
					// find energy at each class
					/*
					  E sub { x sub s }
					    = { ( y sub s - mu sub { x sub s } ) sup 2 } over sigma sub {x sub s} sup 2
					    + U( x sub s ) + ln( sigma sub {x sub s} sup 2 )
					*/
					float vr  = 1; // var[i][z].nvalue(x,y);
					float en = pixel - means[i][z].nvalue(x,y);
					vr *= vr;
					en *= en;
					en /= vr;	// en = 1/(v^2) * (y - u)^2
					en += alpha * pots[i](x,y,z) + log( vr );
					energy[i] = en;
					
				}
			
				// find lowest energy class
				int lowat = 0;
				float low = energy[0];
				for(i=1;i<k;i++){
					if(energy[i] < low){
						low = energy[i];
						lowat = i;
					}
				}
				segm[z].nput(x,y, lowat);
				if(lowat != oclass){
					chnged++;
					for(i=0;i<k;i++){
#if 0
						// is there a way to update the maens quickly
						Update *nu = new Update;
						nu->pt = Point(x,y,z);
						nu->newclass = lowat;
						nu->oldclass = oclass;
						nu->next = updates;
#endif						
						pots[i][z].put(x,y, potential(segm, x,y,z, i, beta1, beta2));
						if(x) pots[i][z].put(x-1,y, potential(segm, x-1,y,z, i, beta1, beta2));
						if(y) pots[i][z].put(x,y-1, potential(segm, x,y-1,z, i, beta1, beta2));
						if(z) pots[i][z-1].put(x,y, potential(segm, x,y,z-1, i, beta1, beta2));
						if(x-segm.width+1)  pots[i][z].put(x+1,y, potential(segm, x+1,y,z, i, beta1, beta2));
						if(y-segm.height+1) pots[i][z].put(x,y+1, potential(segm, x,y+1,z, i, beta1, beta2));
						if(z-segm.depth+1)  pots[i][z+1].put(x,y, potential(segm, x,y,z+1, i, beta1, beta2));
					}
				}
			} // for z,y,x
			cerr << "iter #" << iterno << " changed " << chnged  << endl;
			if(!chnged) break;

			segm[ depth/2 ].save("=d/segml.ras");
			
		} // for iter
	} // for winsize

	delete [] means;
	delete [] pots;
}


static float potential(const Solid& segm, int x, int y, int z, int here, float b1, float b2){
	float pot = 0.0;

	// add up clique potentials
	// there are 6 2-pixel 3D cliques

	if(z)
		if( here == segm[z-1].nvalue(x,y) )
			pot -= b2;
		else	pot += b2;
	if(z-segm.depth+1)
		if( here == segm[z+1].nvalue(x,y) )
			pot -= b2;
		else	pot += b2;
	if(x)
		if( here == segm[z].nvalue(x-1, y) )
			pot -= b1;
		else	pot += b1;
	if(x-segm.width+1)
		if( here == segm[z].nvalue(x+1, y) )
			pot -= b1;
		else	pot += b1;
	if(y)
		if( here == segm[z].nvalue(x, y-1) )
			pot -= b1;
		else	pot += b1;
	if(y-segm.height+1)
		if( here == segm[z].nvalue(x, y+1) )
			pot -= b1;
		else	pot += b1;
	return pot;
}

static void calc_pots(Solid* pots, const Solid& segm, int k, float beta1, float beta2){
	int x,y,z, i;

	for(z=0;z<segm.depth;z++)for(y=0;y<segm.height;y++)for(x=0;x<segm.width;x++){
		for(i=0;i<k; i++){
			float p = potential(segm, x,y,z, i, beta1, beta2);
			pots[i][z].put(x,y, p );
			p = pots[i](x,y,z);
		}
	}
}

	
static void calc_means_var(Solid* means, Solid* var, const Solid& img, const Solid& segm, int k, float winsize){
	Point p;
	Point final(img.width, img.height, img.depth);

	/*
	  how 'bout if @@ pt we keep mean[k] and count[k]
	  at x+1, mean[k] = mean[k](x-1) * count[k](x-1)
	                  <add and count the 2 end planes>
			  - sum[k](left-plane)  * cnt[k](left-plane)
			  + sum[k](right-plane) * cnt[k](right-plane)
			  
	*/
	  
	if(winsize <1.0){
		Point ws;
		Matrix  cnt(img.width, k),
			sum(img.width, k),
			pl_sum(img.width, k),
			pl_cnt(img.width, k);
			
		ws =  final * winsize;

		looppp(p, orgin, final){

			if( p.x == 0){
				sum = 0;
				cnt = 0;
				pl_sum = 0;
				pl_cnt = 0;
			}
			

			// ...
			
		}
	}else{
		// no moving window if winsize > 1

		int i;
		Vector sum(k), sum2(k), pts(k);
		float pv;
		
		sum = 0;
		sum2= 0;
		pts = 0;
	
		looppp(p, orgin, final){
			pv = img[p];
			int i = (int)segm[ p ];
			sum[i] += pv;
			sum2[i]+= pv*pv;
			pts[i] ++;
		}
		sum /= pts;
		sum2 /= pts;
		loop(i,k){
			sum2[i] -= sum[i] * sum[i];
		}
		looppp(p, orgin, final){
			for(int i=0; i<k; i++){
				means[i][p.z].put(p, sum[i]);
				var[i][p.z].put(p, sum2[i]);
			}
		}
	}
}

#if 0
static void update(Update* u, Solid* pots, const Solid& segm, int k, float beta1, float beta2){
	Update *uo;
	
	while(u){
		int x=u->pt.x, y=u->pt.y, z=u->pt.z, i;

		for(i=0;i<k;i++){
			
			pots[i][z].put(x,y, potential(segm, x,y,z, i, beta1, beta2));
			if(x) pots[i][z].put(x-1,y, potential(segm, x-1,y,z, i, beta1, beta2));
			if(y) pots[i][z].put(x,y-1, potential(segm, x,y-1,z, i, beta1, beta2));
			if(z) pots[i][z-1].put(x,y, potential(segm, x,y,z-1, i, beta1, beta2));
			if(x-segm.width+1)  pots[i][z].put(x+1,y, potential(segm, x+1,y,z, i, beta1, beta2));
			if(y-segm.height+1) pots[i][z].put(x,y+1, potential(segm, x,y+1,z, i, beta1, beta2));
			if(z-segm.depth+1)  pots[i][z+1].put(x,y, potential(segm, x,y,z+1, i, beta1, beta2));
		}

		uo = u;
		u = u->next;
		delete uo;
	}
}
#endif
		


		
	
	

	

		


		


	
	
