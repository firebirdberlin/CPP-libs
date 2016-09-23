/**
 * Fl_BoundedFloat_Input.hpp
 *
 * Copyright 2013 Stefan Fruhner <stefan.fruhner@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 *
 **/


#ifndef FL_BOUNDEDFLOAT_INPUT_HPP
#define FL_BOUNDEDFLOAT_INPUT_HPP

#include <stdlib.h>
#include <stdio.h>
#include <FL/Fl_Float_Input.H>
namespace mylibs {
class Fl_BoundedFloat_Input: public Fl_Float_Input
{
	public:
		Fl_BoundedFloat_Input(int x, int y, int w, int h, const char *label=0);
		int handle(int e);
		void check_range();
		void bounds(float lower, float upper);

	private:
		/* add your private declarations */
		float lb; // lower bound
		float ub; // upper bound
};
};
#endif /* FL_BOUNDEDFLOAT_INPUT_HPP */
