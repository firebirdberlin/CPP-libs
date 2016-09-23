//      TipWin.cpp
//
//      Copyright 2011 Stefan Fruhner <stefan.fruhner@gmail.com>
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


#include "TipWin.hpp"


TipWin::TipWin()
	:Fl_Menu_Window(1,1) {	  // will autosize
	strcpy(tip, "X.XX");
	set_override();
	end();
}


void TipWin::draw() {
	draw_box(FL_BORDER_BOX, 0, 0, w(), h(), Fl_Color(175));
	fl_color(FL_BLACK);
	fl_font(labelfont(), labelsize());
	fl_draw(tip, 3, 3, w()-6, h()-6, Fl_Align(FL_ALIGN_LEFT|FL_ALIGN_WRAP));
}

void TipWin::value(float f) {
	std::stringstream ss;
	ss << f;
	strcpy(tip, ss.str().c_str());

	// Recalc size of window
	fl_font(labelfont(), labelsize());
	int W = w(), H = h();
	fl_measure(tip, W, H, 0);
	W += 8;
	size(W, H);
	redraw();
}

void TipWin::value(const std::string &f) {
	strcpy(tip, f.c_str());

	// Recalc size of window
	fl_font(labelfont(), labelsize());
	int W = w(), H = h();
	fl_measure(tip, W, H, 0);
	W += 8;
	size(W, H);
	redraw();
}

