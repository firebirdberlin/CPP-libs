/*
 *      point.hpp
 *
 *      Copyright 2009,2012 Stefan Fruhner <stefan.fruhner@gmail.com>
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


#ifndef _POINT_HPP
#define _POINT_HPP

#include <cstdio>
#include <iomanip>
#include <cmath>
#include <vector>
#include <iostream>
#include <limits>
#include "myexception.hpp"
#include "cmdline.hpp"


/*! \attention SMALL_VALUE is needed for comparisons of different points.
 * You can alter this value by defining SMALL_VALUE before including point.hpp
 */
#ifndef SMALL_VALUE
#define SMALL_VALUE 1.E-6
#endif

using namespace std;

class Point;
class PointSpherical;
class PointCylindrical;
class PointPolar;

//! Class to work with 3d vectors and points (+ time)
class Point {
	public:
		double 		   x,y,z; 	// coordinates
		double			   t;	// time
	private:
		size_t nr_attributes;	// nr of attributes
		vector<int>        a;	// some attribute, e.g. region
		int  	 		 bnd;	// boundary markers
	public:
		Point();
		Point(double x, double y, double z=0., double t = 0., int attribute = 0);
		Point(const vector<double> &pt);
		Point(const Point &copy);
		Point(const PointSpherical   &p, int attribute = 0);
		Point(const PointCylindrical &p, int attribute = 0);
		Point(const PointPolar &p, int attribute = 0);
		~Point();

		void set(double x, double y, double z=0., double t = 0., int attribute = 0);
		void set(const vector<double> &pt);
		void setInfinity();
		void setInfinityMin();

		double abs() const;
		double norm() const;
		void normalize();
		Point cross(const Point& other) const;
		double dot(const Point& other) const;


		void prnt(bool attributes=false, bool boundaries=false) const;
		void info(const char *name=NULL) const {
			if (name) cout << name << " = ";
			this->prnt(true, true);
		}

		bool operator==(const Point &other) const {
				//return ((z==other.z) && (y==other.y) && (x==other.x));
			return ((z-SMALL_VALUE < other.z) && (z+SMALL_VALUE > other.z) &&
					(y-SMALL_VALUE < other.y) && (y+SMALL_VALUE > other.y) &&
					(x-SMALL_VALUE < other.x) && (x+SMALL_VALUE > other.x));
		}

		bool operator!=(const Point &other) const {
			return ( !((z-SMALL_VALUE < other.z) && (z+SMALL_VALUE > other.z)) ||
					 !((y-SMALL_VALUE < other.y) && (y+SMALL_VALUE > other.y)) ||
					 !((x-SMALL_VALUE < other.x) && (x+SMALL_VALUE > other.x)));
				//return ( (z!=other.z) || (y!=other.y) || (x!=other.x));
		}

		Point & operator=(const Point & other);
		Point operator-(const Point &other) const;
		Point operator+(const Point& other) const;
		Point operator*(const double val) const;
		double operator*(const Point &other) {return this->dot(other);} // v1 * v2 // dot-product
		Point operator/(const double val) const;
		void operator+=(const Point& other);
		void operator-=(const Point& other);
		void operator*=(const double val);
		void operator/=(const double val);
		bool operator<(const Point& other) const;
		bool operator>(const Point& other) const;
		bool operator>=(const Point& other) const;
		bool operator<=(const Point& other) const;
		double & operator[](int item);
		Point operator-()	const;
		Point operator+()	const;


		int attribute() const;
		int attribute_at(size_t idx = 0) const;
		void attribute(const int value);
		size_t attributes() const;
		void add_attribute(const int attribute);
		void clear_attributes();

		int  boundary(){return this->bnd;};
		void boundary(int value){this->bnd = value;};

		void spherical_coordinates(double &r, double &phi, double &theta) const;
		void spherical_coordinates_degrees(double &r, double &phi, double &theta) const;

		// Rotation matrizes for coordinate axis
		void rotate_x(double alpha);
		void rotate_y(double alpha);
		void rotate_z(double alpha);
		void rotate(double phi, double theta);

		template < class ForwardIterator >
		ForwardIterator closest_point(ForwardIterator first, ForwardIterator last );

		class Exception_Input :public myexception {
		public:	Exception_Input(std::string id) :	myexception(id+"Input error. Cannot proceed !"){}
		};

		class Exception_NaN :public myexception {
		public:	Exception_NaN(std::string id) :	myexception(id+"NaN is not a cool number !"){}
		};

		class Exception_ZeroLength :public myexception {
		public:	Exception_ZeroLength(std::string id) :	myexception(id+"Cannot continue due to zero length vector."){}
		};

		class Exception_IndexError :public myexception {
		public:	Exception_IndexError(std::string id) :	myexception(id+"Index out of range."){}
		};
};

class PointSpherical {
	public:
		double r,phi,theta;	// coordinates
		double t;			// time

		PointSpherical(const double v_r, const double v_phi, const double v_theta, const double v_t = 0.);
		PointSpherical(const Point &p);
		PointSpherical();
		inline double abs(){return r;}
};

class PointCylindrical {
	public:
		double rho,phi,z;	// coordinates
		double t;			// time

		PointCylindrical(const double v_r, const double v_phi, const double v_z, const double v_t = 0.);
		PointCylindrical(const Point &p);
		PointCylindrical();
};

class PointPolar {
	public:
		double r,phi;	// coordinates
		double t;		// time

		PointPolar(const double v_r, const double v_phi, const double v_t = 0.);
		PointPolar(const Point &p);
		PointPolar();
};


//! Lexiographical comparison of `Point`
struct PointCompare {

/**
 * \attention
 * This comparison class is needed for red black trees.
 * See STL maps : http://www.cplusplus.com/reference/stl/map/map.html
 */

	/**
	 * PointCompare::operator()
	 * The comparison is implemented as functor. So it can be called
	 * as PointCompare(p1, p2). The result of this functor must
	 * be true is p1 < p2 and false otherwise.
	 * \param self: a point to be compared.
	 * \param other: a point to be compared with
	 * \return True if self is `smaller` or false otherwise
	 **/
	bool operator() (const Point& self, const Point& other) {

		if (self.x < other.x) return true;
		if ((self.x == other.x) && (self.y < other.y)) return true;
		if ((self.x == other.x) && (self.y == other.y) && (self.z < other.z)) return true;
		return false;
	}

};

struct PointCompare_zyx {//!< same as above, but comparison results in an order, where x varies fastest
	/**
	 * PointCompare::operator()
	 * The comparison is implemented as functor. So it can be called
	 * as PointCompare(p1, p2). The result of this functor must
	 * be true if p1 < p2 and false otherwise.
	 * \param self: a point to be compared.
	 * \param other: a point to be compared with
	 * \return True if self is `smaller` or false otherwise
	 **/
	bool operator() (const Point& self, const Point& other) {
		if (self.z < other.z) return true;
		if ((self.z == other.z) && (self.y < other.y)) return true;
		if ((self.z == other.z) && (self.y == other.y) && (self.x < other.x)) return true;
		return false;
	}

};

// usuful functions

Point operator*(const double val,const Point &p);
ostream &operator<<( ostream &out, const Point &p);
double distance(const Point &p1, const Point &p2);

template < class ForwardIterator >
ForwardIterator Point::closest_point(ForwardIterator first, ForwardIterator last ){
	ForwardIterator closest = first;
	double dist = (*this - *first).abs(); // distance between intersection and the interpolated point
	double d2   = dist;
	for ( ; first != last; ++first){
		if ( ( d2 = (*this - *first).abs() ) < dist ) {
			dist 	= d2;
			closest = first;
		}
	}
	return closest;
}

//////////////////////////// Exceptions ////////////////////////////////////////
//! \todo wait an appropriate amount of time and delete these exceptions
class PointException_NaN :public  myexception {
	public:
		PointException_NaN(std::string id) :
			myexception(id+"NaN is not a cool number !"){}
}  __attribute__((deprecated));

class PointException_ZeroLength :public  myexception {
	public:
		PointException_ZeroLength(std::string id) :
			myexception(id+"Cannot continue due to zero length vector."){}
}  __attribute__((deprecated));

class PointException_IndexError :public  myexception {
	public:
		PointException_IndexError(std::string id) :
			myexception(id+"Index out of range."){}
} __attribute__((deprecated));



#endif /* POINT_HPP */
