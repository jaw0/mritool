

#ifndef _function_h
#define _function_h

#ifdef RCSID
static const char *const rcsid_function_h
="@(#)$Id: function.h,v 1.1 1995/11/23 04:31:13 jaw Exp jaw $";
#endif

#include <math.h>
#include <complex.h>

class Function {
	// Abstract
  public:
	virtual ~Function() {}
	virtual complex bvalue(float f) const =0;
	
	complex operator[](float f) const {return bvalue(f);}

};



#endif /* !_function_h */

