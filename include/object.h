

// Copyright (c) 1993 Jeff Weisberg
// University of Rochester
//	Dept. of Electrical Engineering
//	Dept. of Neurology, C.E.P.
//
// see the file "License"

#ifndef _OBJECT_H
#define _OBJECT_H

static const char *const rcsid_object_h
	= "@(#)$Id: object.h,v 1.1 1995/11/23 04:30:01 jaw Exp jaw $";

#undef PATIENT

#include <stddef.h>

extern void* operator new(size_t);
extern void  operator delete(void *);

class Object {
    friend void garbageman(void);
  public:
	enum Type {
		NONE, OBJECT, BASE, IMAGE, SERIES, STUDY, PATIENT,
		SNAKE, SNAKELET, POINT, FIMAGE, UIMAGE
	};
  private:
	char		_lock;
	char		_valid;
	static Object*	_head;
	static Object* 	_tail;
	Object*		_next;
	Object*		_prev;
	Object(Object&);

  protected:
	virtual void lock(void)	 { _lock = 'L'; }
	virtual void unlock(void){ _lock = 0; }
	virtual void cleanup(void);
	virtual int  locked(void) const {return _lock=='L';}
	virtual int  isvalid(void)const {return _valid=='V';}

  public:
	Object();
	~Object();
	virtual Type mytype(void) const {return OBJECT;}
	virtual char* identity(void) const {return "Object";}

};


#endif !_OBJECT_H




