//		RandomNumber.cpp
//
//		Copyright 2011 Rico Buchholz <buchholz@itp.tu-berlin.de>
//
//		This file is part of mylibs.
//
//		mylibs is free software; you can redistribute it and/or modify
//		it under the terms of the GNU General Public License as published by
//		the Free Software Foundation; either version 2 of the License, or
//		(at your option) any later version.
//
//		mylibs is distributed in the hope that it will be useful,
//		but WITHOUT ANY WARRANTY; without even the implied warranty of
//		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//		GNU General Public License for more details.
//
//		You should have received a copy of the GNU General Public License
//		along with mylibs; if not, write to the Free Software
//		Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
//		MA 02110-1301, USA.

#ifndef RANDOMNUMBER_CPP
#define RANDOMNUMBER_CPP
/** \file RandomNumber.cpp
 * \brief Defines methods of the RandomNumber-class.
 */

#include <math.h>
#include <time.h>

#ifdef HAVE_LIBGSL
#include <gsl/gsl_randist.h>
#endif

#include "RandomNumber.hpp"

RandomNumber::RandomNumber() :
#ifdef HAVE_LIBGSL
	ptr_random_number_generator(NULL),
#endif
	seed(0),
	sigma(1),
	mu(0),
	range_min(0),
	range_max(1),
	amplitude(1),
	probability(0.5),
	type_id(DEFAULT) {
#ifdef HAVE_LIBGSL
	//! Create and start the random number generator.
	ptr_random_number_generator = gsl_rng_alloc(gsl_rng_mt19937);
	type_id = GSL_RNG_MT19937;
	gsl_rng_set(ptr_random_number_generator, 0);
#endif
}

/** \fn RandomNumber::RandomNumber(myIniFiles &)
 * \brief Basic-constructor.
 *	Constructor which takes a ini-file. The pointer for the random number
 *	generator is initialized with NULL. Rest of the work is passed to the
 *	Initialize-method.
 *
 * \param ini: Reference to a myIniFiles-object, which is passed to the Initialize-
 *				method.
 */
RandomNumber::RandomNumber(myIniFiles & ini) :
#ifdef HAVE_LIBGSL
	ptr_random_number_generator(NULL),
#endif
	seed(0),
	sigma(1),
	mu(0),
	range_min(0),
	range_max(1),
	amplitude(1),
	probability(0.5),
	type_id(DEFAULT) {
	Initialize(ini);
}

/** \fn RandomNumber::~RandomNumber()
 *	Destructor. We have only implicit memory allocated, so we deallocate it also
 *	implicit by using the corresponding function of the gsl.
 */
RandomNumber::~RandomNumber() {
#ifdef HAVE_LIBGSL
	gsl_rng_free(ptr_random_number_generator);
	ptr_random_number_generator = NULL;
#endif
}

/** \fn double RandomNumber::DistributionEqual() const
 *
 *	Delivers a floating point number with double precision from the intervall
 *	\f$ [0,1)\f$ and an uniform distribution.
 *
 * \note This method will also work if gsl wasn't found during ccompilation of
 *		the program. Instead of the gsl-function is then the standard random
 *		number generator of c++ used.
 *
 * \return Double in the range \f$ [0,1)\f$ with uniform distribution.
 */
double RandomNumber::DistributionEqual() const {
#ifdef HAVE_LIBGSL
	return gsl_rng_uniform(ptr_random_number_generator);
#else
	return double(rand())/double(RAND_MAX);
#endif
}

/** \fn double RandomNumber::DistributionNormal() const
 *	Delivers a floating point number with double precision which has a normal
 *	probability distribution
 *	\f$ p(x) dx = {1 \over \sqrt{2 \pi \sigma^2}} \exp (-x^2 / 2\sigma^2) dx \f$
 *	Thereby the member-variable 'sigma' is used for the standard deviation
 *	\f$ \sigma \f$. The member-variable 'mu' shifts the mean.
 *
 * \note This method won't work if gsl wasn't found during compilation of the
 *		program.
 *
 * \return Double with normal-distribution which has mean 'mu' and standard
 *				deviation 'sigma'.
 */
double RandomNumber::DistributionNormal() const {
#ifdef HAVE_LIBGSL
	return (gsl_ran_gaussian(ptr_random_number_generator, sigma) + mu);
#else
	throw string("Need gsl");
	return 0.0;
#endif
}

/** \fn double RandomNumber::DistributionNormalCut(double const, double const) const
 *	This method uses DistributionNormal to get an random number. If it is within
 *	the given bounds it is returned, otherwise the corresponding bound is
 *	returned.
 */
double RandomNumber::DistributionNormalCut(double const lower, double const upper) const {
	double const rnd(DistributionNormal());
	if(lower > rnd) return lower;
	if(upper < rnd) return upper;
	return rnd;
}

/** \fn double RandomNumber::DistributionLaplace() const
 *	Delivers a floating point number with double precision which has a laplacian
 *	probability distribution \f$ p(x) dx = {1 \over 2 a}  \exp(-|x/a|) dx \f$.
 *	For the width \f$ a \f$ of the distribution is the member-variable 'sigma'
 *	used.
 *
 * \note This method won't work if gsl wasn't found during compilation of the
 *		program.
 *
 * \return Double with Laplace-distribution which has width 'sigma'.
 */
double RandomNumber::DistributionLaplace() const {
#ifdef HAVE_LIBGSL
	return gsl_ran_laplace(ptr_random_number_generator, sigma);
#else
	throw string("Need gsl");
	return 0.0;
#endif
}

/** \fn double RandomNumber::DistributionFlat() const
 *	Returns a double within a range \f$ [a, b] \f$, whereby the probability
 *	distribution is constant \f$ p(x) dx = {1 \over (b-a)} dx \f$. For the range
 *	borders will be used the member-variables \f$ range_min \f$ and \f$ range_max \f$.
 *
 * \note This method won't work if gsl wasn't found during compilation of the
 *		program.
 *
 * \return A double between \f$ range_min \f$ and \f$ range_max \f$ with a constant
 *				probability distribution.
 */
double RandomNumber::DistributionFlat() const {
#ifdef HAVE_LIBGSL
	return gsl_ran_flat(ptr_random_number_generator, range_min, range_max);
#else
	throw string("Need gsl");
	return 0.0;
#endif
}

/** \fn unsigned int RandomNumber::DistributionPoisson() const
 *	Delivers a unsigned integer, which follows the probability distribution \f$
 *	p(k) = {\mu^k \over k!} \exp(-\mu) \f$ for \f$ k \geq 0\f$. For the mean \f$ \mu \f$
 *	will be the member-variable with the same name used.
 *
 * \note This method won't work if gsl wasn't found during compilation of the
 *		program.
 *
 * \return An unsigned integer which follows a Poisson-distribution.
 */
unsigned int RandomNumber::DistributionPoisson() const {
#ifdef HAVE_LIBGSL
	return gsl_ran_poisson(ptr_random_number_generator, mu);
#else
	throw string("Need gsl");
	return 0;
#endif
}

/** \fn unsigned int RandomNumber::DistributionBernoulli() const
 *	Delivers zero ore one with probability \f$ p(0) = 1 - p\f$ and \f$ p(1) = p\f$.
 *	For the parameter \f$ p \f$ will be the member-variable 'probability' used.
 *
 * \note This method won't work if gsl wasn't found during compilation of the
 *		program.
 *
 * \return Zero ore one, whith a probability distribution which is the above mentioned.
 */
unsigned int RandomNumber::DistributionBernoulli() const {
#ifdef HAVE_LIBGSL
	return gsl_ran_bernoulli(ptr_random_number_generator, probability);
#else
	throw string("Need gsl");
	return 0;
#endif
}

/** \fn void RandomNumber::AddGaussianWhiteNoise(double * const, size_t, bool) const
 *	Adds to a array of doubles with size number gausian white noise.
 *
 * \note This method won't work if gsl wasn't found during compilation of the
 *		program, because it uses DistributionNormal();
 *
 * \param values: Array of doubles, given as pointer.
 * \param number: Number of elements in the array.
 * \param greater_zero: If this is set to true, it is ensured, that each element
 *				remains greater or equal to zero. This is done by generating a new
 *				random number.
 *				Default option is false.
 */
void RandomNumber::AddGaussianWhiteNoise(double * const values, size_t number, bool greater_zero) const {
	size_t i;
	double temp_random;
	double d = 0.003*sqrt(2*sigma*amplitude);

	for(i = 0; i < number; ++i) {
		temp_random = d*DistributionNormal();

		if(true == greater_zero) {
			while(0 >= (*(values + i) + temp_random)) {
				temp_random = d*DistributionNormal();
			}
		}

		*(values + i) += temp_random;
	}
}

/** \fn void RandomNumber::SetSeed(size_t const s)
 *	Sets the seed from a given size_t.
 * \param s: The new value for the seed.
 */
void RandomNumber::SetSeed(size_t const s) {
	seed = s;
}

/** \fn void RandomNumber::SetSeed(myIniFiles &)
 *	Sets the seed from a mystring-object. If the string contains a option-character
 *	it is handled. In every case SetSeed(size_t) called, with an appropiate value.
 * \param ini: Reference to a myIniFiles-object, from which the option or value
 *				for the seed is taken.
 */
void RandomNumber::SetSeed(myIniFiles& ini) {
	ini.set_active_section("RandomNumber");

	mystring temp_str(ini.read("seed", mystring("")));

	if(temp_str == "t") {
		time_t TimeSeed;
		time(&TimeSeed);

		SetSeed(size_t(TimeSeed));
		return;
	}

	SetSeed(ini.read("seed", seed));

	ini.unset_active_section();
}

/** \fn void RandomNumber::SetSigma(double const)
 *	Sets the 'sigma'-member-variable.
 * \param s: The new value for the variable. Has to be positive.
 */
void RandomNumber::SetSigma(double const s) {
	if(0 < sigma) {
		sigma = s;
	}
}

/** \fn void RandomNumber::SetMu(double const)
 *	Sets the 'mu'-member-variable.
 * \param m: The new value for the variable.
 */
void RandomNumber::SetMu(double const m) {
	mu = m;
}

/** \fn void RandomNumber::SetRangeMin(double)
 * \param min: New value for the minimum of the range.
 */
void RandomNumber::SetRangeMin(double const min) {
	range_min = min;
}

/** \fn void RandomNumber::SetRangeMax(double const)
 * \param max: New value for the maximum of the range.
 */
void RandomNumber::SetRangeMax(double const max) {
	range_max = max;
}

/** \fn void RandomNumber::SetAmplitude(double const)
 * \param amp: New value for the amplitude.
 */
void RandomNumber::SetAmplitude(double const amp) {
	if(0 < amp) {
		amplitude = sqrt(2*amp);
	}
}

/** \fn void RandomNumber::SetProbability(double const)
 * \param prob: New value for the probability.
 */
void RandomNumber::SetProbability(double const prob) {
	if((0 <= prob)  and (1 >= prob)) {
		probability = prob;
	}
}

/** \fn void RandomNumber::Register(myIniFiles &)
 * 	Registers all parameters needed by this class in the given ini-file.
 * \param ini: Reference to a ini-file, where the parameters should be registered.
 */
void RandomNumber::Register(myIniFiles& ini) {
	ini.set_active_section("RandomNumber");

	ini.register_param("seed",		NULL, "(Integer) Value for the seed of the random number generator.", true);
	ini.register_param("sigma",		NULL, "(double) Standarddeviation(or width, depending on distribution) for non-equal-distributions.", true);
	ini.register_param("mu",		NULL, "(double) Mean for non-equal-distributions.", true);
	ini.register_param("min",		NULL, "(double < max) Minimum value for flat distriubution.", true);
	ini.register_param("max",		NULL, "(double > min) Maximum value for flat distriubution.", true);
	ini.register_param("amplitude",	NULL, "(double > 0) Amplitude for the random number (depends on distribution.", true);

	ini.unset_active_section();
}

/** \fn void RandomNumber::WriteParams(myIniFiles &) const
 *	Writes the parameters of the cartesian space to the given ini-file.
 * \param ini: Reference to an ini-file, where to write the parameter in.
 */
void RandomNumber::WriteParams(myIniFiles& ini) const {
	ini.set_active_section("RandomNumber");

	ini.write("seed",		seed);
	ini.write("sigma",		sigma);
	ini.write("mu",			mu);
	ini.write("min",		range_min);
	ini.write("max",		range_max);
	ini.write("amplitude",	amplitude);

	ini.unset_active_section();
}

/** \fn void RandomNumber::Initialize(myIniFiles &)
 * \brief Initiazlizes the object, depending on the settings in the ini-file.
 *
 *	This method will (re-)initialize the object. Therefore are the settings used
 *	made in the given myIniFiles. If for a parameter no setting is found it
 *	remains unchanged.<br>
 *	Also the seed of the random number generator is set within this method.
 *
 * \param ini: Reference to the ini-file, where the parameters can be found.
 */
void RandomNumber::Initialize(myIniFiles& ini) {
	{
		//! Reading the setting from the ini-file and set the corresponding parameter.
		SetSeed(ini);

		ini.set_active_section("RandomNumber");

		SetSigma(ini.read("sigma", sigma));
		SetMu(ini.read("mu", mu));
		SetRangeMin(ini.read("min", range_min));
		SetRangeMax(ini.read("max", range_max));
		SetAmplitude(ini.read("amplitude", amplitude));

		ini.unset_active_section();
	}

	{
#ifdef HAVE_LIBGSL
		// If a random number generator was already created, free the memory.
		if(NULL != ptr_random_number_generator) {
			gsl_rng_free(ptr_random_number_generator);
			ptr_random_number_generator = NULL;
		}

		//! Create and start the random number generator.
		ptr_random_number_generator = gsl_rng_alloc(gsl_rng_mt19937);
		type_id = GSL_RNG_MT19937;
		gsl_rng_set(ptr_random_number_generator, this->GetSeed());
//#else
		//throw string("Need gsl");
#endif
	}
}

//unsigned long int gsl_rng_get (const gsl_rng * r)
//This function returns a random integer from the generator r. The minimum and
//maximum values depend on the algorithm used, but all integers in the range [min,max]
//are equally likely. The values of min and max can determined using the auxiliary functions gsl_rng_max (r) and gsl_rng_min (r).

//double gsl_rng_uniform (const gsl_rng * r)
//This function returns a double precision floating point number uniformly distributed in the range [0,1). The range includes 0.0 but excludes 1.0. The value is typically obtained by dividing the result of gsl_rng_get(r) by gsl_rng_max(r) + 1.0 in double precision. Some generators compute this ratio internally so that they can provide floating point numbers with more than 32 bits of randomness (the maximum number of bits that can be portably represented in a single unsigned long int).

//double gsl_rng_uniform_pos (const gsl_rng * r)
//This function returns a positive double precision floating point number uniformly distributed in the range (0,1), excluding both 0.0 and 1.0. The number is obtained by sampling the generator with the algorithm of gsl_rng_uniform until a non-zero value is obtained. You can use this function if you need to avoid a singularity at 0.0.

//unsigned long int gsl_rng_uniform_int (const gsl_rng * r, unsigned long int n)
//double gsl_ran_gaussian (const gsl_rng * r, double sigma)
//This function returns a Gaussian random variate, with mean zero and standard deviation sigma. The probability distribution for Gaussian random variates is,
//p(x) dx = {1 \over \sqrt{2 \pi \sigma^2}} \exp (-x^2 / 2\sigma^2) dx
//for x in the range -\infty to +\infty. Use the transformation z = \mu + x on the numbers returned by gsl_ran_gaussian to obtain a Gaussian distribution with mean \mu. This function uses the Box-Muller algorithm which requires two calls to the random number generator r.

//double gsl_ran_gaussian_pdf (double x, double sigma)
//This function computes the probability density p(x) at x for a Gaussian distribution with standard deviation sigma, using the formula given above.

//double gsl_ran_gaussian_ziggurat (const gsl_rng * r, double sigma)
//double gsl_ran_gaussian_ratio_method (const gsl_rng * r, double sigma)
//This function computes a Gaussian random variate using the alternative Marsaglia-Tsang ziggurat and Kinderman-Monahan-Leva ratio methods. The Ziggurat algorithm is the fastest available algorithm in most cases.

//double gsl_ran_ugaussian (const gsl_rng * r)
//double gsl_ran_ugaussian_pdf (double x)
//double gsl_ran_ugaussian_ratio_method (const gsl_rng * r)
//These functions compute results for the unit Gaussian distribution. They are equivalent to the functions above with a standard deviation of one, sigma = 1.

//double gsl_cdf_gaussian_P (double x, double sigma)
//double gsl_cdf_gaussian_Q (double x, double sigma)
//double gsl_cdf_gaussian_Pinv (double P, double sigma)
//double gsl_cdf_gaussian_Qinv (double Q, double sigma)
//These functions compute the cumulative distribution functions P(x), Q(x) and their inverses for the Gaussian distribution with standard deviation sigma.

//double gsl_cdf_ugaussian_P (double x)
//double gsl_cdf_ugaussian_Q (double x)
//double gsl_cdf_ugaussian_Pinv (double P)
//double gsl_cdf_ugaussian_Qinv (double Q)
//These functions compute the cumulative distribution functions P(x), Q(x) and their inverses for the unit Gaussian distribution.
#endif // RANDOMNUMBER_CPP
