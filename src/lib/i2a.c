#include <i2a.h>

void ui2a(int num, int base, int *size, char*bf){
	int n = 0;
	int dgt;
	unsigned int d = 1;
	
	while( (num / d) >= base ) d *= base;
	while( d != 0 ) {
		dgt = num / d;
		num %= d;
		d /= base;
		if( n || dgt > 0 || d == 0 ) {
			*bf++ = dgt + ( dgt < 10 ? '0' : 'a' - 10 );
			++n;
		}
	}
	*bf = 0;
	*size = n;
}

void i2a(int num, int *size, char*bf){
	*size = 0;
	if(num < 0){
		num = -num;
		*bf++ = '-';
		*size = 1;
	}

	int usize;
	ui2a(num, 10, &usize, bf);
	*size = usize + *size;
}
