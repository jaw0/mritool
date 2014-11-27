
// Copyright (c) 1993 Jeff Weisberg
// University of Rochester
//      Dept. of Electrical Engineering
//      Dept. of Neurology, C.E.P.
//
// see the file "License"

#ifdef RCSID
static const char *const rcsid
= "@(#)$Id: rdb_client.cc,v 1.1 1995/11/23 04:30:27 jaw Exp jaw $";
#endif

// Optimization: Full


// remote database client code

#include <jlisp.h>
#include <jl_proto.h>
#include <defproto.h>
#include <mritool.h>
#include <rdb_client.h>
#include <fstream.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netdb.h>

extern "C" Obj fill_in_sain(struct sockaddr_in *sain, Obj adr, Obj prt, char *name);

void rdb_auth(void);

static FILE *rdbs_in=0, *rdbs_out=0;

#define CODE_SECTION	SECTION_INIT

static void rdb_gets(char *f, char *buf, int si){
	int i;
	
	fgets(buf, si, rdbs_in);
	debugp( DBG_VERBOSE )
		cerr << ">" << buf << endl;

	for(i=0; buf[i]!='\n'; i++)
		f[i] = buf[i];
	f[i] = 0;
}

static int rdb_atoi(char *buf, int si){
	fgets(buf, si, rdbs_in);
	debugp( DBG_VERBOSE )
		cerr << ">" << buf << endl;

	return atoi( buf );
}

int rdb_initialize(void){
	Obj server_host, server_port;
	struct servent *sp;
	struct sockaddr_in sain;
	char buf[8192];
	int fd;

	// to where ought we connect
	
	server_host = getvalue(maksym_c("mri:rdb:host"));
	if(! STRINGP(server_host))
		server_host = makstr_c("localhost");

	server_port = getvalue(maksym_c("mri:rdb:port"));
	if(! DEFINEDP(server_port))
		server_port = MAKINT(MRI_RDB_PORT);
	
	fill_in_sain(&sain, server_host, server_port, "rdb_initialize");

	// attempt to connect to server
	
	fd = socket(AF_INET, SOCK_STREAM, 0);
	if(! fd){
		cerr << "socket failed" << endl;
		return 0;
	}
	if( connect(fd, (sockaddr*)&sain, sizeof(sain)) == -1){
		cerr << "connect failed" << endl;
		return 0;
	}
	rdbs_in  = fdopen(fd, "r");
	rdbs_out = fdopen(fd, "w");

	/*
	  participate in intial handshake:
	  server		client

	  HELO ...
	                        HELO ...
	  OK
	*/
	  
	fgets(buf, sizeof(buf), rdbs_in);
	debugp( DBG_VERBOSE )
		cerr << ">" << buf << endl;

	if( strncmp(buf, "HELO", 4)){
		cerr << "initial handshake failed" << endl;
		return 0;
	}
	
	fprintf(rdbs_out, "HELO \n");	
	fflush( rdbs_out);
	debugp( DBG_VERBOSE )
		cerr << "<" <<  "HELO " << endl;
	
	fgets(buf, sizeof(buf), rdbs_in);
	debugp( DBG_VERBOSE )
		cerr << ">" << buf << endl;

	if( strncmp(buf, "OK", 2)){
		cerr << "initial handshake failed" << endl;
		return 0;
	}

	// attempt to authenticate connection
	rdb_auth();

	return 1;
}
	

int rdb_create_patient_db(void){
	char buf[8192];
	Patient *newone, *prev=0, *pt;
	int total = 0;
	int i,n;
	
	fprintf(stderr,"\nConnecting to patient database server  ");

	if(! rdb_initialize() )
		return 0;

	fprintf(rdbs_out, "LIST\n");
	fflush(rdbs_out);
	debugp( DBG_VERBOSE )
		cerr << "<" << "LIST" << endl;

	fgets(buf, sizeof(buf), rdbs_in);
	debugp( DBG_VERBOSE )
		cerr << ">" << buf << endl;

	if( strncmp(buf, "OK", 2))
		return 0;


	// process list
	while(1){
		
		fgets(buf, sizeof(buf), rdbs_in);
		debugp( DBG_VERBOSE )
			cerr << ">" << buf << endl;

		if(! strncmp(buf, ")", 1))
			break;

		newone = new Patient;

		for(i=0; buf[i]!='\n'; i++)
			newone->pid[i] = buf[i];
		newone->pid[i] = 0;
		newone->num = atoi( newone->pid );
		total ++;
		if(!G.patients_l) G.patients_l = newone;
		else prev->next = newone;
		prev = newone;

	}

	// get patient info
	for(pt=G.patients_l; pt; pt=pt->next){

		fprintf(rdbs_out, "INFO %s\n", pt->pid);
		fflush(rdbs_out);
		debugp( DBG_VERBOSE )
			cerr << "<" << "INFO " << pt->pid << endl;
		
		fgets(buf, sizeof(buf), rdbs_in);
		debugp( DBG_VERBOSE )
			cerr << ">" << buf << endl;

		if( strncmp(buf, "OK", 2))
			return 0;

		// dirname
		rdb_gets(pt->dirname, buf, sizeof(buf));
		// name
		rdb_gets(pt->name, buf, sizeof(buf));
		// hospital
		rdb_gets(pt->hospital, buf, sizeof(buf));
		// sex
		pt->sex = rdb_atoi(buf, sizeof(buf));
		// age
		pt->age = rdb_atoi(buf, sizeof(buf));
		// weight
		pt->weight = rdb_atoi(buf, sizeof(buf));
		// num of exams
		pt->num_of_studies = rdb_atoi(buf, sizeof(buf));
		pt->studies = 0;
		
		// other?
		do{
			fgets(buf, sizeof(buf), rdbs_in);
			debugp( DBG_VERBOSE )
				cerr << ">" << buf << endl;
		}while( strncmp(buf, ")", 1));

		
		if( our_patient( pt->name )){
			pt->flag = 1;
		}else{
			total --;
		}
	}

	Patient *curr_p = G.patients_l;
	// cache them up

	G.patients_a = new Patient* [total+1];

	curr_p = G.patients_l;
	i = 0;
	while (curr_p){
		if (curr_p->flag) G.patients_a[i++] = curr_p;
		curr_p = curr_p->next;
	}

	G.num_of_patients = total;		// *THIS MANY* !!!

	extern int cmp_pat(Patient **a, Patient **b);

	qsort(G.patients_a, total, sizeof( Patient* ), (int(*)(const void*, const void*))cmp_pat);
	
	cerr << "Found " << total << " patients" << endl;
		
	return 1;
}
	
	

int Patient::rdb_learn(void){
	char buf[8192];
	Study *newone, *prev, *st;
	Study *head=0, *curr;
	int i, n;
	int total = 0;
	
	if(! rdbs_in)
		return 0;

	fprintf(rdbs_out, "LIST %s\n", pid);
	fflush(rdbs_out);
	debugp( DBG_VERBOSE )
		fprintf(stderr, "<LIST %s\n", pid);

	fgets(buf, sizeof(buf), rdbs_in);
	debugp( DBG_VERBOSE )
		cerr << ">" << buf << endl;

	if( strncmp(buf, "OK", 2))
		return 0;

	while(1){
		fgets(buf, sizeof(buf), rdbs_in);
		debugp( DBG_VERBOSE )
			cerr << ">" << buf << endl;

		if(! strncmp(buf, ")", 1))
			break;

		newone = new Study;

		for(i=0; buf[i]!='\n'; i++)
			newone->name[i] = buf[i];
		newone->name[i] = 0;
		newone->setm(this);
		newone->num = atoi( newone->name );
		total ++;
		if(!head) head = newone;
		else prev->next = newone;
		prev = newone;
	}

	studies = new Study* [ total + 1 ];
	num_of_studies = total;
	n = 0;
	for(st=head; st; st=st->next){

		studies[n++] = st;

		fprintf(rdbs_out, "INFO %s %s\n", pid, st->name);
		fflush(rdbs_out);
		debugp( DBG_VERBOSE )
			fprintf(stderr, "<INFO %s %s\n", pid, st->name);
		
		fgets(buf, sizeof(buf), rdbs_in);
		debugp( DBG_VERBOSE )
			cerr << ">" << buf << endl;

		if( strncmp(buf, "OK", 2))
			return 0;


		// dirname
		rdb_gets(st->dirname, buf, sizeof(buf));
		// date
		rdb_gets(st->date, buf, sizeof(buf));
		// description
		rdb_gets(st->descr, buf, sizeof(buf));
		// num of series
		st->num_of_series = rdb_atoi(buf, sizeof(buf));
		st->series = 0;
		st->flag = 1;
		
		// other?
		do{
			fgets(buf, sizeof(buf), rdbs_in);
			debugp( DBG_VERBOSE )
				cerr << ">" << buf << endl;
		}while( strncmp(buf, ")", 1));
		
	}
	// cache them up

	studies = new Study* [total+1];
	st = head;
	i = 0;
	while (st){
		studies[i++] = st;
		st = st->next;
	}

	return 1;
}

int Study::rdb_learn(void){
	char buf[8192];
	Series *newone, *prev, *se;
	Series *head=0, *curr;
	int i, n;
	int total = 0;
	
	if(! rdbs_in)
		return 0;

	fprintf(rdbs_out, "LIST %s %s\n", mine->pid, name);
	fflush(rdbs_out);
	debugp( DBG_VERBOSE )
		fprintf(stderr, "<LIST %s %s\n", mine->pid, name);

	fgets(buf, sizeof(buf), rdbs_in);
	debugp( DBG_VERBOSE )
		cerr << ">" << buf << endl;

	if( strncmp(buf, "OK", 2))
		return 0;

	while(1){
		fgets(buf, sizeof(buf), rdbs_in);
		debugp( DBG_VERBOSE )
			cerr << ">" << buf << endl;

		if(! strncmp(buf, ")", 1))
			break;

		newone = new Series;

		for(i=0; buf[i]!='\n'; i++)
			newone->name[i] = buf[i];
		newone->name[i] = 0;
		newone->setm(this);
		newone->num = atoi( newone->name );
		total ++;
		if(!head) head = newone;
		else prev->next = newone;
		prev = newone;
	}

	series = new Series* [ total + 1 ];
	num_of_series = total;
	n = 0;

	for(se=head; se; se=se->next){

		series[n++] = se;

		fprintf(rdbs_out, "INFO %s %s %s\n", mine->pid, name, se->name);
		fflush(rdbs_out);
		debugp( DBG_VERBOSE )
			fprintf(stderr, "<INFO %s %s %s\n", mine->pid, name, se->name);
		
		fgets(buf, sizeof(buf), rdbs_in);
		debugp( DBG_VERBOSE )
			cerr << ">" << buf << endl;

		if( strncmp(buf, "OK", 2))
			return 0;

		// dirname
		rdb_gets(se->dirname, buf, sizeof(buf));
		// date
		rdb_gets(se->date, buf, sizeof(buf));
		// description
		rdb_gets(se->descr, buf, sizeof(buf));
		// protocol
		rdb_gets(se->protocol, buf, sizeof(buf));
		// num of series
		se->num_of_images = rdb_atoi(buf, sizeof(buf));
		se->images = 0;
		se->flag = 1;
		
		// other?
		do{
			fgets(buf, sizeof(buf), rdbs_in);
			debugp( DBG_VERBOSE )
				cerr << ">" << buf << endl;
		}while( strncmp(buf, ")", 1));
		
	}
	// cache them up

	series = new Series* [total+1];
	se = head;
	i = 0;
	while (se){
		series[i++] = se;
		se = se->next;
	}

	return 1;
}

int Series::rdb_learn(void){
	char buf[8192];
	Image  *newone, *prev, *im;
	Image  *head=0, *curr;
	int i, n;
	int total = 0;
	
	if(! rdbs_in)
		return 0;

	fprintf(rdbs_out, "LIST %s %s %s\n", mine->mine->pid, mine->name, name);
	fflush(rdbs_out);
	debugp( DBG_VERBOSE )
		fprintf(stderr, "<LIST %s %s %s\n", mine->mine->pid, mine->name, name);

	fgets(buf, sizeof(buf), rdbs_in);
	debugp( DBG_VERBOSE )
		cerr << ">" << buf << endl;

	if( strncmp(buf, "OK", 2))
		return 0;

	while(1){
		fgets(buf, sizeof(buf), rdbs_in);
		debugp( DBG_VERBOSE )
			cerr << ">" << buf << endl;

		if(! strncmp(buf, ")", 1))
			break;

		newone = new Image;

		for(i=0; buf[i]!='\n'; i++)
			newone->name[i] = buf[i];
		newone->name[i] = 0;
		newone->setm(this);
		sscanf(newone->name, "I.%d", &(newone->num));
		total ++;
		if(!head) head = newone;
		else prev->next = newone;
		prev = newone;
	}

	images = new Image* [ total + 1 ];
	num_of_images = total;
	n = 0;

	for(im=head; im; im=im->next){

		images[n++] = im;

		fprintf(rdbs_out, "INFO %s %s %s %d\n", mine->mine->pid, mine->name, name, im->num);
		fflush(rdbs_out);
		debugp( DBG_VERBOSE )
			fprintf(stderr, "<INFO %s %s %s %d\n", mine->mine->pid, mine->name, name, im->num);
		
		fgets(buf, sizeof(buf), rdbs_in);
		debugp( DBG_VERBOSE )
			cerr << ">" << buf << endl;

		if( strncmp(buf, "OK", 2))
			return 0;

		// dirname
		rdb_gets(im->dirname, buf, sizeof(buf));
		im->flag = 1;
		
		// other?
		do{
			fgets(buf, sizeof(buf), rdbs_in);
			debugp( DBG_VERBOSE )
				cerr << ">" << buf << endl;
		}while( strncmp(buf, ")", 1));
		
	}
	// cache them up

	images = new Image* [total+1];
	im = head;
	i = 0;
	while (im){
		images[i++] = im;
		im = im->next;
	}

	return 1;
}

int Image::rdb_learn(void){
	// this exists only for symetry
	return 0;
}


void Image::rdb_doit(void){
	int fd=0, fdd;
	struct sockaddr_in sr, sl;
	int nln = sizeof(sr);
	int loport, remport;
	char buf[8192];
	FILE *fp;

	if( getvalue(maksym_c("mri:rdb:doit-p")) != IC_TRUE)
		goto failed;
	
	// create stream for data connection
	fd = socket(AF_INET, SOCK_STREAM, 0);
	if(! fd){
		cerr << "socket failed" << endl;
		goto failed;
	}

	// get remote info, for later use
	if( getpeername(fileno(rdbs_in), (sockaddr*)&sr, &nln)==-1){
		cerr << "getpeername failed" << endl;
		goto failed;
	}

	// grab first available local port
	sl.sin_addr.s_addr = INADDR_ANY;
	sl.sin_port = 0;	
	if( bind(fd, (sockaddr*)&sl, sizeof(sl))==-1){
		cerr << "bind failed" << endl;
		goto failed;
	}
	nln = sizeof(sl);
	getsockname(fd, (sockaddr*)&sl, &nln);
	loport = ntohs(sl.sin_port);

	listen(fd, 1);

	fprintf(rdbs_out, "DATA %s %s %s %d %d\n",
		mine->mine->mine->pid,
		mine->mine->name,
		mine->name,
		num,
		loport);
	fflush(rdbs_out);
	debugp( DBG_VERBOSE )
		fprintf(stderr, "DATA %s %s %s %d %d\n",
			mine->mine->mine->pid,
			mine->mine->name,
			mine->name,
			num,
			loport);
	
	fgets(buf, sizeof(buf), rdbs_in);
	debugp( DBG_VERBOSE )
		cerr << ">" << buf << endl;

	if( strncmp(buf, "OK", 2)){
		goto failed;
	}

	remport = atoi( buf + 3 );

	fdd = 0;
	// loop, checking connection is from proper place
	do {
		if( fdd) close(fdd);
		
		nln = sizeof(sl);
		fdd = accept(fd, (sockaddr*)&sl, &nln);

	}while( remport!=ntohs(sl.sin_port) && sr.sin_addr.s_addr!=sl.sin_addr.s_addr );

	sleep( 3 );
	fp = fdopen(fdd, "r");
	doit( fp );


	return;
	
  failed:

	cerr << "rdb_doit(): get remote data failed, trying local" << endl;
	if(fd)
		close(fd);
	// remove .RDB extension and try local file
	dirname[strlen(dirname) - strlen(".RDB")] = 0;
	doit();

}

struct AuthData {
	AuthData *next;
	char *word;
	int n;
};

static AuthData *auth_data=0;
static int auth_length;

static int auth_init(void){
	FILE *fp;
	char fn[8192];
	char buf[8192];
	int n = 0;
	
	strcpy( fn, mriname("~/.mri-rdb-auth"));
	fp = fopen( fn, "r");

	if(! fp) return 0;
	
	while(! feof(fp)){
		AuthData *newone = new AuthData;
		fgets(buf, sizeof(buf), fp);

		if(buf[strlen(buf)-1]=='\n') buf[strlen(buf)-1] = 0;
		newone->word = strdup( buf );
		newone->n = n++;

		newone->next = auth_data;
		auth_data = newone;
	}

	auth_length = n;
	return 1;
}

void rdb_auth(void){
	char buf[8192];
	char word[8192];
	char nbuf[8192];
	int io, n0, n1, no;
	int i, n;
	AuthData *ad;

	if(! auth_init()){
		cerr << "rdbs: authentication failed" << endl;
		return;
	}
	
	fprintf(rdbs_out, "AUTH\n");
	fflush(rdbs_out);

	fgets(buf, sizeof(buf), rdbs_in);
	debugp( DBG_VERBOSE )
		cerr << ">" << buf << endl;

	for(i=0; buf[i]!=' '; i++)
		word[i] = buf[i];
	word[i] = 0;

	i++;
	for(n=0; buf[i]!=' '; i++,n++)
		nbuf[n] = buf[i];
	nbuf[n] = 0;
	n0 = atoi( nbuf );

	i++;
	for(n=0; buf[i]; i++,n++)
		nbuf[n] = buf[i];
	nbuf[n] = 0;
	n1 = atoi( nbuf );

	// find initial word
	ad = auth_data;
	while( ad && strcmp(word, ad->word))
		ad = ad->next;

	io = ad ? ad->n : 0;
	no = (io * n0 + n1) % auth_length;

	ad = auth_data;

	while(ad && ad->n != no)
		ad = ad->next;

	// send response
	fprintf(rdbs_out, "%s\n", ad ? ad->word : "" );
	fflush(rdbs_out);

	debugp( DBG_VERBOSE )
		fprintf(stderr, "<%s\n", ad ? ad->word : "" );

	fgets(buf, sizeof(buf), rdbs_in);
	debugp( DBG_VERBOSE )
		cerr << ">" << buf << endl;

	if( strncmp(buf, "OK", 2))
		cerr << "rdbs: authentication failed" << endl;

}

void Patient::rdb_mark(void){
	char buf[8192];
	
	fprintf(rdbs_out, "MARK %s\n", pid);
	fflush(rdbs_out);

	debugp( DBG_VERBOSE )
		fprintf(stderr, "<MARK %s\n", pid);

	fgets(buf, sizeof(buf), rdbs_in);
	debugp( DBG_VERBOSE )
		cerr << ">" << buf << endl;

	if( strncmp(buf, "OK", 2))
		cerr << "rdbs: mark failed" << endl;
}

void Study::rdb_mark(void){
	char buf[8192];
	
	fprintf(rdbs_out, "MARK %s %s\n", mine->pid, name);
	fflush(rdbs_out);

	debugp( DBG_VERBOSE )
		fprintf(stderr, "<MARK %s %s\n", mine->pid, name);

	fgets(buf, sizeof(buf), rdbs_in);
	debugp( DBG_VERBOSE )
		cerr << ">" << buf << endl;

	if( strncmp(buf, "OK", 2))
		cerr << "rdbs: mark failed" << endl;
}

void Series::rdb_mark(void){
	char buf[8192];
	
	fprintf(rdbs_out, "MARK %s %s %s\n", mine->mine->pid, mine->name, name);
	fflush(rdbs_out);

	debugp( DBG_VERBOSE )
		fprintf(stderr, "<MARK %s %s %s\n", mine->mine->pid, mine->name, name);

	fgets(buf, sizeof(buf), rdbs_in);
	debugp( DBG_VERBOSE )
		cerr << ">" << buf << endl;

	if( strncmp(buf, "OK", 2))
		cerr << "rdbs: mark failed" << endl;
}

void Image::rdb_mark(void){
	char buf[8192];
	
	fprintf(rdbs_out, "MARK %s %s %s %s\n", mine->mine->mine->pid, mine->mine->name, mine->name, name);
	fflush(rdbs_out);

	debugp( DBG_VERBOSE )
		fprintf(stderr, "<MARK %s %s %s\n", mine->mine->mine->pid, mine->mine->name, mine->name, name);

	fgets(buf, sizeof(buf), rdbs_in);
	debugp( DBG_VERBOSE )
		cerr << ">" << buf << endl;

	if( strncmp(buf, "OK", 2))
		cerr << "rdbs: mark failed" << endl;
}

void rdb_arcv(void){
	char buf[8192];

	fprintf(rdbs_out, "ARCV\n");
	fflush(rdbs_out);

	debugp( DBG_VERBOSE )
		fprintf(stderr, "<ARCV\n");

	fgets(buf, sizeof(buf), rdbs_in);
	debugp( DBG_VERBOSE )
		cerr << ">" << buf << endl;

	if( strncmp(buf, "OK", 2))
		cerr << "rdbs: arcv failed" << endl;
}

void rdb_dele(void){
	char buf[8192];

	fprintf(rdbs_out, "DELE\n");
	fflush(rdbs_out);

	debugp( DBG_VERBOSE )
		fprintf(stderr, "<DELE\n");

	fgets(buf, sizeof(buf), rdbs_in);
	debugp( DBG_VERBOSE )
		cerr << ">" << buf << endl;

	if( strncmp(buf, "OK", 2))
		cerr << "rdbs: dele failed" << endl;
}










