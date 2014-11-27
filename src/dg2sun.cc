
#include <math.h>

float dg2sun(int hi, int lo){
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
 
