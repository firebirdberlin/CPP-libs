/*
 *      gipl.cpp
 *
 *      Copyright 2008 Stefan Fruhner <fruhner@e82135>
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *      MA 02110-1301, USA.
 */


#include "gipl.h"
using namespace mylibs;

GIPL::GIPL(){
	dimension_val = 0;
	set_standard_header();
}

/** name: GIPL::GIPL
 * Constructor for a gipl file that automatically reads in the header of a file
 * \param fn: filename of the gipl-file to read */
GIPL::GIPL(string fn){
	#ifdef DEBUG
	cout << " - Loading gipl file " << fn << endl;
	#endif

	dimension_val = 0;
	set_standard_header();

	read_header(fn);
}

GIPL::GIPL(my_regular_grid &grid){
	set_standard_header();

	this->dims(grid.dims(0),grid.dims(1),grid.dims(2),grid.dims(3));
	this->pixdim(grid.pixdim());
	this->origin(grid.origin());
}

void GIPL::dims(uint i,uint j,uint k,uint l){
	header.dims[0] = (i < 1) ? 1: i; //! dimension value less than 1 makes no sense
	header.dims[1] = (i < 1) ? 1: j; //! and MUST be forbidden (see set_dimension_data()
	header.dims[2] = (i < 1) ? 1: k;
	header.dims[3] = (l < 1) ? 1: l;

	set_dimension_data(); // set dimension, slice_size and # items
	return;
}

void GIPL::dims(int *N){
	GIPL::dims(N[0], N[1], N[2], N[3]);
	return;
}

void GIPL::origin(double i, double j, double k, double l){
	header.origin[0] = i;
	header.origin[1] = j;
	header.origin[2] = k;
	header.origin[3] = l;
	return;
}

void GIPL::pixdim(double i, double j, double k, double l){
	header.pixdim[0] = i;
	header.pixdim[1] = j;
	header.pixdim[2] = k;
	header.pixdim[3] = l;
	return;
}

void GIPL::origin(Point p){
	origin(p.x, p.y, p.z, p.t);
}

void GIPL::pixdim(Point p){
	pixdim(p.x, p.y, p.z, p.t);
	return;
}

bool GIPL::read_header(string fn){
	FILE *ou = 0;
	uchar buf[512];
	int counter = 0; // counts how many bytes where read

	ou = fopen(fn.c_str(),"r");
	if (!ou) throw GIPL_exception(GIPL_FILE_OPEN_FAILURE); // file error

	for (int i=0;i<4;i++) {
		if ( fread(buf,sizeof(ushort),1,ou) != 1 )
			cmdline::exit(string(__FILE__)+":"+toString(__LINE__) + "Did not read enough items.");
		header.dims[i] = mk_unsigned_short(buf);
		if (header.dims[i] == 0) header.dims[i]= 1; // ensure that no value is 0
		counter += sizeof(ushort);
	}
	if ( fread(buf,sizeof(ushort),1,ou) != 1 )
		cmdline::exit(string(__FILE__)+":"+toString(__LINE__) + "Did not read enough items.");
	header.image_type = mk_unsigned_short(buf);
	counter += sizeof(ushort);

	for (int i=0;i<4;i++) {
		if (fread(buf,sizeof(float),1,ou) != 1)
		cmdline::exit(string(__FILE__)+":"+toString(__LINE__) + "Did not read enough items.");
		header.pixdim[i] = mk_float(buf);
		counter += sizeof(float);
	}

	if (fread(header.infoline,80,1,ou) != 1)
		cmdline::exit(string(__FILE__)+":"+toString(__LINE__) + "Did not read enough items.");
	counter += 80;

	for (int i=0;i<20;i++) {
		if ( fread(buf,sizeof(float),1,ou) != 1)
			cmdline::exit(string(__FILE__)+":"+toString(__LINE__) + "Did not read enough items.");
		header.matrix[i] = mk_float(buf);
		counter += sizeof(float);
	}

	if ( fread(buf,sizeof(char),1,ou) != 1)
		cmdline::exit(string(__FILE__)+":"+toString(__LINE__) + "Did not read enough items.");
	header.flag1 = (char)buf[0];
	counter += sizeof(char);
	if( fread(buf,sizeof(char),1,ou) != 1)
		cmdline::exit(string(__FILE__)+":"+toString(__LINE__) + "Did not read enough items.");
	
	header.flag2 = (char)buf[0];
	counter += sizeof(char);

	if( fread(buf,sizeof(double),1,ou) != 1)
		cmdline::exit(string(__FILE__)+":"+toString(__LINE__) + "Did not read enough items.");
	header.min = mk_double(buf);
	counter += sizeof(double);
	if( fread(buf,sizeof(double),1,ou) != 1)
		cmdline::exit(string(__FILE__)+":"+toString(__LINE__) + "Did not read enough items.");
	header.max = mk_double(buf);
	counter += sizeof(double);

	for (int i=0;i<4;i++) {
		if( fread(buf,sizeof(double),1,ou) != 1)
			cmdline::exit(string(__FILE__)+":"+toString(__LINE__) + "Did not read enough items.");	
		header.origin[i] = mk_double(buf);
		counter += sizeof(double);
	}

	if( fread(buf,sizeof(float),1,ou) != 1)
		cmdline::exit(string(__FILE__)+":"+toString(__LINE__) + "Did not read enough items.");
	header.pixval_offset = mk_float(buf);
	counter += sizeof(float);
	if( fread(buf,sizeof(float),1,ou) != 1)
		cmdline::exit(string(__FILE__)+":"+toString(__LINE__) + "Did not read enough items.");
	header.pixval_cal = mk_float(buf);
	counter += sizeof(float);

	if( fread(buf,sizeof(float),1,ou) != 1)
		cmdline::exit(string(__FILE__)+":"+toString(__LINE__) + "Did not read enough items.");
	header.userdef1 = mk_float(buf);
	counter += sizeof(float);
	if( fread(buf,sizeof(float),1,ou) != 1)
		cmdline::exit(string(__FILE__)+":"+toString(__LINE__) + "Did not read enough items.");
	header.userdef2 = mk_float(buf);
	counter += sizeof(float);

	if( fread(buf,sizeof(uint),1,ou) != 1)
		cmdline::exit(string(__FILE__)+":"+toString(__LINE__) + "Did not read enough items.");
	header.magic_number = mk_unsigned_int(buf);
	counter += sizeof(uint);

	fclose(ou);
	filename = fn;
	header_size_val = counter;

	set_dimension_data();

	if (header.image_type == GIPL_NONE) throw GIPL_exception(NO_GIPL_HEADER_FAILURE);


	return true;
}

/** index
 * The data is stored for 1 to 4 dimensions. To keep the code general
 * the arrays used should be only one-dimensional. This function `index`
 * returns an one-dimensional field index for a four-dimensional
 * description which is more intuitive.
 *
 * \attention 	In the gipl format it is important to notice that the
 * 				data is stored a reversed order. The data is organized
 * 				as a stack where x varies fastest, then y, z and t.
 * 				An advantage is that one can slice the data easily.
 *
 * \param i: index in x -directions
 * \param j: y-index
 * \param k: z-index
 * \param l: t-index
 *
 * \return 1-D field index as unsigned int
 */
uint GIPL::index(uint i,uint j, uint k, uint l){
	ushort* d = header.dims;
	bool index_ok = ((i <= d[0]) && (j <= d[1])&& (k <= d[2])&& (l <= d[3]));

	if (index_ok)
	switch (dimension_val){
		case 1: return												    i;
				break;
		case 2: return 										 j * d[0] + i;
				break;
		case 3: return 						 k * d[1]*d[0] + j * d[0] + i;
				break;
		case 4: return  l * d[2]*d[1]*d[0] + k * d[1]*d[0] + j * d[0] + i;
				break;
	}
	else return 0;

	return 0;
}

/** set_dimension_data
 *
 * Sets all values needed to determine the dimesionality, as number of items in
 * the file, the number of slices, where slice means the last dimension, which
 * can be time or the third spatial dimension. The size of one slice is also
 * set.
 *
 * \return Nothing.
 **/
void GIPL::set_dimension_data(){

	dimension_val = 1;
	for (int i = 3; i > -1; i--)// determine the number of slices (last valid entry in dims)
		if (header.dims[i] > 1) {
			dimension_val = i+1;
			break;
	}

//	cout << "GIPL dimensions : " << (int) dimension_val << endl;
	return;

//	dimension_val = 1;
//	//items_val  = header.dims[0];
//	for (int i = 1; i < 4; i++){  //! determine the number of slices (last valid entry in dims)
//		if (header.dims[i] == 1){ //! the number of slices is given by the number of items in the last dimension (mostly called `z` or `t`)
//			////nr_slices = header.dims[i-1];
//			break;
//		}
//		//slice_size_val = items_val;		 // size of each slice
//		//items_val *= header.dims[i]; 	 // number of items in the matrix
//		dimension_val ++;			 // dimension of the problem
//	}
}

ushort GIPL::mk_unsigned_short(uchar *ptr) {
	return (ushort)0x0100*(ushort)ptr[0]+
		   (ushort)0x0001*(ushort)ptr[1];
}

uint GIPL::mk_unsigned_int(uchar *ptr) {
	return (uint)0x01000000*(uint)ptr[0]+
		   (uint)0x00010000*(uint)ptr[1]+
		   (uint)0x00000100*(uint)ptr[2]+
		   (uint)0x00000001*(uint)ptr[3];
}

float GIPL::mk_float(uchar *ptr) {
	float tmp;
	uchar pt[4];
	pt[0] = ptr[3];
	pt[1] = ptr[2];
	pt[2] = ptr[1];
	pt[3] = ptr[0];
	memcpy(&tmp,pt,4);
	return tmp;
}

double GIPL::mk_double(uchar *ptr) {
	double tmp;
	uchar pt[8];
	pt[0] = ptr[7];
	pt[1] = ptr[6];
	pt[2] = ptr[5];
	pt[3] = ptr[4];
	pt[4] = ptr[3];
	pt[5] = ptr[2];
	pt[6] = ptr[1];
	pt[7] = ptr[0];
	memcpy(&tmp,pt,8);
	return tmp;
}

void GIPL::mk_wr_unsigned_short(ushort value, uchar *ptr) {
	uchar pt[2];
	memcpy(pt,&value,2);
	ptr[0] = pt[1];
	ptr[1] = pt[0];
	return;
}

void GIPL::mk_wr_unsigned_int(uint value, uchar *ptr) {
	uchar pt[4];
	memcpy(pt,&value,4);
	ptr[0] = pt[3];
	ptr[1] = pt[2];
	ptr[2] = pt[1];
	ptr[3] = pt[0];
	return;
}

void GIPL::mk_wr_float(float value, uchar *ptr) {
	uchar pt[4];
	memcpy(pt,&value,4);
	ptr[0] = pt[3];
	ptr[1] = pt[2];
	ptr[2] = pt[1];
	ptr[3] = pt[0];
	return;
}

void GIPL::mk_wr_double(double value, uchar *ptr) {
	uchar pt[8];
	memcpy(pt,&value,8);
	ptr[0] = pt[7];
	ptr[1] = pt[6];
	ptr[2] = pt[5];
	ptr[3] = pt[4];
	ptr[4] = pt[3];
	ptr[5] = pt[2];
	ptr[6] = pt[1];
	ptr[7] = pt[0];
	return;
}

void GIPL::print_header(){
	cout << "Header information of " << filename         << endl;
	cout << "-------------------------------------------"<< endl;
	cout << "infoline        :\t" << header.infoline     << endl;
	cout << "image_type      :\t" << header.image_type   << " ("
		 << header.TypeDesc[header.image_type] 	<< ")"	 << endl;
	cout << "dimension       :\t" << dimension_val       << endl;
	cout << "dims[0]         :\t" << header.dims[0]      << endl;
	cout << "dims[1]         :\t" << header.dims[1]      << endl;
	cout << "dims[2]         :\t" << header.dims[2]      << endl;
	cout << "dims[3]         :\t" << header.dims[3]      << endl;
	cout << "pixdim[0]       :\t" << header.pixdim[0]    << endl;
	cout << "pixdim[1]       :\t" << header.pixdim[1]    << endl;
	cout << "pixdim[2]       :\t" << header.pixdim[2]    << endl;
	cout << "pixdim[3]       :\t" << header.pixdim[3]    << endl;
	cout << "min             :\t" << header.min          << endl;
	cout << "max             :\t" << header.max          << endl;
	cout << "origin[0]       :\t" << header.origin[0]    << endl;
	cout << "origin[1]       :\t" << header.origin[1]    << endl;
	cout << "origin[2]       :\t" << header.origin[2]    << endl;
	cout << "origin[3]       :\t" << header.origin[3]    << endl;
	cout << "matrix          :\t ";
	for (int i=0;i<20;i++) cout   << header.matrix[i]    << ' ';
	cout << '\n';
	cout << "flag1           :\t" << header.flag1        << endl;;
	cout << "flag2           :\t" << header.flag2        << endl;
	cout << "pixval_offset   :\t" << header.pixval_offset << endl;
	cout << "pixval_cal      :\t" << header.pixval_cal   << endl;
	cout << "sizeof one item :\t" << header.userdef1
		 << " (for GIPL_USER_DEFINED only)"              << endl;
	cout << "num. of arrays  :\t" << header.userdef2     << endl;
	cout << "magic_number \t:\t"  << header.magic_number << endl << endl;
}

void GIPL::set_standard_header(){

	header.TypeDesc[1]   = "GIPL_BINARY";
	header.TypeDesc[7]   = "GIPL_CHAR";
	header.TypeDesc[8]   = "GIPL_U_CHAR";
	header.TypeDesc[15]  = "GIPL_SHORT";
	header.TypeDesc[16]  = "GIPL_U_SHORT";
	header.TypeDesc[31]  = "GIPL_U_INT";
	header.TypeDesc[32]  = "GIPL_INT";
	header.TypeDesc[64]  = "GIPL_FLOAT";
	header.TypeDesc[65]  = "GIPL_DOUBLE";
	header.TypeDesc[144] = "GIPL_C_SHORT";
	header.TypeDesc[160] = "GIPL_C_INT";
	header.TypeDesc[193] = "GIPL_C_DOUBLE";
	header.TypeDesc[200] = "GIPL_SURFACE";
	header.TypeDesc[201] = "GIPL_POLYGON";
	header.TypeDesc[255] = "GIPL_USER_DEFINED";


	header.dims[0] = 1;
	header.dims[1] = 1;
	header.dims[2] = 1;
	header.dims[3] = 1;
	header.image_type = GIPL_NONE; //GIPL_U_CHAR; //uchar [0;255]
	header.pixdim[0] = 0.; // dimensions of each pixel
	header.pixdim[1] = 0.;
	header.pixdim[2] = 0.;
	header.pixdim[3] = 0.0;
	strncpy(header.infoline,"No Patient information.",80); // comment
	for (int i=0;i<20;i++) header.matrix[i] = 0.0;
	header.flag1 = UNDEFINED;
	header.flag2 = UNDEFINED;
	header.min = 0.0;
	header.max = 255.0;
	header.origin[0] = 0.0;
	header.origin[1] = 0.0;
	header.origin[2] = 0.0;
	header.origin[3] = 0.0;
	header.pixval_offset = 0.0;
	header.pixval_cal = 0.0;
	header.userdef1 = 0.0;	/// size of the datatype used in bytes (not standard)
	header.userdef2 = 1.0; 	/// number of different data stored in GIPL (not standard)
	header.magic_number = GIPL_MAGIC_NUMBER1;

	this->set_dimension_data();
}

/**
 * name: save_header
 *
 * Creates a file with the header matching to an GIPL-Object and returns
 * the pointer to the file. Because this function just writes the header
 * the written file is not complete and though the file is not closed.
 *
 * \param fn : file name of the target file
 * \return Pointer to the file.
 */
FILE* GIPL::save_header(string fn){

	if (header.image_type == GIPL_NONE)
		throw GIPL_exception(NO_GIPL_TYPE_FAILURE);
	FILE *ou;
	uchar buf[512];
	size_t bytes = 0;

	ou = fopen(fn.c_str(),"w");

	//!START: write the header
	for (int i=0;i<4;i++) {
		mk_wr_unsigned_short(header.dims[i],buf);
		bytes += 2 * fwrite(buf,2,1,ou);
	}

	mk_wr_unsigned_short(header.image_type,buf);
	bytes += 2 * fwrite(buf,2,1,ou);

	for (int i=0;i<4;i++) {
		mk_wr_float(header.pixdim[i],buf);
		bytes += 4* fwrite(buf,4,1,ou);
	}

	bytes += 80 * fwrite(header.infoline,80,1,ou);

	for (int i=0;i<20;i++) {
		mk_wr_float(header.matrix[i],buf);
		bytes += 4* fwrite(buf,4,1,ou);
	}

	bytes += fwrite(&(header.flag1),1,1,ou);
	bytes += fwrite(&(header.flag2),1,1,ou);
	mk_wr_double(header.min,buf);
	bytes += 8*fwrite(buf,8,1,ou);
	mk_wr_double(header.max,buf);
	bytes += 8*fwrite(buf,8,1,ou);

	for (int i=0;i<4;i++) {
		mk_wr_double(header.origin[i],buf);
		bytes += 8*fwrite(buf,8,1,ou);
	}

	mk_wr_float(header.pixval_offset,buf);
	bytes += 4*fwrite(buf,4,1,ou);
	mk_wr_float(header.pixval_cal,buf);
	bytes += 4*fwrite(buf,4,1,ou);
	mk_wr_float(header.userdef1,buf);
	bytes += 4*fwrite(buf,4,1,ou);
	mk_wr_float(header.userdef2,buf);
	bytes += 4*fwrite(buf,4,1,ou);

	mk_wr_unsigned_int(header.magic_number,buf);
	bytes += 4*fwrite(buf,4,1,ou);
	header_size_val = bytes;
	//!END: write the header
	return ou; //return the file pointer

	//// write the data
	//fwrite(matrix,sizeof(matrix[0]),items_val,ou);
	//fclose(ou);
}

/** name: GIPL::size_of_item()
 *
 * Returns the size of one item depending on the datatype set
 * @return size
 **/
size_t GIPL::size_of_item(){
	switch (header.image_type){
		case GIPL_BINARY	:  return sizeof(bool);  break;
		case GIPL_CHAR 		:  return sizeof(char);  break;
		case GIPL_U_CHAR 	:  return sizeof(uchar); break;
		case GIPL_SHORT		:  return sizeof(short); break;
		case GIPL_U_SHORT	:  return sizeof(ushort);break;
		case GIPL_INT		:  return sizeof(int);   break;
		case GIPL_U_INT		:  return sizeof(uint);  break;
		case GIPL_FLOAT		:  return sizeof(float); break;
		case GIPL_DOUBLE	:  return sizeof(double);break;
		case GIPL_USER_DEFINED:
			return (size_t) header.userdef1; break;
		default: throw(GIPL_TYPE_ERROR);
	}
}

