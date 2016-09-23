/**
 * Fl_Boundedint_Input.cpp
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


#include "Fl_BoundedInt_Input.hpp"
namespace mylibs {

Fl_BoundedInt_Input::Fl_BoundedInt_Input(int x, int y, int w, int h, const char *label)
	: Fl_Int_Input(x,y,w,h, label), lb(0), ub(1000000){

	check_range();
}

void Fl_BoundedInt_Input::check_range(){
	if (atoi(value()) < lb){
		char val[100];
		sprintf(val, "%d", lb);
		value(val);
	}

	if (atoi(value()) > ub) {
		char val[100];
		sprintf(val, "%d", ub);
		value(val);
	}
	return;
}

int Fl_BoundedInt_Input::handle(int e){


	switch(e) {
		case FL_LEAVE:
		case FL_DEACTIVATE:
		case FL_UNFOCUS:  // leave focus
				check_range();
                break;
        }

	return(Fl_Int_Input::handle(e));
}


void Fl_BoundedInt_Input::bounds(int lower, int upper){
	if ( lower < upper ) {
		lb = lower;
		ub = upper;
	} else{
		ub = lower;
		lb = upper;
	}
	check_range();
	return;
}
};
