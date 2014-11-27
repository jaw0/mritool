


#include <ptable.h>
#include <string.h>
#include <fstream.h>
#include <fullname.h>

int Ptable::hash(const char* p)const{
	int i = 0;

	while(*p) i = i<<1 ^ *p++;
	i = i<0 ? -i : i;

	i %= tsize;
	return i;
}

PBucket* PBucket::find(const char* p) const{

	if(!this || !p) return 0;
	if( !strcmp(p, ext) ) return (PBucket*)this;	// need cast away const for new compiler

	if(!next) return 0;
	return next->find(p);
}

PBucket* Ptable::find(const char* p){
	PBucket *b;

	if(!this || !p) return 0;
	if( cache && !strcmp(p, cache->ext) ){
		return cache;
	}else{
		b = bkt[ hash(p) ];
		b = b ? b->find(p) : 0;
		if(b) cache = b;
		return b;
	}
}

int Ptable::exist(const char*n){

	return find(n) ? 1 :0;
}

PBucket* PBucket::old(void) const{

	if(!ext) return (PBucket*)this;	// need cast away const for new compiler
	if(next) return next->old();
	return 0;
}

	
void Ptable::add(const char* ext, const char* sn, const char* ln){
	int h = hash(ext);
	PBucket *s, *nx = bkt[h];

	if(exist(ext)){
		// redefinition
	}

	if( nx && (s=nx->old()) )
		;
	else
		s = new PBucket;
	s->ext = new char [strlen(ext) +1];
	strcpy(s->ext, ext);
	if(sn){
		s->sp  = new char [strlen(sn) +1];
		strcpy(s->sp, sn);
	}
	if(ln){
		s->lp = new char[ strlen(ln) +1 ];
		strcpy(s->lp, ln);
	}
	s->next = nx;

	cache = s;
	bkt[h] = s;
}

void Ptable::remove(const char* ext){
	PBucket *s = find(ext);

	if(s){
		delete [] s->ext;
		delete [] s->sp;
		delete [] s->lp;
		s->ext = 0;
	}
}

const char* Ptable::slookup(const char* ext){
	PBucket* s = find(ext);

	if(s) return s->sp;
	return 0;
}

const char* Ptable::llookup(const char* ext){
	PBucket* s = find(ext);

	if(s) return s->lp;
	return 0;
}

void Ptable::init(void){
	int i;
	bkt = new PBucket* [ tsize ];
	for(i=0;i<tsize;i++){
		bkt[i] = 0;
	}
	cache = 0;
}

Ptable::~Ptable(){
	int i;
	PBucket* s;
	
	for(i=0;i<TABLESIZE;i++){
		s = bkt[i];
		while(s){
			delete [] s->ext;
			delete [] s->sp;
			delete [] s->lp;
			s = s->next;
		}
	}
}

Ptable::Ptable(int size)
: tsize(size){
	init();
}

Ptable::Ptable(const char *(*a)[3], int size)
: tsize(size){
	
	init();
	addlist(a);
}

void Ptable::addlist(const char *(*a)[3]){
	int i;
	
	for(i=0;a[i][0] && *a[i][0];i++){
		add(a[i][0], a[i][1], a[i][2]);
	}
}

Ptable::Ptable(const char*file,  int size)
: tsize(size){
	
	init();
	addfile(file);
}

void Ptable::addfile(const char*file){
	ifstream is( fullname(file));

	while(is){
		char e[1024], s[1024], l[1024];
		is >> ws;
		if(is.peek()== '#'){
			// allow comments in file
			is.ignore(1024, '\n');
			continue;
		}
		is.get(e, 1023, ',');
		is.get();
		is >> ws;
		is.get(s, 1023, ',');
		is.get();
		is >> ws;
		is.get(l, 1023, '\n');
		is.get();
		
		if(*e) add(e, s, l);
	}
}


	
	
	
	
