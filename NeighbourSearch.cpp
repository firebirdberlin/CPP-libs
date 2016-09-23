//		./NeighbourSearch.cpp
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
/** \page mylibs
 * \section sec_NeighbourSearch NeighbourSearch
 * \subsection files Files
 * NeighbourSearch.h, NeighbourSearch.c, lists.h, lists.c
 * \section description Description
 * Enhanced Neighbour Search: Searching for nearest neighbour by dividing the
 * space into subspaces. See function comments for more details.
 *
 * \subsection changes Changes
 * - Feb. 08
 *  - first code, designed as multi-purpose library
 * - 12.06.2008
 *  - completed some function descriptions
 *  - find_nearest_neighbour: abort, when the distance
 *      to the first found particle is already 0.0
 *      smaller is not possible.
 *      (SnakeOutDirected was very slow in that case)
 * - 28.09.2009
 *  - C++ version, only one h-file, so that the project can be included in my
 *    template library
 *
 * \subsection example Example
 * see mymesh.h: convert_to_my_matrix() for using instructions, or read my comments
 **/

#include "NeighbourSearch.hpp"
#ifdef __cplusplus
namespace mylibs {
namespace NeighbourSearch{
#endif
double randomize(int max){
	return (double)rand()/(double)RAND_MAX * 2. * max - max;
}

/** random_points
    fills an array with randomly distributed points
 * \param a: Array of points is filled with random values for x, y, z
 * \param len: number of points in the array
 * \return 0
 **/
int random_points(Point *a, int len){
	int i;

	for(i = 0; i < len; i++) {
		a[i].x = randomize(DIV);
		a[i].y = randomize(DIV);
		a[i].z = randomize(DIV);
	}
	return 0;
}

/** copyPoint
    copies an element of the type Point into another
 * \param to: the target
 * \param from the source
 * \return void
 **/
void copyPoint(Point *to, Point *from){
	to->x = from->x;
	to->y = from->y;
	to->z = from->z;
}

/** distance
 *  computes the distance between two points
 * \param p1: pointer to a point
 * \param p2: pointer to a point
 * \return value (distance)
 **/

double distance(Point *p1, Point *p2){
	double x,y,z;
	x = p1->x - p2->x;
	y = p1->y - p2->y;
	z = p1->z - p2->z;
	return sqrt(x*x + y*y + z*z);
}

/** findBoxbyIndex
 * For the three dimensional indices i,j and k the one-dim.- index is
 * computed. The adress of the box is returned.
 * \param i: index i
 * \param j: index j
 * \param k: index k
 * \param cont: The nb-search object
 * \return *Box: pointer to the Box with the index (i,j,k)
 **/
Box *findBoxbyIndex(int i,int j, int k , NeighbourSearch* cont){
	int c;
	c = INDEX(i,j,k,cont->div);
	if ((c > -1) && (c < cont->numBox))
		return &(cont->boxes[c]);
	//else fprintf(stderr, "debug findBoxbyIndex() : %d %d %d => %d\n", i,j,k,c);

	return NULL;	//index out of range
}

/**  findBox
 * For one given point this function computes, the right box index for
 * it and returns the address of the box.
 * \param p: a point
 * \param *cont: The nb-search object
 * \return *Box: pointer to the Box which contains the point p.
 **/
Box *findBox(Point* p, NeighbourSearch* cont){
	int x, y, z;

	//get the indices of the boxes
	x = (int) floor((p->x + fabs(cont->min))/cont->boxsize);
	y = (int) floor((p->y + fabs(cont->min))/cont->boxsize);
	z = (int) floor((p->z + fabs(cont->min))/cont->boxsize);

	// \attention this case should never happen
//	if ((x < 0) || (y < 0) || (z < 0)) {
//		fprintf(stderr,"error: This case should not happen, if the Search-algorithm was\n
//       initialized correctly. Be sure to call the function \n
//       NeighbourSearch_Create with two arrays, containing the points to be \n
//       searched in and additionally the points we are searching the \n
//       neighbours for.\n");
//		fprintf(stderr, "       %d=%f\t%d=%f\t%d=%f\n",   x, p->x,y, p->y,z, p->z);
//		fprintf(stderr, "       %d=%f\t%d=%f\t%d=%f\n",	  x, floor((p->x + fabs(cont->min))/cont->boxsize),
//														  y, floor((p->y + fabs(cont->min))/cont->boxsize),
//														  z, floor((p->z + fabs(cont->min))/cont->boxsize));
//	}

	//! If the index is equal to the number of boxes in one direction,
	//! then the point must be on the border of the last box (or even
	//! outside).
	if (x == cont->div) x--; else if (x < 0) x = 0;
	if (y == cont->div) y--; else if (y < 0) y = 0;
	if (z == cont->div) z--; else if (z < 0) z = 0;
	//compute the right one-dimensional index
	return findBoxbyIndex(x,y,z,cont);
}

/**  NeighbourSearch_Create
 build NeighbourSearch initializes the objects needed for computation. It
 has to be called before a nearest neighbour search can be performed.
 The parameters needed are:
 \param items	 : 	An array of the type point. In all these points can
					be searched.
 \param len	 : 	The size of the array above.
 \param items2	 : 	Additional items that contain other points: important
					to get the maximum sizes
 \param len2	 : 	The size of this array
 \param number	 : 	Number of boxes in 1 dimension (defines how many
					sub-boxes are created)
 \return 			NeighbourSearch object
 **/
NeighbourSearch *NeighbourSearch_Create(Point *items, int len,Point *items2, int len2, int number){
	int i=0;
	int j=0;
	int k=0;
	NeighbourSearch *cont = NULL;
	#ifdef __cplusplus
		cont = new NeighbourSearch;
		cont->boxes = NULL;
	#else
		cont =(NeighbourSearch * ) malloc(sizeof(NeighbourSearch));	// get memory
	#endif

	if (cont == NULL) {
		fprintf(stderr,"Out of memory\n");
		return NULL;
	}
	cont->div = number;

	cont->min = items[0].x; // initialize with first value
	cont->max = items[0].x; // initialize with first value
	//get the minima of the NeighbourSearch
	for (i=0; i< len; i++){
			cont->max = MAX(cont->max , items[i].x);
			cont->max = MAX(cont->max , items[i].y);
			cont->max = MAX(cont->max , items[i].z);

			cont->min = MIN(cont->min , items[i].x);
			cont->min = MIN(cont->min , items[i].y);
			cont->min = MIN(cont->min , items[i].z);
		}
	//get the maxima of the NeighbourSearch
	for (i=0; i< len2; i++){
			cont->min = MIN(cont->min , items2[i].x);
			cont->min = MIN(cont->min , items2[i].y);
			cont->min = MIN(cont->min , items2[i].z);

			cont->max = MAX(cont->max , items2[i].x);
			cont->max = MAX(cont->max , items2[i].y);
			cont->max = MAX(cont->max , items2[i].z);
		}
	#ifdef DEBUG
	printf("\t min/ max\t\t: %f / %f\n", cont->min, cont->max);
	#endif
	//compute the distance between min and max
	double diff;
	diff = cont->max - cont->min;

	#ifdef DEBUG
	//print the distances
	printf("\t (max - min)\t\t: %f\n", diff);
	#endif

	//compute how large one box is
	diff /= number;

	#ifdef DEBUG
	printf("\t (max - min)/(# boxes)\t: %f\n", diff);
	#endif
	//copyPoint(&(cont->boxsize), &diff); //saves the size of each box

	cont->boxsize = diff;
	cont->numBox  = number * number * number;

	{
		#ifdef __cplusplus
			cont->boxes = new Box[cont->numBox];
			if (cont->boxsize == 0.) throw std::string("Error boxsize is exactly zero.");
		#else
			cont->boxes = (Box*)malloc(cont->numBox * sizeof(Box));		//allocate memory for all boxes
			if (cont->boxes == NULL) {
				fprintf(stderr,"Out of memory\n");
				return NULL;
			}
		#endif


		Box *b = cont->boxes;
		int c = 0;
		////Build all boxes
		for (i=0; i< number; i++){			//x-direction
			for (j=0; j< number; j++){		//y-direction
				for (k=0; k< number; k++){	//z-direction
					c			= INDEX(i,j,k, number);
					#ifdef __cplusplus
					b[c].points = new std::vector<Point*>;
					#else
					b[c].points	= new_list();						// init all lists
					#endif

					b[c].min.x	= cont->min +  i    * cont->boxsize;
					b[c].max.x	= cont->min + (i+1) * cont->boxsize;

					b[c].min.y	= cont->min +  j    * cont->boxsize;
					b[c].max.y	= cont->min + (j+1) * cont->boxsize;

					b[c].min.z	= cont->min +  k    * cont->boxsize;
					b[c].max.z	= cont->min + (k+1) * cont->boxsize;

					b[c].index	= c;
					b[c].i		= i;
					b[c].j		= j;
					b[c].k		= k;
					b[c].len	= 0;
					}
				}
			}
		#ifdef DEBUG
		printf("fill the boxes\n"); fflush(stdout);
		#endif
		//fill all the boxes
		for (i = 0; i<len; i++){				//walk through all the points
			Box *b = findBox(&(items[i]), cont);
			if (b) {
				#ifdef __cplusplus
					b->points->push_back( &(items[i]));
				#else
					insert_list_element(b->points,b->points->prev,&(items[i]));	//add item to the end of a list
				#endif
				b->len++; //count the new item
			}
			else fprintf(stderr,"Item %d dropped: Box not found.\n", i);
			}

		//determine average list size
		double mean=0.;

		for(j=0;j<cont->numBox; j++) {
			mean += b[j].len;
			}
		//print average list size
		#ifdef DEBUG
		printf("\t average number of items per box: %f \t (%d)\n", mean/(float)cont->numBox, cont->numBox);
		#endif
	}
	return cont;
}
/**  NeighbourSearch_Finalize
 * finally we need to free the space we allocated
 * \param cont: The NeigbourSearch object which was created by NeighbourSearch_Create
 * \return void
 **/
void NeighbourSearch_Finalize(NeighbourSearch *cont){
	#ifdef __cplusplus
		if (cont){
			delete[] cont->boxes;
			delete cont;
		}
	#else
		if (cont){
			free(cont->boxes);
			free(cont);
		}
	#endif
	cont = NULL;
	return;
}

/** name: NeighbourSearch::findNeighbourInBox
 * Find the nearest neighbour in one box, by
 * comparing the test point with all points in the box
 * \param p : the pointer to a point
 * \param b : the pointer to a box in that the function searches
 * \param min: the pointer to the double that saves the minimal distance
 * \return Point
 **/
Point *findNeighbourInBox(Point *p, Box *b, double *min){
	#ifndef __cplusplus
		list_t *l = b->points;
	#endif
	Point *nb = NULL;
	Point *neighbour = NULL;		//return NULL if not found
	double dist;

	#ifdef __cplusplus
	if (b->points->size() == 0) return NULL;

	for (unsigned int i = 0; i < b->points->size(); i++){
		nb = (Point *) b->points->at(i);
		if ( nb == NULL ) break;
		dist = distance(nb,p);	// get the distance between the two points
		if (dist <= *min){		// get the minimal value
			*min = dist;
			neighbour = nb;
		}
	}
	#else // c code
	int s;
	s = list_size(l);
	//s = b->len;
	if (s == 0) return NULL;

	while ((l = l->next)){
	//while ((l = l->prev)){
			nb = (Point*) l->val;
			if ( nb == NULL ) break;

			dist = distance(nb,p);	// get the distance between the two points

			if (dist <= *min){		// get the minimal value
				*min = dist;
				neighbour = nb;
			}
	}
	#endif
	return neighbour;
}

/** name: NeighbourSearch::findBetterNeighbour
 *	Searches for a neighbour which is closer than the point proposed by
 * 	the user.
 * \param p     : the actual point
 * \param nb    : neighbour
 * \param dist  : distance between both
 * \param b     : the box in which the search is performed
 * \return Point: pointer to the neighbour
 **/
Point *findBetterNeighbour(Point *p, Point *nb, double *dist, Box *b){
	double dist2=1.e40;
	Point *nb2 = NULL;
	if ( b )  {// is b valid ?
		nb2 = findNeighbourInBox(p, b, &dist2);
		if ((nb2) && ( dist2 < *dist )) {
			*dist = dist2;		//set the new distance
			nb = nb2;			//set the new point
			return nb2; 		//and finally return it
		} else return nb;	//otherwise the old neigbour was closer
	} else return nb;
}

/** SnakeOut
 * If no point was found in the box *b then one has to search in all
 * neighbouring boxes. For this reason this function first tries to find
 * a neighbour in the first layer. If none was found a neigbour in the
 * second layer around p is searched and so on.
 *
 * \brief Remark:	If one point was found one has to check one outer layer more
 * 			because there could be particles with closer distances. For
 * 			cubic boxes the distance to the point in box 2 can be closer
 * 			than the one in box 3, which is in the first layer around 1.
 \verbatim
________________
|   |   |   |   |
|___|___|___|___|
|   | 1 |   | 2 |
|___|__*|___|*__|
|   |   |\  |   |
|___|___|3_\|___|
|   |   |   |   |
|___|___|___|___|
 \endverbatim
 * \param p : the point for which we want to know the neighbour
 * \param nb: the last neighbout we have found, typically none
 * 				 we store our candidate here
 * \param b : the box were *p lies in
 * \param dist: we store the distance to the resulting neigbour here
 * \param cont: the actual NeighbourSearch object
 * \return Point, which is the closest neighbour
 */
Point *SnakeOut(Point *p, Point *nb,Box *b, double *dist, NeighbourSearch* cont){
	int i,j,k;
	int snake = 1;				//! start with radius of one box
	int count = 0; 				//! counts how often particles were found
	double old = *dist;

	while (1==1){
	//! step over the neighbouring boxes in the radius of snake
	for (i= -snake; i < snake+1; i++){			//x-direction
		if (((b->i + i) < 0) || ((b->i + i) >= cont->div)) continue;

		for (j= -snake; j < snake+1; j++){		//y-direction
			if (((b->j + j) < 0) || ((b->j + j) >= cont->div)) continue;

			for (k= -snake; k < snake+1; k++){	//z-direction
				if (((b->k + k) < 0) || ((b->k + k) >= cont->div)) continue;
				if ((abs(i)!=snake) && (abs(j)!=snake)&& (abs(k)!=snake)) continue; // ignore all boxes in the inner

				// all the neighbouring boxes
				Box *bo2 = findBoxbyIndex(b->i +i, b->j + j, b->k + k, cont);
				if ((bo2) )//&& (bo2->len > 0)){
					nb = findBetterNeighbour(p,nb, dist, bo2);
			}
		}
	}
	//! only if there was a point which is closer count this one (first layer check)
	if (*dist < old) { count++; old = *dist;}

	//! if all indices are out of range stop the loop
	if (	 (((b->i -snake) < -1) && ((b->i +snake)  > cont->div))
		&&   (((b->j -snake) < -1) && ((b->j +snake)  > cont->div))
		&&   (((b->k -snake) < -1) && ((b->k +snake)  > cont->div)))
		return nb;


	//! if surely the second layer has been checked, stop the loop, too
	if ( count >= 2 ) return nb;

	//! otherwise increase the radius once more
	snake++;
	}
	return nb;
}

/** SnakeOutDirected
 * If we already know a neighbour in the one box that contains our test
 * point then we have to check only neigbouring points. Points that are
 * closer can only be in the layers around in the distance of *dist.
 * Depending on that distance we can decide which boxes don't have
 * to be checked because they are too far away.
 * In the example below the test point lies in the box 1 in the lower
 * right corner. The point above is its nearest neighbour. Because its
 * distance is closer then all the boxes left, we can leave them out.
 * In two dimensions it is enough to check the boxes 2,3 and 4 only.
 \verbatim
________________
|   |   |   |   |
|___|___|___|___|
|   | *1|  2|   |
|___|__*|___|___|
|   |  4|  3|   |
|___|___|___|___|
|   |   |   |   |
|___|___|___|___|
 \endverbatim
 * @param p : the test point for which we want to know the neighbour
 * @param nb: the last neighbout we have found,
 * 				 we store our better candidate here
 * @param b : the box were *p lies in
 * @param dist: we store the distance of the resulting neigbour here
 * @param cont: the actual NeighbourSearch object
 * @return Point, which is the closest neighbour
 */
Point *SnakeOutDirected(Point *p, Point *nb,Box *b, double *dist, NeighbourSearch* cont){
	int i,j,k;
	int snakex = 1;				//start with radius of one box
	int snakez = 1;				//start with radius of one box
	int snakey = 1;				//start with radius of one box
	int snakexm = 1;			//start with radius of one box
	int snakezm = 1;			//start with radius of one box
	int snakeym = 1;			//start with radius of one box
	int x;

	//! how many boxes are in the distance dist?
	x = (int) ceil(cont->boxsize/ *dist);

	//! Now we find out how many boxes we have to check .
	//! lower indices i,j,k) : 0 means: don't go in that direction
	snakex	= ((b->min.x < p->x - *dist) ? 0 : x );
	snakey	= ((b->min.y < p->y - *dist) ? 0 : x );
	snakez	= ((b->min.z < p->z - *dist) ? 0 : x );

	//! ... and higher indices i,j,k
	snakexm	= ((b->max.x > p->x + *dist) ? 0 : x );
	snakeym	= ((b->max.y > p->y + *dist) ? 0 : x );
	snakezm	= ((b->max.z > p->z + *dist) ? 0 : x );

	//! step over the neighbouring boxes in the radius of x
	for (i= -snakex; i < snakexm+1; i++){			//x-direction
		if (((b->i + i) < 0) || ((b->i + i) >= cont->div)) continue;

		for (j= -snakey; j < snakeym+1; j++){		//y-direction
			if (((b->j + j) < 0) || ((b->j + j) >= cont->div)) continue;

			for (k= -snakez; k < snakezm+1; k++){	//z-direction
				if (((b->k + k) < 0) || ((b->k + k) >= cont->div)) continue;
				if ((i==0)&&(j==0)&&(k==0)) continue; // ignore the centering box, we know the best neighbour already

				// all the neighbouring boxes
				Box *bo2 = findBoxbyIndex(b->i +i, b->j + j, b->k + k, cont);
				if ((bo2))
					nb = findBetterNeighbour(p,nb, dist, bo2);
				}
			}
		}

	return nb;
}

/** SearchInDistance
 * This function does the same job as SnakeOutDirected. It checks the
 * distance to the test point for all the boxes we have. For this reason
 * it is much slower than SnakeOutDirected, but much easier to
 * understand, so I left this function ...
 * @param p		: The test point for which we want to know the neighbour
 * @param nb		: the last neighbout we have found,
 * 				 	  we store our better candidate here
 * @param dist	: we store the distance of the resulting neigbour here
 * @param cont	: the actual NeighbourSearch object
 * @return Point, which is the closest neighbour
 */
Point *SearchInDistance(Point *p, Point *nb, double *dist, NeighbourSearch* cont){
	int i;

	for (i=0; i<cont->numBox; i++){
		Box *b  = &(cont->boxes[i]);
		double minx, miny, minz, maxx, maxy, maxz;
		minx = b->min.x- *dist;
		maxx = b->max.x+ *dist;
		miny = b->min.y- *dist;
		maxy = b->max.y+ *dist;
		minz = b->min.z- *dist;
		maxz = b->max.z+ *dist;

		if (		((b->max.x <= p->x) && (maxx >= p->x))
		 		||	((b->min.x >= p->x) && (minx <= p->x))
		 		||	((b->max.y <= p->y) && (maxy >= p->y))
		 		||	((b->min.y >= p->y) && (miny <= p->y))
		 		||	((b->max.z <= p->z) && (maxz >= p->z))
		 		||	((b->min.z >= p->z) && (minz <= p->z))
		 		//|| 	(b == mybox)	// not needed because this box has already been checked
			)
				nb = findBetterNeighbour(p,nb, dist, b);
		}
	return nb;
}


/** Search
 * The function search performs a standard search of the nearest
 * neighbour by comparing the test point with all the candidates. This
 * function is the slowest possibility of searching the nearest neigbour.
 * This function is the current default for cases when the point p is
 * outside all boxes. 
 * @param p		: The test point.
 * @param nb	: Here we store the closest neighbour.
 * @param dist	: Here we store the distance to nb.
 * @param cont	: The container structure which stores all the points.
 * @return Point* that is the closest neighbour.
 */
Point *Search(Point *p, Point *nb, double *dist, NeighbourSearch* cont){
	int i;
	for (i=0; i<cont->numBox; i++)
			nb = findBetterNeighbour(p,nb, dist, &(cont->boxes[i]));
	return nb;
}
/** find_nearest_neigbour
 * This function is the heart of this code. It should be called by
 * the user.
 * @param p		: The test point of the type 'Point'.
 * @param d		: The pointer to the place were I can save the distance.
 * @param cont	: The container object that stores the boxes.
 * @return Point: The pointer to the closest point is returned.
 *                If the distance is exactly 0.0, the the search is aborted
 *                and the point itself is returned.
 */
Point *find_nearest_neighbour(Point* p, double *d, NeighbourSearch* cont){
	//! -# first step:
	//! We want to know which box contains the test point.
	Box *bo = findBox(p, cont);
	Point *nb = NULL; //neighbour
	double dist =1.e40;

	//! -# second step:
	//! we look for the closest neigbour in the box of the test point
	if ( bo ) {
		nb = findNeighbourInBox(p, bo, &dist);
		/** \attention if the distance is exactly 0.0, we have found a point which
		 equals our test point. No other point can be closer, so we
		 can finish our search. */
		if ((nb) && (dist == 0.0)) { *d = dist; return nb; }

		//! -# third step:
		//! if the box was empty, we want to look for neigbours in
		//! sourrounding boxes
		if (!(nb)) nb = SnakeOut(p, nb,bo, &dist, cont);
		//! otherwise we only have to look in neighbouring boxes for
		//! points that or more closer
		else nb = SnakeOutDirected(p, nb, bo, &dist, cont);
	
		//! another possibility is to call SearchInDistance, but this function
		//! is much slower for many boxes
	
		//else nb = SearchInDistance(p, nb, bo, &dist, cont);
	}
	/** \todo 	In the case the point we search a neighbour for lies
	 * 			outside the searching box a slow standard search is
	 * 			performed. This needs to be improved !
	 **/
	else  nb = Search(p, nb, &dist, cont);

	//save the shortest distance
	*d = dist;
	return nb; //!... and return the neigbour ... ready
}
#ifdef __cplusplus
} // end of namespace NeighbourSearch
} // end of namespace mylibs
#endif

