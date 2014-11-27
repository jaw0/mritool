

#ifndef _matrix_h
#define _matrix_h

#ifdef RCSID
static const char *const rcsid_matrix_h
="@(#)$Id: matrix.h,v 1.1 1995/11/23 04:31:15 jaw Exp jaw $";
#endif

#include <iostream.h>
#include <math.h>
#include <complex.h>
#include <function.h>
#include <vector.h>
#include <point.h>

#ifndef MAX
#	define MAX(x,y)	((x)>(y)?(x):(y))
#endif
#ifndef MIN
#	define MIN(x,y) ((x)<(y)?(x):(y))
#endif

class Matrix {
	// has to be accessed as M[y][x] (not [x][y])

  private:
	Vector* row;
	int     _height;

  public:
	int height, width;

	Matrix() {row=0; _height=height=width=0;}
	Matrix(int, int);	// x,y
	Matrix(int);		// y
	Matrix(Matrix&);
	~Matrix();
	
	Vector& operator[](int i);
	Vector	operator[](int) const;
	float&	operator()(int x, int y) {return operator[](y)[x];}
	float&	operator[](Point p) 	 {return operator[](p.y)[p.x];}

	Matrix& operator=(Matrix&);
	Matrix& operator=(float);
	Matrix& operator=(Vector& v);

	Matrix& operator+=(Matrix&);
	Matrix& operator-=(Matrix&);
	Matrix& operator*=(float);

	void 	eigen(Matrix& eV, Vector& ev);	// find eigen{values,vectors}
	void 	eigen(Vector& ev, Matrix& eV) {eigen(eV, ev);}
	void 	inv(void);				// invert matrix
	void 	gaussj(Matrix&);
	Matrix	t(void) const;		// transpose

    friend Matrix operator+(Matrix&, Matrix&);
    friend Matrix operator-(Matrix&, Matrix&);
    friend Matrix operator*(const Matrix&, const Matrix&);
    friend Matrix operator*(const Matrix&, float);

    friend Matrix arccos(Matrix&);
	
    friend ostream& operator<<(ostream&, Matrix&);


};

extern Vector operator*(const Matrix&, const Vector&);
extern Vector operator*(const Vector&, const Matrix&);

#endif
