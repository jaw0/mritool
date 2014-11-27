

#ifdef RCSID
static const char *const rcsid
= "@(#)$Id: vector.cc,v 1.1 1995/11/23 04:31:08 jaw Exp jaw $";
#endif

#include <matrix.h>
#include <math.h>
#include <fstream.h>

#define loop(i,j)	for(i=0;i<j;i++)
#define ABS( x ) 	( ((x)<0) ? -(x) : (x) )

Vector::~Vector(){
	delete [] data;
	_length = 0;
}

Vector::Vector(int l){
	_length = length = l;
	data = new float [_length];
}

Vector::Vector(const BVector& v){
	_length = 0;
	data = 0;
	length = v.length;
	this->operator=(v);
}

Vector::Vector(int l, const Function& f){
	int i;
	
	_length = length = l;
	data = new float [_length];

	loop(i, length) data[i] = real(f[i]);
}

float& Vector::operator[](int i){
	int j;

	if(i<0) i = -i;
	
	if(i>=_length){
		// beyond the edge!
		// make more room 
		float *ndata = new float [ i+1 ];
		loop(j, _length) ndata[j] = data[j];
		for(;j<=i;j++)  ndata[j] = 0;		// init to 0
		delete [] data;
		data = ndata;
		_length = i+1;
	}
	return data[i];
}

float Vector::vvalue(int i)  const{
	if(i<0 || i> _length) return 0;
	return data[int(i)];
}

Vector& Vector::operator=(const Function& f){
	int i;

	loop(i,length)
		operator[](i) = real(f.bvalue(i));

	return *this;  
}

Vector& Vector::operator=(const Vector& v){
	int i;

	loop(i,length)
		operator[](i) = v.vvalue(i);

	return *this;  
}

Vector& Vector::operator+=(const Function& f){
	int i;

	loop(i,length)
		operator[](i) += real(f.bvalue(i));
	return *this;
}

Vector& Vector::operator-=(const Function& f){
	int i;

	loop(i,length)
		operator[](i) -= real(f.bvalue(i));
	return *this;
}
Vector& Vector::operator*=(const Function& f){
	int i;

	loop(i,length)
		operator[](i) *= real(f.bvalue(i));
	return *this;
}
Vector& Vector::operator/=(const Function& f){
	int i;

	loop(i,length)
		operator[](i) /= real(f.bvalue(i));
	return *this;
}

Vector& Vector::operator=(float f){
	int i;

	loop(i,length) operator[](i) =  f;
	return *this;
}

Vector& Vector::operator+=(float f){
	int i;

	loop(i,length) operator[](i) +=  f;
	return *this;
}
Vector& Vector::operator-=(float f){
	int i;

	loop(i,length) operator[](i) -=  f;
	return *this;
}
Vector& Vector::operator*=(float f){
	int i;

	loop(i,length) operator[](i) *=  f;
	return *this;
}
Vector& Vector::operator/=(float f){
	int i;

	loop(i,length) operator[](i) /=  f;
	return *this;
}
	
Vector Vector::eigenline(void) const{
	// find best fit line
	// using eigenvector
	
	int i, j;
	float min,max;
	float center=0;
	float val, xval;
	float scale;
	Matrix I(2,2);
	
	if(!length) return 0;

	// find center of mass and scaling
	min = max = operator[](0);
	loop(i, length){
		val = operator[](i);
		center += val;
		if(val>max)max = val;
		if(val<min)min = val;
	}
	center /= length;
	if( min != max)
		scale = max - min;
	else
		scale = 1;

	// find inertia matrix
	I = 0;
	loop(i, length){
		val = (operator[](i) - center) / scale;
		I[0][0] += val * val;
		xval = float(i)/(length-1) - 0.5;
		I[1][1] += xval * xval;
		I[0][1] += xval * val;
	}
	I[1][0] = I[0][1];	// it is symetric

	Matrix eV(2,2);		// eigenvectors
	Vector ev(2);		// eigenvalues

	I.eigen(eV, ev);

	// which is largest ev?
	if(ev[0] < ev[1])
		j = 0;
	else
		j = 1;

	Vector result(length);

	// generate result
	loop(i, length){
		xval = float(i)/(length-1) - 0.5;
		result[i] = xval * eV[j][1]/eV[j][0] * scale + center;
	}

	return result;
}
Vector Vector::eigenline(Vector& xa) const{
	// find best fit line
	// using eigenvector
	
	int i, j, leng = MIN(length, xa.length);
	float ymin,ymax, xmin,xmax;
	float ycenter=0, xcenter=0;
	float val, xval;
	float yscale, xscale;
	Matrix I(2,2);
	
	if(!length) return 0;

	// find center of mass and scaling
	ymin = ymax = operator[](0);
	xmin = xmax = xa[0];
	loop(i, leng){
		val = operator[](i);
		ycenter += val;
		if(val>ymax)ymax = val;
		if(val<ymin)ymin = val;

		val = xa[i];
		xcenter += val;
		if(val>xmax)xmax = val;
		if(val<xmin)xmin = val;
	}
	xcenter /= leng;	ycenter /= leng;
	if( xmax != xmin)
		xscale = xmax - xmin;
	else
		xscale = 1;
	if( ymax != ymin)
		yscale = ymax - ymin;
	else
		yscale = 1;

	// find inertia matrix
	I = 0;
	loop(i, leng){
		val = (operator[](i) - ycenter) / yscale;
		I[0][0] += val * val;
		xval = ( xa[i] - xcenter ) / xscale;
		I[1][1] += xval * xval;
		I[0][1] += xval * val;
	}
	I[1][0] = I[0][1];	// it is symetric

	Matrix eV(2,2);		// eigenvectors
	Vector ev(2);		// eigenvalues

	I.eigen(eV, ev);

	// which is largest ev?
	if(ev[0] < ev[1])
		j = 0;
	else
		j = 1;

	Vector result(leng);

	// generate result
	loop(i, leng){
		xval = ( xa[i] - xcenter ) / xscale;
		result[i] = xval * eV[j][1]/eV[j][0] * yscale + ycenter;
	}

	return result;
}
	

Vector operator+(const Vector& a, const Vector& b){
	int i;
	Vector foo(a.length);
	
	loop(i, a.length) foo.data[i] = a[i] + b[i];

	return foo;
}

Vector operator-(const Vector& a, const Vector& b){
	int i;
	Vector foo(a.length);
	
	loop(i, a.length) foo.data[i] = a[i] - b[i];

	return foo;
}

Vector operator+(const Vector& a, float b){
	int i;
	Vector foo(a.length);
	
	loop(i, a.length) foo.data[i] = a[i] + b;

	return foo;
}

Vector operator-(const Vector& a, float b){
	int i;
	Vector foo(a.length);
	
	loop(i, a.length) foo.data[i] = a[i] - b;

	return foo;
}

Vector operator*(const Vector& a, const Vector& b){
	int i;
	Vector foo(a.length);
	
	loop(i, a.length) foo.data[i] = a[i] * b[i];

	return foo;
}

Vector operator/(const Vector& a, const Vector& b){
	int i;
	Vector foo(b.length);

	// use b.length to avoid divide by 0
	loop(i, b.length) foo.data[i] = a[i] / b[i];
	return foo;
}

Vector operator*(const Vector& a, float b){
	int i;
	Vector foo(a.length);
	
	loop(i, a.length) foo.data[i] = a[i] * b;

	return foo;
}

Vector operator/(const Vector& a, float b){
	int i;
	Vector foo(a.length);
	
	loop(i, a.length) foo.data[i] = a[i] / b;

	return foo;
}

ostream& operator<<(ostream& os, Vector& v){
	int i;

	loop(i,v.length)
		os << v.data[i] << "\t";
	return os;
}

Vector arccos(const Vector& v){
	int i;
	double dist=0;
	Vector foo(v.length);

	loop(i,v.length) dist += v[i] * v[i];
	loop(i,v.length) foo[i] = acos( double(v[i] / dist) );

	return foo;
}

int Vector::save(char* name) const{
	int i;
	ofstream of(name);
	
	if(!of) return 0;

	of << "# Vector[" << length << "]; File: " << name << endl;
	loop(i, length){
		of << operator[](i) << endl;
	}
	return 1;
}

int Vector::load(char* name){
	int i=0;
	ifstream ifs(name);

	if(!ifs) return 0;

	while(ifs){
		if(ifs.peek()=='#'){
			ifs.ignore(1024, '\n');
			continue;
		}
		ifs >> operator[](i++);
	}
	return 1;
}


Vector::operator CVector() const{
	int i;
	CVector foo(length);

	loop(i, length)
		foo[i] = operator[](i);
	return foo;
}

Matrix Vector::t(void) const{
	int i;
	Matrix foo(1, length);

	loop(i, length)
		foo[i][0] = operator[](i);

	return foo;
}

Vector Vector::d(int n) const{
	Vector foo(length), bar(length);
	int i;
	
	foo = *this;
	while(n--){
		loop(i, length-1)
			bar[i] = foo[i+1] - foo[i];
		bar[length-1] = bar[length];

		foo = bar;
	}
	return foo;
}

int operator==(const Vector& a, const Vector& b){
	if(a.length != b.length) return 0;

	int i;
	loop(i, a.length)
		if( a[i] != b[i] ) return 0;

	return 1;
}


	
	

	

