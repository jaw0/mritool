
 
// Copyright (c) 1993 Jeff Weisberg
// University of Rochester
//      Dept. of Electrical Engineering
//      Dept. of Neurology, C.E.P.
//
// see the file "License"
 
#ifdef RCSID
static const char *const rcsid
= "@(#)$Id: cl_patient.cc,v 1.1 1995/11/23 04:30:13 jaw Exp jaw $";
#endif

// this file is a source of great ugliness....

// Optimization: -O2

#include <jlisp.h>
#include <jl_proto.h>
#include <defproto.h>
#include <mritool.h>
#include <iostream.h>
#include <grp.h>

extern Obj buttons_sym;

// some constructors

Base::Base(){	
	dirname[0] = 0;
	num = 0;
	selected = 0;
	flags = 0;
	flag = 0;
}

Image::Image()
:fimg(256, 256, 0),
 undo(256, 256, 0),
 segm(256, 256, 0),
 view(512, 512, 0),
 drawing (512, 512, 0),
 drawing2(512, 512, 0) {

	 Obj foo;
	 
	 if(! buttons_sym){
		buttons_sym = maksym("mri:buttons");
		internal_gc_protect = Fcons( buttons_sym, internal_gc_protect);
	 }
	 foo = getvalue(buttons_sym);
	 if( NCONSP(foo))
		 return;
	 int nbuttons = CINT( Flength(foo));	
	 
	 next = 0;
	 mine = 0;
	 window = 128;
	 level = 128;
	 nlevs = 0;
	 dg_data = 0;

	 num_of_snakes = nbuttons;
	 snake = new Snake [num_of_snakes];

 }

Stats::Stats()
: com(3), ev(3), I(3,3), eV(3,3) {

	com = ev = 0;
	I = eV = 0;
	vol = sa = sf = sp = 0;
}
ostream& operator<<(ostream& os, Stats& st){

	os << "COM	: " << st.com << endl;
	os << "evs	: " << st.ev  << endl;
	os << "   EVs	: " << endl;
	os << st.eV  << endl;
	os << "Vol	: " << st.vol << endl;
	os << "    Surface" << endl;
	os << "Faces	: " << st.sf  << endl;
	os << "Pixels	: " << st.sp  << endl;
	os << "Area	: " << st.sa  << endl;

	return os;
	
}
Series::Series(){
	int i;

	Obj foo;
	
	if(! buttons_sym){
		buttons_sym = maksym("mri:buttons");
		internal_gc_protect = Fcons( buttons_sym, internal_gc_protect);
	 }
	foo = getvalue(buttons_sym);
	if( NCONSP(foo))
		return;
	int nbuttons = CINT( Flength(foo));	

	num_of_images = 0;
	next = 0;
	mine = 0;
	images = 0;
	roi = new Series::Roi [nbuttons];
	stats = new Stats [nbuttons];
	for(i=0;i<nbuttons;i++){
		roi[i].present = 0;
	}
	c_roi = 0;
}

Study::Study(){

	num_of_series = 0;
	next = 0;
	mine = 0;
	series = 0;
	
}
Patient::Patient(){

	next = 0;
	studies = 0;
	num_of_studies = 0;
}


// to clear out some memory, should we need it (but keep important data!)

void Patient::cleanup(){
	int i;

	for(i=0;i<num_of_studies;i++){
		studies[i]->cleanup();
	}
}

void Patient::lock(void){
	int i;

	Object::lock();
	
	for(i=0;i<num_of_studies;i++)
		studies[i]->lock();
}

void Patient::unlock(void){
	int i;


	Object::unlock();	
	for(i=0;i<num_of_studies;i++)
		studies[i]->unlock();
}


void Study::cleanup(){
	int i;

	for(i=0;i<num_of_series;i++)
		series[i]->cleanup();
}
void Study::lock(){
	int i;

	Object::lock();
	for(i=0;i<num_of_series;i++)
		series[i]->lock();
}
void Study::unlock(){
	int i;

	Object::unlock();	
	for(i=0;i<num_of_series;i++)
		series[i]->unlock();
}

void Series::cleanup(){
	int i;

	for(i=0;i<num_of_images;i++)
		images[i]->cleanup();
}
void Series::lock(){
	int i;

	Object::lock();
	for(i=0;i<num_of_images;i++)
		images[i]->lock();
}
void Series::unlock(){
	int i;

	Object::unlock();	
	for(i=0;i<num_of_images;i++)
		images[i]->unlock();
}

void Image::cleanup(){
	int i;

	if(dg_data) delete [] dg_data;
	dg_data = 0;
	
	fimg.cleanup();
	segm.cleanup();
	undo.cleanup();
	view.cleanup();
	drawing.cleanup();
	drawing2.cleanup();
	
	for(i=0;i<num_of_snakes;i++)
		snake[i].shed();
}
void Image::lock(void){
	int i;
	
	Object::lock();
	for(i=0;i<num_of_snakes;i++)
		snake[i].lock();
}
void Image::unlock(void){
	int i;
	
	Object::unlock();
	for(i=0;i<num_of_snakes;i++)
		snake[i].unlock();
}

int Image::mkdirhier(void){
	// make required directories for image below current dir
	// ie: make ./XXX/patdir/studydir/seriesdir
	// make them rwxrwsr-x, group=mri
	
	int i;
	struct stat buf;
	char pref[64];
	gid_t grid;
	struct group *grp;

	strcpy(pref, mine->mine->mine->dirname);
	for(i=0;pref[i];i++)if(pref[i]=='/')pref[i]=0;

	grp = getgrnam("mri");
	if( grp )
		grid = grp->gr_gid;
	else
		grid = -1;

	mkdir( pref, 0775);				// prefix dir
	chmod( pref, 02775);
	chown( pref, -1, grid);
	
	mkdir( mine->mine->mine->dirname, 0775);	// patdir
	chmod( mine->mine->mine->dirname, 02775);
	chown( mine->mine->mine->dirname, -1, grid);
	
	mkdir( mine->mine->dirname, 0755);		// study dir
	chmod( mine->mine->dirname, 02775);
	chown( mine->mine->dirname, -1, grid);
	
	mkdir( mine->dirname, 0777);			// series dir
	chmod( mine->dirname, 02775);
	chown( mine->dirname, -1, grid);
	
	i = stat(mine->dirname, &buf);
	return i ;
}


Patient *find(int pid) {
	int i;

	for(i=0;i<G.num_of_patients;i++)
		if(pid == G.patients_a[i]->num) return G.patients_a[i];

	return 0;
}

Study* Patient::find(int n) const {
	int i;

	if( !this) return 0;
	
	for(i=0;i<num_of_studies;i++)
		if(n == studies[i]->num) return studies[i];

	return 0;
}

Series* Study::find(int n) const {
	int i;

	if( !this) return 0;
	
	for(i=0;i<num_of_series;i++)
		if(n == series[i]->num) return series[i];

	return 0;
}

Image *Series::find(int n) const {
	int i;

	if( !this) return 0;
	
	for(i=0;i<num_of_images;i++)
		if(images[i]->num == n) return images[i];

	// bug(__FILE__ ": Series::find, not found");
	return 0;
}

Obj Image::lois(void) const {
	int i;
	Obj v;

	if(! this) return IC_NIL;

	v = makvect(num_of_snakes);
	for(i=0; i<num_of_snakes; i++)
		CVECTOR(v)[i] = MAKINT( snake[i].loi );
	return Fvect_list( v );
}

Obj Patient::list(void) {
	int i;
	Obj v;

	if(! this) return IC_NIL;
	if(! studies)
		learn();
	
	v = makvect( num_of_studies);
	for(i=0;i<num_of_studies;i++)
		CVECTOR(v)[i] = Fcons( MAKINT(studies[i]->num),
			makstr( studies[i]->name));
	return Fvect_list(v);
}

Obj Study::list(void) {
	int i;
	Obj v;

	if(! this) return IC_NIL;
	if(! series)
		learn();
	
	v = makvect( num_of_series);
	for(i=0;i<num_of_series;i++)
		CVECTOR(v)[i] = Fcons( MAKINT(series[i]->num),
			makstr( series[i]->name));
	return Fvect_list(v);
}

Obj Series::list(void) {
	int i;
	Obj v;

	if(! this) return IC_NIL;
	if(! images)
		learn();
	
	v = makvect( num_of_images);
	for(i=0;i<num_of_images;i++)
		CVECTOR(v)[i] = Fcons( MAKINT(images[i]->num),
			makstr( images[i]->name));
	return Fvect_list(v);
}


Obj pat_list(void){
	Obj v = makvect( G.num_of_patients);
	int i;

	for(i=0;i<G.num_of_patients;i++)
		CVECTOR(v)[i] = Fcons( MAKINT( G.patients_a[i]->num),
			makstr( G.patients_a[i]->name));
	return Fvect_list(v);
}
	
Obj Image::info(void) const{
	Obj l = IC_NIL;

	if(!this) return l;
	
	l = Fcons(Fcons(maksym_c("name"),       makstr( (char*)name )),         l);
	l = Fcons(Fcons(maksym_c("thickness"),  makfloat( thickness )),         l);
	l = Fcons(Fcons(maksym_c("x-pix-dim"),  makfloat( x_pix_dim )),     	l);
	l = Fcons(Fcons(maksym_c("y-pix-dim"),  makfloat( y_pix_dim )),  	l);
	l = Fcons(Fcons(maksym_c("pixel-size"), makfloat( pixel_size )), 	l);
	l = Fcons(Fcons(maksym_c("fov"),        makfloat( fov )),        	l);

	return l;
}

Obj Series::info(void) const{
	Obj l = IC_NIL;

	if(!this) return l;
	
	l = Fcons(Fcons(maksym_c("name"),       makstr( (char*)name )),         l);
	l = Fcons(Fcons(maksym_c("date"),       makstr( (char*)date )),         l);
	l = Fcons(Fcons(maksym_c("desrciption"),makstr( (char*)descr )),        l);
	l = Fcons(Fcons(maksym_c("plane"),      makstr( (char*)plane )),        l);
	l = Fcons(Fcons(maksym_c("protocol"),   makstr( (char*)protocol )),     l);
	l = Fcons(Fcons(maksym_c("images"),     MAKINT( num_of_images)), 	l);

	return l;
}

Obj Study::info(void) const{
	Obj l = IC_NIL;

	if(!this) return l;
	
	l = Fcons(Fcons(maksym_c("name"),       makstr( (char*)name )),         l);
	l = Fcons(Fcons(maksym_c("date"),       makstr( (char*)date )),         l);
	l = Fcons(Fcons(maksym_c("description"),makstr( (char*)descr )),        l);
	l = Fcons(Fcons(maksym_c("series"),     MAKINT( num_of_series)), 	l);

	return l;
}

Obj Patient::info(void) const{
	Obj l = IC_NIL;

	if(!this) return l;
	
	l = Fcons(Fcons(maksym_c("name"),       makstr( (char*)name )),         l);
	l = Fcons(Fcons(maksym_c("pid"),        makstr( (char*)pid )),          l);
	l = Fcons(Fcons(maksym_c("sex"),        MAKCHAR( sex )),         	l);
	l = Fcons(Fcons(maksym_c("age"),        MAKINT( age )),          	l);
	l = Fcons(Fcons(maksym_c("weight"),     MAKINT( weight )),       	l);
	l = Fcons(Fcons(maksym_c("hospital"),   makstr( (char*)hospital )),     l);

	return l;
}




	

