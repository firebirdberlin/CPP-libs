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

#include <mylibs/myinifiles.hpp>
#include <mylibs/mystring.hpp>
#include <mylibs/cmdline.hpp>
#include "../IGBheader.hpp"

using namespace std;
using namespace mylibs;

template<class T>
void convert(ifstream &infile, gzFile ou){
	T val;
	char c, str[256];
//	stringstream c;
	while (infile.good()){     // loop while extraction from file infile possible
//		c = infile.get();       // get character from file
//		c >> val;
		infile >> val;
//		fwrite(&val, sizeof(T), 1, ou);
		gzwrite(ou, &val, sizeof(T));

	}
}

int main(int argc, char **argv){

	myIniFiles ini(argc, argv);

	ini.register_param("input",  "i", "[=file name] Input data, should be a text file, one value per line");
	ini.register_param("output", "o", "[=file name] Output data file");

	mystring IDs = "[";
	for (size_t i = IGB_MIN_TYPE; i < IGB_MAX_TYPE ; ++i)
		IDs += "'"+ IGBheader::TypeID2Name(i) + "', ";

	IDs = IDs.slice(0,-2) + "]";
	ini.register_param("type",   "t", "Output data type " + IDs);
	ini.register_param("unit",   "u", "The unit of the data, e.g. ms, mV, ...");
	ini.register_param("dim",    "d", "The dimension of the data.");

/////////////////////////// Reading of params //////////////////////////////////
	mystring input    = ini.read("input" , mystring());
	mystring output(input);
	output.file_new_ext(".igb"); output += ".gz";
	output 			  = ini.read("output", output);
	if (not output.endswith(".igb.gz")) output+=".igb.gz";
	mystring typeName = ini.read("type", mystring("IGB_NONE"));
	mystring unit	  = ini.read("unit", mystring(""));

	vector<int> dimension;
	ini.read("dim", dimension);

/////////////////////////// Error checking /////////////////////////////////////
	if ( not input.file_exists() ) cmdline::exit("input file doesn't exist.");

	int datatype = IGBheader::Name2TypeID(typeName);
	if (datatype == -1) cmdline::exit(   "Datatype '" + toString(datatype)
										+"' unknown. Datatype must be out of \n"
										+ IDs);
	if (dimension.size() > 0 && dimension.size() != 4) {
		cmdline::exit("Error: Dimension must have 4 values for x,y,z,t !!!");
	}

////////////////////////////////////////////////////////////////////////////////

	ifstream infile;
	infile.open (input.c_str(), ifstream::in);
	size_t nr_lines=0;
	string line;
    while( getline(infile, line) ) nr_lines++;
    infile.seekg(0);
	infile.close();

	infile.open (input.c_str(), ifstream::in);
	IGBheader igb;
	gzFile ou = gzopen(output.c_str(),"w");
	igb.fileptr(ou);
	if (dimension.size() == 0){
		igb.dim(nr_lines);
	} else {

		cout << "Dimension settings" << endl;
		for (vector<int>::iterator it = dimension.begin(); it != dimension.end(); it++){
			cout << " | " << *it << endl;
		}

		igb.dim(dimension[0], dimension[1], dimension[2], dimension[3]);
	}

	igb.unites(unit.c_str());
	igb.comment("written by dat2IGB (C) 2013 Stefan Fruhner");
	igb.type(datatype);

	igb.write(); // create a file with header

	switch (datatype){
	case IGB_BYTE: 		convert<unsigned char>	(infile, ou); break;
	case IGB_CHAR: 		convert<char>			(infile, ou); break;
	case IGB_SHORT: 	convert<short>			(infile, ou); break;
	case IGB_LONG: 		convert<long>			(infile, ou); break;
	case IGB_FLOAT: 	convert<float>			(infile, ou); break;
	case IGB_DOUBLE: 	convert<double>		(infile, ou); break;
	case IGB_INT: 		convert<int >			(infile, ou); break;
	case IGB_UINT: 		convert<unsigned int>	(infile, ou); break;
	case IGB_COMPLEX:
	case IGB_D_COMPLEX:
	case IGB_RGBA:
	case IGB_STRUCTURE:
	case IGB_POINTER:
	case IGB_LIST:
	default: {cerr << "Datatype " << IGBheader::TypeID2Name(datatype)
				  << " currently not implemented." << endl;
				exit(-2);
			}
	}

	gzclose(ou);
	infile.close();


	return 0;
}
