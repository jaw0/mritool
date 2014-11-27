
#ifndef _vector_h
#define _vector_h

#ifdef RCSID
static const char *const rcsid_vector_h
="@(#)$Id: vector.h,v 1.1 1995/11/23 04:31:17 jaw Exp jaw $";
#endif

#include <iostream.h>
#include <math.h>
#include <complex.h>
#include <function.h>

class BVector;
class Vector;
class CVector;
class Matrix;


class BVector  : public Function {
	// Abstract
  protected:
	int _length;
	
  public:
	int length;

	BVector(){length=_length=0;}
	virtual ~BVector() {}

	virtual complex bvalue(float) const =0;
	virtual void	bput(float, complex) =0;

};



class Vector : public BVector {
  protected:
	float *data;
  public:

	Vector() {data=0;}
	Vector(int);
	Vector(const BVector&);
	Vector(int, const Function&);
	virtual ~Vector();

	virtual complex bvalue(float i) const {return vvalue(int(i));}
	virtual void	bput(float i, complex c){vput(int(i), real(c));}
	
	float  vvalue(int) const;
	void   vput(int i, float f) {operator[](i) = f;}
	float& operator[](int);
	float  operator[](int i) const {return vvalue(i);}
	operator CVector() const;
	
	Vector	eigenline(void) const;	// best fit line
	Vector	eigenline(Vector&) const;
	Matrix	t(void) const;		// vertical
	Vector	d(int n=1) const;	// n-th derivative
	virtual int	save(char*) const;
	virtual int	load(char*);

	
	Vector& operator= (const Function&);
	Vector& operator= (const Vector&);
        Vector& operator+=(const Function&);
        Vector& operator-=(const Function&);
        Vector& operator*=(const Function&);    // point by point
        Vector& operator/=(const Function&);
        Vector& operator= (float);
        Vector& operator+=(float);
        Vector& operator-=(float);
        Vector& operator*=(float);
        Vector& operator/=(float);

    friend Vector operator+(const Vector&, const Vector&);
    friend Vector operator-(const Vector&, const Vector&);
    friend Vector operator*(const Vector&, const Vector&);
    friend Vector operator/(const Vector&, const Vector&);
    friend Vector operator+(const Vector&, float);
    friend Vector operator-(const Vector&, float);
    friend Vector operator*(const Vector&, float);
    friend Vector operator/(const Vector&, float);
    friend int operator==(const Vector&, const Vector&);

    friend Vector arccos(const Vector&);

    friend ostream& operator<<(ostream&,  Vector&);

};


class CVector : public BVector {
  protected:
	complex *data;
  public:

	CVector() {data=0;}
	CVector(int);
	CVector(const CVector&);
	CVector(int, const Function&);
	virtual ~CVector();
	
	virtual complex bvalue(float i) const {return vvalue(int(i));}
	virtual void	bput(float i, complex c){vput(int(i), c);}

	virtual complex vvalue(int i) const;
	virtual void vput(int i, complex c) {
		operator[](i) = c;
	}

	complex& operator[](int);
	complex  operator[](int i) const {return vvalue(i);}

	operator Vector() const { return r();}

	virtual int	save(char*) const;
	virtual int	load(char*);
	Vector	r(void) const;		// real part
	Vector	i(void) const;		// imag part
	CVector	d(int n=1) const;	// n-th derivative

	CVector& operator= (const Function&);
	CVector& operator= (const CVector&);
        CVector& operator+=(const Function&);
        CVector& operator-=(const Function&);
        CVector& operator*=(const Function&);    // point by point
        CVector& operator/=(const Function&);
        CVector& operator= (complex);
        CVector& operator+=(complex);
        CVector& operator-=(complex);
        CVector& operator*=(complex);
        CVector& operator/=(complex);
	
    friend CVector operator+(const CVector&, const CVector&);
    friend CVector operator-(const CVector&, const CVector&);
    friend CVector operator*(const CVector&, const CVector&);
    friend CVector operator/(const CVector&, const CVector&);
    friend CVector operator+(const CVector&, complex);
    friend CVector operator-(const CVector&, complex);
    friend CVector operator*(const CVector&, complex);
    friend CVector operator/(const CVector&, complex);

    friend ostream& operator<<(ostream&,  CVector&);

};

extern Vector operator*(const Matrix&, const Vector&);
extern Vector operator*(const Vector&, const Matrix&);

extern CVector convolve (const BVector&, const BVector&); 
extern CVector cconvolve(const BVector&, const BVector&);


typedef Vector  Signal;
typedef CVector CSignal;




#endif /* !_vector_h */
