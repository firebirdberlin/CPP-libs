/*
 *		gipldo.cpp
 *		Creates a header for a file without header and saves new file.
 *
 *		Copyright 2008 Stefan Fruhner <jesus@computer>
 *
 *		This program is free software; you can redistribute it and/or modify
 *		it under the terms of the GNU General Public License as published by
 *		the Free Software Foundation; either version 2 of the License, or
 *		(at your option) any later version.
 *
 *		This program is distributed in the hope that it will be useful,
 *		but WITHOUT ANY WARRANTY; without even the implied warranty of
 *		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
 *		GNU General Public License for more details.
 *
 *		You should have received a copy of the GNU General Public License
 *		along with this program; if not, write to the Free Software
 *		Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *		MA 02110-1301, USA.
 */


/**
	* \page mylibs
	* \section gipldo
	* \subsection sec_intro Intention
	*	Do various things with gipl files. For a list compile the program with
	*
	*  \verbatim $ cd demos && make gipldo \endverbatim
	*  and excute
	* \verbatim $ ./gipldo help \endverbatim
	*/

#include <stdio.h>
#include <mylibs/mystring.hpp>
#include <mylibs/myinifiles.hpp>
#include <mylibs/gipl.h>
#include <mylibs/mymatrix.hpp>

//#include <mcheck.h> // mtrace

#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

using namespace std;

void exit(string message){
	cerr << message << endl;
	exit(-1);
}

FILE * fopen(string filename, const char *modes){
	FILE *f = fopen( filename.c_str() , modes );
	if (f==NULL) {
		cerr << "File error : "  << filename << endl;
		exit (-1);
	}
	return f;
}

/** name: mkdir
 *
 * Tries to create a folder, exits on error.
 * @param f: name of the folder to be created
 **/
void mkdir(mystring f){
	if (not f.strip().empty()){
		int ret = mkdir(f.c_str(), 0700);
		if (ret != 0)
			switch (errno){
				case EEXIST: break; // ignore, if dir already exists
				default:
					cerr << "Error "<< ret <<", while creating directory "
									<< f << "." << endl;
					exit(ret);
			}
	}
}

template <class T>
void save(const char* filename, mymatrix<T> &matrix){
	//sprintf(fn, "median_gradient.gipl");
	my_regular_grid grid = matrix;
	GIPL header(grid);

	header.set_image_type(matrix[0]);
	cout << "Image type is: " << header.image_type() << endl;

	FILE *ou = header.save_header(string(filename));
	matrix.save_to_file(ou);
	cout << filename <<"(" << header.dims(0)<< "x" << header.dims(1)
		 << "x"<<header.dims(2)<< "x"<<header.dims(3)<<") saved." << endl;
	return;
}

/** name: copy_data
 * Copys a file starting at a certain position. Both files have to be open.
 * \param infile: The file to read.
 * \param start: Postion in the infile to start the copying in bytes.
 * \param outfile: The file to be written.
 */
void copy_data(FILE *infile, size_t start, FILE *outfile){
	const size_t packet_size = 1024; // 1kB

// obtain file size:
//	fseek (pinFile , 0 , SEEK_END);
//	size_t filesize = ftell (pinFile);
//	rewind (pinFile);

	fseek (infile , start , SEEK_SET); // seek the file at the start position

	size_t bytes_read = packet_size;
	char * buffer = (char*) malloc (sizeof(char)*packet_size);
	if (buffer == NULL) {fputs ("Memory error",stderr); exit (2);}

	while (bytes_read == packet_size){
		bytes_read = fread(buffer,1,packet_size,infile);
		fwrite(buffer, bytes_read, 1, outfile);
	}
	// terminate

	free (buffer);
}

template <class T>
void create_mask(vector<mystring> & files, mystring& outfile, const float threshold, const bool invert){
	GIPL img(files[0]);					// read the header
	my_regular_grid grid = img.grid();
	mymatrix<T> matrix(grid);
	matrix.read_data(files[0], img.header_size());	// read the data of the first image
	mymatrix<char> *mask = matrix.mask(threshold);

	if (invert){
		for (unsigned long i = 0; i < mask->items(); i++){
			mask->operator[](i) = (mask->operator[](i) == (char) 0) ? (char) 1 : (char) 0;
		}
	}

	save(outfile.c_str(), *mask);
	delete mask;
	return;
}

template <class T>
void subtract_gipl_files(vector<mystring> & files, mystring& outfile, bool ignore_zeros = false){
	if (files.size() < 2 ) exit("I need at least two files to diff ... ");
	GIPL img1(files[0]);	// read the header
	GIPL img2(files[1]);	// now file 2

	if (img1.image_type() != img2.image_type()) exit("File types are different !");


	my_regular_grid g1 = img1.grid();
	my_regular_grid g2 = img2.grid();
	if (g1 != g2)exit("Dimensions are different !");

	mymatrix<T> matrix1(g1);
	matrix1.read_data(files[0], img1.header_size());	// read the data of the first image
	mymatrix<T> matrix2(g2);
	matrix2.read_data(files[1], img2.header_size());	// read the data of the first image

	if (ignore_zeros){
		for (long i = 0; i < (long) matrix1.items(); i++){
			if (( matrix1[i] == (T) 0 ) or ( matrix2[i] == (T) 0 )){
				matrix1[i] = (T) 0;
				matrix2[i] = (T) 0;
			}
		}
	}

	matrix1.sub(matrix2);

//				for (uint i = 0; i < matrix1.items(); i++){
//					if (matrix1[i] > 0. and matrix2[i]> 0.)
//						matrix1[i] -= matrix2[i];
//					else
//						matrix1[i] = 0.;
////					cout << matrix1[i] << endl;
//				}


	FILE *f = img1.save_header(outfile);
	matrix1.save_to_file(f);

}

template <class T>
void rescale_to_256(mymatrix<T> &matrix, bool verbose = false){
	if (verbose) cout << "Rescaling to the range [0:256]" << endl;

	#pragma omp parallel for
	for (long i = 0; i < (long) matrix.items() ; i++)
		matrix[i] &= 7; // bitwise modulo 8
}

void rescale_to_256(mymatrix<float> & matrix, bool verbose = false){
	if (verbose) cout << "Rescaling to the range [0:256]" << endl;
	matrix.adjust_contrast(0., 256.);
}

void rescale_to_256(mymatrix<double> & matrix, bool verbose = false){
	if (verbose) cout << "Rescaling to the range [0:256]" << endl;
	matrix.adjust_contrast(0., 256.);
}


template <class T>
void crop_image(vector<mystring> & files, mystring crop_coords, mystring outfile, bool verbose = false){

	size_t plus = crop_coords.find("+");
	mystring tmp = crop_coords(0, plus);
	myStringList coord = tmp.split(":");

	tmp = crop_coords(plus);
	myStringList sizes = tmp.split("+");

	if (coord.size() != sizes.size())
		exit("Number of coordinates and number of image sizes must match, e.g 100:100+70+70");

	if (coord.size() > 4)
		exit("Maximum 4 values for coordinates : x,y,z,t");

	// convert to size_t
	size_t *c = new size_t[4];
	for (size_t i = 0; i < coord.size(); i++) coord[i] >> c[i];

	size_t *w = new size_t[4];
	for (size_t i = 0; i < sizes.size(); i++) {
		sizes[i] >> w[i];
		w[i]++; // increase by 1
	}

	mystring basename = outfile.file_base(true);
	size_t cnt = 0;
	for (size_t i = 0; i < files.size(); i++){
		if (verbose) cout << "Cropping " << files[i] << endl;
		GIPL img(files[0]);					// read the header

		my_regular_grid grid = img.grid();
		for (size_t j = 0; j < coord.size(); j++){
			if (c[j] >= img.dims(j)){
				cerr << "Coordinate out of range ("<< j <<":"<< c[j] << ")-- cannot crop" << endl;
				continue;
			}

			if (c[j]+w[j] > img.dims(j)){
				cerr << "Width out of range ("<< j <<":"<< w[j] << ")-- cannot crop" << endl;
				continue;
			}
		}

		for (size_t j = coord.size(); j < 4 ; j++){
			c[j] = 0;
			w[j] = img.dims(j);
		}

		// print out results
		cout << " corner to start cropping : ";
		for (size_t j = 0; j < 4; j++) cout << c[j] << " ";cout << endl;

		cout << " witdh of new image :";
		for (size_t j = 0; j < 4; j++) cout << w[j] << " ";cout << endl;

		mymatrix<T> original(grid);
		original.read_data(files[i], img.header_size());
		my_regular_grid copy_grid = grid;
		copy_grid.dims(w[0], w[1], w[2], w[3]);

		mymatrix<T> copy(copy_grid);
		for (uint t = 0; t < w[3]; t++)
		for (uint z = 0; z < w[2]; z++)
		for (uint y = 0; y < w[1]; y++)
		for (uint x = 0; x < w[0]; x++){
			uint idx = copy.index(x,y,z,t);
			uint ori = original.index(c[0]+x, c[1]+y,c[2]+z, c[3]+t);

			copy[idx] = original[ori];
		}

		cout << basename << endl;

		char fn[256];
		sprintf(fn, "%s%04d.gipl", basename.c_str(), (int) cnt++);
		save(fn, copy);
	}
	delete[] c;
	delete[] w;
}

template <class T>
void merge_gipl_files(vector<mystring> & files, mystring& outfile, bool rescale=false){
	GIPL img(files[0]);					// read the header
	my_regular_grid grid = img.grid();
	mymatrix<T> matrix(grid);
	matrix.read_data(files[0], img.header_size());	// read the data of the first image

	/** \attention	rescaling works, but seems not to be recognizced by itksnap
	* 				itksnap and ImageJ show only colors with values 0-256, higher
	* 				values are divided by 256 and the rest
	*/
	if (rescale) rescale_to_256(matrix, true);

	// process all other images
	for (uint f = 1; f < files.size(); f++){
		cout << " Reading "<< files[f];
		GIPL img2(files[f]);			// read the header

		if (img.image_type() != img2.image_type()){
			cerr << "Image types are different ... skip." << endl;
			continue;
		}

		mymatrix<T> matrix2(img.dims(0),img.dims(1),img.dims(2),img.dims(3));

		if (not matrix.equal_sized(matrix2)){
			cerr << "Dimensions don't match ! Cannot merge images ... skip." << endl;
			continue;
		}

		cout << " 1/3 ";
		matrix2.read_data(files[f], img2.header_size());	// read the data of the next image
		cout << "\b\b\b\b 2/3 ";

		if (rescale) rescale_to_256(matrix2);

		cout << "\b\b\b\b 3/3 " << endl;
		matrix.add(matrix2);
	}

	if (rescale){ // wenn rescale, dann als uchar abspeichern, spart Speicherplatz
		img.image_type(GIPL_U_CHAR);
		mymatrix<unsigned char> ch_mat(img.dims(0), img.dims(1), img.dims(2), img.dims(3));

		// copy values of the matrix
		#pragma omp parallel for
		for (long i = 0; i < (long) ch_mat.items(); i++) ch_mat[i] = (unsigned char) matrix[i];

		uchar min, max;
		ch_mat.minmax(min, max);
		img.set_minmax(min, max);

		cout << endl;
		img.print_header();
		FILE *f = img.save_header(outfile);
		ch_mat.save_to_file(f);

		cout << "bytes written: " << img.header_size() + sizeof(ch_mat[0]) * ch_mat.items() << endl;
	}
	else{
		T min,max;
		matrix.minmax(min, max);
		img.set_minmax(min, max);

		cout << endl;
		img.print_header();
		FILE *f = img.save_header(outfile);
		matrix.save_to_file(f);
		cout << "bytes written: " << img.header_size() + sizeof(matrix[0]) * matrix.items() << endl;
	}
}

template <class T>
void pm3d(mystring &file, mystring& outfile, vector<int> slice_nr, mystring cutout_interval, bool png){
	for (size_t slice = 0; slice < slice_nr.size(); slice++){
		//cout << "slice_nr[slice] = "<< slice_nr[slice] << endl;
		GIPL img(file);
		my_regular_grid grid = img.grid();

		mymatrix<T> matrix(grid);
		matrix.read_data(file, img.header_size());	// read the data of the first image

		if (not cutout_interval.empty()){
			T min, max;
			matrix.minmax(min, max);

			T lower = min;
			T upper = max;
			myStringList l = cutout_interval.split();
			try{
			if ( l.size() > 0 ){T tl; l[0] >> tl; lower = (tl < min)? min: tl;}
			if ( l.size() > 1 ){T tu; l[1] >> tu; upper = (tu > max)? max: tu;}
			} catch (mystring_exception &e) {
				cerr << e.what() << endl;
				exit(-1);
			}
			cout << "Cutting : " << lower << "," << upper << endl;

			#pragma omp parallel for
			for (long j = 0; j < (long) matrix.items() ; j++)
				if (matrix[j] < lower or matrix[j] > upper) matrix[j] = 0;
		}

		mystring fn  =  outfile.path_join(file.file_base(true) + "_"
							+ toString(slice_nr[slice], 6) + ".dat");

		matrix.save_to_pm3dmap(fn, slice_nr[slice], false, png);
	}
}

template <class T>
void save_histogram(mystring &file, mystring outfile){

	GIPL img(file);
	my_regular_grid grid = img.grid();

	mymatrix<T> matrix(grid);
	matrix.read_data(file, img.header_size());	// read the data of the first image
	mystring fn;
	fn = outfile.path_join(file.file_base(true) + ".hist");
	matrix.save_histogram(fn);
	cout << "Histogram saved to " << fn << endl;
}

template <class T>
void adjust_contrast(mystring &file, mystring outfile, mystring range){

	if ( range.empty() ) exit("adjust:contrast needs a value range, e.g. -r 0.2 1.4");
	GIPL img(file);
	my_regular_grid grid = img.grid();

	mymatrix<T> matrix(grid);
	matrix.read_data(file, img.header_size());	// read the data of the first image
	mystring fn;
	fn = outfile.path_join(file.file_base(true) + ".hist");
//	matrix.save_histogram(fn);
	T min=0., max = 0.;
	myStringList l = range.split();
	try{
		if ( l.size() > 2 ){
			l[0] >> min;
			l[1] >> max;
		}
	} catch (mystring_exception &e) {
		cerr << e.what() << endl;
		exit(-1);
	}

	matrix.adjust_contrast(min, max);
}

//template <class T>
//void detect_HIGH_edges(mystring &file, mystring outfile, T epsilon){

	//if ( range.empty() ) exit("adjust:contrast needs a value range, e.g. -r 0.2 1.4");
	//GIPL img(file);
	//my_regular_grid grid = img.grid();

	//mymatrix<T> matrix(grid);
	//matrix.read_data(file, img.header_size());	// read the data of the first image
	//mystring fn;
	//fn = outfile.path_join(file.file_base(true) + ".hist");
////	matrix.save_histogram(fn);
	//T min=0., max = 0.;
	//myStringList l = range.split();
	//try{
		//if ( l.size() > 2 ){
			//l[0] >> min;
			//l[1] >> max;
		//}
	//} catch (mystring_exception &e) {
		//cerr << e.what() << endl;
		//exit(-1);
	//}

	//matrix.adjust_contrast(min, max);
//}

void help(string action){
//	cout << "printing help for action "<< action << endl;
	if (action == "merge")
		cout << "merge - Combines GIPL images by adding the color values\n\n\
params:\n\
 -r : rescales all color values to the range [0:256] by computing value %= 256;\n\
 \n\
Usage: gipldo merge file1.gipl file2.gipl merged.gipl \n\n";
	else
	if (action == "pm3d")
		cout << "pm3d - Extracts one z-slice of a 3d gipl-image\n\n\
params:\n\
 -s <int>, ...  : Set the slice-nr to be extracted.\n\
 -c min, max	: Set interval to be extracted, value outside this interval\n\
                  are set to 0.\n\
 -o <string>	: Folder to output the data.\n\
 \n\
Usage: gipldo pm3d file.gipl -s 1\n\n";
	else
	if (action == "sub")
		cout << "sub - Subtracts two gipl images if their sizes match (file1-file2)\n\n\
 \n\
Usage: gipldo sub file1.gipl file2.gipl output.gipl\n\n";
	if (action == "mask")
		cout << "mask - Create a mask from a gipl file\n\n\
 \n\
Usage: gipldo mask file.gipl -t <float> output.gipl\n\n";
	else
	if (action == "adjust_contrast")
		cout << "Rescales all values to a new range [min, max]\n\
 \n\
Usage: gipldo adjust_contrast file1.gipl [file2.gipl, ... ] output.gipl -r 0 1.4" << endl;
	else
	if (action == "copy")
		cout << "Copies a gipl file\n\
 \n\
Usage: gipldo copy file1.gipl file2.gipl output.gipl" << endl;
	else
	if (action == "create_header")
		cout << "Adds a header to a gipl file.\n\
params:\n\
 -d <int,int,int,int> # of items in each direction and time\n\
 -p <double,double,double,double> pixel spacing \n\
 -0 <double,double,double,double> # spatial and temporal origin of the data\n\
 -t image type of the data, use '-a list' to get the data type definitions\n\
 -m 80 chars o user text, e.g, patient's name\n\
 \n\
Usage: gipldo create_header file1.gipl output.gipl -d 256 256 1 1" << endl;
	else
	if (action == "delete_header")
		cout << "Removes a header from a gipl file.\n\
 \n\
Usage: gipldo delete_header file1.gipl output.gipl" << endl;
	else
	if (action == "hist")
		cout << "Saves a file containing a histogram.\n\
 \n\
Usage: gipldo hist file.gipl [output]" << endl;
	else
	if (action == "info")
		cout << "Prints the header information of a gipl file.\n\
 \n\
Usage: gipldo info file.gipl" << endl;
	else
	if (action == "list")
		cout << "Lists out the data type definitions of the gipl format." << endl;
	else
		cout << "No help available for "<< action << endl;
}

int main(int argc, char **argv){

	myIniFiles ini(argc, argv, false);
	ini.dependency_checking(false);

	ini.set_info("gipldo - Create header for gipl files or delete them.\n\n Usage: gipldo help <action>");
	ini.register_param("input"				,"i", "file to be read in", true);
	ini.register_param("output"				,"o", "file to be written", true);
	ini.register_param("action"				,"a", "action : adjust_contrast, copy, create_header, crop, delete_header,hist, info, list, merge, pm3d, sub, mask", true);
	ini.register_param("range"				,"R", "values out of this interval are set to 0", true, "action=adjust_contrast");
	ini.register_param("dims"				,"d", "<int,int,int,int> # of items in each direction and time ", true, "action=create_header");
	ini.register_param("pixdim"				,"p", "<double,double,double,double> pixel spacing ", true, "action=create_header");
	ini.register_param("origin"				,"0", "<double,double,double,double> # spatial and temporal origin of the data", true, "action=create_header");
	ini.register_param("image_type"			,"t", "image type of the data, use '-a list' to get the data type definitions", true, "action=create_header");
	ini.register_param("infoline"			,"m", "80 chars o user text, e.g, patient's name", true, "action=create_header");
	ini.register_param("rescale_to_256"		,"r", "If all data values should be rescaled to [0:256]", false, "action=merge");
	ini.register_param("slice","s", "slice number ^= index of plane in z - direction", true, "action=pm3d");
	ini.register_param("png",NULL, "Create png files instead of showing the data on the screen ", false, "action=pm3d");
	ini.register_param("cutout_interval","c", "values out of this interval are set to 0", true, "action=pm3d");
	ini.register_param("no-zero-diff","z", "Set value to zero where one of both gipl-files is zero.", false, "action=sub");
	ini.register_param("threshold",NULL, "Threshold value for creating the mask. (Values >t will be set to 1, 0 otherwise.)", true, "action=mask");
	ini.register_param("invert",NULL,"create an inverted mask", false, "action=mask");
	ini.register_param("size"		 ,"C", " position to start crop and witdh in the form 'x:y:z:t+dx+dy+dz+dt'", true, "action=crop");

	ini.check(1);


	// read settings from inifile
	vector<mystring> files;
	ini.readVector("input" , files);

	mystring outfile = ini.read("output", mystring() );
	mystring action  = ini.read("action", mystring() );

	mystring non_opt_args = ini.param_non_opt_args();
	myStringList arglist = non_opt_args.split();

	cout << "Non opt args :" << non_opt_args << endl;
	if (not ini.param_exists("action") and arglist.size() > 0 )
		action = arglist[0];

	if (action != "help" and action != "list"){
		if (files.size() == 0 and arglist.size() > 1) {

			/** \attention If no output file was defined before, then the last
			 *  argument is the output. */
			if (action != "pm3d" and outfile.empty() and arglist.size() > 2)
				for (size_t i = 1; i < arglist.size()-1; i++)
					files.push_back(arglist[i]);
			else //! otherwise every given file is used as input
				for (size_t i = 1; i < arglist.size(); i++)
					files.push_back(arglist[i]);
		}

		// last argument should be the output file
		if (outfile.empty() and arglist.size() > 2) outfile = arglist[-1];

		if (files.size() == 0)	exit("Missing input file.");

		if (files[0] == outfile and action != "info" and action != "list")
			exit("Input file and output file must be different.");
	}
// ####################### main action starts here ############################
	FILE * pinFile, *poutFile;
	// copy a file

	if (action == "copy"){
		cout << " - Copying file " << files[0] << endl;
		pinFile  = fopen( files[0]  , "rb" );
		poutFile = fopen( outfile , "w" );

		copy_data(pinFile, 0, poutFile);

		fclose (pinFile);
		fclose (poutFile);
	}
	else
	if (action == "create_header"){
		GIPL gipl;
		cout << " - Creating header for "<< files[0] << endl;

		vector<uint> d(4,1);
		ini.readVector("dims", d, d);
		if (d.size() != 4 ) exit("dims must have 4 values");
		gipl.dims(d[0], d[1], d[2], d[3]);
		ini.writeVector("dims",d);

		vector<double> p(4,1.);
		ini.readVector("pixdim", p, p);
		if (p.size() != 4 ) exit("pixdim must have 4 values");
		gipl.pixdim(p[0], p[1], p[2], p[3]);
		ini.writeVector("pixdim",p);

		vector<double> o(4,1.);
		ini.readVector("origin", o, o);
		if (o.size() != 4 ) exit("origin must have 4 values");
		gipl.origin(o[0], o[1], o[2], o[3]);
		ini.writeVector("origin",o);

		ushort image_type = ini.read("image_type", 0);
		if (image_type== GIPL_NONE) exit("image_type must be set.");
		gipl.image_type(image_type);
		ini.write("image_type", image_type);

		mystring info = ini.read("infoline", mystring("written by gipldo © Stefan Fruhner"));	//!< some comment
		gipl.infoline(info.c_str());
		ini.write("infoline", info);

		pinFile = fopen( files[0] , "rb" );
		poutFile = gipl.save_header(outfile);
		copy_data(pinFile, 0, poutFile);

		fclose (pinFile);
		fclose (poutFile);
		ini.write("input", files[0]);

		ini.save();
	}
	else
	if (action == "crop"){
		GIPL gipl(files[0]);
		cout << " - Cropping file "<< files[0] << " ("
			 << gipl.header_size() << " bytes )." << endl;

		mystring crop = ini.readstring("size", "none");
		if (crop == "none")
			exit("Need --size x:y:z:t+dx+dy+dz+dt");

		GIPL img1(files[0]);	// read the header
		switch (img1.image_type()){
		case GIPL_BINARY:	crop_image<bool  >(files, crop, outfile);break;	/* all the easy datatypes **/
		case GIPL_CHAR:		crop_image<char  >(files, crop, outfile);break;
		case GIPL_U_CHAR:	crop_image<uchar >(files, crop, outfile);break;
		case GIPL_SHORT:	crop_image<short >(files, crop, outfile);break;
		case GIPL_U_SHORT:	crop_image<ushort>(files, crop, outfile);break;
		case GIPL_INT:		crop_image<int   >(files, crop, outfile);break;
		case GIPL_U_INT:	crop_image<uint  >(files, crop, outfile);break;
		case GIPL_FLOAT:	crop_image<float >(files, crop, outfile);break;
		case GIPL_DOUBLE:	crop_image<double>(files, crop, outfile);break;
		case GIPL_C_SHORT:
		case GIPL_C_INT:
		case GIPL_C_DOUBLE:
		case GIPL_SURFACE:
		case GIPL_POLYGON:
			cerr << "Datatype not implemented " << img1.image_type() << endl;
			break;
		default:
			cerr << "Datatype unknown : " << img1.image_type() << endl;
			exit(-2);
			break;
		}
	}
	else
	if (action == "delete_header"){
		GIPL gipl(files[0]);
		cout << " - Removing header from "<< files[0] << " ("
			 << gipl.header_size() << " bytes )." << endl;

		pinFile  = fopen( files[0]  , "rb" );
		poutFile = fopen( outfile , "w"  );

		copy_data(pinFile, gipl.header_size(), poutFile);

		fclose (pinFile);
		fclose (poutFile);
	}
	else
	if (action == "list"){
		GIPL gipl;
		gipl.info_types();
	}
	else
	if (action == "info"){
		GIPL gipl(files[0]);
		gipl.print_header();
		my_regular_grid g = gipl.grid();
		if (gipl.image_type() == GIPL_FLOAT){
			mymatrix<float> data(g);
			data.read_data(files[0],gipl.header_size());

//			data.info();

			float mi = 0.;
			float ma = 0.;
			data.minmax(mi,ma);
			cout << "minimum value : " << mi << endl;
			cout << "maximum value : " << ma << endl;
		}
		if (gipl.image_type() == GIPL_CHAR){
			mymatrix<char> data(g);
			data.read_data(files[0],gipl.header_size());

//			data.info();

			char mi = 0.;
			char ma = 0.;
			data.minmax(mi,ma);
			cout << "minimum value : " << int(mi) << endl;
			cout << "maximum value : " << int(ma) << endl;
		}
	}
	else
	if (action == "sub") {

		GIPL img1(files[0]);	// read the header

		bool zero = ini.param_exists("no-zero-diff");

		switch (img1.image_type()){

		case GIPL_BINARY:	subtract_gipl_files<bool  >(files,outfile, zero);break;	/* all the easy datatypes **/
		case GIPL_CHAR:		subtract_gipl_files<char  >(files,outfile, zero);break;
		case GIPL_U_CHAR:	subtract_gipl_files<uchar >(files,outfile, zero);break;
		case GIPL_SHORT:	subtract_gipl_files<short >(files,outfile, zero);break;
		case GIPL_U_SHORT:	subtract_gipl_files<ushort>(files,outfile, zero);break;
		case GIPL_INT:		subtract_gipl_files<int   >(files,outfile, zero);break;
		case GIPL_U_INT:	subtract_gipl_files<uint  >(files,outfile, zero);break;
		case GIPL_FLOAT:	subtract_gipl_files<float >(files,outfile, zero);break;
		case GIPL_DOUBLE:	subtract_gipl_files<double>(files,outfile, zero);break;
		case GIPL_C_SHORT:
		case GIPL_C_INT:
		case GIPL_C_DOUBLE:
		case GIPL_SURFACE:
		case GIPL_POLYGON:
			cerr << "Datatype not implemented " << img1.image_type() << endl;
			break;
		default:
			cerr << "Datatype unknown : " << img1.image_type() << endl;
			exit(-2);
			break;
		}
	}
	else
	if (action == "mask") {

		GIPL img1(files[0]);	// read the header

		float threshold = ini.read("threshold", 0.);
		bool invert 	= ini.exists("invert");

		switch (img1.image_type()){
		case GIPL_BINARY:	create_mask<bool  >(files,outfile, threshold, invert);break;	/* all the easy datatypes **/
		case GIPL_CHAR:		create_mask<char  >(files,outfile, threshold, invert);break;
		case GIPL_U_CHAR:	create_mask<uchar >(files,outfile, threshold, invert);break;
		case GIPL_SHORT:	create_mask<short >(files,outfile, threshold, invert);break;
		case GIPL_U_SHORT:	create_mask<ushort>(files,outfile, threshold, invert);break;
		case GIPL_INT:		create_mask<int   >(files,outfile, threshold, invert);break;
		case GIPL_U_INT:	create_mask<uint  >(files,outfile, threshold, invert);break;
		case GIPL_FLOAT:	create_mask<float >(files,outfile, threshold, invert);break;
		case GIPL_DOUBLE:	create_mask<double>(files,outfile, threshold, invert);break;
		case GIPL_C_SHORT:
		case GIPL_C_INT:
		case GIPL_C_DOUBLE:
		case GIPL_SURFACE:
		case GIPL_POLYGON:
			cerr << "Datatype not implemented " << img1.image_type() << endl;
			break;
		default:
			cerr << "Datatype unknown : " << img1.image_type() << endl;
			exit(-2);
			break;
		}
	}
	else
	if (action == "adjust_contrast") {
		for (size_t i = 0; i < files.size(); i++){
		mystring range = ini.read("range", mystring());
		GIPL img1(files[i]);	// read the header
		switch (img1.image_type()){

		case GIPL_BINARY:	adjust_contrast<bool  >(files[i],outfile,range);break;	/* all the easy datatypes **/
		case GIPL_CHAR:		adjust_contrast<char  >(files[i],outfile,range);break;
		case GIPL_U_CHAR:	adjust_contrast<uchar >(files[i],outfile,range);break;
		case GIPL_SHORT:	adjust_contrast<short >(files[i],outfile,range);break;
		case GIPL_U_SHORT:	adjust_contrast<ushort>(files[i],outfile,range);break;
		case GIPL_INT:		adjust_contrast<int   >(files[i],outfile,range);break;
		case GIPL_U_INT:	adjust_contrast<uint  >(files[i],outfile,range);break;
		case GIPL_FLOAT:	adjust_contrast<float >(files[i],outfile,range);break;
		case GIPL_DOUBLE:	adjust_contrast<double>(files[i],outfile,range);break;
		case GIPL_C_SHORT:
		case GIPL_C_INT:
		case GIPL_C_DOUBLE:
		case GIPL_SURFACE:
		case GIPL_POLYGON:
			cerr << "Datatype not implemented " << img1.image_type() << endl;
			break;
		default:
			cerr << "Datatype unknown : " << img1.image_type() << endl;
			exit(-2);
			break;
		}
		}
	}
	else
	if (action == "merge") {
		if (files.size() < 2 )exit("I need at least two files to merge ... ");

		bool rescale = ini.exists("rescale_to_256");

		GIPL img1(files[0]);	// read the header
		switch (img1.image_type()){

		case GIPL_BINARY:	merge_gipl_files<bool  >(files,outfile,rescale);break;	/* all the easy datatypes **/
		case GIPL_CHAR:		merge_gipl_files<char  >(files,outfile,rescale);break;
		case GIPL_U_CHAR:	merge_gipl_files<uchar >(files,outfile,rescale);break;
		case GIPL_SHORT:	merge_gipl_files<short >(files,outfile,rescale);break;
		case GIPL_U_SHORT:	merge_gipl_files<ushort>(files,outfile,rescale);break;
		case GIPL_INT:		merge_gipl_files<int   >(files,outfile,rescale);break;
		case GIPL_U_INT:	merge_gipl_files<uint  >(files,outfile,rescale);break;
		case GIPL_FLOAT:	merge_gipl_files<float >(files,outfile,rescale);break;
		case GIPL_DOUBLE:	merge_gipl_files<double>(files,outfile,rescale);break;
		case GIPL_C_SHORT:
		case GIPL_C_INT:
		case GIPL_C_DOUBLE:
		case GIPL_SURFACE:
		case GIPL_POLYGON:
			cerr << "Datatype not implemented " << img1.image_type() << endl;
			break;
		default:
			cerr << "Datatype unknown : " << img1.image_type() << endl;
			exit(-2);
			break;
		}
	}
	else
	if (action == "pm3d"){

	 mkdir(outfile);
	 for (size_t i = 0; i < files.size(); i++){
		GIPL img(files[i]);
		vector<int> slices;
		ini.readVector("slice", slices);
		if (slices.size() == 0) exit("Pm3d : Slice number needed : use -s <int>, ... ");

		// interval of values which should be ignored, they will be set to 0 instead
		mystring cutout_interval = ini.read("cutout_interval", mystring());
		bool png = ini.exists("png");

		switch (img.image_type()){
		case GIPL_BINARY:	pm3d<bool  >(files[i],outfile,slices,cutout_interval,png);break;	/* all the easy datatypes **/
		case GIPL_CHAR:		pm3d<char  >(files[i],outfile,slices,cutout_interval,png);break;
		case GIPL_U_CHAR:	pm3d<uchar >(files[i],outfile,slices,cutout_interval,png);break;
		case GIPL_SHORT:	pm3d<short >(files[i],outfile,slices,cutout_interval,png);break;
		case GIPL_U_SHORT:	pm3d<ushort>(files[i],outfile,slices,cutout_interval,png);break;
		case GIPL_INT:		pm3d<int   >(files[i],outfile,slices,cutout_interval,png);break;
		case GIPL_U_INT:	pm3d<uint  >(files[i],outfile,slices,cutout_interval,png);break;
		case GIPL_FLOAT:	pm3d<float >(files[i],outfile,slices,cutout_interval,png);break;
		case GIPL_DOUBLE:	pm3d<double>(files[i],outfile,slices,cutout_interval,png);break;
		case GIPL_C_SHORT:
		case GIPL_C_INT:
		case GIPL_C_DOUBLE:
		case GIPL_SURFACE:
		case GIPL_POLYGON:
			cerr << "Datatype not implemented " << img.image_type() << endl;
			break;
		default:
			cerr << "Datatype unknown : " << img.image_type() << endl;
			exit(-2);
			break;
		}
	 }
	}
	else
	if (action == "hist") {
		for (size_t i = 0; i < files.size(); i++){
		GIPL img(files[i]);
		switch (img.image_type()){

		case GIPL_BINARY:	save_histogram<bool  		 >(files[i],outfile);break;	/* all the easy datatypes **/
		case GIPL_CHAR:		save_histogram<char  		 >(files[i],outfile);break;
		case GIPL_U_CHAR:	save_histogram<unsigned char >(files[i],outfile);break;
		case GIPL_SHORT:	save_histogram<short 		 >(files[i],outfile);break;
		case GIPL_U_SHORT:	save_histogram<unsigned short>(files[i],outfile);break;
		case GIPL_INT:		save_histogram<int   		 >(files[i],outfile);break;
		case GIPL_U_INT:	save_histogram<unsigned int  >(files[i],outfile);break;
		case GIPL_FLOAT:	save_histogram<float 		 >(files[i],outfile);break;
		case GIPL_DOUBLE:	save_histogram<double		 >(files[i],outfile);break;
		case GIPL_C_SHORT:
		case GIPL_C_INT:
		case GIPL_C_DOUBLE:
		case GIPL_SURFACE:
		case GIPL_POLYGON:
			cerr << "Datatype not implemented " << img.image_type() << endl;
			break;
		default:
			cerr << "Datatype unknown : " << img.image_type() << endl;
			exit(-2);
			break;

		}
		}
	}
	else
	if (action == "help") {
//		arglist.prnt();
		if (arglist.size() > 1) help(arglist[1]);
		else
		ini.print_help();
	}
	else {
		cerr << "Action unknown : '" << action << "'"<< endl;
	}
//	muntrace();
	return 0;
}
