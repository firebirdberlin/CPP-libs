/**
 * Fl_Gnuplot_Window.hpp
 *
 * Copyright 2012 Stefan Fruhner <stefan.fruhner@gmail.com>
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


#ifndef Fl_GnUPLOT_WINDOW_HPP
#define Fl_GnUPLOT_WINDOW_HPP

#include <FL/Fl_Double_Window.H>
#include "Fl_Gnuplot.hpp"

class Fl_Gnuplot_Window: public Fl_Double_Window
{
	public:
		Fl_Gnuplot_Window(int x, int y, int w,int h,const char*l=0);
		Fl_Gnuplot_Window(int w,int h,const char*l=0);
		virtual ~Fl_Gnuplot_Window();

//	private:
	public:
		/* add your private declarations */
		mylibs::Fl_Gnuplot graph;
};

#endif /* Fl_GnUPLOT_WINDOW_HPP */
