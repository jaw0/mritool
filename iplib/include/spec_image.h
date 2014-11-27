#include <headerpp.h>

//	create new class on top of (jon's) image
//	so as to allow me to add my own functions

class spec_image : public image {
protected:
	// any private vars go here
public:
	// public vars, and fnc.s go here

	spec_image() {	// constructor
		depth = 8;
		color = 0;
	}
	spec_image(int w, int h) : image(w,h) {	// constructor
		depth = 8;
		color = 0;
	}
	~spec_image() {} // destructor

	// make a white circle at (xo,yo) of radius ro
	void circ(int xo, int yo, float ro);

	// better save and load routines
	// use my =x:n/file convention, and expand ~
	void fsave(char *);
	void fload(char *);

	void zero();			// set image to 0	
	void db();			// convert to db scale 
	void sqr();			// square
	void sqrt();			// square root
	void medn(int size);		// median filter
	void avrg(int size);		// averaging filter
	void lapl(float a);		// laplacian
	void punch(int a, int b);	// punch
	void swap(int a, int b);	// swap colors
	void thresh(int level);		// threshold 
	void slide(int shift);		// slide image and wrap it around ( -> )

		// output useful data	
	friend 	ostream& operator<<(ostream&, spec_image&);
};


