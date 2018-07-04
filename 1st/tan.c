#include <stdio.h>
#include <math.h>

#define Pl 3.1415926

int main(void)
{

	double ang, rst;

	scanf("%lf", &ang);

	rst = tan(ang*Pl/180.0);
	printf("tan %.1lf(deg) = %.3lf\n", ang, rst);
}

