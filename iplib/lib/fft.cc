
// stolen from Jon Riek
// blame him...

#include <images.h>
#include <stdlib.h>

void FFTAR(complex * , int , int , int , int );
void FFTAC(complex * , int , int , int , int );
void FFT2D(complex * , int , int , int , int );
void FFT1D(float * , float * , int , int , int );
void FFT1DAllRows(float * , float *, int , int , int , int );
void Transpose(float * , float * , int ,int);
void fft1dpp(complex * , int , int , int );
void Transpose2(float *,float *,int,int);
 
extern "C"  {
        void FFTMX2D(float *,float *,int,int,int);
        void FFTMX1D(float *,float *,int,int);
};

#define SWAP(a,b) {temp=a;a=b;b=temp;}
#define MUL(a,b,c,d) {temp=a*c-b*d;tempi=a*d+b*c;}

#define TWOPI	(2 * M_PI)

void FFTAR(complex *com, int mm, int nn, int isign, int centerDC)
{
	int size=mm*nn;
	float *re=new float[2*size];
	float *im=re+size;
	float *tr,*ti;
	complex *tc;
	tr=re;ti=im;tc=com;
	for(int i=0;i<size;i++)
		{
			*(tr++)=real(*tc);
			*(ti++)=imag(*(tc++));
		}
	FFT1DAllRows( re,im, mm, nn, isign, centerDC );
	tc=com;tr=re;ti=im;
	for(i=0;i<size;i++) *(tc++)=complex(*(tr++),*(ti++));
	delete [] re;
	return;
}


void FFTAC(complex *com, int mm, int nn, int isign, int centerDC)
{
	int size=mm*nn;
	float *re=new float[2*size];
	float *im=re+size;
	float *tr,*ti;
	complex *tc;
	tr=re;ti=im;tc=com;
	for(int i=0;i<size;i++)
		{
			*(tr++)=real(*tc);
			*(ti++)=imag(*(tc++));
		}
	if(mm==nn)
		{
			Transpose( re,im, mm,nn );
			FFT1DAllRows( re,im, mm, nn, isign, centerDC );
			Transpose( re,im, mm,nn );
		}
	else
		{
			Transpose2(re, im, mm, nn );
			FFT1DAllRows( re,im, nn, mm, isign, centerDC );
			Transpose2( re, im, nn, mm );
		}
	tc=com;tr=re;ti=im;
	for(i=0;i<size;i++) *(tc++)=complex(*(tr++),*(ti++));
	delete [] re;
	return;
}


void FFT2D(complex *com, int mm, int nn, int isign, int centerDC)
{
	int size=mm*nn;
	float *re=new float[2*size];
	float *im=re+size;
	float *tr,*ti;
	complex *tc;
	tr=re;ti=im;tc=com;
	for(int i=0;i<size;i++)
		{
			*(tr++)=real(*tc);
			*(ti++)=imag(*(tc++));
		}
	if(mm==nn)
		{
			FFT1DAllRows( re,im, mm, nn, isign, centerDC );
			Transpose( re,im, mm,nn );
			FFT1DAllRows( re,im, mm, nn, isign, centerDC );
			Transpose( re,im, mm,nn );
		}
	else
		{
			FFT1DAllRows( re,im, mm, nn, isign, centerDC );
			Transpose2(re, im, mm, nn );
			FFT1DAllRows( re,im, nn, mm, isign, centerDC );
			Transpose2( re, im, nn, mm );
		}
	tc=com;tr=re;ti=im;
	for(i=0;i<size;i++) *(tc++)=complex(*(tr++),*(ti++));
	delete [] re;
	return;
}

void fft1dpp(complex *ar,int nn,int isign, int centerDC)
{
	float *re,*im;

	re=new float[nn];
	im=new float[nn];
	if(!re || !im) exit(1);
	for(int i=0;i<nn;i++) 
		{
			re[i]=real(ar[i]);
			im[i]=imag(ar[i]);
		}
	FFT1D(re,im,nn,isign,centerDC);
	for(i=0;i<nn;i++) ar[i]=complex(re[i],im[i]);
	delete [] re;
	delete [] im;
}

void FFT1D(float *re, float *im, int nn, int isign, int centerDC )
{
	short	mmax,m,j,istep,i;
	float temp,tempi;
	float wr,wi,wpr,wpi;
	double	theta;
	
	j = 0;									/* bit reverse section */
	for( i=0; i<nn; i++)					
		{
			if(j > i)
				{
					SWAP(re[j],re[i])
						SWAP(im[j],im[i])
				}
			m = nn>>1;
			while((m > 1) && (j >= m))
				{
					j -= m;
					m >>= 1;
				}
			j += m;
		}
	
	mmax = 1;
	while (nn > mmax)				/* outer loop executed log2(nn) times */
		{
			istep = 2*mmax;
			
			theta = TWOPI/(isign*istep);
			wpr=cos(theta);
			wpi=sin(theta);
			wr = 1.0;
			wi = 0;
			if( (istep == nn) && centerDC )			/* if centerDC is TRUE, slight modification of */
				{										/* the last repetition of the outer loop will */
					for ( m=0; m<mmax; m++ )			/* swap halves of the transform array with no */
						{									/* extra processing at all */
							for ( i=m; i<nn; i+=istep )
								{
									j = i+mmax;
									MUL(wr,wi,re[j],im[j])
										re[j]=re[i]+temp;
									im[j]=im[i]+tempi;
								/* only changes are in these 2 lines */
								/* - was changed to + in this one */
									re[i]-=temp;
									im[i]-=tempi;	/* and += was changed to -= in this one */
								/* See original code below */
								}
							MUL(wr,wi,wpr,wpi)
								wr=temp;wi=tempi;
						}
					mmax = istep;
				}
			else
				if( (istep == 2) && centerDC )			/* if centerDC is TRUE, slight modification of */
					{										/* the first repetition of the outer loop will */
						for ( m=0; m<mmax; m++ )			/* swap halves of the orignal array with no */
							{									/* extra processing at all */
								for ( i=m; i<nn; i+=istep )
									{
										j = i+mmax;
										MUL(wr,wi,re[i],im[i])
											re[i]=re[j]+temp;
										im[i]=im[j]+tempi;
										re[j]-=temp;
										im[j]-=tempi;
							/* only changes are in these 6 lines */
							/* All i's and j's were swapped */
							/* See original code below */
									}
								MUL(wr,wi,wpr,wpi)
									wr=temp;wi=tempi;
							}
						mmax = istep;
					}
				else
					{
						for ( m=0; m<mmax; m++ )			/* The reason for so much code duplication is to */
							{									/* cut down on the processing overhead.  If the */
								for ( i=m; i<nn; i+=istep )		/* "if" is confined to the outermost loop, it will */
									{								/* be called the least number of times. */
										j = i+mmax;
										MUL(wr,wi,re[j],im[j])
											re[j]=re[i]-temp;
										im[j]=im[i]-tempi;
										re[i]+=temp;
										im[i]+=tempi;
									}
								MUL(wr,wi,wpr,wpi)
									wr=temp;wi=tempi;
							}
						mmax = istep;
					}
		}
	
	if (isign == 1)
		for (i=0; i<nn; i++)
			{
				re[i]/=nn;im[i]/=nn;
			}
}


/*******
*
*	FFT1DAllRows() : Performs a 1D FFT on all of the rows of the complex data set
*					 defined by the 2 2D arrays "real" & "imag" of length "nn"*"nn".
*					This should save time by putting more code inside the inner loop
*					thus reducing the overhead of implementing the loop. This neat
*					little hack is courtesy of Greg Forbes.
*					Also passed is a boolean determining whether the DC term of each transform
*					ends up in the center of the array instead of in the first element.
*
*******/
void FFT1DAllRows(float *re, float *im, int mm, int nn, int isign, int centerDC)

{
	short	mmax,m,j,istep,i,row;
	float temp,tempi,*tr,*ti,wr,wi,wpr,wpi;
	double	theta;
	
	tr=re;
	ti=im;
	
	j = 0;									/* bit reverse section */
	for( i=0; i<mm; i++)					
		{
			if(j > i)
				{
					for( row=0; row<nn; row++, tr+=mm ,ti+=mm)		/* this loop down all rows of image */
						{
							SWAP(tr[j],tr[i]) SWAP(ti[j],ti[i])
						}
					tr=re;ti=im;
				}
			m = mm>>1;
			while((m > 1) && (j >= m))
				{
					j -= m;
					m >>= 1;
				}
			j += m;
		}
	
	/*m = nn/2*log((double)nn)/log(2.0);*/
	tr=re;
	ti=im;
	mmax = 1;
	while (mm > mmax)				/* outer loop executed log2(nn) times */
		{
			istep = 2*mmax;
			
			theta = TWOPI/(isign*istep);
			wpr=cos(theta);
			wpi=sin(theta);
			
			wr = 1.0;
			wi=0;
			if( (istep == mm) && centerDC )			/* if centerDC is TRUE, slight modification of */
				{										/* the last repetition of the outer loop will */
					for ( m=0; m<mmax; m++ )			/* swap halves of the transform array with no */
						{									/* extra processing at all */
							for ( i=m; i<mm; i+=istep )
								{
									j = i+mmax;
									for( row=0; row<nn; row++, tr+=mm, ti+=mm )	/* this loops down all rows of image */
										{
											MUL(wr,wi,tr[j],ti[j])
												tr[j]=tr[i]+temp;
											ti[j]=ti[i]+tempi;
											tr[i]-=temp;
											ti[i]-=tempi;
										}
									tr=re;ti=im;
								}
							MUL(wr,wi,wpr,wpi)
								wr=temp;wi=tempi;
						}
					mmax = istep;
				}
			else
				if( (istep == 2) && centerDC )			/* if centerDC is TRUE, slight modification of */
					{										/* the first repetition of the outer loop will */
						for ( m=0; m<mmax; m++ )			/* swap halves of the original array with no */
							{									/* extra processing at all */
								for ( i=m; i<mm; i+=istep )
									{
										j = i+mmax;
										for( row=0; row<nn; row++,tr+=mm,ti+=mm)	/* this loops down all rows of image */
											{
												MUL(wr,wi,tr[i],ti[i])
													tr[i]=tr[j]+temp;
												ti[i]=ti[j]+tempi;
												tr[j]-=temp;
												ti[j]-=tempi;

											}
										tr=re;ti=im;
									}
								MUL(wr,wi,wpr,wpi)
									wr=temp;wi=tempi;
							}
						mmax = istep;
					}
				else
					{
						for ( m=0; m<mmax; m++ )			/* The reason for so much code duplication is to */
							{									/* cut down on the processing overhead.  If the */
								for ( i=m; i<mm; i+=istep )		/* "if" is confined to the outermost loop, it will */
									{								/* be called the least number of times. */
										j = i+mmax;
										for( row=0; row<nn; row++, tr+=mm, ti+=mm )
											{
												MUL(wr,wi,tr[j],ti[j])
													tr[j]=tr[i]-temp;
												ti[j]=ti[i]-tempi;
												tr[i]+=temp;
												ti[i]+=tempi;
											}
										tr=re;ti=im;
									}
								MUL(wr,wi,wpr,wpi)
									wr=temp;wi=tempi;
							}
						mmax = istep;
					}
		}
	
	if (isign == 1)
		for (i=0; i<mm; i++)
			{
				for( row=0; row<nn; row++, tr+=mm ,ti+=mm)
					{
						tr[i] /= mm;ti[i]/=mm;
					}
				tr=re;ti=im;
			}
}


/*******
*
*	Transpose() : Transposes an "mm"x"nn" matrix.
*					Routine begins in the top left corner of matrix and
*					swaps elements in columns of top-right half with their
*					counterparts in the rows of bottom-left half.
*  
*					Inner loop moves down columns of top-right half
*					and across rows of bottom-left half.
*					Outer loop resets ptrs to top and left edges and
*					moves to next column of top-right half and next
*					row of bottom-left half.
*
*******/
void Transpose(float *matrix1,float *matrix2, int mm, int nn)
{
	float	*topR1, *botL1;		/* ptrs to top-right and bottom-left sections of mat1 */
	float	*tTopR1, *tBotL1;	/* temp ptrs to top-right and bottom-left sections  of mat1 */
	float	*topR2, *botL2;		/* same as above for matrix2 */
	float	*tTopR2, *tBotL2;	/* same as above for matrix2 */
	float	temp;				/* used for the SWAP */
	int	h, v;				/* loop counters */
	int	moveLeft, moveDown;	/* consts used to increment ptr through 2-D array */
	
	moveLeft = 1;
	moveDown = mm;
	
	topR1 = matrix1 + moveLeft;
	botL1 = matrix1 + moveDown;
	topR2 = matrix2 + moveLeft;
	botL2 = matrix2 + moveDown;
	
	for( h=1; h<nn; h++, topR1=tTopR1+moveLeft, botL1=tBotL1+moveDown, topR2=tTopR2+moveLeft, botL2=tBotL2+moveDown ) 
		{
			tTopR1 = topR1;		/* remember locations along top and left of matrix1 */
			tBotL1 = botL1;
			tTopR2 = topR2;		/* remember locations along top and left of matrix2 */
			tBotL2 = botL2;
			for( v=1; v<=h; v++, topR1+=moveDown, botL1+=moveLeft, topR2+=moveDown, botL2+=moveLeft ) 
				{
					SWAP( *topR1, *botL1 );
					SWAP( *topR2, *botL2 );
				}
		}
}

void Transpose2(float *matrix,float *matrix2, int mm, int nn)

{
	float temp1,temp2,temp3,temp4,*t1,*t2;
	long *track,loc;
	int i,j,row,col;
	int size = mm*nn;
	t1=matrix;
	t2=matrix2;
	track=new long[(size+31)/32];

	for(i=0;i<nn;i++)
		for(j=0;j<mm;j++,t1++,t2++)
			{
				row=i;col=j;
				loc=col*nn+row;
				temp1=*t1;
				temp2=*t2;
				while(!(track[loc/32]& 1<<(loc%32)))
					{
						temp3=matrix[loc];
						temp4=matrix2[loc];
						matrix[loc]=temp1;
						matrix2[loc]=temp2;
						temp1=temp3;
						temp2=temp4;
						track[loc/32]|=1<<(loc%32);
						row=(int)loc/mm;
						col=(int)(loc%mm);
						loc=col*nn+row;
					}
			}

	delete [] track;

	return;
}
