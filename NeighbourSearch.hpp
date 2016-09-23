//		./NeighbourSearch.hpp
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

#ifndef __NeighbourSearch_H
#define __NeighbourSearch_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <time.h>


#ifdef __cplusplus
	#include <iostream>
    #include <vector>
    #include <deque>
	#include <string>
	#include "point.hpp"
namespace mylibs {
namespace NeighbourSearch{
#else
    #include "lists.h"
#endif


#ifndef	    TRUE
#define	    TRUE	    1
#endif

#ifndef	    FALSE
#define	    FALSE	    0
#endif

#ifndef	    boolean
#define	    boolean	    short
#endif

#define LEN 1000
#define LEN2 20000
#define DIV 10.

#ifndef MIN
#define MIN(a,b) (((a) <= (b)) ? (a) : (b) )
#endif

#ifndef MAX
#define MAX(a,b) (((a) >= (b)) ? (a) : (b) )
#endif

#define INDEX(i,j,k, all) (i*all*all+j*all+k) // converts ijk index to one dimensional index

//#ifndef _POINT_
// /**
// * \attention If you want to use your own Point definition, then you can
// * define the makro _Point_ and a struct or class named Point inside your
// * main program. At least the members x,y and z have to be defined.
// */
//#define _POINT_
//typedef struct point { double x,  y,  z; } Point;
//#endif // ifndef _Point_

//if you want to use float values then uncomment the line above and use the following one
//typedef struct point { float x,  y,  z; } Point;

/** \brief The struct box stores a subspace of the complete volume.
    It contains a list of all points within this subvolume, the lowest and
    highest coordinates and an internal index
**/
typedef struct box {
	Point min;	//!< point with lowest (x,y,z) - tuple
	Point max;	//!< point with highest (x,yz) - tuple
	int index;	//!< index of the box
	int i,j,k;	//!< 3-dimensional index tuple of the box

#ifdef __cplusplus
	std::vector<Point*> *points; // we want to save a vector of Pointers
#else
	list_t *points; //!< list of points in the box
#endif
	int len;		//!< # of points
}Box;

/** \brief
    NeighbourSearch is the main structure in this unit. It stores all subvolumes
    (named as boxes) of equal size (boxsize)
**/
typedef struct neighboursearch {
	double min;		//!< minimal value of x, y, or z
	double max;		//!< maximal value of x, y, or z
	int div;		//!< how often shall one NeighbourSearch be divided into boxes?
	double boxsize;	//!< the boxes are cubic, boxsize saves the length of one side
	Box *boxes;		//!< array of subboxes
	int numBox;		//!< number of the subboxes
}NeighbourSearch;

double randomize(int max);

//!fills an array with randomly distributed points
int random_points(Point *a, int len);
//!copies an element of the type Point into another
void copyPoint(Point *to, Point *from);

//!computes the distance between two points
double distance(Point *p1, Point *p2);

//!For the three dimensional indices i,j and k the one-dim.- index is
//! computed. The adress of the box is returned.
Box *findBoxbyIndex(int i,int j, int k , NeighbourSearch* cont);

//For one given point this function computes, the right box index for
//it and returns the adress of the box.
Box *findBox(Point* p, NeighbourSearch* cont);

/** buildNeighbourSearch initializes the objects needed for computation. It
 * has to be called before a nearest neighbour search can be performed.
 * The parameters needed are:
 * \param items	: An array of the type point. In all these points can be searched.
 * \param len		: The size of the array above.
 * \param items2	 : Additional items that contain other points: important to get the maximum sizes
 * \param len2	 : The size of this array
 * \param number: Number of boxes in 1 dimension (defines how many sub-boxes are created)
 * \return NeighbourSearch object
 */
NeighbourSearch *NeighbourSearch_Create(Point *items, int len,Point *items2, int len2, int number);

//! finally we need to free the space we allocated
void NeighbourSearch_Finalize(NeighbourSearch *cont);

/** findNeighBourinBox: find the nearest neighbour in one box.
 *  parameters:
 *  p : the pointer to a point
 *  b : the pointer to a box in that the function searches
 *  min: the pointer to the double that saves the minimal distance
 */
Point *findNeighbourInBox(Point *p, Box *b, double *min);

/**
 * name: findBetterNeighbour
 * @param p   : the actual point
 * @param nb  : neighbour
 * @param dist: distance between both
 * @param b   : the box in which the search is performed
 * @return * of the neighbour
 */
Point *findBetterNeighbour(Point *p, Point *nb, double *dist,Box *b);

/** name: SnakeOut
 * If no point was found in the box *b then one has to search in all
 * neighbouring boxes. For this reason this function first tries to find
 * a neighbour in the first layer. If none was found a neigbour in the
 * second layer around p is searched and so on.
 *
 * Remark:	If one point was found one has to check one outer layer more
 * 			because there could be particles with closer distances. For
 * 			cubic boxes the distance to the point in box 2 can be closer
 * 			than the one in box 3, which is in the first layer around 1.
 * 			________________
 *			|	|	|	|	|
 *			|___|___|___|___|
 *			|   | 1 |   | 2 |
 * 			|___|__*|___|*__|
 *			|	|	|\	|	|
 *			|___|___|3_\|___|
 *			|   |   |   |   |
 * 			|___|___|___|___|
 * @param p   : the point for which we want to know the neighbour
 * @param nb  : the last neighbout we have found, typically none
 * 			    we store our candidate here
 * @param b   : the box were *p lies in
 * @param dist: we store the distance to the resulting neigbour here
 * @param cont: the actual NeighbourSearch object
 * @return	*Point, which is the closest neighbour
 */
Point *SnakeOut(Point *p, Point *nb,Box *b, double *dist, NeighbourSearch* cont);

/** name: SnakeOutDirected
 * If we already know a neighbour in the one box that contains our test
 * point then we have to check only neigbouring points. Points that are
 * closer can only be in the layers around in the distance of *dist.
 * Depending on that distance we can decide which boxes we don't have
 * to check because they are to far away.
 * In the example below the test point lies in the box 1 in the lower
 * right corner. The point above is its nearest neighbour. Because its
 * distance is closer then all the boxes left, we can leave them out.
 * In two dimensions it is enough to check the boxes 2,3 and 4 only.
 *
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
 * @param p   : the test point for which we want to know the neighbour
 * @param nb  : the last neighbout we have found,
 * 			    we store our better candidate here
 * @param b   : the box were *p lies in
 * @param dist: we store the distance of the resulting neigbour here
 * @param cont: the actual NeighbourSearch object
 * @return	*Point, which is the closest neighbour
 */
Point *SnakeOutDirected(Point *p, Point *nb,Box *b, double *dist, NeighbourSearch* cont);

/**
 * name: SearchInDistance
 * This function does the same job as SnakeOutDirected. It checks the
 * distance to the test point for all the boxeswe have. For this reason
 * it is much slower than SnakeOutDirected, but much easier to
 * understand, so I left this function ...
 * @param p		: The test point for which we want to know the neighbour
 * @param nb		: the last neighbout we have found,
 * 				 	  we store our better candidate here
 * @param mybox	: the box were *p lies in
 * @param dist	: we store the distance of the resulting neigbour here
 * @param cont	: the actual NeighbourSearch object
 * @return	*Point, which is the closest neighbour
 */
Point *SearchInDistance(Point *p, Point *nb,Box *mybox,  double *dist, NeighbourSearch* cont);

/**
 * name: Search
 * The function search performs a standard search of the nearest
 * neighbour by comparing the test point with all the candidates. This
 * function is the slowest possibility of searching the nearest neigbour.
 * It is not used, but left here for comparison reasons.
 * @param p		: The Test point.
 * @param nb		: Here we store the closest neighbour.
 * @param dist	: Here we store the distance to nb.
 * @param cont	: The structure which contains all the points.
 * @return *Point that is the closest neighbour.
 */
Point *Search(Point *p, Point *nb, double *dist, NeighbourSearch* cont);

/**
 * name: find_nearest_neigbour
 * This function is the heart of this code. It should be called by
 * the user.
 * @param
 * 		*p		: The test point of the type 'Point'.
 * 		*d		: The pointer to the place were I can save the distance.
 * 		*cont	: The conatiner object that stores the boxes.
 * @return
 * 		*Point  : The pointer to the closest point is returned.
 */
Point *find_nearest_neighbour(Point* p, double *d, NeighbourSearch* cont);

class NSearch{
	private:
		NeighbourSearch *cont;
		double dist;
		bool ok;

	public:

		/** name: NSearch::NSearch
		 * \param items: array of points to search in
		 * \param len: length of this array
		 * \param items2: array of additionally points (to ensure the min and max of box is comuted right)
		 * \param len2: length of that array
		 * \param numbox: nr of boxes in each direction
		 * \return
		 */
		NSearch(Point *items, int len, Point *items2=NULL, int len2=0, int numbox=4):
			cont(NULL), dist(1.e99), ok(false){
			cont = NeighbourSearch_Create(items, len, items2, len2, numbox);
		}

		NSearch(std::vector<Point> &items, std::vector<Point> &items2, int numbox=4) :
			cont(NULL), dist(1.e99), ok(false){
			Point *a = &items[0];
			Point *b = &items2[0];
			cont = NeighbourSearch_Create(a, (int)items.size(), b, (int)items2.size(), numbox);
		}

		NSearch(std::deque<Point> &items, std::vector<Point> &items2, int numbox=4) :
			cont(NULL), dist(1.e99), ok(false){
			Point *a = &items[0];
			Point *b = &items2[0];
			cont = NeighbourSearch_Create(a, (int)items.size(), b, (int)items2.size(), numbox);
		}

		~NSearch(){
			NeighbourSearch_Finalize(this->cont);
		}

		Point* find_neighbour(Point& p){
			this->ok = false;
			Point *nb = find_nearest_neighbour(&p, &(this->dist), this->cont);
			this->ok = (nb != NULL);
			return nb;
		}

		Point* find_neighbour(const Point& p){
			Point pt = p;
			this->ok = false;
			Point *nb = find_nearest_neighbour(&pt, &(this->dist), this->cont);
			this->ok = (nb != NULL);
			return nb;
		}

		double distance(){return dist;}
		bool found(){return ok;}
		void info(){
			printf("\nInfo for NSearch object\n");
			printf("boxsize : %f\n", cont->boxsize);
			printf("    div : %d\n" , cont->div);
			printf("    max : %f\n", cont->max);
			printf("    min : %f\n", cont->min);
			printf(" numBox : %d\n" , cont->numBox);
		}
		double max(){return cont->max;}
		double min(){return cont->min;}
		double numboxes(){return cont->numBox;}
		double boxsize(){return cont->boxsize;}
};

} // end of namespace NeighbourSearch
} // edn of namespace mylibs
#endif // define _NEIGHBOURSEARCH_H
