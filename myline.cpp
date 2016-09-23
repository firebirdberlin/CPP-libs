//		./myline.cpp
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
#include "myline.hpp"

using namespace std;

#define mini(a,b) ( (a < b) ? ( a ) : ( b ) )
#define maxi(a,b) ( (a > b) ? ( a ) : ( b ) )

#ifndef print
#define print(x) (cout << __FILE__ << ":" << __FUNCTION__ <<"() (" <<__LINE__ << ") : " << x << endl)
#endif

/** name: myLine::myLine
 *	Standardconstructor, sets default value zero for the members.
 */
myLine::myLine():a(0), b(0), attribute(0) { }

myLine::myLine(const int a, const int b, const int att) {
			this->a = a;
			this->b = b;
			this->attribute = att;
		}

void myLine::swap(){ //swap items
			int tmp = this->b; // swap indices
			this->b = this->a;
			this->a = tmp;
		}

/** \fn void myLine::prnt() const
 * \brief Writes [a, b]\ to the standard-output.
 */
void myLine::prnt() const {
	cout << "[" << a << "," << b << "]" << endl;
}

/** \fn bool myLine::operator==(const myLine &) const
 * \brief Tests for equality.
 *
 *	This member tests if another myLine is equal to this one. Equality thereby
 *	means that all the members are equal.
 *
 * \param other: The myLine that should be compared with this one.
 * \return True if the members are equal, false otherwise.
 */
bool myLine::operator==(const myLine &other) const {
	return ((other.a == a) and (other.b == b));
}

/** \fn bool cmpLine::operator()(const myLine, const myLine) const
 * \return 	True if the minimum of a and b of the first line is lower than the
 *			minimum of a and b of the other line or if these minima are equal
 *			and the corresponding maximum of the first point is lower than
 *			the maximum of the other point. Otherwise false.<br>
 *			To use an picture: True is returned if the first line starts
 *			earlier or if the lines start from the same point but the first
 *			one is shorter.
 */
bool cmpLine::operator()(const myLine self, const myLine other) const {
	int min1 = mini(self.a, self.b);
	int min2 = mini(other.a, other.b);

	if (min1 < min2) return true;
	else
	if (min1 == min2){
		int max1 = maxi(self.a, self.b);
		int max2 = maxi(other.a, other.b);

		if (max1 < max2) return true;
	}

	// works for sorted lines with  a < b always
	//if (self.a < other.a) return true;
	//if ((self.a == other.a) && (self.b < other.b)) return true;

	return false;
}


PointCurve::PointCurve() : v_arc_length(0), segments(vector<mylibs::Line>()) {} //!< constructor

/** PointCurve::PointCurve()
 * \brief Convert a curve into a point curve.
 *
 *	This constructor gives the possibility, to create a PointCurve from
 *	a vector of Points and a Curve. The latter one
 *
 * \param curve:
 * \param points: Reference to a vector of Points, which should be used to
 *				create this PointCurve.
 *
 */
PointCurve::PointCurve(const Curve & curve, const vector<Point> &points) :
	v_arc_length(0), segments(vector<mylibs::Line>()) {
	for (Curve::const_iterator it = curve.begin(); it != curve.end(); ++it){
		append(points[(*it)]);
	}
	return;
}

/** PointCurve::PointCurve()
 * \brief Convert a Curve into a PointCurve.
 * \param curve: A Curve, which is used for indexing the Point's
 *				of the deque.
 * \param points: Reference to a deque of Point's which should be added to this
 *				PointCurve. The order is thereby dedicted by the Curve.
 */
PointCurve::PointCurve(const Curve & curve, const deque<Point> &points) :
	v_arc_length(0), segments(vector<mylibs::Line>()) {
	for (Curve::const_iterator it = curve.begin(); it != curve.end(); ++it){
		append(points[(*it)]);
	}
	return;
}

/** \fn PointCurve::PointCurve(const char * const)
 * \brief Constructor for reading curves from a file.
 *
 *	This constructor will read the points from a file. Therefore the
 * 	filename is passed to the method read_node().
 *
 * \param filename: A c-string, determining the name of the file from
 * 					which to load the points.
 */
PointCurve::PointCurve(const char * const filename) :
	v_arc_length(0), segments(vector<mylibs::Line>()){
	this->read_node(filename);
	return;
}

/** \fn PointCurve::PointCurve(const mystring&)
 * \brief Constructor for reading curves from a file.
 *
 *	This constructor will read the points from a file.
 *  Therefore the filename is passed to the method read_node().
 *
 * \param filename: A mystring, determining the name of the file from
 * 					which to load the points.
 */
PointCurve::PointCurve(const mystring& filename) :
	v_arc_length(0), segments(vector<mylibs::Line>()){
	this->read_node(filename.c_str());
	return;
}


/** \fn size_t PointCurve::split_multiple_curves(vector<PointCurve> &, PointCurve &, size_t)
 * \brief Split a PointCurve into seperate curves.
 *
 *	This method will split a single PointCurve-object into multiple curves.
 *	Different curves are thereby determined via different attributes.
 *	\note The user has to take care that the curves within the PointCurve-
 *		object are not seperated by other points/curves.
 *
 * \param curves: 	Reference to a vector of PointCurves. The curves are stored
 * 					within this vector.
 *					\note Previous PoinCurves will be deleted.
 * \param C: 		Reference to the PointCurve that should be split.
 * \param split_attribute : The attribute-ID which is used to split the curves (default 0).
 * \return The number of different curves (attributes) that were found in the
 *				given PointCurve.
 */
size_t PointCurve::split_multiple_curves(vector<PointCurve>& curves, PointCurve &C, size_t split_attribute) {
	curves.clear();
	if (C.size() == 0) return 0;
	int att = C[0].attribute_at(split_attribute);
	Point p = C[0];
	PointCurve curve;
	for (size_t i = 0; i < C.size(); i++){
		if (p.attribute_at(split_attribute) != att){
			curves.push_back(curve);
			curve.clear();
			att = p.attribute_at(split_attribute);
		}
		curve.push_back(p);
		p = C.next();
	}
	curves.push_back(curve);
	return curves.size();
}

/** \fn size_t PointCurve::split_multiple_curves(list<PointCurve> &, PointCurve &, size_t)
 * \brief Split a PointCurve into seperate curves.
 *
 *	This method will split a single PointCurve-object into multiple curves.
 *	Different curves are thereby determined via different attributes.
 *	\note The user has to take care that the curves within the PointCurve-
 *		object are not seperated by other points/curves.
 *
 * \param curves: Reference to a list of PointCurves. The curves found are
 * 				  stored within this list.
 *				\note Previous PoinCurves will be deleted.
 * \param C: Reference to the PointCurve that should be split.
 * \param split_attribute : The attribute-ID which is used to split the curves (default 0).
 * \return  The number of different curves (attributes) that were found in the
 *			given PointCurve.
 */

size_t PointCurve::split_multiple_curves(list<PointCurve>& curves, PointCurve &C, size_t split_attribute) {
	curves.clear();

	if (C[0].attributes() > 1 and split_attribute == 0)
	if(split_attribute==0)mylibs::cmdline::warning(EXCEPTION_ID + "Splitting by the first attribute. The curve read in has more than one attribute !!");

	if (C.size() == 0) return 0;
	int att = C[0].attribute_at(split_attribute);
	Point p = C[0];
	PointCurve curve;
	for (size_t i = 0; i < C.size(); i++){
		if (p.attribute_at(split_attribute) != att){
			curves.push_back(curve);
			curve.clear();
			att = p.attribute_at(split_attribute);
		}
		curve.push_back(p);
		p = C.next();
	}
	curves.push_back(curve);
	return curves.size();
}


/** name: PointCurve::load_multiple_curves()
 *	Load *.node file that contains multiple curves. These have to be
 *	distinguished by different attributes (e.g. numbers from 0 to n). If the file
 *	contains only one curve, the function will work.
 * @param filename: name of the node-file.
 * @param curves  : Vector of curves that will be filled.
 * @return number of curves read.
 **/
size_t PointCurve::load_multiple_curves(const char * const filename, vector<PointCurve>& curves){
	PointCurve C(filename);

	if (C.size() == 0) throw PointCurve::Exception_IOError(EXCEPTION_ID+"Loaded no points - file empty ?");

	return split_multiple_curves(curves, C);
}

/** \fn Point PointCurve::min() const
  * Find the minimum coordinate which is used by the curve.
  * \attention 	x,y,z are treated independently, so the point found must not
  * 			lie on the curve!
  *
  * \return A point which coordinates are the minmal values found for each
  *				direction (independently).
  */
Point PointCurve::min() const {
	myCurve<Point>::const_iterator i = begin();
	Point min;

	min.x = i->x;
	min.y = i->y;
	min.z = i->z;

	for (i = begin(); i != end(); i++){
		min.x = ((i->x < min.x) ?  i->x : min.x);
		min.y = ((i->y < min.y) ?  i->y : min.y);
		min.z = ((i->z < min.z) ?  i->z : min.z);
	}
	return min;
}

/** \fn Point PointCurve::max() const
  * Find the maximum coordinate which is used by the curve.
  * \attention 	x,y,z are treated independently, so the point found must not
  * 			lie on the curve!
  *
  * \return A point which coordinates are the maximal values found for each
  *				direction (independently).
  */
Point PointCurve::max() const {
	myCurve<Point>::const_iterator i = begin();
	Point max;

	max.x = i->x;
	max.y = i->y;
	max.z = i->z;

	for (i = begin(); i != end(); i++){
		max.x = ((i->x > max.x) ?  i->x : max.x);
		max.y = ((i->y > max.y) ?  i->y : max.y);
		max.z = ((i->z > max.z) ?  i->z : max.z);
	}
	return max;
}

/** \fn Point PointCurve::center_of_mass() const
  * \brief Computes the center of mass.
  * \return The computed center of mass as Point.
  */
Point PointCurve::center_of_mass() const {
	myCurve<Point>::const_iterator i = begin();
	Point com;

	for (i = begin(); i != end(); i++) com += *i;
	com /= length();
	return com;
}

/** name: PointCurve::compute_arc_length
 * Computes the arc-length of a point on the PointCurve. The point of interest
 * is defined by the parameter 'end'. If no parameter is supplied then the
 * length of the complete curve is computed.
 * @param end: index of the point to compute the arc-length for
 * @return arc length
 **/
double PointCurve::compute_arc_length(const size_t end) {
	double arc = 0.;
	PointCurve::iterator it_end = this->end();
	if (end > size()-1 || end == SIZE_T_MAX) it_end--; // the last item is not important 'cause there starts no segment
	else{
		advance(it_end, -size() + end); //set the iterator to the right element
	}

	PointCurve::const_iterator it 	 = begin();
	PointCurve::const_iterator it_next = begin();
	for (it = begin(); it != it_end; it++){
		it_next++;
		arc += (*it_next - *it).abs();
	}

	if (end > size()-1 || end == SIZE_T_MAX) v_arc_length = arc;
	return arc;
}

double PointCurve::arc_length(){
	if (v_arc_length == 0.) compute_arc_length();
	return v_arc_length;
}

double PointCurve::arc_length(const Point &pt) {
	double arc = 0.;
	if (is_on_curve(pt, &arc)) return arc;

	throw PointCurve::Exception_PointNotFound(EXCEPTION_ID);
}

/** PointCurve::arc_length_correction()
 * For circular curves the values given for a specific arc length can be
 * greater than the arc length. This function return the correct value in
 * the interval [0:arc_length]. Negative values are meant to measure the
 * arc_length from the end of the curve.
 *
 */
double PointCurve::arc_length_correction(const double value) {
	if (not circular())
		throw myexception(EXCEPTION_ID+"No correction for not circular curves.");
	if (value < 0.){

		throw myexception(EXCEPTION_ID+"Not implemented value = " + toString(value));
//		while (value < 0.) value += arc_length();
//		return value;
	}
	else{
		return fmod(value, arc_length());
	}
}

/** PointCurve::arc_distance()
 * Computes the distance of two given points along the curve. Please note that
 * it is different from the distance of the point along a line connecting these
 * points.
 * If one of both points is not on the line an exception is thrown.
 * \attention For circular curves the shortest distance is returned, whereas
 * 			  the longer distance can be computed as 'arc_length() - distance'.
 * \param p1 : the first point
 * \param p2 : the second point
 * \return The distance as double value.
 */
double PointCurve::arc_distance(const Point &p1,const Point &p2) {
	double a1=0., a2=0.;
	if (not is_on_curve(p1, &a1)) throw PointCurve::Exception_PointNotFound(EXCEPTION_ID
									+"Point #1 does not belong to the curve.");
	if (not is_on_curve(p2, &a2)) throw PointCurve::Exception_PointNotFound(EXCEPTION_ID
									+"Point #2 does not belong to the curve.");
	double dist = (a1 >= a2) ? a1 - a2 : a2 -a1;

	//! \note For circular curves return the shortest distance
	if (circular() && dist > .5* arc_length()) return arc_length() - dist;

	return dist;
}

/** PointCurve::arc_ascending_distance()
 * Computes the distance of two given points along the curve. In contrast to
 * arc_distance() the value is computed assuming that the second point has a
 * position at a larger arc length. So for circular curves the distance is
 * computed correctly in one direction only.
 * Example:
\verbatim
p0	p1
 >----x---
 |		|
 -x-------
  p2
\endverbatim
 * The curve is circular and starts at p0. The are two possible distances
 * between the points p1 and p2: p1 to p2 and p2 to p1.
 *
 * Giving p1 as first argument to the function would result in p1 to p2, whereas
 * the result will be the distance p2 to p1, if p2 is given as first parameter.
 *
 * If one of both points is not on the line an exception is thrown.
 * \param p1 : the first point
 * \param p2 : the second point
 * \return The distance as double value.
 */
double PointCurve::arc_ascending_distance(const Point &p1, const  Point &p2) {
	double a1=0., a2=0.;
	if (not is_on_curve(p1, &a1)){
		p1.info();
		throw PointCurve::Exception_PointNotFound(EXCEPTION_ID+"Point #1 does not belong to the curve.");
	}
	if (not is_on_curve(p2, &a2)){
		p2.info();
		throw PointCurve::Exception_PointNotFound(EXCEPTION_ID+"Point #2 does not belong to the curve.");
	}
//	printf(">> %6.3f %6.3f\n", a1, a2);
	if (a2 >= a1) return a2 - a1; // if p2 is at a position with higher arc length then return the difference

	/*! \note Only for circular curves there is a result still possible. We
	 * compute the distance between the second point and the end of the curve
	 * and add the arc lenght of the first point
	 */
	if (circular())	return arc_length() - a1 + a2;

	throw myexception(EXCEPTION_ID +"No valid distance found, maybe arc(p1) > arc(p2) ?");
}

bool PointCurve::save(const char* const fn, const size_t dim){
	mystring fname(fn);

	if (fname.file_ext() == "node") return save_node(fn,(size_t) dim);
	else
	if (fname.file_ext() == "vpts") return save_vpts(fn);

	return false;
}

/** name: PointCurve::save_node()
 * Saves all nodes of the curve to a node file (cf. tetgen, triangle).
 * The attribute of the points is always written.
 * @param fn: filename of the file to be written
 * @param dim: dimension 1-4, where 4 is the time variable
 * @return
 **/
bool PointCurve::save_node(const char* const fn, const size_t dim) {
	if (size() == 0) return false;

	print("  - Writing *.node file : " << fn);
	size_t nr_attributes = begin()->attributes();

	// check #attributes is uniform
	for (PointCurve::iterator it = begin(); it != end(); it++){
		if (nr_attributes != it->attributes())
			throw PointCurve::Exception_WhatTheHell(EXCEPTION_ID
					+" #Attributes of the curve is not uniform !");
	}

	ofstream f(fn);

	//!  First line: [# of vertices] [dimension]  [# of attributes] [# of boundary markers (0 or 1)]
	//!  Remaining lines: [vertex #] [x] [y] [attributes] [boundary marker]
	f << size() << " "
	  << dim << " "
	  << nr_attributes << " 0" << endl; //! \note bnd flags are currently omitted !

	size_t cnt = 0;
	for (PointCurve::iterator it = begin(); it != end(); it++){
		// determine order of manitude for alinment
		f << setw(log10((float)size())+1) << cnt++ << " ";
		switch (dim){
			case 1:
				f << scientific << setprecision(15) << setw(25)
				  << it->x << " ";
				break;
			case 2:
				f << scientific << setprecision(15) << setw(25)
				  << it->x << " "
				  << scientific << setprecision(15) << setw(25)
				  << it->y << " ";
				break;
			case 3:
				f << scientific << setprecision(15) << setw(25)
				  << it->x << " "
				  << scientific << setprecision(15) << setw(25)
				  << it->y << " "
				  << scientific << setprecision(15) << setw(25)
				  << it->z << " ";
				break;
			case 4:
				f << scientific << setprecision(15) << setw(25)
				  << it->x << " "
				  << scientific << setprecision(15) << setw(25)
				  << it->y << " "
				  << scientific << setprecision(15) << setw(25)
				  << it->z << " "
				  << scientific << setprecision(15) << setw(25)
				  << it->t << " ";
				break;
			default:
				throw PointCurve::Exception_IOError(EXCEPTION_ID+"dim value out of range !");
		}

		for (size_t i = 0; i < nr_attributes; i++){
			f << it->attribute_at(i) << " ";
		}

		//! \note If you want to save bnd flags, here would be the place

		f << endl;

	}

	f.close();
	return true;
}

/** name: PointCurve::save_vpts()
 * Saves all nodes of the curve to a vpts file (cf. carp).
 *
 * \note For circular curves the last point will be removed because it is
 * identical to the first one.
 *
 * @param fn: filename of the file to be written
 * @return
 **/
bool PointCurve::save_vpts(const char* const fn) {
	if (size() == 0) return false;
	print("  - Writing *.vpts file : " << fn);

	ofstream f(fn);

	//!  First line: [# of vertices] [dimension]  [# of attributes] [# of boundary markers (0 or 1)]
	//!  Remaining lines: [vertex #] [x] [y] [attributes] [boundary marker]

	PointCurve::iterator itend = end();
	if (circular()){
		f << size()-1 << endl;
		itend--;
	} else
		f << size() << endl;
	for (PointCurve::iterator it = begin(); it != itend; it++){
		f << scientific << setprecision(15) << setw(25)
				  << it->x << " "
				  << scientific << setprecision(15) << setw(25)
				  << it->y << " "
				  << scientific << setprecision(15) << setw(25)
				  << it->z << " " << endl;
	}
	f.close();
	return true;

}

void PointCurve::read_node(const char* const fn){
	// read a node file, that describes the mesh
	print("  - Reading *.node file : " << fn);

	ifstream f(fn);

	size_t itm = 0;
	size_t dim = 0;
	size_t att = 0;
	size_t bnd = 0;

	//!  First line: [# of vertices] [dimension]  [# of attributes] [# of boundary markers (0 or 1)]
	//!  Remaining lines: [vertex #] [x] [y] [attributes] [boundary marker]
	int dummy = 0;
	f >> itm >> dim >> att >> bnd;
//	cout << itm << " " << dim << " " << att << " " << bnd << endl;

	if (f.fail()) PointCurve::Exception_IOError(EXCEPTION_ID+"Input error : failbit was set.");

	if (dim>4) throw PointCurve::Exception_IOError(EXCEPTION_ID+"Dimension is neither 1d,2d,3d nor 4d.");
	if (bnd > 1) cerr << "Warning: Some boundary markers will be ignored." << endl;
	this->clear();
	for (size_t i = 0; i < itm; i++){
		Point p;

		f >> dummy;

		if (dim > 0) f >> p.x;
		if (dim > 1) f >> p.y;
		if (dim > 2) f >> p.z;

		if ( att > 0 ) {
			for (size_t j = 0; j < att; j++) {
				f >> dummy;
				p.add_attribute(dummy);
			}
		}

		if ( bnd > 0 ) {
			f >> dummy;
			p.boundary(dummy);
			for (size_t j = 1; j < bnd; j++) f >> dummy;
		}
		if (not f.good()) throw PointCurve::Exception_IOError(EXCEPTION_ID+" Infile not good ("+toString(size())+ ") ! ");
		push_back(p);
	}

	f.close();
	return;
}

/** name: PointCurve::attribute
 *
 * Set the attribute for each point in range [start, end]. The attribute can be
 * used to define a region.
 *
 * \param value : attribute to be set
 * \param start : index where to start	(default : 0)
 * \param end	: index where to end = last item that is altered (default : 0)
 * \return none
 */
void PointCurve::attribute(const int value, const size_t start, size_t end){
	if ((start >= size() ) or (end >= size() ) )
		throw PointCurve::Exception_IndexError(EXCEPTION_ID);

	if (end < start) throw PointCurve::Exception_IndexError(EXCEPTION_ID+"end < start.");

	// standard value of 0 means all elements from start to the end
	if (end == 0) end = this->size() - 1;

	Point *pt = &this->operator[](start);
	for (size_t i = start; i < end + 1; i++){
		pt->clear_attributes(); // clear attribute that are already present
		pt->attribute(value);
		pt = &this->next(); // choose next point
	}
}

/** name: PointCurve::gen_segments()
 * Creates line segments that connect all points. Works only if the points
 * are ordered by arc length. The result is stored in the variable
 * this->segments.
 * \attention 	If the curve is circular() then the last point equals the first
 * 				point in the list, therefor the segments computed represent the
 * 				closed curve, too.
 **/
void PointCurve::gen_segments(){
	// build vector of segments
	segments.clear();
	Point p1 = this->operator[](0);
	for (size_t i = 0; i < this->size()-1; i++){
		Point p2 = this->next();
		segments.push_back(mylibs::Line(p1,p2, mylibs::Line::segment));
		p1 = p2;
	}
}

/** name: PointCurve::isflatxy()
 * The function PointCurve::inside() is only applicable to curves whose points
 * lie entirely in a xy-plane, otherwise it would not encircle one point.
 *
 * @return true or false
 **/
bool PointCurve::isflatxy() const {
	// check if all z-values are equal
	//Point pt = this->operator[](0);
	//double zval = pt.z;
		//if (pt.z != zval) return false;

	//return true;
	PointCurve::const_iterator it = this->begin();
	const double zval = it->z;
	for(it = this->begin(); it != this->end(); ++it) {
		if(zval != it->z) return false;
	}
	return true;
}

/** PointCurve::is_on_curve()
  * \param pt : 	The point for which should be tested if it is on the curve or not.
  * \param arc:		Pointer to the variables that stores the arc length,
  * 				standard: NULL (= no computation)
  * \return	: 	True or false whether the point belongs to the curve or not
  **/
bool PointCurve::is_on_curve(const Point &pt, double * const arc){
	if (segments.size() -1 != size()) gen_segments();

	double m = 0.;
	bool found = false;
	size_t itm = 0;
	// search if the point pt is on the segment
	for (size_t i = 0; i < segments.size(); i++){
		if (segments[i].is_on_line(pt, m)) {found = true; itm = i; break; }
	}

	if (arc != NULL and found){
		// m should be in [0., 1.]
		double aip = compute_arc_length(itm); 	// compute arc length of the Point at index i
		*arc = (aip + m * (segments[itm].length()));
	}

	return found;
}

/** PointCurve::point_at()
 *  Computes the point that fits to a given arc length. For circular curves
 *  the given position can be a multiple of the arc length.
 * \param arc : position of the point on the curve
 * \return the point that belogs to arc
 */
Point PointCurve::point_at(double arc){
	double a = 0.;
	double tmp = 0.;
	size_t i = 0;
//!\todo compute_arc_length if needed
	// for circular curves the position can be a multiple of the arc length
	if ( circular() and arc > arc_length()) arc = fmod(arc, arc_length());

	for (i = 0; i < nr_segments(); i++){
		tmp  = segment(i).length();
		if (a <= arc and a+tmp > arc ) break;
		else a+= tmp;
	}

	if (i == nr_segments()){
		if (a == arc) return segment(i).p2();
		else
		if (a > arc)
			throw myexception(EXCEPTION_ID+"arc value "+toString(arc)+"/" + toString(arc_length())+" to large.");
		else
			throw PointCurve::Exception_WhatTheHell(EXCEPTION_ID);
	}

	mylibs::Line &l = segment(i);
	double diff = (arc - a) / l.length();
	if ( diff > 1. ) throw myexception(EXCEPTION_ID+"diff > 1.0");

	Point result = l.point_at(diff);
	return result;
}

/** name: PointCurve::find_point_outside()
 * Searches for a point that is definitely outside the curve.
 * @return true or false
 **/
Point PointCurve::find_point_outside(){

	// first we test if the curve is flat in xy-plane
	Point pt = this->operator[](0);
	double zval = pt.z;
		if (pt.z != zval) throw myexception(EXCEPTION_ID+"Curve must lie completely in xy-plane");

	// (1) look for the point with the smallest x-coordinate
	Point p = this->operator[](0);
	for (size_t i = 1; i < this->size(); i++){
		Point &p2 = this->next();
		if (p2 < p) p = p2;
	}

	// (2) an now subtract 1., this point is definitly outside
	Point normal(1.,0.,0.);
	p -= normal;

	return p;
}

/** name: PointCurve::inside()
 * Checks if the curve encircles a point. A line segment is constructed that
 * points from the test point to another point that lies outside the curve.
 * Then the number of intersections with all line segments is computed. This
 * number is even if the point is inside the curve and odd if it is outside.
 *
 *******************************************************************************
 * \attention All points of the PointCurve must lie within the xy-plane to		*
 * find all intersections correctly. This										*
 * can be checked using PointCurve::isflatxy(). This is not done here for the  *
 * sake of speed.																*
 *******************************************************************************
 * \param point 		: The point for which we want to know if it is outside
 * \param point_outside	: A point that is definitly outside the curve.
 * @return true or false
 **/
bool PointCurve::inside(const Point &point, const Point &point_outside) {
	if (not this->circular()) throw myexception(EXCEPTION_ID+"The function\
 PointCurve::inside(Point&) is only useful for circular curves in xy-plane");

	// if segments where not created, do this now
	if (segments.size() -1 != size()) gen_segments();

	if (point == point_outside) return false; // otherwise the line would be degenerated

	mylibs::Line l = mylibs::Line(point, point_outside, mylibs::Line::segment);
	Point intersec;
	size_t cnt_intersections = 0;
	for (size_t i = 0; i < segments.size(); i++){
		mylibs::Line &seg = segments[i];

		if ( seg.intersection(l, intersec) ) {

			cnt_intersections++;
#ifdef DEBUG
			cout << "intersec:"; intersec.info();
			seg.p1().info();
			seg.p2().info();
			if (seg.p2() != intersec) cout << "inequal p2"<<endl;
			if (seg.p1() != intersec) cout << "inequal p1"<<endl;

			if (seg.p2() == intersec) cout << "equal p2"<<endl;
			if (seg.p1() == intersec) cout << "equal p1"<<endl;
#endif
			if ((intersec == seg.p1()) or (intersec == seg.p2())) return true;
		}

	}

#ifdef DEBUG
	cout << "intesections : " << cnt_intersections << endl;
#endif

	// even number of intersections means the point is outside
	if (cnt_intersections % 2 == 0) return false;
	else 							return true;
}


/** name: PointCurve::inside()
 * This is an alternative version of inside().
 * The test is done differently. First the center of mass (com) is computed.
 * Then a line is constructed using the test point and the com. The again all
 * the intersections of the line with the segments of the curve are computed.
 *
 * Since the former line - not only a segment as in inside(Point&, Point&) - is
 * infinitely extended it has multiple intersections with a closed curve (at least 2).
 *
 * From the parameter m one knows if the point is inside the curve.
 * m < 0 : intersection is left of the point p1
 * m > 0 : intersection is right of the point p1
 *
 * \attention
 * m = 0 : The test point is an intersection point itself. We count this one
 * as inside since we would miss the right number of intersections if we
 * wouldn't do this.
 *
 * Now we have to count the number of intersections that are left and right of
 * the point along the line.
 * If the point is outside, then all intersections are on the same side
 * opposite to the direction of the point.
 *
 * If the point is inside, then on both sides there is an odd number of
 * intersections. Also important to mention is that there is a change in the
 * sign for the m left of the point and the m right of it.
 *
 *******************************************************************************
 * \attention All points of the PointCurve must lie within the xy-plane to	 *
 * find all intersections correctly. This									  *
 * can be checked using PointCurve::isflatxy(). This is not done here for the  *
 * sake of speed.															  *
 *******************************************************************************
 *
 * \param point : The point for which we want to know if it is outside
 * @return true or false
 **/
bool PointCurve::inside(const Point &point) {
	if (not this->circular()) throw myexception(EXCEPTION_ID+"The function\
 PointCurve::inside(Point&) is only useful for circular curves in xy-plane");

	// if segments where not created, do this now
	if (segments.size() -1 != size()) gen_segments();

	Point com = this->center_of_mass();
	mylibs::Line l = mylibs::Line(point, com, mylibs::Line::line); //!< \bug This test is nonsense if the point is the center of mass itself !!! We need to check for the lenght of the line !!!
	Point intersec;
	size_t cnt_intersections = 0;

	size_t cnt_positive = 0;
	size_t cnt_negative = 0;
	double param = 0.;

	myList<double> ms;
	for (size_t i = 0; i < segments.size(); i++){
		mylibs::Line &seg = segments[i];
		bool found = seg.intersection(l, intersec);
		if ( found ) {
			cnt_intersections++;

			param = seg.intersection_parameter_mb();

			/** \attention if the intersection is the end point between two
			  * segments, then this point has to be counted only once.
			  * Otherwise we would count one intersection too much.
			  */
			if (not ms.contains(param)){
				ms.push(param);
				if (param <= 0.) cnt_negative++;
				else
				if (param > 0.) cnt_positive++;
			}
#ifdef DEBUG
			cout << ::std::setprecision(15) << " param " << param << endl;
#endif
		}
	}

#ifdef DEBUG
	cout << "intesection with pos. params :" << cnt_positive << endl;
	cout << "intesection with neg. params :" << cnt_negative << endl;
#endif
	//! \todo check this function !!!
	// Odd number of intersections on both sides means the point is inside
	if ((cnt_negative % 2 != 0) and (cnt_positive % 2 != 0)) return true;
	else // otherwise it must have been outside
		return false;
}

mylibs::LineNormalDirection PointCurve::where_is_inside(){
	if ( not circular() )throw myexception(EXCEPTION_ID+"PointCurve is not closed, so \
there's no defintion of inside or outside,");

	mylibs::Line l0 = mylibs::Line(operator[](0), operator[](1), mylibs::Line::segment);

	Point mid = l0.point_at(0.5);
	Point mid_l = mid + l0.normal_lhs();
	Point mid_r = mid + l0.normal_rhs();

	Point outside = find_point_outside();
	bool mid_l_inside = inside(mid_l, outside);
	bool mid_r_inside = inside(mid_r, outside);

	if (mid_l_inside && not mid_r_inside) return mylibs::Line_LHS;
	else
	if (mid_r_inside && not mid_l_inside) return mylibs::Line_RHS;

	return mylibs::Line_DIRECTION_UNDEFINED;
}

list<Point> PointCurve::intersections(const mylibs::Line &line){
	list<Point> result(0, Point());;
	Point inter = Point();
	if (segments.size() -1 != size()) gen_segments();
	for (size_t j = 0; j < nr_segments(); j++){
		mylibs::Line &seg = segment(j);
		if (	seg.intersection(line, inter) == true
				and inter != result.back()
				and inter != result.front() ){
			result.push_back(inter);
		}
	}
	return result;
}

list<Point> PointCurve::intersections(PointCurve &other){
	list<Point> result(0, Point());
	list<Point> tmp(0, Point());
	for (size_t i = 0; i < nr_segments(); i++){
		tmp = other.intersections(segment(i));
		while (tmp.size() > 0 ){
			if (tmp.front() != result.back()
					and tmp.front() != result.front()
				) result.push_back(tmp.front());
			tmp.pop_front();
		}
	}

	tmp.sort(PointCompare());// sort the list
	tmp.unique();			 // remove equal items
	return result;
}

/** name: PointCurve::perpendicular_foot_of()
 *	Computes the perpendicular foot on the curve for a given point pt.
 * 	How this is computed can be seen in Line::perpendicular_foot_of(). Since
 * 	there are multiple possibilities to constuct a perpendicular to the
 * 	PointCurve, the foot with the shortest distance to the point pt wil be
 * 	determined.
 * @param pt: Point for which the foot of perpendicular willbe computed
 * @param foot : A reference to the point where the result is to be stored.
 * @result True, if a point was found.
 */
bool PointCurve::perpendicular_foot_of(const Point& pt, Point &foot){
	if (segments.size() -1 != size()) gen_segments();

	double dist = -1.;
	double temp = -1.;
	Point tp;

	for (size_t i = 0; i < segments.size(); i++){
		segments[i].LineType(mylibs::Line::segment);
		if (segments[i].perpendicular_foot_of(pt, tp)){
			temp = (Point(pt) - tp).abs();
			if (dist < 0. or temp < dist ){
				foot = tp;
				dist = temp;
			}
		}
		segments[i].LineType(mylibs::Line::segment);
	}

	return (dist < 0.) ? false : true;
}

#define mini(a,b) ( (a < b) ? ( a ) : ( b ) )
#define maxi(a,b) ( (a > b) ? ( a ) : ( b ) )

#ifdef HAVE_LIBGSL
/** name: PointCurve::compute_spline()
 *
 * Computes a spline for a given PointCurve in 3D. One can select which fraction
 * of points of this curve should be used for interpolation.
 *
 * This function is also computes 1st od 2nd derivatives with respect to the
 * arc-length of the curve.
 *
 * @param fraction	: fraction of point which should be used for interpolation [0., 1.] (standard is 0.3)
 * @param resolution : resolution of the interpolated curve
 * @param deriv		: Which derivative should be computed ? (0: only spline,
 *							1: spline of 1st derivative , 2: spline of 2nd derivative )
 * @param save_files : For debugging reasons the files curve.dat and spline.dat can be created to plot the curves.
 * @return A list of Points sorted by arc length (= PointCurve)
 */
PointCurve PointCurve::compute_spline(double fraction, double resolution, const unsigned short deriv, bool verbose){
	if (verbose)
		print("Doing spline interpolation of PointCurve... ");


	// error handling
	if ((fraction > 1.) or (fraction < 0.))
		throw PointCurve::Exception_RangeViolation(EXCEPTION_ID+"fraction must be out of [0. , 1.]");
	if (this->size() < 2)
		throw PointCurve::Exception_CurveTooShort(EXCEPTION_ID+"A curve must have at least 2 points");

	if  ( deriv > 2 )
		throw PointCurve::Exception_NotImplemented(EXCEPTION_ID+" deriv out of range (0-2). Higher derivatives are not implemented.");

	// steps between
	size_t steps = (size_t) (1./fraction); 				//! stepsize
	size_t len = (size_t) (this->size()/(double)steps); 	//! # of steps = # of sampling points

	if (verbose) {
		print(" - Creating arrays for data in x,y,z of size " << len+1);
		print(" - steps = " << steps);
	}

	//! Prepare data to be interpolated
	double *xval = new double[len+1]; // attention +1 is needed for circular curves
	double *yval = new double[len+1];
	double *zval = new double[len+1];
	double *grid = new double[len+1]; // may not be within the selected points

	// compute the arc length of the curve
	double *arc  = new double[size()];
	double data_resolution_max = (operator[](1) - operator[](0)).abs();
	double data_resolution_min = (operator[](1) - operator[](0)).abs();

	double v_arc_length = 0;
	arc[0] = 0.; // start of the curve is 0.
	for (size_t i = 1; i < this->size(); i++){
		double abs = (operator[](i) - operator[](i-1)).abs();
		if (verbose){
			data_resolution_max = (data_resolution_max < abs) ? abs : data_resolution_max;
			data_resolution_min = (data_resolution_min > abs) ? abs : data_resolution_min;
		}
		v_arc_length += abs;
		arc[i] = v_arc_length;
	}

	if (verbose)
		print(" - Arc length of the curve : " << v_arc_length);

	// select all points used for interpolation
	size_t interpol = 0;						// definitely use the first point for interpolation
	for (size_t cnt = 0; cnt < len ; cnt++){
		Point &pt = operator[](interpol);
		xval[cnt] = pt.x;
		yval[cnt] = pt.y;
		zval[cnt] = pt.z;
		grid[cnt] = arc[interpol];

		if (cnt < (len-1)) interpol+=steps; // while not at the end look for the next point to interpolate
		/*! \attention :	Do not increase value of interpol when at the end of loop.
		 * 					We will	use	this value to check, if the last point of the
		 * 					curve was included in the interpolation list. */
	}

	if (circular()){
		if (interpol != this->size()-1){ // append last point if not already happend
			Point idx = operator[](-1);  // select the last point of the curve
			xval[len] = idx.x;
			yval[len] = idx.y;
			zval[len] = idx.z;
			grid[len] = arc[this->size()-1];
			len++; // increase the number of items by one
		}
	}

	if (verbose){
		print(" - Maximum resolution of the data: " << data_resolution_max);
		print(" - Minimum resolution of the data: " << data_resolution_min);
		print(" - # of interpolation points : " << len << " ( of " << this->size() << " )");
	}

	if (len <= 3) throw PointCurve::Exception_CurveTooShort(EXCEPTION_ID);

	gsl_interp_accel *accx = gsl_interp_accel_alloc();
	gsl_interp_accel *accy = gsl_interp_accel_alloc();
	gsl_interp_accel *accz = gsl_interp_accel_alloc();


	const gsl_interp_type *t = ( circular() == true) ?  gsl_interp_cspline_periodic : gsl_interp_cspline;

	gsl_spline *splinex = gsl_spline_alloc (t, len);
	gsl_spline *spliney = gsl_spline_alloc (t, len);
	gsl_spline *splinez = gsl_spline_alloc (t, len);

	// how does GSL error handling work ?
	// In principle the following code works, but the error sometimes is thrown already in gsl_spline_alloc()
//	unsigned int min_size = gsl_spline_min_size (splinex);
//	if (len-1 <= min_size) throw PointCurve::Exception_CurveTooShort(EXCEPTION_ID + toString(min_size));

	if (verbose) print(" - gsl_spline_init(..) with len = "<< len);

	gsl_spline_init(splinex, grid, xval, len);
	gsl_spline_init(spliney, grid, yval, len);
	gsl_spline_init(splinez, grid, zval, len);


	if (verbose) print("	-> Initialization done.");
	//! Step2 ) evaluate the spline on the grid
	/** inline void
	  * eval_UBspline_1d_d(UBspline_1d_d * restrict spline, double x, double* restrict val);*/
	PointCurve spline;
	double maximum = v_arc_length;
	if (this->circular()) maximum = v_arc_length - resolution;

	switch (deriv){
	case 0:
		if (verbose) print(" - gsl_spline_eval(..)");
		for (double x=0.; x<= maximum; x+=resolution) {
			double vx = gsl_spline_eval(splinex, x, accx);
			double vy = gsl_spline_eval(spliney, x, accy);
			double vz = gsl_spline_eval(splinez, x, accz);
			Point pt = Point(vx, vy, vz);
			spline.append(pt);
		}
		break;
	case 1:
		if (verbose) print(" - gsl_spline_eval_deriv(..)");
		for (double x=0.; x<= maximum; x+=resolution) {
			double vx = gsl_spline_eval_deriv(splinex, x, accx);
			double vy = gsl_spline_eval_deriv(spliney, x, accy);
			double vz = gsl_spline_eval_deriv(splinez, x, accz);
			Point pt = Point(vx, vy, vz);
			spline.append(pt);
		}
		break;
	case 2:
		if (verbose) print(" - gsl_spline_eval_deriv2(..)");
		for (double x=0.; x<= maximum; x+=resolution) {
			double vx = gsl_spline_eval_deriv2(splinex, x, accx);
			double vy = gsl_spline_eval_deriv2(spliney, x, accy);
			double vz = gsl_spline_eval_deriv2(splinez, x, accz);
			Point pt = Point(vx, vy, vz);
			spline.append(pt);
		}
		break;
	}
	if (verbose) print(" - gsl_spline_free(..)");
	gsl_spline_free (splinex);
	gsl_spline_free (spliney);
	gsl_spline_free (splinez);
	gsl_interp_accel_free (accx);
	gsl_interp_accel_free (accy);
	gsl_interp_accel_free (accz);

	/*! If the original spline was circular, then we append the first point at
	 *  the end again to make this one circular() to. **/
	if (this->circular()) spline.append(spline.front());

	if (verbose){
		FILE *out = fopen("curve.dat", "w");
		if ( out == NULL ) throw PointCurve::Exception_IOError(EXCEPTION_ID+"Could not open curve.dat.");
		for (size_t x=0; x<len; x++)
			fprintf (out, "%.6f %.6f %.6f %.6f \n", arc[x], xval[x], yval[x], zval[x]);
		fclose(out);

		out = fopen("spline.dat", "w");
		if ( out == NULL ) throw PointCurve::Exception_IOError(EXCEPTION_ID+"Could not open spline.dat.");
		double arcl = 0;
		for (size_t x=0; x< spline.size(); x++) {
			fprintf (out, "%.6f %.6f %.6f %.6f\n", arcl, spline[x].x, spline[x].y, spline[x].z);
			arcl += resolution;
		}
		fprintf (out, "\n");
		fclose(out);
	}
	delete[] xval;
	delete[] yval;
	delete[] zval;
	delete[] grid;
	delete[] arc;
	return spline;
}
// no GSL
#elif USE_EINSPLINE
/** name: PointCurve::compute_spline
 *
 * Computes a spline for a given PointCurve in 3D. One can select which fraction
 * of points of this curve should be used for interpolation.
 *
 * Therefore the arc length is computed as non uniform grid for the
 * interpolation with einspline.
 *
 * One can set the resolution of the interpolated curve.
 *
 * @param fraction: fraction of point which should be used for interpolation [0., 1.] (standard is 0.3)
 * @param resolution : resolution of the interpolated curve
 * @param save_files: For debugging reasons the files curve.dat and spline.dat can be created to plot the curves.
 * @return A list of Points sorted by arc length (= PointCurve)
 */
PointCurve PointCurve::compute_spline(double fraction, double resolution, bool save_files){
	#ifdef DEBUG
		print("Doing spline interpolation of PointCurve... ");
	#endif

	// error handling
	if ((fraction > 1.) or (fraction < 0.)) throw PointCurve::Exception_RangeViolation(EXCEPTION_ID+"fraction must be out of [0. , 1.]");
	if (this->size() < 2) throw PointCurve::Exception_CurveTooShort(EXCEPTION_ID);

	// steps between
	int steps = (int) (1./fraction); 				//! stepsize
	int len = (int) (this->size()/(double)steps); 	//! # of steps = # of sampling points

	//! Prepare data to be interpolated
	double *xval = new double[len+1]; // Arrays must be one element longer,
	double *yval = new double[len+1]; // possibly we need the last element
	double *zval = new double[len+1]; // because the last point of the curve
	double *grid = new double[len+1]; // may not be within the selected points

	double *arc  = new double[this->size()];

	// compute the arc length of the curve
	double data_resolution = (operator[](1) - operator[](0)).abs();
	double v_arc_length = 0;
	arc[0] = 0.; // start of the curve is 0.
	for (size_t i = 1; i < this->size(); i++){
		double abs = (operator[](i) - operator[](i-1)).abs();
		data_resolution = maxi(data_resolution, abs);
		v_arc_length += abs;
		arc[i] = v_arc_length;
	}

	#ifdef DEBUG
		print(" - Arc length of the curve : " << v_arc_length);
	#endif

	// select all points used for interpolation
	size_t interpol = 0;						// definitely use the first point for interpolation
	for (int cnt = 0; cnt < len ; cnt++){
		Point &pt = operator[](interpol);
		xval[cnt] = pt.x;
		yval[cnt] = pt.y;
		zval[cnt] = pt.z;
		grid[cnt] = arc[interpol];

		if (cnt < (len-1)) interpol+=steps; // while not at the end look for the next point to interpolate
		/*! \attention :	Do not increase value of interpol when at the end of loop.
		 * 					We will	use	this value to check, if the last point of the
		 * 					curve was included in the interpolation list. */
	}

	if (interpol != this->size()-1){ // append last point if not already happend
		Point idx = operator[](-1); // select the last point of the curve
		xval[len] = idx.x;
		yval[len] = idx.y;
		zval[len] = idx.z;
		grid[len] = arc[this->size()-1];
		len++; // increase the number of items by one
	}
	delete[] arc;

	#ifdef DEBUG
		print(" - Maximum resolution of the data: " << data_resolution);
		print(" - # of interpolation points : " << len << "( of " << this->size() << " )");
	#endif


	//NUgrid* create_general_grid (double *points, int num_points);
	#ifdef DEBUG
		print(" - Interpolation interval : [" << grid[0]<<","<< grid[len-1]<<"]");
	#endif
	/***/
	NUgrid  *x_grid = create_general_grid(grid,len);


	/** Boundary Conditions should be periodic for circular curves**/
	BCtype_d xBC = {NATURAL, NATURAL , 0.,0.};
	if ( this->circular() ) {
		xBC.lCode = PERIODIC;
		xBC.rCode = PERIODIC;
	}

	//! Step1 ) Create a bspline object
	/**UBspline_1d_d * create_UBspline_1d_d (Ugrid x_grid, BCtype_d xBC, double *data); */
	NUBspline_1d_d *spline_1d_x = create_NUBspline_1d_d (x_grid, xBC, xval);
	NUBspline_1d_d *spline_1d_y = create_NUBspline_1d_d (x_grid, xBC, yval);
	NUBspline_1d_d *spline_1d_z = create_NUBspline_1d_d (x_grid, xBC, zval);

	//! Step2 ) evaluate the spline on the grid
	/** inline void
	  * eval_UBspline_1d_d(UBspline_1d_d * restrict spline, double x, double* restrict val);*/
	PointCurve spline;
	double maximum = v_arc_length;
	if (this->circular()) maximum = v_arc_length - resolution;

	for (double x=0.; x<= maximum; x+=resolution) {
		double vx=0., vy=0., vz=0.;
		eval_NUBspline_1d_d(spline_1d_x, x, &vx);
		eval_NUBspline_1d_d(spline_1d_y, x, &vy);
		eval_NUBspline_1d_d(spline_1d_z, x, &vz);

		Point pt = Point(vx, vy, vz);
		spline.append(pt);
	}

	//! if the original spline was circular, then we append the first point at the end again.
	if (this->circular()) spline.append(spline.front());

	#ifdef DEBUG
		print("Freeing memory\n");
	#endif
	//! Step 3) destroy the bspline object
	/**void destroy_Bspline (void *spline);**/
	destroy_Bspline(spline_1d_x);
	destroy_Bspline(spline_1d_y);
	destroy_Bspline(spline_1d_z);
	destroy_grid(x_grid);

	if (save_files){
		FILE *out = fopen("curve.dat", "w");
		if ( out == NULL ) throw PointCurve::Exception_IOError(EXCEPTION_ID+"Could not open curve.dat.");
		for (int x=0; x<len; x++)
			fprintf (out, "%.6lf %.6lf %.6lf %.6lf \n", grid[x], xval[x], yval[x], zval[x]);
		fclose(out);


		out = fopen("spline.dat", "w");
		if ( out == NULL ) throw PointCurve::Exception_IOError(EXCEPTION_ID+"Could not open spline.dat.");
		double arc = 0;
		for (size_t x=0; x< spline.size(); x++) {
			fprintf (out, "%.6lf %.6lf %.6lf %.6lf\n", arc, spline[x].x, spline[x].y, spline[x].z);
			arc += resolution;
		}
		fprintf (out, "\n");
		fclose(out);
	}

	delete[] xval;
	delete[] yval;
	delete[] zval;
	delete[] grid;
	return spline;
}
#endif
#undef maxi
#undef mini

/** name: PointCurve::arc_length()
 * Computes the arc-length of a point on the PointCurve for a given iterator.
 * @param itend: iterator to the point to compute the arc-length for
 * @return arc length value
 **/
double PointCurve::arc_length(PointCurve::const_iterator itend){
	double arc = 0.;

	if (itend != begin())
		itend--; // always step to the element  before the last element

	PointCurve::const_iterator it 	 = begin();
	PointCurve::const_iterator it_next = begin(); it_next++; // always point to the following element
	for (it = begin(); it != itend; it++,it_next++){
		arc += (*it_next - *it).abs();
	}

	v_arc_length = arc;
	return arc;
}

 /** name: normal()
  *
  * @param it: iterator pointing to the point of the curve, where the normal shall be computed.
  * @return A normalized vector pointing normal to the curve.
  **/
Point PointCurve::normal(PointCurve::const_iterator it) const {

	Point n;
	Point t(tangent(it));

	n.x = -t.y; // left hand side normal
	n.y = t.x;
	n.z = t.z;
	n.normalize(); // exception is thrown if n cannot be normalized
	return n;
}

/**
 * PointCurve::tangent()
 * Computes the tangent at the point given by the iterator it. The tangent is
 * computed as the mean value of the difference vectors to the previous and
 * the following neighbour along the curve. It is also checked if the
 * curve is circular, then the appropriate neighbours are chosen, if the
 * selected point is at the beginning or the end of the curve.
 *
 * @param it: Iterator for one point
 * @return Normalized tangential vector.
 **/
Point PointCurve::tangent(PointCurve::const_iterator it) const {
	PointCurve::const_iterator itm(it);
	// Handle a size of zero, one or two in a specificaly way.
	if(size() > 2) {
		++it;
		if(it == end()) {
			if(circular()) 	{--itm;	it = begin(); ++it;	}
			else 			{--itm;--it;}
		} else
		if(itm == begin()) {
			if(circular()) {itm = end();--itm;--itm;}
		} else 				{--itm;	}
	} else if(size()==2) {
		// If there are only two points, one is the first, the other the last.
		it = end(); --it; itm = begin();
	} else if(size() < 2) {
		/*	Only one point, so both iterators are the same or there are no
		 *	points. In both cases a zero-point will be returned, because this
		 *	couldn't be normalized.
		 */
		throw PointCurve::Exception_CurveTooShort(EXCEPTION_ID);
	}

	Point t(*it - *itm);
	t.normalize();
	return t;

}

 /** name: PointCurve::curvature
  *	Computes the curvature of a PointCurve by choosing 3 points, which distance
  * corresponds to the windowsize. Assuming that these points lie on a circle
  * one can calculate the radius r by using the formula for the computation of a
  * circumscribed circle (* http://en.wikipedia.org/wiki/Circumscribed_circle)
  *
  * The 3 Points are denoted by \f$ \vec A, \vec B\f$ and \f$\vec C\f$.
  *
  * We define
  * \f$ \vec a = \vec A - \vec C \f$ and \f$ \vec b = \vec B - \vec C\f$
  *
  * The radius is then
  *
  * \f[
		r = \frac{|\vec a| \cdot |\vec b| \cdot |\vec a - \vec b|}
		 			{ 2 | \vec a \times \vec b|}
	\f]
  *
  * The curvature of a circle is \f$ k=1/r \f$.
  * @param windowsize: value of the distance between the points.
  * @return array of the curvature for each point of a PointCurve as double.
  **/
double* PointCurve::curvature(int windowsize) {
	if ( size() < 2 ) throw PointCurve::Exception_CurveTooShort(EXCEPTION_ID);

	bool closed = circular();
	int n = (int) this->size();
	if(windowsize>=n) windowsize=n-1;	// set the maximum window size
	double* c = new double[n]; // array for the curvature

	/** We use 3 Points on the curve for the computation:
	 * 	x --- x --- x
	 * (1)	(2)	(3)
	 *  Windowsize ( --- ) defines the distance in indices.
	 * std:advance also works if the begin or the end of the list is reached.
	 * In the case the curve is closed, points at the end of the list are
	 * neighbours of (2). For closed curves the last point equals the first
	 * point. Also we have to omit end(), since this iterator points to the
	 * end of the list, which contains no data.
	 **/
	for(int i=0;i< n;i++){
		PointCurve::iterator i2 = begin(); std::advance(i2, i);
		PointCurve::iterator i1 = begin();
		PointCurve::iterator i3 = begin();
		if (not closed){
			if ((i == 0) or (i == n-1)) continue; // in this case no valid neighbours

			if ( i - windowsize > 0) 	std::advance(i1, i-windowsize);
			else i1 = begin();

			if ( i + windowsize < n) 	std::advance(i3, i+windowsize);
			else std::advance(i3, n-1);

		} else {
			if ( i - windowsize < 0) 	std::advance(i1, i-windowsize-2);
 			else					 	std::advance(i1, i-windowsize  );

			if ( i + windowsize > n-1) 	std::advance(i3, i+windowsize+2);
			else						std::advance(i3, i+windowsize);
		}

		Point a = *i1 - *i3;
		Point b = *i2 - *i3;


		Point  cp = a.cross(b);
		double sd = ( a.norm() * b.norm() * (a-b).norm() );
//		c[i] = sqrt( (4. * cp.norm()) / (a.norm() * b.norm() * (a-b).norm()) );

		if(sd!=0.)c[i] = sqrt( (4. * cp.norm()) / sd );
		else throw Point::Exception_NaN(EXCEPTION_ID + "Denominator is zero.");

		if (cp.z>0.) c[i] *= -1.; // set the sign of the curvature
	}

	if (not closed){
		c[0]	= c[1];
		c[n-1]	= c[n-2];
	}
	return c;
}


/** name: PointCurve::mean_curvature
 * Computes the mean curvature of the complete curve using PointCurve::curvature().
 * @return The curvature value.
 **/
double PointCurve::mean_curvature(int windowsize) {
	double value_all=0;
	size_t n=this->size();
	double* s_array = curvature(windowsize);
	for(size_t i=0;i<n;i++)value_all+=s_array[i];
	delete s_array;
	return (value_all/(double)size());
}

  /** name: PointCurve::curvature
  *	Computes the curvature of a PointCurve by computing the (forward) tangent
  * for a given point denoted by the iterator it.
  *
  * @param it: iterator to the curve.
  * @return Value of the curvature as double.
  **/
double PointCurve::curvature(const PointCurve::const_iterator it) const {
	PointCurve::const_iterator ip = it; ip++;		// ip: the following point
	if( ip==end() ){ 								// ip is already behind the last point (=it) of the curve.
		if (circular()){							// If circular curve, then
			if ( size() < 3 ) throw PointCurve::Exception_CurveTooShort(EXCEPTION_ID);
			ip = begin();ip++;		 				// choose the second point in the list
			Point dt  = tangent(ip) - tangent(it); 	// Tangentendifferenz berechnen
			Point n	= normal(it);					// Normale
			double s  = n.dot(dt);					// Skalarprodukt
			double dp = (*ip - *it ).abs(); 		// berechne Abstände

			// Krümmung
			if(dp==0.)	throw PointCurve::Exception_ZeroDistance(EXCEPTION_ID);
			else 		return (s < 0.) ? -1.*(dt/dp).abs() : (dt/dp).abs();
		}
		else return 0.; // no curvature for the last point

	} else{											 //else normal point
		if ( size() < 2 ) throw PointCurve::Exception_CurveTooShort(EXCEPTION_ID);
		Point dt = tangent(ip) - tangent(it);	 	//Tangentendifferenz berechnen
		Point n = normal(it);						// Normale
		double s = n.dot(dt);						// Skalarprodukt
		double dp = (*ip - *it ).abs(); 			// berechne Abstände

		// Krümmung
		if(dp==0.)	throw PointCurve::Exception_ZeroDistance(EXCEPTION_ID);
		else 		return (s < 0.) ? (dt/dp).abs() : -1. * (dt/dp).abs();
	}
	throw PointCurve::Exception_WhatTheHell(EXCEPTION_ID);
}// method curvature

/** name: PointCurve::mean_curvature
 * Computes the mean curvature of the complete curve using PointCurve::curvature().
 * @return The curvature value.
 **/
double PointCurve::mean_curvature() const{
	double value_all=0;
	for (PointCurve::const_iterator it = begin(); it != end(); it++){
		value_all+=curvature(it);
	}
	return (value_all/(double)size());
}

/** name: PointCurve::speed_normal()
 * Computes the normal speed at the given point *it. Therefor a copy of the
 * same curve for another time is needed. The function computes the shortest
 * normal distance between both curves (There could be more intersections
 * with of curve's normal and the 'future' curve.) Together with the time
 * difference this gives the approximated velocity.
 * @param it: Iterator of the point of interest
 * @param other: A copy of the curve for t+dt.
 * @param dt: time difference between both curves.
 * @return Normal velocity as a Point (vector)
 **/
Point PointCurve::speed_normal(PointCurve::const_iterator it, PointCurve &other, const double dt) const {
	Point norm = normal(it);

	mylibs::Line l = mylibs::Line(*it,*it + norm);

	list<Point> intersections = other.intersections(l);
	Point min=intersections.front();
	double dist = (*it - min).abs();
	for(PointCurve::iterator i= intersections.begin(); i != intersections.end(); i++){
		double d = (*it - *i).abs();
		if(d < dist ) {
			min=*i;
			dist = d;
		}
	}

	return (*it -min) / dt;
}

/** name: PointCurve::speed_x()
 * Similar to PointCurve::speed_normal(PointCurve::const_iterator it,
 * PointCurve &other, const double dt), but assuming that front propagation is
 * in x-direction ONLY and that there is a set of'future' curves,
 * for which we don't know which one is the correct one. Again
 * the closest intersection of the 'future' curve and the curve's normal is
 * returned.
 * \attention 	If front propagation differs from x direction this function
 * 				produces results that no longer have any meaning.
 *
 * @param it: Iterator of the point of interest
 * @param other: A set of curves, assuming one of them is the copy of this curve for t+dt.
 * @param dt: time difference between both curves.
 * @return Normal velocity as a Point (vector)
 **/
Point PointCurve::speed_x(PointCurve::const_iterator it, list <PointCurve> &other, const double dt) const {
	Point *minima = new Point[other.size()];
	Point xpoint=*it;
	(xpoint).x=(*it).x+1;
	mylibs::Line l = mylibs::Line(*it,xpoint);
	size_t cnt = 0;
	for(list<PointCurve>::iterator j = other.begin(); j != other.end(); ++j, ++cnt){
		list<Point> intersections = j->intersections(l);
		minima[cnt]=intersections.front();
		double dist = (*it - minima[cnt]).abs();
		for(PointCurve::iterator i= intersections.begin(); i != intersections.end(); i++){
			double d = (*it - *i).abs();
			if(d < dist ) {
				minima[cnt]=*i;
				dist = d;
			}
		}
	}
	Point min=minima[0];
	for(unsigned int m = 1; m < (other.size()); m++){
		if((*it - minima[m-1]).abs()>(*it - minima[m]).abs()) min=minima[m];
	}
	delete[] minima;
	return (*it -min) / dt;
}


/** name: PointCurve::speed_normal()
 * Similar to PointCurve::speed_normal(PointCurve::const_iterator it,
 * PointCurve &other, const double dt), but assuming that there is a set of
 * 'future' curves, for which we don't know which on is the correct one. Again
 * the closest intersection of the 'future' curve and the curve's normal is
 * returned.
 * @param it: Iterator of the point of interest
 * @param other: A set of curves, assuming one of them is the copy of this curve for t+dt.
 * @param dt: time difference between both curves.
 * @return Normal velocity as a Point (vector)
 **/
Point PointCurve::speed_normal(PointCurve::const_iterator it, vector<PointCurve> &other, const double dt) const {
	Point norm = normal(it);
	Point *minima = new Point[other.size()];
	mylibs::Line l = mylibs::Line(*it,*it + norm);
	for(unsigned int j = 0; j < (other.size()); ++j){
		list<Point> intersections = other[j].intersections(l);
		minima[j]=intersections.front();
		double dist = (*it - minima[j]).abs();
		for(PointCurve::iterator i= intersections.begin(); i != intersections.end(); i++){
			double d = (*it - *i).abs();
			if(d < dist ) {
				minima[j]=*i;
				dist = d;
			}
		}
	}
	Point min=minima[0];
	for(unsigned int m = 1; m < (other.size()); ++m){
		if((*it - minima[m-1]).abs()>(*it - minima[m]).abs())min=minima[m];
	}
	delete[] minima;
	return (*it -min) / dt;
}

/** name: PointCurve::speed_normal()
 * Similar to PointCurve::speed_normal(PointCurve::const_iterator it,
 * PointCurve &other, const double dt), but assuming that there is a set of
 * 'future' curves, for which we don't know which on is the correct one. Again
 * the closest intersection of the 'future' curve and the curve's normal is
 * returned.
 * @param it: Iterator of the point of interest
 * @param other: A set of curves, assuming one of them is the copy of this curve for t+dt.
 * @param dt: time difference between both curves.
 * @return Normal velocity as a Point (vector)
 **/
Point PointCurve::speed_normal(PointCurve::const_iterator it, list<PointCurve> &other, const double dt) const {
	Point norm = normal(it);
//	Point minima[other.size()] ;
	Point *minima = new Point[other.size()];
	mylibs::Line l = mylibs::Line(*it,*it + norm);
	size_t cnt = 0;
	for(list<PointCurve>::iterator j = other.begin(); j != other.end(); ++j, ++cnt){
		list<Point> intersections = j->intersections(l);
		minima[cnt]=intersections.front();
		double dist = (*it - minima[cnt]).abs();
		for(PointCurve::iterator i= intersections.begin(); i != intersections.end(); i++){
			double d = (*it - *i).abs();
			if(d < dist ) {
				minima[cnt]=*i;
				dist = d;
			}
		}
	}
	Point min=minima[0];
	for(unsigned int m = 1; m < (other.size()); ++m){
		if((*it - minima[m-1]).abs()>(*it - minima[m]).abs())min=minima[m];
	}
	delete[] minima;
	return (*it -min) / dt;
}

/** name: ::translate((const Point & t)
 * Translates all the points of the curve by a constant vector.
 * @param t: vector that is used for tranlating all the points of the curve
 **/
void PointCurve::translate(const Point & t){
	for (PointCurve::iterator it = begin(); it != end() ; it++){
		*it += t;
	}
}

#ifdef HAVE_LIBGSL
// functions needed for computing an ellipse

struct ellipse_params{
 double a;	// major axis
 size_t k;	// point index
 double S_n; // circumference/n
 double e;	// eccentricity
};


double ellipse_arc(double x, void *params){
	struct ellipse_params *p = (struct ellipse_params *) params;
	return p->a * gsl_sf_ellint_E(x, p->e, GSL_PREC_DOUBLE) - p->k * p->S_n;
}

double ellipse_deriv (double x, void *params){
	struct ellipse_params *p = (struct ellipse_params *) params;
	double sinsq = sin(x)*sin(x);
	return p->a * sqrt(1. - p->e * p->e * sinsq);
}

void ellipse_fdf (double x, void *params, double *y, double *dy){
	struct ellipse_params *p = (struct ellipse_params *) params;
	// function
	*y =  p->a * gsl_sf_ellint_E(x, p->e, GSL_PREC_DOUBLE) - p->k * p->S_n;
	// derivative
	double sinsq = sin(x)*sin(x);
	*dy = p->a * sqrt(1. - p->e * p->e * sinsq);
}



PointCurve PointCurve::Ellipse(const double a, const double b, const size_t n, const double fraction ){

	if (fraction < 0.0 or fraction > 1.0) throw PointCurve::Exception_RangeViolation(EXCEPTION_ID+ "fraction not in [0.,1.]");

	PointCurve ellipse;

	// compute circumference
	double e = sqrt((a*a - b*b)/(a*a)); // eccentricity

	//	 gsl_sf_ellint_E (double phi, double k, gsl_mode_t mode)
	// circumference = a E
	double circ = fraction * a * gsl_sf_ellint_E(2.*M_PI, e, GSL_PREC_DOUBLE);

	int status;
	int iter = 0, max_iter = 100;
	const gsl_root_fdfsolver_type *T;
	gsl_root_fdfsolver *s;
	double phi0, phi = 0;
	gsl_function_fdf FDF;
	struct ellipse_params params = {a, 0, circ/n, e};

	FDF.f      = &ellipse_arc;
	FDF.df     = &ellipse_deriv;
	FDF.fdf    = &ellipse_fdf;
	FDF.params = &params;

//	T = gsl_root_fdfsolver_newton;
	T = gsl_root_fdfsolver_steffenson;
	s = gsl_root_fdfsolver_alloc (T);

	for (size_t k = 0; k < n; k++){
		iter = 0;
		params.k = k;
		// initial guess is k 2 pi/n which is exact for the circle
		phi = k*2.*M_PI/n;
		gsl_root_fdfsolver_set (s, &FDF, phi);
//		printf("guess = %f\n", x);
//		printf ("%-5s %10s %10s\n",	"iter", "root", "err(est)");
		do
		 {
			iter++;
			status = gsl_root_fdfsolver_iterate (s);
			phi0 = phi;
			phi = gsl_root_fdfsolver_root (s);
			status = gsl_root_test_delta (phi, phi0, 0, 1e-12);

//			if (status == GSL_SUCCESS) printf ("Converged:\n");

//			printf ("%5d %10.7f %10.7f\n", iter, x, x - x0);
		 }
		while (status == GSL_CONTINUE && iter < max_iter);

		if (status != GSL_SUCCESS) fprintf (stderr, "Did not converge !\n");

		PointPolar p((b / (sqrt( 1.-e*e*cos(phi)*cos(phi)))), phi);
		ellipse.append(Point(p));
	}
	gsl_root_fdfsolver_free (s);
	ellipse.append(ellipse[0]);

	return ellipse;
}


PointCurve PointCurve::Ellipse(const double a, const double b, const double segment_length, const double fraction ){

	if (fraction < 0.0 or fraction > 1.0) throw PointCurve::Exception_RangeViolation(EXCEPTION_ID+ "fraction not in [0.,1.]");
	// compute circumference
	double e = sqrt((a*a - b*b)/(a*a)); // eccentricity

	// gsl_sf_ellint_E (double phi, double k, gsl_mode_t mode)
	// circumference = a E
	double circ = fraction * a * gsl_sf_ellint_E(2.*M_PI, e, GSL_PREC_DOUBLE);

	// number of segments
	const size_t n = (size_t)ceil(circ/segment_length);

	return Ellipse(a,b,n, fraction);
}

PointCurve PointCurve::Circle(const double r, const double segment_length){
	return Ellipse(r,r, segment_length);
}

PointCurve PointCurve::Circle(const double r, const size_t nr_segments){
	double segment_length = 2. * M_PI * r / ((double) nr_segments);
	return Ellipse(r,r, segment_length);
}
#endif

/** name: find_line_b
 *
 * This is the path finding algorithm. As params we need a map of
 * Lines, which need to be sorted. The second param is the line which
 * is the last path element which was found. find_line_b searches for
 * a point index which equals line.b.
 * \attention Throws an exception if no point was found
 *
 * @param line : This is the last line which was found.
 * @return Point index of the next path point (int).
 */
int LineMap::find_line_b(const myLine& line ) const {
	// we always search for line.b in the other lines
	myMap<myLine, int>::const_iterator it;

	for ( it = begin(); it != end(); it++){
		if ((it->first.a == line.b) && (it->first.b != line.a))
			return it->first.b;
		else
		if ((it->first.b == line.b) && (it->first.a != line.a))
			return it->first.a;
	}

	throw PointCurve::Exception_PointNotFound(EXCEPTION_ID+"Could not find a matching point.");
}

/** name: LineMap::path_finder
 *
 * Sorts a list of lines that way, that points are ordered by arc length.
 * Finds circular paths as well as paths with a certain start and end.
 * Returns a list of PointCurve (myList<Curve>)
 *
 * @return myList<Curve> = List of Curves
 */
myList<Curve> LineMap::path_finder() {
	myList<Curve> raender;

	while (size() > 0){
		Curve rand;

		//choose the first vector in mymap
		myLine line = myLine(begin()->first.a,begin()->first.b) ;
		rand.append(line.a); //append first two points
		rand.append(line.b);
		erase(line); // ... and delete it

		while(1==1){
			myLine line = myLine(rand[-2],rand[-1]);

			int next;
			try {
				next = this->find_line_b(line);
				rand.append(next);
				this->erase(myLine(rand[-2],rand[-1])); // ... and delete the last line segment
			}
			catch (string e){
				cout << "Exception : " << e << endl;
				break; // on exception we can leave the loop
			}
			// if starting point == next point then we have found a circular curve
			//  break the loop and start again
			if ( rand[0] == next) break;
		}
		raender.append(rand);
	}
	return raender;
}

/** name: path_finder
 *
 * Same as LineMap::path_finder() but converts Curve into PointCurve.
 * Returns a list of PointCurve (myList<PointCurve>)
 *
 * @param p: List of Points
 * @return myList<PointCurve> = List of PointCurves
 */
myList<PointCurve> LineMap::path_finder(const vector<Point> &p){
	myList<Curve> raender = LineMap::path_finder();

	// convert Curve into PointCurve
	myList<PointCurve> point_curve;
	for (size_t i = 0; i < raender.length(); i++){
		PointCurve PC(raender[i], p);
		PC.compute_arc_length();
		point_curve.append(PC);

	}

	return point_curve;
}

/** name: path_finder
 *
 * Same as LineMap::path_finder() but converts Curve into PointCurve.
 * Returns a list of PointCurve (myList<PointCurve>)
 *
 * @param p: List of Points
 * @return myList<PointCurve> = List of PointCurves
 */
myList<PointCurve> LineMap::path_finder(const deque<Point> &p) {
	myList<Curve> raender = LineMap::path_finder();

	// convert Curve into PointCurve
	myList<PointCurve> point_curve;
	for (size_t i = 0; i < raender.length(); i++){
		PointCurve PC(raender[i], p);
		PC.compute_arc_length();
		point_curve.append(PC);
	}

	return point_curve;
}


#undef mini
#undef maxi
#undef print
