
/* stolen from Jon Riek
   blame him...
*/

#include <math.h>
#define max(a,b) ((a)>(b)?(a):(b))
#define min(a,b) ((a)<(b)?(a):(b))
#define SWAP(a,b) {temp=a;a=b;b=temp;}
void 
factor(n, nfac, kt, m)
     int             n, *nfac, *kt, *m;
{
	int             nf, k, jj, j, maxp, maxf;

	*m = 0;
	nf = abs(n);
	k = nf;
	if (nf == 1)
		return;
	/* Pull out the factors of 4**2  */

	while (k % 16 == 0) 
		{
			(*m)++;
			nfac[*m] = 4;
			k /= 16;
		}

	/* Pull out the factors of 3**2, 5**2, 7**2,...  */

	j = 3;
	jj = 9;
	while (jj <= k) 
		{
			while (k % jj == 0) 
				{
					(*m)++;
					nfac[*m] = j;
					k /= jj;
				}
			j += 2;
			jj = j * j;
		}

	if (k <= 4) 
		{
			*kt = *m;
			nfac[*m + 1] = k;
			if (k != 1)
				(*m)++;
		} 
	else 
		{
			if (k % 4 == 0) 
				{
					(*m)++;
					nfac[*m] = 2;
					k /= 4;
				}
			/* all square factors out now, but k >5 still */
			*kt = *m;
			j = 2;
			while (j <= k) 
				{
					if (k % j == 0) 
						{
							(*m)++;
							nfac[*m] = j;
							k /= j;
						}
					j = ((j + 1) / 2) * 2 + 1;
				}
		}
	for (j = *kt; j > 0; j--) 
		{
			(*m)++;
			nfac[*m] = nfac[j];
		}
	return;
}



void Transpose(matrix1, matrix2, mm)
     float          *matrix1, *matrix2;
     int             mm;
{
	float          *topR1, *botL1;	/* ptrs to top-right and bottom-left
					* sections of mat1 */
	float          *tTopR1, *tBotL1;	/* temp ptrs to top-right and
						* bottom-left sections  of
						* mat1 */
	float          *topR2, *botL2;	/* same as above for matrix2 */
	float          *tTopR2, *tBotL2;	/* same as above for matrix2 */
	float           temp;	/* used for the SWAP */
	int             h, v;	/* loop counters */
	int             moveLeft, moveDown;	/* consts used to increment
						* ptr through 2-D array */

	moveLeft = 1;
	moveDown = mm;

	topR1 = matrix1 + moveLeft;
	botL1 = matrix1 + moveDown;
	topR2 = matrix2 + moveLeft;
	botL2 = matrix2 + moveDown;

	for (h = 1; h < mm; h++, topR1 = tTopR1 + moveLeft, botL1 = tBotL1 + moveDown, topR2 = tTopR2 + moveLeft, botL2 = tBotL2 + moveDown) 
		{
			tTopR1 = topR1;	/* remember locations along top and left of
					* matrix1 */
			tBotL1 = botL1;
			tTopR2 = topR2;	/* remember locations along top and left of
					* matrix2 */
			tBotL2 = botL2;
			for (v = 1; v <= h; v++, topR1 += moveDown, botL1 += moveLeft, topR2 += moveDown, botL2 += moveLeft) 
				{
					SWAP(*topR1, *botL1);
					SWAP(*topR2, *botL2);
				}
		}
}


void Transpose2(matrix, imatrix, matrix2, imatrix2, mm, nn)
     float          *matrix, *matrix2, *imatrix, *imatrix2;
     int             mm, nn;
{
	float          *temp1, *temp2, *temp3, *tempi1, *tempi2, *tempi3;
	int             i, j;
	int             size = mm * nn;
	temp1 = matrix;
	temp2 = matrix2;
	tempi1 = imatrix;
	tempi2 = imatrix2;
	for (i = 0; i < mm; i++, temp1++, tempi1++)
		for (j = 0, temp3 = temp1, tempi3 = tempi1; j < nn; j++, temp3 += mm, tempi3 += mm) 
			{
				*(temp2++) = *temp3;
				*(tempi2++) = *tempi3;
			}
	return;
}


void fft1dmx(a,b,nc,isn,m,kt,nfac)
     float *a,*b;
     int nc,isn,m,kt,*nfac;
{
	int             nt, nn, jc, lim, klim, i, jf, kk, kspan,
		k1, k2, l;
	int             mm, k3, ins, k4, j, jj, maxf, k, kspnn, okk;
	float           rad, c72, s72, s120, ak, dr, s1, c1, cd, sd, bk,
		bj, c2, s2, c3;
	float           akp, ajp, ajm, bkp, bjm, s3, aa, bb, akm, bkm,
		aj, bjp;
	float          *at, *ck, *bt, *sk;
	int            *np;

	/*
	* FFTMX is a subroutine called by FFT to compute mixed-radix Fourier
	* Transform
	*/

	maxf = m - kt;
	maxf = nfac[maxf];
	if (kt > 0)
		maxf = max(nfac[kt], maxf);
	at = (float *) malloc(4 * (maxf + 1) * sizeof(float));
	ck = at + maxf;
	bt = ck + maxf;
	sk = bt + maxf;
	maxf = nc;
	for (i = 1; i < kt; i++)
		maxf /= (nfac[i] * nfac[i]);
	np = (int *) malloc(maxf * sizeof(int));
	nt = nc;
	rad = atan(1.0);
	s72 = rad / 0.625;
	c72 = cos(s72);
	s72 = sin(s72);
	s120 = sqrt(0.75);
	if (isn > 0) 
		{
			ak = 1.0 / nc;
			for (j = 1; j <= nt; j++) 
				{
					a[j] *= ak;
					b[j] *= ak;
				}
		} 
	else 
		{
			s72 = -s72;
			s120 = -s120;
			rad = -rad;
		}
	kspan = nc;
	nn = nt - 1;
	jc = 1;
	/*---- sin, cos values are re-initialized each lim step ---*/
	lim = 32;
	klim = lim;
	i = 0;
	jf = 0;
	maxf = m - kt;
	maxf = nfac[maxf];
	if (kt > 0)
		maxf = max(nfac[kt], maxf);
	/* compute Fourier Transform */
	for (i = 1; i <= m; i++) 
		{
			dr = 8.0/kspan;
			cd = 1.0 - cos(dr * rad);
			sd = sin(dr * rad);
			kk = 1;
			if (nfac[i] == 2) 
				{
					/*
					* transform for factor of 2 (including rotation
					* factor)
					*/
					kspan >>= 1;
					for (k1 = kspan + 2; kk <= 1; kk -= nn)
						for (; kk <= nn; kk = k2 + kspan) 
							{
								k2 = kk + kspan;
								ak = a[k2];
								bk = b[k2];
								a[k2] = a[kk] - ak;
								b[k2] = b[kk] - bk;
								a[kk] = a[kk] + ak;
								b[kk] = b[kk] + bk;
							}
					if(kk<=kspan)
						{
							for(;kk <= 2;k1 += 2,kk = ((k1 - kspan) >> 1) + 1)
								{
									c1 = 1.0 - cd;
									s1 = sd;
									mm = min(k1 >> 1, klim);
								  ll80:				k2 = kk + kspan;
									ak = a[kk] - a[k2];
									bk = b[kk] - b[k2];
									a[kk] = a[kk] + a[k2];
									b[kk] = b[kk] + b[k2];
									a[k2] = c1 * ak - s1 * bk;
									b[k2] = s1 * ak + c1 * bk;
									kk = k2 + kspan;
									if (kk < nt)
										goto ll80;
									k2 = kk - nt;
									c1 = -c1;
									kk = k1 - k2;
									if (kk > k2)
										goto ll80;
									kk++;
									if (kk <= mm)
										{
											ak = c1 - (cd * c1 + sd * s1);
											s1 = (sd * c1 - cd * s1) + s1;
											c1 = 0.5 / (ak * ak + s1 * s1) + 0.5;
											s1 = c1 * s1;
											c1 = c1 * ak;
											goto ll80;
										}
									if (kk < k2) 
										{
											s1 = (kk - 1.0) * dr * rad;
											c1 = cos(s1);
											s1 = sin(s1);
											mm = min(k1 / 2, mm + klim);
											goto ll80;
										}
								}
						}
				}
			/* -------------------- factor of 3---- */
			else if (nfac[i] == 3) 
				{
					kspnn = kspan;
					kspan /= 3;
					for (; kk <= kspan; kk -= nn)
						for (; kk < nn; kk = k2 + kspan) 
							{
								k1 = kk + kspan;
								k2 = k1 + kspan;
								ak = a[kk];
								bk = b[kk];
								aj = a[k1] + a[k2];
								bj = b[k1] + b[k2];
								a[kk] = ak + aj;
								b[kk] = bk + bj;
								ak = -0.5 * aj + ak;
								bk = -0.5 * bj + bk;
								aj = (a[k1] - a[k2]) * s120;
								bj = (b[k1] - b[k2]) * s120;
								a[k1] = ak - bj;
								b[k1] = bk + aj;
								a[k2] = ak + bj;
								b[k2] = bk - aj;
							}
				} 
			else if (nfac[i] == 4) 
				{
					/* ------- factor of 4 -------- */
					kspnn = kspan;
					kspan = kspan / 4;
				  ll120:		c1 = 1.0;
					s1 = 0;
					mm = min(kspan, klim);
					goto ll150;
				  ll130:		c2 = c1 - (cd * c1 + sd * s1);
					s1 = (sd * c1 - cd * s1) + s1;
					c1 = 0.5 / (c2 * c2 + s1 * s1) + 0.5;
					s1 = c1 * s1;
					c1 = c1 * c2;
				  ll140:		c2 = c1 * c1 - s1 * s1;
					s2 = c1 * s1 * 2.0;
					c3 = c2 * c1 - s2 * s1;
					s3 = c2 * s1 + s2 * c1;
				  ll150:		k1 = kk + kspan;
					k2 = k1 + kspan;
					k3 = k2 + kspan;
					akp = a[kk] + a[k2];
					akm = a[kk] - a[k2];
					ajp = a[k1] + a[k3];
					ajm = a[k1] - a[k3];
					a[kk] = akp + ajp;
					ajp = akp - ajp;
					bkp = b[kk] + b[k2];
					bkm = b[kk] - b[k2];
					bjp = b[k1] + b[k3];
					bjm = b[k1] - b[k3];
					b[kk] = bkp + bjp;
					bjp = bkp - bjp;
					if (isn < 0) 
						{
							akp = akm + bjm;
							akm = akm - bjm;
							bkp = bkm - ajm;
							bkm = bkm + ajm;
						} 
					else 
						{
							akp = akm - bjm;
							akm = akm + bjm;
							bkp = bkm + ajm;
							bkm = bkm - ajm;
						}
					if (s1 == 0.0) 
						{
							a[k1] = akp;
							b[k1] = bkp;
							a[k2] = ajp;
							b[k2] = bjp;
							a[k3] = akm;
							b[k3] = bkm;
						} 
					else 
						{
							a[k1] = akp * c1 - bkp * s1;
							b[k1] = akp * s1 + bkp * c1;
							a[k2] = ajp * c2 - bjp * s2;
							b[k2] = ajp * s2 + bjp * c2;
							a[k3] = akm * c3 - bkm * s3;
							b[k3] = akm * s3 + bkm * c3;
						}
					kk = k3 + kspan;
					if (kk <= nt)
						goto ll150;
				  ll170:		kk+= 1 - nt;
					if (kk <= mm)
						goto ll130;
					if (kk < kspan) 
						{
							s1 = (kk - 1.0) * dr * rad;
							c1 = cos(s1);
							s1 = sin(s1);
							mm = min(kspan, mm + klim);
							goto ll140;
						}
					kk = kk - kspan + 1;
					if (kk <= 1)
						goto ll120;
				}
			/*------ factor of 5 ---*/
			else if (nfac[i] == 5) 
				{
					kspnn = kspan;
					kspan /= 5;
					c2 = c72 * c72 - s72 * s72;
					s2 = 2.0 * c72 * s72;
					for (; kk <= kspan; kk -= nn)
						for (; kk < nn; kk = k4 + kspan) 
							{
								k1 = kk + kspan;
								k2 = k1 + kspan;
								k3 = k2 + kspan;
								k4 = k3 + kspan;
								akp = a[k1] + a[k4];
								akm = a[k1] - a[k4];
								bkp = b[k1] + b[k4];
								bkm = b[k1] - b[k4];
								ajp = a[k2] + a[k3];
								ajm = a[k2] - a[k3];
								bjp = b[k2] + b[k3];
								bjm = b[k2] - b[k3];
								aa = a[kk];
								bb = b[kk];
								a[kk] = aa + akp + ajp;
								b[kk] = bb + bkp + bjp;
								ak = akp * c72 + ajp * c2 + aa;
								bk = bkp * c72 + bjp * c2 + bb;
								aj = akm * s72 + ajm * s2;
								bj = bkm * s72 + bjm * s2;
								a[k1] = ak - bj;
								a[k4] = ak + bj;
								b[k1] = bk + aj;
								b[k4] = bk - aj;
								ak = akp * c2 + ajp * c72 + aa;
								bk = bkp * c2 + bjp * c72 + bb;
								aj = akm * s2 - ajm * s72;
								bj = bkm * s2 - bjm * s72;
								a[k2] = ak - bj;
								a[k3] = ak + bj;
								b[k2] = bk + aj;
								b[k3] = bk - aj;
							}
				}
			/* ---- odd factors---- */
			else if(nfac[i]%2)
				{
					k = nfac[i];
					kspnn = kspan;
					kspan = kspan / k;
					if (k != jf) 
						{
							jf = k;
							s1 = rad / ((float) k / 8.0);
							c1 = cos(s1);
							s1 = sin(s1);
							ck[jf] = 1.0;
							sk[jf] = 0.0;
							for (j = 1; j < k; j++) 
								{
									ck[j] = ck[k] * c1 + sk[k] * s1;
									sk[j] = ck[k] * s1 - sk[k] * c1;
									k--;
									ck[k] = ck[j];
									sk[k] = -sk[j];
								}
						}
					okk = kk;
					for (kk = okk; kk <= kspan; kk -= nn)
						for (; kk <= nn; kk += kspnn) 
							{
								k1 = kk;
								k2 = kk + kspnn;
								aa = a[kk];
								bb = b[kk];
								ak = aa;
								bk = bb;
								j = 1;

								for (k1 += kspan; k1 < k2; k1 += kspan) 
									{
										k2 = k2 - kspan;
										j++;
										at[j] = a[k1] + a[k2];
										ak = at[j] + ak;
										bt[j] = b[k1] + b[k2];
										bk = bt[j] + bk;
										j++;
										at[j] = a[k1] - a[k2];
										bt[j] = b[k1] - b[k2];
									}
								a[kk] = ak;
								b[kk] = bk;
								k1 = kk;
								k2 = kk + kspnn;
								for (j = 1; j < k; j++) 
									{
										k1 = k1 + kspan;
										k2 = k2 - kspan;
										jj = j;
										ak = aa;
										bk = bb;
										aj = 0.0;
										bj = 0.0;
										k = 1;
									  ll280:		k = k + 1;
										ak = at[k] * ck[jj] + ak;
										bk = bt[k] * ck[jj] + bk;
										k = k + 1;
										aj = at[k] * sk[jj] + aj;
										bj = bt[k] * sk[jj] + bj;
										jj = jj + j;
										if (jj > jf)
											jj = jj - jf;
										if (k < jf)
											goto ll280;
										k = jf - j;
										a[k1] = ak - bj;
										b[k1] = bk + aj;
										a[k2] = ak + bj;
										b[k2] = bk - aj;
									}
							}
				}

			/*
			* ---multiply by rotation factor (except for factors of 2
			* and 4)-----
			*/
		  ll290:		if (i < m && nfac[i] != 2 && nfac[i] != 4) 
			  {
				  kk = 2;
			    ll300:		c2 = 1.0 - cd;
				  s1 = sd;
				  mm = min(kspan, klim);
				  goto ll320;
			    ll310:		c2 = c1 - (cd * c1 + sd * s1);
				  s1 = s1 + (sd * c1 - cd * s1);
				  c1 = 0.5 / (c2 * c2 + s1 * s1) + 0.5;
				  s1 = c1 * s1;
				  c2 = c1 * c2;
			    ll320:		c1 = c2;
				  s2 = s1;
				  for (kk += kspan; kk <= kspnn; kk += -nt + kspan) 
					  {
						  for (; kk <= nt; kk += kspnn) 
							  {
								  ak = a[kk];
								  a[kk] = c2 * ak - s2 * b[kk];
								  b[kk] = s2 * ak + c2 * b[kk];
							  }
						  ak = s1 * s2;
						  s2 = s1 * c2 + c1 * s2;
						  c2 = c1 * c2 - ak;
					  }
				  kk+= 1 - kspnn;
				  if (kk <= mm)
					  goto ll310;
				  if (kk < kspan) 
					  {
						  s1 = (kk - 1.0) * dr * rad;
						  c2 = cos(s1);
						  s1 = sin(s1);
						  mm = min(kspan, mm + klim);
						  goto ll320;
					  }
				  kk = kk - kspan + 2;
				  if (kk <= 2)
					  goto ll300;
			  }
		}
	np[1] = nc;
	if (kt >0)
		{
			k = (kt<<1)+ 1;
			if (m < k)
				k--;
			j = 1;
			np[k + 1] = 1;
			for(j=1;j<k;j++)
				{
					np[j + 1] = np[j] / nfac[j];
					np[k] = np[k + 1] * nfac[j];
					k--;
				}
			k3 = np[k + 1];
			kspan = np[2];
			kk = 2;
			k2 = kspan + 1;
			j = 1;
			/* permutation for single-variate transform */
		  ll370:		ak = a[kk];
			a[kk] = a[k2];
			a[k2] = ak;
			bk = b[kk];
			b[kk] = b[k2];
			b[k2] = bk;
			kk++;
			k2 += kspan;
			if (k2 < nc)
				goto ll370;
		  ll380:		k2-=np[j];
			j++;
			k2 += np[j + 1];
			if (k2 > np[j])
				goto ll380;
			j = 1;
		  ll390:		if (kk < k2)
			  goto ll370;
			kk++;
			k2 += kspan;
			if (k2 < nc)
				goto ll390;
			if (kk < nc)
				goto ll380;
			jc = k3;
		}
	if (2 * kt + 1 >= m)
		return;
	kspnn = np[kt + 1];
	/* permutation for square-free factors of n */

	nfac[m-kt+1] = 1;

  ll450:	for(j=m-kt;j>kt;j--) nfac[j] = nfac[j] * nfac[j + 1];
	kt++;
	nn = nfac[kt] - 1;
	jj = 0;
	j = 0;
	goto ll480;
  ll460:	jj = jj - k2;
	k2 = kk;
	k = k + 1;
	kk = nfac[k];
  ll470:	jj = kk + jj;
	if (jj >= k2)
		goto ll460;
	np[j] = jj;
  ll480:	k2 = nfac[kt];
	k = kt + 1;
	kk = nfac[k];
	j = j + 1;
	if (j <= nn)
		goto ll470;
	/* determine the permutation cycle of length greater than 1 */
	j = 0;
	goto ll500;
  ll490:	k = kk;
	kk = np[k];
	np[k] = -kk;
	if (kk != j)
		goto ll490;
	k3 = kk;
  ll500:	j = j + 1;
	kk = np[j];
	if (kk < 0)
		goto ll500;
	if (kk != j)
		goto ll490;
	np[j] = -j;
	if (j != nn)
		goto ll500;
	/* recorder a and b, following the permutation cycles */
	goto ll570;
  ll510:	j--;
	if (np[j] < 0)
		goto ll510;
	jj = jc;
  ll520:	kspan = jj;
	if (jj > maxf)
		kspan = maxf;
	jj = jj - kspan;

	k = np[j];
	kk = jc * k + i + jj;
	k1 = kk + kspan;
	for(k2=1;k1!=kk;k2++,k1--)
		{
			at[k2] = a[k1];
			bt[k2] = b[k1];
		}
  ll540:		k1 = kk + kspan;
	k2 = k1 - (k + np[k]);
	k = -np[k];
  ll550:		a[k1] = a[k2];
	b[k1] = b[k2];
	k1--;
	k2--;
	if (k1 != kk)
		goto ll550;
	kk = k2;
	if (k != j)
		goto ll540;
	k1 = kk + kspan;
	k2 = 0;
  ll560:		k2 = k2 + 1;
	a[k1] = at[k2];
	b[k1] = bt[k2];
	k1--;
	if (k1 != kk)
		goto ll560;
	if (jj != 0)
		goto ll520;
	if (j != 1)
		goto ll510;
  ll570:	j = k3 + 1;
	nt = nt - kspnn;
	i = nt;
	if (nt >= 0)
		goto ll510;

	free( at );
	free( np );
	return;

}

void FFTMX1D(a, b, n, isn)
     float *a,*b;
     int n,isn;
{
	int	nfac[16], kt, m, i;

	factor(n,nfac,&kt,&m);
	fft1dmx(a-1,b-1,n,isn,m,kt,nfac);
}

void FFTMX2D(a, b, ix, iy, isn)
     float          *a, *b;
     int             ix, iy, isn;
{
	int             nfac[16], kt, m, i;
	float         **rp, **ip, *ta, *tb;

	rp = (float **) malloc(max(ix, iy) * sizeof(float *));
	ip = (float **) malloc(max(ix, iy) * sizeof(float *));
	rp[0] = a - 1;
	ip[0] = b - 1;

	factor(ix, nfac, &kt, &m);
	for (i = 1; i < iy; i++) 
		{
			rp[i] = rp[i - 1] + ix;
			ip[i] = ip[i - 1] + ix;
		}
	fftar(rp, ip, ix, iy, isn, m, kt, nfac);
	if (ix == iy) 
		{
			factor(iy, nfac, &kt, &m);
			Transpose(a, b, ix);
			fftar(rp, ip, iy, ix, isn, m, kt, nfac);
			Transpose(a, b, iy);
		} 
	else
		{
			factor(iy, nfac, &kt, &m);
			ta = (float *) malloc(ix * iy * sizeof(float));
			tb = (float *) malloc(ix * iy * sizeof(float));
			Transpose2(a, b, ta, tb, ix, iy);
			rp[0] = ta - 1;
			ip[0] = tb - 1;
			for (i = 1; i < ix; i++) 
				{
					rp[i] = rp[i - 1] + iy;
					ip[i] = ip[i - 1] + iy;
				} 
			fftar(rp, ip, iy, ix, isn, m, kt, nfac);
			Transpose2(ta, tb, a, b, iy, ix);
			free(ta);
			free(tb);
		}
	
	free( rp );
	free( ip );

	return;
}



fftar(a, b, nc, nr, isn, m, kt, nfac)
     float         **a, **b;
     int             nc, nr, isn, m, kt, *nfac;
{
	int             nt, nn, jc, lim, klim, i, jf, kk, kspan,
		k1, k2, l;
	int             mm, k3, ins, k4, j, jj, maxf, k, kspnn, okk;
	float           rad, c72, s72, s120, ak, dr, s1, c1, cd, sd, bk,
		bj, c2, s2, c3;
	float           akp, ajp, ajm, bkp, bjm, s3, aa, bb, akm, bkm,
		aj, bjp;
	float          *at, *ck, *bt, *sk;
	int            *np;

	/*
	* FFTMX is a subroutine called by FFT to compute mixed-radix Fourier
	* Transform
	*/

	maxf = m - kt;
	maxf = nfac[maxf];
	if (kt > 0)
		maxf = max(nfac[kt], maxf);
	at = (float *) malloc(4 * (maxf + 1) * sizeof(float));
	ck = at + maxf;
	bt = ck + maxf;
	sk = bt + maxf;
	maxf = nc;
	for (i = 1; i < kt; i++)
		maxf /= (nfac[i] * nfac[i]);
	np = (int *) malloc(maxf * sizeof(int));
	nt = nc;
	rad = atan(1.0);
	s72 = rad / 0.625;
	c72 = cos(s72);
	s72 = sin(s72);
	s120 = sqrt(0.75);
	if (isn > 0) 
		{
			ak = 1.0 / nc;
			for (l = 0; l < nr; l++)
				for (j = 1; j <= nt; j++) 
					{
						a[l][j] *= ak;
						b[l][j] *= ak;
					}
		} 
	else 
		{
			s72 = -s72;
			s120 = -s120;
			rad = -rad;
		}
	kspan = nc;
	nn = nt - 1;
	jc = 1;
	/*---- sin, cos values are re-initialized each lim step ---*/
	lim = 32;
	klim = lim;
	i = 0;
	jf = 0;
	maxf = m - kt;
	maxf = nfac[maxf];
	if (kt > 0)
		maxf = max(nfac[kt], maxf);
	/* compute Fourier Transform */
	for (i = 1; i <= m; i++) 
		{
			dr = 8.0/kspan;
			cd = 1.0 - cos(dr * rad);
			sd = sin(dr * rad);
			kk = 1;
			if (nfac[i] == 2) 
				{
					/*
					* transform for factor of 2 (including rotation
					* factor)
					*/
					kspan >>= 1;
					for (k1 = kspan + 2; kk <= 1; kk -= nn)
						for (; kk <= nn; kk = k2 + kspan) 
							{
								k2 = kk + kspan;
								for (l = 0; l < nr; l++) 
									{
										ak = a[l][k2];
										bk = b[l][k2];
										a[l][k2] = a[l][kk] - ak;
										b[l][k2] = b[l][kk] - bk;
										a[l][kk] = a[l][kk] + ak;
										b[l][kk] = b[l][kk] + bk;
									}
							}
					if(kk<=kspan)
						{
							for(;kk <= 2;k1 += 2,kk = ((k1 - kspan) >> 1) + 1)
								{
									c1 = 1.0 - cd;
									s1 = sd;
									mm = min(k1 >> 1, klim);
								  ll80:				k2 = kk + kspan;
									for (l = 0; l < nr; l++) 
										{
											ak = a[l][kk] - a[l][k2];
											bk = b[l][kk] - b[l][k2];
											a[l][kk] = a[l][kk] + a[l][k2];
											b[l][kk] = b[l][kk] + b[l][k2];
											a[l][k2] = c1 * ak - s1 * bk;
											b[l][k2] = s1 * ak + c1 * bk;
										}
									kk = k2 + kspan;
									if (kk < nt)
										goto ll80;
									k2 = kk - nt;
									c1 = -c1;
									kk = k1 - k2;
									if (kk > k2)
										goto ll80;
									kk++;
									if (kk <= mm)
										{
											ak = c1 - (cd * c1 + sd * s1);
											s1 = (sd * c1 - cd * s1) + s1;
											c1 = 0.5 / (ak * ak + s1 * s1) + 0.5;
											s1 = c1 * s1;
											c1 = c1 * ak;
											goto ll80;
										}
									if (kk < k2) 
										{
											s1 = (kk - 1.0) * dr * rad;
											c1 = cos(s1);
											s1 = sin(s1);
											mm = min(k1 / 2, mm + klim);
											goto ll80;
										}
								}
						}
				}
			/* -------------------- factor of 3---- */
			else if (nfac[i] == 3) 
				{
					kspnn = kspan;
					kspan /= 3;
					for (; kk <= kspan; kk -= nn)
						for (; kk < nn; kk = k2 + kspan) 
							{
								k1 = kk + kspan;
								k2 = k1 + kspan;
								for (l = 0; l < nr; l++) 
									{
										ak = a[l][kk];
										bk = b[l][kk];
										aj = a[l][k1] + a[l][k2];
										bj = b[l][k1] + b[l][k2];
										a[l][kk] = ak + aj;
										b[l][kk] = bk + bj;
										ak = -0.5 * aj + ak;
										bk = -0.5 * bj + bk;
										aj = (a[l][k1] - a[l][k2]) * s120;
										bj = (b[l][k1] - b[l][k2]) * s120;
										a[l][k1] = ak - bj;
										b[l][k1] = bk + aj;
										a[l][k2] = ak + bj;
										b[l][k2] = bk - aj;
									}
							}
				} 
			else if (nfac[i] == 4) 
				{
					/* ------- factor of 4 -------- */
					kspnn = kspan;
					kspan = kspan / 4;
				  ll120:		c1 = 1.0;
					s1 = 0;
					mm = min(kspan, klim);
					goto ll150;
				  ll130:		c2 = c1 - (cd * c1 + sd * s1);
					s1 = (sd * c1 - cd * s1) + s1;
					c1 = 0.5 / (c2 * c2 + s1 * s1) + 0.5;
					s1 = c1 * s1;
					c1 = c1 * c2;
				  ll140:		c2 = c1 * c1 - s1 * s1;
					s2 = c1 * s1 * 2.0;
					c3 = c2 * c1 - s2 * s1;
					s3 = c2 * s1 + s2 * c1;
				  ll150:		k1 = kk + kspan;
					k2 = k1 + kspan;
					k3 = k2 + kspan;
					for (l = 0; l < nr; l++) 
						{
							akp = a[l][kk] + a[l][k2];
							akm = a[l][kk] - a[l][k2];
							ajp = a[l][k1] + a[l][k3];
							ajm = a[l][k1] - a[l][k3];
							a[l][kk] = akp + ajp;
							ajp = akp - ajp;
							bkp = b[l][kk] + b[l][k2];
							bkm = b[l][kk] - b[l][k2];
							bjp = b[l][k1] + b[l][k3];
							bjm = b[l][k1] - b[l][k3];
							b[l][kk] = bkp + bjp;
							bjp = bkp - bjp;
							if (isn < 0) 
								{
									akp = akm + bjm;
									akm = akm - bjm;
									bkp = bkm - ajm;
									bkm = bkm + ajm;
								} 
							else 
								{
									akp = akm - bjm;
									akm = akm + bjm;
									bkp = bkm + ajm;
									bkm = bkm - ajm;
								}
							if (s1 == 0.0) 
								{
									a[l][k1] = akp;
									b[l][k1] = bkp;
									a[l][k2] = ajp;
									b[l][k2] = bjp;
									a[l][k3] = akm;
									b[l][k3] = bkm;
								} 
							else 
								{
									a[l][k1] = akp * c1 - bkp * s1;
									b[l][k1] = akp * s1 + bkp * c1;
									a[l][k2] = ajp * c2 - bjp * s2;
									b[l][k2] = ajp * s2 + bjp * c2;
									a[l][k3] = akm * c3 - bkm * s3;
									b[l][k3] = akm * s3 + bkm * c3;
								}
						}
					kk = k3 + kspan;
					if (kk <= nt)
						goto ll150;
				  ll170:		kk+= 1 - nt;
					if (kk <= mm)
						goto ll130;
					if (kk < kspan) 
						{
							s1 = (kk - 1.0) * dr * rad;
							c1 = cos(s1);
							s1 = sin(s1);
							mm = min(kspan, mm + klim);
							goto ll140;
						}
					kk = kk - kspan + 1;
					if (kk <= 1)
						goto ll120;
				}
			/*------ factor of 5 ---*/
			else if (nfac[i] == 5) 
				{
					kspnn = kspan;
					kspan /= 5;
					c2 = c72 * c72 - s72 * s72;
					s2 = 2.0 * c72 * s72;
					for (; kk <= kspan; kk -= nn)
						for (; kk < nn; kk = k4 + kspan) 
							{
								k1 = kk + kspan;
								k2 = k1 + kspan;
								k3 = k2 + kspan;
								k4 = k3 + kspan;
								for (l = 0; l < nr; l++) 
									{
										akp = a[l][k1] + a[l][k4];
										akm = a[l][k1] - a[l][k4];
										bkp = b[l][k1] + b[l][k4];
										bkm = b[l][k1] - b[l][k4];
										ajp = a[l][k2] + a[l][k3];
										ajm = a[l][k2] - a[l][k3];
										bjp = b[l][k2] + b[l][k3];
										bjm = b[l][k2] - b[l][k3];
										aa = a[l][kk];
										bb = b[l][kk];
										a[l][kk] = aa + akp + ajp;
										b[l][kk] = bb + bkp + bjp;
										ak = akp * c72 + ajp * c2 + aa;
										bk = bkp * c72 + bjp * c2 + bb;
										aj = akm * s72 + ajm * s2;
										bj = bkm * s72 + bjm * s2;
										a[l][k1] = ak - bj;
										a[l][k4] = ak + bj;
										b[l][k1] = bk + aj;
										b[l][k4] = bk - aj;
										ak = akp * c2 + ajp * c72 + aa;
										bk = bkp * c2 + bjp * c72 + bb;
										aj = akm * s2 - ajm * s72;
										bj = bkm * s2 - bjm * s72;
										a[l][k2] = ak - bj;
										a[l][k3] = ak + bj;
										b[l][k2] = bk + aj;
										b[l][k3] = bk - aj;
									}
							}
				}
			/* ---- odd factors---- */
			else if(nfac[i]%2)
				{
					k = nfac[i];
					kspnn = kspan;
					kspan = kspan / k;
					if (k != jf) 
						{
							jf = k;
							s1 = rad / ((float) k / 8.0);
							c1 = cos(s1);
							s1 = sin(s1);
							ck[jf] = 1.0;
							sk[jf] = 0.0;
							for (j = 1; j < k; j++) 
								{
									ck[j] = ck[k] * c1 + sk[k] * s1;
									sk[j] = ck[k] * s1 - sk[k] * c1;
									k--;
									ck[k] = ck[j];
									sk[k] = -sk[j];
								}
						}
					okk = kk;
					for (l = 0; l < nr; l++)
						for (kk = okk; kk <= kspan; kk -= nn)
							for (; kk <= nn; kk += kspnn) 
								{
									k1 = kk;
									k2 = kk + kspnn;
									aa = a[l][kk];
									bb = b[l][kk];
									ak = aa;
									bk = bb;
									j = 1;

									for (k1 += kspan; k1 < k2; k1 += kspan) 
										{
											k2 = k2 - kspan;
											j++;
											at[j] = a[l][k1] + a[l][k2];
											ak = at[j] + ak;
											bt[j] = b[l][k1] + b[l][k2];
											bk = bt[j] + bk;
											j++;
											at[j] = a[l][k1] - a[l][k2];
											bt[j] = b[l][k1] - b[l][k2];
										}
									a[l][kk] = ak;
									b[l][kk] = bk;
									k1 = kk;
									k2 = kk + kspnn;
									for (j = 1; j < k; j++) 
										{
											k1 = k1 + kspan;
											k2 = k2 - kspan;
											jj = j;
											ak = aa;
											bk = bb;
											aj = 0.0;
											bj = 0.0;
											k = 1;
										  ll280:		k = k + 1;
											ak = at[k] * ck[jj] + ak;
											bk = bt[k] * ck[jj] + bk;
											k = k + 1;
											aj = at[k] * sk[jj] + aj;
											bj = bt[k] * sk[jj] + bj;
											jj = jj + j;
											if (jj > jf)
												jj = jj - jf;
											if (k < jf)
												goto ll280;
											k = jf - j;
											a[l][k1] = ak - bj;
											b[l][k1] = bk + aj;
											a[l][k2] = ak + bj;
											b[l][k2] = bk - aj;
										}
								}
				}

			/*
			* ---multiply by rotation factor (except for factors of 2
			* and 4)-----
			*/
		  ll290:		if (i < m && nfac[i] != 2 && nfac[i] != 4) 
			  {
				  kk = 2;
			    ll300:		c2 = 1.0 - cd;
				  s1 = sd;
				  mm = min(kspan, klim);
				  goto ll320;
			    ll310:		c2 = c1 - (cd * c1 + sd * s1);
				  s1 = s1 + (sd * c1 - cd * s1);
				  c1 = 0.5 / (c2 * c2 + s1 * s1) + 0.5;
				  s1 = c1 * s1;
				  c2 = c1 * c2;
			    ll320:		c1 = c2;
				  s2 = s1;
				  for (kk += kspan; kk <= kspnn; kk += -nt + kspan) 
					  {
						  for (; kk <= nt; kk += kspnn) 
							  {
								  for (l = 0; l < nr; l++) 
									  {
										  ak = a[l][kk];
										  a[l][kk] = c2 * ak - s2 * b[l][kk];
										  b[l][kk] = s2 * ak + c2 * b[l][kk];
									  }
							  }
						  ak = s1 * s2;
						  s2 = s1 * c2 + c1 * s2;
						  c2 = c1 * c2 - ak;
					  }
				  kk+= 1 - kspnn;
				  if (kk <= mm)
					  goto ll310;
				  if (kk < kspan) 
					  {
						  s1 = (kk - 1.0) * dr * rad;
						  c2 = cos(s1);
						  s1 = sin(s1);
						  mm = min(kspan, mm + klim);
						  goto ll320;
					  }
				  kk = kk - kspan + 2;
				  if (kk <= 2)
					  goto ll300;
			  }
		}
	np[1] = nc;
	if (kt >0)
		{
			k = (kt<<1)+ 1;
			if (m < k)
				k--;
			j = 1;
			np[k + 1] = 1;
			for(j=1;j<k;j++)
				{
					np[j + 1] = np[j] / nfac[j];
					np[k] = np[k + 1] * nfac[j];
					k--;
				}
			k3 = np[k + 1];
			kspan = np[2];
			kk = 2;
			k2 = kspan + 1;
			j = 1;
			/* permutation for single-variate transform */
		  ll370:		for (l = 0; l < nr; l++) 
			  {
				  ak = a[l][kk];
				  a[l][kk] = a[l][k2];
				  a[l][k2] = ak;
				  bk = b[l][kk];
				  b[l][kk] = b[l][k2];
				  b[l][k2] = bk;
			  }
			kk++;
			k2 += kspan;
			if (k2 < nc)
				goto ll370;
		  ll380:		k2-=np[j];
			j++;
			k2 += np[j + 1];
			if (k2 > np[j])
				goto ll380;
			j = 1;
		  ll390:		if (kk < k2)
			  goto ll370;
			kk++;
			k2 += kspan;
			if (k2 < nc)
				goto ll390;
			if (kk < nc)
				goto ll380;
			jc = k3;
		}
	if (2 * kt + 1 >= m)
		return;
	kspnn = np[kt + 1];
	/* permutation for square-free factors of n */

	nfac[m-kt+1] = 1;

  ll450:	for(j=m-kt;j>kt;j--) nfac[j] = nfac[j] * nfac[j + 1];
	kt++;
	nn = nfac[kt] - 1;
	jj = 0;
	j = 0;
	goto ll480;
  ll460:	jj = jj - k2;
	k2 = kk;
	k = k + 1;
	kk = nfac[k];
  ll470:	jj = kk + jj;
	if (jj >= k2)
		goto ll460;
	np[j] = jj;
  ll480:	k2 = nfac[kt];
	k = kt + 1;
	kk = nfac[k];
	j = j + 1;
	if (j <= nn)
		goto ll470;
	/* determine the permutation cycle of length greater than 1 */
	j = 0;
	goto ll500;
  ll490:	k = kk;
	kk = np[k];
	np[k] = -kk;
	if (kk != j)
		goto ll490;
	k3 = kk;
  ll500:	j = j + 1;
	kk = np[j];
	if (kk < 0)
		goto ll500;
	if (kk != j)
		goto ll490;
	np[j] = -j;
	if (j != nn)
		goto ll500;
	/* recorder a and b, following the permutation cycles */
	goto ll570;
  ll510:	j--;
	if (np[j] < 0)
		goto ll510;
	jj = jc;
  ll520:	kspan = jj;
	if (jj > maxf)
		kspan = maxf;
	jj = jj - kspan;

	for (l = 0; l < nr; l++) 
		{
			k = np[j];
			kk = jc * k + i + jj;
			k1 = kk + kspan;
			for(k2=1;k1!=kk;k2++,k1--)
				{
					at[k2] = a[l][k1];
					bt[k2] = b[l][k1];
				}
		  ll540:		k1 = kk + kspan;
			k2 = k1 - (k + np[k]);
			k = -np[k];
		  ll550:		a[l][k1] = a[l][k2];
			b[l][k1] = b[l][k2];
			k1--;
			k2--;
			if (k1 != kk)
				goto ll550;
			kk = k2;
			if (k != j)
				goto ll540;
			k1 = kk + kspan;
			k2 = 0;
		  ll560:		k2 = k2 + 1;
			a[l][k1] = at[k2];
			b[l][k1] = bt[k2];
			k1--;
			if (k1 != kk)
				goto ll560;
		}
	if (jj != 0)
		goto ll520;
	if (j != 1)
		goto ll510;
  ll570:	j = k3 + 1;
	nt = nt - kspnn;
	i = nt;
	if (nt >= 0)
		goto ll510;

	free( at );
	free( np );
	return;

}

