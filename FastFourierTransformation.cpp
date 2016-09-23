//		./FastFourierTransformation.cpp
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
#include "FastFourierTransformation.hpp"
using namespace std;

/** name: power_of_two
 * Checks if the given int is a power of two
 * @param v: the number you want to check.
 */
bool FastFourierTransformation::power_of_two(int &v) {
	return v && !(v & (v - 1));
}

size_t FastFourierTransformation::next_higher_power2(size_t k) {
	if (k == 0) return 1;
	k--;
	for (size_t i=1; i<sizeof(size_t)*CHAR_BIT; i<<=1)
		k = k | k >> i;
	return k+1;
}


size_t FastFourierTransformation::next_smaller_power2(size_t v) {
	return next_higher_power2(v) >> 1;
}

FastFourierTransformation::FastFourierTransformation(const double* s1, int nr_elements, bool use_zero_padding):
	stride(1), input(s1), input_f(0), output(0), n(nr_elements),
	zero_padding(use_zero_padding), zero_start(nr_elements),
	v_sampling_rate(1.0) {

	if ( not power_of_two(nr_elements)) {
		if (not zero_padding) {
			n = next_smaller_power2(nr_elements);
			//throw string("The number of elements must be a power of two");
		} else {
			n = next_higher_power2(nr_elements);
			zero_start = nr_elements;
		}
		if (n == 0) throw string("# of elements to small (0)");
	}

	output = new double[n];
	if (! output) throw string("Could not alloc memory");
}

FastFourierTransformation::FastFourierTransformation(const float* s1, int nr_elements, bool use_zero_padding):
	stride(1),
	input(0),
	input_f(0),
	output(0), n(nr_elements), zero_padding(use_zero_padding), zero_start(nr_elements),
	v_sampling_rate(1.0) {


	if ( not power_of_two(nr_elements)) {
		if (not zero_padding) {
			n = next_smaller_power2(nr_elements);
			//throw string("The number of elements must be a power of two");
		} else {
			n = next_higher_power2(nr_elements);
			zero_start = nr_elements;
		}
		if (n == 0) throw string("# of elements to small (0)");
	}

	input_f = new double[n];
	for (size_t i = 0; i < (size_t) n; i++) input_f[i] = (double) s1[i];
	input = input_f;

	output = new double[n];
	if (! output) throw string("Could not alloc memory");
}

FastFourierTransformation::~FastFourierTransformation() {
	delete[] input_f;
	delete[] output;

	input  = NULL;
	input_f  = NULL;
	output = NULL;
}
/**
	This function computes an in-place radix-2 FFT of length n and stride
	stride on the real array data. The output is a half-complex sequence,
	which is stored in-place. The arrangement of the half-complex terms uses
	the following scheme: for k < n/2 the real part of the k-th term is
	stored in location k, and the corresponding imaginary part is stored in
	location n-k. Terms with k > n/2 can be reconstructed using the symmetry
	z_k = z^*_{n-k}. The terms for k=0 and k=n/2 are both purely real, and
	count as a special case. Their real parts are stored in locations 0 and n
	/2 respectively, while their imaginary parts which are zero are not
	stored.

	The following table shows the correspondence between the output data and
	the equivalent results obtained by considering the input data as a
	complex sequence with zero imaginary part (assuming stride=1),


		  complex[0].real    =    data[0]
          complex[0].imag    =    0
          complex[1].real    =    data[1]
          complex[1].imag    =    data[n-1]
          ...............         ................
          complex[k].real    =    data[k]
          complex[k].imag    =    data[n-k]
          ...............         ................
          complex[n/2].real  =    data[n/2]
          complex[n/2].imag  =    0
          ...............         ................
          complex[k'].real   =    data[k]        k' = n - k
          complex[k'].imag   =   -data[n-k]
          ...............         ................
          complex[n-1].real  =    data[1]
          complex[n-1].imag  =   -data[n-1]

**/
int FastFourierTransformation::Compute() {
#ifdef HAVE_LIBGSL

	if (not zero_padding) {
		memcpy(output, input, n*sizeof(double));
		int status = gsl_fft_real_radix2_transform (output, 1, n);
		if (status) { /* an error occurred */
			printf ("error: %s\n", gsl_strerror (status));
		}
		return n;
	} else { // zero padding
//		double * rect=new double[n];
//		for (int i = 0; i < n; i++)
//		{
//			rect[i]= (i<zero_start)?1. : 0.;
//		}
//		gsl_fft_real_radix2_transform (rect, stride, n);

		memcpy(output, input, zero_start*sizeof(double));
		for (int i = zero_start; i < n; i++) { // pad zeros at the end of the signal
			output[i] = 0.;
		}

		int status = gsl_fft_real_radix2_transform (output, stride, n);
		if (status) { /* an error occurred */
			printf ("error: %s\n", gsl_strerror (status));
		}
//		for (int i = 0; i < n; i++){
//			output[i] -= rect[i];
//		}
//		delete[] rect;
		return zero_start;
	}

	//— Function: int gsl_fft_real_radix2_transform (double data[], size_t stride, size_t n)
#else
	throw string("To use this function, please link the gsl library into the project.");
#endif
}

int FastFourierTransformation::ComputeInverse(double *data_ptr) {
#ifdef HAVE_LIBGSL
	//— Function: int gsl_fft_halfcomplex_radix2_inverse (double data[], size_t stride, size_t n)
	memcpy(data_ptr, output, n*sizeof(double));
	int status = gsl_fft_halfcomplex_radix2_inverse (data_ptr, stride, n);
	if (status) { /* an error occurred */
		printf ("error: %s\n", gsl_strerror (status));
	}
	return (zero_padding) ? zero_start : n;
#else
	throw string("To use this function, please link the gsl library into the project.");
#endif
}

int FastFourierTransformation::ComputeInverse(float *data_ptr) {
#ifdef HAVE_LIBGSL
	//— Function: int gsl_fft_halfcomplex_radix2_inverse (double data[], size_t stride, size_t n)
	double *tmp = new double[n];
	memcpy(tmp, output, n*sizeof(double));
	int status = gsl_fft_halfcomplex_radix2_inverse (tmp, stride, n);
	if (status) { /* an error occurred */
		printf ("error: %s\n", gsl_strerror (status));
	}
	for (size_t i = 0; i < (size_t) n; i++) data_ptr[i] = (float) tmp[i];
	delete[] tmp;
	return (zero_padding) ? zero_start : n;
#else
	throw string("To use this function, please link the gsl library into the project.");
#endif
}

int FastFourierTransformation::GetRealCoeff(double *data_ptr) {
	int iter_end = (zero_padding) ? zero_start/2 : n/2;
	memcpy(data_ptr, output, iter_end *sizeof(double));

	return iter_end;
}

int FastFourierTransformation::GetImagCoeff(double *data_ptr) {
	int iter_end = (zero_padding) ? zero_start/2 : n/2;
	data_ptr[0] = 0.;
	for (int i = 1; i < n/2; i++) data_ptr[i] = output[n - i];
	return iter_end;
}

int FastFourierTransformation::GetRealCoeff(float *data_ptr) {
	int iter_end = (zero_padding) ? zero_start/2 : n/2;
	for (size_t i = 0; i < (size_t) iter_end; i++) data_ptr[i] = output[i];

	return iter_end;
}

int FastFourierTransformation::GetImagCoeff(float *data_ptr) {
	int iter_end = (zero_padding) ? zero_start/2 : n/2;
	data_ptr[0] = 0.;
	for (int i = 1; i < n/2; i++) data_ptr[i] = (float) output[n - i];

	return iter_end;
}


int FastFourierTransformation::GetPowerSpectrum(double *xaxis, double *data_ptr) {
	int iter_end = (zero_padding) ? zero_start/2 : n/2;
	double c = 1./(double)n / v_sampling_rate;

	data_ptr[0] = output[0]*output[0];
	data_ptr[0] = 20. * log10(data_ptr[0]*c);

	for (int i = 1; i < iter_end; i++) {
		data_ptr[i] = output[i]*output[i] + output[n-i] * output[n-i];
		data_ptr[i] = 20.*log10(data_ptr[i]*c);
	}

	c = 1./n * v_sampling_rate;
	for (int j = 0; j < n/2; j++) xaxis[j] = j * c;

	return iter_end;
}

int FastFourierTransformation::GetPowerSpectrumLinear(double *xaxis, double *data_ptr) {
	int iter_end = (zero_padding) ? zero_start/2 : n/2;
	double c = 1./(double)n / v_sampling_rate;

	data_ptr[0] = output[0]*output[0];
	double max = data_ptr[0];
	for (int i = 1; i < iter_end; i++) {
		data_ptr[i] = output[i]*output[i] + output[n-i] * output[n-i];
		max = (data_ptr[i] > max) ? data_ptr[i]:max;
	}

	// scale to largest magnitude
	for (int i = 0; i < iter_end; i++) data_ptr[i] /= max;

	// compute frequencies
	c = 1./n * v_sampling_rate;
	for (int j = 0; j < n/2; j++) xaxis[j] = j * c;

	return iter_end;
}

int FastFourierTransformation::GetPowerSpectrum(float *xaxis, float *data_ptr) {
	int iter_end = (zero_padding) ? zero_start/2 : n/2;
	double c = 1./(double)n / v_sampling_rate;

	data_ptr[0] = output[0]*output[0];
	data_ptr[0] = 20. * log10(data_ptr[0]*c);

	for (int i = 1; i < iter_end; i++) {
		data_ptr[i] = output[i]*output[i] + output[n-i] * output[n-i];
		data_ptr[i] = 20.*log10(data_ptr[i]*c);
	}

	// compute frequencies
	c = 1./n * v_sampling_rate;
	for (int j = 0; j < n/2; j++) xaxis[j] = j * c;

	return iter_end;
}

int FastFourierTransformation::GetPowerSpectrumLinear(float *xaxis, float *data_ptr) {
	int iter_end = (zero_padding) ? zero_start/2 : n/2;
	double c = 1./(double)n / v_sampling_rate;

	data_ptr[0] = output[0]*output[0];
	double max = data_ptr[0];
	for (int i = 1; i < iter_end; i++) {
		data_ptr[i] = output[i]*output[i] + output[n-i] * output[n-i];
		max = (data_ptr[i] > max) ? data_ptr[i]:max;
	}

	// scale to largest magnitude
	for (int i = 0; i < iter_end; i++) data_ptr[i] /= max;


	c = 1./n * v_sampling_rate;
	for (int j = 0; j < n/2; j++) xaxis[j] = j * c;

	return iter_end;
}

void FastFourierTransformation::CutFrequenciesAbove(int N) {
	for (size_t i = N+1; i < (size_t) n/2 ; i++) {
		output[i] = 0.;
		output[n-i] = 0.;
	}
}

void FastFourierTransformation::CutFrequenciesBelow(int N) {
	N = (N > n/2) ? n/2 : N;
	if (N == 0) return;
	output[0] = 0.;
	for (size_t i = 1; i < (size_t) N ; i++) {
		output[i]   = 0.;
		output[n-i] = 0.;
	}
}


void FastFourierTransformation::CutFrequenciesAbove(double f) {
//	  f = (float) Nj/n * v_sampling_rate;
//<=> Nj = n * f / v_sampling_rate

	int N = n * f / v_sampling_rate;
	for (size_t i = N+1; i < (size_t) n/2 ; i++) {
		output[i]   = 0.;
		output[n-i] = 0.;
	}
}


void FastFourierTransformation::CutFrequenciesBelow(double f) {
//	  f = (float) Nj/n * v_sampling_rate;
//<=> Nj = n * f / v_sampling_rate

	int N = n * f / v_sampling_rate;
	N = (N > n/2) ? n/2 : N;
	if (N == 0) return;
	output[0] = 0.;
	for (size_t i = 1; i < (size_t) N ; i++) {
		output[i]   = 0.;
		output[n-i] = 0.;
	}
}

size_t FastFourierTransformation::Frequency2Index(double f) const {
	//	  f = (float) Nj/n * v_sampling_rate;
	//<=> Nj = n * f / v_sampling_rate
	return n * f / v_sampling_rate;
}

double FastFourierTransformation::Index2Frequency(size_t index) const {
	return index * 1./n * v_sampling_rate;
}
