
// Copyright (c) 1993 Jeff Weisberg
// University of Rochester
//	Dept. of Electrical Engineering
//	Dept. of Neurology, C.E.P.
//
// see the file "License"

#ifdef RCSID
static const char *const rcsid
= "@(#)$Id: object.cc,v 1.1 1995/11/23 04:30:21 jaw Exp jaw $";
#endif

#include <mritool.h>
#include <object.h>
#include <stddef.h>
#include <stdio.h>

// we want to keep a 2-LL of Objects
	
Object* Object::_head = 0;
Object* Object::_tail = 0;

// there seems to be a problem with the static members
// (wrong things are be overwritten)
// try fully qualifying...

Object::Object(){
	
	_lock  = 'L';
	_valid = 'V';
	_next  =  0;
	_prev  =  0;

	if( !Object::_head )
		Object::_head = this;
	else
		Object::_tail->_next = this;
	
	_prev = _tail;
	Object::_tail =  this;

}

Object::~Object(){

	if( !Object::_head || !this){
		bug(__FILE__ ": object::~", this?"!_head":"!this");
		return;
	}

	if( _prev) _prev->_next = _next;
	if( _next) _next->_prev = _prev;
	if( Object::_head == this)Object::_head = _next;
	if( Object::_tail == this)Object::_tail = _prev;

	_next  = 0;
	_prev  = 0;
	_valid = 0;
}


void garbageman(void){
	// garbageman, oh garbageman
	
	Object* ob;

	cerr << "Trashman's here!" << endl;
	
	for(ob=Object::_head; ob; ob=ob->_next)
		if( !ob->locked()){
			cerr << "\tCleaned up a " << ob->identity() << endl;
			ob->cleanup();
		}
}
	    

void Object::cleanup(void){
	// since we never explicitly create an Object
	// this ought never be called
	// the derived (virtual) cleanup should be
	bug(__FILE__ ": object::cleanup");
}

void out_of_memory(void){
	// installed as new_handler in main()
	fprintf(stderr, "Out of memory...Dooooh!\n");
	abort();
	sleep(60);
	exit(-1);
}

#if 1
void* operator new(size_t size){
	void *m;

	m = malloc(size);
	if(!m){
		// who you gonna call?
		garbageman();
		m = malloc(size);
		if(! m)
			out_of_memory();
	}
	return m;
}

void operator delete(void *v){
	::free( (char*)v);
}
#endif



	






	
	

	

























