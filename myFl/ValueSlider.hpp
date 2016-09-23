//		./ValueSlider.h
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



#ifndef _ValueSlider_HPP
#define _ValueSlider_HPP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <FL/fl_draw.H>
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Slider.H>
#include <string>
#include "TipWin.hpp"

namespace mylibs {
// VALUE SLIDER WITH FLOATING TIP WINDOW
class ValueSlider : public Fl_Slider {
	TipWin *tipwin;
	void value_cb2() {
		std::stringstream ss;
		ss << value_scaled() << _unit;
		tipwin->value(ss.str());
		tipwin->position(Fl::event_x_root(), Fl::event_y_root()+20);
	}
	static void value_cb(Fl_Widget*, void*data) {
		ValueSlider *val = (ValueSlider*)data;
		val->value_cb2();
	}
public:
	ValueSlider(int x,int y,int w,int h,const char*l=0);
	~ValueSlider();
	int handle(int e);

	float offset() {return _offset;};
	void offset(float f){_offset = f;}

	float factor() {return _factor;};
	void factor(float f){_factor = f;}

	const std::string& unit() {return _unit;};
	void unit(const std::string s){_unit = s;}
	void unit(const char* s){_unit = std::string(s);}

	inline float value_scaled(){ return _offset + value()*_factor;}


private:
	float       _offset;
	float       _factor;
	std::string   _unit;
};
}; // end of namespace mylibs
#endif //_ValueSlider_H
