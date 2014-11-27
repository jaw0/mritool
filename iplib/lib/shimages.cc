

// Copyright (c) 1993 Jeff Weisberg
// University of Rochester
//	Dept. of Electrical Engineering
//	Dept. of Neurology, C.E.P.
//
// see the file "License"

#ifdef RCSID
static const char *const rcsid
= "@(#)$Id: shimages.cc,v 1.1 1995/11/23 04:31:06 jaw Exp jaw $";
#endif

#include <images.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define loop(i,j)	for(i=0;i<j;i++)


ShMem::ShMem(){
	shm_key = sh_unique_key++;
	shm_id = 0;
}

ShMem::~ShMem() {}
	
void* ShMem::shmalloc(int bytes){

	if(!shm_id){
		shm_id = shmget( shm_key, bytes, IPC_CREAT | SHM_RND );
	}

	return (void*)shmat( shm_id, 0, 0);
}

void ShMem::shmfree(void* addr){

	if(addr && shm_id)
		shmdt( (char*)addr );
}

ShFimage::ShFimage(){
}
ShFimage::~ShFimage(){
}
ShUimage::ShUimage(){
}
ShUimage::~ShUimage(){
}

ShFimage::ShFimage(int x, int y){
	height = y;
	width = x;
	
	// create and allocate shared memory
	size = height * width;
	data = (float*)shmalloc( size * sizeof(float) );
	Fimage_fixed::init(data, size);
}

ShUimage::ShUimage(int x, int y){
	height = y;
	width = x;
	
	// create and allocate shared memory
	size = height * width;
	data = (u_char*)shmalloc( size * sizeof(u_char) );
	Uimage_fixed::init(data, size);
}
		
void ShFimage::cleanup(void){

	shmfree( data );
	size = 0;
	data = 0;
}

void ShUimage::cleanup(void){

	shmfree( data );
	size = 0;
	data = 0;
}


