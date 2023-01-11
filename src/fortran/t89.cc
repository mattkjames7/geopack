#include "header.h"

/* these parameters appear in Tsyganenko's original Fortran code, I have
 * attempted to work out how they correspond to the T89 paper  
 * (https://doi.org/10.1016/0032-0633(89)90066-4) */
double _Tail0[] = 		{    -116.53,    -55.553,    -101.34,    -181.69,    -436.54,    -707.77,    -1190.4}; //Tail current
double _Tail1[] = 		{     -10719,     -13198,     -13480,     -12320,      -9001,    -4471.9,     2749.9}; //Tail current
double _AmpClsSym[] = 	{     42.375,     60.647,     111.35,     173.79,     323.66,     432.81,     742.56}; //symmetric closure current
double _AmpClsAsym[] = 	{     59.753,     61.072,     12.386,    -96.664,    -410.08,    -435.51,    -1110.3}; //asymmetric closure current
double _AmpRC[] = 		{     -11363,     -16064,     -24699,     -39051,     -50340,     -60400,     -77193}; //ring current
double _C6[] = 			{     1.7844,     2.2534,     2.6459,     3.2633,     3.9932,     4.6229,     7.6727}; //C_06 These are CF and Birkeland current field parameters
double _C7[] = 			{     30.268,     34.407,     38.948,     44.968,     58.524,     68.178,     102.05}; //C_07
double _C8[] = 			{  -0.035372,  -0.038887,   -0.03408,  -0.046377,  -0.038519,  -0.088245,  -0.096015}; //C_08
double _C9[] = 			{  -0.066832,  -0.094571,   -0.12404,   -0.16686,   -0.26822,   -0.21002,   -0.74507}; //C_09
double _C10[] =			{   0.016456,   0.027154,   0.029702,   0.048298,   0.074528,    0.11846,    0.11214}; //C_10
double _C11[] = 		{    -1.3024,    -1.3901,    -1.4052,    -1.5473,    -1.4268,    -2.6711,    -1.3614}; //C_11
double _C12[] = 		{  0.0016529,   0.001346,  0.0012103,  0.0010277, -0.0010985,  0.0022305,  0.0015157}; //C_12
double _C13[] = 		{  0.0020293,  0.0013238,  0.0016381,  0.0031632,  0.0096613,    0.01091,   0.022283}; //C_13
double _C14[] = 		{     20.289,     23.005,      24.49,     27.341,     27.557,     27.547,     23.164}; //C_14
double _C15[] = 		{  -0.025203,  -0.030565,  -0.037705,  -0.050655,  -0.056522,   -0.05408,  -0.074146}; //C_15
double _TailTilt0[] = 	{     224.91,     55.047,    -298.32,     -514.1,    -867.03,    -424.23,    -2219.1}; //Tail current tilt angle
double _TailTilt1[] = 	{    -9234.8,    -3875.7,     4400.9,      12482,      20652,     1100.2,      48253}; //tail current tilt angle
double _deltax[] = 		{     22.788,     20.178,     18.692,     16.257,     14.101,     13.954,     12.714}; // \delta x
double _a_RC[] = 		{     7.8813,     7.9693,     7.9064,     8.5834,     8.3501,     7.5337,     7.6777}; // a_RC
double _D_0[] = 		{     1.8362,     1.4575,     1.3047,     1.0194,    0.72996,    0.89714,    0.57138}; // D_0
double _gamma_RC[] = 	{   -0.27228,    0.89471,     2.4541,     3.6148,     3.8149,     3.7813,     2.9633}; // gamma_RC
double _R_c[] = 		{     8.8184,     9.4039,     9.7012,     8.6042,     9.2908,     8.2945,     9.3909}; // R_c
double _G[] = 			{     2.8714,     3.5215,     7.1624,     5.5057,     6.4674,      5.174,     9.7263}; // G
double _a_T[] = 		{     14.468,     14.474,     14.288,     13.778,     13.729,     14.213,     11.123}; // a_T - characteristic radius of the tail current
double _D_y[] = 		{     32.177,     36.555,     33.822,     32.373,     28.353,     25.237,     21.558}; // D_y - characteristic scale distance in Y
double _delta[] = 		{       0.01,       0.01,       0.01,       0.01,       0.01,       0.01,       0.01}; // Delta - thickening rate of tail current sheet
double _Q[] = 			{          0,          0,          0,          0,          0,          0,          0}; // Q - set to 0
double _X_0[] = 		{     7.0459,     7.0787,     6.7442,     7.3195,     7.4237,     7.0037,     4.4518}; // x_0
double _gamma_T[] = 	{          4,          4,          4,          4,          4,          4,          4}; // \gamma_T
double _D_yc[] = 		{         20,         20,         20,         20,         20,         20,         20}; // D_yc (equation 19) - fixed at 20

void t89(	int iopt, double *parmod, double psi,
			double x, double y, double z, 
			double *bx, double *by, double *bz) {

	/* set the parameters based on iopt (Kp + 1)*/
	int Kp = iopt - 1;
	if (Kp < 0) {
		Kp = 0;
	} else if (Kp > 6) {
		Kp = 6;
	}
	double Tail0 = _Tail0[Kp];
	double Tail1 = _Tail1[Kp];
	double AmpClsSym = _AmpClsSym[Kp];
	double AmpClsAsym = _AmpClsAsym[Kp];
	double AmpRC = _AmpRC[Kp];
	double C6 = _C6[Kp];
	double C7 = _C7[Kp];
	double C8 = _C8[Kp];
	double C9 = _C9[Kp];
	double C10 = _C10[Kp];
	double C11 = _C11[Kp];
	double C12 = _C12[Kp];
	double C13 = _C13[Kp];
	double C14 = _C14[Kp];
	double C15 = _C15[Kp];
	double TailTilt0 = _TailTilt0[Kp];
	double TailTilt1 = _TailTilt1[Kp];
	double deltax = _deltax[Kp];
	double a_RC = _a_RC[Kp];
	double D_0 = _D_0[Kp];
	double gamma_RC = _gamma_RC[Kp];
	double R_c = _R_c[Kp];
	double G = _G[Kp];
	double a_T = _a_T[Kp];
	double D_y = _D_y[Kp];
	double delta = _delta[Kp];
	double Q = _Q[Kp];
	double X_0 = _X_0[Kp];
	double gamma_T = _gamma_T[Kp];
	double D_yc = _D_yc[Kp];

	/* get SM coordinates using dipole tilt */
	double tps, sps, cps, xsm, zsm;
	smCoords(x,z,psi,&sps,&cps,&tps,&xsm,&zsm);
 
	/* get the tail current sheet shape */
	double zs, double dzsdx, double dzszy;
	tailCurrentSheetShape(xsm,y,sps,tps,R_c,G,&zs,&dzsdx,&dzsdy);

	/* get the ring current contribution to the field */
	double BxRC, double ByRC, double BzRC;
	ringCurrentComps(xsm,y,zsm,cps,sps,zs,dzsdx,dzsdy,D_0,
						gamma_RC,a_RC,AmpRC,
						&BxRC,&ByRC,&BzRC);
}


void smCoords(	double x, double z, double psi,
				double *sps, double *cps, double *tps,
				double *xsm, double *zsm) {

	
	*sps = sin(psi);
	*cps = cos(psi);
	*tps = (*sps)/(*cps);
	*xsm = x*(*cps) - z*(*sps);
	*zsm = x*(*sps) + z*(*cps);

}

void tailCurrentSheetShape(	double xsm, double ysm, 
							double sps, double tps,
							double R_c, double G,
							double *Zs, double *dZsdx, double *dZsdy) {

	/* equation 11 and it's derivatives wrt x and y */
	double y2 = ysm*ysm;
	double y4 = y2*y2;
	double Ly4 = 10000.0;
	double xrc = xsm + R_c;
	double xrc2 = xrc*xrc;
	double y4Ly4 = y4 + Ly4;
	*Zs = 0.5*tps*(xrc - sqrt(xrc2 + 16.0)) - (G*sps*y4)/y4Ly4;
	
	*dZsdx = 0.5*tps*(1.0 - xrc/sqrt(xrc2 + 16.0));

	*dZsdy = -(4.0*G*Ly4*y2*ysm*sps)/(y4Ly4*y4Ly4);

}


void ringCurrentComps(	double x, double y, double z,
						double cps, double sps,
						double zs, double dzsdx, double dzsdy,
						double D_0, double gamma_RC, 
						double a_RC, double C_3,
						double *Bx, double *By, double *Bz) {
	/* ring current contribution from eqs 13, 16 and 17
	* the output needs to be multiplied by C_3, I think
	* also, the original code calls the components 
	* DBXDP, DBZDP, - are they derivative wrt rho?!
	* a bit confusing for me... */

	/* define a few things that get used more than once and
	that are used as intermediate steps*/
	double Lrc = 5;
	double Lrc2 = 25;
	double x2 = x*x;
	double y2 = y*y;
	double rho2 = x2 + y2;
	double zr = z - zs;
	double x2L2 = sqrt(x2 + Lrc2);
	
	/* calculate the current sheet thickness, I think (eq 13) 
	for some reason gamma_1*h_1 term is missed out in the code*/
	double h = 0.5*(1 + x/x2L2);
	double D = D_0 + gamma_RC*h;
	double dhdx = 0.5*Lrc2/(x2L2*x2L2*x2L2);
	double dDdx = gamma_RC*dhdx;
	
	/* more bits of eq 13*/
	double xi = sqrt(zr*zr + D*D);
	double a_xi = a_RC + xi;
	double a_xi2 = a_xi*a_xi;
	double S = sqrt(rho2 + a_xi2);
	double S2 = S*S;
	double S5 = S*S2*S2;

	/* part of eq 17, I think there might be a typo in the paper,
	the equation has x z_r at the end, but it doesn't appear in
	the code. Also, C_3 is missing from the code, but I think that 
	is multiplied by each component at the end.*/
	double Q = (3*a_xi)/(S5*xi);

	/* get the SM components */
	double Bxsm = Q*x*zr;
	*By = Q*y*zr;
	double Bzsm = (2*a_xi2 - rho2)/S5 + Bxsm*dzsdx + (*By)*dzsdy - Q*D*x*dDdx;

	/* convert to GSM and multiply by ring current amplitude, I think*/
	*Bx = C_3*(Bxsm*cps + Bzsm*sps);
	*By = (*By)*C_3;
	*Bz = C_3*(Bzsm*cps - Bxsm*sps);

}


void tailCurrentField() {

	/* this uses equations 13,14 and 15*/
	double x2 = x*x;
	double y2 = y*y;
	double rho2 = x2 + y2;
	double zr = z - zs;
	double L2 = 40.0;
	double x2L2 = x*x + L2;
	double rtx2L2 = sqrt(x2L2);

	/* h_T and its derivatives */
	double h = 0.5*(1 + x/rtx2L2);
	double dhdx = 0.5*L2/(x2L2*rtx2L2);

	/* D_T and its derivatives */
	double D = D_0 + delta*y2 + gamma_T*h;
	double dDdx = gamma_T*dhdx;
	double dDdy = 2*delta*y;

	/* xi_T and S_T */
	double xi = sqrt(zr*zr + D*D);
	double atxi = a_T + xi;
	double S = sqrt(rho2 + atxi*atxi);

	/* W(x,y) and its derivatives */
	double xx0 = x - x_0;
	double xx02 = xx0*xx0;
	double Dx2 = 170.0;
	double x2D2 = xx02 + Dx2;
	double rtx2D2 = sqrt(x2D2);
	double Dy2 = D_y*D_y;
	double y2D21 = 1 + y2/Dy2;
	double W = 0.5*(1 - xx0/rtx2D2)/y2D21;
	double dWdx = -Dx2/(2*y2D21*rtx2D2*x2D2);
	double dWdy = -0.5*(1-xx0/rtx2D2)*(2*y)/(Dy2*y2D21*y2D21);

	/* Q_T, equation 15 */
	double Q = (W/(xi*S))*((C1/(S + a_T + xi)) + (C2/(S*S)));

	/* the field components in SM */



}

void tailClosureCurrent() {
/* it appears that lines 398-428 and 468-470 correspond to
the tail closure current sheets at z = +/- 30 R_E in 
equations 18 and 19 */



}

void cfClosureCurrent(	double x, double y, double z, double deltax,
						double C6, double C7, double C8, double C9,
						double C10, double C11, double C12, double C13,
						double C14, double C15, double cps, double sps,
						double *Bx, double *By, double *Bz) {

	/* calculate the missing coefficients */
	double C16 = -0.5*(C6/deltax + C10);
	double C17 = -(C7/deltax + C11);
	double C18 = -(C8/deltax + 3*C12);
	double C19 = -(C9/deltax + C13)/3.0;

	/* get a few of the terms that we need */
	double exdx = exp(x/deltax);
	double y2 = y*y;
	double y3 = y*y2;
	double z2 = z*z;
	double z3 = z*z3;
	double yz2 = y*z2;
	double zcosp = z*cps;
	double yzcosp = y*zcosp;

	/* now equations 20 */
	*Bx = exdx*(C6*zcosp + (C7 + C8*y2 + C9*z2)*sps));
	*By = exdx*(C10*yzcosp + (C11*y + C12*y3 + C13*yz2)*sps);
	*Bz = exdx*((C14 + C15*y2 + C16*z2)*cps + (C17*z + C18*zy2 + C19*z3)*sps);

}


