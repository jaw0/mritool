

#ifdef RCSID
static const char *const rcsid
= "@(#)$Id: matrix.cc,v 1.1 1995/11/23 04:31:05 jaw Exp jaw $";
#endif

#include <matrix.h>

#define loop(i,j)	for(i=0;i<j;i++)
#define ABS( x ) 	( ((x)<0) ? -(x) : (x) )


Matrix::~Matrix(){
	delete [] row;
	_height = 0;
}

Matrix::Matrix(int y){
	
	_height = height = y;
	width = 0;
	row = new Vector [height];
}

Matrix::Matrix(int x, int y){
	int i;

	_height = height = y;
	width  = x;
	row = new Vector [height];
	if(width>0)
		loop(i,height){
			row[i][width-1] = 0;	// alloc vector mem
			row[i].length = width;
		}
}

Matrix::Matrix(Matrix& m){
	_height = 0;
	row = 0;
	height = m.height;
	width = m.width;
	operator=(m);
}

Vector& Matrix::operator[](int i){
	int j,k;

	if(i<0) i = -i;
	
	if(i>=_height){
		// beyond the edge!
		// make more room
		Vector *nv = new Vector [ i+1 ];
		if(width>0)
			loop(k,i+1){
				nv[k].length = width;
				nv[k] = 0;	// will alloc vector mem
			}
		loop(j,_height) nv[j] = row[j]; 		// copy vectors
		if (row) delete [] row;
		row = nv;
		_height = i+1;
	}
	return row[i];
}

Vector Matrix::operator[](int i) const {

	if(i<0 || i>=_height) return row[0];
	return row[i];
}

Matrix& Matrix::operator=(Matrix& m){
	int i;

	loop(i,m.height) operator[](i) = m[i];
			// using op[] will cause mem to be allocated if needed
	return *this;
}

Matrix& Matrix::operator=(float f){
	int i;

	loop(i,height) operator[](i) = f;
	return *this;
}

Matrix& Matrix::operator=(Vector& v){
	int i;

	loop(i,height) operator[](i) = v;
	return *this;
}


Matrix& Matrix::operator+=(Matrix& m){
	int i;

	loop(i,m.height) operator[](i) += m[i];
	return *this;
}

Matrix& Matrix::operator-=(Matrix& m){
	int i;

	loop(i,m.height) operator[](i) -= m[i];
	return *this;
}

Matrix& Matrix::operator*=(float f){
	int i;

	loop(i,height) operator[](i) *= f;
	return *this;
}

Matrix operator+(Matrix& a, Matrix& b){
	int i;
	Matrix foo(a.width, a.height);

	loop(i,a.height) foo.row[i] = a[i] + b[i];
	return foo;
}

Matrix operator-(Matrix& a, Matrix& b){
	int i;
	Matrix foo(a.width, a.height);

	loop(i,a.height) foo.row[i] = a[i] - b[i];
	return foo;
}

Matrix operator*(const Matrix& a, float b){
	int i;
	Matrix foo(a.width, a.height);

	loop(i,a.height) foo.row[i] = a[i] * b;
	return foo;
}
	
Matrix operator*(const Matrix& a, const Matrix& b){
	int i,j,k;
	Matrix foo(a.width, b.height);
	Vector av, bv;
	float bf;
	
	if(a.height != b.width){
		// something bad!
		return foo;
	}
	foo = 0.0;
	loop(k, b.height){
		bv = b[k];
		loop(j, a.height){
			av = a[j];
			bf = bv[j];
			loop(i, a.width){
				foo[k][i] += av[i] * bf;
			}
		}
	}

	return foo;
}
				  
ostream& operator<<(ostream& os, Matrix& m){
	int i;

	loop(i,m.height)
		os << m[i] << endl;
	return os;
}

					  

#define ROTATE(a,i,j,k,l) g=a[i][j];h=a[k][l];a[i][j]=g-s*(h+g*tau);\
	a[k][l]=h+s*(g-h*tau);

void Matrix::eigen(Matrix& eV, Vector& ev){
	// find eigen{values, vectors}
	// matrix ought be real and symetric and .....
	
	int n=height, nrot;
	Matrix a(n+1,n+1),v(n+1,n+1);
	int j,iq,ip,i;
	float tresh,theta,tau,t,sm,s,h,g,c;
	Vector b(n+1), z(n+1),d(n+1);

	
	for(j=0;j<n;j++)for(i=0;i<n;i++)
		a[j+1][i+1] = operator[](j)[i];	// slide matrix to 1,1 orgin

	
	for (ip=1;ip<=n;ip++) {
		for (iq=1;iq<=n;iq++) v[ip][iq]=0.0;
		v[ip][ip]=1.0;
	}
	for (ip=1;ip<=n;ip++) {
		b[ip]=d[ip]=a[ip][ip];
		z[ip]=0.0;
	}
	nrot=0;
	for (i=1;i<=50;i++) {
		sm=0.0;
		for (ip=1;ip<=n-1;ip++) {
			for (iq=ip+1;iq<=n;iq++)
				sm += fabs(a[ip][iq]);
		}
		if (sm == 0.0) {
			// slide back to 0,0 space
			for(j=0;j<n;j++){
				for(i=0;i<n;i++)
					eV[j][i] = v[j+1][i+1];
				ev[j] = d[j+1];
			}
			return;
		}
		if (i < 4)
			tresh=0.2*sm/(n*n);
		else
			tresh=0.0;
		for (ip=1;ip<=n-1;ip++) {
			for (iq=ip+1;iq<=n;iq++) {
				g=100.0*fabs(a[ip][iq]);
				if (i > 4 && fabs(d[ip])+g == fabs(d[ip])
					&& fabs(d[iq])+g == fabs(d[iq]))
					a[ip][iq]=0.0;
				else if (fabs(a[ip][iq]) > tresh) {
					h=d[iq]-d[ip];
					if (fabs(h)+g == fabs(h))
						t=(a[ip][iq])/h;
					else {
						theta=0.5*h/(a[ip][iq]);
						t=1.0/(fabs(theta)+sqrt(1.0+theta*theta));
						if (theta < 0.0) t = -t;
					}
					c=1.0/sqrt(1+t*t);
					s=t*c;
					tau=s/(1.0+c);
					h=t*a[ip][iq];
					z[ip] -= h;
					z[iq] += h;
					d[ip] -= h;
					d[iq] += h;
					a[ip][iq]=0.0;
					for (j=1;j<=ip-1;j++) {
						ROTATE(a,j,ip,j,iq)
					}
					for (j=ip+1;j<=iq-1;j++) {
						ROTATE(a,ip,j,j,iq)
					}
					for (j=iq+1;j<=n;j++) {
						ROTATE(a,ip,j,iq,j)
					}
					for (j=1;j<=n;j++) {
						ROTATE(v,j,ip,j,iq)
					}
					++nrot;
				}
			}
		}
		for (ip=1;ip<=n;ip++) {
			b[ip] += z[ip];
			d[ip]=b[ip];
			z[ip]=0.0;
		}
	}
	// nrerror("Too many iterations in routine JACOBI");
}

#undef ROTATE

					
						
Vector operator*(const Matrix& m, const Vector& v){
	// assume [] * |
	Vector foo(m.height);
	int i,j;

	foo = 0;

	loop(j,m.height)
		loop(i,v.length){
			foo[j] += v[i] * m[j][i];
		}

	return foo;
}

Vector operator*(const Vector& v, const Matrix& m){
	// assume - * []
	Vector foo(m.width);
	int i,j;

	foo = 0;

	loop(j,m.width)
		loop(i,v.length){
			foo[j] += v[i] * m[i][j];
		}
	return foo;
}



#define SWAP(a,b) {float temp=(a);(a)=(b);(b)=temp;}

void Matrix::gaussj(Matrix& inp){

	int n=height, m=inp.height;
		    
	Matrix a(n+1,n+1),b(n+1,m+1);
        int i,icol,irow,j,k,l,ll;
        float big,dum,pivinv;
	Vector indxc(n), indxr(n), ipiv(n);
	
	for(j=0;j<n;j++)for(i=0;i<n;i++)
		a[j+1][i+1] = operator[](j)[i];	// slide matrix to 1,1 orgin
	
	for(j=0;j<n;j++)for(i=0;i<m;i++)
		b[j+1][i+1] = inp[j][i];

	ipiv = 0;
        for (i=1;i<=n;i++) {
                big=0.0;
                for (j=1;j<=n;j++)
                        if (ipiv[j] != 1)
                                for (k=1;k<=n;k++) {
                                        if (ipiv[k] == 0) {
                                                if (fabs(a[j][k]) >= big) {
                                                        big=fabs(a[j][k]);
                                                        irow=j;
                                                        icol=k;
                                                }
                                        } else if (ipiv[k] > 1)
						; //nrerror("GAUSSJ: Singular Matrix-1");
                                }
                ++(ipiv[icol]);
                if (irow != icol) {
                        for (l=1;l<=n;l++) SWAP(a[irow][l],a[icol][l])
                        for (l=1;l<=m;l++) SWAP(b[irow][l],b[icol][l])
                }
                indxr[i]=irow;
                indxc[i]=icol;
                if (a[icol][icol] == 0.0) ; //nrerror("GAUSSJ: Singular Matrix-2");
                pivinv=1.0/a[icol][icol];
                a[icol][icol]=1.0;
                for (l=1;l<=n;l++) a[icol][l] *= pivinv;
                for (l=1;l<=m;l++) b[icol][l] *= pivinv;
                for (ll=1;ll<=n;ll++)
                        if (ll != icol) {
                                dum=a[ll][icol];
                                a[ll][icol]=0.0;
                                for (l=1;l<=n;l++) a[ll][l] -= a[icol][l]*dum;
                                for (l=1;l<=m;l++) b[ll][l] -= b[icol][l]*dum;
                        }
        }
        for (l=n;l>=1;l--) {
                if (indxr[l] != indxc[l])
                        for (k=1;k<=n;k++)
                                SWAP(a[k][int(indxr[l])],a[k][int(indxc[l])]);
        }
	// slide back to 0,0 space
	for(j=0;j<n;j++)for(i=0;i<n;i++)
			operator[](j)[i] = a[j+1][i+1];
	
	for(j=0;j<n;j++)for(i=0;i<m;i++)
		inp[j][i] = b[j+1][i+1];

}

#undef SWAP


void Matrix::inv(void){

	Matrix bar(height,1);

	gaussj(bar);
}

Matrix arccos(Matrix& m){
	int i;
	Matrix foo(m.height,m.width);

	loop(i,m.height) foo.row[i] = arccos( m[i] );

	return foo;
}

Matrix Matrix::t(void) const{
	int i,j;
	Matrix foo(width, height);
	
	loop(j, height) loop(i, width)
		foo[i][j] = operator[](j)[i]; 

	return foo;
}

