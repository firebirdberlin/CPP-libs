/*
 * regression.c
 *
 * Example program for linear regression of a derivative.
 * gcc -o regression regression.c `gsl-config --cflags` `gsl-config --libs`
 *
 * Copyright 2012 Stefan Fruhner <stefan.fruhner@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 *
 */


#include <stdio.h>
#include <gsl/gsl_fit.h>

int main(int argc, char **argv)
{
	double cov11 = 0.;
	double sumsq = 0.;
	double c1 = 0.;

	//double dx[5]  = {2.3,2.4,2.1,2.6,3.};
	//double dfx[5] = {10, 12, 15, 8, 16};

	double dx[7]  = {2.3,2.4,2.1, 0., 0.,2.6,3.};
	double dfx[7] = {10, 12, 15, 0., 0.,8, 16};

	gsl_fit_mul(&dx[0],1,&dfx[0],1, 7, &c1, &cov11, &sumsq);

	size_t i = 0;
	for (i = 0; i < 7; i++){
		printf("dfx[%d]/dx[%d] = %02.4lf/%2.4lf =  %lf \n", (int)i,(int)i, dfx[i],dx[i], dfx[i]/dx[i]);
	}
	printf("---------------------------\n");
	printf("Y = c1 X ==> c1 = %lf\n", c1);
	printf("cov11 = %lf\n", cov11); // variance of c1
	printf("sumsq = %lf\n", sumsq);
	return 0;
}

