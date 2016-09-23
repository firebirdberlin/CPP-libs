
#include "ValueSlider.hpp"

namespace mylibs {

ValueSlider::ValueSlider(int x,int y,int w,int h,const char*l):
			Fl_Slider(x,y,w,h,l),
			_offset(0.),
			_factor(1.),
			_unit(std::string()) {
	type(FL_HOR_SLIDER);
	callback(value_cb, (void*)this);
	Fl_Group *save = Fl_Group::current();   // save current widget..
	tipwin = new TipWin();				  // ..because this trashes it
	tipwin->hide();
	Fl_Group::current(save);				// ..then back to previous.
	//save->add(tipwin);// If this is done the tips are not shown anymore.
}

/** \fn ValueSlider::~ValueSlider()
 *	Destructor, we have reserved space for the TipWin, which musst be freed.
 *	And because the TipWin wasn't added, we have to do it manually.
 */
ValueSlider::~ValueSlider() {
	delete tipwin; tipwin = NULL;
}

int ValueSlider::handle(int e) {
	switch(e) {
		case FL_MOVE: // mouse moves over widget
			break;
		case FL_PUSH:
		case FL_DRAG:{ // mouse move while button down
			// XXX: if offscreen, move tip ABOVE mouse instead
			tipwin->position(Fl::event_x_root(), Fl::event_y_root()+20);
			std::stringstream ss;
			ss << value_scaled() << _unit;
			tipwin->value(ss.str());
			tipwin->show();
			break;
		}
		case FL_HIDE:	   // valuator goes away
		case FL_RELEASE:	// release mouse
		case FL_LEAVE:	  // leave focus
			// Make sure tipwin closes when app closes
			tipwin->hide();
			break;
	}
	return(Fl_Slider::handle(e));
}
}; //end of namespace mylibs
