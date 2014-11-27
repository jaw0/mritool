
#ifndef _func_delta_h
#define _func_delta_h


#ifdef RCSID
static const char *const rcsid_func_delta_h
="@(#)$Id: delta.h,v 1.1 1995/11/23 04:31:12 jaw Exp jaw $";
#endif

#include <function.h>

class Delta : public Function {
	float at;
  public:
	Delta(){at=0;}
	Delta(float a){at=a;}

	virtual complex bvalue(float x) const {
		return (x==at);
	}
};




#endif !_func_delta_h
