
// Copyright (c) 1993 Jeff Weisberg


#ifdef RCSID
static const char *const rcsid
= "@(#)$Id: cimages.cc,v 1.1 1995/11/23 04:30:56 jaw Exp jaw $";
#endif

#include <images.h>
void FFT2D(complex * , int , int , int , int );
 
extern "C"  {
        void FFTMX2D(float *,float *,int,int,int);
};

void Bimage::ifft(int dir, int cntr){
 	fft(dir, cntr);
}
void Bimage::fft(int dir, int center){
	// code stolen from Jon Riek
	// blame him

	int mfft,nfft,i,j,k,isn,isn2,size=width*height;
	mfft=nfft=1;

	// Jon's library miscalculated and dumped core
	// if heigth or width == 1
	mfft = 1 << int( ceil( log2( width )));
	nfft = 1 << int( ceil( log2( height)));
	
	if(height==nfft && width==mfft){
		complex *data;
		data = new complex[width * height];
		for(j=0;j<height;j++)
			for(i=0;i<width;i++)
				data[j*width+i] = cvalue(i, j);
		FFT2D(data,width,height,dir,center);

		for(j=0;j<height;j++)
			for(i=0;i<width;i++)
				cput(i,j, data[j*width+i]);

		delete [] data;
		return;
	}else{
		float *a,*b,*ta,*tb;
		complex *tc;
		a=new float[size];
		b=new float[size];

		ta=a;
		tb=b;
		
		if(!center){
			for(k=0;k<height;k++)
				for(j=0;j<width;j++){

					*(ta++) = ::real( cvalue(j, k));
					*(tb++) = ::imag( cvalue(j, k));
				}
		}else{	
			isn2=1;
			for(k=0;k<height;k++,isn2=-isn2)
				for(isn=isn2,j=0;j<width;j++,tc++,isn=-isn){
					
					*(ta++)= ::real( cvalue(j, k))*isn;
					*(tb++)= ::imag( cvalue(j, k))*isn;
				}
		}
		FFTMX2D(a,b,width,height,dir);
		ta = a; tb = b;
		
		for(k=0;k<height;k++)
			for(j=0;j<width;j++)
				cput(j, k, complex( *(ta++),*(tb++)));

		delete [] a;	// Jon's library leaks
		delete [] b;	// (no delete)
	}
	return;
}

void Bimage::real(const Bimage& a){
	int i;

	if(!checksize(a.width*a.height)) return;

	width = a.width;
        height = a.height;
 
        for(i=0;i<width*height;i++){
                float v = ::real( a.cvalue(i));
                put(i, v);
        }
}

void Bimage::imag(const Bimage& a){
	int i;

	if(!checksize(a.width*a.height)) return;

	width = a.width;
        height = a.height;
 
        for(i=0;i<width*height;i++){
                float v = ::imag( a.cvalue(i));
                put(i, v);
        }
}
void Bimage::magnitude(const Bimage& a){
	int i;

	if(!checksize(a.width*a.height)) return;

	width = a.width;
        height = a.height;
 
        for(i=0;i<width*height;i++){
                float v = abs( a.cvalue(i));
                put(i, v);
        }
}


void Bimage::phase(const Bimage& a){
	int i;

	if(!checksize(a.width*a.height)) return;

	width = a.width;
        height = a.height;
 
        for(i=0;i<width*height;i++){
                float v = arg( a.cvalue(i));
                put(i, v);
        }
}

