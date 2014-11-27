
#include <iostream.h>

void bug(const char* msg){
	cerr << "Internal Error: " << msg << endl;
}

void bug(const char* msga, const char* msgb){
	cerr << "Internal Error: " << msga << " " << msgb << endl;
}
