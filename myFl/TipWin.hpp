//      TipWin.hpp
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


#ifndef TIPWIN_HPP
#define TIPWIN_HPP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <string>
#include <FL/fl_draw.H>
#include <FL/Fl.H>
#include <FL/Fl_Slider.H>
#include <FL/Fl_Menu_Window.H>
#include <FL/Fl_Tooltip.H>


class TipWin : public Fl_Menu_Window {
	char tip[40];
public:
	TipWin();
	void draw();
	void value(float f);
	void value(const std::string &f);
};

#endif /* TIPWIN_HPP */
