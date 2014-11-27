

// $Id: project.cc,v 1.1 1995/11/23 04:30:23 jaw Exp jaw $

#include <mritool.h>
#include <fullname.h>
#include <jl_proto.h>

class What_Patients {
	friend void get_what_patients();
	friend int our_patient(char *name);
private:
	char *name;
	What_Patients *next;
public:
	What_Patients(){
		name = 0;
		next = 0;
	}
	~What_Patients(){}
};
What_Patients **what_patients;


void get_what_patients(void){
	FILE *fp;
	What_Patients *head=NULL, *curr=0, *newc;
	int count=0, i;
	char line[1024], *ln;
	Obj pf = getvalue(maksym("mri:patients"));
	
	if (! STRINGP(pf)) {
		what_patients = NULL;
		return;
	}
	if ( !(fp=fopen(mriname( CCHARS(pf)),"r"))){
		what_patients = NULL;
		return;
	}
	while ( fgets(line,1024,fp) ){
		line[strlen(line)-1] = '\0'; /* chop newline */
		for(i=0;i<=strlen(line);i++)line[i]=tolower(line[i]); /* lowercase it */
		ln = new char [ strlen(line)+1 ];
		strcpy(ln,line);
		newc = new What_Patients;
		newc->name = ln;
		if (!head)
			head = newc;
		else 
			curr->next = newc;
		curr = newc;
		count ++;
	}
	fclose(fp);
	what_patients = new What_Patients* [ count +1 ];
	for(i=0,curr=head;i<count;i++,curr=curr->next)
		what_patients[i] = curr;
	what_patients[count] = NULL;
//	q_sort(what_patients,0,count-1,strcmp);
}

int our_patient(char *name){
	char lname[256];
	int found, i,j;

	if (!what_patients) return(1);
	if ( !strlen(name)) return(0); /* do they have a name */
	for(j=0;j<=strlen(name);j++)lname[j]=tolower(name[j]); /* lowercase it */

	for(i=0,found=0;((what_patients[i])&&(!found));i++){
		if ( !strcmp(lname,what_patients[i]->name)) found =1;
	}
	return(found);
}

