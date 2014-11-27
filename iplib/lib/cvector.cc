

#ifdef RCSID
static const char *const rcsid
= "@(#)$Id: cvector.cc,v 1.1 1995/11/23 04:30:59 jaw Exp jaw $";
#endif

#include <matrix.h>
#include <math.h>
#include <fstream.h>

#define loop(i,j)	for(i=0;i<j;i++)
#define ABS( x ) 	( ((x)<0) ? -(x) : (x) )


CVector::~CVector(){
	delete [] data;
	_length = 0;
}

CVector::CVector(int l){
	_length = length = l;
	data = new complex [_length];
}

CVector::CVector(const CVector& v){
	_length = 0;
	data = 0;
	length = v.length;
	this->operator=(v);
}
CVector::CVector(int l, const Function& f){
	int i;
	
	_length = length = l;
	data = new complex [_length];

	loop(i, length) data[i] = f[i];
}

complex& CVector::operator[](int i){
	int j;

	if(i<0) i = -i;
	
	if(i>=_length){
		// beyond the edge!
		// make more room 
		complex *ndata = new complex [ i+1 ];
		loop(j, _length) ndata[j] = data[j];
		for(;j<=i;j++)  ndata[j] = 0;		// init to 0
		delete [] data;
		data = ndata;
		_length = i+1;
	}
	return data[i];
}

complex CVector::vvalue(int i) const{
	if(i<0 || i> _length) return 0;
	return data[i];
}


CVector operator+(const CVector& a, const CVector& b){
	int i;
	CVector foo(a.length);
	
	loop(i, a.length) foo.data[i] = a[i] + b[i];

	return foo;
}

CVector operator-(const CVector& a, const CVector& b){
	int i;
	CVector foo(a.length);
	
	loop(i, a.length) foo.data[i] = a[i] - b[i];

	return foo;
}

CVector operator+(const CVector& a, complex b){
	int i;
	CVector foo(a.length);
	
	loop(i, a.length) foo.data[i] = a[i] + b;

	return foo;
}

CVector operator-(const CVector& a, complex b){
	int i;
	CVector foo(a.length);
	
	loop(i, a.length) foo.data[i] = a[i] - b;

	return foo;
}

CVector operator*(const CVector& a, const CVector& b){
	int i;
	CVector foo(a.length);
	
	loop(i, a.length) foo.data[i] = a[i] * b[i];

	return foo;
}

CVector operator/(const CVector& a, const CVector& b){
	int i;
	CVector foo(b.length);

	// use b.length to avoid divide by 0
	loop(i, b.length) foo.data[i] = a[i] / b[i];
	return foo;
}

CVector operator*(const CVector& a, complex b){
	int i;
	CVector foo(a.length);
	
	loop(i, a.length) foo.data[i] = a[i] * b;

	return foo;
}

CVector operator/(const CVector& a, complex b){
	int i;
	CVector foo(a.length);
	
	loop(i, a.length) foo.data[i] = a[i] / b;

	return foo;
}

ostream& operator<<(ostream& os, CVector& v){
	int i;

	loop(i,v.length)
		os << v.data[i] << "\t";
	return os;
}


int CVector::save(char* name) const{
	int i;
	ofstream of(name);
	
	if(!of) return 0;

	of << "# CVector[" << length << "]; File: " << name << endl;
	loop(i, length){
		of << "{" << ::real(operator[](i)) << " , "
		   << ::imag(operator[](i)) << "}" << endl;
	}
	return 1;
}

int CVector::load(char* name){
	int i=0;
	ifstream ifs(name);
	double ar, ai;
	
	if(!ifs) return 0;

	while(ifs){
		if(ifs.peek()=='#'){
			ifs.ignore(1024, '\n');
			continue;
		}
		ifs >> ar >> ai;
		operator[](i++) = complex(ar, ai);
	}
	return 1;
}

Vector CVector::r(void) const {
	int i;
	Vector foo(length);

	loop(i, length){
		foo[i] = ::real(operator[](i));
	}
	return foo;
}
Vector CVector::i(void) const {
	int i;
	Vector foo(length);

	loop(i, length){
		foo[i] = ::imag(operator[](i));
	}
	return foo;
}

CVector CVector::d(int n) const{
	CVector foo(length), bar(length);
	int i;
	
	foo = *this;
	while(n){
		loop(i, length-1)
			bar[i] = foo[i+1] - foo[i];
		bar[length-1] = bar[length];

		foo = bar;
	}
	return foo;
}

CVector& CVector::operator=(complex f){
	int i;

	loop(i,length) operator[](i) = f;
	return *this;
}
CVector& CVector::operator+=(complex f){
	int i;

	loop(i,length) operator[](i) += f;
	return *this;
}
CVector& CVector::operator-=(complex f){
	int i;

	loop(i,length) operator[](i) -= f;
	return *this;
}
CVector& CVector::operator*=(complex f){
	int i;

	loop(i,length) operator[](i) *= f;
	return *this;
}
CVector& CVector::operator/=(complex f){
	int i;

	loop(i,length) operator[](i) /= f;
	return *this;
}

CVector& CVector::operator=(const Function& f){
	int i;
	
	loop(i,length) operator[](i) = f.bvalue(i);
	return *this;  
}

CVector& CVector::operator=(const CVector& v){
	int i;

	loop(i,length)
		operator[](i) = v.bvalue(i);

	return *this;  
}

CVector& CVector::operator+=(const Function& f){
	int i;

	loop(i,length) operator[](i) += f.bvalue(i);
	return *this;
}

CVector& CVector::operator-=(const Function& f){
	int i;

	loop(i,length) operator[](i) -= f.bvalue(i);
	return *this;
}
CVector& CVector::operator*=(const Function& f){
	int i;

	loop(i,length) operator[](i) *= f.bvalue(i);
	return *this;
}
CVector& CVector::operator/=(const Function& f){
	int i;

	loop(i,length) operator[](i) /= f.bvalue(i);
	return *this;
}

CVector convolve(const BVector& a, const BVector& b){
	int i,j;
	CVector foo(a.length+b.length-1);

	loop(i, foo.length){

		foo[i] = 0;
		for(j=MAX(0,i-b.length+1);j<MIN(i+1,a.length);j++)
			foo[i] += a.bvalue(j) * b.bvalue(i-j);
	}
	return foo;
}

CVector cconvolve(const BVector& a, const BVector& b){
	int i,j;
	CVector foo(MAX(a.length,b.length));

	if(foo.length==b.length){
		loop(i, foo.length){
			foo[i] = 0;
			loop(j, a.length)
				foo[i] += a.bvalue(j) * b.bvalue((i-j+b.length)%b.length);
		}
	}else{
		loop(i, foo.length){
			foo[i] = 0;
			loop(j, b.length)
				foo[i] += b.bvalue(j) * a.bvalue((i-j+a.length)%a.length) ;
		}
	}
	return foo;
}
