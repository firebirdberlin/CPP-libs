//		./demos/gipl_demo.cpp
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

const int NX=40;
const int NY=20;
const int NZ=5;

typedef struct test{
	double u;
	double v;
} Test;

int main(int argc, char** argv)
{
	//! schreibe gipl datei mit der u Konzentration, diese kann mit meshview dargestellt werden
	GIPL *out = new GIPL();

	out->dims(NX,NY,NZ); 		// Anzahl elemente in x,y,z Richtung
	out->origin(0.,0.0,0.);		// Nullpunkt in realen koordinaten
	out->pixdim(0.1, 0.1, 0.1);	// Pixel spacing in realen K.
	out->set_minmax(0., NX/2.);	// minimum and maximum values in the file

	out->image_type(GIPL_DOUBLE); // Datentyp der Daten : WICHTIG

	out->infoline("State variable u - Oregonator model 3k");

	/** \note you may want to append a different data array to the file. This
	 * can be done if it is of the same type. This technique is not GIPL alike
	 *  and therefore most programm will only read the first array.
	 **/
	out->num_data_arrays(2); //!< increase the value of stored arrays
	FILE *f = out->save_header("Test.gipl");

	// first data array
	if (f){
		//!attention Reihenfolge erst z, dann y dann x !!!!
		for(int z = 0; z < NZ; z++){
		for(int y = 0; y < NY; y++){
		for(int x = 0; x < NX; x++){
			double value = (double) (x);
			if ((x > NX/2))
			 //|| (y > NY/2) ||(z > NY/2) )
				value = NX - x;
			fwrite( &value,	sizeof(double),	1, f);
		}}}
	}

	// second data array
	if (f){ /// ... and append a second one
		//!attention Reihenfolge erst z, dann y dann x !!!!
		for(int z = 0; z < NZ; z++){
		for(int y = 0; y < NY; y++){
		for(int x = 0; x < NX; x++){
			double value = (double) (x);
			value = NY;
			fwrite( &value,	sizeof(double),	1, f);
		}}}
	}

	fclose(f);

//////////////// some exmaple for saving a user defined type ///////////////////
	// some user defined type
	Test *data = new Test[out->items()];
	for (size_t i = 0; i < out->items(); i++) {
		data[i].u = (double) i;
		data[i].v = (double) i*10;
	}

	// write it to a file
	out->image_type(GIPL_USER_DEFINED); //! set image type
	out->size_of_item(sizeof(Test));	//! set the size needed

	//! Since the filetype is user defined, it is quite nice to tell something
	//! about it here.
	out->infoline("Datatype: Test, size of data type is found in userdef1");
	out->num_data_arrays(1);
	//! the rest of the header was already defined in the above example
	f = out->save_header("Test2.gipl");
	cout << "Header size " << out->header_size() << endl;
	fwrite( data,	sizeof(Test), out->items(), f);
	fclose(f);

	// save as text file (please compare the file sizes !)
	f = fopen("Test2.txt", "w");
	for (uint i = 0; i < out->items(); i++){
		fprintf(f,"%lf %lf\n", data[i].u, data[i].v);
	}
	fclose(f);
	delete[] data;
	delete out;


	return 0;
}
