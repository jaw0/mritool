
#include <math.h>

float dg2sun( hi, lo)
	int hi, lo;{
	float foo,bar;
	int i;
	int expon;


	expon = ((hi&0x7F00)>>8)-64;
	foo = pow(16.0, (float)expon);
	if (hi&(1<<15)) foo = -foo;

	i = (lo&0xFFFF)|((hi&0xFF)<<16);
	bar = ((float)i)/((float)(1<<24));

	return (foo*bar);

}
 
#ifdef SLOW_OLD_YUCKY_WAY_TO_DO_IT
	expon = -64;
	for(i=0;i<7;i++)
		if(hi&(1<<(i+8))) expon+=1<<i;
	temp=pow(16.0,expon);
	if(hi&(1<<15)) temp=-temp;
	temp2=0;
	for(i=0;i<8;i++) 
		if(hi&(1<<(7-i))) temp2+=1.0/(1<<(i+1));
	for(i=0;i<16;i++) 
		if(lo&(1<<(15-i))) temp2+=1.0/(1<<(i+9));
	return(temp*temp2);
#endif



