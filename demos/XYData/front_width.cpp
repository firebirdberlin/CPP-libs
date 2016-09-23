//      front_width.cpp
//
//      Copyright 2012 Stefan Fruhner <stefan.fruhner@gmail.com>
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


#include <iostream>
#include <mylibs/xydata.hpp>
#include <mylibs/myinifiles.hpp>
#include <math.h>

int main(int argc, char **argv)
{
	myIniFiles ini(argc, argv);

	ini.set_info("Analyses xy-datasets and locates fronts within the data.");

	ini.register_param("input" , "i", "input data");
	ini.register_param("output", "o", "output data");

	mystring input  = ini.read("input", mystring());
	mystring output = ini.read("output", mystring());

	ini.check();

	XYData func(input.c_str());

	double min = func.min();
	double max = func.max();
	XYData::Range xrange = func.xrange();
	XYData::Range yrange(
							min + 0.1 * (max -min),
							min + 0.9 * (max -min)
						);
	list<XYData::Range> steps = func.find_step(yrange);

	for (list<XYData::Range>::iterator it = steps.begin(); it != steps.end(); ++it){
		cout << it->min << "\t" << it->max << "\t" << fabs(it->max - it->min) << endl;
	}

	return 0;
}

