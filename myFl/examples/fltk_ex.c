#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Gl_Window.H>
#include <FL/gl.h>

//#include "draw.h"

//
// OpenGL App With FLTK Widgets
// erco 11/08/06
//

int draw_func(float fg, int w, int h){
	// Draw 'X' in fg color
	glColor3f(fg, fg, fg);
	glBegin(GL_LINE_STRIP); glVertex2f(w, h); glVertex2f(-w,-h); glEnd();
	glBegin(GL_LINE_STRIP); glVertex2f(w,-h); glVertex2f(-w, h); glEnd();

	return(0);
}


// OPENGL WINDOW CLASS
class MyGlWindow : public Fl_Gl_Window {
    double fg;                       // foreground brightness
    double bg;                       // background brightness
    // FIX OPENGL VIEWPORT
    //     Do this on init or when window's size is changed
    void FixViewport(int W,int H) {
        glLoadIdentity();
        glViewport(0,0,W,H);
        glOrtho(-W,W,-H,H,-1,1);
    }
    // DRAW METHOD
    void draw() {
        if (!valid()) { valid(1); FixViewport(w(), h()); }      // first time? init
        // Clear screen to bg color
        glClearColor(bg, bg, bg, 0.0);
        glClear(GL_COLOR_BUFFER_BIT);
        // Draw 'X' in fg color
        //glColor3f(fg, fg, fg);
        //glBegin(GL_LINE_STRIP); glVertex2f(w(), h()); glVertex2f(-w(),-h()); glEnd();
        //glBegin(GL_LINE_STRIP); glVertex2f(w(),-h()); glVertex2f(-w(), h()); glEnd();
		draw_func(fg, w(), h());
    }
    // HANDLE WINDOW RESIZING
    void resize(int X,int Y,int W,int H) {
        Fl_Gl_Window::resize(X,Y,W,H);
        FixViewport(W,H);
        redraw();
    }
public:
    // OPENGL WINDOW CONSTRUCTOR
    MyGlWindow(int X,int Y,int W,int H,const char*L=0) : Fl_Gl_Window(X,Y,W,H,L) {
        fg = 1.0;
        bg = 0.0;
        end();
    }
    void SetBrightness(double new_fg, double new_bg)
        { fg = new_fg; bg = new_bg; redraw(); }
};

// APP WINDOW CLASS
class MyAppWindow : public Fl_Window {
    MyGlWindow *mygl;                    // opengl window
    Fl_Value_Slider *fg_brightness;      // fg brightness slider
    Fl_Value_Slider *bg_brightness;      // bg brightness slider
public:
    Fl_Group *gr, *gr2;
private:
    // Someone changed one of the sliders
    void ValueChanged_CB2() {
        mygl->SetBrightness(fg_brightness->value(), bg_brightness->value());
    }
    static void ValueChanged_CB(Fl_Widget*, void*userdata) {
        MyAppWindow *appwin = (MyAppWindow*)userdata;
        appwin->ValueChanged_CB2();
    }
public:
    // APP WINDOW CONSTRUCTOR
    MyAppWindow(int W,int H,const char*L=0) : Fl_Window(W,H,L) {
        // OpenGL window
        mygl = new MyGlWindow(10, 10, W-50, h()-80);

        gr = new Fl_Group(0,0,w()-30, h());
        // Foreground slider
        fg_brightness = new Fl_Value_Slider(120, h()-60, gr->w()/2, 20, "FG Bright");
        fg_brightness->align(FL_ALIGN_LEFT);
        fg_brightness->type(FL_HOR_SLIDER);
        fg_brightness->bounds(0.0, 1.0);
        fg_brightness->value(1.0);
        fg_brightness->callback(ValueChanged_CB, (void*)this);
        // Background slider
        bg_brightness = new Fl_Value_Slider(120, h()-30, gr->w()/2, 20, "BG Bright");
        bg_brightness->align(FL_ALIGN_LEFT);
        bg_brightness->type(FL_HOR_SLIDER);
        bg_brightness->bounds(0.0, 1.0);
        bg_brightness->value(0.0);
        bg_brightness->callback(ValueChanged_CB, (void*)this);
        gr->end();

        gr2 = new Fl_Group(w()-30,0,30, h());
        gr2->box(FL_UP_FRAME);
        gr2->end();
        end();
    }
};

// MAIN
int main() {
    MyAppWindow win(800, 600, "OpenGL Test App");
    //win.resizable(win.gr);
    //win.resizable(win.gr);
    //win.size(win.w()+30, win.h());
    //win.init_sizes();
    win.resizable(win.gr);
    win.show();
    return(Fl::run());
}
