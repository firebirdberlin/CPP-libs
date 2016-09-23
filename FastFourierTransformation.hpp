//      FastFourierTransformation.hpp
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


#ifndef FASTFOURIERTRANSFORMATION_HPP
#define FASTFOURIERTRANSFORMATION_HPP

#include <stdio.h>
#include <string>
#include <string.h>
#include <limits.h>
#include <math.h>

#ifdef HAVE_LIBGSL
#include <gsl/gsl_errno.h>
#include <gsl/gsl_fft_real.h>
#include <gsl/gsl_fft_halfcomplex.h>
#endif

class FastFourierTransformation {
	public:
		FastFourierTransformation(const double* s1, int nr_elements, bool use_zero_padding = false);
		FastFourierTransformation(const float* s1, int nr_elements, bool use_zero_padding = false);
		virtual ~FastFourierTransformation();

		int Compute();
		int ComputeInverse(double *data_ptr);
		int ComputeInverse(float  *data_ptr);

		int GetRealCoeff(double *data_ptr);
		int GetImagCoeff(double *data_ptr);

		int GetRealCoeff(float *data_ptr);
		int GetImagCoeff(float *data_ptr);

		int GetPowerSpectrum(double *xaxis, double *data_ptr);
		int GetPowerSpectrumLinear(double *xaxis, double *data_ptr);
		int GetPowerSpectrum(float *xaxis, float *data_ptr);
		int GetPowerSpectrumLinear(float *xaxis, float *data_ptr);

		int& size(){return n;}

		// spectral filter for given mode numbers N
		void CutFrequenciesAbove(int N);
		void CutFrequenciesBelow(int N);

		void HighPassFilter(int N) {CutFrequenciesBelow(N);}
		void LowPassFilter(int N) {CutFrequenciesAbove(N);}
		void BandPassFilter(int lower, int upper) {
			CutFrequenciesBelow(lower);
			CutFrequenciesAbove(upper);
		}


		// spectral filter for given freqeuncies in Hz
		void CutFrequenciesAbove(double f);
		void CutFrequenciesBelow(double f);

		void HighPassFilter(double f) {CutFrequenciesBelow(f);}
		void LowPassFilter(double f) {CutFrequenciesAbove(f);}
		void BandPassFilter(double f_lower, double f_upper) {
			CutFrequenciesBelow(f_lower);
			CutFrequenciesAbove(f_upper);
		}

		void   sampling_rate(double v) {v_sampling_rate = v;}
		double sampling_rate() {return v_sampling_rate;}

		size_t Frequency2Index(double f) const;
		double Index2Frequency(size_t index) const;

		static bool   power_of_two(int &v);
		static size_t next_higher_power2(size_t k);
		static size_t next_smaller_power2(size_t v);

	protected:

	private:
		int 			 stride;
		const double	 *input;
		double		   *input_f;
		double 			*output;
		int 				  n;
		bool 	   zero_padding;
		int 		 zero_start;
		double 	v_sampling_rate;
};

#endif //FASTFOURIERTRANSFORMATION_HPP
