
 
// Copyright (c) 1993 Jeff Weisberg
// University of Rochester
//      Dept. of Electrical Engineering
//      Dept. of Neurology, C.E.P.
//
// see the file "License"
 
#ifdef RCSID
static const char *const rcsid
= "@(#)$Id: patient_db.cc,v 1.1 1995/11/23 04:30:22 jaw Exp jaw $";
#endif

// Optimization: Full

#include <jlisp.h>
#include <jl_proto.h>
#include <defproto.h>
#include <mritool.h>

#include <signa-consts.h>
#include <gemr_imgf.h>

#include <dirent.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fstream.h>

#define direct dirent
#define USE_CACHE	1

static ofstream *ccache;
static int nstu=0, nser=0, nim=0;

int cmp_pat(Patient **a, Patient **b){
	return strcmp( (**a).name, (**b).name);
}

int cmp_img(Image **a, Image **b){
	return strcmp( (**a).name, (**b).name);
}

void create_patient_db(void){
	// create the patient database
	DIR *dot, *top;
	Patient *newone, *prev=0;
	struct direct *dotent, *topent;
	struct stat dirst, cachst;
	int total;
	char rawdir[1024];

	strcpy(rawdir, mriname("=R"));

	fprintf(stderr,"\nCreating patient database  ");
	get_what_patients();
	spin();
	G.patients_l = NULL;
	total = 0;
	if ( chdir(rawdir)  == -1){
		cerr << "Could not change dir to: " << rawdir << endl;
		cerr << "Check the setting of mri:patdir" << endl;
		exit(-1);
	}

	if( USE_CACHE && !stat(".", &dirst) && !stat(".cache-pt", &cachst)
	&& cachst.st_mtime >= dirst.st_mtime){
		// use cache file
		Obj fp, l, pt;

		fp = Fopenread( makstr_c(".cache-pt") );
		l = Fread( fp );
		Fclose( fp );
		while( CONSP( l )){
			pt = CAR(l);
			l = CDR(l);

			newone = new Patient;
			sprintf(newone->dirname, "%s/%s", CCHARS(CAR(pt)), CCHARS(CADR(pt)));
			sprintf(newone->pid, "%s", CCHARS(CADR(pt)));
			if( newone->learn( CADDR(pt) ) && our_patient(newone->name) ){
				total ++;
				newone->flag = 1;
				if(!G.patients_l) G.patients_l = newone;
				else prev->next = newone;
				prev = newone;
			}else{
				delete newone;
			}
			spin();
		}
	}else{
		if ( !(dot=opendir("."))){
			cerr << "Could not open current directory!!!" << endl;
			exit(-1);
		}
		spin();

		while ( (dotent=readdir(dot)) ){ /* for@ toplevel dir */
			spin();
			if ( dotent->d_name[0] =='.' ) continue;
			if (!(top=opendir(dotent->d_name))) continue; 		/* may just be a file */
			while ( (topent=readdir(top))){ 			/* for@ patient in here */
				if ( topent->d_name[0] == '.' ) continue;
				if (!isdigit(topent->d_name[0])) continue; 	/* make sure PID is numeric */
				newone = new Patient;
			
				sprintf(newone->dirname, "%s/%s", dotent->d_name, topent->d_name);
				sprintf(newone->pid, "%s", topent->d_name);
				if( newone->learn() && our_patient(newone->name) ){
					total ++;
					newone->flag = 1;
					if(!G.patients_l) G.patients_l = newone;
					else prev->next = newone;
					prev = newone;
				}else{
					delete newone;
				
				}
				spin();
				chdir(rawdir);	// ?
			}
			closedir(top);
		}
		closedir(dot);
	}
	/* we now have ll of patient IDs and their assoc dir. */

	Patient *curr_p = G.patients_l;
	// cache them up

	G.patients_a = new Patient* [total+1];

	curr_p = G.patients_l;
	int i = 0;
	while (curr_p){
		if (curr_p->flag) G.patients_a[i++] = curr_p;
		curr_p = curr_p->next;
		spin();
	}

	G.num_of_patients = total;		// *THIS MANY* !!!

	// sort it
	// check out this cast!---------------------------v
	qsort(G.patients_a, total, sizeof( Patient* ), (int(*)(const void*, const void*))cmp_pat);
	
	chdir(rawdir);
	fprintf(stderr,"\b Done.\n");

	cerr << "Found " << total << " patients" << endl;
	cerr << nstu << " studies" << endl;
	cerr << nser << " series" << endl;
	cerr << nim  << " images" << endl;
		

}	// all done here!


int Patient::learn(Obj cachdt){
	// gather data on this patient

	DIR	*patdir, *studydir;
	Study	*curr_s, *head_s = 0, *prev_s;
	struct direct *ent;
	struct stat dirst, cachst;
	char rawdir[1024];

	if( rdb_learn())
		return 1;
	
	strcpy(rawdir, mriname("=R"));
	chdir(rawdir);
	if (chdir(dirname)==-1) return 0;
	
	if( USE_CACHE && NCONSP(cachdt) && !stat(".", &dirst) && !stat(".cache-ex", &cachst)
	    && cachst.st_mtime >= dirst.st_mtime){	
		Obj fp;
		
		fp = Fopenread( makstr_c(".cache-ex") );
		cachdt = Fread( fp );
		Fclose( fp );
	}

	if( USE_CACHE && cachdt && CONSP(cachdt) ){
		Obj st;

		while( CONSP( cachdt )){
			st = CAR( cachdt );
			cachdt = CDR(cachdt);
			curr_s = new Study;
		
			curr_s->setm(this);
			strcpy(curr_s->name, CCHARS(CAR(st)));
			sprintf(curr_s->dirname, "%s/%s", dirname, CCHARS(CAR(st)));
			if(curr_s->learn( CADR(st) )){			// gather and report on data here
				num_of_studies++;
				flag = 1;			// indicate valid data for this pat.
				if(!head_s){
					head_s=curr_s;
					prev_s=curr_s;
				}else{
					prev_s->next = curr_s;
					prev_s = curr_s;
				}
			}else{
				delete curr_s;
			}
		}
	}else{
	
		chdir(rawdir);
		patdir = opendir(dirname);
		if (chdir(dirname)==-1) return 0;

		while ( (patdir)&&(ent=readdir(patdir)) ){	// for@ study
			if ( ent->d_name[0] == '.') continue;
			if ( !(studydir=opendir(ent->d_name)) ) continue; // not a dir
			if(chdir(ent->d_name)==-1) continue;	// not a dir?
			curr_s = new Study;
		
			curr_s->setm(this);
			strncpy(curr_s->name, ent->d_name,31);
			sprintf(curr_s->dirname, "%s/%s", dirname, ent->d_name);
			if(curr_s->learn()){			// gather and report on data here
				num_of_studies++;
				flag = 1;			// indicate valid data for this pat.
				if(!head_s){
					head_s=curr_s;
					prev_s=curr_s;
				}else{
					prev_s->next = curr_s;
					prev_s = curr_s;
				}
			}else{
				delete curr_s;
			}
			closedir(studydir);
			chdir(rawdir);			// dir may get currupted down below
			chdir(dirname);				// and I don't know why yet
		}

		closedir(patdir);
		chdir("..");
	}
	
	if(flag){
		// build study cache and attach it
		studies = new Study* [num_of_studies];
		
		curr_s = head_s;
		int foo=0;
		while (curr_s){
			studies[foo++] = curr_s;
			curr_s = curr_s->next;
		}
		/* and sort it */

		nstu += num_of_studies;
	}

	return flag;
}



int Study::learn(Obj cachdt){
	// gather data on this study

	DIR	*studir, *serdir;
	Series	*curr_s, *head_s = 0, *prev_s;
	struct direct *ent;
	struct stat dirst, cachst;
	char rawdir[1024];

	if( rdb_learn())
		return 1;
	
	strcpy(rawdir, mriname("=R"));
	chdir(rawdir);
	if (chdir(dirname)==-1) return 0;
	
	if( USE_CACHE && NCONSP(cachdt) && !stat(".", &dirst) && !stat(".cache-se", &cachst)
	    && cachst.st_mtime >= dirst.st_mtime){
		Obj fp;
		
		fp = Fopenread( makstr_c(".cache-se") );
		cachdt = Fread( fp );
		Fclose( fp );
	}

	if( USE_CACHE && cachdt && CONSP(cachdt) ){
		Obj se;

		while( CONSP( cachdt )){
			se = CAR( cachdt );
			cachdt = CDR(cachdt);
			
			curr_s = new Series;
			curr_s->setm(this);
			strcpy(curr_s->name, CCHARS(CAR(se)));
			sprintf(curr_s->dirname, "%s/%s", dirname, CCHARS(CAR(se)));
			if(curr_s->learn( CADR(se) )){			// gather and report on data here
				num_of_series++;
				flag = 1;			// indicate valid data for this study
				if(!head_s){
					head_s=curr_s;
					prev_s=curr_s;
				}else{
					prev_s->next = curr_s;
					prev_s = curr_s;
				}
			}else{
				delete curr_s;
			}
		}
	}else{
		
		chdir(rawdir);
		studir = opendir(dirname);
		if (chdir(dirname)==-1) return 0;	// not a dir

		while ( (studir)&&(ent=readdir(studir)) ){	// for@ sseries
			if ( ent->d_name[0] == '.') continue;
			if ( !(serdir=opendir(ent->d_name)) ) continue;
			chdir(ent->d_name);
			curr_s = new Series;
			curr_s->setm(this);
			strncpy(curr_s->name, ent->d_name, 31);
			sprintf(curr_s->dirname, "%s/%s", dirname, ent->d_name);
			if(curr_s->learn()){			// gather and report on data here
				num_of_series++;
				flag = 1;			// indicate valid data for this study
				if(!head_s){
					head_s=curr_s;
					prev_s=curr_s;
				}else{
					prev_s->next = curr_s;
					prev_s = curr_s;
				}
			}else{
				delete curr_s;
			}
			closedir(serdir);
			chdir(rawdir);			// dir may get currupted down below
			chdir(dirname);				// and I don't know why yet

		}

		closedir(studir);
		chdir("..");
	}
	
	if(flag){
		// build series cache and attach it
		series = new Series* [num_of_series];
		
		curr_s = head_s;
		int foo=0;
		while (curr_s){
			series[foo++] = curr_s;
			curr_s = curr_s->next;
		}
		/* and sort it */

		nser += num_of_series;
	}

	return flag;
}


int Series::learn(Obj cachdt){
	// gather data on this series

	DIR	*serdir;
	Image	*curr_i, *head_i = 0, *prev_i;
	struct direct *ent;
	struct stat dirst, cachst;
	char rawdir[1024];

	if( rdb_learn())
		return 1;
	
	strcpy(rawdir, mriname("=R"));
	chdir(rawdir);
	if (chdir(dirname)==-1) return 0;
	
	if( USE_CACHE && NCONSP(cachdt) && !stat(".", &dirst) && !stat(".cache-im", &cachst)
	    && cachst.st_mtime >= dirst.st_mtime){
		Obj fp;
		
		fp = Fopenread( makstr_c(".cache-im") );
		cachdt = Fread( fp );
		Fclose( fp );
	}

	if( USE_CACHE && cachdt && CONSP(cachdt) ){
		Obj im;

		while( CONSP( cachdt )){
			im = CAR( cachdt );
			cachdt = CDR(cachdt);

			curr_i = new Image;
			curr_i->setm(this);
			if( STRINGP( CAR(im))){
				strcpy(curr_i->name, CCHARS( CAR(im)));
				sprintf(curr_i->dirname, "%s/%s", dirname, CCHARS( CAR(im)));
				sscanf(CCHARS( CAR(im)), "I.%d", &(curr_i->num) );
			}else{
				cerr << "cache corrupt" << endl;
			}
			if(curr_i->learn( CDR(im))){			// gather and report on data here
				num_of_images++;
				flag = 1;			// indicate valid data for this series
				if(!head_i){
					head_i=curr_i;
					prev_i=curr_i;
				}else{
					prev_i->next = curr_i;
					prev_i = curr_i;
				}
			}else{
				delete curr_i;
			}
		}
	}else{
		
		chdir(rawdir);
		serdir = opendir(dirname);
		if (chdir(dirname)==-1) return 0;	// not a dir

		ofstream of(".cache-im");
		ccache = &of;
		*ccache << "\t\t(";
		while ( (serdir)&&(ent=readdir(serdir)) ){	// for@ image
			if ( ent->d_name[0] != 'I') continue;
			// assume images are named in the form: I.nnn[.Z | .gz]
			if ( (!isdigit(ent->d_name[2]))||(!isdigit(ent->d_name[3]))) continue; /*well, huh? is it?*/
			curr_i = new Image;
		
			curr_i->setm(this);
			strncpy(curr_i->name,ent->d_name, 31);
			sprintf(curr_i->dirname, "%s/%s", dirname, ent->d_name);
			sscanf(ent->d_name, "I.%d", &(curr_i->num) );

			*ccache << "\n\t\t\t(\"" << curr_i->name << "\"";
			
			if(curr_i->learn()){			// gather and report on data here
				num_of_images++;
				flag = 1;			// indicate valid data for this series
				if(!head_i){
					head_i=curr_i;
					prev_i=curr_i;
				}else{
					prev_i->next = curr_i;
					prev_i = curr_i;
				}
			}else{
				delete curr_i;
			}
			*ccache << ")";
		}

		*ccache << ")" << endl;
		closedir(serdir);
		chdir("..");
	}
	if(flag){
		// build image cache and attach it
		images = new Image* [num_of_images];
		
		curr_i = head_i;
		int foo=0;
		while (curr_i){
			images[foo++] = curr_i;	
			curr_i = curr_i->next;		

		}
		qsort(images, num_of_images, sizeof( Image* ), (int(*)(const void*, const void*))cmp_img);

		nim += num_of_images;
	}

	return flag;
}


int Image::learn(Obj cachdt){
	// slurp data from image

	FILE *fp;
	Series *c_ser = mine;
	Study *c_stu = c_ser->mine;
	Patient *c_pat = c_stu->mine;
	struct GEMR_Header hdr;
	int pipep=0, c;

	if( rdb_learn())
		return 1;
	
	if(!c_ser->flag){		// is series data filled in yet?

		if( USE_CACHE && cachdt && CONSP(cachdt)){

			if( STRINGP( Fnth(cachdt, MAKINT(0))))
				strncpy(c_ser->descr, CCHARS(Fnth(cachdt, MAKINT(0))), 59);
			if( STRINGP( Fnth(cachdt, MAKINT(1))))
				strncpy(c_ser->date,  CCHARS(Fnth(cachdt, MAKINT(1))), 15);
			if( STRINGP( Fnth(cachdt, MAKINT(2))))
				strncpy(c_ser->protocol, CCHARS(Fnth(cachdt, MAKINT(2))), 31);
			c_ser->num = atoi( c_ser->name );

			if(!c_stu->flag){
				if( STRINGP( Fnth(cachdt, MAKINT(3))))
					strncpy(c_stu->date,  CCHARS(Fnth(cachdt, MAKINT(3))), 15);
				if( STRINGP( Fnth(cachdt, MAKINT(4))))
					strncpy(c_stu->descr, CCHARS(Fnth(cachdt, MAKINT(4))), 59);
				c_stu->num = atoi( c_stu->name );
			
				if(!c_pat->flag){
					if( STRINGP( Fnth(cachdt, MAKINT(5))))
						strncpy(c_pat->name,     CCHARS(Fnth(cachdt, MAKINT(5))), 31);
					if( STRINGP( Fnth(cachdt, MAKINT(6))))
						strncpy(c_pat->hospital, CCHARS(Fnth(cachdt, MAKINT(6))), 31);
					c_pat->num = atoi(c_pat->pid);
					if( INUMP( Fnth(cachdt, MAKINT(7))))
						c_pat->sex = CINT(Fnth(cachdt, MAKINT(7)));
					if( INUMP( Fnth(cachdt, MAKINT(8))))
						c_pat->age = CINT(Fnth(cachdt, MAKINT(8)));
					if( INUMP( Fnth(cachdt, MAKINT(9))))
						c_pat->weight = CINT(Fnth(cachdt, MAKINT(9)));
				}
			}
		}else{
			char *cmd;
		
			c = name[strlen(name)-1];
			switch(c){
			case 'Z':
				cmd = "uncompress -c";
				pipep = 1;
				break;
			case 'z':
				cmd = "gunzip -c";
				pipep = 1;
				break;
			}

			if(pipep){
				char buf[8192];
				sprintf(buf, "%s %s", cmd, name);
				fp = popen(buf, "r");
			}else{
				fp = fopen(name, "r");
			}
			if( !fp ){
				// error
				return 0;
			}

			hdr.read_header(fp);
			/* we got one, milk as much data as we can from it*/

			strncpy(c_ser->descr, hdr.get_str_se_desc(), 59);
			c_ser->descr[59] = 0;
			strncpy(c_ser->date,  hdr.get_str_se_date(), 15);
			c_ser->date[15] = 0;
			strncpy(c_ser->protocol, hdr.get_str_se_protocol(), 31);
			c_ser->protocol[31] = 0;
			c_ser->num = atoi( c_ser->name );
			// sscanf (c_ser->name,  "%d",&(c_ser->num) );

			if(!c_stu->flag){
				strncpy(c_stu->date,  hdr.get_str_st_date(), 15);
				c_stu->date[15] = 0;
				strncpy(c_stu->descr, hdr.get_str_st_desc(), 59);
				c_stu->descr[59] = 0;
				// sscanf (c_stu->name,  "%d", &(c_stu->num));
				c_stu->num = atoi( c_stu->name );
			
				if(!c_pat->flag){
					strncpy(c_pat->name,     hdr.get_str_pt_name(), 31);
					c_pat->name[31] = 0;
					strncpy(c_pat->hospital, hdr.get_str_hosp(), 31);
					c_pat->hospital[31] = 0;
					c_pat->num = atoi(c_pat->pid);
					c_pat->sex = hdr.get_int_pt_sex();
					c_pat->age = hdr.get_int_pt_age();
					c_pat->weight = hdr.get_int_pt_weight();
				}
			}
			if(pipep)
				pclose(fp);
			else
				fclose(fp);

			*ccache << "" << endl;
			*ccache << "\t\t\t\"" << c_ser->descr    << "\" \"" << c_ser->date  << '"' << endl;
			*ccache << "\t\t\t\"" << c_ser->protocol << '"' << endl;
			*ccache << "\t\t\t\"" << c_stu->date     << "\" \"" << c_stu->descr << '"' << endl;
			*ccache << "\t\t\t\"" << c_pat->name     << '"' << endl;
			*ccache << "\t\t\t\"" << c_pat->hospital << '"' << endl;
			*ccache << "\t\t\t" << (int)c_pat->sex    << " " << c_pat->age  << " " << c_pat->weight;
			
		}
	}
	return 1;	// we know that this data is good
}








