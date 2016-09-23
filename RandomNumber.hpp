//		RandomNumber.hpp
//
//		Copyright 2011 Rico Buchholz <buchholz@klee>
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


#ifndef RANDOMNUMBER_HPP
#define RANDOMNUMBER_HPP

#ifdef HAVE_LIBGSL
	#include <gsl/gsl_rng.h>
	#include <gsl/gsl_randist.h>
#endif

#include "myinifiles.hpp"

/** \class RandomNumber
 *	\brief Binds functions for random number generation.
 *
 * This class binds functions from the gsl for random number generation. This
 * includes equal distributed integers, as well as probability distributions (for
 * example normal or poisson).
 *
 * \note Most of the methods for getting random numbers will only work as
 *		thought, if gsl was found during compilation. If this was not the case,
 *		the corresponding functions will throw a string.
 */
class RandomNumber {
	public:
		enum type {DEFAULT, GSL_RNG_MT19937};
		RandomNumber();
		RandomNumber(myIniFiles& ini);
		virtual ~RandomNumber();

		double			DistributionEqual() const;
		double			DistributionNormal() const;
		double			DistributionNormalCut(double const lower, double const upper) const;
		double			DistributionLaplace() const;
		double			DistributionFlat() const;
		unsigned int	DistributionPoisson() const;
		unsigned int	DistributionBernoulli() const;

		void			AddGaussianWhiteNoise(double * const values, size_t number, bool greater_zero = false) const;

		size_t			GetSeed()			const {return seed;};
		double			GetSigma()			const {return sigma;};
		double			GetMu()				const {return mu;};
		double			GetRangeMin()		const {return range_min;};
		double			GetRangeMax()		const {return range_max;};
		double			GetAmplitude()		const {return amplitude;};
		double			GetProbability()	const {return probability;};
		type			GetType()			const {return type_id;};

		void SetSeed(size_t const s);
		void SetSeed(myIniFiles& ini);
		void SetSigma(double const s);
		void SetMu(double const m);
		void SetRangeMin(double min);
		void SetRangeMax(double max);
		void SetAmplitude(double amp);
		void SetProbability(double prob);

		// *********************************************************
		// ----- Registration and initialisation of parameters -----
		static	void Register(myIniFiles& ini);
		virtual	void WriteParams(myIniFiles & ini) const;
	protected:
		void Initialize(myIniFiles& ini);

#ifdef HAVE_LIBGSL
		gsl_rng * ptr_random_number_generator;
#endif
		size_t seed;
		double sigma;
		double mu;
		double range_min;
		double range_max;
		double amplitude;//!< The used amplitude \f$ \sqrt{2D} \f$. The user defines \f$ D \f$
		double probability;

		type type_id;
	private:
};

#endif // RANDOMNUMBER_HPP
