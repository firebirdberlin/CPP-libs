//		./MCubes/Triangle.hpp
//
//		Copyright 2011 Stefan Fruhner <stefan.fruhner@gmail.com>
//		Copyright 2011 Arash Azhand <azhand@itp.tu-berlin.de>
//		Copyright 2011 Philipp Gast <avatar81@itp.tu-berlin.de>
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
#ifndef __TRIANGLE_HPP
#define __TRIANGLE_HPP

#include <mylibs/point.hpp>

namespace MCubes {

class Triangle {
	public:
		Point v1, v2, v3;
		Point NormalVector;

		// ******************************************
		// *------ Constructors & Destructors ------*
		// ******************************************
		Triangle(const Point &a,const Point &b,const Point &c) {
			this->v1 = a;
			this->v2 = b;
			this->v3 = c;

			this->NormalVector = (a - b).cross(a - c);
			this->NormalVector.normalize();
		}

		Triangle() {}
		~Triangle() {};
};

}

#endif
