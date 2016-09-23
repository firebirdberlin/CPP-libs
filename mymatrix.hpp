/*
 *      mymatrix.h
 *
 *      Copyright 2009 Stefan Fruhner <jesus@computer>
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


#ifndef MYMATRIX_H
#define MYMATRIX_H

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <limits.h>
#include <map>
#include <math.h>
#include <string>
#include <vector>
#include <utility>
#include <algorithm>
#include "mystring.hpp"
#include "mystack.h"
#include "my_regular_grid.hpp"

using namespace std;

enum mymatrix_exceptions {PIXDIM_FAILURE, INDEX_OUT_OF_RANGE_FAILURE,
							DIMENSION_FAILURE, INVALID_SLICE_FAILURE};

class mymatrix_exception : public exception {
	mymatrix_exceptions id;
	public:
		mymatrix_exception(mymatrix_exceptions i) : id(i) {}
		virtual const char* what() const throw(){
			switch (id){
				case PIXDIM_FAILURE:
					return "mymatrix: Pixel dimensions must be set with mymatrix::pixdim()."; break;
				case INDEX_OUT_OF_RANGE_FAILURE:
					return "mymatrix: Index out of range."; break;
				case DIMENSION_FAILURE:
					return "mymatrix: Dimension mismatch."; break;
				case INVALID_SLICE_FAILURE:
					return "mymatrix: The slice you've chosen is invalid."; break;
				default:
					return "mymatrix: Unclarified Exception."; break;
			}
		};

};
#ifndef print
#define print(x) (cout << __FILE__ << ":" << __FUNCTION__ <<"() (" <<__LINE__ << ") : " << x << endl)
#endif
#define absolute(a) ((a >= (Type)0.) ? (a) : (-a))
#define NX dimensions[0]
#define NY dimensions[1]
#define NZ dimensions[2]
#define NT dimensions[3]

template <class T1, class T2>
class sPair: public pair<T1, T2>{
	bool operator() (T1 i,T2 j) { return (i<j);}
};

/** \class mymatrix
 * Template class for working with pixel data from images (e.g. GIPL)
 **/
template <class Type>
class mymatrix: public my_regular_grid{

	private:
		/* add your private declarations */
		Type *matrix;
		void init_mymatrix();

	public:
		mymatrix(int dimx, int dimy=1, int dimz=1, int dimt=1) :
		  my_regular_grid(dimx,dimy,dimz, dimt), matrix(0) {
			init_mymatrix();
		};

		mymatrix(int *dims) :
		  my_regular_grid(dims[0],dims[1],dims[2],dims[3]), matrix(0){
			init_mymatrix();
		};

		mymatrix(my_regular_grid &grid):
		 my_regular_grid(grid.dims(0),grid.dims(1),grid.dims(2),grid.dims(3)),
		 matrix(0){
			this->dims(	grid.dims(0),
						grid.dims(1),
						grid.dims(2),
						grid.dims(3));
			this->pixdim(grid.pixdim());
			this->origin(grid.origin());
//			this->pixdim_set = grid.pixdim_set;
			init_mymatrix();
		}

		mymatrix(mymatrix &source) :
		  my_regular_grid(source.dims(0),source.dims(1),source.dims(2),source.dims(3)),
		  matrix(0) { // copy constructor
			#ifdef DEBUG
				print("Coyping matrix ...");
			#endif
			init_mymatrix();

			//copy origin and pixdim
			for (int i = 0; i < 4; i++) v_pixdim[i] = source.pixdim(i);
			for (int i = 0; i < 4; i++) v_origin[i] = source.origin(i);
			pixdim_set = source.pixdim_set;

			//copy the data
			for (unsigned long i = 0; i < items_val ; i++)
			 		this->matrix[i] = source.matrix[i];
			#ifdef DEBUG
				print("..ready.");
			#endif
		};

		~mymatrix(){
			#ifdef DEBUG
				print("Freeing matrix");
			#endif
			delete[] this->matrix;
			#ifdef DEBUG
				print("..ready.");
			#endif
		};

		/** mymatrix<Type>::clear()
		  *
		  * Set all values in the matrix to 0.
		  */
		void clear(){
			for (size_t i = 0; i < items_val ; i++) matrix[i] = 0;
		}


		Type& operator[](unsigned long index){
			if (index < items_val)
				return this->matrix[index];
			else
				return this->matrix[0];
		}

		Type& at(unsigned long index){ return this->operator[](index);}

		bool equal_sized(mymatrix<Type> &other);

		bool operator==(mymatrix<Type> &other);

		bool read_data(string filename, int header_size = 0, FILE * ou = 0);
		bool save_to_file(FILE* ou);
		bool save_to_file(string fn);

		bool assign(Type *data);
		bool append(mymatrix<Type> &matrix2, int size2=-1);
		bool extend(mymatrix<Type> &matrix2);
		bool extend_t(mymatrix<Type> &matrix2);

		Type* get_slice(unsigned int which);

		bool adjust_contrast(Type new_min, Type new_max);

		double mean_value();
		mymatrix<double>* gradientd(unsigned int dx=1, unsigned int dy=0, unsigned int dz=0, unsigned int dt=0);
		void gradient(unsigned int dx=1, unsigned int dy=0, unsigned int dz=0, unsigned int dt=0);
		void median(int px, int type=0);
		void rank(int px);
		void binarize(Type threshold);
		void average(int px);
		void detect_edgesXY(Type epsilon= (Type)0.);
		void detect_HIGH_edgesXY(Type epsilon= (Type)0.);

		bool add(mymatrix<Type> &other, bool only_add_where_different=false);
		bool sub(mymatrix<Type> &other);
		mymatrix<char>* mask(const Type threshold);

		void minmax(Type &min, Type &max);
		bool save_histogram(string fn, bool inc_null = true);
		void save_to_pm3dmap(string fn, unsigned int which_slice, bool redblue=false, bool png=false);
		void flood_fill(Type color, size_t idx);
		void flood_fill_recursive(Type color, size_t idx, bool first);
		void resolution2x();
		void info();
		Type* data(){return matrix;};
};

/**
 * name: mymatrix::read_data
 *
 * Read matrix data from a binary saved file. The data has to be
 * organized such that x varies fastest, then y,z and t.
 *
 * We have to know the header_size, to seek to the right position in
 * the file. If a pointer to a file is given and is != NULL then the
 * filename is ignored. Files are always closed after reading.
 *
 * @param filename	 : The name of the file (including path)
 * @param header_size: Size of the file_header in bytes. (see gipl.h)
 * 					   (optional, 0 if not given)
 * @param ou		 : File pointer of a file, which contains the data
 * 					  (optional -- file is opened if needed)
 * @return
 */
template <class Type>
bool mymatrix<Type>::read_data(string filename, int header_size, FILE * ou){
	delete[] this->matrix; //! free the memory

	this->set_dimension_data();

	#ifdef DEBUG
		print("Reading "<< filename << " with " << nr_slices << " x " << this->slice_size_val <<" items.");
	#endif

	this->matrix = new Type[items_val];

	if (!ou){
		ou = fopen(filename.c_str(), "r");
		if (!ou) return false;
		fseek(ou, header_size, SEEK_SET); //! seek to the end of the header
	}
	if (!ou) return false;

	size_t items_read = fread(this->matrix, sizeof(Type), items_val, ou);	  //! read all data
	fclose(ou);
	#ifdef DEBUG
		print("...ready");
	#endif
	if (items_read != items_val)
		cerr << "Warning : less items read, than expected ( "<< items_read <<"<"<< items_val<<" )." << endl;
	return true;
}

/** name: save_to_file
 *
 *	Saves a matrix to a file with a give file pointer.
 *	After writing the file is closed.
 *
 *	\param ou : pointer to an (open) file
 *	\return True or False if writing was successful
 *
 * */
template <class Type>
bool mymatrix<Type>::save_to_file(FILE* ou){
	#ifdef DEBUG
		print("Saving to file with file pointer");
	#endif
	this->set_dimension_data();

	if (!ou) {
		print("Warning: Abort because empty file pointer !");
		return false; // if a NULL file pointer was given
	}
	// write the data
	size_t items_written = fwrite(this->matrix,sizeof(this->matrix[0]),items_val,ou);
	fclose(ou);
	#ifdef DEBUG
		print(".. ready.");
	#endif
	if (items_written != items_val)
		cerr << " Warning : Items written (" << items_written << ") not equals the items to be written: "<< items_val << endl;
	return true;
}

template <class Type>
bool mymatrix<Type>::save_to_file(string fn){
	#ifdef DEBUG
		print("Saving to file with name "<< fn);
	#endif
	FILE *ou = 0;
	this->set_dimension_data();

	ou = fopen(fn.c_str(),"w");
	if (!ou) {
		print("Warning: Abort because empty file pointer !");
		return false; // if a NULL file pointer was given
	}

	// write the data
	fwrite(this->matrix,sizeof(Type),items_val,ou);
	fclose(ou);
	return true;
}

/**
 * name: mymatrix::operator ==
 * Compares two matrices if they are equal sized and their values are
 * equal.
 * @param other : The matrix to compare with.
 * @return True, only if the are equal in size and grey values.
 */
template <class Type>
bool mymatrix<Type>::operator==(mymatrix<Type> &other){
	if (equal_sized(other) == true)
		return (memcmp(matrix, other.matrix, sizeof(Type)*items_val) == 0);
	else return false;
}

/**
 * name: mymatrix::equal_sized
 * Checks if two matrices have the same dimensions.
 * @param other : The matrix to compare with.
 * @return True, only if 4 the values for dimension are equal.
 */
template <class Type>
bool mymatrix<Type>::equal_sized(mymatrix<Type> &other){
	bool res = false;
	for (int i = 0; i < 4; i++)	{
		res = (dims(i) == other.dims(i));
		if (res == false) return res;
	}
	return res;
}

template <class Type>
void mymatrix<Type>::init_mymatrix(){
	this->matrix = new Type[items_val]; // init with NULL
	for (unsigned long i = 0; i < items_val; i++) matrix[i] = (Type)0;
	//! compute the distances that are needed to find the direct neighbours of an item.
	dx = index(1,0,0,0);
	dy = index(0,1,0,0);
	dz = index(0,0,1,0);
	dt = index(0,0,0,1);
}

/** assign
 * Assigns some matrix data to the matrix object. The number
 * of items is not checked. The user is responsible for
 * checking the dimension of the matrix.
 *
 * \param data : the data to be assigned
 * \return true or false
 **/
template <class Type>
bool mymatrix<Type>::assign(Type *data){
	#ifdef DEBUG
		print("Assigning data");
	#endif
	delete[] this->matrix; // clear matrix buffer
	this->matrix = data;   // assign new data

	#ifdef DEBUG
		print("... ready.");
	#endif
	return true;
}

/** name: mymatrix::append
 *  Appends to a matrix another matrix. Can only be performed,
 * 	if both matrices are of the same dimension.
 *
 * \param matrix2	: the second slice
 * \param size2		: size of the matrix 2 (standard value -1 which means size2=items_val)
 * \return true or false
 **/
template <class Type>
bool mymatrix<Type>::append(mymatrix<Type> &matrix2, int size2){

	if (this->dimension_val != matrix2.dimension_val){
		cerr << "Error: Cannot append! Dimensions don't match." << endl;
		throw mymatrix_exception(DIMENSION_FAILURE);
	}

	/*if size1 and size2 == -1 then the complete size should be used*/
	if (size2 == -1){
		size2 = matrix2.items_val;
		this->dimensions[dimension_val]++;
	}

	#ifdef DEBUG
		print("Appending matrix with size val "<< size2);
	#endif

	Type *buf = new Type[items_val + size2]; // get memory for buffer
	/** copy matrix1 into the buffer
	 *  \attention the third argument to memcopy is the size in bytes !*/
	memcpy( buf         , this->matrix, items_val * sizeof(Type) );
	//! copy matrix2 into the buffer starting from the first address which should be empty
	memcpy((buf + items_val), matrix2.matrix, size2 * sizeof(Type) );

	this->assign(buf);	   // assign
	this->set_dimension_data();

	#ifdef DEBUG
		print("New matrix with size");
		cout << "[";
		for (int i = 0; i < 4; i++)
			cout << dimensions[i] << " ";
		cout << "]" << endl;
	#endif
	return true;
}

/** name: mymatrix::extend
 * Extends a matrix by one slice, e.g. another set of values
 * for a different time.
 *
 * \param matrix2	: the second slice
 * \return true or false
 **/
template <class Type>
bool mymatrix<Type>::extend(mymatrix<Type> &matrix2){

	unsigned int size2 = matrix2.items_val;
	unsigned int slices2 = 0;
	unsigned int szval = this->slice_size_val;


	// Determine if the second matrix is of the same or a lower dimension
	if (matrix2.dim() == this->dim() - 1) slices2 = 1;
	else
	if (matrix2.dim() == this->dim()) slices2 = matrix2.slices();
	else throw mymatrix_exception(DIMENSION_FAILURE);

	// check for correct size of one slice
	if ( slice_size_val * slices2 !=  size2 ){
		cerr << "Error: Cannot extend matrix! Dimensions don't match to slice size." << endl;
		cerr << " Tried to append  "  <<  size2      << " x " << slices2 << " items,"  << endl;
		cerr << " but slice_size is " <<  slice_size_val << " ."  << endl;
		return false;
	}

//			cout << "Extending matrix" << endl;
//			cout << " appending  "  <<  size2      << " x " << slices2 << " items,"  << endl;
//			cout << " and slice_size is " <<  slice_size_val << " ."  << endl;

	#ifdef DEBUG
		print("Extending matrix with size val "<< size2);
	#endif

	Type *buf = new Type[items_val + size2]; // get memory for buffer
	/** copy matrix1 into the buffer
	 *  \attention the third argument to memcopy is the size in bytes !*/
	memcpy( buf             , this->matrix  , items_val * sizeof(Type) );
	//! copy matrix2 into the buffer starting from the first address which should be empty
	memcpy((buf + items_val), matrix2.matrix,     size2 * sizeof(Type) );

	this->assign(buf);	// assign
	if (this->nr_slices > 0 ) this->dimensions[this->dimension_val-1]+= slices2; // count the new part

	this->nr_slices += slices2;
	this->items_val += size2;
	this->slice_size_val = szval;
//			this->info();

	#ifdef DEBUG
		print("New matrix with size");
		cout << "[";
		for (int i = 0; i < 4; i++)
			cout << dimensions[i] << " ";
		cout << "]" << endl;
	#endif
	return true;
}

/** name: mymatrix::extend
 * Extends a matrix by one slice, e.g. another set of values
 * for a different time.
 *
 * \param matrix2	: the second slice
 * \return true or false
 **/
template <class Type>
bool mymatrix<Type>::extend_t(mymatrix<Type> &matrix2){
	unsigned int szval1  = dimensions[0] * dimensions[1] * dimensions[2];
	unsigned int szval2  = matrix2.dims(0) * matrix2.dims(1) * matrix2.dims(2);

	unsigned int size2   = matrix2.dims(3) * szval2;

	// Determine if the second matrix has the same spatial discretization
	if (szval1 != szval2) throw mymatrix_exception(DIMENSION_FAILURE);

	#ifdef DEBUG
		print("Extending matrix with size "<< size2);
	#endif

	Type *buf = new Type[items_val + size2]; // get memory for buffer
	/** copy matrix1 into the buffer
	 *  \attention the third argument to memcopy is the size in bytes !*/
	memcpy( buf             , this->matrix  , items_val * sizeof(Type) );
	//! copy matrix2 into the buffer starting from the first address which should be empty
	memcpy((buf + items_val), matrix2.matrix,     size2 * sizeof(Type) );

	this->assign(buf);						// assign buffer
	this->dimensions[3] += matrix2.dims(3); // update # time slices
	this->nr_slices 	 = dimensions[3];	// which equals # slices now
	this->items_val 	+= size2;			// update # items
	this->slice_size_val = szval1;			// update slice size
	this->dimension_val  = 4;				// we now have 4d data, because of different time values
	dx = index(1,0,0,0);					// update the differences in indices
	dy = index(0,1,0,0);
	dz = index(0,0,1,0);
	dt = index(0,0,0,1);

	#ifdef DEBUG
		print("New matrix with size");
		cout << "[";
		for (int i = 0; i < 4; i++)
			cout << dimensions[i] << " ";
		cout << "]" << endl;
	#endif
	return true;
}


/**
 * name: mymatrix::gradientd
 *
 * Computes the gradient of an image. The gradient is always
 * computed as mid-point gradient over three points. The
 * direction of the gradient is set by the parameters dx, dy,
 * dz and dt. For example with the setting [1,0,0,0] a pure
 * x-gradient is computed, [0,1,0,0] it will become a
 * y-gradient. [1,1,0,0] is in xy-direction.
 *
 *
 * The result is return as pointer of mymatrix<double>, so the returned object
 * must be deleted by the user using the delete command
 *
 * All differences are computed from left to right (smaller
 * values to higher values in [x,y,z,t]). So positive changes
 * mean an increase and negative changes a decrease. If the
 * paramter absgradient is set to true, then negative
 * differences will be made positive (So the information of the
 * direction gets lost.),
 *
 * Finally an array of type 'Type' is returned
 * \param dx : items to compute the difference to in the left and right x-direction
 * \param dy : same as dx, but in y-direction
 * \param dz : same as dx, but in z-direction
 * \param dt : same as dt, but in t-direction
 * \return An array of type 'Type' with the same number of items as the matrix.
 */
template <class Type>
mymatrix<double>* mymatrix<Type>::gradientd(unsigned int dx, unsigned int dy, unsigned int dz, unsigned int dt){

	//mymatrix<double> *grad_intern = new mymatrix<double>(dims(0), dims(1), dims(2), dims(3));
	mymatrix<double> *grad_intern = new mymatrix<double>(*this);
	//! compute the gradient
	for (unsigned int t = dt; t < NT-dt; t++){
	 for (unsigned int k = dz; k < NZ-dz; k++){
	  for (unsigned int j = dy; j < NY-dy; j++){
	   for (unsigned int i = dx; i < NX-dx; i++){

		 unsigned int id1 = index(i-dx,j-dy,k-dz, t-dt);
		 unsigned int idx = index(i   ,j   ,k   , t   );
		 unsigned int id3 = index(i+dx,j+dy,k+dz, t+dt);

		 double d1 = (double) matrix[idx] - (double)matrix[id1];
		 double d2 = (double) matrix[id3] - (double)matrix[idx];

		 grad_intern->operator[](idx) = d1 + d2 / 2.; 		// mid-point gradient
	   }
	  }
	 }
	}
	return grad_intern;
}

/**
 * name: mymatrix::gradient
 *
 * Similar as mymatrix::gradientd but does not return anything. The result is
 * stored in this->matrix.
 *
 * Since the datatype may not contain negative values, the result is adjusted to
 * [0:255]
 *
 * \param dx : items to compute the difference to in the left and right x-direction
 * \param dy : same as dx, but in y-direction
 * \param dz : same as dx, but in z-direction
 * \param dt : same as dt, but in t-direction
 * \return An array of 'Type' with the same number of items as the matrix.
 */
template <class Type>
void mymatrix<Type>::gradient(unsigned int dx, unsigned int dy, unsigned int dz, unsigned int dt){

	mymatrix<Type> 		temp(*this);
	mymatrix<double> 	*grad_intern = this->gradientd(dx,dy,dz,dt);
	const int max_range = 255;
	grad_intern->adjust_contrast(0, max_range);


	#ifdef DEBUG
		print("Copying values");
	#endif
	#pragma omp parallel for
	for (long i = 0; i < (long) items_val; i++)
		matrix[i] 	= (Type) grad_intern->operator[](i);
	#ifdef DEBUG
		print("... ready");
	#endif
	delete grad_intern;
}

/** median
 *
 * A median filter: computes the median color in the window of
 * egde length 2*px+1, and sets this color for the central pixel.
 *
 * \param px   : pixel size of the window
 * \param type : which type of median shall be computed
 * 				 0: standard mid color is chosen
 * 				 1: min
 * 				 2: max
 *				 3: mid - zero value
 *				 4: max - zero value
 */
template <class Type>
void mymatrix<Type>::median(int px, int type){

	my_regular_grid grid = *this;
	mymatrix<Type> result(grid);
	int items = (2*px+1)*(2*px+1);
	Type *colors = new Type[(2*px+1)*(2*px+1)];
	for (unsigned int j = px; j < dims(1)-px; j++){
	for (unsigned int i = px; i < dims(0)-px; i++){
		int c = 0;
		for (unsigned int y = j-px; y < j+px+1 ; y++)
		for (unsigned int x = i-px; x < i+px+1 ; x++){
			unsigned int idx = index(x,y,0,0);
			colors[c++] = this->operator[](idx);
		}

		// sortieren
		for (int c1 = 0; c1 < items -1 ; c1++)
			for (int c2 = c1+1; c2 < items ; c2++){
				if ( colors[c1] > colors[c2] ) {
					unsigned short tmp = colors[c1];
					colors[c1] = colors[c2];
					colors[c2] = tmp;
				}
		}
		unsigned int idx = index(i,j,0,0);
		//median_out[idx] = colors[8]-colors[0] ;
		switch (type){
		case 0:
			result[idx] = colors[items/2];
			break;
		case 1:
			result[idx] = colors[0] ;
			break;
		case 2:
			result[idx] = colors[items];
			break;
		case 3:
			result[idx] = colors[items/2]-colors[0] ;
			break;
		case 4:
			result[idx] = colors[items-1]-colors[0] ;
			break;
		}


		//median_out[idx] = colors[items] ;
		//cout << colors[8] << endl;
		}
	}

	// copy all entries
	for (unsigned int i = 0; i < this->items() ; i++)
		this->operator[](i) = result[i];


	delete[] colors;
}


/** rank
 *
 * Rangbildberechnung
 *
 * \param px   : (1/2 pixel size of the window) - 1
 */
template <class Type>
void mymatrix<Type>::rank(int px){
	for (unsigned int j = px; j < dims(1)-px+1; j += 2*px+1)
	for (unsigned int i = px; i < dims(0)-px+1; i += 2*px+1){
		vector< sPair<Type, int> > colors;
		for (unsigned int y = j-px; y < j+px+1 ; y++)
		for (unsigned int x = i-px; x < i+px+1 ; x++){
			unsigned int idx = index(x,y,0,0);
			sPair<Type, int> p;
			p.first  = this->operator[](idx);
			p.second = idx;
			colors.push_back(p);
		}

		sort(colors.begin(), colors.end());
		Type last_valid_rank = 0;
		for (unsigned int it = 0; it < colors.size() ; it++){
			int idx = colors[it].second;
			if (it > 0 and  colors[it].first == colors[it-1].first){
				this->operator[](idx) = last_valid_rank;
			}
			else {
				this->operator[](idx) = (Type) it;
				last_valid_rank = (Type) it;
			}
		}
	}
}

/** rank
 *
 * Binaerbildberechnung
 *
 * \param threshold   : threshold - values below => 0 and values above => 1
 */
template <class Type>
void mymatrix<Type>::binarize(Type threshold){
	for (unsigned int i = 0; i < items(); i++){
		matrix[i] =  (matrix[i] < threshold) ? 0 :1;
	}
}

/**
 * name: mymatrix::add
 *
 * Add the grey values of two matrices,
 *
 * @param other: the second matrix
 * @param only_add_where_different : only add value where matrices
 * 		  differ (default: false)
 * @return
 */
template <class Type>
bool mymatrix<Type>::add(mymatrix<Type> &other, bool only_add_where_different){

	if (equal_sized(other)){
		if (only_add_where_different == false){
			#pragma omp parallel for
			for (long i = 0; i < (long) items_val; i++)
				matrix[i] += other.matrix[i];
		}
		else {
			#pragma omp parallel for
			for (long i = 0; i < (long) items_val; i++)
				if (matrix[i] - other.matrix[i] != (Type)0)
					matrix[i] += other.matrix[i];
		}
	return true;
	}

return false;
}

/**
 * name: mymatrix::sub
 *
 * Subtract the grey values of two matrices,
 *
 * @param other: the second matrix
 * @return
 */
template <class Type>
bool mymatrix<Type>::sub(mymatrix<Type> &other){
	if (equal_sized(other)){
		for (unsigned long i = 0; i < items_val; i++)
			matrix[i] -= other.matrix[i];
	return true;
	}
return false;
}

/**
 * name: mymatrix::mask
 *
 * Creates a mask using a simple threshold value. If the value in the
 * matrix is > t then 1 will be applied -- 0 otherwise.
 *
 * @param other: the second matrix
 * @return
 */
template <class Type>
mymatrix<char>* mymatrix<Type>::mask(const Type threshold){
	my_regular_grid grid(*this);
	mymatrix<char> *mask = new mymatrix<char>(grid);
	for (unsigned long i = 0; i < items_val; i++){
		mask->operator[](i) = (matrix[i] > threshold ) ? (char) 1 : (char) 0;
	}
	return mask;
}

/** average
 *
 * Averages all pixel values within a window of edge length 2*px+1.
 *
 * \param px   : half of the pixel size of the window
 */
template <class Type>
void mymatrix<Type>::average(int px){

	my_regular_grid grid = *this;
	mymatrix<Type> result(grid);
	int items = (2*px+1)*(2*px+1);

	// get global minimum and maximum
	Type gmin, gmax;
	minmax(gmin, gmax);

	for (unsigned int j = px; j < dims(1)-px; j++)
	for (unsigned int i = px; i < dims(0)-px; i++){
		double color = 0;
		Type mean ;
		Type min, max;
		int cnt = 0;
		for (unsigned int y = j-px; y < j+px+1 ; y++)
		for (unsigned int x = i-px; x < i+px+1 ; x++){
			unsigned int idx = index(x,y,0,0);
			if (cnt++ == 0){
				min = this->operator[](idx);
				max = this->operator[](idx);
			}
			else{
				min = (this->operator[](idx) < min ) ? this->operator[](idx): min;
				max = (this->operator[](idx) > max ) ? this->operator[](idx): max;
			}
			color += (double) this->operator[](idx);
		}

		unsigned int idx = index(i,j,0,0);
		mean = (Type) color/ items;
		result[idx] = mean;

		//unsigned int idx = index(i,j,0,0);
		//Type gmean =  (Type) (((double) gmin + (double) gmax) / 2.);
		//cout << this->operator[](idx) << " " << gmean << " " << mean << endl;
		////if (this->operator[](idx) > mean) result[idx] = mean;

		//if (this->operator[](idx) > mean and mean < gmean) result[idx] = 0;
		////else
		////if (this->operator[](idx) < mean and mean > gmean) result[idx] = 255;
		//else
			//result[idx] = mean;

		//if (this->operator[](idx) > mean) result[idx] = mean;
		//else
		//result[idx] = this->operator[](idx);
		//result[idx] = 0;
	}

	// copy all entries
	for (unsigned int i = 0; i < this->items() ; i++)
		this->operator[](i) = result[i];
}

/** name:mymatrix::get_slice()
 * Filters one slice of of gipl matrix, which can denote the z
 * component of the volume or the time.
 *
 * \param which: the matrix to be sliced
 * \return the matrix that is the slice
 **/
template <class Type>
Type* mymatrix<Type>::get_slice(unsigned int which){

	// check the slice nr.
	if (which >= this->nr_slices){
		cerr << which << " refers to no valid slice in [0,"<<this->nr_slices-1<<"].)" << endl;
		return 0;
	}
	Type *buf = new Type[this->slice_size_val]; // get memory for buffer

	/** copy the complete slice into slice->matrix
	 * \attention (whole_matrix + this->slice_size * which) is the pointer to the first element of interest
	 * \attention the third argument to memcopy is the size in bytes !
	 **/
	memcpy(buf, (this->matrix + this->slice_size_val * which), this->slice_size_val * sizeof(Type) );

	return buf;
}

/**
 * name: mymatrix::adjust_contrast
 *
 * Rearrages all grey values to another range [new_min, new_max].
 * So increasing the range new_max - new_min increases the
 * contrast in the image, ohterwise the contrast is decreased.
 *
 * This function can also be used to convert data between
 * different data types.
 *
 * First the minimum and maximum value is computed. The all
 * values are rescaled.
 *
 * @param new_min: new minimal value
 * @param new_max: new maximal value
 * @return True if something was done,
 * false if new_max < new_min.
 */
template <class Type>
bool mymatrix<Type>::adjust_contrast(Type new_min, Type new_max){
	#ifdef DEBUG
		print("Adjusting contrast");
	#endif
	if (new_max <= new_min) return false;

	Type mini, maxi;
	this->minmax(mini, maxi);

	Type diff = maxi - mini;
	Type new_diff = new_max - new_min;

	double ratio =  ((double) new_diff/ (double )diff);
	#pragma omp parallel for
	for (long i = 0; i < (long) items_val; i++){
		matrix[i] -= mini;
		matrix[i] = (Type) (matrix[i]  * ratio);
		matrix[i] += new_min;
		}
	#ifdef DEBUG
		print("...ready.");
	#endif
	return true;
}

template <class Type>
double mymatrix<Type>::mean_value(){
	double value;
	for (unsigned long i = 0;i < items_val ;i++ ) value += (double)matrix[i];

	value /= (double) items_val;
	return value;
}


template <class Type>
void mymatrix<Type>::detect_edgesXY(Type epsilon){
	bool *check = new bool[items_val];

	for (unsigned long i = 0;i < items_val ;i++ ) check[i] = false;

	//! compute the gradient
	for (unsigned int t = 0; t < NT; t++){
	 for (unsigned int k = 0; k < NZ; k++){
	  for (unsigned int j = 1; j < NY-1; j++){
	   for (unsigned int i = 1; i < NX-1; i++){
		 unsigned int idx = index(i,j,k,t);

		//! check sourrounding pixels, if they are equal
		for (unsigned short l=i-1;l < i+2 ;l++){
			for (unsigned short m=j-1;m < j+2 ;m++ ){
				unsigned int testindex = index(l,m,k,t);

				/**\attention because we can use unsigned datatypes we cannot compute the absolute value (there are no negative values))*/
				if (matrix[testindex] >= matrix[idx])
					check[idx] = (matrix[testindex] - matrix[idx] <= epsilon);
				else
					check[idx] = (matrix[idx] - matrix[testindex] <= epsilon);
				//check[idx] = (matrix[testindex] == matrix[idx]);
				if (not check[idx]) break;
			}
			if (not check[idx]) break;
		}
	   }
	  }
	 }
	}

	for (unsigned long i = 0; i < items_val ;i++ ){
		matrix[i] = (check[i] == true)? (Type) 0 : matrix[i];
	}
	delete[] check;
}

template <class Type>
void mymatrix<Type>::detect_HIGH_edgesXY(Type epsilon){

	double mean = this->mean_value();
	cout << "mean = " << mean << endl;

	bool *check = new bool[items_val];

	for (unsigned long i = 0;i < items_val ;i++ ) check[i] = false;

	//! compute the gradient
	for (unsigned int t = 0; t < NT; t++){
	 for (unsigned int k = 0; k < NZ; k++){
	  for (unsigned int j = 1; j < NY-1; j++){
	   for (unsigned int i = 1; i < NX-1; i++){
		 unsigned int idx = index(i,j,k,t);

		//if ( (double) matrix[idx] < mean ) {
			//matrix[idx] = (Type) 255;
			//continue; // ignore small values
		//}

		//! check sourrounding pixels, if they are equal
		for (unsigned short l=i-1;l < i+2 ;l++){
			for (unsigned short m=j-1;m < j+2 ;m++ ){
				unsigned int testindex = index(l,m,k,t);

				/**\attention because we can use unsigned datatypes we cannot compute the absolute value (there are no negative values))*/
				if (matrix[testindex] >= matrix[idx])
					check[idx] = (matrix[testindex] - matrix[idx] <= epsilon);
				else
					check[idx] = (matrix[idx] - matrix[testindex] <= epsilon);
				//check[idx] = (matrix[testindex] == matrix[idx]);
				//if (not check[idx]) break;
			}
			//if (not check[idx]) break;
		}
	   }
	  }
	 }
	}

	//this->adjust_contrast(0, 100);

	for (unsigned long i = 0; i < items_val ;i++ ){
		matrix[i] = (check[i] == true)? 0: matrix[i];
	}
	delete[] check;
}


template <class Type>
void mymatrix<Type>::minmax(Type &min, Type &max){
	min = this->matrix[0];
	max = this->matrix[0];

	for (unsigned long i = 0; i < items_val; i++){
		min = ( this->matrix[i] < min ) ? this->matrix[i] : min;
		max = ( this->matrix[i] > max ) ? this->matrix[i] : max;
	}
	return;
}

/**
 * name: mymatrix::save_histogram
 *
 * Counts all gray values in an image and saves a text file
 *
 * In the textfile there are two colums
 * 'color value'	'nr of these values in the image'
 *
 * @param fn : Name of the textfile to write.
 * @param inc_null: if true, then all 0 values will be included
 * 					else they will be ignored.
 * @return true
 */
template <class Type>
bool mymatrix<Type>::save_histogram(string fn, bool inc_null){
	#ifdef DEBUG
		print("Saving histogram");
	#endif
	map<Type, int> Histogram;

	//compute the histogram
	for (unsigned long j = 0; j < items_val; j++) Histogram[matrix[j]]++;

	/** save the histogram */
	ofstream outfile;
	outfile.open(fn.c_str());
	outfile << "#color value" << "\t" << "times counted" << endl;

	for (typename map<Type, int>::iterator it=Histogram.begin() ; it != Histogram.end(); it++ ){
		if ((not inc_null) and ( (it->first) == (Type) 0)) continue;

		// convert chars to numbers
		if (typeid(Type) == typeid(char) or typeid(Type) == typeid(unsigned char))
			outfile << (int) it->first << "\t" << (*it).second << endl;
		else
			outfile <<       it->first << "\t" << (*it).second << endl;
	}
	outfile.close();
	#ifdef DEBUG
		print("... ready.");
	#endif
	return true;
	}

/**
 * name: mymatrix::save_to_pm3dmap
 *
 * Create a pm3d map that can be plotted with gnuplot
 *
 * The file is saved in matrix form.
 * @param fn 			: Name of the textfile to write.
 * @param which_slice	: which slice
 * @param redblue		: force color scale from red to blue
 * @param png 			: forces png output instead of x11
 * @return Nothing.
 */
template <class Type>
void mymatrix<Type>::save_to_pm3dmap(string fn, unsigned int which_slice, bool redblue, bool png){

	switch (dim()){
	case 2:
	case 3:
		if (which_slice >= dims(2)){
			info();
			cerr << which_slice << " " << dims(2)<< endl;
			throw mymatrix_exception(INVALID_SLICE_FAILURE);
		}
		break;
	default:{
		cerr << "Not implemented for dimensions != 3 ." << endl;
		throw mymatrix_exception(DIMENSION_FAILURE);
		}
	}

	// write file containing data values as matrix (in gnuplot style)
	ofstream out;
	out.open(fn.c_str());
	for (unsigned int j = 0; j < dims(1); j++){ // y = columns
		for (unsigned int i = 0; i < dims(0); i++){ // x = rows
			unsigned int idx = index(i,j,which_slice);
			out << this->at(idx) << " ";
		}
	out << endl;
	}
	out.close();

	// gnuplot command file
	ofstream gnu;
	mystring gnufile(fn);
	gnufile = gnufile.file_base() + ".gnu";
	mystring pngfile(fn);
	pngfile = pngfile.file_base(true) + ".png";
	mystring fn2(fn);
	fn2 = fn2.file_base(true) + ".dat";

	gnu.open(gnufile.c_str());
	gnu << "set terminal x11" << endl;
	if (not png) gnu << "#";
	gnu << "set terminal png" << endl;
	if (not png) gnu << "#";
	gnu << "set output \""<< pngfile <<"\"\n" << endl;
	gnu << "set pm3d map"<< endl;

	gnu << "set xtics (" ;
	int xstep = dims(0)/4;
	for (unsigned int i = 0; i < dims(0); i+=xstep){
		gnu << "\""<< origin(0) + i * pixdim(0) << "\"" << " " << i <<", ";
	}
	gnu << "\""<< origin(0) + dims(0) * pixdim(0) <<"\"" << " " << dims(0) <<" )" << endl;

	gnu << "set ytics (" ;
	int ystep = dims(1)/4;
	for (unsigned int i = 0; i < dims(1); i+=ystep){
		gnu << "\""<< origin(1) + i * pixdim(1) <<"\"" << " " << i <<", ";
	}
	gnu << "\""<< origin(1) + dims(1) * pixdim(1) <<"\"" << " " << dims(1) <<" )" << endl;

	gnu << "set yrange [] reverse" << endl; // reverse the y-axis

	Type minimum=0.,maximum=0.;
	this->minmax(minimum, maximum);

	if (not redblue) gnu << "#";
	gnu << "set palette defined ("<< minimum <<" \"blue\", 0 \"white\", "<< maximum  << " \"red\") " << endl;

	gnu << "splot \""<< fn2 << "\" matrix" << endl;

	if (png) gnu << "#";
	gnu << "pause -1" << endl;

	gnu.close();

	if (png) { //! call gnuplot to create png
		mystring fng =  "cd " +gnufile.file_path() +" && gnuplot "
						+ gnufile.file_base(true) + ".gnu";
		int res = system(fng.c_str());
		if (res == -1) cerr << "Error: Could not execute " << fng << endl;
	}

	return;
}

/** name: flood_fill
 * Fills a matrix with a certain color value unless a order is found.
 *  - avoiding recursion, which increases stability
 *  - implemented for xy planes for the moment.
 * 	- threshhold can be imlemented by adding a range to color_start check
 * \param color	: The color value to fill the data
 * \param itm	: The index of the pixel which should be coloured first
 */
template <class Type>
void mymatrix<Type>::flood_fill(Type color, size_t itm){
	if (itm >= items()) return;  // index was out of range

	if (dim() != 2 ){
		cerr << "Error: The function flood fill is implemented for 2d only" << endl;
		throw mymatrix_exception(DIMENSION_FAILURE);
	}

	Type color_start = matrix[itm];		// start pixel: save it's color

	if (color_start == color) return; // color is already set

	myStack<size_t> stack;
	stack.push(itm); // put the start element on the stack

	while ( not stack.empty() ) {
		size_t idx = stack.takeout();
//		print(stack.size());
		if ( matrix[idx] == color_start) { // ckeck the color
			matrix[idx] = color;		// change the color

			stack.push(idx + dx);		// put the neigbours into the stack
			stack.push(idx - dx);		//! \attention movement is only in xy plane
			stack.push(idx + dy);
			stack.push(idx - dy);
		}
	}
	return;
}

 /** name: flood_fill_recursive
 * Fills a matrix with a certain color value unless a order is found.
 *  - recursive algorithm
 *  - implemented for xy planes for thr moment.
 * 	- threshhold can be imlemented by adding a range to color_start check
 * \todo Test for iteration depth > segfaults when depth is to high
 * \param color	: The color value to fille the data
 * \param itm	: The index of the pixel which should be coloured
 * \param first : Should be true if called from outside. Recursive calls shall
 * 				  call flood_fill with first=false)
 */
template <class Type>
void mymatrix<Type>::flood_fill_recursive(Type color, size_t itm, bool first){

	static Type color_start;
	static int iter = 0;

	if (first) iter = 0;
	iter++;
//	print(itm << " " << items());
	print(iter);
	if ((itm < 0) || (itm >= items())) return;  // index was out of range

	if ( first ) color_start = matrix[itm];		// if startign pixel then save it's color

	if ( matrix[itm] != color_start) return; 	// stop if color doesn't match
	else matrix[itm] = color;					// else set the color

	// fill the neighbouring pixels
	flood_fill(color, itm + dx, false);
	flood_fill(color, itm - dx, false);
	flood_fill(color, itm + dy, false);
	flood_fill(color, itm - dy, false);
	return;
}


template <class Type>
/** name: mymatrix::resolution2x
 * Increases the resolution by the factor 2.
**/
void mymatrix<Type>::resolution2x(){
	my_regular_grid grid = *this;
	grid.dims(grid.dims(0) * 2,grid.dims(1) * 2,grid.dims(2) * 2, grid.dims(3));
	mymatrix<Type> result(grid);


	for (unsigned int t = 0; t < dims(3) ; t++){
	for (unsigned int k = 0; k < dims(2); k++){
	for (unsigned int j = 0; j < dims(1); j++){
	for (unsigned int i = 0; i < dims(0); i++){
		unsigned int idx   = index(i,j,k,t);
		result[result.index(2*i  ,2*j  ,2*k  ,t  )] = this->operator[](idx);
		result[result.index(2*i+1,2*j  ,2*k  ,t  )] = this->operator[](idx);
		result[result.index(2*i  ,2*j+1,2*k  ,t  )] = this->operator[](idx);
		result[result.index(2*i+1,2*j  ,2*k  ,t  )] = this->operator[](idx);
		result[result.index(2*i+1,2*j+1,2*k  ,t  )] = this->operator[](idx);
		result[result.index(2*i  ,2*j  ,2*k+1,t  )] = this->operator[](idx);
		result[result.index(2*i  ,2*j+1,2*k+1,t  )] = this->operator[](idx);
		result[result.index(2*i+1,2*j  ,2*k+1,t  )] = this->operator[](idx);
		result[result.index(2*i+1,2*j+1,2*k+1,t  )] = this->operator[](idx);
	}
	}
	}
	}

	this->dims(grid.dims(0),grid.dims(1),grid.dims(2), grid.dims(3));
	this->pixdim(grid.pixdim(0)*0.5 ,grid.pixdim(1)*0.5,grid.pixdim(2)*0.5, grid.pixdim(3));


	if (dims(0) > 1 and pixdim(0) == 0.) this->pixdim(1.		, pixdim(1)	,pixdim(2)	,pixdim(3) );
	if (dims(1) > 1 and pixdim(1) == 0.) this->pixdim(pixdim(0)	, 1.		,pixdim(2)	,pixdim(3) );
	if (dims(2) > 1 and pixdim(2) == 0.) this->pixdim(pixdim(0)	, pixdim(1)	,1.			,pixdim(3) );
	if (dims(3) > 1 and pixdim(3) == 0.) this->pixdim(pixdim(0)	, pixdim(1)	,pixdim(2)	,1.		   );




	delete[] this->matrix;
	this->matrix = new Type[items_val]; // init with NULL
	for (unsigned int i = 0; i < items(); i++)
		this->operator[](i) = result[i];


}

template <class Type>
void mymatrix<Type>::info(){
		cout << "dimensions[0] :\t" << this->dimensions[0]  << endl;
		cout << "dimensions[1] :\t" << this->dimensions[1]  << endl;
		cout << "dimensions[2] :\t" << this->dimensions[2]  << endl;
		cout << "dimensions[3] :\t" << this->dimensions[3]  << endl;
		cout << "slice_size    :\t" << this->slice_size_val << endl;
		cout << "nr_slices     :\t" << this->nr_slices      << endl;
		cout << "items         :\t" << this->items_val      << endl;
		cout << "dimensions    :\t" << this->dimension_val  << endl;
		cout << "pixdim[0]     :\t" << this->v_pixdim[0]    << endl;
		cout << "pixdim[1]     :\t" << this->v_pixdim[1]    << endl;
		cout << "pixdim[2]     :\t" << this->v_pixdim[2]    << endl;
		cout << "pixdim[3]     :\t" << this->v_pixdim[3]    << endl;
		cout << "origin[0]     :\t" << this->v_origin[0]    << endl;
		cout << "origin[1]     :\t" << this->v_origin[1]    << endl;
		cout << "origin[2]     :\t" << this->v_origin[2]    << endl;
		cout << "origin[3]     :\t" << this->v_origin[3]    << endl;
		cout << "pixdim_set    :\t" << this->pixdim_set     << endl;

		cout << "dx            :\t" << this->dx             << endl;
		cout << "dy            :\t" << this->dy             << endl;
		cout << "dz            :\t" << this->dz             << endl;
		cout << "dt            :\t" << this->dt             << endl;
}

#undef absolute
#undef print
#undef NX
#undef NY
#undef NZ
#undef NT

#endif /* MYMATRIX_H */
