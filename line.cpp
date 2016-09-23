//      line.cpp
//
//      Copyright 2010 Stefan Fruhner <stefan@kandinsky>
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


#include "line.hpp"
namespace mylibs {
Line::Line(const Point &a, const Point &b, const int att) :
	pa(a), pb(b),
	ma(0), mb(0),
	linetype(Line::line),
	a(att) {

	if ( pb-pa == Point(0.,0.,0.) ) throw Line::Exception_InputError(EXCEPTION_ID);
}

Line::Line(const Point &a,const Point &b, const type t, const int att) :
	pa(a), pb(b),
	ma(0), mb(0),
	linetype(t),
	a(att) {

	if ( pb-pa == Point(0.,0.,0.) ) throw Line::Exception_InputError(EXCEPTION_ID);
}

void Line::LineType(const Line::type t){
	this->linetype = t;
}

Line::type& Line::LineType(){
	return this->linetype;
}

void Line::attribute(const int a){
	this->a = a;
}

int& Line::attribute(){
	return this->a;
}

/** \fn void Line::swap()
 * \brief Swaps the two points \f$ \vec{pa} \f$ and \f$ \vec{pb} \f$.
 */
void Line::swap(){ //swap items
	Point t = pb;		// swap pointers
	pb = pa;
	pa = t;
}

bool Line::operator==(const Line &other) const {
	return ((other.pa == pa) and (other.pb == pb));
}

/** name: Line::intersection
  * Computes the intersection of two lines dependent of their type in 3
  * dimensions.
  *
  * - line - line
  * - line - ray
  * - line -segment and so on.
  *
  * @param other		: 	the other line
  * @param intersection : 	The Point, where the intersection coordinates shall be saved
  * @param small_value  : 	If the lines intersect in 2d we test if the z-value
  * 						of the intersection point is equal. For numerical
  * 						reasons we compare if they are similar using a small
  * 						number. If the difference is smaller than this
  * 						number the intersection will be treated as found.
  * @return True or false, whether a intersection was found or not.
 **/
bool Line::intersection(const Line &other, Point &intersection,const double small_value){
	/** first we compute the parameters m_a and m_b for both lines at
	 *  the intersection in the xy-plane **/
	double denom =   (other.pb.y - other.pa.y) * (this->pb.x - this->pa.x)
				   - (other.pb.x - other.pa.x) * (this->pb.y - this->pa.y);

	/**
	 * If the denominator is 0 than both lines are parallel in the xy-plane.
	 **/
	if (denom == 0.) return false;

	this->ma  =   (other.pb.x - other.pa.x) * (this->pa.y - other.pa.y)
				- (other.pb.y - other.pa.y) * (this->pa.x - other.pa.x);
	this->ma /= denom;


	this->mb  =  (this->pb.x - this->pa.x) * (this->pa.y - other.pa.y)
				- (this->pb.y - this->pa.y) * (this->pa.x - other.pa.x);
	this->mb /= denom;

	/**	Now we need to check if the lines may be skew, which means
	 *	they intersect in the xy-projection, but their distance in z
	 *	remains constant.
	 **/
	 //// compute zvalues of possible intersection
	double za = this->pa.z + this->ma * (this->pb.z - this->pa.z);
	double zb = other.pa.z + this->mb * (other.pb.z - other.pa.z);

	double diff = (za > zb) ? za - zb : zb - za;
//	printf( "%.15f %.15f\n", diff , small_value);
	if (  diff > small_value ){ // lines must be skew, no intersection
//		printf( "%.15f %.15f %.15f %.15f %.15f\n", za , zb, diff, small_value, zb + small_value );
//		cout << pa.z << " "<<other.pa.z<< " "<<  za << " " << zb << endl;
		return false;
	}

	//Depending on the line type it may be an intersection or not
	switch (this->linetype){
		case Line::line: // easiest case ma doesn't matter
			break;
		case Line::ray: // ma must be positive for intersection
			if (ma < 0.) return false;
			break;
		case Line::segment: // ma must be in [0.,1.]
			if (ma < 0. or ma > 1.) return false;
			break;
		default: throw myexception_UnknownError();
	}

	// check parameter for second line
	switch (other.linetype){
	 case Line::line: // nothing to to,intersection is computed below
						break;
	 case Line::ray:  // for intersection mb must be >= 0.
						if (mb < 0.) return false;
						break;
	 case Line::segment: // mb must be in [0., 1.]
						if (mb < 0. or mb > 1.) return false;
						break;
	}

	// found an intersection, compute its coordinates
	intersection.x = this->pa.x + ma * (this->pb.x - this->pa.x);
	intersection.y = this->pa.y + ma * (this->pb.y - this->pa.y);
	intersection.z = za; // already computed

	return true;
}

/** name: Line::intersection_parameter_ma
  * Return the intersection paramter ma of this line.
  * \attention Of course this value is only valid, if there was a intersection
  * computation before this function is called AND if the return value was
  * 'true'.
  * @return value of ma
  **/
double Line::intersection_parameter_ma() const {
	return this->ma;
}

/** name: Line::intersection_parameter_mb
  * Return the intersection paramter mb of the other line.
  * \attention Of course this value is only valid, if there was a intersection
  * computation before this function is called AND if the return value was
  * 'true'.
  * @return value of mb
  **/
double Line::intersection_parameter_mb() const {
	return this->mb;
}

/** \fn double Line::length() const
 * \brief Get the length of the line.
 *
 *	This function returns the length of the line, if it exist. This is the case
 *	for the following types:
 *	- Line::segment
 *
 * \return The length of the segment for a Line::segment.
 */
double Line::length() const {
	switch (linetype){
		case Line::segment: return (pb-pa).abs(); break;
		default : throw Line::Exception_MathError(EXCEPTION_ID
						+"Length of an infinitely extended line is not "
						+"defined. Length() is only defined for Line::segment. "
					);
	}

	return 0.;
}

/** \fn Point Line::point_at(const double) const
 * \brief Get the point with line parameter m.
 *
 *	This function returns the point of the line
 *	\f$ \vec l = \vec{pa} + n (\vec{pb} - \vec{pa} ) \f$
 *	for whom is \f$ n = m \f$.
 *
 * \param m: Double giving relative position along the line (in terms of the
 *			 tangent vector).
 * \return Point with position m.
 */
Point Line::point_at(const double m) const{
	return pa + m * (pb - pa);
}

/** \fn Point Line::point_in_dist(const double) const
 * \brief Compute the point on the line, that has a certain distance to
 *  Point pa.
 *
 *	This function returns the point of the line
 *	\f$ \vec l = \vec{pa} + distance * |(\vec{pb} - \vec{pa} )| \f$
 *
 * \param distance: Distance of the desired point.
 * \return Resulting Point.
 */
Point Line::point_in_dist(const double distance) const{
	Point v = (pb - pa);
	v.normalize();
	return (pa + distance * v);
}
/** name Line::tangent()
 * Compute the tangential vector of a line.
 * \return normalized tangential vector
 */
Point Line::tangent() const {
	Point t = pb - pa;
	t.normalize();
	return t;
}

/** name Line::normal
 * Compute the normal vector to a line. If the direction of the line is given
 * by \f$ \vec v = (a,b) \f$ then the vectors \f$ \vec n = (-b,a) \f$ and
 * \f$ \vec n = (b,-a) \f$ point in the normal direction -- either at the left
 * hand side or the right hand side.
 * \return normal vector
 */
Point Line::normal() const {
	Point n;
	Point vec = pb - pa;

	n.x = -vec.y; // left hand side normal
	n.y =  vec.x;
	n.z =  vec.z;
	n.normalize();
	return n;
}

/** name Line::normal_lhs
 * Left hand side normal \f$ \vec n = (-b,a) \f$ - alias for normal()
 * \return normal vector
 */
Point Line::normal_lhs() const {
	return normal();
}

/** name Line::normal_rhs
 * Right hand side normal \f$ \vec n = (b,-a) \f$
 * \return normal vector
 */
Point Line::normal_rhs() const {
	Point n;
	Point vec = pb - pa;

	n.x =   vec.y; // right hand side normal
	n.y = - vec.x;
	n.z =   vec.z;
	n.normalize();
	return n;
}

Point Line::binormal() const {
	Point b = tangent().cross(normal());
	return b;
}

/** name Line::frenet_frame
 * Compute tangent, normal and binormal of a line, which determine the frenet
 * frame.
 * @param t: reference to tangent
 * @param n: reference to normal
 * @param b: reference to binormal vector
 *
 */
void Line::frenet_frame(Point &t,Point &n, Point &b) const {
	t = tangent();
	n = normal();
	b = t.cross(n);
}

/** name: Line::is_on_line
 * Checks if a point lies on a line. The result depends on the line type.
 * For infintely extended lines, the result is true, if the point is on that
 * line. For line segments and rays the line parameter m has to be in an
 * appropriate range.
 * @param pt: The point to be examined.
 * @param m : A double valued variable which stores the line parameter.
 * @param small_value: A double value which acts as a cut-off-parameter: if the
 *				distance is smaller than this, the Point pt is considered to be
 *				on the line.<br>
 *				\todo Implement small_value comparison for m.
 * @return True or false depending on the Line::linetype.
 **/
bool Line::is_on_line(const Point &pt, double &m, const double small_value __attribute__ ((unused)) ) const {
	Point v = pb - pa;

//	double m = 0.;

	if (v.x != 0.) m = (pt.x - pa.x)/ v.x;
	else
	if (v.y != 0.) m = (pt.y - pa.y)/ v.y;
	else
	if (v.z != 0.) m = (pt.z - pa.z)/ v.z;
	else throw Line::Exception_InputError("Line is invalid");

	Point test = (pa + v*m);

	//fprintf(stderr, "%.15f %.15f %.15f\n", test.x, test.y, test.z);
	//fprintf(stderr, "%.15f %.15f %.15f\n", pt.x, pt.y, pt.z);
	//cout << "-" << endl;

	// \todo Implement small_value comparison for m */

	switch (linetype){
	case Line::line	 :	return (pt == test); break;
	case Line::ray	 :	if (m >= 0.) return (pt == test); break;
	case Line::segment:	if (m >= 0. && m < 1.) return (pt == test);	break;
	}
	return false;
}

/** name: Line::perpendicular_foot_of
 * Computes the perpendicular foot for a given point. First the problem is
 * solved in the xy-plane, since the normal vector to a line is not clearly
 * defined in three dimensions. The z-coordinate is computed in the end from
 * the line equation.
 * \f$ \vec p = \vec p_a + m (\vec p_b - \vec p_a) \f$
 * @param 	pt   : The point for which we want to now the perpendicular foot.
 * @param 	foot : The point where we want to write the result to.
 * @return 	True, if a perpendicular foot exists for this type of line -- false,
 * 			otherwise.
 **/
bool Line::perpendicular_foot_of(const Point& pt, Point& foot) {
	// first we solve the problem in the xy-plane
	Point tp(pt); tp.z = 0;
	Point ta(pa); ta.z = 0.;
	Point tb(pb); tb.z = 0.;
	Line tl(ta, tb, this->LineType());
	Point vec = pb - pa;
	// compute the normal vector in the xy-plane
	// in the same step we construct the difference vector of pt and n
	Point n = Point();
	n.x = pt.x - vec.y;
	n.y = pt.y + vec.x;
	n.z = 0.;
//	n.normalize(); // Not needed, since we construct a line.

	// now build the perpendicular line connecting the segment and the point
	Line l(tp,n,Line::line);

	if (tl.intersection(l, foot)){
		foot.z = pa.z + (foot.x - pa.x) * vec.z/vec.x; // compute z-value of intersection
		return true;
	}
	return false;
}

/** \fn void Line::prnt(const char * const) const
 *  \brief Alias for Line::info()
 */
void Line::prnt(const char * const name) const {
	this->info(name);
}

/** \fn void Line::info(const char * const) const
 *	Prints the Line in the form
 *	\f$ name = \vec{pa} + m * (\vec{pb} - \vec{pa}) \f$ on stdout.
 *
 * \param name: A c-string, which contains the name / an identifier for the Line,
 * 				that should be printed.
 */
void Line::info(const char * const name) const {
	size_t l = strlen(name);
	string empty;
	for (size_t i = 0; i < l; i++) empty+= " ";
	printf("%s    (%+.6E)       (%+.6E) \n", empty.c_str() ,pa.x,pb.x - pa.x );
	printf("%s =  |%+.6E| + m * |%+.6E| \n", name		   ,pa.y,pb.y - pa.y );
	printf("%s    (%+.6E)       (%+.6E) \n", empty.c_str(), pa.z,pb.z - pa.z );
}

} // end of namespace mylibs
