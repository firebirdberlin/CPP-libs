//		Array.hpp
//
//		Copyright 2011,2012 Stefan Fruhner <stefan.fruhner@gmail.com>
//		Copyright 2011,2012 Rico Buchholz <buchholz@itp.tu-berlin.de>
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

#ifndef __ARRAY_HPP
#define __ARRAY_HPP

/** \file Array.hpp
 * 	Manage 2D Arrays in memory ...
 */

#include <iostream>
#include <cstring>
#include <ctime>
#include <string.h>
#include "algorithm.hpp"
#include "myio.hpp"


using namespace std;
namespace mylibs {
/** \class Array
 *	\brief Template class for storing data.
 *
 *	Template class for storing data. Storing is within blocks of same variables
 *	(e.g. concentration). Also number of elements and variables is explicitly
 *	stored in class variables.
 *	In summary nr_variables*nr_elements*size(T) byte are needed by an instance
 *  of this class.
 *
 *	To work properly the datatype has to implement at least the following
 *	operators:
 *	- '<' (smaller-than-operator)
 *	- '>' (greater-than-operator)
 *	- '=' (assignment-operator)
 *
 * \note The methods which are related to the netcdf-file-format will only be
 *		present if there was the HAVE_LIBNETCDF-flag set, during creating the
 *		program (which will be the case if the netcdf-libary is found).
 *
 */
template <typename T>
class Array {
	public:
////////////////////////////////////////////////////////////////////////////////
		class DataException : public exception {
			public:
				DataException(std::string s) {err_msg = s;};
				~DataException() throw() {};
				virtual const char* what() const throw() {return err_msg.c_str();};
			private:
				std::string err_msg;
		};
		class NumberOfVariablesInFileDoesNotMatch : public DataException {
			public:
				NumberOfVariablesInFileDoesNotMatch(std::string s) : DataException(s + std::string("The number of variables found in a file and the number of variables of this object differ!\n")) {};
				~NumberOfVariablesInFileDoesNotMatch() throw() {};
		};
		class UnknownDimension : public DataException {
				UnknownDimension(std::string s) : DataException(s + std::string("The dimension of space is unknown/can not be handled!\n")) {};
				~UnknownDimension() throw() {};
		};
		class VariableCouldNotBeRead : public DataException {
			public:
				VariableCouldNotBeRead(std::string s) : DataException(s + std::string("The attempt to read a variable from a file failed!\n")) {};
				~VariableCouldNotBeRead() throw() {};
		};
		class IndexOutOfBounds : public DataException {
			public:
				IndexOutOfBounds(std::string s) : DataException(s + std::string("An index is out of bounds.\n")) {};
				~IndexOutOfBounds() throw() {};
		};
		class FileNotFound : public DataException {
			public:
				FileNotFound(std::string s) : DataException(s + std::string("A file could not be created/found.\n")) {};
				~FileNotFound() throw() {};
		};
		class DimensionNotFound : public DataException {
			public:
				DimensionNotFound(std::string s) : DataException(s + std::string("Dimension is defined but information missing. Maybe the file is corrupted?\n")) {};
				~DimensionNotFound() throw() {};
		};
		class VariableNotFound : public DataException {
			public:
				VariableNotFound(std::string s) : DataException(s + std::string("Variable could not be read. Maybe the file is corrupted or of a different model?\n")) {};
				~VariableNotFound() throw() {};
		};
		class AllocationFailed : public DataException {
			public:
				AllocationFailed(std::string s) : DataException(s + std::string("Memory allocation failed.\n")) {};
				~AllocationFailed() throw() {};
		};
		class NullPointerNotAllowed : public DataException {
			public:
				NullPointerNotAllowed(std::string s) : DataException(s + std::string("Null Pointer not allowed at this place.\n")) {};
				~NullPointerNotAllowed() throw() {};
		};
		class VariableNotWritten : public DataException {
			public:
				VariableNotWritten(std::string s) : DataException(s + std::string("A variable could not be written to the file.\n")) {};
				~VariableNotWritten() throw() {};
		};
////////////////////////////////////////////////////////////////////////////////
		// ***************************************
		// ----- Constructors & destructors ------
		Array(const size_t n, const size_t ele);
		Array(Array<T> const * const other);
		Array(Array<T> const & other);

		~Array();

		// *******************************
		// ----- Object info methods -----
		T* GetData(const size_t n) const;
		T* GetDataUnsafe(size_t const n);
		const	T* GetData(const size_t n, bool const) const;

		const size_t GetNrVariables() const {return nr_variables;};
		const size_t GetItems() const {return nr_elements;};

		bool Save(const string & filename) const;
		bool SaveData_dat(const char* filename) const;
		bool SaveData_raw(const char* filename) const;

		bool Load(string const & filename);
	private:
		bool LoadData_dat(string const & filename);
	public:
		size_t LoadData_raw(const char * filename, const size_t NTraces, const size_t NElements);

		T		GetMin(size_t const index) const;
		T		GetMax(size_t const index) const;
		void	GetMinAndMax(size_t const index, T& min, T& max) const;

		void Info() const;
		void clear();

		// ***************************************
		// ----- Object manipulating methods -----
		void init(size_t varnr, const T& value);
		void append(const T *newdata);
		void CopyDataset(const size_t target, const size_t source);

		void Resize(size_t const elements);
		void Transpose();
//		T& operator()(const size_t& i1, const size_t& i2);

		size_t index(const size_t& i1, const size_t& i2){
			return i1*nr_elements + i2;
		}

	protected:
		void Copy(Array<T> const * const other);

		T * data;
		size_t nr_variables;	//!< Number of variables for each element (e.g. number of concentrations or timesteps).
		size_t nr_elements;		//!< Number of elements (e.g. number of points).

};

////////////////////////////////////////////////////////////////////////////////

/** \fn Array<T>::Data(const size_t, const size_t, const size_t)
 *	Constructor, that need the number of variables and elements. It reserves memory
 *	for variables*elements T's.
 * \param n: The number of variables, e.g different traces.
 * \param ele: The number of elements that should be stored, e.g. time steps.
 */
template <typename T>
Array<T>::Array(const size_t n, const size_t ele)
	:	data(NULL),
	    nr_variables(n),
	    nr_elements(ele){

	// allocate memory
	data = new T[nr_variables*nr_elements];
}

/** \fn Array<T>::Data(Array<T> const * const)
 * \brief Copyconstructor.
 *
 *	This is a copyconstructor which takes a pointer to an other Array<T>-object.
 *	The work is done via the Copy-method.
 *
 * \param other: Pointer to the Data-object that should be copied.
 */
template <typename T>
Array<T>::Array(Array<T> const * const other)
	:	data(NULL),
	    nr_variables(0),
	    nr_elements(0) {
	Copy(other);
}

/** \fn Array<T>::Data(Data const &)
 * \brief Copyconstructor.
 *
 *	This is a copyconstructor which takes a reference to an other Array<T>-
 *	object. The work is done via the Copy-method.
 *
 * \param other: Reference to the Data-object that should be copied.
 */
template <typename T>
Array<T>::Array(Array const & other)
	:	data(NULL),
	    nr_variables(0),
	    nr_elements(0){
	Copy(&other);
}

/** \fn Array<T>::~Data()
 *	Destructor, frees the memory the constructor or the append-method has reserved.
 */
template <typename T>
Array<T>::~Array() {
	//std::cout<<"Will now delete data.\n";
	delete[] data;
	//std::cout<<"Data deleted.\n";
}

/** \fn void Array<T>::append(const T *)
 * \brief Append variable/move the first one out.
 *
 *	Appends data.
 * \attention It is not tested if \a newdata points within the array of this
 *		object. If this is the case the method may not work.
 *
 * \param newdata: Pointer to the array of objects that should be appended.
 */
template <typename T>
void Array<T>::append(const T *newdata) {
	if(nr_variables > 0) {
		// Create a new array which has bigger size.
		T *data2 = new T[(nr_variables + 1)*nr_elements];

		//! Copy the old data in to the new array.
		memcpy( data2, data, nr_variables*nr_elements * sizeof(T));

		/** Copy the new data in the array at the end (until now there are
		 *	nr_variables*nr_elements items in the new array, now copy the
		 *	new nr_elemnts items).
		 */
		memcpy((data2 + (nr_variables*nr_elements)), newdata, nr_elements* sizeof(T));

		// Delete the old data, because we don't need it anymore.
		delete[] data;
		//! We set the pointer of the old data to the new one.
		data = data2;
	} else { // [if(nr_variables>ß)]
		delete[] data;
		data = new T[(nr_variables + 1)*nr_elements];
		memcpy(data, newdata, nr_elements* sizeof(T));
	}

	//! Increase the number of variables
	nr_variables++;

	return;
}

/** \fn T* Array<T>::GetData(const size_t) const
 *
 * Return the address where the variable begins, whose index is given as
 * paramter.
 *
 * \param n: number of the variable
 * \return Pointer to array start
 **/
template <typename T>
inline T* Array<T>::GetData(const size_t n) const {

	if (n < nr_variables) {
		return data + n*nr_elements;
	}

	//! If n exeeds the number of variables, throw an exeption.
	stringstream err;
	err << EXCEPTION_ID
		<< "Trying to retrieve variable #"<<n<<", but there are only "
		<< nr_variables <<endl;
	throw IndexOutOfBounds(err.str());
}

/** \fn T* Array<T>::GetDataUnsafe(size_t const)
 *
 * Return the address where the variable begins, whose index is given as
 * paramter.
 *
 * \attention This version doesn't check if the given index is within bounds, so
 *		you mustn't use this method if you are not sure to be within range.
 *
 * @param n: number of the variable
 *				\attention Has to be within bounds (e.g. lower than
 *					\a nr_varibales and greater than/equal to zero).
 * @return Pointer to array start
 **/
template <typename T>
inline T* Array<T>::GetDataUnsafe(size_t const n) {
	return data + n*nr_elements;
}

/** \fn const T* Array<T>::GetData(const size_t, bool) const
 *
 * Return the address where the variable begins, whose index is given as
 * paramter. This is a constant version, which takes an additional, unused boolean.
 *
 * \param n: number of the variable
 * \return Pointer to array start
 **/
template <typename T>
inline const T* Array<T>::GetData(const size_t n, bool const) const {
	//! If n exeeds the number of variables, throw an exeption.
	if (n >= nr_variables) {
		stringstream err;
		err << EXCEPTION_ID
			<< "Trying to get variable "<<n<<", but there are only "<<nr_variables<<"\n";
		throw IndexOutOfBounds(err.str());
	}

	return data + n*nr_elements;
}

/** \fn void Array<T>::init(const size_t, const T&)
 *	Initializes a block of a variable with a given value.
 *	\param varnr: Number of variable that should be initialized.
 *	\param value: The value with that the variable should be initialized.
 * \return void
 */
template <typename T>
void Array<T>::init(const size_t varnr, const T& value) {
	//! If n exeeds the number of variables, throw an exeption.
	if (varnr >= nr_variables) {
		stringstream err;
		err << EXCEPTION_ID
			<< "Trying to initialize variable "<<varnr
			<< ", but there are only "<<nr_variables<<"\n";
		throw IndexOutOfBounds(err.str());
	}

	//! Go through all points, for everyone the variable with number 'varnr' is set to 'value'.
	for (size_t i = 0; i < nr_elements; i++) {
		*(data + varnr*nr_elements + i ) = value;
	}
}

/** \fn void Array<T>::CopyDataset(const size_t, const size_t)
 *
 *	Copy the dataset with index source to the one with index target.
 *
 * \param target: Index where to write the copy.
 * \param source: Index from which to read the copy.
 */
template <typename T>
void Array<T>::CopyDataset(const size_t target, const size_t source) {
	for(size_t i = 0; i < nr_variables; ++i) {
		*(data + i*nr_elements + target) = *(data + i*nr_elements + source);
	}
}

/** \fn T Array<T>::GetMin(size_t const) const
 *	Finds the minimum value for the variable 'index' in the array.
 *	\note To work properly, the type 'T' has to implement the greater-than- and
 *		the assignment-operator.
 * \param index: The index of the variable for that the minimumvalue should be
 *				determined.
 * \return The minimum value in the index as value.
 */
template <typename T>
inline T Array<T>::GetMin(size_t const index) const {
	size_t i;
	T* ptr_values = data + index*nr_elements;
	T min = *ptr_values;

	for(i = 1; i < nr_elements; ++i) {
		min = (min > *(ptr_values + i)) ? *(ptr_values + i): min;
	}

	return min;
}

/** \fn T Array<T>::GetMax(size_t const) const
 *	Finds the maximum value for the variable 'index' in the array.
 *	\note To work properly, the type 'T' has to implement the smaller-than- and
 *		the assignment-operator.
 * \param index: The index of the variable for that the maximumvalue should be
 *				determined.
 * \return The maximum value in the index as value.
 */
template <typename T>
inline T Array<T>::GetMax(size_t const index) const {
	size_t i;
	T* ptr_values = data + index*nr_elements;
	T max = *ptr_values;

	for(i = 1; i < nr_elements; ++i) {
		max = (max < *(ptr_values + i)) ? *(ptr_values + i): max;
	}

	return max;
}

/** \fn void Array<T>::GetMinAndMax(size_t const, T&, T&) const
 *	Finds the minimum and maximum value for the variable 'index' in the array.
 *	\note To work properly, the type 'T' has to implement the smaller-than-,
 *		greater-than- and the assignment-operator.
 * \param index: The index of the variable for that the maximumvalue should be
 *				determined.
 * \param min:
 * \param max: References to variables with same type as the template-parameter.
 *				The minimum-/maximumvalue found will be stored here.
 */
template <typename T>
inline void Array<T>::GetMinAndMax(size_t const index, T& min, T& max) const {
	T* ptr_values = data + index*nr_elements;

	// replaced by a parallelized version
	mylibs::algorithm::minmax(ptr_values, nr_elements, min, max);
}

/** \fn void Array<T>::Info() const
 * \brief Prints infos about object to standard output.
 *
 *	This method will write information about this object to the standard output.
 *	This includes the pointer \a this and the values of the non-pointer members
 *	(nr_variables and nr_elements).
 */
template <typename T>
void Array<T>::Info() const {
	std::cout<<"Info about data object at "<<data<< "\n";
	std::cout<<"Number variables:      "<<nr_variables<<"\n";
	std::cout<<"Number of elements:    "<<nr_elements<<"(for each variable)\n";
}

/** \fn bool Array<T>::LoadData_dat(string  const &, const PointerStruct &)
 *
 *	Loads data from a text file, with given filename.
 *  Format: Columns of n variables
 * #  u v ...,
 *    1 2 ...
 *    ...
 *    ...
 *    ...
 *
 *  where the number of rows gives the number of elements<br>
 *	Lines starting with '#' are ignored.
 *
 * \param filename: Reference to a string, that contains the filename.
 * \param params: Reference to a PointerStruct-structure. Not used at the moment.
 * \return False to indicate that space size has not changed.
 */
template <typename T>
bool Array<T>::LoadData_dat(string  const & filename) {
	if (filename.size() == 0) return false;

	if (not  mylibs::IO::file_exists(filename.c_str())){
		std::stringstream err;
		err << EXCEPTION_ID << "Opening file " <<filename<<" failed.\n";
		throw FileNotFound(err.str());
	}

	FILE *ptr_file = fopen(filename.c_str(), "r");

	char test[1024] = "#";//!< Buffer variable.
	size_t index = 0;
	while (fgets(test,1024, ptr_file)) {
		mystring line(test);
		line.strip();
		if (line[0] == '#') continue;//!< Skip comment and parameter lines
		vector<mystring> l = line.splitv();
		if (nr_variables != l.size()) { // incorrect number of traces ?
			std::stringstream err;
			err << EXCEPTION_ID
				<<" Input data in file "<<filename<<" invalid. Number of variables expected: "
				<< nr_variables<<" found "<<l.size()<<" within the line '"<< line<<"'.\n";
			throw NumberOfVariablesInFileDoesNotMatch(err.str());
		}

		for (size_t i = 0; i < nr_variables; ++i) {
			l[i] >> *(data + index  + i*nr_elements);
		}
		++index;
	}
	fclose(ptr_file);

	// Up to now a change of the size is not done by this method, so false has
	// to be returned.
	return false;
}

/**
 * \fn bool Array<T>::LoadData_raw( const char*, const PointerStruct &)
 *
 *	Loads data from a binary file with given filename expecting that
 * 	there's no header.
 *
 *  Format:
 *		v1(t1) v2(t1) ...
 *		v1(t2) v2(t2) ...
 *		...
 *		...
 *		...
 * Either the number of variables of the number of elements can be
 * (size_t)-1 which means the appropriate value is automatically
 * computed from the file size.
 *
 * \param filename  : Reference to a string, that contains the filename.
 * \param NTraces   : Number of variables to be read.
 * \param NElements : Number of values per variable to be read, e.g. points in time.
 * \return Number of elements read in
 */
template <typename T>
size_t Array<T>::LoadData_raw(const char* filename, const size_t NTraces, const size_t NElements) {
	if (strlen(filename) == 0) return 0;

	if (not  mylibs::IO::file_exists(filename)){
		std::stringstream err;
		err << EXCEPTION_ID << "Opening file " <<filename<<" failed.\n";
		throw FileNotFound(err.str());
	}
	size_t sz = mylibs::IO::file_size(filename);


	if (NTraces == (size_t) -1) {
		if ((NElements == (size_t)-1) or (sz % (NElements * sizeof(T)) != 0 )){
			cerr << __FUNCTION__ << ": " << "Number of traces does not agree with the file size of "
				 << filename <<"." << endl;
			return 0;
		}
		nr_variables = sz/(NElements * sizeof(T));
		nr_elements  = NElements;
	}

	if (NElements == (size_t) -1) {
		if ((NTraces == (size_t)-1) or (sz % (NTraces * sizeof(T)) != 0 )){
			cerr << __FUNCTION__ << ": " << "Number of traces does not agree with the file size of "
				<< filename <<"." << endl;
			return 0;
		}
		nr_variables = NTraces;
		nr_elements  = sz/(NTraces * sizeof(T));
	}

	delete[] data; data = NULL;
	data = new T[nr_elements*nr_variables];
	FILE *ptr_file = fopen(filename, "r");
	size_t items_read = fread(data,sizeof(T), nr_elements*nr_variables, ptr_file);
	fclose(ptr_file);
	cout << "Items read " << items_read << endl;
	cout << " -> # traces " << nr_variables << endl;
	cout << " -> # values " << nr_elements << endl;

	return nr_elements;
}

/**
 * \fn bool Array<T>::SaveData_raw( const char*, const PointerStruct &)
 *
 *	Loads data from a binary file with given filename expecting that
 * 	there's no header.
 *
 *  Format:
 *		v1(t1) v2(t1) ...
 *		v1(t2) v2(t2) ...
 *		...
 *		...
 *		...
 * Either the number of variables of the number of elements can be
 * (size_t)-1 which means the appropriate value is automatically
 * computed from the file size.
 *
 * \param filename  : Reference to a string, that contains the filename.
 * \return false on error, true otherwise.
 */
template <typename T>
bool Array<T>::SaveData_raw(const char* filename) const {
	if (strlen(filename) == 0) return false;

	FILE *ptr_file = fopen(filename, "wb"); // write binary
	size_t items_written = fwrite(data,sizeof(T), nr_elements*nr_variables, ptr_file);
	fclose(ptr_file);
	if (items_written != nr_elements*nr_variables) {
		cerr << EXCEPTION_ID
			 << "The number of items written differs from the expected value ("
			 << items_written << "/" << nr_elements*nr_variables << endl;
		return false;
	}
	return true;
}

/** bool Array<T>::Load(string const &, const PointerStruct &)
 * \brief Loads data in format depending on extension of given file(-name).
 *
 * \param filename: Reference to a std::string, which gives place and name of
 *				the file from which to load the data. Its extension determines
 *				type.
 */
template <typename T>
bool Array<T>::Load(const string &filename) {
	const mystring ext(mystring(filename).file_ext());

	if((ext == "dat") or (ext == "txt")) return LoadData_dat(filename);

	return LoadData_raw(filename, nr_variables, nr_elements);
}

/** \fn void Array<T>::Save(string const &, SaveInfo const * const) const
 * \brief Saves the data in format depending on extension of given filename.
 *
 * \param filename: Reference to a std::string, which gives place and name of
 *				the file where to save the data. Its extension determines the
 *				type that should be used for storing.
 */
template <typename T>
bool Array<T>::Save(string const & filename) const {
	mystring const ext(mystring(filename).file_ext());
	if(("dat" == ext) or ("txt" == ext)) {
		return SaveData_dat(filename);
	}
	return SaveData_dat(filename);
}

/** \fn bool Array<T>::SaveData_dat(string const &, SaveInfo const * const) const
 * \brief Save data in a text-file.
 *
 *	Writes the data to a text file. At the beginning of the file a
 *	header is placed, containing the number of
 *	variables, the number of elements and a short information about the
 * 	format.
 *
 * \param filename: c string wich contains the filename.
 */
template <typename T>
bool Array<T>::SaveData_dat(const char* filename) const {
	//! If the filename is empty, throw an error.
	if(strlen(filename) == 0) {
		std::stringstream err;
		err << EXCEPTION_ID
			<< " Filename is empty, so file couldn't be opened.\n";
		throw FileNotFound(err.str());
		return false;
	}

	std::ofstream of;
	of.open(filename);

	if(of.good()) {
		//! First some header information.
		time_t seconds_passed;
		time(&seconds_passed);

		string const str_date(ctime(&seconds_passed));
		of << "# "<< str_date << endl;
		of << "# number variables : " << nr_variables << std::endl;
		of << "# number elements  : "  << nr_elements << std::endl;

		of << "## now data with format"  << std::endl;
		of << "## v1 v2 v3 ..." << std::endl;
		size_t i = 0;
		size_t j = 0;

		//! Now write all values for one point in one line and do this for every point.
		for(i = 0; i < nr_elements; ++i) { // one row of data
			for(j = 0; j < nr_variables; ++j) {
				of << data[i + nr_elements * j] << " ";
			}
			of << std::endl;
		}

	} else {
		std::stringstream err;
		err << EXCEPTION_ID << "Opening file for reading data failed.\n";
		throw FileNotFound(err.str());
	}

	of.close();
	return true;
}



/** \fn void Array<T>::Copy(Array<T> const * const)
 * \brief Make this object a copie of another.
 *
 *	This method changes this object, so it is a deep copy of another one.<br>
 *	If no other object is given a exception is thrown. If this object is itself
 *	given, the method will return immadiatly, because it is assumed that a
 *	object is always a copy of itself.<br>
 *	Before copying the data, the old ones are deleted. Then nr_variables,
 *	nr_elements are set. If this is done the new array is created
 *	and then the data is copied.
 *
 * \param other: Pointer to another Array<T> object, which should be copied. If
 *				the pointer is NULL an excpetion is thrown. If it points to
 *				this object, nothing will be made (it is assumed that an object
 *				is ever a copy of itself).
 */
template <typename T>
void Array<T>::Copy(Array<T> const * const other) {

	if(not other) {
		std::stringstream err;
		err << EXCEPTION_ID << " NULL pointer as argument.\n";
		throw NullPointerNotAllowed(err.str());
	}

	//! If other points to this object, there is nothing to do.
	if(this == other) return;

	//! Delete the old data;
	delete[] data; data = NULL;

	//! Set membervariables to the new values.
	this->nr_variables 	= other->GetNrVariables();
	this->nr_elements 	= other->GetItems();

	//! Create the new data-array.
	data = new T[nr_variables*nr_elements];

	//! Copy the data to the new array.
	memcpy(data, other->GetData(0, true), nr_variables*nr_elements* sizeof(T));
}

/** \fn Array<T>::Resize(size_t const)
 * \brief Changes the number of elements.
 *
 *	This method will delete the data-array and create a new one with a given
 *	number of elements (if the new size is different from the old one).
 *
 * \param elements: The new number of elements the array should have per
 *				variable.
 */
template <typename T>
void Array<T>::Resize(size_t const elements) {
	if(nr_elements != elements) {
		delete[] data;
		data = NULL;
		data = new T[nr_variables*elements];

		if(data) 	nr_elements = elements;
		else {
			std::stringstream err;
			err << EXCEPTION_ID
				<< " Trying to create new array with "
				<< nr_variables<<"*"<<elements <<" = "
				<< nr_variables*elements <<" elements.\n";
			throw AllocationFailed(err.str());
		}
	}
}

template <typename T>
void Array<T>::Transpose() {

	T *tmp = new T[nr_variables*nr_elements];

	for (size_t i = 0; i < nr_variables; i++){
		for (size_t j = 0; j < nr_elements; j++){

			tmp[j*nr_variables + i] = data[i*nr_elements + j];
		}
	}

	size_t t     = nr_elements;
	nr_elements  = nr_variables;
	nr_variables = t;


	delete[] data;
	data = tmp;

}


template <typename T>
void Array<T>::clear(){
	delete[] data; data = NULL;
	nr_variables 		= 0;
	nr_elements 		= 0;
}

};  // end of namespace mylibs
#endif
