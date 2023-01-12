#include <stdio.h>
#define _USE_MATH_DEFINES
#include <geopack.h>
#include <math.h>

bool testT89();

int main() {

	printf("Testing T89\n");
	testT89();
}


bool testT89() {

	/* create some test position vectors */
	double x[] = {5.0,10.0,-10.0,-5.0};
	double y[] = {0.0,-4.0,3.0,-2.0};
	double z[] = {1.0,0.0,-2.0,-1.0};
	double psi[] = {0.0,0.35,-0.2,0.1};
	int iopt[] = {1,3,5,6};
	double parmod[10];

	/* variables to store field vectors */
	double Bxc[4], Byc[4], Bzc[4];
	double Bxf[4], Byf[4], Bzf[4];

	/* get the vectors */
	int i;
	for (i=0;i<4;i++) {
		t89c_(&iopt[i],parmod,&psi[i],&x[i],&y[i],&z[i],&Bxf[i],&Byf[i],&Bzf[i]);
		t89(iopt[i],parmod,psi[i],x[i],y[i],z[i],&Bxc[i],&Byc[i],&Bzc[i]);
	}

	/* compare them */
	for (i=0;i<4;i++) {
		printf("dx: %f, dy: %f, dz: %f\n",Bxf[i]-Bxc[i],Byf[i]-Byc[i],Bzf[i]-Bzc[i]);
	}

	return true;
}

