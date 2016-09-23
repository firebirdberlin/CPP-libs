//		./algorithm.hpp
//
//		Copyright 2012 Stefan Fruhner <stefan.fruhner@gmail.com>
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

#ifndef __MY_ALGORITHM_HPP
#define __MY_ALGORITHM_HPP

#include <vector>
#include <algorithm>

namespace mylibs{
/** \namespace algorithm
 * \brief Contains some template functions for arrays and vectors.
 */
namespace algorithm {

template <class T>
T min(const T *v,const size_t len);
template <class T>
T min(const std::vector<T>& v);

template <class T>
T max(const T *v,const size_t len);
template <class T>
T max(const std::vector<T>& v);

template <class T>
size_t min_element(const T *v,const size_t len);
template <class T>
size_t max_element(const T *v,const size_t len);

template <class T>
size_t min_element(const std::vector<T> &v);
template <class T>
size_t max_element(const std::vector<T> &v);

template <class T>
void minmax(const T *v,const size_t len, T &gmin, T &gmax);
template <class T>
void minmax(const std::vector<T>& v, T &gmin, T &gmax);

template <class T>
void moving_average(T *v,const size_t len,const size_t window=1);

/** \fn template <class T> T min(const T *,const size_t)
 * \brief Extract the minimum of an array.
 *
 * This function will return the minimum value within an array of
 * template type T.
 * If _OPENMP is defined the search will be parallelized with openmp,
 * otherwise the corresponding std-function will be used.
 * If the length of the given array is zero, a myexception is thrown.
 *
 * \param v: Array from which to determine the minimum.
 * \param len: The length of the array.
 * \return The minimum value within the array.
 */
template <class T>
T min(const T *v,const size_t len){
	if (len == 0) throw myexception(EXCEPTION_ID + "Array length is 0 !");
#ifdef _OPENMP
	T shared_min = v[0];
	#pragma omp parallel
	{
		T min = v[0];
		#pragma omp for nowait
		for(size_t ii=0; ii<len; ++ii){
			min = (v[ii] < min) ? v[ii] : min;
		}
		#pragma omp critical
		{
			shared_min = (shared_min < min) ? shared_min : min;
		}
	}
	return shared_min;

#else // no openmp
	return *std::min_element(v, v+len);
#endif
}

/** \fn template <class T> T min(const std::vector<T> &)
 * \brief Extract the minimum of a std:vector.
 *
 * This function will return the minimum value within a std::vector of
 * template type T.
 *
 * \param v: std::vector from which to determine the minimum.
 * \return The minimum value within the vector.
 */
template <class T>
T min(const std::vector<T>& v){
	return min(&v[0], v.size());
}

/** \fn template <class T> T max(const T *,const size_t)
 * \brief Extract the minimum of an array.
 *
 * This function will return the minimum value within an array of
 * template type T.
 * If _OPENMP is defined the search will be parallelized with openmp,
 * otherwise the corresponding std-function will be used.
 * If the length of the given array is zero, a myexception is thrown.
 *
 * \param v: Array from which to determine the minimum.
 * \param len: The length of the array.
 * \return The minimum value within the array.
 */
template <class T>
T max(const T *v,const size_t len){
	if (len == 0) throw myexception(EXCEPTION_ID + "Array length is 0 !");

#ifdef _OPENMP
	T shared_max = v[0];
	#pragma omp parallel
	{
		T max = v[0];
		#pragma omp for nowait
		for(size_t ii=0; ii<len; ++ii){
			max = (v[ii] > max) ? v[ii] : max;
		}
		#pragma omp critical
		{
			shared_max = (shared_max > max) ? shared_max : max;
		}
	}
	return shared_max;

#else // no openmp
	return *max_element(v, v+len);
#endif
}

/** \fn template <class T> T max(const std::vector<T> &)
 * \brief Extract the minimum of a std:vector.
 *
 * This function will return the maximum value within a std::vector of
 * template type T.
 * This method uses the variant for an array.
 *
 * \param v: std::vector from which to determine the maximum.
 * \return The maximum value within the vector.
 */
template <class T>
T max(const std::vector<T>& v){
	return max(&v[0], v.size());
}

/** \fn template <class T> void minmax(const T *, const size_t, T &, T &)
 * \brief Determine minimum and maximum value within an array.
 *
 * This method will determine the minimum and maximum value within an
 * array of a template parameter T.
 * If _OPENMP is defined, this method will use openmp for
 * parallelization, otherwise std-functions are used.
 *
 * \param v: The array from which to determine the minimum and maximum
 *			value.
 * \param len: The length of the array.
 * \param gmin, gmax: References to values of type T, where the minimum
 *			and maximum are stored, respectively.
 */
template <class T>
void minmax(const T *v, const size_t len, T &gmin, T &gmax){
#ifdef _OPENMP
	gmax = v[0];
	gmin = v[0];
	#pragma omp parallel
	{
		T max = v[0];
		T min = v[0];
		#pragma omp for nowait
		for(size_t ii=0; ii< len; ++ii){
			max = (v[ii] > max) ? v[ii] : max;
			min = (v[ii] < min) ? v[ii] : min;
		}
		#pragma omp critical
		{
			gmax = (gmax > max) ? gmax : max;
			gmin = (gmin < min) ? gmin : min;
		}
	}
	return;
#else // no openmp
	gmax = *max_element(v, v+len);
	gmin = *min_element(v, v+len);
#endif
}

/** \fn template <class T> void minmax(const std::vector<T>&, T &, T &)
 * \brief Determine minimum and maximum value within an std::vector.
 *
 * This method uses the variant for an array.
 *
 * \param v: The vector from which to determine the minimum and maximum
 *			value.
 * \param gmin, gmax: References to values of type T, where the minimum
 *			and maximum are stored, respectively.
 */
template <class T>
void minmax(const std::vector<T>& v, T &gmin, T &gmax){
	minmax(&v[0], v.size(), gmin, gmax);
}


/** \fn template <class T> size_t min_element(const T *,const size_t)
 * \brief Extract the minimum of an array and return the index of the
 * associated element.
 *
 * This function will return the index of the the minimum value within
 * an array of template type T.
 * If _OPENMP is defined the search will be parallelized with openmp,
 * otherwise the corresponding std-function will be used.
 * If the length of the given array is zero, a myexception is thrown.
 *
 * \param v: Array from which to determine the minimum.
 * \param len: The length of the array.
 * \return The index of the minimum value within the array.
 */
template <class T>
size_t min_element(const T *v,const size_t len){
	if (len == 0) throw myexception(EXCEPTION_ID + "Array length is 0 !");
#ifdef _OPENMP
	T 	   shared_min = v[0];
	size_t shared_idx =   0;
	#pragma omp parallel
	{
		T min 		 = v[0];
		size_t index =   0;
		#pragma omp for nowait
		for(size_t ii=0; ii<len; ++ii){
			if (v[ii] < min){min = v[ii]; index = ii;}
		}
		#pragma omp critical
		{
			if (shared_min > min) {
				shared_min = min;
				shared_idx = index;
			}
		}
	}
	return shared_idx;

#else // no openmp
	return std::distance(v,*std::min_element(v, v+len));
#endif
}

template <class T>
size_t min_element(const std::vector<T> &v){
	return min_element(&v[0], v.size());
}

template <class T>
size_t max_element(const std::vector<T> &v){
	return max_element(&v[0], v.size());
}

/** \fn template <class T> size_t max_element(const T *,const size_t)
 * \brief Extract the maximum of an array and return the index of the
 * associated element.
 *
 * This function will return the index of the the maximum value within
 * an array of template type T.
 * If _OPENMP is defined the search will be parallelized with openmp,
 * otherwise the corresponding std-function will be used.
 * If the length of the given array is zero, a myexception is thrown.
 *
 * \param v: Array from which to determaxe the maximum.
 * \param len: The length of the array.
 * \return The index of the maximum value within the array.
 */
template <class T>
size_t max_element(const T *v,const size_t len){
	if (len == 0) throw myexception(EXCEPTION_ID + "Array length is 0 !");
#ifdef _OPENMP
	T 	   shared_max = v[0];
	size_t shared_idx =   0;
	#pragma omp parallel
	{
		T max 		 = v[0];
		size_t index =   0;
		#pragma omp for nowait
		for(size_t ii=0; ii<len; ++ii){
			if (v[ii] > max){max = v[ii]; index = ii;}
		}
		#pragma omp critical
		{
			if (shared_max < max) {
				shared_max = max;
				shared_idx = index;
			}
		}
	}
	return shared_idx;

#else // no openmp
	return std::distance(v,*std::max_element(v, v+len));
#endif
}


/** \fn template <class T> void moving_average(T *, const size_t, const size_t)
 * \brief Compute a moving average for an array.
 *
 * This method computes a moving average of an array. The result is then
 * stored in the given array.
 * The average \f$ \overline{a}_{i} \f$ for the point with index i is calculated
 * via the formula
 * \f[
 *	\overline{a}_{i} =  \frac{1}{2width}\sum\limits_{j = i - width}^{i + width - 1} a_{j}
 * \f]
 * \note The template class has to supply the operators T += T and (T /= size_t).
 *
 * \param v: The array with the values that should be averaged.
 * \param len: The length of the array.
 * \param window: Averaging over the point in the center and the 'window' points
 *		to the left and 'window' points to the right (window = 2, would mean to average
 *		over a total of 5 points: i-2, i-1,i,i+1,i+2).
 */
template <class T>
void moving_average(T *v, const size_t len, const size_t window){
// moving average
	if (v and len > 2*window+1) {
		T *tmp = new T[len];									// tmp memory
		if (! tmp)
			throw myexception(EXCEPTION_ID+ "Out of memory");

		for (size_t k = 0; k < len; ++k) tmp[k] = 0.;		// initialize all values

		for (size_t k = window; k < len-window; ++k) {		// compute moving average
			for (size_t w = k-window; w < k+window+1; ++w) tmp[k] += v[w];
			tmp[k] /= (2*window + 1);
		}

		for (size_t k = 0; k < len ; ++k) v[k] = tmp[k]; // copy averaged values back
		delete[] tmp;
	}
}


} // end of namespace algorithm
} // end of namespace mylibs
#endif
