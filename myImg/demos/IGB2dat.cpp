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
#include <zlib.h>
#include <mylibs/myinifiles.hpp>
#include <mylibs/mystring.hpp>
#include <mylibs/cmdline.hpp>
#include "../IGBheader.hpp"

using namespace std;
using namespace mylibs;

template<class T>
void convert(gzFile in, ofstream& o, IGBheader &igb){
	T val;

	while(gzread(in, &val, sizeof(val)) > 0){
		o << val << endl;
	}

//	char c, str[256];
////	stringstream c;
//	while (infile.good()){     // loop while extraction from file infile possible
////		c = infile.get();       // get character from file
////		c >> val;
//		infile >> val;
//		fwrite(&val, sizeof(T), 1, ou);

//	}
}

void convert_structure(gzFile in, ofstream &ou, IGBheader &igb){
	mystring sd(igb.struct_desc());

	sd = "3 double 2 float 4 int";

	vector<int> numbers;
	vector<string> strings;
	vector<mystring> types;

	sd.extract(numbers);
	sd.extract(strings);


	for (vector<string>::iterator it = strings.begin(); it != strings.end(); ++it){
		cout << *it << endl;
		if (*it== "float") types.push_back(*it);
		else
		if (*it== "double") types.push_back(*it);
		else
		if (*it== "char") types.push_back(*it);
		else
		if (*it== "int") types.push_back(*it);
		else
		if (*it== "uchar") types.push_back(*it);
		else
		if (*it== "uint") types.push_back(*it);
		else
		if (*it== "short") types.push_back(*it);
		else
		if (*it== "long") types.push_back(*it);
	}

	for (vector<int>::iterator it = numbers.begin(); it != numbers.end(); ++it)
		cout << *it << endl;

	for (vector<mystring>::iterator it = types.begin(); it != types.end(); ++it)
		cout << *it << endl;


//	T val;
//	char c, str[256];
////	stringstream c;
//	while (infile.good()){     // loop while extraction from file infile possible
////		c = infile.get();       // get character from file
////		c >> val;
//		infile >> val;
//		fwrite(&val, sizeof(T), 1, ou);

//	}
}

int main(int argc, char **argv){

	myIniFiles ini(argc, argv);

	ini.register_param("input",  "i", "[=file name] Input data, should be a text file, one value per line");
	ini.register_param("output", "o", "[=file name] Output data file");
	ini.register_flag("verbose", "v", "Additional output to stdout");
//	mystring IDs = "[";
//	for (size_t i = IGB_MIN_TYPE; i < IGB_MAX_TYPE ; ++i)
//		IDs += "'"+ IGBheader::TypeID2Name(i) + "', ";

//	IDs = IDs.slice(0,-2) + "]";
//	ini.register_param("type",   "t", "Input data type " + IDs);
//	ini.register_param("unit",   "u", "The unit of the data, e.g. ms, mV, ...");

/////////////////////////// Reading of params //////////////////////////////////
	bool verbose	  = ini.exists("verbose");
	mystring input    = ini.read("input" , mystring());
	mystring output(input);
	output.file_new_ext(".dat");
	output 			  = ini.read("output", output);
//	mystring typeName = ini.read("type", mystring("IGB_NONE"));
//	mystring unit	  = ini.read("unit", mystring(""));

/////////////////////////// Error checking /////////////////////////////////////
	if ( not input.file_exists() ) cmdline::exit("input file does'nt exist.");

//	int datatype = IGBheader::Name2TypeID(typeName);
//	if (datatype == -1) cmdline::exit(   "Datatype '" + toString(datatype)
//										+"' unknown. Datatype must be out of \n"
//										+ IDs);


////////////////////////////////////////////////////////////////////////////////
	gzFile in = gzopen(input.c_str(), "r");
	ofstream ou(output.c_str());
	IGBheader igb(in);
	if (verbose) igb.info();

	int datatype = igb.type();
	switch (datatype){
	case IGB_BYTE: 		convert<unsigned char>(in, ou, igb); break;
	case IGB_CHAR: 		convert<char>		  (in, ou, igb); break;
	case IGB_SHORT: 	convert<short>		  (in, ou, igb); break;
	case IGB_LONG: 		convert<long>		  (in, ou, igb); break;
	case IGB_FLOAT: 	convert<float>		  (in, ou, igb); break;
	case IGB_DOUBLE: 	convert<double>	  	  (in, ou, igb); break;
	case IGB_INT: 		convert<int >		  (in, ou, igb); break;
	case IGB_UINT: 		convert<unsigned int> (in, ou, igb); break;
	case IGB_STRUCTURE: convert_structure(in, ou, igb); break;
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

//	ifstream infile;
//	infile.open (input.c_str(), ifstream::in);
//	size_t nr_lines=0;
//	string line;
//    while( getline(infile, line) ) nr_lines++;
//    infile.seekg(0);
//	infile.close();

//	infile.open (input.c_str(), ifstream::in);
//	IGBheader igb;
//	FILE *ou = fopen(output.c_str(),"w");
//	igb.fileptr(ou);
//	igb.dim(nr_lines);
//	igb.unites(unit.c_str());
////	igb.comment("written by dat2IGB (C) 2011 Stefan Fruhner");
//	igb.type(datatype);

//	igb.write(); // create a file with header

//	switch (datatype){
//	case IGB_BYTE: 		convert<byte>		 (infile, ou); break;
//	case IGB_CHAR: 		convert<char>		 (infile, ou); break;
//	case IGB_SHORT: 	convert<short>		 (infile, ou); break;
//	case IGB_LONG: 		convert<long>		 (infile, ou); break;
//	case IGB_FLOAT: 	convert<float>		 (infile, ou); break;
//	case IGB_DOUBLE: 	convert<double>		 (infile, ou); break;
//	case IGB_INT: 		convert<int >		 (infile, ou); break;
//	case IGB_UINT: 		convert<unsigned int>(infile, ou); break;
//	case IGB_COMPLEX:
//	case IGB_D_COMPLEX:
//	case IGB_RGBA:
//	case IGB_STRUCTURE:
//	case IGB_POINTER:
//	case IGB_LIST:
//	default: {cerr << "Datatype " << IGBheader::TypeID2Name(datatype)
//				  << " currently not implemented." << endl;
//				exit(-2);
//			}
//	}

//	fclose(ou);
//	infile.close();

	gzclose(in);
	ou.close();


	return 0;
}
