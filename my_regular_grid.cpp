
#include "my_regular_grid.hpp"

//#define DEBUG
#ifndef mrg_print
#define mrg_print(x) (cout << __FILE__ << ":" << __FUNCTION__ <<"() (" <<__LINE__ << ") : " << x << endl)
#endif

#define absolute(a) ((a >= (Type)0.) ? (a) : (-a))

#define NX dimensions[0]
#define NY dimensions[1]
#define NZ dimensions[2]
#define NT dimensions[3]

my_regular_grid::my_regular_grid(int dimx, int dimy, int dimz, int dimt){
	init_dimension(dimx, dimy, dimz, dimt);
}

void my_regular_grid::init_dimension(const int dimx, const int dimy, const int dimz,const int dimt){
	dimensions[0] = (dimx < 1) ? 1:dimx; //! dimension value less than 1 makes no sense
	dimensions[1] = (dimy < 1) ? 1:dimy;
	dimensions[2] = (dimz < 1) ? 1:dimz;
	dimensions[3] = (dimt < 1) ? 1:dimt;

	for (int i = 0; i < 4; i++) v_pixdim[i] = 0.;
	for (int i = 0; i < 4; i++) v_origin[i] = 0.;
	pixdim_set = false;

	set_dimension_data();
}

/** set_dimension_data
 * Sets all values needed to determine the dimensionality, as number of items in
 * the file, the number of slices, where slice means the last dimension, which
 * can be time or the third spatial dimension. The size of one slice is also
 * set.
 *
 * \return Nothing.
 **/
void my_regular_grid::set_dimension_data(){
	#ifdef DEBUG
		mrg_print("{");
		mrg_print("OLD dimension  :  " << dimension_val);
		mrg_print("OLD item number: " << items_val);
		mrg_print("OLD slice size : " << slice_size_val);

		mrg_print("Setting matrix with sizes");
		cout << "[";
		for (int i = 0; i < 4; i++)
			cout << dimensions[i] << " ";
		cout << "]" << endl;
	#endif

	dimension_val = 1;
	items_val  = dimensions[0];
	slice_size_val = items_val;

	for (int i = 3; i > -1; i--)// determine the number of slices (last valid entry in dims)
		if (dimensions[i] > 1) {
			nr_slices = dimensions[i];
			dimension_val = i+1;
			break;
		}

	items_val = 1;  // compute number of items
	for (int i = 0; i < 4; i++) items_val *= dimensions[i];

	slice_size_val = 1;  // compute number of items
	for (int i = 0; i < dimension_val-1; i++) slice_size_val *= dimensions[i];

//			for (int i = 1; i < 4; i++){ // determine the number of slices (last valid entry in dims)
//				if (dimensions[i] == 1){ // the number of slices is given by the number of items in the last dimension (mostly called `z` or `t`)
//					nr_slices = dimensions[i-1];
//					break;
//				}
//				slice_size_val = items_val;		 // size of each slice
//				items_val *= dimensions[i]; 	 // number of items in the matrix
//				dimension_val ++;			 // dimension of the problem
//			}

	//! compute the distances that are needed to find the direct neighbours of an item.
	dx = index(1,0,0,0);
	dy = index(0,1,0,0);
	dz = index(0,0,1,0);
	dt = index(0,0,0,1);

	#ifdef DEBUG
		mrg_print("New dimension  :  " << dimension_val);
		mrg_print("New item number: " << items_val);
		mrg_print("New slice size : " << slice_size_val);
		mrg_print("}");
	#endif
}

void my_regular_grid::dims(const size_t i,const size_t j,const size_t k,const size_t l){				// set the size of dimensions
	dimensions[0] = (i < 1) ? 1: i; //! dimension value less than 1 makes no sense
	dimensions[1] = (j < 1) ? 1: j; //! and MUST be forbidden (see set_dimension_data()
	dimensions[2] = (k < 1) ? 1: k;
	dimensions[3] = (l < 1) ? 1: l;
//			init_dimension();
	set_dimension_data();
}

/** name: my_regular_grid::d()
 * Because the data is organized in a 1D-array but we can have 4D data,
 * one needs to know, how many items lie between two neighbouring pixels
 * in x,y,z and t direction.
 * \param i : [0,1,2,3] for [x,y,z,t]
 * \return Index-difference in the array.
 */
size_t my_regular_grid::d(short i){	//!< get the index-distance in the direction i
	switch (i){
		case 0: return dx; break;
		case 1: return dy; break;
		case 2: return dz; break;
		case 3: return dt; break;
	}
	return 0;
}

Point my_regular_grid::coords(size_t x,size_t y, size_t z, size_t t){
														// compute image coordinates for a set of
	return Point(	origin(0) + x * pixdim(0),			// indices and return a point
					origin(1) + y * pixdim(1),
					origin(2) + z * pixdim(2),
					origin(3) + t * pixdim(3));
}

Point my_regular_grid::coords_bottom_right(){							// Point at the bottom right corner of the image
	Point pt = coords(NX-1,NY-1,NZ-1,NT-1); // last voxel
	Point pix = pixdim(); // but we need to add the last one to get the border of the cube
	return pt + pix;
}

Point my_regular_grid::coords_top_left(){								// Point at the top left corner of the image
	return coords(0,0,0,0);
}

/**
 * name: my_regular_grid::CoordsToIndex()
 * Computes the index to a given set of coordinates.
 * Throws an exception if the result is out of range.
 * \attention at least the pixel dimensions have to be set before this
 * function is called.
 * \param coords: Point with some coords
 * \return The index in the array.
 */
size_t my_regular_grid::CoordsToIndex(Point coords){
	//if (not pixdim_set) throw mymatrix_exception(PIXDIM_FAILURE);
	if (not pixdim_set) throw Exception_InitializationError(EXCEPTION_ID+ "pixdim must be set.");
	//! \attention The value must have the same unit as pixdim and origin
	// Step 1) remove the orignin from coordinates
	coords.x -= origin(0);
	coords.y -= origin(1);
	coords.z -= origin(2);
	coords.t -= origin(3);

	//Step 2) compute the indices
	size_t i=0, j=0, k=0, l=0;
	i = ((pixdim(0) != 0.) ? (size_t) (coords.x/pixdim(0)) : 0);
	j = ((pixdim(1) != 0.) ? (size_t) (coords.y/pixdim(1)) : 0);
	k = ((pixdim(2) != 0.) ? (size_t) (coords.z/pixdim(2)) : 0);
	l = ((pixdim(3) != 0.) ? (size_t) (coords.t/pixdim(3)) : 0);
	//coords.prnt();
	//print(i << " " << j << " " << k << " "<< l);
	//Step 3) Check all indices
	string collect = "[";
	if ( i > dims(0) ) collect += "x" ;
	if ( j > dims(1) ) collect += "y" ;
	if ( k > dims(2) ) collect += "z" ;
	if ( l > dims(3) ) collect += "t" ;
	collect += "]";

	if (collect.size() > 2) {
		throw my_regular_grid::Exception_ConversionError(EXCEPTION_ID \
		+ "Error: Index out of range " + collect + ".");
	}
	//step 4 ) compute the matrix index
	return index(i,j,k,l);
}

void my_regular_grid::info(){
	for (int i = 0; i < 4; i++)
		cout << "dimensions["<<i<<"] :\t" << this->dimensions[i] << endl;
	cout << "slice_size    :\t" << this->slice_size_val << endl;
	cout << "nr_slices     :\t" << this->nr_slices      << endl;
	cout << "items         :\t" << this->items_val      << endl;
	cout << "dimensions    :\t" << this->dimension_val  << endl;
	for (int i = 0; i < 4; i++)
		cout << "pixdim["<<i<<"]     :\t" << this->v_pixdim[i] << endl;
	for (int i = 0; i < 4; i++)
		cout << "origin["<<i<<"]     :\t" << this->v_origin[i] << endl;
	cout << "pixdim_set    :\t" << this->pixdim_set     << endl;
}

bool my_regular_grid::operator==(my_regular_grid &other){
	for (int i = 0; i < 4; i++){
		if (this->dims(i) != other.dims(i) ) 		return false;
		if (this->pixdim(i) != other.pixdim(i) ) 	return false;
		if (this->origin(i) != other.origin(i) ) 	return false;
	}
	return true;
}

bool my_regular_grid::operator!=(my_regular_grid &other){
	for (int i = 0; i < 4; i++){
		if (this->dims(i) != other.dims(i) ) 		return true;
		if (this->pixdim(i) != other.pixdim(i) ) 	return true;
		if (this->origin(i) != other.origin(i) ) 	return true;
	}
	return false;
}

/** index
 * The data is stored for 1 to 4 dimensions. To keep the code general
 * the arrays used should be only one-dimensional. This function `index`
 * returns an one-dimensional field index for a four-dimensional
 * description which is more intuitive.
 *
 * \attention 	In the gipl format it is important to notice that the
 * 				data is stored a reversed order. The data is organized
 * 				as a stack where x varies fastest, then y, z and t.
 * 				An advantage is that one can slice the data easily.
 *
 * \param i: index in x -directions
 * \param j: y-index
 * \param k: z-index
 * \param l: t-index
 *
 * \return 1-D field index as size_t
 */
size_t my_regular_grid::index(size_t i,size_t j, size_t k, size_t l){
	size_t* d = dimensions;
	bool index_ok = ((i <= d[0]) && (j <= d[1])&& (k <= d[2])&& (l <= d[3]));

	if (index_ok)
	switch (dimension_val){
		case 1: return												   i;
				break;
		case 2: return 									    j * d[0] + i;
				break;
		case 3: return 					    k * d[1]*d[0] + j * d[0] + i;
				break;
		case 4: return l * d[2]*d[1]*d[0] + k * d[1]*d[0] + j * d[0] + i;
				break;
	}
	else return 0;

	return 0;
}


///** name: mymatrix::index_1to4
// * 	Computes x,y,z and t for a 1d index.
// * \param index: 1d index
// * \param x: variable for x-index
// * \param y: variable for y-index
// * \param z: variable for z-index
// * \param t: variable for t-index
// */
//void my_regular_grid::index_1to4(unsigned int index, unsigned int &x, unsigned int &y, unsigned int&z, unsigned int &t){
//	unsigned int rest = index;
//	t = rest/(NZ*NY*NX);
//	rest = rest % (NZ*NY*NX);

//	z = rest/(NY*NX);
//	rest = rest % (NY*NX);

//	y = rest/(NX);
//	x = rest % (NX);
//}

void my_regular_grid::index_1to4(size_t index, size_t &x, size_t &y, size_t&z, size_t &t){
	size_t rest = index;
	t = rest/(NZ*NY*NX);
	rest = rest % (NZ*NY*NX);

	z = rest/(NY*NX);
	rest = rest % (NY*NX);

	y = rest/(NX);
	x = rest % (NX);
}

#undef NX
#undef NY
#undef NZ
#undef NT
#undef mrg_print
#undef absolute
