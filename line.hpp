//      line.hpp
//
//      Copyright 2010 Stefan Fruhner <stefan.fruhner@gmail.com>
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


#ifndef LINE_HPP
#define LINE_HPP

#include <string.h>
#include "myexception.hpp"
#include "point.hpp"

namespace mylibs {
enum LineNormalDirection {Line_DIRECTION_UNDEFINED, Line_RHS, Line_LHS};

/*! \class Line
 * \brief Similar to myLine, but stores coordinates.
 *
 * A line is defined by \f$ \vec l = \vec{pa} + m (\vec{pb} - \vec{pa} ) \f$
 *
 * The type of the line is a element of the enum LineType.
 *
 * For rays the \f$p_a\f$ is the ray's starting point.
 *
 **/
class Line {
	public:
		/** \enum type
		  * \brief Enumeration for the type of a Line.
		  *
		  *	This enum contains values for indicating the type of a line.<br>
		  *	For the meaning of the values take a look at the documentaion of the
		  *	corresponding class.
		  **/
		enum type {line, segment, ray};
		enum NormalDirection {DIRECTION_UNDEFINED, RHS, LHS};

	public:
		Line(const Point &a, const Point &b, const int att = 0);
		Line(const Point &a, const Point &b, const type t, const int att = 0);

		void LineType(const type t);
		Line::type& LineType();

		void attribute(const int a);
		int& attribute();

		void swap();

		bool operator==(const Line &other) const;
		bool intersection(const Line &other, Point &intersection, const double small_value=1.E-5);
		double intersection_parameter_ma() const;
		double intersection_parameter_mb() const;
		double length() const;
		Point& p1(){return pa;}
		Point& p2(){return pb;}
		Point  point_at(const double m = 0.) const;
		Point  point_in_dist(const double distance) const;
		Point tangent() const;
		Point normal() const;
		Point normal_rhs() const;
		Point normal_lhs() const;
		Point binormal() const;
		void frenet_frame(Point &t, Point &n, Point &b) const;
		bool is_on_line(const Point &pt, double &m,const double small_value=1.E-5) const;
		bool perpendicular_foot_of(const Point& pt, Point& foot);
		void prnt(const char *name = "line") const;
		void info(const char *name = "line") const;

	private:
		Point pa, pb; 	//!< end points of the line
		double ma, mb; 	//!< parameters, needed for intersections
		type linetype;
		int a;	// define some attributes

	public: // Exceptions
		class Exception_InputError : public myexception {
			public:	Exception_InputError(std::string id) :
				myexception(id+"Error. pa and pb do not build a line."){}
		};
		class Exception_MathError : public myexception {
			public:	Exception_MathError(std::string id) :
				myexception(id+"Error. Mathematical error."){}
		};
};

/** \class Line_Segment
 * \brief Defines a segment, e.g. a straight curve with start and end.
 */
class Line_Segment : public Line {
	public:
		Line_Segment(const Point &a, const Point &b, const int att = 0)
				: Line(a,b,Line::segment, att) {};
};

/** \class Line_Ray
 * \brief Defines a ray, e.g. a straight curve with a beginning but no ending.
 */
class Line_Ray : public Line {
	public:
		Line_Ray(const Point &a, const Point &b, const int att = 0)
				: Line(a,b,Line::ray, att) {};
};

//////////////////////////////// EXCEPTIONS ////////////////////////////////////

class LineException_InputError : public myexception {
	public:
		LineException_InputError(std::string id) :
			myexception(id+"Error. pa and pb do not build a line."){}
} __attribute__((deprecated));

} // end of namespace mylibs
#endif /* LINE_HPP */
