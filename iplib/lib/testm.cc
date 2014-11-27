

#include <matrix.h>
#include <func/gaussian.h>

main(){

	Vector a(100), b(100), g(94, Gaussian(0,2.0));
	int i;
	
	for(i=0;i<100;i++) a[i] = i%10;

	a[50] = .5;

	b = cconvolve(a, g);

	// cout << b.t() << endl;
	cout << g.t() << endl;

}

