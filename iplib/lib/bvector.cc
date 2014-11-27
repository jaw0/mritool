

#include <vector.h>

#define loop(i,j)	for(i=0;i<j;i++)


#if 0
BVector& BVector::operator=(const BVector& f){
	int i;
	
	loop(i,_length)
		bput(i, f.bvalue(i) );

	return *this;  
}

BVector& BVector::operator+=(const BVector& f){
	int i;

	loop(i,_length) bput(i, f.bvalue(i) + bvalue(i) );
	return *this;
}

BVector& BVector::operator-=(const BVector& f){
	int i;

	loop(i,_length) bput(i, bvalue(i) - f.bvalue(i));
	return *this;
}
BVector& BVector::operator*=(const BVector& f){
	int i;

	loop(i,_length) bput(i, bvalue(i) * f.bvalue(i));
	return *this;
}
BVector& BVector::operator/=(const BVector& f){
	int i;

	loop(i,_length) bput(i, bvalue(i) / f.bvalue(i));
	return *this;
}

#endif
