

#ifndef _func_gaussian_h
#define _func_gaussian_h

#ifdef RCSID
static const char *const rcsid_func_gaussian_h
="@(#)$Id: gaussian.h,v 1.1 1995/11/23 04:31:12 jaw Exp jaw $";
#endif

#include <function.h>

class Gaussian : public Function {
	float mu, sigma2;
  public:
	Gaussian(){mu=sigma2=0;}
	Gaussian(float a, float b){mu=a;sigma2=b*b;}

	virtual complex bvalue(float x) const {
		return exp((x-mu)*(mu-x)/2.0/sigma2) / (sqrt(2*M_PI*sigma2));
	}
};


#endif !_func_gaussian_h
