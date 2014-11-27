

// Copyright (c) 1993 Jeff Weisberg
// University of Rochester
//	Dept. of Electrical Engineering
//	Dept. of Neurology, C.E.P.
//
// see the file "License"

#ifdef RCSID
static const char *const rcsid
= "@(#)$Id: bsave.cc,v 1.1 1995/11/23 04:30:54 jaw Exp jaw $";
#endif

#include <images.h>
#include <fstream.h>
#include <rasterfile.h>
#include <unistd.h>
#include <stdlib.h>
#include <fullname.h>
#include <sys/stat.h>
#include <sys/wait.h>
#ifdef PSAVE
#	include <pstream.h>
#	include <ptable.h>
#endif

#define loop(i,j)	for(i=0;i<j;i++)
int mkdirhier(const char* path);
static const char* extension(const char*);

inline int stringlen(const char* name){
	int i; for(i=0;name[i];i++); return i;
}
inline void stringcat(char* t, const char* a){
	while(*t) t++;	// to end
	while(*a) *t++ = *a++;
	*t = 0;
}

int Bimage::save(const char *name) const{
        int i;
	const char *fname = fullname(name);
	const char* cmd;
	int slen=0;

	if(*fname!='|'){
		// save to a file
		slen = stringlen(fname);
		char *fooo = new char[ slen+1 ];
		for(i=0;i<=slen;i++)fooo[i] = fname[i];
		for(i=slen;i>=0;i--){
			if(fooo[i]=='/'){
				fooo[i] = 0;
				// make sure directory exists
				if(*fooo) mkdirhier(fooo);
				break;
			}
		}
		delete [] fooo;
		// file xlation based on .extension
		cmd = ptable ? ptable->slookup( extension(fname) ) : 0;
	}else{
		// allow save to "|command"
		cmd = fname;
	}

	if( cmd && *cmd ){
		char* buffer = new char[ stringlen(cmd) + stringlen(fname) +30 ];
		*buffer = 0;
		stringcat(buffer, "( ");
		if(*cmd!='|'){
			stringcat(buffer, cmd);
			stringcat(buffer, " > ");
			stringcat(buffer, fname);
		}else{
			stringcat(buffer, cmd+1);
		}
		stringcat(buffer, " ) 2>/dev/null");
		
		opstream op(buffer);
		if(!op) return 0;
		int status = save( op );
		delete [] buffer;
		
		// pstream does not properly wait for children
		// and xview won't let us catch sigchld
		// so we do the wait here...
		
		waitpid(-1, 0, WNOHANG);
		return status;
		
	}else{
		ofstream fp(fname);
		if(!fp) return 0;
		return save( fp );
	}
}

int Bimage::load(const char* name){
	const char *fname = fullname(name);
	const char *cmd = ptable ? ptable->llookup(extension(fname)) : 0;

	if( cmd ){
		char* buffer = new char[ stringlen(cmd) + stringlen(fname) +30 ];
		*buffer = 0;
		if(*cmd!='|'){
			stringcat(buffer, "( cat ");
			stringcat(buffer, fname);
			stringcat(buffer, " | ");
			stringcat(buffer, cmd);
		}else{
			stringcat(buffer, "( ");
			stringcat(buffer, cmd+1);
		}
		stringcat(buffer, " ) 2>/dev/null");

		ipstream ip(buffer);
		if(!ip) return 0;
		int status = load( ip );
		delete [] buffer;
		
		// see comment above in save...
		waitpid(-1, 0, WNOHANG);
		return status;
		
	}else{
		ifstream fp(fname);
		if(!fp) return 0;
		return load( fp );
	}
}

int Bimage::save(ostream& fp) const{
	int x,y;
	int xmod;
        rasterfile rast;
	
        rast.ras_magic     = RAS_MAGIC;
        rast.ras_type      = RT_STANDARD;
        rast.ras_maptype   = cmap ? RMT_EQUAL_RGB : RMT_NONE;
        rast.ras_maplength = cmap ? cmap->len*3 : 0;
        rast.ras_width     = width;
        rast.ras_height    = height;
        rast.ras_depth     = 8;
        rast.ras_length    = width * height;
 
        xmod = width + (2-width%2)%2;
 
	fp.write( (char*)&rast, sizeof(rasterfile));
	if(cmap && cmap->len){
		fp.write(cmap->r, cmap->len);
		fp.write(cmap->g, cmap->len);
		fp.write(cmap->b, cmap->len);
	}
	
	if(size < width*height) return 0;
	u_char *rowbf = new u_char[ xmod ];
	
        for(y=0;y<height;y++){
                for(x=0;x<width;x++){
                        rowbf[x] = u_char( value(y*width + x) );

                }
                for(;x<xmod;x++) rowbf[x] = 0;
		fp.write(rowbf, xmod);
        }
	delete [] rowbf;
        return 1;
}

int Bimage::load(istream& fp){
        int x,y;
        u_char foo;
        int xmod;
	
        rasterfile rast;
        fp.read( (char*)&rast, sizeof(rasterfile));
        if( rast.ras_magic   != RAS_MAGIC)   return 0;
        if( rast.ras_type    != RT_STANDARD) return 0;
 
        width = rast.ras_width;
        height = rast.ras_height;
        resize();

	if( rast.ras_maplength){
		if(! cmap){
			cmap = prv_cmap = new Cmap;
			flags &= HAS_PRV_CMAP;
		}	
		for(int i=0;i<256;i++)
			cmap->r[i] = cmap->g[i] = cmap->b[i] = i;
		fp.read(cmap->r, rast.ras_maplength/3);
		fp.read(cmap->g, rast.ras_maplength/3);
		fp.read(cmap->b, rast.ras_maplength/3);
	}
	
        xmod = width + (2-width%2)%2;
	
        switch(rast.ras_depth){
 
          case 8: {
		  // Compiler upgrade -- grumble, grumble, ...
		  u_char *rowbf = new u_char[ xmod ];
		  for(y=0;y<height;y++){
			  fp.read(rowbf, xmod);
			  for(x=0;x<width;x++){
				  put(y*width+x, float(rowbf[x]));
			  }
		  }
		  delete [] rowbf;
		  break;
	  }
	  case 1:
		for(y=0;y<height;y++){
			for(int xs=0;xs<(width+7)/8;xs++){
				fp.read(&foo, 1);
				for(int i=0;i<8;i++)
					put(y*width+xs*8+i, (foo&(1<<(7-i)))?0xFF:0 );
			}
			for(; xs< (xmod+7)/8; xs++) fp.read(&foo, 1);
		}
		break;
				
          default:
                break;
        }
 
        return 1;
}



int mkdirhier(const char* path){
	const char *p=path;
	char *ppath;
	int slen=0;
	for(;path[slen];slen++);
	ppath = new char [slen +1];
	char *pp = ppath;

	while(*p) *pp++ = *p++;		// copy path
	*++pp = 0;
	while(pp!=ppath){		// remove last piece
		if(*pp=='/'){
			*pp=0;
			break;
		}
		pp--;
	}

	if(!*ppath || pp==ppath){
		// foo!
	}else{
		mkdirhier(ppath);
	}
	delete [] ppath;
	
	if( access( path, F_OK )){		// returns 1 on error
		return !mkdir( path,0755 );	// ditto
	}
	return 1;	// already exists
}

const char* extension(const char* name){
	const char* p=name;

	while(*p)p++; // find end
	do{
		p--;
		if(*p=='.') return p[1] ? p+1 : 0;
	}while(p!=name);
	
	return 0;
}

	



