//      xydata.hpp
//
//      Copyright 2012 Stefan Fruhner <stefan.fruhner@gmail.com>
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


#ifndef XYDATA_HPP
#define XYDATA_HPP

#include <iostream>
#include <set>
#include <vector>
#include <utility>
#include "myexception.hpp"
#include "algorithm.hpp"
#include "cmdline.hpp"
#include <float.h>
#include <math.h>

#ifdef HAVE_LIBGSL
	#include <gsl/gsl_fit.h>		 // lin. least squares fit
	#include <gsl/gsl_errno.h>  	 // USE GSL interpolation
	#include <gsl/gsl_spline.h>
#endif



/**
 * \class XYData
 *
 * This class deals with xy-datasets and can be used for function analysis for
 * a discrete function f(x).
 *
 *
 */
class XYData
{
	public:

		/** \class LinLSQ
		 *	LinLSQ handles linear least square fits.
		 */
		class LinLSQ{
			public:
				LinLSQ() : c0(0.), c1(0.), cov00(0.), cov01(0.), cov11(0.), sumsq(0.) {};

				/** \fn double rsq() const
				 * \brief squared correlation coefficient
				 * \return Returns the squared correlation coefficient.
				 */
				double rsq() const { return (cov01*cov01) / (cov00 * cov11);};

				pair<double,double> y(double x) {
					double value  = 0.;
					double stddev = 0.;
					#ifdef HAVE_LIBGSL
						gsl_fit_linear_est (x, c0, c1, cov00, cov01, cov11, &value, &stddev);
					#else
						throw myexception("Need GSL");
					#endif
					return pair<double, double>(value, stddev);
				}

				void info(){
					cout << "Linear least squares fit " << endl;
					cout << "  fit(x) = "
						 << c0 <<" + " << c1 <<" * x"   << endl;
					cout << "     r^2 = " << rsq()      << endl;
				}
				double c0, c1;
				double cov00, cov01, cov11;
				double sumsq;

		};

#ifdef HAVE_LIBGSL
		/** \class LinLSQ
		 *	LinLSQ handles linear least square fits.
		 */
		class CSpline{
			public:
				CSpline(double *xdata, double *ydata, size_t size) :
							grid(xdata), data(ydata), len(size),
							spline(0), t(gsl_interp_cspline),
							accel(0) {
					accel  = gsl_interp_accel_alloc();

					// for periodic splines
//					t = gsl_interp_cspline_periodic
				};

				~CSpline(){
					gsl_spline_free (spline);
					gsl_interp_accel_free (accel);
				}

				void compute(){
					if (! spline) spline = gsl_spline_alloc (t, len);
					gsl_spline_init(spline, grid, data, len);
				}

				double y(double x) {
					#ifdef HAVE_LIBGSL
						return gsl_spline_eval(spline, x, accel);
					#else
						throw myexception("Need GSL");
					#endif
					return 0.;
				}

				void info(){
					cout << "CSpline " << endl;
					cout << "  size = " << len << endl;
				}
			private:
				double 			    *grid;
				double 			    *data;
				size_t 			      len;
				gsl_spline 		  *spline;
				const gsl_interp_type  *t;
				gsl_interp_accel   *accel;
		};
#else
	class CSpline {
		public:
			CSpline(){throw myexception("Need gsl installed!");}
	};
#endif // HAVE_LIBGSL

		typedef unsigned char Flags;

		//Flags
		enum FlagOptions {none = 0x01, rising = 0x02, falling = 0x04};

		class Range {
			public:
				Range() : min(0.), max(0.){}
				Range(double v1,double v2) : min(v1), max(v2){
				}

				double size() const {
					return (max-min);
				}

				void info() const{
					cout << "Range = [" << min <<", "<< max << "]" << endl;
				}

				double min,max;
		};

		XYData(const double *x,const double *y, size_t sz);
		XYData(const char* filename);
		~XYData();

		void clear();
		double mean() const;
		double stddev() const;
		double MeanSquaredError() const;

		#ifdef HAVE_LIBGSL
		XYData::LinLSQ  LinearLeastSquaresFit();
		XYData::CSpline compute_spline();
		#else
		XYData::LinLSQ  LinearLeastSquaresFit() {throw myexception("Need gsl installed!");}
		XYData::CSpline compute_spline() {throw myexception("Need gsl installed!");}
		#endif // gsl present


		// f(x)
		double y(double x);
		double f(double x) { return y(x);}

		double& operator[](size_t x);
		double& y(size_t x_index){return operator[](x_index);}

		// x(y)
		std::set<double> x(double y, XYData::Flags flag = XYData::none);
		double& x(size_t x_index);

		list<Range> find_step(Range yrange, XYData::Flags flag = XYData::none);
		list<Range> find_peak(const double &threshold, XYData::Flags flag = XYData::none);
		list<size_t> Coords2Index(const list<XYData::Range> &l);

		// bounds
		double max() const;
		double min() const;

		double max(const Range &xrange);
		double min(const Range &xrange);

		size_t min_element() const;
		size_t min_element(const XYData::Range &xrange);

		size_t max_element() const;
		size_t max_element(const XYData::Range &xrange);

		size_t extremum_element() const;
		size_t extremum_element(const XYData::Range &xrange);

		double xmin() const {if (xdata ) return xdata[0]; else return 0;}
		double xmax() const {if (xdata ) return xdata[len-1]; else return 0;}

		double slope(double x) const;
		XYData::Flags slope_type(double x) const;

		int detect_peaks(	vector<size_t> &max_peaks,
							vector<size_t> &min_peaks,
							double delta = 0., /* delta used for distinguishing peaks */
							bool max_first=true /* should we search emission peak first of absorption peak first? */
						);

		void moving_average(size_t window = 1) const;
		pair<size_t,double *> average_triggered(vector<size_t> triggers,
												const size_t offset_negative=0,
												const size_t offset_positive=0);

		const Range xrange() const;
		const Range yrange() const;

		void info(const char* label=NULL) const;

	private:
		void set_search_range(double lower=DBL_MIN, double upper=DBL_MAX);
		void set_search_range(const Range &range);

	private:
		size_t xindex(double xvalue) const;
		bool   in_range(double x);

		/* add your private declarations */
		double *xdata;
		double *ydata;
		size_t len;
		bool   local_data;
		Range search_range;

	public:

		// Exceptions
		class Exception_Range : public myexception {
			public:	Exception_Range(std::string id) :
					myexception(id+" Range error."){}
			};
		class Exception_IO : public myexception {
			public:	Exception_IO(std::string id) :
					myexception(id+" I/O error."){}
			};
};

#endif /* XYDATA_HPP */
