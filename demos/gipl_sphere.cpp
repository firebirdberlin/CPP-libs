//		./demos/gipl_sphere.cpp
//
//		Copyright 2011 Stefan Fruhner <stefan.fruhner@gmail.com>
//
//		This program is free software; you can redistribute it and/or modify
//		it under the terms of the GNU General Public License as published by
//		the Free Software Foundation; either version 2 of the License, or
//		(at your option) any later version.
//
//		This program is distributed in the hope that it will be useful,
//		but WITHOUT ANY WARRANTY; without even the implied warranty of
//		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//		GNU General Public License for more details.
//
//		You should have received a copy of the GNU General Public License
//		along with this program; if not, write to the Free Software
//		Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
//		MA 02110-1301, USA.
/*
 *      gipl_demo.cpp
 * 		Demo program how to write gipl files.
 * 		g++ gipl_demo.cpp ../gipl.cpp -o gipl_demo -Wall -I..
 */

#include <iostream>
#include <mylibs/gipl.h>

#define print(x) (cout << __FILE__ << ":" << __FUNCTION__ <<"() (" <<__LINE__ << ") : " << x << endl)

const unsigned short NX=40;
const unsigned short NY=40;
const unsigned short NZ=40;

// center of the sphere
const unsigned short CX=20;
const unsigned short CY=20;
const unsigned short CZ=20;

//radius
const unsigned short Radius = 15;

int main(int argc, char** argv){
	//! schreibe gipl datei mit der u Konzentration, diese kann mit meshview dargestellt werden
	GIPL *out = new GIPL();

	out->dims(NX,NY,NZ); 		// Anzahl elemente in x,y,z Richtung
	out->origin(0.,0.,0.);		// Nullpunkt in realen koordinaten
	out->pixdim(0.1, 0.1, 0.1);	// Pixel spacing in realen K.
	out->set_minmax(0., NX/2.);	// minimum and maximum values in the file

	out->image_type(GIPL_U_SHORT); // Datentyp der Daten : WICHTIG

	out->infoline("A sphere embedded in a bath.");

	/** \note you may want to append a different data array to the file. This
	 * can be done if it is of the same type. This technique is not GIPL alike
	 *  and therefore most programm will only read the first array.
	 **/

	FILE *f = out->save_header("Sphere.gipl");


	if (f){
		//!attention Reihenfolge erst z, dann y dann x !!!!
		for(unsigned short z = 0; z < NZ; z++){
		for(unsigned short y = 0; y < NY; y++){
		for(unsigned short x = 0; x < NX; x++){

			unsigned short value = (x-CX)*(x-CX) + (y-CY)*(y-CY) + (z-CZ)*(z-CZ);
			unsigned short val = 0;
			if (value <= Radius * Radius)	val = 20;

			fwrite( &val,	sizeof(unsigned short),	1, f);
		}}}
	}

	fclose(f);

	delete out;

	return 0;
}
