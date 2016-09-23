// BoundingBox.cpp
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


#include "BoundingBox.hpp"

namespace mylibs {

BoundingBox::BoundingBox() {
	setInfinity();
}

BoundingBox::BoundingBox(const Point &minP, const Point &maxP) :
	min(minP), max(maxP){

}

bool BoundingBox::check(){
	return (	(min.x < max.x)
			and
				(min.y < max.y)
			and
				(min.z < max.z)
				);
}

void BoundingBox::setInfinity(){
	max.setInfinity();
	min.setInfinityMin();
}

void BoundingBox::fromString(const mystring conditions){
		setInfinity(); // sets the BB to infinite size
		myStringList l = conditions.split(",;:| \t\v\f\n");
		for(myStringList::iterator it = l.begin(); it != l.end(); it++){
			vector<double> vals;
			it->extract(vals);
			if (vals.size() == 0) continue;

			cout << *it << " " << vals[0] << endl;
			if (it->startswith("x>")) { 	min.x = vals[0];	}
			else
			if (it->startswith("x<")) {		max.x = vals[0];	}
			else
			if (it->startswith("y>")) {		min.y = vals[0]; 	}
			else
			if (it->startswith("y<")) {		max.y = vals[0]; 	}
			else
			if (it->startswith("z>")) {		min.z = vals[0];	}
			else
			if (it->startswith("z<")) {		max.z = vals[0];	}
			else{
				cout << "Invalid condition : " << *it << endl;
				throw myexception(EXCEPTION_ID+"Error: Condition not valid !");
			}
		}

		cout << *this << endl;

		if (not check()) throw myexception(EXCEPTION_ID+"Error: BoundingBox invalid!");


}

const list<Point> BoundingBox::corners() const{
	list<Point> p;
	Point pt(min);
	// 0
	p.push_back(pt);

	// 1
	pt.x = max.x;
	p.push_back(pt);

	// 2
	pt.z = max.z;
	p.push_back(pt);

	// 3
	pt = min;
	pt.z = max.z;
	p.push_back(pt);

	// 4
	pt = min;
	pt.y = max.y;
	p.push_back(pt);

	// 5
	pt.x = max.x;
	p.push_back(pt);

	// 6
	p.push_back(max);

	// 7
	pt = max;
	pt.x = min.x;
	p.push_back(pt);
	return p;
}

double BoundingBox::distance_to(const BoundingBox &other){
	const list<Point> c1 = this->corners();
	const list<Point> c2 = other.corners();

	vector<double> dist;
	for (list<Point>::const_iterator it = c1.begin(); it != c1.end() ; it++){
	for (list<Point>::const_iterator jt = c2.begin(); jt != c2.end() ; jt++){
		dist.push_back((*it-*jt).abs());
	}
	}

	return algorithm::min(dist);
}

double BoundingBox::max_distance_to(const BoundingBox &other){
	const list<Point> c1 = this->corners();
	const list<Point> c2 = other.corners();

	vector<double> dist;
	for (list<Point>::const_iterator it = c1.begin(); it != c1.end() ; it++){
	for (list<Point>::const_iterator jt = c2.begin(); jt != c2.end() ; jt++){
		dist.push_back((*it-*jt).abs());
	}
	}

	return algorithm::max(dist);
}

bool BoundingBox::inside(const Point &p){
	return (		 p.x >= min.x
				and  p.y >= min.y
				and  p.z >= min.z
				and  p.x <= max.x
				and  p.x <= max.y
				and  p.z <= max.z );
}



ostream &operator<<( ostream &out, const mylibs::BoundingBox &bb){
	out << bb.get_min() << endl << bb.get_max() << endl;
	return out;
}

} // end of namespace mylibs
