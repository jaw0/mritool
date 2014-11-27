

#ifndef _func_sinc_h
#define _func_sinc_h

#ifdef RCSID
static const char *const rcsid_func_sinc_h
="@(#)$Id: sinc.h,v 1.1 1995/11/23 04:31:12 jaw Exp jaw $";
#endif

#include <function.h>

class Sinc : public Function {
	float mu, per;
  public:
	Sinc(float a=0.0, float b=1.0){mu=a;per=b;}

	virtual complex bvalue(float x) const {
		float v = M_PI*(x-mu)/per;
		return v ? sin(v)/v : 1.0;
	}
};


#endif !_func_sinc_h
