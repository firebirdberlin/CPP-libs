//		./point.cpp
//
//		Copyright 2011,2012 Stefan Fruhner <stefan.fruhner@gmail.com>
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
#include "point.hpp"

Point::Point()
	: nr_attributes(0){// standard constructor
	this->x		= 0;
	this->y		= 0;
	this->z		= 0;
	this->t		= 0;
	this->bnd	= 0;
}

Point::Point(double x, double y, double z, double t, int attribute)
	: nr_attributes(0) {
	if (x!=x) 				  throw Point::Exception_NaN(EXCEPTION_ID + " (x) " );
	if (y!=y) 				  throw Point::Exception_NaN(EXCEPTION_ID + " (y) " );
	if (z!=z) 				  throw Point::Exception_NaN(EXCEPTION_ID + " (z) " );
	if (attribute!=attribute) throw Point::Exception_NaN(EXCEPTION_ID + " (attribute) " );

	this->x = x;
	this->y = y;
	this->z = z;
	this->t = t;
	this->bnd = 0;

	add_attribute(attribute);
}

/** \fn Point::Point(const vector<double> &)
 * (Stefan Fruhner - 26.01.2012 15:27:43 CET)
 * Create a Point from a vector of doubles. Each value is assigned to x,y,z
 * and t according to its index.
 **/
Point::Point(const vector<double> &pt)
	: nr_attributes(0) {
	this->set(pt);
	this->bnd = 0;
}

Point::Point(const Point &copy)
	: nr_attributes(0) {
	if (copy.x!=copy.x)		 throw Point::Exception_NaN(EXCEPTION_ID + " (x) " );
	if (copy.y!=copy.y)		 throw Point::Exception_NaN(EXCEPTION_ID + " (y) " );
	if (copy.z!=copy.z)		 throw Point::Exception_NaN(EXCEPTION_ID + " (z) " );
	if (copy.t!=copy.t)		 throw Point::Exception_NaN(EXCEPTION_ID + " (t) " );
	if (copy.bnd!=copy.bnd)	 throw Point::Exception_NaN(EXCEPTION_ID + " (t) " );

	this->x = copy.x;
	this->y = copy.y;
	this->z = copy.z;
	this->t = copy.t;
	this->bnd = copy.bnd;

	nr_attributes = copy.attributes();
	a.clear();
//	this->a = (int*) realloc(this->a, (nr_attributes) * sizeof(int));

	for(size_t i = 0; i < nr_attributes; ++i) {
//		this->a[i] = copy.attribute_at(i);
		a.push_back(copy.attribute_at(i));
	}
}

Point::Point(const PointSpherical   &p, int attribute)
	: t(p.t), nr_attributes(0), bnd(0) {
	x = p.r * sin(p.theta) * cos(p.phi);
	y = p.r * sin(p.theta) * sin(p.phi);
	z = p.r * cos(p.theta);
	add_attribute(attribute);
}

Point::Point(const PointCylindrical &p, int attribute)
	: t(p.t), nr_attributes(0), bnd(0) {
	x = p.rho * cos(p.phi);
	y = p.rho * sin(p.phi);
	z = p.z;
	add_attribute(attribute);
}

/** Point::Point(const PointPolar &p, int attribute)
 * Creates a point from a point given in polar coordinates
 * \param p : point in polar coordinates
 * \param attribute : attribute value
 **/
Point::Point(const PointPolar &p, int attribute) :
	t(p.t), nr_attributes(0), bnd(0){
	x = p.r * cos(p.phi);
	y = p.r * sin(p.phi);
	z = 0.;
	add_attribute(attribute);
}

Point::~Point(){
	//! \todo segfaults after Neighboursearch was used
//	free(a); a = NULL;
	a.clear();
	nr_attributes = 0;
}

void Point::set(double x, double y, double z, double t, int attribute){
	if (x!=x) throw Point::Exception_NaN(EXCEPTION_ID + " (x) " );
	if (y!=y) throw Point::Exception_NaN(EXCEPTION_ID + " (y) " );
	if (z!=z) throw Point::Exception_NaN(EXCEPTION_ID + " (z) " );
	if (t!=t) throw Point::Exception_NaN(EXCEPTION_ID + " (t) " );
	if (attribute!=attribute) throw Point::Exception_NaN(EXCEPTION_ID + " (attribute) " );

	this->x = x;
	this->y = y;
	this->z = z;
	this->t = t;
	this->bnd = 0;

	add_attribute(attribute);
}

void Point::set(const vector<double> &pt){
	if ( pt.size() > 4 ) {
		mylibs::cmdline::info(pt);
		throw Point::Exception_Input(EXCEPTION_ID
				+ " Got too much values, don't know how to handle.");
	}

	this->x = ( pt.size() > 0 ) ? pt[0] : 0. ;
	this->y = ( pt.size() > 1 ) ? pt[1] : 0. ;
	this->z = ( pt.size() > 2 ) ? pt[2] : 0. ;
	this->t = ( pt.size() > 3 ) ? pt[3] : 0. ;

	if (x!=x) throw Point::Exception_NaN(EXCEPTION_ID + " (x) " );
	if (y!=y) throw Point::Exception_NaN(EXCEPTION_ID + " (y) " );
	if (z!=z) throw Point::Exception_NaN(EXCEPTION_ID + " (z) " );
	if (t!=t) throw Point::Exception_NaN(EXCEPTION_ID + " (t) " );
}

void Point::setInfinity(){
	this->x = std::numeric_limits<double>::max();
	this->y = std::numeric_limits<double>::max();
	this->z = std::numeric_limits<double>::max();

}

void Point::setInfinityMin(){
	this->x = -std::numeric_limits<double>::max();
	this->y = -std::numeric_limits<double>::max();
	this->z = -std::numeric_limits<double>::max();

}

int Point::attribute() const {
	if (nr_attributes > 0) return this->a[0];
	return 0; // for compatibility
}


int Point::attribute_at(size_t idx) const{
	if (idx < nr_attributes) return this->a[idx];
	throw PointException_IndexError(EXCEPTION_ID + "Attribute index out of range");
}

void Point::attribute(const int value){
	add_attribute(value);
}

size_t Point::attributes() const{
	return nr_attributes;
}

void Point::add_attribute(const int attribute){
//	this->a = (int*) realloc(this->a, (nr_attributes+1) * sizeof(int));
//	this->a[nr_attributes++] = attribute;
	this->a.push_back(attribute);
	nr_attributes++;
}

void Point::clear_attributes(){
	nr_attributes = 0;
//	free(a); a = NULL;
	a.clear();
}

/** name: Point::cross()
 * Computes the cross product of two vectors.
 * @param other: Other vector to compute cross product with.
 * @return Point
 **/
Point Point::cross(const Point &other) const {
	Point res;
	res.x = y * other.z - z * other.y;
	res.y = z * other.x - x * other.z;
	res.z = x * other.y - y * other.x;

	return res;
}

/** name: Point::dot()
 * Computes the dot product of two vectors.
 * @param other: Other vector to compute dot product with.
 * @return Double
 **/
double Point::dot(const Point &other) const {
	return x * other.x + y*other.y + z * other.z;
}

/** name: Point::abs()
 * Absolute value of a vector.
 * @return value
 **/
double Point::abs() const {
	double v = sqrt(x*x + y*y + z*z);
	if (v != v) throw Point::Exception_NaN(EXCEPTION_ID);
	return v;
}

/** name: Point::norm
 * Squared absolute value of a vector.
 * @return Value
 **/
double Point::norm() const {
	double v = x*x + y*y + z*z;
	if (v != v) throw Point::Exception_NaN(EXCEPTION_ID);
	return v;
}

 /** name: Point::normalize()
  * Normalizes a vector using abs():
  **/
void Point::normalize(){
	double absolute = abs();
	if (absolute == 0.) throw Point::Exception_ZeroLength(EXCEPTION_ID);

	*this /= absolute;
}

/** name: Point::prnt()
  * Prints out information of this Point to stdout.
  * @param attributes: True if attributes shall be printed.
  * @param boundaries : True if boundaries shall be printed.
  **/
void Point::prnt(bool attributes, bool boundaries) const{
//			cout << resetiosflags(ios::fixed | ios::scientific);
//			cout << setprecision(7) << right << showpos <<  " [ "<< x << " , " << y << " , " << z << " ] ";
//			cout << " [ "<< x << " , " << y << " , " << z << " ] ";
	printf("[ %+6.6f, %+6.6f, %+6.6f ] ",x,y,z);
	if ( attributes ){
		for (size_t i = 0; i < nr_attributes; i++){
			cout << a[i] << " ";
		}
	}
	if ( boundaries) cout << bnd;
	cout << endl;
}

Point Point::operator-(const Point& other) const{
	Point diff = Point(*this);
	diff.x -= other.x;
	diff.y -= other.y;
	diff.z -= other.z;
	return diff;
}

/** name: Point::operator-
 * \brief Spatial negative-operator.
 *
 *	This is the spatial negative operator. It changes sign of the spatial
 *	components.
 * \return Point, where the signs of the components are opposite to those of
 *				this one.
 */
Point Point::operator-() const{
	Point neg = Point(*this);
	neg.x = -neg.x;
	neg.y = -neg.y;
	neg.z = -neg.z;
	return neg;
}

Point Point::operator+(const Point &other) const{
	Point sum = Point(*this);
	sum.x += other.x;
	sum.y += other.y;
	sum.z += other.z;
	return sum;
}

Point Point::operator+() const{
	Point pos = Point(*this);
	//pos.x = +pos.x;// no need to do this, so just for remembering what this method is for, as comment.
	//pos.y = +pos.y;
	//pos.z = +pos.z;
	return pos;
}


Point Point::operator*(const double val) const{
	Point res = Point(*this);
	res.x *= val;
	res.y *= val;
	res.z *= val;
	return res;
}

Point Point::operator/(const double val) const{
	Point res= Point(*this);
	res.x /= val;
	res.y /= val;
	res.z /= val;
	return res;
}

void Point::operator+=(const Point &other){
	x += other.x;
	y += other.y;
	z += other.z;
}

void Point::operator-=(const Point &other){
	x -= other.x;
	y -= other.y;
	z -= other.z;
}

void Point::operator*=(const double val){
	x *= val; y *= val; z *= val;
}

void Point::operator/=(const double val){
	x /= val; y /= val;	z /= val;
}

bool Point::operator<(const Point &other) const {
	if ( x < other.x) return true;
	if ((x == other.x) && (y < other.y)) return true;
	if ((x == other.x) && (y == other.y) && (z < other.z)) return true;
	return false;
}

bool Point::operator>(const Point &other) const {
	if ( x > other.x) return true;
	if (x == other.x){
		if (y > other.y) return true;
		if ((y == other.y) && (z > other.z)) return true;
	}
	return false;
}

bool Point::operator>=(const Point &other) const {
	return (operator>(other) or operator==(other));
}


bool Point::operator<=(const Point &other) const {
	return (operator<(other) or operator==(other));
}

/** Point::operator[]
 *  [] operator to ensure, that the coordinates can be used like
 *  they where part of an array.
 * \param item: Int 0..4 for x,y,z,t
 * \return Reference to the coordinate
 */
double & Point::operator[](const int item){
	switch (item){
	case 0:	return this->x;	break;
	case 1:	return this->y;	break;
	case 2:	return this->z;	break;
	case 3:	return this->t;	break;
	default: throw Point::Exception_IndexError(EXCEPTION_ID);
	}
}

Point & Point::operator=(const Point & other) {
	x = other.x;
	y = other.y;
	z = other.z;
	t = other.t;

	nr_attributes = other.attributes();
	a.clear();
//	this->a = (int*) realloc(this->a, (nr_attributes) * sizeof(int));

	for(size_t i = 0; i < nr_attributes; ++i) {
//		this->a[i] = other.attribute_at(i);
		a.push_back(other.attribute_at(i));
	}

	return (*this);
}

/** Point::spherical_coordinates()
 *	Computes the polar coordinates describing the vector. The angles are
 *  computed in radians.
 * \param r: References to the variables that stores r
 * \param phi 	: azimuth
 * \param theta : inclination
 **/
void Point::spherical_coordinates(double &r, double &phi, double &theta) const{
	r		= abs();
	phi		= atan2(y, x);
	theta	= acos(z / r);
	return;
}

/** Point::spherical_coordinates_degrees()
 *	Computes the polar coordinates describing the vector. The angles are
 *  computed in arc degrees.
 * \param r: References to the variables that stores r
 * \param phi 	: azimuth
 * \param theta : inclination
 **/
void Point::spherical_coordinates_degrees(double &r, double &phi, double &theta) const{
	r		= abs();
	phi		= atan2(y, x) * 180./M_PIl;
	theta	= acos(z / r) * 180./M_PIl;
	return;
}

/** Point::rotate_x()
 * Applies the rotation matrix with respect to the x-axis.
 \f[ 	\begin{pmatrix}
			1 & 0 			& 0 			\\
			0 & cos \alpha 	& -sin \alpha 	\\
			0 & sin \alpha 	&  cos \alpha
		\end{pmatrix}
 \f]
 * \param alpha: rotation angle in radians
 **/
void Point::rotate_x(double alpha){
//	x = x;
	y =  cos(alpha) * y - sin(alpha) * z;
	z =  sin(alpha) * y + cos(alpha) * z;
	return;
}

/** Point::rotate_y()
 * Applies the rotation matrix with respect to the y-axis.
 \f[ 	\begin{pmatrix}
			cos \alpha 	& 0 & sin \alpha  \\
			0 			& 1 & 0 		  \\
			-sin \alpha & 0	&  cos \alpha
		\end{pmatrix}
 \f]
 * \param alpha: rotation angle in radians
 **/
void Point::rotate_y(double alpha){
	x =  cos(alpha) * x + sin(alpha) * z;
//	y = y;
	z = -sin(alpha) * x + cos(alpha) * z;
	return;
}

/** Point::rotate_z()
 * Applies the rotation matrix with respect to the z-axis.
 \f[ 	\begin{pmatrix}
			cos \alpha & -sin \alpha & 0  \\
			sin \alpha & cos \alpha	 & 0  \\
			0 		   & 0           & 1
		\end{pmatrix}
 \f]
 * \param alpha: rotation angle in radians
 **/
void Point::rotate_z(double alpha){
	x =  cos(alpha) * x - sin(alpha) * y;
	y =  sin(alpha) * x + cos(alpha) * y;
//	z = z;
	return;
}

/** Point::rotate()
 * Rotate the vector using the angles phi and theta, which must be given in
 * radians.
 * \param phi: inclination
 * \param theta: azimuth
 **/
void Point::rotate(double phi, double theta){
	rotate_z(phi);
	rotate_x(theta);
}

Point operator*(const double val, const Point &p){
	return p * val;
}

double distance(const Point &p1, const Point &p2){
	return(Point(p1)-Point(p2)).abs();
}


PointSpherical::PointSpherical(const double v_r, const double v_phi, const double v_theta, const double v_t) :
	r(v_r), phi(v_phi), theta(v_theta), t(v_t) {
}

PointSpherical::PointSpherical(const Point &p){
	r		= p.abs();
	phi		= atan2(p.y, p.x);
	theta	= acos(p.z / r);
	t 		= p.t;
}

PointSpherical::PointSpherical(){
	r = phi = theta = t = 0.;
}


PointCylindrical::PointCylindrical(const double v_r, const double v_phi, const double v_z, const double v_t) :
	rho(v_r), phi(v_phi), z(v_z), t(v_t) {
}

PointCylindrical::PointCylindrical(const Point &p){
	rho	= sqrt(p.x * p.x + p.y * p.y);
	phi	= atan2(p.y, p.x);
	z	= p.z;
	t 	= p.t;
}

PointCylindrical::PointCylindrical(){
	rho = phi = z = t = 0.;
}

/** name: PointPolar::PointPolar(const Point &p)
 * Creates a point polar coordinates.
 * \param v_r : length of the polar vector
 * \param v_phi : angle in rad
 * \param v_t : time value
 *
 **/
PointPolar::PointPolar(const double v_r, const double v_phi, const double v_t) :
	r(v_r), phi(v_phi), t(v_t) {

}

PointPolar::PointPolar(const Point &p){
	r	= sqrt(p.x * p.x + p.y * p.y);
	phi	= atan2(p.y, p.x);
	t 	= p.t;
}

PointPolar::PointPolar(){
	r = phi = t = 0.;
}


ostream &operator<<( ostream &out,const Point &p){
	out
//		<< '['
//		<< ::std::setprecision(12)
//		<< ::std::scientific
//		<< scientific << setprecision(15) << setw(25)
		<< p.x << " "
//		<< scientific << setprecision(15) << setw(25)
		<< p.y << " "
//		<< scientific << setprecision(15) << setw(25)
		<< p.z << " ";
//		<< ']'
//		<< ::std::fixed;
	return out;
}

ostream &operator<<( ostream &out, const PointSpherical &p){
	out
		<< scientific << setprecision(15) << setw(25)
		<< p.r     << " "
		<< scientific << setprecision(15) << setw(25)
		<< p.phi   << " "
		<< scientific << setprecision(15) << setw(25)
		<< p.theta << " ";
	return out;
}

ostream &operator<<( ostream &out, const PointCylindrical &p){
	out
		<< scientific << setprecision(15) << setw(25)
		<< p.rho << " "
		<< scientific << setprecision(15) << setw(25)
		<< p.phi << " "
		<< scientific << setprecision(15) << setw(25)
		<< p.z   << " ";
	return out;
}

ostream &operator<<( ostream &out, const PointPolar &p){
	out
//		<< "[ "
//		<< ::std::setprecision(12)
//		<< ::std::scientific
		<< p.r   << " "
		<< p.phi << " ";
//		<< ']'
//		<< ::std::fixed;

	return out;
}
