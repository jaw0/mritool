
#ifndef _ptable_h
#define _ptable_h

class PBucket;
class Ptable;

class PBucket {
    friend class Ptable;
  private:
	PBucket *next;

	PBucket* find(const char*) const;
	PBucket* old(void) const;
	
  public:
	char *ext;
	char *sp, *lp;

	PBucket() {next=0;ext=sp=lp=0;}
	~PBucket();
};

#define TABLESIZE 237
class Ptable {
  private:
	PBucket** bkt;
	PBucket* cache;
	
	int hash(const char*)const;
	PBucket* find(const char*);

	void init(void);
  public:
	const int tsize;
	
	Ptable(int s=TABLESIZE);
	Ptable(const char*(*)[3], int s=TABLESIZE);
	Ptable(const char*file, int s=TABLESIZE);
	~Ptable();

	void add(const char*, const char*, const char* l=0);	// add an entry
	void addfile(const char*);				// add from a file
	void addlist(const char*(*)[3]);	      		// add from a list
	void remove(const char*);				// remove an entry	
	const char* slookup(const char*);			// lookup an extension
	const char* lookup (const char*n) {return slookup(n);}
	const char* llookup(const char*);
	int exist(const char*);

};



	
#endif /* !_ptable_h */

