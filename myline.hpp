//		./myline.hpp
//
//		Copyright 2011 Stefan Fruhner <stefan.fruhner@gmail.com>
//		Copyright 2011 Sonja Molnos <sonja@itp.tu-berlin.de>
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
#ifndef MYLINE_HPP
#define MYLINE_HPP

#include <cstdio>
#include <iostream>
#include <iomanip>
#include <string.h>
#include "myexception.hpp"

#ifdef HAVE_LIBGSL
	#include <gsl/gsl_errno.h>  	 // USE GSL interpolation
	#include <gsl/gsl_spline.h>
	#include <gsl/gsl_sf_ellint.h> 	 // elliptic integrals
	#include <gsl/gsl_roots.h> 	 // elliptic integrals
#elif USE_EINSPLINE
	#include <einspline/nubspline.h> // USE_EINSPLINE
#endif

#include <vector>
#include <deque>
#include <limits.h>
#include "line.hpp"
#include "lists.h"
#include "maps.h"
#include "cmdline.hpp"


#ifndef SIZE_T_MAX
#define SIZE_T_MAX ((size_t) -1)
#endif

using namespace std;
/**
 *		myline.hpp
 *
 *		\page mylibs
 *		\section sec_myline myLine
 *		Classes for working with lines (connection between two points) and
 *		curves (arc length sorted lists of points).
 *
 *		Classes:
 *		 - myLine is dealing only with indices as edge points, no coordinates
 *		 - Line is similar to myLine, but stores coordinates of the edge points,
 *		   comes with methods for computing the intersection,
 *		 - myCurve is a list of objects that can construct a curve
 *		 - PointCurve = myCurve<Point>
 *		   - methods for:
 *		    - sorting lines by arc length and creating `curves`
 *		    - interpolating curves
 *
 *
 */

/** \class myLine
 * \brief myLine connects two points, only the indices are saved. This makes
 * the class coordinate independent.
 **/
class myLine {
	public:
		int a,b; 		// indices of points
		int attribute;	// define some attributes

		myLine();
		myLine(const int a, const int b, const int att=0);
		void swap();
		void prnt() const;
		bool operator==(const myLine &other) const;
};

/** \class cmpLine
 * 	\brief Comparison class for Lines
 **/
class cmpLine {
	public:
		bool operator()(const myLine self,const myLine other) const;
};

/** \class myCurve
 * \brief A curve is a list of points (sorted by arc-length)
 * \attention Sorting by arc-length must be done by the user
 **/
template <class Type>
class myCurve: public myList<Type> {
	public:
		myCurve() : v_circular(false){};


		bool circular() const { //! if circular curve, then the firstpoint equals the last point
			return (this->front() == this->back());
		}

		bool operator==(const myCurve<Type> &other) const {
			// check size
			if (this->size() != other.size()) return false;

			//! check if all elements are equal
			//! \attention Curves can be equal, but their order can be reversed
			myCurve<Type> tmp1(*this);
			myCurve<Type> tmp2(other);

			if ( tmp1.circular() ) tmp1.pop(); // erase first element from circular curves
			if ( tmp2.circular() ) tmp2.pop(); // erase first element from circular curves

			//! \attention Sorting means we find two curves are equal if they cross the same points
			tmp1.sort();
			tmp2.sort();

			Type elem1 = tmp1[0];
			Type elem2 = tmp2[0];
			for (size_t i = 0; i < tmp1.size(); i++){
				if (elem1 != elem2) return false;
				elem1 = tmp1.next();
				elem2 = tmp2.next();
			}
			// here we now for sure both curves are equal
			return true;
		}

	private:
		bool v_circular;
};

typedef myCurve<int> Curve;		//!< Class for working with curves when points are referenced by index.

/** PointCurve
 * \brief 	Class for working with curves. This is just a list of Points,
 * 			which shall be sorted by arc length().
 * \attention 	Sorting using the member sort() destroys this order, because it
 * 				is sorting lexicographically.
 */
class PointCurve : public myCurve<Point> {
	private:
		double v_arc_length;
		vector<mylibs::Line> segments;

		void gen_segments();
	public:
		PointCurve();
		/** Convert a curve into a point curve */
		PointCurve(const Curve & curve, const vector<Point> &points);
		PointCurve(const Curve & curve, const deque<Point> &points);
		PointCurve(const char* filename);
		PointCurve(const mystring& filename);

		static size_t split_multiple_curves(vector<PointCurve>& curves, PointCurve &C, size_t split_attribute=0);
		static size_t split_multiple_curves(list<PointCurve>& curves, PointCurve &C, size_t split_attribute=0);
		static size_t load_multiple_curves(const char * const filename, vector<PointCurve>& curves);

		Point min() const;
		Point max() const;

		/** center_of_mass:
		  * Computes the center of mass.
		  */
		Point center_of_mass() const;
		double compute_arc_length(size_t end = SIZE_T_MAX);

		double arc_length();
		double arc_length(PointCurve::const_iterator it);
		double arc_length(const Point &pt);
		double arc_length_correction(const double value);
		double arc_distance(const Point &p1,const Point &p2);
		double arc_ascending_distance(const Point &p1, const Point &p2);

		double curvature(const PointCurve::const_iterator it) const;
		double mean_curvature() const;

		double* curvature(int windowsize = 1);
		double mean_curvature(int windowsize = 1);

		Point normal(PointCurve::const_iterator it) const;
		Point tangent(PointCurve::const_iterator it) const;
		Point speed_normal(PointCurve::const_iterator it, PointCurve &other, const double dt) const;
		Point speed_normal(PointCurve::const_iterator it, vector<PointCurve> &other, const double dt) const;
		Point speed_normal(PointCurve::const_iterator it, list<PointCurve> &other, const double dt) const;
		Point speed_x(PointCurve::const_iterator it, list <PointCurve> &other, const double dt) const;

#ifdef HAVE_LIBGSL
		PointCurve compute_spline(double fraction=0.3, double resolution=0.25, const unsigned short deriv = 0, bool verbose=false);
#elif USE_EINSPLINE
		PointCurve compute_spline(double fraction=0.3, double resolution=0.25, bool save_files=false);
#endif
		bool save(const char* const fn, const size_t dim=2);
		bool save_node(const char* const fn, const size_t dim=2);
		bool save_vpts(const char* const fn);
		void read_node(const char* const fn);

		bool save_to_file(string &fn);

		bool inside(const Point &point, const Point &point_outside);
		bool inside(const Point &point);
		mylibs::LineNormalDirection where_is_inside();
		list<Point> intersections(const mylibs::Line &line);
		list<Point> intersections(PointCurve &other);
		bool isflatxy() const;
		bool is_on_curve(const Point &pt, double *arc = NULL);
		Point point_at(double arc);
		Point find_point_outside();

		bool perpendicular_foot_of(const Point& pt, Point &foot);
		size_t nr_segments(){
			if (segments.size() == 0) gen_segments();
			return segments.size();
		}
		mylibs::Line& segment(size_t i){return segments[i];}

		// manipulation of PointCurves
		void translate(const Point & t);
		void attribute(const int value, const size_t start=0, size_t end = 0);

		// generation of PointCurves
#ifdef HAVE_LIBGSL
		static PointCurve Ellipse(const double a, const double b, const size_t n, const double fraction=1.);
		static PointCurve Ellipse(const double a, const double b, const double segment_length, const double fraction=1.);
		static PointCurve Circle(const double r, const double segment_length);
		static PointCurve Circle(const double r, const size_t nr_segments);
#endif

	public: //////////////////////////////// EXCEPTIONS ////////////////////////////////////

		class Exception_IOError : public myexception {
			public:
				Exception_IOError(std::string id) :
					myexception(id+"I/O error. Filename mismatch ?"){}
		};

		class Exception_IndexError : public  myexception {
			public:
				Exception_IndexError(std::string id) :
					myexception(id+"IndexError. Index out of range ?"){}
		};

		class Exception_PointNotFound : public  myexception {
			public:
				Exception_PointNotFound(std::string id) :
					myexception(id+"Point was not found on the curve."){}
		};

		class Exception_ZeroDistance :public  myexception {
			public:
				Exception_ZeroDistance(std::string id) :
					myexception(id+"Distance between neighbouring points is zero ? What the hell is going on here ?"){}
		};

		class Exception_CurveTooShort :public  myexception {
			public:
				Exception_CurveTooShort(std::string id) :
					myexception(id+"The curve is too short for continuing the operation."){}
		};

		class Exception_RangeViolation :public  myexception {
			public:
				Exception_RangeViolation(std::string id) :
					myexception(id+"Variable value out of range."){}
		};

		class Exception_NotImplemented :public  myexception {
			public:
				Exception_NotImplemented(std::string id) :
					myexception(id+"This feature is not implemented."){}
		};

		class Exception_WhatTheHell :public  myexception {
			public:
				Exception_WhatTheHell(std::string id) :
					myexception(id+"What the hell is happening here ? This error is not meant to occur."){}
		};

};


/** \class LineMap
  * \brief 	LineMap is a map of Line map<Line>. A Line saves to int values which
  * 		belong to an index in an array of points. This class is used, to
  * 		sort lines by arc length.
  */
class LineMap : public map<myLine,int, cmpLine> {
	private:
		int find_line_b(const myLine& line ) const;
	public:
		myList<Curve> path_finder();
		myList<PointCurve> path_finder(const vector<Point> &p);
		myList<PointCurve> path_finder(const deque<Point> &p);
};


#endif //__MYLINE_HPP
