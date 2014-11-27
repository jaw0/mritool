
#include <spec_image.h>
extern "C" {
#	include <ras_stuff.h>
	void unlink(char *pathname);
};

void spec_image::circ(int xo, int yo, float ro){
	//	(xo,yo) is center of circle
	//	ro is radius
	int x,y;
	float r2, ro2=ro*ro;

	for(y=0;y<height;y++)
		for(x=0;x<width;x++){
			r2 = (x-xo)*(x-xo) + (y-yo)*(y-yo);
			if(r2<ro2) data[y*width+x] = 255;	
		}

}

void spec_image::fsave(char *name){
	char *foo = fullname(name);

	unlink(foo);	// so it doesn't ask if I want to overwrite it
	save(foo);
}

void spec_image::fload(char *name){
	char *foo = fullname(name);

	load(foo);
}
void spec_image::db(){
	int i,size=height*width;

	for(i=0;i<size;i++){
		data[i] = 20.0 * log( data[i] );
	}
}

void spec_image::sqr(){
	int i,size=width*height;

	for(i=0;i<size;i++) data[i] *= data[i];
}

void spec_image::sqrt(){
	int i,size=width*height;

	for(i=0;i<size;i++) data[i] = ::sqrt( data[i] );
}

void spec_image::swap(int a, int b){	
	int i,size=width*height;

	for(i=0;i<size;i++){
		if( (int)data[i]==a ) data[i] = b;
		else if( (int)data[i]==b ) data[i] = a;
	}
}

ostream& operator<<(ostream& os, spec_image& a){

	os<< a.width << " * " << a.height << " * " << a.depth << 
		(a.color?" color image":" image") << "\n" << flush;

	return os;
}

void spec_image::thresh(int level){
	int i, size = width*height;

	for(i=0;i<size;i++)
		data[i] = (data[i]>level)?255:0;
}



void spec_image::lapl(float a){
	int i, n, size=width*height;
	int x, y;
	float b = 1.0, L;
	image ndat(width,height);

	for (y=0;y<height;y++){
		n=y*width;
		for(x=0;x<width;x++){
			/*for @ pt in image */
			if ( (x != 0)&&(x != width)&&(y != 0)&&(y != height) ){
				L=4*data[n]-data[n-1]-data[n+1]-data[n-width]-data[n+width];
				ndat[n] = (b*data[n] + a*L);
			}
			n++;
		}
	}
	for(i=0;i<size;i++) data[i] = ndat[i];
}



void spec_image::slide(int shift){
	int i, size=width*height;
	int x,y;
	image ndat(width, height);

	for (y=0;y<height;y++){
		for (x=0;x<width;x++){
			ndat[y*width+x] = data[y*width+((x-shift)%width)];
			
		}
	}
	for(i=0;i<size;i++) data[i] = ndat[i];
}

void spec_image::zero(void){
	int i, size=width*height;

	for(i=0;i<size;i++) data[i] = 0;
}


void spec_image::avrg(int wsize){
	
	int i,j, ii,jj;
	float t, k=wsize*wsize;
	
	for(j=wsize/2;j<height-wsize/2;j++)for(i=wsize/2;i<width-wsize/2;i++){
		t = 0;
		for(jj=-wsize/2;jj<wsize/2;jj++)for(ii=-wsize/2;ii<wsize/2;ii++){
			t += data[ i + ii + (j + jj)* width];
		}
		data[i + j * width] = t / k;
	}
}
		











	
