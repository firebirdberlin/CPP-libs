// BoundingBox.hpp
//
// Copyright 2013 Stefan Fruhner <stefan.fruhner@gmail.com>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
// MA 02110-1301, USA.


#ifndef BOUNDINGBOX_HPP
#define BOUNDINGBOX_HPP

#include "mystring.hpp"
#include "point.hpp"
#include "algorithm.hpp"


namespace mylibs{
class BoundingBox
{
	public:
		BoundingBox();
		BoundingBox(const Point &minP, const Point &maxP);

		bool check();
		void setInfinity();
		void fromString(const mystring conditions);

//		bool 	overlap(const BoundingBox &other);
		const list<Point> corners() const;
		double distance_to(const BoundingBox &other);
		double max_distance_to(const BoundingBox &other);

		bool inside(const Point &p);
		double width(){return max.x - min.x;};
		double depth(){return max.y - min.y;};
		double height(){return max.z - min.z;};

		const Point& get_min() const {return min;}
		const Point& get_max() const {return max;}
	private:
		/* add your private declarations */
		Point min; // smallest coords
		Point max; // largest coords of the BoundingBox
};

ostream &operator<<( ostream &out, const BoundingBox &bb);
} // end of namespace mylibs

#endif /* BOUNDINGBOX_HPP */
