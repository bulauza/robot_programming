#include <stdio.h>
#include <math.h>

#define Pl 3.1415926

int main(void)
{

	double ang, rst;

	scanf("%lf", &ang);

	rst = sin(ang*Pl/180.0);
	printf("sin %.1lf(deg) = %.3lf\n", ang, rst);
}

