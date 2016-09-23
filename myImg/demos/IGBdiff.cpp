//      dat2IGB.cpp
//
//      Copyright 2011 Stefan Fruhner <stefan.fruhner@gmail.com>
//
//      This program is free software; you can redistribute it and/or modify
//      it under the terms of the GNU General Public License as published by
//      the Free Software Foundation; either version 2 of the License, or
//      (at your option) any later version.
//
//      This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY; without even the implied warranty of
//      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//      GNU General Public License for more details.
//
//      You should have received a copy of the GNU General Public License
//      along with this program; if not, write to the Free Software
//      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
//      MA 02110-1301, USA.


#include <iostream>

#include <mylibs/Profiler.hpp>
#include <mylibs/myinifiles.hpp>
#include <mylibs/mystring.hpp>
#include <mylibs/cmdline.hpp>
#include <myImg/IGBdata.hpp>
#include "../IGBheader.hpp"

using namespace std;
using namespace mylibs;


/** naem:eval(size_t sz, gzFile in1, gzFile in2, gzFile ou)
 *
 * 	Evaluates the differences of full slices in the igb files.
 **/
template<class T>
void eval(size_t sz, gzFile in1, gzFile in2, gzFile ou){

	T v1[sz];
	T v2[sz];
	float o[sz];
	gzread(in1, v1, sizeof(T)* sz );
	gzread(in2, v2, sizeof(T)* sz );
	for (size_t i = 0; i < sz; i++){
		o[i] = (float) v1[i] - (float) v2[i];
	}

	gzwrite(ou, o, sizeof(float) * sz);
}

int main(int argc, char **argv){

	myIniFiles ini(argc, argv);

	ini.register_param("input" , "i", "[=file name] Input data, should be a text file, one value per line");
	ini.register_param("second", "j", "[=file name] Input data, second file.");
	ini.register_param("output", "o", "[=file name] Output data file");
	ini.register_flag("verbose", "v", "switch on verbosity messages");

/////////////////////////// Reading of params //////////////////////////////////
	bool verbose 	  = ini.exists("verbose");
	mystring input1  = ini.read("input" , mystring());
	mystring input2  = ini.read("second", mystring());
	mystring output("diff.igb.gz");
	output 			  = ini.read("output", output);
	if (not output.endswith(".igb.gz")) output+=".igb.gz";

/////////////////////////// Error checking /////////////////////////////////////
	if ( not input1.file_exists() ) cmdline::exit("input file 1 doesn't exist.");
	if ( not input2.file_exists() ) cmdline::exit("input file 2 doesn't exist.");

////////////////////////////////////////////////////////////////////////////////
	#define MAXFRAMES 10
	gzFile f1 = gzopen(input1.c_str(), "r");
	gzFile f2 = gzopen(input2.c_str(), "r");
	gzFile ou = gzopen(output.c_str(), "w");

	IGBheader igbh1(f1);
	IGBheader igbh2(f2);
	IGBheader igbou(igbh1);
	igbou.type(IGB_FLOAT); // output is always evaluated to be a float

	if (not igbh1.compatible(igbh2))
		cmdline::exit("The input files are not compatible !");

	if (verbose) igbou.info();

	igbou.fileptr(ou);
	igbou.comment("difference signal");
	igbou.write(); // the header

	size_t sz = igbh1.slicesize();
	Profiler profiler(igbh1.slices());
	for (size_t slice = 0; slice < igbh1.slices(); slice++){
		int datatype = igbh1.type();
		switch (datatype){
		case IGB_BYTE: 		eval<unsigned char>(sz, f1,f2, ou); break;
		case IGB_CHAR: 		eval<char>			(sz, f1,f2, ou); break;
		case IGB_SHORT: 	eval<short>			(sz, f1,f2, ou); break;
		case IGB_LONG: 		eval<long>			(sz, f1,f2, ou); break;
		case IGB_FLOAT: 	eval<float>			(sz, f1,f2, ou); break;
		case IGB_DOUBLE: 	eval<double>		(sz, f1,f2, ou); break;
		case IGB_INT: 		eval<int >			(sz, f1,f2, ou); break;
		case IGB_UINT: 		eval<unsigned int>	(sz, f1,f2, ou); break;
		case IGB_STRUCTURE:
		case IGB_COMPLEX:
		case IGB_D_COMPLEX:
		case IGB_RGBA:
		case IGB_POINTER:
		case IGB_LIST:
		default: {cerr << "Datatype " << IGBheader::TypeID2Name(datatype)
					  << " currently not implemented." << endl;
					exit(-2);
				}
		}
		profiler.info();
	}
	profiler.finalize();

	gzclose(f1);
	gzclose(f2);
	gzclose(ou);


	return 0;
}
