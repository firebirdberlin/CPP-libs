//		./PlotWin.h
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
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Group.H>
#include "Fl_Gnuplot.hpp"

#ifndef __PLOTWIN_H
#define __PLOTWIN_H
// OPENGL WINDOW CLASS

//void end_callback(Fl_Widget*, void*) {
//		if (fl_ask("Are you really absolutely positively"
//					" certain you want to quit?"));
////			exit(0);
//		}

class MyGraphWindow : public Fl_Window {
	public:
	// WINDOW CONSTRUCTOR
	MyGraphWindow(int X,int Y,int W,int H,const char*L=0) :
        Fl_Window(X,Y,W,H,L),
        toolbox(5, 5, W-10,H-10, NULL),
        graph(10, 10, this->w() - 15, this->h()- 30)
    {
		resizable(this);
//		this->callback(end_callback);
		this->add_resizable(toolbox);
		toolbox.add_resizable(graph);

		toolbox.box(FL_ROUNDED_BOX);
		toolbox.labeltype(FL_ENGRAVED_LABEL);

//		double dxx[]={1.,2.,3.4, 4.};
//		double dyy[]={2.,1.,2.4, 2.};
//		graph.add_2d_data(dxx,dyy,4,"Test");

		toolbox.add(graph);
		toolbox.end();
		this->add(&toolbox);

        end();
//		print("parent : " << this->parent());
	}
	private:
		// HANDLE WINDOW RESIZING
		void resize(int /* X */, int /* Y */, int W, int H) {
			toolbox.resize(5,5,W-10, H-10);
			graph.resize(10,10,W-15, H-30);
			toolbox.redraw();
			graph.redraw();
			redraw();
		}
		Fl_Group toolbox;
	public:
		mylibs::Fl_Gnuplot graph;
};

#endif //__PLOTWIN_H
