//		./mymesh.cpp
//
//		Copyright 2011 Stefan Fruhner <stefan.fruhner@gmail.com>
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

#include "mymesh.hpp"

using namespace std;
using namespace mylibs;
#ifdef DEBUG
	#define print(x) (cout << __FILE__ << ":" << __FUNCTION__ <<"() (" <<__LINE__ << ") : " << x << endl)
#else
	#define print(x) (cout << x << endl)
#endif

//-------------------------- F u n c t i o n s ---------------------------------
/**  Det3
 *   Determinant of a 3x3 matrix given by 3 vectors
 *   \f$ \left|\vec a \, \vec b \, \vec c \right| \f$
 *
 * \param a,b,c: vectors
 * \return double value
 **/
double Det3(const Point &a, const Point &b,const Point &c) {
    return (a.x*b.y*c.z + b.x*c.y*a.z + c.x*a.y*b.z
          - c.x*b.y*a.z - b.x*a.y*c.z - a.x*c.y*b.z);
}

/**  TetrahedronVolume
 * The Volume of a tetrahedron is computed using the determinant of the span
 *  \f$ \{ \vec b- \vec a, \vec c- \vec a, \vec d- \vec a \} \f$
 * \param a,b,c,d: points defining a tetrahedron
 * \return Volume as double value
 **/
double TetrahedronVolume(Point &a, Point &b, Point &c, Point &d){
	Point x = a - b;
	Point y = a - c;
	Point z = a - d;
	return Det3(x,y,z)/6.;
}

/**  TriangleArea
 *
 * The Volume of a triangle in the xy-plane is computed
 * using the determinant of the span (1 x y)
 *  F = 1/2  | 1 x1 y1 |
 *           | 1 x2 y2 |
 *           | 1 x3 y3 |
 * where
 * \param a,b,c: points defining a triangle
 * \return Area as double value
 **/
double TriangleArea(const Point &a,const Point &b,const Point &c){
	Point x(1.,1.,1.);
	Point y(a.x,b.x,c.x);
	Point z(a.y,b.y,c.y);
	return 0.5 * Det3(x,y,z);
}


//---------------------------- C l a s s e s -----------------------------------
SurfaceMesh::Deformation::Deformation(vector<mystring> meshes)
	: source(0), target(0) {
	deform_bwd.clear();
	deform_fwd.clear();

	for (size_t i = 0; i < meshes.size(); ++i){
		mystring sb = meshes[i].file_strip_ext() + "_bdef.1.node";
		mystring sf = meshes[i].file_strip_ext() + "_fdef.1.node";
		if (not sb.file_exists() or not sf.file_exists())
			throw myexception("ATTENTION : Could not load deformation files");

		SurfaceMesh *mf = new SurfaceMesh(sf);
		SurfaceMesh *mb = new SurfaceMesh(sb);
		deform_fwd.push_back(mf);
		deform_bwd.push_back(mb);
	}
	cout << EXCEPTION_ID << "... initialization ready" << endl;
}

SurfaceMesh::Deformation::~Deformation(){
	for (size_t i = deform_bwd.size(); i > 0; --i)
		delete deform_bwd[i-1];

	for (size_t i = deform_fwd.size(); i > 0; --i)
		delete deform_fwd[i-1];

	deform_bwd.clear();
	deform_fwd.clear();
}

void SurfaceMesh::pclear(){
	delete pSearch; pSearch = NULL;
	p.clear();				// pts
}

/** name: SurfaceMesh::clear
 * Clean up
 */
void SurfaceMesh::clear(){
	pclear();				// pts

	f.clear();				// elements
	l.clear();				// line segments
	vtx.clear();			// vertex lists
	if (p_attributes){
		delete p_attributes;	// attributes
		p_attributes  = NULL;
	}
	nr_attributes = 0;

	pointmap.clear();
	borders.clear();
	regions.clear();
	for (size_t i = 0; i < this->tetlist.size(); i++)	tetlist[i].clear();
	this->tetlist.clear();
	this->nb.clear();
}

/** name: SurfaceMesh::determine_dimension()
 * (Stefan Fruhner - 10.11.2011 10:50:40 CET)
 *	Checks all points of the mesh an sets the private variable dim to an
 * 	appropriate value.
 * @return 	The dimension (0,1,2,3), where 0 means no points are stored in the
 * 			mesh.
 **/
size_t SurfaceMesh::determine_dimension(){
	this->dim = 0;
	if (points() > 0){
		double x = p[0].x;
		for (size_t i = 1; i < points(); i++){
			if ( p[i].x != x ) { this->dim = 1; break;}
		}

		double y = p[0].y;
		for (size_t i = 1; i < points(); i++){
			if ( p[i].y != y ) { this->dim = 2; break;}
		}

		double z = p[0].z;
		for (size_t i = 1; i < points(); i++){
			if ( p[i].z != z ) { this->dim = 3; break;}
		}
	}

	if (elements() > 0){

		switch ( f[0].v.size() ){
			case 3: elemtype = triangles; break;
			case 4: elemtype = tets; break;
			default: elemtype = hybrid;
		}


		for (size_t i = 1; i < elements(); ++i){
			switch ( f[i].v.size() ){
				case 3: if (elemtype != triangles) {
							elemtype = hybrid; goto elembreak;
						}
						break;
				case 4: if (elemtype != tets) {
							elemtype = hybrid; goto elembreak;
						}
						break;
				default: elemtype = hybrid; goto elembreak;
			}
		}

elembreak:
		if (elemtype == hybrid) cout << "Attention: hybrid mesh detected !" << endl;

	}
	return dim;
}

/** name: SurfaceMesh::save_off()
 * Saves the mesh in off format (geomview).
 * \param fn : name of the file to be created
 * \param region : Save only a file which shows the defined region (default -1 = all regions)
 * \return True or False, if writing was possible.
 */
bool SurfaceMesh::save_off(const char *fn, int region){

	scan_for_regions();
	map<int, unsigned char> region_map;
	for (unsigned char i = 0; i < regions.size(); i++)
		region_map[regions[i]] = i;

	// write an off file, that describes the mesh
	print(" - Writing *.off file : " << fn);
	FILE *ou = 0;

	ou = fopen(fn,"w");
	if (!ou)return false;

	int cnt = elements();
	if (region > -1){
		cnt = 0;
		for (size_t i = 0; i < elements(); i++){
			if (f[i].attribute == region) cnt++;
		}
	}

	fprintf(ou, "OFF\n");
	fprintf(ou, "%d %d %d\n", (int) points(), cnt, 0);

	for (size_t i = 0; i < points(); i++)
		fprintf(ou, "%f %f %f\n", p[i].x,p[i].y, p[i].z );

	for (size_t i = 0; i < elements(); i++){
		if ((region == -1) || (f[i].attribute == region)){
			mystring text("");
			for (size_t j = 0; j < f[i].v.size(); j++){
				text += toString(f[i].v[j]);
				if (j < f[i].v.size()-1) text += " ";
			}
			fprintf(ou, "%d %s %d\n",(int) f[i].v.size(), text.c_str(), (int) (50 + region_map[f[i].attribute]));
//			fprintf(ou, "%d %d %d %d %d\n", 3, f[i].a(), f[i].b(), f[i].c(), 50 + region_map[f[i].attribute]);
		}
//			fprintf(ou, "%d %d %d %d %d\n", 3, f[i].a, f[i].b, f[i].c, 50 + region_map[f[i].attribute]);
	}

	fclose(ou);
	return true;
}


/** name: SurfaceMesh::save_smesh()
 * Saves the mesh in smesh format (tetgen).
 * (c.f. http://tetgen.berlios.de/fformats.smesh.html )
 * \param fn : name of the file to be created
 * \param region : Save only a file which shows the defined region (default -1 = all regions)
 * \return True or False, if writing was possible.
 */
bool SurfaceMesh::save_smesh(const char *fn, int region){
	// write an smesh file, that describes the mesh
	print("  - Writing *.smesh file : " << fn);

	// determine the Nr of faces to be plotted
	int cnt = elements();
	if (region > -1){
		cnt = 0;
		for (size_t i = 0; i < elements() ; i++){
			if (f[i].attribute == region) cnt++;
		}
	}

	FILE *ou = 0;
	ou = fopen(fn,"w");
	if (!ou)return false;

//	First line: <# of points> <dimension (must be 3)> <# of attributes> <# of boundary markers (0 or 1)>
	fprintf(ou, "%lu 3 0 1\n", points());
	for (unsigned int i = 0; i < points(); i++)
		fprintf(ou, "%u %f %f %f %d\n", i, p[i].x, p[i].y, p[i].z, p[i].boundary() );

//	#  One line: <# of facets> <boundary markers (0 or 1)>
	fprintf(ou, "%d 1\n", cnt);
//	<# of corners> <corner 1> <corner 2> ... <corner #> [boundary marker]
	for (size_t i = 0; i < elements(); i++){
		if ((region == -1) || (f[i].attribute == region))
			fprintf(ou, "3 %d %d %d %d\n", f[i].a(), f[i].b(), f[i].c(), (int) f[i].attribute);
//			fprintf(ou, "3 %d %d %d %d\n", f[i].a, f[i].b, f[i].c, (int) f[i].attribute);
	}

//#  One line: <# of holes>
//# Following lines list # of holes:
//<hole #> <x> <y> <z>
	fprintf(ou, "0\n");

//	#  One line: <# of region>
//	fprintf(ou, "%d\n",(int) regions.size());
	fprintf(ou, "%d\n",(int) region_def.size());
//	# Following lines list # of region attributes:
//	<region #> <x> <y> <z><region number><region attribute>
	for (size_t i = 0; i < region_def.size(); i++){
		fprintf(ou, "%d %f %f %f %d %f\n", (int)i, region_def[i].pos.x,region_def[i].pos.y, region_def[i].pos.z, region_def[i].attribute, region_def[i].constraint);
	}
	fclose(ou);
	return true;
}

bool SurfaceMesh::save_smesh(mystring fn, int region){
	mystring name = fn.file_base() + ".smesh";
	return this->save_smesh(name.c_str(), region);
}

void SurfaceMesh::save_vect(const char*fn, vector<Point> &vect){

	if (fn == NULL) return;

	if ( vect.size() != p.size() ) throw myexception("# vects != # points !");

	ofstream f(fn);

	vector<Point>::iterator v = vect.begin();
	f << p.size() << endl;
	for (vector<Point>::iterator it = p.begin(); it != p.end(); ++it, ++v){
//		f << it << " " << v << endl;
		f << it->x << " " << it->y << " " << it->z << " ";
		f <<  v->x << " " <<  v->y << " " <<  v->z << endl;
	}



	f.close();

}

/**
 * name: SurfaceMesh::append_point
 * Tries to append a point to the list of points.
 * and returns the index of the inserted point,
 *
 * Therefore a red black tree is used, to check if a certain
 * point was already used. using this function only unique points
 * can be added.
 *
 * If insertion was successful or the index of
 * an identical point which is already in the list.
 *
 *
 * @param point	: The point to be appended
 * @param idx	: index of the point in the original data, that identifies one point
 * @return Index of the newly inserted point or it's identical point.
 */
int SurfaceMesh::append_point(Point &point, uint idx){
	delete pSearch; pSearch = NULL;
	//! variables for a red black tree
	pair<map<uint,int>::iterator,bool> res;
	res=pointmap.insert(pair<uint,int>(idx, this->points()) ); // element insertion

	if (res.second==true){ // element was successfully inserted
		/** \attention Because we need a the indices of all points we save all
		  * points in an array. The rbtree is only used to find identical points. */
		// add this point to the array
		p.push_back(point);
		nb.clear(); // clear neighbour lists
	}
	return res.first->second; // always return the index of the point
}

/**
 * name: SurfaceMesh::append_point
 * Appends a point to the list without doing any checks
 * @param point	: The point to be appended
 */
void SurfaceMesh::append_point(const Point &point){
	// add this point to the array
	delete pSearch; pSearch = NULL;
	p.push_back(point);
	nb.clear(); // clear neighbour lists
}

/**
 * name: SurfaceMesh::append_point
 * Appends a point to the list of points, but checks if the point already was
 * inserted. The return value is always the index of the point in the point
 * list. This index is important for constructing the elements in the mesh.
 * @param point	: The point to be appended
 */
size_t SurfaceMesh::append_point_uniquely(const Point &point){
	nb.clear(); // clear neighbour lists
	delete pSearch; pSearch = NULL;

	pair<map<Point,size_t>::iterator,bool> res;   // this pair is the result of an insert operation

	// now we call the insert function
	res=point_rbtree.insert (pair<Point,size_t>(point, points()) );

	if (res.second==true){ // element must be new
		p.push_back(point); // append it to the list of points
	}

	return res.first->second; // return the index of the element
}


/** name: SurfaceMesh::points_array()
 * Converts the points of the mesh to an array of Points. Internally a
 * vector<Point> is used, which may not be suitable for the users needs.
 * \attention The user is responsible for deleting the memory.
 * return Array of point.
 */
Point * SurfaceMesh::points2array() const{
	Point *pts = new Point[points()];

	// currently the list of points is a vector<Points>, we copy the values
	for (size_t i = 0; i < points(); i++){
		pts[i] = p[i];
	}
	return pts;
}

bool SurfaceMesh::append_face(const Face &face){
	f.push_back(face);
	nb.clear(); // clear neighbour lists
	return true;
}

bool SurfaceMesh::append_line(const myLine &line){
	l.push_back(line);
	return true;
}

bool SurfaceMesh::append_region(const Region reg){
	region_def.append(reg);
	return true;
}

void SurfaceMesh::set_region_at_vtxID(const int vertexID, const int regionID, const int only_region){
	if (tetlist.size() != p.size()) compute_tetlist();

	if (only_region > -1){
		for (size_t i = 0; i < tetlist[vertexID].size(); i++){
			int tID = tetlist[vertexID][i];
			if (f[tID].attribute == only_region)
				f[tID].attribute = regionID;
		}
	} else{
		for (size_t i = 0; i < tetlist[vertexID].size(); i++){
			int tID = tetlist[vertexID][i];
			f[tID].attribute = regionID;
		}
	}
}

void SurfaceMesh::exchange_face_attribute(const int from, const int to){
	for (size_t i = 0; i < elements(); i++){
		 if (f[i].attribute == from ) f[i].attribute = to;
	}

}

/** name: SurfaceMesh::bounding_box
 *	Determines the bounding box of the mesh. The minimum and maximum
 * 	coordinates are collected. Two points min and max define a box, that
 * 	encloses the mesh.
 *
 * @param min : Reference to the Point where the minimum coordinates should be
 * 				saved.
 * @param max : Reference to the Point where the maximum coordinates should be
 * 				saved.
 * @return void
 **/
void SurfaceMesh::bounding_box(Point &min, Point &max) const {
	if ( points() > 0 ){
		min = p[0];
		max = p[0];
		for (size_t i = 1; i < points(); i++){
			min.x = (min.x > p[i].x) ? p[i].x : min.x;
			min.y = (min.y > p[i].y) ? p[i].y : min.y;
			min.z = (min.z > p[i].z) ? p[i].z : min.z;

			max.x = (max.x < p[i].x) ? p[i].x : max.x;
			max.y = (max.y < p[i].y) ? p[i].y : max.y;
			max.z = (max.z < p[i].z) ? p[i].z : max.z;
		}
	}
	return;
}

mylibs::BoundingBox SurfaceMesh::bounding_box() const {
	if ( points() > 0 ){
		Point min, max;
		min = p[0];
		max = p[0];

		for (vector<Point>::const_iterator it = this->p.begin(); it != this->p.end(); ++it){
			min.x = (min.x > it->x) ? it->x : min.x;
			min.y = (min.y > it->y) ? it->y : min.y;
			min.z = (min.z > it->z) ? it->z : min.z;
			max.x = (max.x < it->x) ? it->x : max.x;
			max.y = (max.y < it->y) ? it->y : max.y;
			max.z = (max.z < it->z) ? it->z : max.z;
		}

//		for (size_t i = 1; i < points(); i++){
//			min.x = (min.x > p[i].x) ? p[i].x : min.x;
//			min.y = (min.y > p[i].y) ? p[i].y : min.y;
//			min.z = (min.z > p[i].z) ? p[i].z : min.z;

//			max.x = (max.x < p[i].x) ? p[i].x : max.x;
//			max.y = (max.y < p[i].y) ? p[i].y : max.y;
//			max.z = (max.z < p[i].z) ? p[i].z : max.z;
//		}
		return mylibs::BoundingBox(min, max);
	}
	return mylibs::BoundingBox(Point(0.,0.,0.), Point(0.,0.,0.));
}

/** name: SurfaceMesh::bounding_box
 * Determines the bounding box for a list of points in the mesh.
 * @param pts: List of point indices.
 * @return BoundingBox
 **/
mylibs::BoundingBox SurfaceMesh::bounding_box(const list<int> &pts) const {
	if ( points() > 0 and pts.size() > 0 ){
		Point min, max;
		min = p[pts.front()];
		max = p[pts.front()];
		for(list<int>::const_iterator it = pts.begin(); it != pts.end(); it++){
			min.x = (min.x > p[*it].x) ? p[*it].x : min.x;
			min.y = (min.y > p[*it].y) ? p[*it].y : min.y;
			min.z = (min.z > p[*it].z) ? p[*it].z : min.z;

			max.x = (max.x < p[*it].x) ? p[*it].x : max.x;
			max.y = (max.y < p[*it].y) ? p[*it].y : max.y;
			max.z = (max.z < p[*it].z) ? p[*it].z : max.z;
		}
		return mylibs::BoundingBox(min, max);
	}
	return mylibs::BoundingBox(Point(0.,0.,0.), Point(0.,0.,0.));
}


/** name: get_valid_line()
 *	Returns the next line from a file stream that is not a comment
 * (starting with '#').
 * @param f: inut file stream
 * @return The complete line as mystring.
 **/
mystring get_valid_line(fstream &f){
	mystring line("# comment");
	while ((!f.eof()) and ( line.is_comment('#'))) {
			getline(f, line);
		}
//	print(line);
	return line;
}

/** name: SurfaceMesh::read_node
 *
 * Read in *.node files as generated by triangle (and tetgen ? )
 *
 * The number of attributes can be set freely. Because attributes can have
 * different types, they are read in as double
 *
 * \param fn : filename of the nodefile
 * \return
 */
bool SurfaceMesh::read_node(const char *fn){
	if (debug) printf(" - Reading %s ", fn);
	try	{
		mystring line("# comment");
		fstream f(fn,fstream::in);
		if (f.fail() ) throw myexception("  File input error (" + string(fn) + ").");

		// clean up nodes
		pclear();
		delete p_attributes;
		p_attributes = 0;

		/** *.node description (from tetgen)
		 * \verbatim
		 * # First line: <# of points> <dimension (must be 3)> <# of attributes> <# of boundary markers (0 or 1)>
		 * # Remaining lines list # of points:
		 * <point #> <x> <y> <z> [attributes] [boundary marker]
		 * \endverbatim
		*/

		/** *.node description (from triangle)
		 * \verbatim
		 * # First line: <# of vertices> <dimension (must be 2)>  <# of attributes> <# of boundary markers (0 or 1)>
		 *  <vertex #> <x> <y> [attributes] [boundary marker]
		 * \endverbatim
		 */

		line = get_valid_line(f);
		myStringList mystrlist = line.split();
		int num_node=0, num_dim=0, num_att=0;
		int attribute = 0;
		bool has_bnd = false;
		num_node = mystrlist[0].ToInt();		/**< num_node: how many nodes							*/
		num_dim  = mystrlist[1].ToInt();		/**< num_dim : how many coords per node (must be 2 or 3)*/
		num_att  = mystrlist[2].ToInt();		/**< num_att : how many attributes 						*/
		has_bnd  = mystrlist[3] =="1";			/**< has_bnd : boundary markers [on/off] = [1/0] 		*/

		if (num_dim > 3) {
			cmdline::warning("Dimension > 3 is is not supported.");
		} else dim = num_dim; // save number of spatial dimensions

		if (num_att > 1){
			vector<double> tmp;
			p_attributes = new vector<Attributes>(num_node, tmp);

//			p_attributes.assign(num_node, tmp);
//				print(" (!) Warning : More than 1 attribute is not supported.");
			}


		int cnt = 0;
		int diff = 0;
		while (!f.eof() ){
			myStringList mylist =  get_valid_line(f).strip().split();

			int index = mylist.take_front().ToInt(); //! index of the node

			if (cnt == 0){
				diff = index - cnt;
				if (diff != 0)
					cmdline::warning("Be careful ... indices are not starting at 0");
			}
			else
			if (index - cnt != diff){
				cmdline::warning("Node indices are not in ascending order.");
			}

			int i = 0;
			double v[3] = {0., 0., 0.};

			int boundary_marker = 0;
			for (i = 0; i < num_dim ; i++)
				v[i] = mylist.take_front().ToDouble();

			Point pt = Point(v[0],v[1], v[2]);
			nr_attributes = num_att;

			// get the attribute(s)
			for (i = 0; i < num_att ; i++) {
				mystring att = mylist.take_front();
				if (i == 0) attribute = att.ToInt();
				p_attributes->at(cnt).push_back(att.ToDouble());
			}
			// get the boundary
			if ( has_bnd )  boundary_marker = mylist.take_front().ToInt();
			pt.boundary(boundary_marker);
			pt.attribute(attribute);
			append_point(pt); //!< append this point to the list of points

			cnt++;
			if (cnt == num_node) break;
		};
		f.close();
	}
	catch (myexception e) {
		cerr << e.what() << endl;
		return false;
	}
	if (debug) cmdline::ok();
	return true;
}

bool SurfaceMesh::read_ele (const char *fn){
	if (debug) printf(" - Reading %s ", fn);
	try	{
		mystring line("# comment");
		fstream f(fn,fstream::in);
		if (f.fail() ) throw myexception("  File input error (" + string(fn) + ").");

		// clean up faces
		f.clear();

		/** *.ele description (from tetgen)
		 * \verbatim
		 * #  First line: <# of tetrahedra> <nodes per tetrahedron> <# of attributes>
		 * # Remaining lines list of # of tetrahedra:
		 * 	<tetrahedron #> <node> <node> <node> <node> ... [attributes]
		 * \endverbatim
		*/

		/** *.ele description (from triangle)
		 *	\verbatim
		 * First line: <# of triangles> <nodes per triangle>  <# of attributes>
		 * Remaining lines: <triangle #> <node> <node> <node> ... [attributes]
		 * \endverbatim
		 */

		line = get_valid_line(f);
		myStringList mystrlist = mystring(line).split();
		int num_ele, num_node, num_att;
		num_ele  = mystrlist[0].ToInt();		/**< num_node: how many nodes							*/
		num_node = mystrlist[1].ToInt();		/**< num_dim : how many coords per node (must be 2 or 3)*/
		num_att  = mystrlist[2].ToInt();		/**< num_att : how many attributes 						*/

		if (num_att > 1)
				cmdline::warning("More than 1 attribute in *.ele is not supported.");

		int cnt = 0;
		int diff = 0;
		while (!f.eof() ){
			mystring stripped = get_valid_line(f).strip();
			myStringList mylist = stripped.split();

			int index = mylist.take_front().ToInt(); //! index of the node

			if (cnt == 0){
				diff = index - cnt;
				if (diff != 0)
					cmdline::warning("Be careful ... indices are not starting at 0.");
			}
			else
			if (index - cnt != diff){
				cmdline::warning("Node indices are not in ascending order.");
			}

			Face face;
//			myList<int> v;
			for (int i = 0; i < num_node ; i++)
				face.v.push_back(mylist.take_front().ToInt());

			for (int i = 0; i < num_att ; i++) {
				switch (i){
					case 0:		face.attribute = mylist.take_front().ToInt();
								break;
					default:	mylist.pop_front();
								break;
					}
			}

			append_face(face); //!< append this point to the list of points

			cnt++;
			if (cnt == num_ele) break;
		};
		f.close();
	}
	catch (myexception e) {
		cerr << e.what() << endl;
		return false;
	}
	if (debug) cmdline::ok();
	return true;
}

/** name: SurfaceMesh::read_poly
 *
 * Reads in poly files from triangle. See to e incompatible to poly format
 * from tetgen. But looks similar to *.smesh. Maybe they are the same
 * \todo Check if tetgen-smesh equals triangle-poly.
 *
 * The poly files contains the boundary computed by triangle
 *
 * \param fn: name of the file to read
 * \return
 */
bool SurfaceMesh::read_poly(const char *fn){
	if (debug) printf(" - Reading %s ", fn);
	try	{
		mystring line("# comment"); // init;
		fstream f(fn,fstream::in);
		if (f.fail() ) throw Exception_IOFailure(EXCEPTION_ID + string(fn));
//		myexception("  File input error (" + string(fn) + ").");

		// clean up nodes
		l.clear();

		/** *.poly description (from triangle)
		 * \verbatim
		 * * First line: <# of vertices> <dimension (must be 2)>  <# of attributes> <# of boundary markers (0 or 1)>
		 * * Following lines: <vertex #> <x> <y> [attributes] [boundary marker]
		 * * One line: <# of segments> <# of boundary markers (0 or 1)>
		 * * Following lines: <segment #> <endpoint> <endpoint> [boundary marker]
		 * * One line: <# of holes>
		 * * Following lines: <hole #> <x> <y>
		 * * Optional line: <# of regional attributes and/or area constraints>
		 * * Optional following lines: <region #> <x> <y> <attribute> <maximum area>
		 * \endverbatim
		 */
//		################################## nodes #############################
		line = get_valid_line(f);
		myStringList mystrlist = line.split();

		bool has_bnd = false;
		int num_node = mystrlist[0].ToInt();		/**< num_node: how many nodes							*/
		int num_dim  = mystrlist[1].ToInt();		/**< num_dim : how many coords per node (must be 2 or 3)*/
//		int num_att  = mystrlist[2].ToInt();		/**< num_att : how many attributes 						*/
		has_bnd      = mystrlist[3] =="1";			/**< has_bnd : boundary markers [on/off] = [1/0] 		*/

		if (num_node > 0){
			cmdline::warning("There are nodes defined in the poly file, which won't be read. In triangle poly files contain only the boundaries with nodes defined in *.node-file.");

			// skip all nodes
			int cnt = 0;
			while (!f.eof() ){
				line = get_valid_line(f);
				cnt++;
				if (cnt == num_node) break;
			}

			if (num_dim < 4) dim = num_dim; // save number of dimensions
		}



//		############################## line segments ###########################
		line = mystring("# comment");
		//! search for description line of segmments
		line = get_valid_line(f);
		mystrlist = line.split();

		int num_seg;
		has_bnd = false;
		num_seg  = mystrlist[0].ToInt();		/**< num_seg : how many segments 						*/
		has_bnd  = mystrlist[1] =="1";			/**< has_bnd : boundary markers [on/off] = [1/0] 		*/

		int cnt = 0;
		int diff = 0;
		while (!f.eof() ){
			mystring stripped = get_valid_line(f).strip();
			myStringList mylist = stripped.split();

			int index = mylist.take_front().ToInt(); //! index of the node
			if (cnt == 0){
				diff = index - cnt;
				if (diff != 0)
					cmdline::warning("Be careful ... indices are not starting t 0.");
			}
			else
			if (index - cnt != diff){
				cmdline::warning("Node indices are not in ascending order.");
			}

			int v[2] = {0,0};
			for (int i = 0; i < 2 ; i++)
				v[i] = mylist.take_front().ToInt();
			int bnd = 0;
			if (has_bnd)
				bnd = mylist.take_front().ToInt();

			myLine segment = myLine(v[0], v[1], bnd);

			append_line(segment); //!< append this point to the list of points
			cnt++;
			if (cnt == num_seg) break; // we have found all segments
		}

//		################################## holes #############################
//		* * One line: <# of holes>
//		 * * Following lines: <hole #> <x> <y>
		line =  mystring("# comment");
		//! search for description line of holes
		mystrlist = get_valid_line(f).split();
		int num_hole = mystrlist[0].ToInt();		/**< num_seg : how many segments 						*/
		if (num_hole > 0 ) {
			if (debug) cmdline::warning("Holes found ! These are currently not supported");

			// skip all holes
			int cnt = 0;
			while (!f.eof() ){
				mystring stripped = get_valid_line(f).strip();
				cnt++;
				if (cnt == num_hole) break;
			}
		}

//		################################## regions #############################
		line = mystring("# comment");
		//! search for description line of regions
		line = get_valid_line(f);
		if ( f.eof() ) {
			if (debug) cmdline::warning("Hint : *.poly : no regions defined");
		}
		else {
			mystrlist = line.split();
			int num_reg = mystrlist[0].ToInt();		/**< num_reg : how many regions				*/

			if (num_reg > 0){
				int cnt = 0;
				while (!f.eof()){
					mystring stripped = get_valid_line(f).strip();
					mystrlist = stripped.split();

					mystrlist.take_front().ToInt();
					Point pos;
					pos.x = mystrlist.take_front().ToDouble();
					pos.y = mystrlist.take_front().ToDouble();

					int att = 0;
					if (mystrlist.size() > 0)
						att = mystrlist.take_front().ToInt();

					double area_constraint = 0.;
					if (mystrlist.size() > 0)
						area_constraint = mystrlist.take_front().ToDouble();

					append_region(Region(pos, att, area_constraint));
					cnt++;
					if (cnt == num_reg) break;
				}
			}
		}
		f.close();
	}
	catch (Exception_IOFailure e) {
		cerr << e.what() << endl;
		return false;
	}
	if (debug) cmdline::ok();
	return true;
}

/** name: SurfaceMesh::read_mesh
 * 	Reads in the mesh structure. The meshes can be in CARP or in tetgen style.
 * @param base: Name of one of the mesh files. If the name has no file extension
 * 				then it is assumed that tetgen style meshes shall be loaded.
 * 				Otherwise base must have the extension .pts or .tetras for
 * 				CARP-style files
 * @return true or false depending on the read result.
 **/
bool SurfaceMesh::read_mesh(const char *base){
	mystring name(base), ext;
	ext  = name.file_ext().lower();
	name = name.file_base();
	if( ext == "pts" or ext == "elem" or ext == "tetras" ){ // try to read CARP type meshes
		if ( not read_pts( (name+".pts").c_str()	  ) ) throw(myexception("Could not read pts file"));

		mystring fn = name+"."+"elem";	// By default load the newer elem format
										// except the user wants explicitly the
										// tetras format or the elem file does
										// not exist.
		if ((ext == "tetras") or ( not fn.file_exists())) fn = name+"."+"tetras";
		if ( not read_tetras( fn.c_str()) ) throw(myexception("Could not read tetras file"));

		return true;
	}

	if ((ext != "node") and (ext != "ele" ) and (ext != "poly"))
		name = base;

	// otherwise try to read tetgen meshes
	bool res1 = read_node( (name + ".node").c_str() );
	bool res2 = read_ele ( (name + ".ele" ).c_str() );
	bool res3 = read_poly( (name + ".poly").c_str() );

	if ((not res1) or (not res2) ) {
		throw myexception(" There were errors while reading the mesh.");
		return false;
	}

	if (not res3) {
		cerr << "Warning " << name << " not found" << endl;
//		return false;
	}
	return true;
}

bool SurfaceMesh::read_pts(list<Point> &plist, const char *fn){
	fstream f(fn,fstream::in);
	if (f.fail() ) throw myexception("  File input error (" + string(fn) + ").");

	plist.clear();

	size_t nr_pts = 0;
	f >> nr_pts;

	double x = 0.; double y = 0.; double z = 0.;
	while (f.good() and not f.eof()){
		f >> x;
		f >> y;
		f >> z;
		if (f.fail()) {
			cerr << fn << " : I/O error." << endl;
			return false;
		}
		plist.push_back(Point(x,y,z));
	};
	f.close();
	return true;
}

bool SurfaceMesh::read_pts(vector<Point> &pvec, const char *fn){
	fstream f(fn,fstream::in);
	if (f.fail() ) throw myexception(" File input error (" + string(fn) + ").");

	pvec.clear();

	size_t nr_pts = 0;
	f >> nr_pts;
	pvec.assign(nr_pts, Point());

	size_t i=0;
	while (f.good() and not f.eof() and i < nr_pts){
		f >> pvec[i].x;
		f >> pvec[i].y;
		f >> pvec[i].z;
		if (f.fail()) {
			cerr << fn << " : I/O error." << endl;
			return false;
		}
		i++;

	};
	f.close();
	return true;
}

bool SurfaceMesh::read_vecdirs(vector<Point> &pvec, const char *fn){
	fstream f(fn,fstream::in);
	if (f.fail() ) throw myexception("  File input error (" + string(fn) + ").");

	pvec.clear();
	size_t i=0;
	while (f.good() and not f.eof()){
		Point p;
		f >> p.x;
		f >> p.y;
		f >> p.z;
		if (f.fail()) {
			cerr << fn << " : I/O error. End of file ?" << endl;
			return false;
		}
		pvec.push_back(p); // append to list
		i++;

	};
	f.close();
	return true;
}

/** name: SurfaceMesh::read_vec
 * Read in *.vec files containing descriptions for vectors using their
 * centers an directions.
 *
 * The format of these files is:
 * <int> <-- Number of vectors
 * x y z a b c
 *
 * ,where (x,y,z) defines a set of coordinates in a cartesian
 * coordinate system and (a,b,c) are vectors defining the length and
 * direction.
 *
 * @param vec_ctrs : vector<Point> to store the coordinates
 * @param vec_dirs : vector<PointSpherical> to store the directions.
 * @param fn       : Name of the file to read from.
 * @return number of vectors
 */
size_t SurfaceMesh::read_vec(vector<Point> &vec_ctrs, vector<PointSpherical> &vec_dirs, const char *fn){
	vec_ctrs.clear();
	vec_dirs.clear();
	size_t numVec = 0;
	mystring filename(fn);
	cmdline::msg(" - Reading " + string(filename));
	if ((filename.file_ext().lower() == "vect") or (filename.file_ext().lower() == "vec")){
		ifstream f(filename.c_str());
		f >> numVec;
		vec_ctrs.assign(numVec, Point());
		vec_dirs.assign(numVec, PointSpherical());

		vector<Point>::iterator           itp = vec_ctrs.begin();
		vector<PointSpherical>::iterator itd = vec_dirs.begin();
		Point pt;
		size_t cnt = 0;
		while (f.good() and cnt < numVec) {
			f >> itp->x;
			f >> itp->y;
			f >> itp->z;
			f >> pt.x;
			f >> pt.y;
			f >> pt.z;
			*itd = PointSpherical(pt);
			itp++; // increase iterators
			itd++;
			cnt++;
		}
		f.close();
		if (cnt != numVec) {
			cerr << "(!!!) Did not read enough items "  << cnt << "/" << numVec;
			cmdline::no();
		}
		cout << " - read " << cnt << " vectors" << endl;

	}
	return numVec;
}

/** name: SurfaceMesh::read_vec
 * Read in *.vec files containing descriptions for vectors using their
 * centers an directions.
 *
 * The format of these files is:
 * <int> <-- Number of vectors
 * x y z a b c
 *
 * ,where (x,y,z) defines a set of coordinates in a cartesian
 * coordinate system and (a,b,c) are vectors defining the length and
 * direction.
 *
 * @param vec_ctrs : vector<Point> to store the coordinates
 * @param vec_dirs : vector<Point> to store the directions.
 * @param fn       : Name of the file to read from.
 * @return number of vectors
 */
size_t SurfaceMesh::read_vec(vector<Point> &vec_ctrs, vector<Point> &vec_dirs, const char *fn){
	vec_ctrs.clear();
	vec_dirs.clear();
	size_t numVec = 0;
	mystring filename(fn);
	cmdline::msg(" - Reading " + string(filename));
	if ((filename.file_ext().lower() == "vect") or (filename.file_ext().lower() == "vec")){
		ifstream f(filename.c_str());
		f >> numVec;
		vec_ctrs.assign(numVec, Point());
		vec_dirs.assign(numVec, Point());

		vector<Point>::iterator itp = vec_ctrs.begin();
		vector<Point>::iterator itd = vec_dirs.begin();
		size_t cnt = 0;
		while (f.good() and cnt < numVec) {
			f >> itp->x;
			f >> itp->y;
			f >> itp->z;
			f >> itd->x;
			f >> itd->y;
			f >> itd->z;
			itp++; // increase iterators
			itd++;
			cnt++;
		}
		f.close();
		if (cnt != numVec) {
			cerr << "(!!!) Did not read enough items "  << cnt << "/" << numVec;
			cmdline::no();
		}
		cout << " - read " << cnt << " vectors" << endl;

	}
	return numVec;
}

/** name: SurfaceMesh::read_pts
 *
 * Read in *.pts files
 * The attribute can be set or not, if not as alue for the attribute 0 is used
 * There is only one attribute allowed, which is of the type int
 *
 * \param fn : filename of the nodefile
 * \return bool
 */
bool SurfaceMesh::read_pts(const char *fn){
	clock_t start=0;
	if (debug) {
		start = clock();
		cmdline::msg(" - Reading " +string(fn));
	}
	try	{
		mystring line("# comment");
		fstream f(fn,fstream::in);
		if (f.fail() ) throw myexception("  File input error (" + string(fn) + ").");

		// clean up nodes
		p.clear();

		line = get_valid_line(f);

		int num_node = line.strip().ToInt();		/**< num_node: how many nodes							*/

		int cnt = 0;
		while (!f.eof() ){
			mystring stripped = get_valid_line(f).strip();
			myStringList mylist = stripped.split();

			int i = 0;
			double v[3] = {0., 0., 0.};
			for (i = 0; i < 3 ; i++)
				v[i] = mylist.take_front().ToDouble();

			// get the attribute
			int attribute = 0;
			if ( mylist.size() > 0 )  attribute = mylist.take_front().ToInt();

			Point pt = Point(v[0],v[1], v[2]);
			pt.attribute(attribute);
			append_point(pt); //!< append this point to the list of points

			cnt++;
			if (cnt == num_node) break;
		};
		f.close();

		//determine dimension
		dim = determine_dimension();

	}
	catch (myexception e) {
		cmdline::warning(e.what());
		return false;
	}
	if (debug){
		clock_t end = clock();
		cout << (end-start)/ 1000. << " msec";
		cmdline::ok();
	}
	return true;
}

/** name: SurfaceMesh::read_tetras()
 *
 * Read in *.tetras and *.elem files. The last one is preferred since this
 * format is newer. the format is automatically detected while reading in the
 * file. in the case of the elem format only triangles (Tr) and tetrahedra will
 * be read in successfully.
 *
 * \param fn       : filename of the nodefile
 * \param nr_edges : number of edge points (= 4 for tets, = 3 for triangles)
 * 					 \attention Overriden for the elem format.
 * \return bool
 */
bool SurfaceMesh::read_tetras(const char *fn, int nr_edges){

	clock_t start=0;
	if (debug) {
		start = clock();
		cmdline::msg(" - Reading " + string(fn));
	}
	Profiler prof(0);
	try	{
		mystring line("# comment");
		fstream f(fn,fstream::in);
		if (f.fail() ) throw myexception("  File input error (" + string(fn) + ").");

		// clean up nodes
		f.clear();

		line = get_valid_line(f);
		// the first line contains the number of elements
		int num_tets = line.strip().ToInt();		/**< num_tets: how many tetras*/
		prof.reset(num_tets);
		int cnt = 0;
		while (!f.eof() ){
			mystring stripped = get_valid_line(f).strip();
			myStringList mylist = stripped.split();
			Face face;

			mystring s = mylist.front();
			if ( s.isalpha()){ // if new *.elem format
				mylist.pop_front();			 // remove the identifier item from the list
				if (s == "Tr") nr_edges = 3;
				else
				if (s == "Tt") nr_edges = 4;
				else
				throw myexception("SurfaceMesh::read_tetras() : Unsuppported element '" + s +"' found !");
			}

			for (int i = 0; i < nr_edges ; i++)
				face.v.push_back(mylist.take_front().ToInt());

			// If mylist has still elements then they are attributes
			// Only one attribute is saved here
			if (mylist.size() > 0)
				face.attribute = mylist.take_front().ToInt();

			append_face(face); //!< append this point to the list of points

			cnt++;
			if (debug) prof.info();
			if (cnt == num_tets) break;
		};
		f.close();
	}
	catch (myexception e) {
		cmdline::warning(e.what());
		return false;
	}
	if (debug){
		prof.finalize();
		clock_t end = clock();
		cout << (end-start)/ 1000. << " msec";
		cmdline::ok();
	}

	return true;
}

/** name: SurfaceMesh::read_vtx()
 * (Stefan Fruhner - 17.10.2011 16:53:44 CEST)
 * Reads in vtx files, where vtx stands for 'vertex'. These files contain an
 * index list of vertices. The indices correspond to the points array 'p'.
 * @param fn: Name of the file to load.
 **/
void SurfaceMesh::read_vtx(const char *fn){
	if (debug) printf(" - Reading %s \n", fn);

	mystring line("# comment");
	fstream f(fn,fstream::in);
	if (f.fail() ) throw myexception("  File input error (" + string(fn) + ").");

	// clean up nodes
	f.clear();

	line = get_valid_line(f).strip();
	// the first line contains the number of elements
	int num_vtx = line.ToInt();		/**< num_vtx: how many vertices ?*/

	line = get_valid_line(f).strip();
	// the second line is allowed to be 'extra' or 'intra'
	mystring extra(line);

	if (extra != "extra") throw myexception("SurfaceMesh::read_vtx() : '" + extra + "' is not supported, only 'extra'");

	vtx.clear();
	int cnt = 0;
	while (!f.eof() ){
		mystring stripped = get_valid_line(f).strip();
		vtx.push_back(stripped.ToInt());
		cnt++;
		if (cnt == num_vtx) break;
	};
	f.close();
	if (debug) cmdline::ok();
	return;
}

 /** name: SurfaceMesh::save_pts()
 * (Stefan Fruhner - 17.10.2011 16:54:16 CEST)
 *
 * Saves a pts file containing all vertices of the mesh.
 * Format:
 * # Nodes
 * x y z attribute
 *
 * @param fn: file name
 * @return True
 **/
bool SurfaceMesh::save_pts(const char *fn){

	print("  - Writing *.pts file : " << fn);

	FILE *o;
	o = fopen(fn, "w");
	if (! o ) throw myexception("  File output error (" + string(fn) + ").");

	fprintf(o,"%d\n", (int) points()); //! write the first line : # of tetras
	for (size_t i = 0; i < points(); i++){
		fprintf(o,"%f %f %f\n", p[i].x,p[i].y,p[i].z);
////		fprintf(o,"%lf %lf %lf %d\n", p[i].x,p[i].y,p[i].z,p[i].attribute());
	}
	fclose(o);
	return true;
}

bool SurfaceMesh::save_pts(mystring fn){
	mystring name = fn.file_base() + ".pts";
	return save_pts(name.c_str());
}

bool SurfaceMesh::save_tetras(const char *fn){
	print("  - Writing *.tetras file : " << fn);
	fstream o(fn, fstream::out);
	if (o.fail() ) throw myexception("  File output error (" + string(fn) + ").");

	size_t nr_faces = 0;

	for (size_t i = 0; i < elements(); i++){
		if (f[i].v.size() == 4) nr_faces++;
	}

	if ( nr_faces != elements() )
		print(" (!) Warning : Not all facets are tetrahedra, skipping these ... ");

	o << nr_faces << endl; //! write the first line : # of tetras
	for (size_t i = 0; i < elements(); i++){
		if (f[i].v.size() == 4) {
			o << f[i].v[0] << " " << f[i].v[1] << " " << f[i].v[2] << " " << f[i].v[3] << " " << f[i].attribute << endl;
		}
	}
	o.close();
	return true;
}

bool SurfaceMesh::save_tetras(mystring fn){
	mystring name = fn.file_base() + ".pts";
//	print(name);
	return save_tetras(name.c_str());
}

bool SurfaceMesh::save_tri(const char *fn){
	cout << "Cannot save "<< fn << "." << endl;
	throw myexception("Not yet implemented");
	return true;
}

bool SurfaceMesh::save_elem(const char *fn){
	print("  - Writing *.elem file : " << fn);
	fstream o(fn, fstream::out);
	if (o.fail() ) throw myexception("  File output error (" + string(fn) + ").");

	size_t  nr_faces = 0;

	for (size_t i = 0; i < elements(); i++){
		if (f[i].v.size() == 3 or (f[i].v.size() == 4)) nr_faces++;
	}

	if ( nr_faces != elements() )
		print(" (!) Warning : Not all facets are tetrahedra or triangles, skipping these ... ");

	Profiler prof(elements());
	o << nr_faces << endl; //! write the first line : # of tetras
	for (size_t i = 0; i < elements(); i++){
		switch (f[i].v.size()){
			case 3: // Triangles
				o << "Tr " << f[i].v[0] << " " << f[i].v[1] << " " << f[i].v[2]
				  << " "   << f[i].attribute   << endl;
				break;
			case 4:	// Tetrahedra
				o << "Tt " << f[i].v[0] << " " << f[i].v[1] << " " << f[i].v[2]
				  << " "   << f[i].v[3] << " " << f[i].attribute   << endl;
				break;
			default:
				throw string("Type of element not supported");
			if (debug) prof.info();
		}
	}
	if (debug) prof.finalize();
	o.close();
	return true;
}


/** name: SurfaceMesh::save_simple_lon()
 * Saves a simple lon-file for every triangular or tetrahedral element in the
 * mesh.  Two type of vectors are written: (1.,0.,0.) if the element's
 * attribute is equal to the function parameter region, or (0.,0.,0.) else.
 * @param fn 	: filename of the file (including extension .lon)
 * @param region: Attribute for which the vector 1.,0.,0. is written.
 * @return
 **/
bool SurfaceMesh::save_simple_lon(const char *fn, int region){
	print("  - Writing *.lon file : " << fn);
	fstream o(fn, fstream::out);
	if (o.fail() ) throw myexception("  File output error (" + string(fn) + ").");
	o << "1" << endl;
	for (size_t i = 0; i < elements(); i++){
		if (f[i].v.size() == 3 or (f[i].v.size() == 4)){//only count triangles or tetrahedra
			if (f[i].attribute == region) o << "1.0 0.0 0.0" << endl;
			else o << "0.0 0.0 0.0" << endl;
		}
	}
	return true;
}

/** name: SurfaceMesh::save_mesh()
 * (Stefan Fruhner - 14.11.2011 11:51:28 CET)
 * Save the mesh in CARP format. Also a simple lon file is created if the
 * parameter region is > -1
 * @param basename : The name of the mesh to be saved. The extensions .pts
 * 					 and .elem are added automatically.
 * @param region   : The region value that denotes the interior or the mesh.
 * 					 -- only needed for computation of simple *.lon files.
 * @return True on success.
 **/
bool SurfaceMesh::save_mesh(const char *basename){
	determine_dimension();
	char base[1024];
	sprintf(base,"%s.pts",basename);
	save_pts(base);
	if (elemtype == triangles)
		sprintf(base,"%s.surf",basename);
	else
		sprintf(base,"%s.elem",basename);
	save_elem(base);

//	if (region == -1) return true;

//	sprintf(base,"%s.lon",basename);
//	save_simple_lon(base, region);
	return true;
}

/** name: SurfaceMesh::index_of_point
 *  Determines the index to a point given.
 * \param pt: a point
 * \return index of that point in the list
 */
size_t SurfaceMesh::index_of_point(Point pt){
	for (size_t i = 0; i < points(); i++){
		if (p[i] == pt) return i;
	}
	throw myexception(EXCEPTION_ID+"Error : Point was not found");
}

/** name: SurfaceMesh::point
 *  Returns a reference to a point that has the index idx.
 * \param idx: Index in the point list
 * \return Reference to Point
 */
const Point& SurfaceMesh::point(size_t idx){
	if (idx < points())	return p[idx];

	throw myexception(EXCEPTION_ID+"Error : Point was not found");
}

const nbset& SurfaceMesh::neighbors(size_t idx){
	if ( nb.size() != points() ) compute_neighbour_list();
	if (idx < points())	return nb[idx];

	throw myexception(EXCEPTION_ID+"Error : Point was not found");
}

/** name: SurfaceMesh::face()
 *  Returns a reference to a face that has the index idx.
 * \param idx: Index in the faces' list
 * \return Reference to Face
 */
const Face& SurfaceMesh::face(size_t idx) const {
	if (idx < elements())	return f[idx];

	throw myexception(EXCEPTION_ID+"Error : Face was not found");
}

/** name: SurfaceMesh::face()
 *  Returns a reference to a face that has the index idx.
 * \param idx: Index in the faces' list
 * \return Reference to Face
 */
const Facet SurfaceMesh::facet(size_t idx) const {
	if (idx < elements() and f[idx].v.size() == 3)
		return Facet(	p[f[idx].v[0]],
						p[f[idx].v[1]],
						p[f[idx].v[2]]);

	throw myexception(EXCEPTION_ID+"Error : Face was not found or the number of\
 vertices did not match 3.");
}

/** name: SurfaceMesh::scan_for_regions()
 * (Stefan Fruhner - 15.11.2011 08:16:27 CET)
 * Scans the SurfaceMesh data structure for different region definitions and
 * returns a list of attributes.
 * @return myList<int> where int denotes a region attribute.
 **/
const myList<int>& SurfaceMesh::scan_for_regions(){
	regions.clear();
	for (size_t i = 0; i < elements(); i++)
		regions.append(f[i].attribute);
	regions.sort();
	regions.unique();
	#ifdef DEBUG
	print("   - Regions in the list");
	regions.prnt();
	#endif
	return regions;
}

/** name: SurfaceMesh::scan_for_regions()
 * (Stefan Fruhner - 15.01.2013)
 * Gives some region stastistics.
 * @return map<int, double> that maps regions IDs to volume.
 **/
map<int,double> SurfaceMesh::region_statistics(mystring source_unit, mystring output_unit){
	scan_for_regions();
	map<int, double> rmap;
	map<int, size_t> relcnt;	// region map number edges
	map<int, double> relmax; 	// region map edge length maximum
	map<int, double> relmin; 	// region map edge length minimum
	map<int, double> relmean; 	// region map edge length mean

	double min_length = 1.e40;
	double max_length = 0.;
	double mean_length = 0.;
	size_t lines = 0;
	// init
	for (myList<int>::iterator i = regions.begin(); i != regions.end(); i++){
		rmap[*i]    = 0.;
		relcnt[*i] = 0;
		relmax[*i]  = 0.;
		relmin[*i]  = 1.e40;
		relmean[*i] = 0.;
	}

	for (size_t i = 0; i < elements(); i++){
		int &reg = f[i].attribute;
		rmap[reg] += Volume(i); // count the volumes

		switch (f[i].v.size()){
			case 3: // triangles
				//! \todo implement me !
				break;
			case 4: // tets
			{
				lines+=4;
				relcnt[reg] +=4;

				double l1 = Line(p[f[i].v[0]], p[f[i].v[1]], Line::segment).length();
				double l2 = Line(p[f[i].v[0]], p[f[i].v[2]], Line::segment).length();
				double l3 = Line(p[f[i].v[0]], p[f[i].v[3]], Line::segment).length();
				double l4 = Line(p[f[i].v[1]], p[f[i].v[2]], Line::segment).length();

				mean_length += l1 + l2 + l3 + l4;
				relmean[reg] += l1 + l2 + l3 + l4;

				min_length = (l1 < min_length) ? l1 : min_length;
				min_length = (l2 < min_length) ? l2 : min_length;
				min_length = (l3 < min_length) ? l3 : min_length;
				min_length = (l4 < min_length) ? l4 : min_length;

				relmin[reg] = (l1 < relmin[reg]) ? l1 : relmin[reg]; // min edge length per region
				relmin[reg] = (l2 < relmin[reg]) ? l2 : relmin[reg];
				relmin[reg] = (l3 < relmin[reg]) ? l3 : relmin[reg];
				relmin[reg] = (l4 < relmin[reg]) ? l4 : relmin[reg];

				max_length = (l1 > max_length) ? l1 : max_length;
				max_length = (l2 > max_length) ? l2 : max_length;
				max_length = (l3 > max_length) ? l3 : max_length;
				max_length = (l4 > max_length) ? l4 : max_length;

				relmax[reg] = (l1 > relmax[reg]) ? l1 : relmax[reg];// max edge length per region
				relmax[reg] = (l2 > relmax[reg]) ? l2 : relmax[reg];
				relmax[reg] = (l3 > relmax[reg]) ? l3 : relmax[reg];
				relmax[reg] = (l4 > relmax[reg]) ? l4 : relmax[reg];

				break;
			}
			default:
				throw string("Error!");
		}
	}

	// compute mean edge lengths
	mean_length /= (double) lines;
	for (myList<int>::iterator i = regions.begin(); i != regions.end(); i++){
		relmean[*i] /= relcnt[*i];
	}

	double factor = units::SI_Prefix_Factor(source_unit, output_unit);

	cout << "Region statistics:" << endl;
	cout << "Region number : volume" << endl;
	double Vol = 0.;
	for (myList<int>::iterator i = regions.begin(); i != regions.end(); i++){
		Vol += rmap[*i];
	}

	for (myList<int>::iterator i = regions.begin(); i != regions.end(); i++){
		cout << "Region " << *i << " : "
			<< rmap[*i]/ factor / factor / factor << " "
			<< output_unit << "^3\t ( " << rmap[*i]/Vol * 100 << " % )"
			<< endl;
	}

	cout << endl << "Edge length statistics " << endl;
	for (myList<int>::iterator i = regions.begin(); i != regions.end(); i++){
		cout << "Region " << *i << " : " << endl;
		cout << "  # edges          : " << relcnt[*i] << endl;
		cout << "  mean edge length : " << relmean[*i] / factor << " " << output_unit  << endl;
		cout << "   min edge length : " << relmin[*i]  / factor << " " << output_unit << endl;
		cout << "   max edge length : " << relmax[*i]  / factor << " " << output_unit << endl;
	}

	cout << endl << "Overall edge length stats " << endl;
	cout << "  Volume           : " << Vol/factor/factor/factor << " " << output_unit  << "^3" << endl;
	cout << "  # edges          : " << lines << endl;
	cout << "  mean edge length : " << mean_length << " " << output_unit << endl;
	cout << "   min edge length : " << min_length  << " " << output_unit << endl;
	cout << "   max edge length : " << max_length  << " " << output_unit << endl;
	return rmap;
}

/** name: SurfaceMesh::compute_surface_i()
 * (Stefan Fruhner - 23.11.2011 11:13:42 CET)
 * Computes the surface of a region with the given ID. If no ID is
 * supplied then the surface of the complete mesh is determined.
 *
 * A face is assumed to be an interior face it is found twice in the
 * mesh. When extracting the counterclockwise order of the vertices is
 * destroyed and hence the face normals aren't remained.
 *
 * \attention 	This algorithm works only for well posed meshes, e.g
 * 				if there are three equal faces will not be detected.
 *
 * @param region: ID of the region to be extracted.
 * @return 	map <list<int> , int >, where each entry belongs to a face . The
 * 			first entry is a list of node indices. The second entry conatin the
 * 			region ID.
 **/
map< list<int> , pair<int,int> > SurfaceMesh::compute_surface_i(const set<int> &region){
	map< list<int> , pair<int,int> > mymap; //!< red black tree \attention only the first entry is used
	pair<map< list<int> ,pair<int,int> >::iterator,bool> res;
	Profiler prof(elements());
	for (size_t i = 0; i < elements(); i++){ // all tets
		if ((region.size() != 0)
			and (region.find(f[i].attribute) == region.end() )) continue; // ignore other regions
		int &a = f[i].attribute;
		// for every tets compute the boundary faces
		vector<int> &v = f[i].v;

		if ( v.size() != 4 ) continue; // skip entries which are no tets

		//! \attention we sort the lists so that we can compare faces simpler
		int l1[] = {v[0], v[1], v[2]}; list<int> f1(l1, l1+3); f1.sort();
		int l2[] = {v[1], v[2], v[3]}; list<int> f2(l2, l2+3); f2.sort();
		int l3[] = {v[0], v[2], v[3]}; list<int> f3(l3, l3+3); f3.sort();
		int l4[] = {v[0], v[1], v[3]}; list<int> f4(l4, l4+3); f4.sort();

		// Try to insert each face to the list
		// If the face is already found in the list, then it must be an interiour face.
		res=mymap.insert(pair< list<int>, pair<int,int> >(f1, pair<int,int>(v[3],a)) ); // element insertion
		if ( not res.second )	mymap.erase(f1);	// on collision erase the first candidate

		res=mymap.insert(pair< list<int>, pair<int,int> >(f2, pair<int,int>(v[0],a)) ); // element insertion
		if ( not res.second )	mymap.erase(f2);

		res=mymap.insert(pair< list<int>, pair<int,int> >(f3, pair<int,int>(v[1],a)) ); // element insertion
		if ( not res.second )	mymap.erase(f3);

		res=mymap.insert(pair< list<int>, pair<int,int> >(f4, pair<int,int>(v[2],a)) ); // element insertion
		if ( not res.second )	mymap.erase(f4);
		if (debug) prof.info();
	}

	if (debug) prof.finalize();

	// set the boundary markers of these points to 1
	for (map< list<int> ,  pair<int,int> >::iterator it=mymap.begin() ; it != mymap.end(); it++ ){
		list<int>::const_iterator i = it->first.begin();
		p[*i++].boundary(1);
		p[*i++].boundary(1);
		p[*i  ].boundary(1);
	}
	return mymap;
}

/** SurfaceMesh::find_border_segments
 *
 * Uses a triangulation to find it's border.
 * Utilizes red-black-trees to be very fast. All lines that
 * are part of a triangle are inserted in the tree. Lines
 * that are inserted twice must be interior lines and are
 * deleted. The remaining lines in the tree must be part of
 * the border.
 *
 * \note This mehtod is only applicable to 2D meshes, whose border is a line.
 *
 * \attention These line segments are not sorted !
 * @param region : find the border of a certain region (default region=0)
 * @return Map that contains the lines as keys (= LineMap).
 */
LineMap SurfaceMesh::find_border_segments(int region){
	if ( dim != 2 ) throw myexception("SurfaceMesh::find_border_segments(): Cannot proceed, the mesh is not 2D !");

	// Find all lines that are used only once in the faces
	LineMap mymap; //!< red black tree \attention only the first entry is used

	pair<map<myLine,int>::iterator,bool> res;
	for (size_t i = 0; i < elements(); i++){
		if ((region != -1) and (f[i].attribute != region)) continue; // ignore other regions
		// for every face insert the boundary
		myLine f1 = myLine(f[i].a(), f[i].b());
		myLine f2 = myLine(f[i].a(), f[i].c());
		myLine f3 = myLine(f[i].b(), f[i].c());

		res=mymap.insert(pair<myLine,int>(f1, i) ); // element insertion
		if ( not res.second )	mymap.erase(f1);	// on collision erase the first candidate \attention every line is used only twice so there's only one collision per Line

		res=mymap.insert(pair<myLine,int>(f2, i) ); // element insertion
		if ( not res.second )	mymap.erase(f2);

		res=mymap.insert(pair<myLine,int>(f3, i) ); // element insertion
		if ( not res.second )	mymap.erase(f3);
	}

	// set the boundary markers
	for (LineMap::iterator it = mymap.begin(); it != mymap.end(); ++it){
		p[it->first.a].boundary(1);
		p[it->first.b].boundary(1);
	}

	return mymap;
}

/** name: SurfaceMesh::compute_surface()
 * (Stefan Fruhner - 14.11.2011 11:51:28 CET)
 * Computes the surface of a region with the given region ID. If no ID is
 * supplied then the surface of the complete mesh is determined.
 *
 * A face is assumed to be an interior face it is found twice in the mesh. When
 * extracting the counterclockwise order of the vertices is destroyed and hence
 * the face normals aren't remained. For this purpose we use
 * SurfaceMesh::compute_surface_i(ID).
 *
 * That's why this function corrects all
 * normals after extracting the surface. Therefor the interior points of each
 * tet is used.
 *
 * \attention 	This algorithm works only for well posed meshes, e.g if there are
 * 			 	three equal faces will not be detected.
 *
 * @param region: ID of the region to be extracted.
 * @return A new SurfaceMesh;
 **/
SurfaceMesh SurfaceMesh::compute_surface(int region){
	set<int> regions;
	regions.insert(region);

	return compute_surface(regions);
}

/** name: SurfaceMesh::compute_surface()
 * (Stefan Fruhner - 14.11.2011 11:51:28 CET)
 * Computes the surface of a region with the given region ID. If no ID is
 * supplied then the surface of the complete mesh is determined.
 *
 * A face is assumed to be an interior face it is found twice in the mesh. When
 * extracting the counterclockwise order of the vertices is destroyed and hence
 * the face normals aren't remained. For this purpose we use
 * SurfaceMesh::compute_surface_i(ID).
 *
 * That's why this function corrects all
 * normals after extracting the surface. Therefor the interior points of each
 * tet is used.
 *
 * \attention 	This algorithm works only for well posed meshes, e.g if there are
 * 			 	three equal faces will not be detected.
 *
 * @param region: ID of the region to be extracted.
 * @return A new SurfaceMesh;
 **/
SurfaceMesh SurfaceMesh::compute_surface(const set<int> &region){
	if ( dim != 3 ) throw myexception("SurfaceMesh::find_border_faces(): Cannot proceed, the mesh is not 3D !");

	// compute surface and set the boundary markers
	map< list<int> , pair<int,int> > mymap = compute_surface_i(region);

	// convert the map to a SurfaceMesh
//	int roi = ( region != -1 ) ? region : 0;
	SurfaceMesh surface;
	for (map< list<int> ,  pair<int,int> >::iterator it=mymap.begin() ; it != mymap.end(); it++ ){
		list<int>::const_iterator i = it->first.begin();
		int &interior = it->second.first;
		int &roi      = it->second.second;

//		indices in the old array
		size_t a = *i++;
		size_t b = *i++;
		size_t c = *i;
		// line from center of mass of the face to the interior point
		Point com = (p[a] + p[b] + p[c])/3. - p[interior]; 	// center of mass
		// face normal of the surface element, which possibly needs to be flipped
		Point  n  = (p[b] - p[a]).cross(p[c] - p[a]);			// Face normal

// 		append Point to the list of points
		a = surface.append_point_uniquely(point(a));
		b = surface.append_point_uniquely(point(b));
		c = surface.append_point_uniquely(point(c));

		// depending on the dot product we exchange two vertices
		if ( n.dot(com) >= 0. )	surface.append_face(Face(a,b,c, roi));
		else					surface.append_face(Face(a,c,b, roi));
	}

	return surface;
}

list<int> SurfaceMesh::boundary_nodes(int region){

	return find_border_nodes(region);
}

/** name: SurfaceMesh::find_border_nodes()
 * (Stefan Fruhner - 22.11.2011 08:46:04 CET)
 * Searches for all nodes that define the border of a certain region.
 * \attention 	If no region ID is given, then the all nodes belonging to
 * 				different regions are reported. Nodes at the outer border of
 * 				the mesh are not reported since only one region attribute is
 * 				assigned to them. For these points another method must be applied.
 * @param region: Region ID of the region of interest
 * @return list of nodes
 **/
list<int> SurfaceMesh::find_border_nodes(int region){
	list<int> *plist = new list<int>[points()]; // list of all attributes for each point
	if (debug) cmdline::msg(" - Collecting attributes for each node ... ");
	Profiler profiler(elements());
	#pragma omp parallel for
	for (size_t i = 0; i < elements(); i++){ 			// check all elements
		for (vector<int>::iterator it = f[i].v.begin(); it != f[i].v.end(); it++){ // walk through the nodes of f[i]
			#pragma omp critical
			plist[*it].push_back(f[i].attribute);
		}
		if (debug) profiler.info();
	}

	if (debug) {
		profiler.finalize();
		cmdline::msg(" - Scanning for boundary nodes ... ");
	}
	profiler.reset(points());
	list<int> res;
	#pragma omp parallel for
	for (size_t i = 0; i < points() ; i++){
		// make every attribute in the lists unique
		plist[i].sort();
		plist[i].unique();
		if (debug) profiler.info();
		// check for correct region
		if (region > -1) {
			bool region_ok = false;
			for(list<int>::iterator it = plist[i].begin(); it != plist[i].end(); it++)
				if (*it == region) region_ok = true;
			if (not region_ok) continue;
		}
		// If a node belongs to at least 2 regions then this node is a boundary node
		#pragma omp critical
		{
			if (plist[i].size() > 1) res.push_back(i);
		}
	}
	delete[] plist;
	if (debug) profiler.finalize();

	// set the boundary markers of these points
	for (list<int>::iterator i = res.begin(); i != res.end(); ++i){
		p[*i].boundary(1);
	}

	//! \attention If (region == -1), then the outer nodes are missing. we add them now
	if (region == -1){
		switch (dim){
		case 2: {
				LineMap mymap = find_border_segments(region);
				for (LineMap::iterator it = mymap.begin(); it != mymap.end(); ++it){
					res.push_back(it->first.a);
					res.push_back(it->first.b);
				}
			break;
		}
		case 3: {
			set<int> reg; // create an empty set
			map< list<int> , pair<int,int> > mymap = compute_surface_i(reg);
			/** append all nodes found to the list
			 * \attention 	Since different surface elements share the same points
			 * 				the resulting list has non-unique entries.*/
			for (map< list<int> ,  pair<int,int> >::iterator it=mymap.begin() ; it != mymap.end(); it++ ){
				for (list<int>::const_iterator i = it->first.begin(); i != it->first.end(); ++i){
					res.push_back(*i);
				}
			}
			break;
		}
		default: throw myexception("dim != 2 && dim != 3 not implemented");
		}
		res.sort();
		res.unique();
	}

	return res;
}


myList<PointCurve> SurfaceMesh::find_border(){
	// Find all lines that are used only once in the faces
	LineMap mymap = find_border_segments(); //!< red black tree \attention only the first entry is used

	myList<PointCurve> borders = mymap.path_finder(p);	//! sort border curve by arc length
														//! \attention mymap will be erased
	print("   - #curves: " <<borders.length());
	for (size_t i = 0; i < borders.length(); i++){
		print("     - " << i <<" : #border segments: " << borders[i].length());
	}

	#ifdef DEBUG
		// print("Raender : " << raender.size());
		// plot "contour.dat" u 1:2:3:4 with vectors
		FILE *ou = fopen("contour.dat","w");
		for (size_t i = 0; i < borders.size(); i++){
			PointCurve &rand = borders[i];
			for (size_t i = 0; i < rand.length() ; i++){

				if (i < rand.length()-1)
					fprintf(ou, "%lf %lf %lf %lf\n", rand[i].x, rand[i].y, rand[i+1].x - rand[i].x, rand[i+1].y - rand[i].y);
			}
		}
		fclose(ou);
	#endif
	return borders;
}

 /** name: SurfaceMesh::compute_borders2D()
   * Determines all borders between regions for 2D meshes, sorts these by
   * arc-length and removes non unique entries in the list. The results are
   * written to SurfaceMesh::borders, which is a list of line segments.
   *
   * @param force_region: Compute only the border of a certain region with
   * 		label 'force_region'
   **/
void SurfaceMesh::compute_borders2D(int force_region){
	#ifdef DEBUG
	print(" - Computing borders for region "<< force_region);
	#endif
	if ( dim != 2 ) throw myexception("SurfaceMesh::compute_borders2D(): Cannot proceed, the mesh is not 2D !");

	borders.clear();
	scan_for_regions();

	for (size_t i = 0; i < regions.size(); i++){
		if ((force_region > -1) and (force_region != regions[i])) continue;
		LineMap mymap = find_border_segments(regions[i]); 	//!< red black tree \attention only the first entry is used
		myList<Curve> border = mymap.path_finder();			//! sort border curve by arc length
															//! \attention mymap will be erased
		borders.extend(border);
	}

	if ( borders.length() == 0 ) return;

	// remove borders which are equal
	for (int i = borders.size() - 1; i > -1  ; i--){	// iter through all borders
		for (int j = 0; j <(int) borders.size() ; j++){	// compare with all others
			if (i==j) continue;
			if (borders[i] == borders[j]){
				borders.erase(i); // erase
				break;
			}
		}
	}

	#ifdef DEBUG

	print(" - " << borders.size() << " borders found");

	FILE *ou = fopen("borders.dat","w");
		for (size_t i = 0; i < borders.size(); i++){
			Curve &rand = borders[i];
			for (size_t i = 0; i < rand.length() ; i++){
				if (i < rand.length()-1)
					fprintf(ou, "%lf %lf %lf %lf\n", p[rand[i]].x, p[rand[i]].y, p[rand[i+1]].x - p[rand[i]].x, p[rand[i+1]].y - p[rand[i]].y);
			}
			fprintf(ou, "\n\n");
	}
	fclose(ou);
	#endif

}

void SurfaceMesh::pseudo3d(double height, double resolution, int force_region){
	scan_for_regions();
	if ((regions.size() == 1) or (force_region > -1)){
		if (regions.size() == 1) force_region = regions[0];
		compute_borders2D(force_region);
		print("   - borders for region "<< force_region << " : " << borders.size());
		// create a copy of that slice which is moved by z - resolution
		int pts = points(); // number of points before pseudo3d
		for (int i = 0; i < pts; i++){
			Point pnt = p[i];
			pnt.z -= resolution/2.;	// translate point by resolution/2 in z-direction
			append_point(pnt);		// append point to list without changing the order
			p[i].z += resolution/2.;// translate original point by reolution/2. in the pos. direction
		}

		int nrele = elements();
		for (int i = 0; i <nrele; i++){
			Face fc = f[i];
			// add to all indices the number of points
			// before pseudo3d
			// (because order is not changed)
			for (size_t i = 0; i < fc.v.size() ; i++) fc.v[i] += pts;

//			interchange two points because the face normal of the backside should be flipped
			int tmp = fc.v[1];
			fc.v[1] = fc.v[2];
			fc.v[2] = tmp;

			append_face(fc);	// append face to list
		}

		// append new surfaces at the border
		for (size_t i = 0; i < borders.length(); i++){
			Curve &rand = borders[i];
			print("   - border # " << i << " length : "<< rand.length());
			int p1 = rand[0];
			int p2 = rand[1];
			for (size_t j = 1; j < rand.length() ; j++){
				append_face(Face(p1, p1 + pts, p2 + pts , force_region));
				append_face(Face(p1, p2 + pts, p2, force_region));
				p1 = p2;
				p2 = rand.next();
			}
		}
	}
	else{
		compute_borders2D();

		float modulus = fmod(height, resolution);
		cout << "modulus = fmod("<<height<<", "<<resolution<<" ) = " << modulus << endl;
		cout << "atan(resolution/(resolution+modulus)) = " << atan(resolution/(resolution+modulus)) << endl;
		if (atan(resolution/(resolution+modulus)) < 0.57594661666666666667){ // triangles showing angles smaller than 33 degrees would be produced
			cerr << "WARNING : angles smaller than 33° occur. Please reduce resolution !!" << endl;
		}

		// create a copy of that slice which is moved by z - resolution
		int pts = points(); // number of points before pseudo3d
		for (int i = 0; i < pts; i++){
			Point pnt = p[i];
			pnt.z -= (height)/2.;		// translate point
			append_point(pnt);			// append point to list without changing the order
			p[i].z += (height)/2.;	// translate original point by reolution/2. in the pos. direction
		}


		// append surfaces of the newly copied face
		int nrele = elements();
		for (int i = 0; i < nrele; i++){
			Face fc = f[i];
			// add to all indices the number of points
			// before pseudo3d
			// (because order is not changed)
			for (size_t i = 0; i < fc.v.size() ; i++) fc.v[i] += pts;

//			interchange two points because the face normal of the backside should be flipped
			int tmp = fc.v[1];
			fc.v[1] = fc.v[2];
			fc.v[2] = tmp;

			append_face(fc);	// append face to list
		}

		// append new surfaces at the borders
		for (size_t i = 0; i < borders.length(); i++){
			Curve &rand = borders[i];
			int p1 = rand[0];
			int p2 = rand[1];

			float mean = 0.;
			Curve::iterator next = ++rand.begin();
			for (Curve::iterator it = rand.begin(); next != rand.end(); it++, next++){
				mean += (p[*it] - p[*next]).abs();
			}
			mean /= rand.length()-1;
			cout << i << ": mean line segment length = " << mean << endl;

			int stack = height/mean; // how many z-slices should be added ?
			TELL(stack);
			TELL(height);
			switch (stack){
				case 0:
				case 1:{
					/** \todo Face normal depends on the direction we
					 *		  iterate along curve, but we don't know
					 * 		  the direction here !
					 **/
					for (size_t j = 1; j < rand.length() ; j++){
						append_face(Face(p1, p1 + pts, p2 + pts, i ));
						append_face(Face(p1, p2 + pts, p2, i));
						p1 = p2;
						p2 = rand.next();
					}
					break;
				}
				default: {
					for (int a = 1 ; a < stack; a++) {
						Point p1a = p[p1];
						p1a.z -= mean*a;
						append_point(p1a);
					}
					for (size_t j = 1; j < rand.length() ; j++){
						int save = points();

						for (int a = 1 ; a <= stack; a++) {
							if (a < stack) {
								Point p2a = p[p2];
								p2a.z -= mean*a;
								append_point(p2a); // index is p.size()-1

								if (a == 1){
									append_face(Face(p1, save-stack + a    , p.size()-1, i ));
									append_face(Face(p1, p.size()-1, p2        , i ));
								}
								else{
									append_face(Face(save-stack+a-1, save-stack+a, p.size()-1, i ));
									append_face(Face(save-stack+a-1, p.size()-1, p.size()-2, i ));
								}
							} else { //a == stack
									append_face(Face(save-stack+a-1, p1+pts, p2+pts, i ));
									append_face(Face(save-stack+a-1, p2+pts, p.size()-1, i ));
							}
						}
						p1 = p2;
						p2 = rand.next();
					}

					break;
				}
			}
		}
//		// append new surfaces at the borders
//		for (size_t i = 0; i < borders.length(); i++){
//			Curve &rand = borders[i];
//			print("   - border # " << i << " length : "<< rand.length());
//			int p1 = rand[0];
//			int p2 = rand[1];
//			for (size_t j = 1; j < rand.length() ; j++){
//				append_face(Face(p1, p1 + pts, p2 + pts, i ));
//				append_face(Face(p1, p2 +pts, p2, i));
//				p1 = p2;
//				p2 = rand.next();
//			}
//		}

		// correction of regions muss be done outside
		//for (size_t i = 0; i < region_def.size(); i++){
			//region_def[i].pos.z -= resolution / 2.;
			//// 5 tets fit into one cube
			//if (i == 1) region_def[i].constraint = (resolution * resolution * resolution) / 5.;
			//else region_def[i].constraint = 100. * (resolution * resolution * resolution) / 5.;
		//}

	}
}

/** SurfaceMesh:compute_neighbour_list()
 *
 * Finds all neighbours for all points and saves them in nb,
 * which is a vector of mylist. The vector index denotes the point index,
 * whereas the list contains all neighbouring points.
 * The neighbours are determined using the elements of the mesh -- faces in
 * two dimensions. Points that are connected by an edge are neighbours.
 */
void SurfaceMesh::compute_neighbour_list(){
	nb.clear();

	nbset tmp;					// constuct an empty set
	nb.assign(points(), tmp);	// allocate memory and append the empty set to every node

	// find all nb
	for (size_t i = 0; i < elements(); i++){
		Face &t = this->f[i];
		for (size_t j = 0; j < t.size(); j++){
		 for (size_t k = 0; k < t.size(); k++)
		  if (j != k) nb[t[j]].insert(t[k]); // for every voxel append each neighbour to the list
		}
	}

//	// delete identical elements
//	#pragma omp parallel for
//	for (size_t i = 0; i < points(); i++){
//		neighbours& list = nb[i];
//		list.sort();	// unique deletes only preceeding elements
//		list.unique();	// thats why the list must be sorted
//	}
	return;
}


/** SurfaceMesh::compute_tetlist()
 *
 * Sometimes it is of interest to which volume elements a node belongs. tetlist
 * is meant to save this information. To fill tetlist this function needs to
 * be called.
 **/
void SurfaceMesh::compute_tetlist(){
	// clean up everything
	this->tetlist.clear();

	neighbours tmp(0);
	tetlist.assign(this->p.size(), tmp);

	Profiler prof(elements());
	// find all nb
	for (size_t i = 0; i < elements(); i++){
		Face &t = this->f[i];
		// for every voxel append each the index of the face i
		for (size_t j = 0; j < t.size(); j++) tetlist[t[j]].push_back(i);
		if (debug) prof.info();
	}
	if (debug) prof.finalize();

	prof.reset(points());
	// delete identical elements
	#pragma omp parallel for
	for (size_t i = 0; i < points(); i++){
		neighbours &list = tetlist[i];
		list.sort();	// unique deletes only preceeding elements
		list.unique();	// thats why the list must be sorted
		if (debug) prof.info();
	}
	if (debug) prof.finalize();
}

void SurfaceMesh::compute_face_neighbour_list(){
	throw myexception(" Not yet implemented");
}

/** name SurfaceMesh::smooth_borders2D()
  *	Laplacian smoothing algorithm to smooth the boundaries of a 2D mesh
  * by moving border voxels to the centroids of their neighbours.
  */
void SurfaceMesh::smooth_borders2D(){
	compute_borders2D();
	print(" - Smoothing curves");
	for (size_t i = 0; i < borders.length(); i++){
		Curve border = borders[i];
		for (size_t j = 1;  j< border.length()-1; j++){
			Point &p1 = this->p[border[j-1]];
			Point &p2 = this->p[border[j  ]];
			Point &p3 = this->p[border[j+1]];
			p2 = (p1+p2+p3)/ 3.;
		}
	}
}


/** name: SurfaceMesh::SmoothSurfaceLaplacian()
  *	Simple Laplacian surface smoother. Computes mean value of location of each
  * point with respect to its neighbors.
  * @param num_iterations : how many iterations shall be done?
  **/
void SurfaceMesh::SmoothSurfaceLaplacian(size_t num_iterations){
	compute_neighbour_list(); // we need all neighbour relations
	size_t iteration = 0;
	vector<Point>o(p);	// original points
	do {
		for (size_t i = 0; i < points(); i++){
			size_t n = nb[i].size();
			if (n != 0) { // Laplacian operation
				p[i] = Point();
				 for (nbset::iterator it=nb[i].begin() ; it != nb[i].end(); it++ ){
					p[i] += o[*it];
				}
				p[i] *= 1./n;
			}
		}
		iteration++;
	} while (iteration < num_iterations);
}

/** SurfaceMesh::SmoothSurfaceLaplacianHC()
 * Smoothing algorithm according to
 * J. Vollmer, R. Mencl, H. Müller / Improved Laplacian Smoothing of Noisy
 * Surface Meshes (	Vollmer:ComputGraphicsForum:99 )
 * http://dx.doi.org/10.1111/1467-8659.00334
 *
 * \todo not yet tested
 * @param num_iterations: how many iterations
 * @param alpha : paramter alpha should be > 0
 * @param beta  : paramter beta  should be in [0:1]
 */
void SurfaceMesh::SmoothSurfaceLaplacianHC(size_t num_iterations, double alpha, double beta){
	compute_neighbour_list(); // we need all neighbour relations
	size_t iteration = 0;
	vector<Point>o(p);	// original points
	do {
		vector<Point>q(p);
		vector<Point>b(points(), Point());
		for (size_t i = 0; i < points(); i++){
			size_t n = nb[i].size();
			if (n != 0) { // Laplacian operation
				p[i] = Point();
				 for (nbset::iterator it=nb[i].begin() ; it != nb[i].end(); it++ )
					p[i] += q[*it];
				p[i] *= 1./n;
			}
			b[i] = p[i] - (alpha * o[i] + (1.-alpha)*q[i]);
		}
		for (size_t i = 0; i < points(); i++){
			size_t n = nb[i].size();
			if (n != 0){
				Point sum = Point();
				 for (nbset::iterator it=nb[i].begin() ; it != nb[i].end(); it++ )
					sum += b[*it];
				p[i] = p[i] - (beta * b[i] + (1.-beta)/n * sum);
			}
		}
		iteration++;
	} while (iteration < num_iterations);
}



/** name: SurfaceMesh::info
 *	Prints out information of the instance.
 */
void SurfaceMesh::info(const char* s) const{
	if (s)	cout << "\tInfo for " << s <<  endl;
	else 	cout << "\tInfo for SurfaceMesh " 			<< endl;
	cout << "\t - Nodes\t:\t"		<< p.size() 		<< endl;
	cout << "\t - Faces\t:\t"		<< f.size() 		<< endl;
	cout << "\t - Lines\t:\t"		<< l.size() 		<< endl;
	cout << "\t - Borders\t:\t"		<< borders.size() 	<< endl;
	cout << "\t - Region Def\t:\t"	<< region_def.size()<< endl;
	cout << "\t - Regions\t:\t"		<< regions.size() 	<< endl;
	cout << "\t - Dimension\t:\t"  	<< dimension() 		<< endl;
}

/**
 * SurfaceMesh::compute_min_max
 *
 * Compute the minimum and the maximum coordinate that is found in
 * the mesh. The points computed are not necessarily points in the mesh,
 * because the components of each point are considered seperately
 * \param min : Point that contains the minimum components at the end
 * \param max : Point that contains the maximum components at the end
 */
void SurfaceMesh::compute_min_max(Point &min, Point &max){
	cmdline::warning("The function SurfaceMesh::compute_min_max() is\
 deprecated. Use SurfaceMesh::bounding_box() instead.");
	min = p[0];
	max = p[0];
	for (size_t i = 1; i < points(); i++){
		min.x = (p[i].x < min.x) ? p[i].x : min.x;
		min.y = (p[i].y < min.y) ? p[i].y : min.y;
		min.z = (p[i].z < min.z) ? p[i].z : min.z;

		max.x = (p[i].x > max.x) ? p[i].x : max.x;
		max.y = (p[i].y > max.y) ? p[i].y : max.y;
		max.z = (p[i].z > max.z) ? p[i].z : max.z;
	}

}

/** name: SurfaceMesh::Volume
 * Computes the volume of a facet constructed from the points given by the user.
 *\param points: vector of points \attention Size of the vector must be 3 or 4 in this version.
 * \return The Volume as double value.
 **/
double SurfaceMesh::Volume(vector<Point> points){
	size_t items = points.size();
	switch (items){
	case 3: // compute the Area of a triangle
		return TriangleArea(points[0], points[1], points[2]);
		break;
	case 4:
		return TetrahedronVolume(points[0], points[1], points[2], points[3]);
		break;
	default:
		{
		string err = "Error Volume for that type of facet (";
		err += toString(items);
		err += " items) is not implemented";
		throw(myexception(err));
		}
	}
}

/** name: SurfaceMesh::Volume
 * Computes the volume of a facet constructed from the points given by te user.
 *\param points: vector of points \attention Size of the vector must be 3 or 4 in this version.
 * \return The Volume as double value.
 **/
double SurfaceMesh::Volume(vector<Point*> points){
	size_t items = points.size();
	switch (items){
	case 3: // compute the Area of a triangle
		return TriangleArea(*(points[0]), *(points[1]), *(points[2]));
		break;
	case 4:
		return TetrahedronVolume(*(points[0]), *(points[1]), *(points[2]), *(points[3]));
		break;
	default:
		throw myexception("Volume for that type of facet is not implemented.");
	}
}

/** name: SurfaceMesh::Volume
 * Computes the Volume of a certain facet from the list f.
 * \param facet: index of the facet in the list
 * \return Volume as double value
 **/
double SurfaceMesh::Volume(int facet){
	Face &t = this->f[facet]; // reference to the facet
	size_t items = t.size();
	switch (items){
	case 3: // compute the Area of a triangle
		return TriangleArea(p[t[0]], p[t[1]], p[t[2]]);
		break;
	case 4:
		return TetrahedronVolume(p[t[0]], p[t[1]], p[t[2]], p[t[3]]);
		break;
	default:
		throw myexception("Volume for that type of facet is not implemented.");
	}
}

/** name: SurfaceMesh::inside()
 * checks if a point is inside a facet, uses barycentric coordinates.
 * A point is inside, if all barycentric coords have the same sign.
 *
 * \todo Get rid of vector-classes. They slow down execution.
 * \param pt: point
 * \param facet : index of facet
 * \param D: 	Vector for barycentric coordinates, where D[0] will be the
 * 				volume of the tet that is used for the construction of coords.
 * 				\attention D is cleared in the function.
 * \param tol:	Due to round off errors 0. is not exactly 0., but a small
 * 				number. tol increases the threshold for small numbers that
 * 				are processed as if the were exactly 0.
 * \return true if inside
 */
bool SurfaceMesh::inside(const Point &pt, const int &facet, vector<double> &D, double tol) {
	Point needle(pt);
	if (dim < 3 ) needle.z = 0.; // if the mesh is only 2D
	if (dim < 2 ) needle.y = 0.; // if the mesh is only 1D

	Face &t = this->f[facet];
	D.clear();
	D.assign(t.size()+1, 0.); // num of edge points + 1 D[0] => volume

	// Volume computation
	D[0] = this->Volume(facet);
	if (D[0] == 0.)	{
		cerr << "\n"<< facet << " " << D[0]<< endl;
		throw myexception("Error: Volume equal to zero occured. Degenerated facet ?");
	}

	// initialize pts vector that is used to compute barycentric coords
	vector<Point*> pts(t.size());
	for (size_t i = 0; i < t.size(); i++) pts[i] = &(p[t[i]]); // list all addresses

	// compute barycentric coordinates subvolumes
	for (size_t i = 1; i <= t.size(); i++){
		pts[i-1] = (Point*) &needle;				// exchange last point by the point to be checked
		if (i > 1) pts[i-2] = &(p[t[i-2]]);			// re-exchange with original point
		D[i] = Volume(pts);							// compute barycentric volume
	}

	bool res = true;
	// check sign
	// Point is inside, if all barycentric coords have the same	 sign
	float sign_p = (D[0] > 0.) ? +1.: -1.; // sign of the volume
	float sign_n = +1.;
	for (size_t i = 1; i < D.size(); i++){
		// If one coordinate is zero it is okay. This means the point lies on
		// the edge of the facet.
		if (fabs(D[i]) < tol) continue; 	// D[i] is close to zero, fixed round off errors
		sign_n = (D[i] > 0.) ? +1.: -1.;	// Sign of the component
		if (sign_n != sign_p) {res = false; break;}
		sign_p = sign_n;
	}
	// rescale barycentric coords using the volume D[0]
	double inv = 1./D[0];
	for (size_t i = 1; i < D.size(); i++) D[i] *= inv;

	return res;
}


/** name: SurfaceMesh::inside
 * Checks if a point is inside of one of the object's elements
 * uses barycentric coordinates
 *
 * Depending if f[i].v has 3 or 4 elements it is tested if the point is
 * contained in the volume (=4) or in the surface (=3) of the mesh.
 * \param pt: point
 * \param D: vector for barycentric ccordinates
 * \param tol : Tolerance value, e.g. a small number > 0.
 * \return true if inside
 */
bool SurfaceMesh::inside(const Point &pt, vector<double> &D, double tol) {
	for (size_t i = 0; i < this->elements(); i++){
		if (this->inside(pt, i, D, tol)) return true;
	}
	return false;
}

/** name: SurfaceMesh::find_element()
 * (Stefan Fruhner - 06.12.2011 09:51:47 CET)
 * Finds the element that contains the point given as first argument.
 * Does basically the same as SurfaceMesh::inside(), but returns the
 * element ID.
 * \param pt: Point to serach the appropriate element for.
 * \param D : vector for barycentric ccordinates
 * \param tol : Tolerance value, e.g. a small number > 0.
 * @return An element ID if valid, otherwise maximum value of size_t
 **/
size_t SurfaceMesh::find_element(const Point &pt, vector<double> &D, double tol){
	for (size_t i = 0; i < this->elements(); i++){
		if (this->inside(pt, i, D, tol)) return i;
	}
	return std::numeric_limits<std::size_t>::max();
}

/** name: SurfaceMesh::find_closest_element()
 * (Stefan Fruhner - 06.12.2011 09:51:47 CET)
 *	Find the clostest element for a given point, by computing all barycentric
 * 	coordinates  and searching for the set of coordinates that fulfills
 * \f$ \sum_(i=1)^n D[i] * D[i] == min \f$
 *
 *	\note	If one element was found that contains the point, then we can
 *			return this tet.
 *
 *  \attention  This function always returns an element even if this is in a
 * 				very far distance.
 *
 * @param pt: Point to search the appropriate element for.
 * \param D : vector for barycentric ccordinates
 * \param tol : Tolerance value, e.g. a small number > 0.
 * @return An element ID.
**/
size_t SurfaceMesh::find_closest_element(const Point &pt, vector<double> &D, double tol){

	double abs = std::numeric_limits<double>::max();
	size_t min = 0;
	vector<double> tD;

	for (size_t i = 0; i < this->elements(); i++){
		if ( this->inside(pt, i, tD, tol) == true ){
			D.swap(tD); // save the results in D
			return i;
		} else {
			double tabs = 0.;
			for (size_t j = 1; j < tD.size(); j++) tabs += tD[j]*tD[j];

			if (tabs < abs) {
				abs = tabs;
				min = i;
				D.swap(tD);		// save the results in D
			}
		}
	}
	return min;
}

 /** name: SurfaceMesh::find_closest_element()
 * (Stefan Fruhner - 06.12.2011 09:51:47 CET)
 * Brute force search for the clostest element in the mesh.
 * @param pt: Point to serach the appropriate element for.
 * @return An element ID.
 **/
size_t SurfaceMesh::find_closest_element(const Point &pt){
	double abs = (pt - centroid(0)).abs();
	size_t min = 0;

	for (size_t i = 0; i < this->elements(); i++){
		Point c = centroid(i);
		double tabs = (pt - c).abs();
		if (tabs < abs) {abs = tabs; min = i;}
	}
	return min;
}

/**
 * name: SurfaceMesh::find_closest_node()
 *
 * Search for the clostest node in the mesh for a given test point.
 * The search operation is provided by the class
 * NeighbourSearch::NSearch which operates much faster than a normal
 * linear search.
 *
 * @param pt : The point we are interested in.
 * @return Index of the clostest node in the vector SurfaceMesh::p.
 *
 */
size_t SurfaceMesh::find_closest_node(Point &pt){

	if (! pSearch )
		pSearch = new NeighbourSearch::NSearch(&(p[0]),p.size());

	Point *P = pSearch->find_neighbour(pt);

	ptrdiff_t diff = P - &(this->p[0]);

	return (size_t) diff;
}

/** name: SurfaceMesh::intersection()
 *	Computes all intersections of the boundary with a line gives as parameter.
 * @param line: must be a line, a segment or a ray.
 * @return A list of points (list<Point>) that intersect the boundary of the mesh.
 **/
list<Point> SurfaceMesh::intersection(Line line) const{
	return boundary_intersections(line);
}

/** name: SurfaceMesh::intersection()
 * Computes all intersections of the boundary with a line gives as parameter.
 * @param line: must be a line, a segment or a ray.
 * @return A list of points (list<Point>) that intersect the boundary of the mesh.
 **/
list<Point> SurfaceMesh::boundary_intersections(Line line) const{
	list<Point> list;
	switch (dim){
		case 2: {
				if (lines() == 0) 	throw myexception("No boundaries computed yet");
				line.p1().z = 0.; // for 2d meshes we set all z-coordinates to 0.
				line.p2().z = 0.;
				Point pt;
				for (size_t i = 0; i < lines(); i++){
					Line segment(p[l[i].a], p[l[i].b], Line::segment); // create a Line
					if (segment.intersection(line, pt)) list.push_back(pt);
				}
			}
			break;
		case 3: {
				Point intersec;
				for (size_t i = 0; i < elements(); i++){
					Facet f = facet(i);
					if ( f.intersection(line, intersec) ) list.push_back(intersec);
				}
			}
			break;

		default: throw myexception_UnknownError("dim should be 2 or 3 !");
	}

	return list;
}

/** name: SurfaceMesh::barycentric_coordinates
 * Computes the barycentric coordinates of a facet/ volume of the mesh for a
 * given point. In fact this function does the same as SurfaceMesh::inside(),
 * but it is useful for readability of the code to have a function that does
 * what it says. It is not always of interest, if a point is inside a volume or
 * not since interpolation works in both cases.
 *
 * \param pt: point
 * \param facet : index of facet/ volume
 * \return 	vector<double> containing the coordinates. Please note that D[0]
 * 			always is used to save the volume of the element.
 */
vector<double> SurfaceMesh::barycentric_coordinates(const Point &pt, const int &facet){
	vector<double> D;
	this->inside(pt, facet, D, 0.);
	return D;
}

void SurfaceMesh::fprintf_barycentric_coordinates(FILE *f, vector<double> D) const {
	switch (D.size()){
	case 4:
		fprintf(f,"%1.3f %1.3f %1.3f %1.3f\n", D[0], D[1], D[2], D[3]);
		break;
	case 5:
		fprintf(f,"%1.3f %1.3f %1.3f %1.3f %1.3f\n", D[0], D[1], D[2], D[3], D[4]);
		break;
	default: cerr <<  "Strange ... not implemented" << endl;
	}
}


/** name: SurfaceMesh::centroid()
 * @param ele_idx : index of an element (aka face/triangle in 2D)
 * @return centroid of this face/ tetrahedron
 **/
Point SurfaceMesh::centroid(size_t ele_idx){
	if (ele_idx >= elements()) throw myexception("Out-of-range error.");
	Point c(0.,0.,0.);
	for (size_t i = 0; i < f[ele_idx].v.size(); i++){
		c += p[f[ele_idx].v[i]];
	}
	c /= f[ele_idx].v.size();
	return c;
}

/** name: SurfaceMesh::centroids()
 * Compute all centroids of the mesh's faces/ volumes.
 *
 * \attention 	It is up to the user to delete the memory allocated by this
 * 				function.
 * @return Pointer to the array. The length is equal to the length of the list
 * of faces/volumes
 **/
Point* SurfaceMesh::centroids(){
	Point *c = new Point[elements()];
	for (size_t i = 0; i < elements() ; i++) c[i] = centroid(i);
	return c;
}

/** \fn Point SurfaceMesh::center_of_mass() const
 * Compute the center of mass of all vertices.
 *
 * @return Point which is the center of mass.
 **/
Point SurfaceMesh::center_of_mass() const{
	Point com;
	for (vector<Point>::const_iterator it = p.begin(); it != p.end(); ++it){
		com += *it;
	}
	com /= points();
	return com;
}

//! \todo Complete this function
//double SurfaceMesh::mean_edge_length(){

//	for (int i = 0; i < elements(); i++){
//		size_t edges = f[i].v.size() *  (f[i].v.size() - 1);
//		for (int j = 0; j < f[i].v.size; j++){
//			for (int k = 0; k < f[i].v.size; k++){
////				f[i].
//			}
//		}
//	}
//}


/** \fn void SurfaceMesh::interpolate_deformation(vector<Point> &, vector<Point> &)
* (Stefan Fruhner - 02.04.2012 17:59:48 CEST)
* Assuming 'this' is a deformation mesh (triangulation with deformation
* vectors as attributes) we compute the interpolation of the deformation
* direction for each node of the array 'nodes'.
* @param def   : Computed deformation vects. Number equals the number
* 				 of nodes the user supplies.
* @param nodes : Points to be moved along deformation vects.
**/
void SurfaceMesh::interpolate_deformation(vector<Point> &def, vector<Point> &nodes, Profiler &prof) {
//	bool interpolated = false;
	if ( attributes() < 3 ){
		throw myexception("Expecting that defomation mesh has 3\
double valued attributes denoting dx,dy and dz.");
	}

	def.clear(); 						// always clear the results before computing new ones
	def.resize(nodes.size(),Point()); 	// initialize with the correct size
	size_t i = 0;

	#pragma omp parallel for
	for (i=0; i < nodes.size(); ++i){
//	for (vector<Point>::const_iterator pt = nodes.begin(); pt != nodes.end(); ++pt, cpt++){
		vector<double> B;
		/** \attention Here we create an interpolation for every node,
		 * 	 		   assuming the deformation mesh has a valid result
		 * 	 		   for every node.
		 **/
		size_t elm = find_closest_element(nodes[i],B,0.);
		Face &face = f[elm];
		// move to the right position for the actual state
		// the attributes are assumed to be deformation vectors
		Point interp(0.,0.,0.);
		for (int k = 0; k < (int) face.v.size(); k++){
			def[i].x += B[k+1] * attribute(face.v[k],0);
			def[i].y += B[k+1] * attribute(face.v[k],1);
			def[i].z += B[k+1] * attribute(face.v[k],2);
		}
		prof.info();
	}
}


void SurfaceMesh::Deformation::initInterpolation(const size_t source_index, const size_t target_index){
	this->source = source_index;
	this->target = target_index;
}



/** \fn void SurfaceMesh::Deformation::interpolate_deformation_vectors_bwd(vector<Point> &, const vector<Point> &)
* (Stefan Fruhner - 24.04.2012 11:34:22 CEST)
* Given a mesh and its deformation vectors for some control points it is an
* _interative_ procedure to compute the deformation vectors from mesh A to
* mesh B for each node of the mesh.
* @param vectors  : The output will be written to vectors.
* @param pts      : vector<Point> that contains all points to be interpolated.
**/
void SurfaceMesh::Deformation::interpolate_deformation_vectors_bwd(vector<Point> &vectors, const vector<Point> &pts){
	vectors.clear();

	if (this->source == 0 and this->target == 0) throw myexception(\
 toString(__FUNCTION__) + " : Attention : you have to set source and\
 target indices before doing an interpolation !");

	if (this->source == this->target) return; // no interpolation needed

	// create a copy of all nodes, that shall be interpolated
	vector<Point> nodes;
	vector<Point> vects;
	nodes.assign(pts.begin(), pts.end());
	vectors.assign(pts.size(), Point()); // one deformation vector per node initialized with (0.,0.,0.)

	size_t max = this->deform_bwd.size();
	Profiler profiler(diff_bwd*pts.size());
	for (size_t i = 0; i < max ; ++i){
		size_t v = (max + this->source-i) % max;
		if (v == this->target) break;
		// interpolate deformation vects
		this->deform_bwd[v]->interpolate_deformation(vects, nodes, profiler);

		// update
		for(size_t j = 0; j < nodes.size(); ++j) {
			// the nodes
			nodes[j] += vects[j];
			// ... and deformation vectors
			vectors[j] += vects[j];
		}
	}
	profiler.finalize();

	initInterpolation(); // reset source and target mesh
	return;
}

void SurfaceMesh::Deformation::interpolate_deformation_vectors_fwd(vector<Point> &vectors, const vector<Point> &pts){

	vectors.clear(); // overall deformation vectors will be computed by this method

	if (this->source == 0 and this->target == 0) throw myexception(\
 toString(__FUNCTION__) + " : Attention : you have to set source and\
 target indices before doing an interpolation !");

	if (this->source == this->target) return; // no interpolation needed

	// create a copy of all nodes, that shall be interpolated
	vector<Point> nodes;		// temporary points for stepwise interpolation, only for internal use
	vector<Point> vects; 		// deformation vectors of consecutive interpolation steps
	nodes.assign(pts.begin(), pts.end());
	vectors.assign(pts.size(), Point()); // one deformation vector per node initialized with (0.,0.,0.)

	size_t max = this->deform_fwd.size();
	Profiler profiler(diff_fwd * pts.size());
	for (size_t i = 0; i < max ; ++i){
		size_t v = (max + this->source+i) % max;
		if (v == this->target) break;
		// interpolate deformation vects
		this->deform_fwd[v]->interpolate_deformation(vects, nodes, profiler);

		// update
		for(size_t j = 0; j < nodes.size(); ++j) {
			// the nodes
			nodes[j] += vects[j];
			// ... and deformation vectors
			vectors[j] += vects[j];
		}
	}
	profiler.finalize();

	initInterpolation(); // reset source and target mesh
	return;
}

void SurfaceMesh::Deformation::interpolate_deformation_vectors(vector<Point> &vectors, const vector<Point> &pts){

	if (this->source == 0 and this->target == 0) throw myexception(\
 toString(__FUNCTION__) + " : Attention : you have to set source and\
 target indices before doing an interpolation !");

	if (this->deform_bwd.size() != this->deform_fwd.size())
		throw myexception(\
 toString(__FUNCTION__) + " : Attention : Forward and backward deformation\
 meshes are not of equal size !");

	if (this->source == this->target) return; // no interpolation needed

	this->diff_fwd = 0;
	this->diff_bwd = 0;
	size_t max = deform_fwd.size();


	switch (source <= target){
	case true  :
					diff_fwd = target - source;
					diff_bwd = source + max - target;
					break;
	case false :
					diff_fwd = max - source + target;
					diff_bwd = source-target;
					break;
	}

	cout << "diff_fwd: " << diff_fwd << endl;
	cout << "diff_bwd: " << diff_bwd << endl;

	switch ( diff_fwd <= diff_bwd ) {
	case true:
		cout << "computing forward deformation vectors" << endl;
		interpolate_deformation_vectors_fwd(vectors, pts);
		break;
	case false:
		cout << "computing backward deformation vectors" << endl;
		interpolate_deformation_vectors_bwd(vectors, pts);
		break;
	}
	return;
}

bool SurfaceMesh::Deformation::inside(const Point &pt, const size_t &index){
	vector<double> D;
	return deform_fwd[index]->inside(pt, D, 0.);
}
