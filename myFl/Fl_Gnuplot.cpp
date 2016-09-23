//		./Fl_Gnuplot.cpp
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
/*
// Gnuplot interface widget
// Copyrigth Jacques Tremblay jackt@gel.ulaval.ca
//
//
// This widget is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA.
*/

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <math.h>

#include "FL/fl_draw.H"
#include "FL/fl_ask.H"
#include "FL/Fl_Menu_Item.H"
#include "FL/Fl_Window.H"
#include "Fl_Gnuplot.hpp"

using namespace std;

namespace mylibs {

#define N_ENTRIES 8192
const int margin_top    = 25;
const int margin_left   = 52;
const int margin_right  = 15;
const int margin_bottom = 26;


const char* Fl_Gnuplot::plot_with(const With &w){
	switch (w){
		case PLOT_WITH_LINES: 			return "lines";
		case PLOT_WITH_DOTS : 			return "dots";
		case PLOT_WITH_STEPS: 			return "steps";
		case PLOT_WITH_ERRORBARS: 		return "xyerrorbars";
		case PLOT_WITH_XERRORBAR:		return "xerrorbar";
		case PLOT_WITH_XYERRORLINES: 	return "xyerrorlines";
		case PLOT_WITH_POINTS: 			return "points";
		case PLOT_WITH_IMPULSES: 		return "impulses";
		case PLOT_WITH_FSTEPS: 			return "fsteps";
		case PLOT_WITH_ERRORLINES:		return "errorlines";
		case PLOT_WITH_XERRORLINES:		return "xerrorlines";
		case PLOT_WITH_YERRORBARS:		return "yerrorbars";
		case PLOT_WITH_LINESPOINTS:		return "linespoints";
		case PLOT_WITH_LABELS:			return "labels";
		case PLOT_WITH_HISTEPS:			return "histeps";
		case PLOT_WITH_FINANCEBARS:		return "financebars";
		case PLOT_WITH_XYERRORBARS:		return "xyerrorbars";
		case PLOT_WITH_YERRORLINES:		return "yerrorlines";
		case PLOT_WITH_VECTORS:			return "vectors";
	}
	return "lines";
}

void Fl_Gnuplot::memory_allocation(size_t items){
	xdata  = (double **)malloc(items * sizeof(double *));
	ydata  = (double **)malloc(items * sizeof(double *));
	titles = (char**   )malloc(items * sizeof(char*)   );
	with   = (With*    )malloc(items * sizeof(With)    );
	ndata  = (int*     )malloc(items * sizeof(int)     );
	if (!xdata || !ydata || !ndata  || !titles || !with) {
		cerr << "Error allocating xdata or ydata or ndata, titles arrays." << endl;
		exit(0);
	}

	// initialization
	for (int i = 0; i < items; i++) {
		 xdata[i] = NULL;
		 ydata[i] = NULL;
		titles[i] = NULL;
		 ndata[i] = 0;
		  with[i] = PLOT_WITH_LINES;
	}
}

void Fl_Gnuplot::memory_allocation_at_index(int index, int n, const char *title, const With plotwith){
	xdata[index]=(double *)malloc(n*sizeof(double));
	ydata[index]=(double *)malloc(n*sizeof(double));
	if (!xdata[index] || !ydata[index]) {
		cerr << "Error allocating xdata[] or ydata[] arrays." << endl;
		exit(0);
	}
	// initialization
	for (int i = 0; i < n; i++){
		xdata[index][i] = 0.;
		ydata[index][i] = 0.;
	}


	titles[index]=(char *)malloc(MAXPATHLEN*sizeof(char));
	if (!title || !*title) {
		titles[index]=NULL;
	} else if (title && *title) {
		sprintf(titles[index],"%s",title);
		includeKey=1;
	}

	ndata[index]=n;
	with [index]=plotwith;
}

void Fl_Gnuplot::memory_allocation_column_mode(size_t items){
	xdata  = (double **)malloc(    1 * sizeof(double *));
	ydata  = (double **)malloc(items * sizeof(double *));
	titles = (char**   )malloc(items * sizeof(char*)   );
	with   = (With*    )malloc(items * sizeof(With)    );
	ndata  = (int*     )malloc(items * sizeof(int)     );
	if (!xdata || !ydata || !ndata  || !titles || !with) {
		cerr << "Error allocating xdata or ydata or ndata, titles arrays." << endl;
		exit(0);
	}

	// initialization
	xdata[0]  = NULL;
	for (int i = 0; i < ycols; i++) {
		ydata[i]  = NULL;
		titles[i] = NULL;
		ndata[i]  = 0;
		with[i]   = PLOT_WITH_LINES;
	}
}

void Fl_Gnuplot::memory_allocation_columns(int col, int n){
	ydata[col]=(double *)malloc(n*sizeof(double));
	if (!ydata[col]) {
		fprintf(stderr,"Error allocating ydata[] arrays\n"); exit(0);
	}
	for (size_t j = 0; j < n; j++) ydata[col][j] = 0.;

	ndata[col]  = n;

}

/**
 * name: Fl_Gnuplot::check_data
 * Check if the data in currentIndex is valid. If not
 * ndata[currentIndex] is set to 0 !!
 * @return false, if data is invalid
 *
 **/
bool Fl_Gnuplot::check_data() {
	if (ndata[currentIndex] == 0) return false;

	XYData::Range yr = XYData(xdata[currentIndex], ydata[currentIndex], ndata[currentIndex]).yrange();
	if (not isfinite(yr.size())) {
		cerr << __FUNCTION__ << ": ATTENTION: yrange is not finite !" << endl;
		ndata[currentIndex] = 0;
		loaded = 0;
		return false;
	}
	return true;
}


/** \fn void gnuplotcom_cb( Fl_Widget *w, void *)
 * \brief Enter a GNUPLOT command to be executed.
 *
 *	This function is the callback for the menu-entry "Gnuplot command" of the
 *	menu which appears after a right-click at a Fl_Gnuplot-object.<br>
 *	This function enables the user to use gnuplot commands (e.g. for setting the
 *	range of the x- or y-axis).
 *	It will ask the user for a gnuplot command via a fl_input and then pass it
 *	to the given Fl_gnuplot-object via its command method.
 *
 * \param w: Pointer to a Fl_Widget, which is assumed to point to a Fl_Gnuplot
 *				object.
 */
void gnuplotcom_cb( Fl_Widget *w, void *) {
	Fl_Gnuplot* graph=(Fl_Gnuplot*)w;

	char *s=(char*)fl_input("Enter Gnuplot commands seperated by ;",graph->command() );
	if( s != NULL ) graph->command(s);
	graph->window()->redraw();
}

/** \fn void set_xaxis_cb( Fl_Widget *w, void *)
 * \brief
 *
 *	This function is the callback for the menu-entry "X-axis" of the
 *	menu which appears after a right-click at a Fl_Gnuplot-object.<br>
 *	This function enables the user to change the varaiable which is used for the
 *	x-axis (e.g. to make a Plot x vs y instead of x/y vs t).
 *	It that will ask the user for a number via a fl_input. If there was
 *	any input, this will be passed to the x_axis-method of the given Fl_Gnuplot
 *	object.
 *
 * \param w: Pointer to a Fl_Widget, which is assumed to point to a Fl_Gnuplot
 *				object.
 */
void set_xaxis_cb( Fl_Widget *w, void *) {
	Fl_Gnuplot* graph=(Fl_Gnuplot*)w;
	char def[10] = "\0";
	sprintf(def, "%d", graph->x_axis());
	char *s=(char*)fl_input("Change the data on the x-axis ;", def);
	if( s != NULL ) graph->x_axis(s);
	graph->window()->redraw();
}

/** \fn void save_cb( Fl_Widget *w, void *)
 * \brief Save the data of a Fl_Gnuplot-object.
 *
 *	This function is the callback for the menu-entry "save" of the
 *	menu which appears after a right-click at a Fl_Gnuplot-object.<br>
 *	This function will save the data of a Fl_Gnuplot-object, that is passed as
 *	argument.
 *	Therefore is first via fl_file_chooser ask for a filename. If the
 *	selected filename was empty nothing is done, otherwise the save_2d_data-
 *	method of the passed Fl_Gnuplot-object will be called.
 *
 * \param w: Pointer to a Fl_Widget, which is assumed to point to a Fl_Gnuplot
 *				object.
 */
void save_cb( Fl_Widget *w, void *) {
	Fl_Gnuplot* graph=(Fl_Gnuplot*)w;

	const char *filename = fl_file_chooser("Select a filename", "*.dat", "data.dat", 0);

	if(! filename)	return;
	graph->save_2d_data(filename);
	//path = string(ptr_path);
}

/** \fn void stride_cb( Fl_Widget *w, void *)
 * \brief Plot only every n-th data-point.
 *
 *	This function is the callback for the menu-entry "stride" of the
 *	menu which appears after a right-click at a Fl_Gnuplot-object.<br>
 *	This function enables the user, to plot only every n-th point. Thereby is n
 *	a positive integer, which is ask for via a fl_input-dialog. The result is
 *	then passed to the given Fl_Gnuplot-object.
 *
 * \param w: Pointer to a Fl_Widget, which is assumed to point to a Fl_Gnuplot
 *				object.
 */
void stride_cb( Fl_Widget *w, void *) {
	Fl_Gnuplot* graph=(Fl_Gnuplot*)w;
	char def[10] = "\0";
	sprintf(def, "%d", graph->stride());
	char *s=(char*)fl_input("Plot every n'th data point ;", def);
	if( s != NULL ) graph->stride(s);
	graph->window()->redraw();
}

/** \fn void internal_plotter_cb( Fl_Widget *w, void *)
 * \brief Save the data of a Fl_Gnuplot-object.
 *
 *	This function is the callback for the menu-entry "internal_plotter" of the
 *	menu which appears after a right-click at a Fl_Gnuplot-object.<br>
 *	It will change the value of the corresponding boolean, of the given
 *	Fl_Gnuplot-object.
 *
 * \param w: Pointer to a Fl_Widget, which is assumed to point to a Fl_Gnuplot
 *				object.
 */
void internal_plotter_cb( Fl_Widget *w, void *) {
	Fl_Gnuplot* graph=(Fl_Gnuplot*)w;
	graph->toggle_internal_plotter();
	graph->window()->redraw();
}

/** \fn Fl_Gnuplot::Fl_Gnuplot(const int, const int, const int, const int, const char * const)
 * \brief Constructor to use, for creating a Fl_Gnuplot-object.
 *
 * \param x, y: x and y position of the widget, respectiveley. These parameters
 *				are passed to the base-class constructor.
 * \param w, h: Width and height, respectiveley, of the widget. These parameters
 *				are passed to the base-class constructor.
 * \param l: Pointer to a c-type string, which is passed to the constructor of
 *				the base class and which is used for the label of the widget.
 */
Fl_Gnuplot::Fl_Gnuplot(const int x, const int y, const int w, const int h, const char * const l) :
	Fl_Box(x,y,w,h,l),xdata(0), ydata(0), ndata(0), with(0), ycols(0),ncurves(0),
	titles(0),includeKey(0), Polar(0),xaxis(0),GPcommand(" "),
	xlab("x-axis"), ylab("y-axis"), Title("Title"),
	v_stride(1),currentIndex(-1),
	internal_function_plotter(false), busy(false),
	mousex1(-1), mousey1(-1),
	mousex2(-1), mousey2(-1),
	x0(this->x() + margin_left),
	y0(this->y() + this->h() - margin_bottom),
	loaded(0), scaleX(1.), scaleY(1.), entries(0),
	nEntries(0), nMaxEntries(N_ENTRIES) {

	if (not mylibs::IO::is_executable("gnuplot")) throw myexception(EXCEPTION_ID+"Could not find gnuplot. Is it really installed ?");
	entries = new OutputEntry[nMaxEntries];
	loaded=0;
}

Fl_Gnuplot::~Fl_Gnuplot() {
	clear_data();
	delete[] entries;
	currentIndex=-1;
}

void Fl_Gnuplot::clear_data() {
	if (ycols != 1) {
		if (xdata  != NULL)	free(xdata[0]);
		for (int i = 0; i < currentIndex + 1; i++) {
			if ((ydata  != NULL) && ( ydata[i] != NULL))	free(ydata[i]);
			if ((titles != NULL) && (titles[i] != NULL))	free(titles[i]);
		}
	} else {
		for (int i = 0; i < currentIndex + 1; i++) {
			if ((xdata  != NULL) && ( xdata[i] != NULL))	free(xdata[i]);
			if ((ydata  != NULL) && ( ydata[i] != NULL))	free(ydata[i]);
			if ((titles != NULL) && (titles[i] != NULL))	free(titles[i]);
		}
	}
	free(xdata);	xdata  = NULL;
	free(ydata);	ydata  = NULL;
	free(titles);	titles = NULL;
	free(ndata);	ndata  = NULL;
	free(with);		with   = NULL;
	ycols  = 0;
}

void Fl_Gnuplot::clear_entries(int release) {
	nEntries = 0;
	if (!release || nMaxEntries <= N_ENTRIES)
		return;

	/** \todo keine Abstürze mehr wenn ich das ier auskommentiere. Frage:
	 * 	Wird das überhaupt benötigt ? Oder ist das jetzt ein Speicherleck ?
	 */
	//cerr << "clear_entries\n";
	//clear_data();

	//delete[] entries; entries = NULL;
	//nMaxEntries = N_ENTRIES;
	//entries = new OutputEntry[nMaxEntries];
	//currentIndex=-1;
	//cerr << "/clear_entries\n";
}

void Fl_Gnuplot::add_entry(char *buf) {
	// strip line ending
	int len = strlen(buf);
	for (int i = 0; i < len; i++) {
		if (buf[i] == '\n') {
			buf[i] = '\0';
			break;
		}
	}

	if (nEntries == nMaxEntries) {
		nMaxEntries += N_ENTRIES;
		OutputEntry *nn = new OutputEntry[nMaxEntries];
		int i;
		for(i=0 ; i<nEntries ; i++) {
			nn[i] = entries[i];
		}
		delete[] entries;
		entries = nn;
	}

	if (buf[0] == 'G') {
		clear_entries();
		nEntries = 1;
		entries[0].k = 'G';
		delete[] entries[0].t;
		entries[0].t = 0;
		return;
	}

	OutputEntry &e = entries[nEntries];
	nEntries++;

	char *str;
	e.k = buf[0];
	delete [] e.t;
	e.t = 0;
	switch(e.k) {
		case 'L':
			sscanf(buf, "L%4d", &e.x);
			e.x = (e.x % 8) + 2;
			break;
		case 'M':
			sscanf(buf, "M%4d%4d", &e.x, &e.y);
			break;
		case 'V': // lines
			sscanf(buf, "V%4d%4d", &e.x, &e.y);
			break;
		case 'P': // points and dots
			sscanf(buf, "P%hi %4d %4d", &e.type, &e.x, &e.y);
			break;
		case 'J':
			sscanf(buf, "J%4d", &e.x);
			break;
		case 'T':
			sscanf(buf, "T%4d%4d", &e.x, &e.y);
			str = buf + 9;
			e.t = new char[strlen(str)+1];
			strcpy(e.t, str);
			break;
			break;
		case 'A':
			sscanf(buf, "A%4d", &e.x);
			break;
		case 'R':
			break;
	}

}


enum JUSTIFY {
	LEFT, CENTRE, RIGHT
};

static int _colors[13] = { FL_WHITE, FL_BLACK, FL_BLACK, FL_BLACK,
                           FL_BLACK, FL_RED, FL_GREEN, FL_BLUE,
                           FL_MAGENTA, FL_CYAN, FL_GRAY, FL_GRAY, FL_GRAY
                         };

void Fl_Gnuplot::load() {
//	int nread;
	if (currentIndex < 0) return;

	int i,j;

	clear_entries(1);

	// xaxis can only be set in column mode
	if (ycols == 1 and xaxis != 0) xaxis = 0;

	scaleX = 1/4096.0*w();
	scaleY = 1/4096.0*h();

	char buf[10*MAXPATHLEN];
//	int tempchar;

	build_command(buf);
	FILE *gppipe = popen(buf, "w");
	setvbuf( gppipe, NULL, _IONBF, 0 );
	if (!gppipe) {
		printf("could not open pipe\n");
		return;
	}

	char *fifofile = tempnam(".", "Fl_Gnuplot_tmp");
	if (!fifofile) {
		cerr << __FILE__ << ":"<< __LINE__ << " Could not create tmpname.";
		exit(-1);
	}

	if (mkfifo(fifofile,448)!=0) {
		fprintf(stderr,"Error creating FIFO pipe. Error was %s\n", strerror(errno));
		exit(-1);
	}

	fprintf(gppipe,"set xlabel '%s'\n",xlab.c_str());
	fprintf(gppipe,"set ylabel '%s'\n",ylab.c_str());
	fprintf(gppipe,"set title '%s'\n", Title.c_str());

	if (GPcommand.size() != 0) {
		fprintf(gppipe,"%s\n",GPcommand.c_str());
	}

	fprintf(gppipe,"set term xlib nopersist\n");
	fprintf(gppipe,"set output \"%s\"\n",fifofile);

	if (Polar) fprintf(gppipe,"set polar\n");
	else fprintf(gppipe,"set nopolar\n");


	if (!includeKey) {
//	if (!includeKey || (xaxis > 0 && xaxis <= currentIndex+1)){
		fprintf(gppipe,"set nokey\n");
	}

	// column mode with different x-axis
	if (xaxis > 0 && xaxis <= currentIndex+1) {
		if (currentIndex == 1 && xaxis == 1) {
			fprintf(gppipe,"set title '%s vs. %s'\n", titles[1], titles[xaxis-1]);
			fprintf(gppipe,"set xlabel '%s'\n", titles[xaxis-1]);
			fprintf(gppipe,"set ylabel '%s'\n",titles[1]);
		} else if (currentIndex == 1 && xaxis == 2) {
			fprintf(gppipe,"set title '%s vs. %s'\n", titles[0], titles[xaxis-1]);
			fprintf(gppipe,"set xlabel '%s'\n", titles[xaxis-1]);
			fprintf(gppipe,"set ylabel '%s'\n",titles[0]);
		} else {
			fprintf(gppipe,"set title 'data vs. %s'\n", titles[xaxis-1]);
			fprintf(gppipe,"set xlabel '%s'\n", titles[xaxis-1]);
			fprintf(gppipe,"set ylabel 'data'\n");
		}
	}

	if (xaxis==0) {
		if (titles[0])
			fprintf(gppipe, "plot '-' title '%s' w %s", titles[0], plot_with(with[0]));
		else {
			fprintf(gppipe, "plot '-' notitle w %s",plot_with(with[0]));
		}
		for (int i = 1; i <= currentIndex; i++) {
			if (titles[i]) 	fprintf(gppipe, ", '-' title '%s' w %s", titles[i],plot_with(with[i]));
			else			fprintf(gppipe, ", '-' notitle w %s", plot_with(with[i]));
		}
		fprintf(gppipe,"\n");
	} else { // xaxis is different
		int start = 1;
		if (xaxis != 1) {
			if (titles[0]) fprintf(gppipe, "plot '-' title '%s' w %s", titles[0], plot_with(with[0]));
			else		   fprintf(gppipe, "plot '-' notitle w %s", plot_with(with[0]));
		} else {
			if (titles[1]) fprintf(gppipe, "plot '-' title '%s' w %s", titles[1], plot_with(with[1]));
			else           fprintf(gppipe, "plot '-' notitle w %s", plot_with(with[1]));
			start = 2;
		}
		for (int i = start; i <= currentIndex; i++) {
			if (xaxis -1 == i) continue;

			if ( titles[i] )
				fprintf(gppipe, ", '-' title '%s' w %s", titles[i], plot_with(with[i]));
			else
				fprintf(gppipe, ", '-' notitle w %s", plot_with(with[i]));
		}
	}
	fprintf(gppipe,"\n");

	FILE *fifofptr;
	if ((fifofptr=fopen(fifofile,"rw"))==NULL) {
		fprintf(stderr,"Error opening FIFO file\n");
		exit(0);
	}

	// write the data to gnuplot
	if (ycols == 1) {
		for (j=0; j<=currentIndex; j++) {
			if (ndata[j] == 0){
				fprintf(gppipe,"%lf %lf\n",0.,0.);
			}
			else {
				for(i=0 ; i<ndata[j] ; i+=v_stride) {
					fprintf(gppipe,"%lf %lf\n",xdata[j][i],ydata[j][i]);
					//printf("%lf %lf\n",xdata[j][i],ydata[j][i]);
				}
			}
			fprintf(gppipe,"e\n");
			//printf("e\n");
			fflush(gppipe);
		}
	} else { // column mode
		if (xaxis == 0 || xaxis > currentIndex+1) {
			for (j=0; j<=currentIndex; j++) {
				if (ndata[0] == 0){
					fprintf(gppipe,"%lf %lf\n",0.,0.);
				}
				else {
					for(i=0 ; i<ndata[0]; i+=v_stride) {
						fprintf(gppipe,"%lf %lf\n",xdata[0][i],ydata[j][i]);
					}
				}
				fprintf(gppipe,"e\n");
				fflush(gppipe);
			}
		} else {
			for (j=0; j<=currentIndex; j++) {
				if (xaxis-1 == j) {
					continue; // skip identity
				}
				if (ndata[j] == 0){
					fprintf(gppipe,"%lf %lf\n",0.,0.);
				}
				else {
					for(i=0 ; i<ndata[j] ; i+=v_stride) {
						fprintf(gppipe,"%lf %lf\n",ydata[xaxis-1][i],ydata[j][i]);
					}
				}
				fprintf(gppipe,"e\n");
				fflush(gppipe);
			}
		}
	}

	fflush(gppipe);
	// read the result
	do {
		if (fgets(buf, 1023, fifofptr)) {
			add_entry(buf);
		}
	} while (buf[0]!='E');

	fclose(fifofptr);
	unlink(fifofile);
	pclose(gppipe);
	free(fifofile);
	loaded = 1;
}

void Fl_Gnuplot::build_command(char *buf) {
	// we want to see nothing gnuplot wants to tell us
//	sprintf(buf, "gnuplot &> /dev/null");
	sprintf(buf, "gnuplot > /dev/null" );
}

void Fl_Gnuplot::play() const {
	char *str;
	double sw;
	int jmode = 0;
	int cx=0, cy=0;
	int x1, y1;
	int lt = 1;
	int i;
	static int text_rotation = 0;

	fl_color(FL_WHITE);
	fl_rectf(x(), y(), w(), h());

	// draw selection rect
	if (mousex1 > -1 and mousex2 > -1) {
		fl_color(FL_RED);
		fl_rect(mousex1, mousey1, mousex2-mousex1, mousey2-mousey1);
	}

	fl_font(FL_TIMES, 12);
	fl_color(FL_BLACK);
	for(i=0 ; i<nEntries ; i++) {
		OutputEntry &e = entries[i];
		switch(e.k) {
			case 'G':
				draw_box();
				draw_label();
				break;
			case 'L':
				lt = e.x;
				fl_color(_colors[lt+3]);
				break;
			case 'M':
				cx = e.x; cy = e.y;
				break;
			case 'V': // lines
				x1 = e.x; y1 = e.y;
				/* possible line styles in fltk 1.3.1
				*
				*      FL_SOLID      -------
				*      FL_DASH       - - - -
				*      FL_DOT        .......
				*      FL_DASHDOT    - . - .
				*      FL_DASHDOTDOT - .. -
				*      FL_CAP_FLAT
				*      FL_CAP_ROUND
				*      FL_CAP_SQUARE (extends past end point 1/2 line width)
				*      FL_JOIN_MITER (pointed)
				*      FL_JOIN_ROUND
				*      FL_JOIN_BEVEL (flat)
				*
				**/
				//void fl_line_style(int style, int width, char* dashes)
				fl_line_style(FL_SOLID, 2);
				fl_line(x() + x1*scaleX, y() + h()-y1*scaleY, x() + cx*scaleX, y() + h()-cy*scaleY);
				fl_line_style(0);
				cx = x1;
				cy = y1;
				break;
			case 'P': {// points
				const int r = 4;
				switch (e.type){
					case -1:
						fl_point(x() + e.x*scaleX, y() + h()-e.y*scaleY);
						break;
					case 0:
						fl_point( 0 + x() + e.x*scaleX, -1 + y() + h()-e.y*scaleY);

						fl_point(-1 + x() + e.x*scaleX,  0 + y() + h()-e.y*scaleY);
						fl_point( 0 + x() + e.x*scaleX,  0 + y() + h()-e.y*scaleY);
						fl_point( 1 + x() + e.x*scaleX,  0 + y() + h()-e.y*scaleY);

						fl_point( 0 + x() + e.x*scaleX,  1 + y() + h()-e.y*scaleY);

						break;
//						fl_draw(".",x() + e.x*scaleX, y() + h()-e.y*scaleY); break;
					case 1:
						for (int i = -r; i <= r; i++)
							fl_point((x() + e.x*scaleX), (y() + h()- (e.y)*scaleY)+i);
						for (int i = -r; i <= r; i++)
							fl_point((x() + e.x*scaleX)+i, y() + h()- (e.y)*scaleY );
						break;
//						fl_draw("+",x() + e.x*scaleX, y() + h()-e.y*scaleY); break;
					case 2:
						for (int i = -r; i <= r; i++)
							fl_point((x() + e.x*scaleX)+i, (y() + h()- (e.y)*scaleY)+i);
						for (int i = -r; i <= r; i++)
							fl_point((x() + e.x*scaleX)+i, (y() + h()- (e.y)*scaleY)-i);
//						fl_draw("x",x() + e.x*scaleX, y() + h()-e.y*scaleY);
						break;
					case 3:
						for (int i = -r; i <= r; i++)
							fl_point((x() + e.x*scaleX)+i, (y() + h()- (e.y)*scaleY)+i);
						for (int i = -r; i <= r; i++)
							fl_point((x() + e.x*scaleX)+i, (y() + h()- (e.y)*scaleY)-i);
						for (int i = -r; i <= r; i++)
							fl_point((x() + e.x*scaleX), (y() + h()- (e.y)*scaleY)+i);
						for (int i = -r; i <= r; i++)
							fl_point((x() + e.x*scaleX)+i, y() + h()- (e.y)*scaleY );
						break;
//						fl_draw("ж",x() + e.x*scaleX, y() + h()-e.y*scaleY); break;
					case 4:
						fl_loop(	x() + e.x*scaleX -r , y() + h()- e.y*scaleY -r,
									x() + e.x*scaleX +r , y() + h()- e.y*scaleY -r,
									x() + e.x*scaleX +r , y() + h()- e.y*scaleY +r,
									x() + e.x*scaleX -r , y() + h()- e.y*scaleY +r
								);
						break;
//						fl_draw("◻",x() + e.x*scaleX, y() + h()-e.y*scaleY); break;
					case 5:{

						for (int i = -r; i <= r; i++)
							for (int j = -r; j <= r; j++)
								fl_point(i + x() + e.x*scaleX, j + y() + h()-e.y*scaleY);
						break;
						}
//						fl_draw("■",x() + e.x*scaleX, y() + h()-e.y*scaleY); break;
					case 6:
						fl_arc(x() + e.x*scaleX -r, y() + h()- e.y*scaleY -r,2*r,2*r,0,360);
//						fl_draw("o",x() + e.x*scaleX, y() + h()-e.y*scaleY);
						break;
					case 7:
						for (size_t i = 0; i < 2*r; i++){
							fl_arc(x() + e.x*scaleX -r/2, y() + h()- e.y*scaleY -r/2,r,r,0,360);
						}
						fl_draw("●",x() + e.x*scaleX, y() + h()-e.y*scaleY);
						break;
					case 8:
//						fl_loop (int x, int y, int x1, int y1, int x2, int y2)
						fl_loop(x() + e.x*scaleX -r , y() + h()- e.y*scaleY +r,
								x() + e.x*scaleX +r , y() + h()- e.y*scaleY +r,
								x() + e.x*scaleX    , y() + h()- e.y*scaleY -r
						);
//						fl_draw("△",x() + e.x*scaleX, y() + h()-e.y*scaleY);
						break;
					case 9:
						for (size_t i = 0; i <=r; i++){
							fl_loop(x() + e.x*scaleX -i , y() + h()- e.y*scaleY +i,
									x() + e.x*scaleX +i , y() + h()- e.y*scaleY +i,
									x() + e.x*scaleX    , y() + h()- e.y*scaleY -i
							);
						}
//						fl_draw("▲",x() + e.x*scaleX, y() + h()-e.y*scaleY);
						break;
					case 10:
						fl_loop(x() + e.x*scaleX -r , y() + h()- e.y*scaleY -r,
								x() + e.x*scaleX +r , y() + h()- e.y*scaleY -r,
								x() + e.x*scaleX    , y() + h()- e.y*scaleY +r
						);
//						fl_draw("▽",x() + e.x*scaleX, y() + h()-e.y*scaleY);
						break;
					case 11:
						for (size_t i = 0; i <=r; i++){
							fl_loop(x() + e.x*scaleX -i , y() + h()- e.y*scaleY -i,
									x() + e.x*scaleX +i , y() + h()- e.y*scaleY -i,
									x() + e.x*scaleX    , y() + h()- e.y*scaleY +i
							);
						}
//						fl_draw("▼",x() + e.x*scaleX, y() + h()-e.y*scaleY);
						break;
					case 12:
						fl_loop(	x() + e.x*scaleX   , y() + h()- e.y*scaleY -r,
									x() + e.x*scaleX +r, y() + h()- e.y*scaleY   ,
									x() + e.x*scaleX   , y() + h()- e.y*scaleY +r,
									x() + e.x*scaleX -r, y() + h()- e.y*scaleY
							);
//						fl_draw("◇",x() + e.x*scaleX, y() + h()-e.y*scaleY);
						break;
					case 13:
						for (size_t i = 0; i <=r; i++){
							fl_loop(	x() + e.x*scaleX   , y() + h()- e.y*scaleY -i,
										x() + e.x*scaleX +i, y() + h()- e.y*scaleY   ,
										x() + e.x*scaleX   , y() + h()- e.y*scaleY +i,
										x() + e.x*scaleX -i, y() + h()- e.y*scaleY
							);
						}
//						fl_draw("◆",x() + e.x*scaleX, y() + h()-e.y*scaleY);
						break;
					case 14:
						for (int i = -r; i < r; i++)
							fl_point((x() + e.x*scaleX), (y() + h()- (e.y)*scaleY)+i);
						for (int i = -r; i < r; i++)
							fl_point((x() + e.x*scaleX)+i, y() + h()- (e.y)*scaleY );
						break;
//						fl_draw("+",x() + e.x*scaleX, y() + h()-e.y*scaleY); break;
					case 15:
						for (int i = -r; i < r; i++)
							fl_point((x() + e.x*scaleX)+i, (y() + h()- (e.y)*scaleY)+i);
						for (int i = -r; i < r; i++)
							fl_point((x() + e.x*scaleX)+i, (y() + h()- (e.y)*scaleY)-i);
						break;
//						fl_draw("❌",x() + e.x*scaleX, y() + h()-e.y*scaleY); break;
					case 16:
						for (int i = -r; i <= r; i++)
							fl_point((x() + e.x*scaleX)+i, (y() + h()- (e.y)*scaleY)+i);
						for (int i = -r; i <= r; i++)
							fl_point((x() + e.x*scaleX)+i, (y() + h()- (e.y)*scaleY)-i);
						for (int i = -r; i <= r; i++)
							fl_point((x() + e.x*scaleX), (y() + h()- (e.y)*scaleY)+i);
						for (int i = -r; i <= r; i++)
							fl_point((x() + e.x*scaleX)+i, y() + h()- (e.y)*scaleY );
						break;
//						fl_draw("ж",x() + e.x*scaleX, y() + h()-e.y*scaleY); break;
					case 17:
							fl_loop(	x() + e.x*scaleX -r , y() + h()- e.y*scaleY -r,
									x() + e.x*scaleX +r , y() + h()- e.y*scaleY -r,
									x() + e.x*scaleX +r , y() + h()- e.y*scaleY +r,
									x() + e.x*scaleX -r , y() + h()- e.y*scaleY +r
							);
//							fl_draw("◻",x() + e.x*scaleX, y() + h()-e.y*scaleY);
							break;
					case 18:
						for (int i = -r; i <= r; i++)
							for (int j = -r; j <= r; j++)
								fl_point(i + x() + e.x*scaleX, j + y() + h()-e.y*scaleY);
						break;
//						fl_draw("■",x() + e.x*scaleX, y() + h()-e.y*scaleY); break;
					case 19:
						fl_arc(x() + e.x*scaleX -r, y() + h()- e.y*scaleY -r,2*r,2*r,0,360);
//						fl_draw("○",x() + e.x*scaleX, y() + h()-e.y*scaleY);
						break;
					default:{ // "."
							fl_point( 0 + x() + e.x*scaleX, -1 + y() + h()-e.y*scaleY);

							fl_point(-1 + x() + e.x*scaleX,  0 + y() + h()-e.y*scaleY);
							fl_point( 0 + x() + e.x*scaleX,  0 + y() + h()-e.y*scaleY);
							fl_point( 1 + x() + e.x*scaleX,  0 + y() + h()-e.y*scaleY);

							fl_point( 0 + x() + e.x*scaleX,  1 + y() + h()-e.y*scaleY);

							break;
						}
				}


//				fl_line_style(0);
				cx = x1;
				cy = y1;
				break;
			}
			case 'J':
				jmode = e.x;
				break;
			case 'A':
				text_rotation = e.x;
				if (text_rotation < 0) text_rotation += 360;
				break;
			case 'T':
				x1 = e.x; y1 = e.y;
				str = e.t;
				sw = fl_width(str);
				switch (jmode) {
					case LEFT:	sw = 0;			break;
					case CENTRE:sw = -sw / 2; 	break;
					case RIGHT:	sw = -sw;		break;
				}
				fl_color(_colors[2]);

//! \todo Think about if FLTK 1.3 should be mandatory
#ifdef HAVE_FLTK_1_3
				if (text_rotation == 90) {
					fl_draw(text_rotation, str, x() + x1*scaleX, y() + h()-y1*scaleY  - sw);
					text_rotation = 0;
				} else
#endif
					fl_draw(str, x() + x1*scaleX + sw, y() + h()-y1*scaleY + fl_height()/3);


				fl_color(_colors[lt+3]);
				break;
			case 'R':
				break;
		}
	}
}

Range Fl_Gnuplot::minmax(const double *data,const int &ndata) {
	if (ndata == 0) {
		Range zero = {0., 0.};
		return zero;
	}
	Range range =  {data[0],data[0]} ;
	for(int i=v_stride ; i<ndata ; i+=v_stride) {
		range.min =  (data[i] < range.min) ? data[i] : range.min;
		range.max =  (data[i] > range.max) ? data[i] : range.max;
	}
	return range;
}

int order_of_magnitude(float a) {
	int e = 0;
	a = (a > 0.) ? a : -a; //fabs(a);
	if ( a == 0 ) return 0;
	if (a < 1.) {
		while (a < 1.) { a *= 10.; e--; }
		e++;
	} else {
		while (a > 1.) { a /= 10.; e++; }
		e--;
	}
	return e;
}

float roundto(float f, int digit) {
	return floor(f * pow( 10., digit) + 0.5) * pow(10., -digit);
}

float floorto(float f, int digit) {
	float p = pow( 10., digit);
	return floor(f * p) / p;
}

float ceilto(float f, int digit) {
	float p = pow( 10., digit);
	return ceil(f * p) / p;
}

void round_ranges(Range &xr, Range &yr) {

//	double p = pow10(floorf(log10f(fabs(xr.max-xr.min))));
//	xr.max = ceilf(xr.max / p) * p;
//	xr.min = floorf(xr.min / p) * p;

//	p = pow10(floorf(log10f(fabs(yr.max-yr.min))));
//	yr.max = ceilf(yr.max / p) * p;
//	yr.min = floorf(yr.min / p) * p;

//second try
	int e1 = order_of_magnitude(xr.min);
	int e2 = order_of_magnitude(xr.max);
	int e3 = order_of_magnitude(yr.min);
	int e4 = order_of_magnitude(yr.max);

	xr.min = floorto(xr.min, -(--e1));// - .2 * pow(1, --e1);
	xr.max = ceilto (xr.max, -(--e2));// + .2 * pow(1, --e2);
	yr.min = floorto(yr.min, -(--e3));// - .2 * pow(1, --e1);
	yr.max = ceilto (yr.max, -(--e4));// + .2 * pow(1, --e2);

// first try
//	xr.min = floorf(xr.min);
//	xr.max = ceilf(xr.max);
//	yr.min = floorf(yr.min);
//	yr.max = ceilf(yr.max);
}

void Fl_Gnuplot::draw_ticks(Range &xr, Range &yr) {
	fl_font(FL_TIMES, 12);
	fl_color(FL_BLACK);



	// x-ticks
	float sw = 0.;
	float step = (xr.max - xr.min) / 5.;
	for (int i = 0; i < 6 ; i++) {
		stringstream st;
		st << (xr.min + i*step);
		sw = fl_width(st.str().c_str());
		fl_draw(st.str().c_str(), x0 + i * (w()-margin_left-margin_right)/5. -sw/2., y0+12);
	}

	//y-ticks
	step = (yr.max - yr.min) / 5.;
	for (int i = 0; i < 6  ; i++) {
		stringstream st;
		st << (yr.min + i*step);
		sw = fl_width(st.str().c_str());
		fl_draw(st.str().c_str(), x0 - sw - 6, y0 - i *(h()-margin_top-margin_bottom)/5.);
	}
}


void Fl_Gnuplot::draw_curve(const double *xdata,const double *ydata, int ndata,
                            Range &rx, Range &ry) {
	if (ndata == 0) return;
	int i = 0;
	float sx = (w()- margin_left - margin_right )/(rx.max - rx.min);
	float sy = (h()- margin_top  - margin_bottom)/(ry.max - ry.min);

	fl_line_style(FL_SOLID, 2);
	float cx = xdata[0], cy = ydata[0];
	for(i=v_stride ; i<ndata ; i+=v_stride) {
		fl_line(x0 + (cx      -rx.min)*sx, y0 - (cy       - ry.min)*sy,
		        x0 + (xdata[i]-rx.min)*sx, y0 - (ydata[i] - ry.min)*sy);
		cx = xdata[i];
		cy = ydata[i];
	}
	fl_line_style(0);
}


Fl_Color chooseColor(int i) {
	i = (i%7);
	switch (i) {
		case 0: return FL_RED;
		case 1: return FL_GREEN;
		case 2: return FL_BLUE;
		case 3: return FL_YELLOW;
		case 4: return FL_MAGENTA;
		case 5: return FL_CYAN;
		case 6: return FL_BLACK;
		default: return FL_RED;
	}
}

void Fl_Gnuplot::play_intern() {
	//char *str;// not used at the moment
	double sw;
	int j = 0;
	x0 = this->x() + margin_left;
	y0 = this->y() + this->h() - margin_bottom;

	fl_color(FL_WHITE);
	fl_rectf(x(), y(), w(), h());

	// draw selection rect
	if (mousex1 > -1 and mousex2 > -1) {
		fl_color(FL_RED);
		fl_rect(mousex1, mousey1, mousex2-mousex1, mousey2-mousey1);
	}
	Range maxx = {0.,0.};
	Range maxy = {0.,0.};
	fl_font(FL_TIMES, 12);
	if (ycols == 1) {
		maxx.min = xdata[0][0];
		maxx.max = xdata[0][ndata[0]-1];

		maxy = minmax(ydata[0], ndata[0]);
		for (j=1; j<=currentIndex; j++) {
			Range ma = minmax(xdata[j], ndata[j]);
			Range my = minmax(ydata[j], ndata[j]);
			maxx.max = (ma.max > maxx.max) ? ma.max : maxx.max;
			maxx.min = (ma.min < maxx.min) ? ma.min : maxx.min;
			maxy.max = (my.max > maxy.max) ? my.max : maxy.max;
			maxy.min = (my.min < maxy.min) ? my.min : maxy.min;
		}
		round_ranges(maxx, maxy);

		for (j=0; j<=currentIndex; j++) {
			fl_color(chooseColor(j));
			draw_curve(xdata[j], ydata[j], ndata[j], maxx, maxy);
		}
	} else { // column mode where xaxis is always the same
		if (xaxis == 0 || xaxis > currentIndex+1) {
			maxx.min = xdata[0][0];
			maxx.max = xdata[0][ndata[0]-1];
			maxy = minmax(ydata[0], ndata[0]);
			for (j=1; j<=currentIndex; j++) {
				Range my = minmax(ydata[j], ndata[j]);
				maxy.max = (my.max > maxy.max) ? my.max : maxy.max;
				maxy.min = (my.min < maxy.min) ? my.min : maxy.min;
			}
			round_ranges(maxx, maxy);

			for (j=0; j<=currentIndex; j++) {
				fl_color(chooseColor(j));
				draw_curve(xdata[0], ydata[j], ndata[0], maxx, maxy);
			}
		} else { // Phasenraum plots
			maxx = minmax(ydata[xaxis-1], ndata[xaxis-1]);
			bool yset = false;
			for (j=0; j<=currentIndex; j++) {
				if (xaxis-1 == j) continue; // skip identity
				if (not yset) {
					maxy = minmax(ydata[j], ndata[j]);
					yset = true;
				}
				Range my = minmax(ydata[j], ndata[j]);
				maxy.max = (my.max > maxy.max) ? my.max : maxy.max;
				maxy.min = (my.min < maxy.min) ? my.min : maxy.min;
			}
			round_ranges(maxx, maxy);
			int c = 0;
			for (j=0; j<=currentIndex; j++) {
				if (xaxis-1 == j) continue; // skip identity

				fl_color(chooseColor(c++));
				draw_curve(ydata[xaxis-1], ydata[j], ndata[j], maxx, maxy);
			}
		}
	}

	fl_line_style(FL_SOLID, 2);
	fl_color(FL_BLACK);
	// Rahmen
	fl_rect(x0-2, y()+margin_top - 2, w()-margin_left - margin_right +2,
	        h() - margin_bottom - margin_top + 4);


	draw_ticks(maxx, maxy);
	// Titel
	if (xaxis == 0) { // normal mde and column mode
		fl_font(FL_TIMES, 12);
		fl_color(FL_BLACK);
		sw = fl_width(Title.c_str());
		fl_draw(Title.c_str(), x0 + (w()-margin_right - margin_left)/2 -sw/2, y()+15);


		sw = fl_width(xlab.c_str());
		fl_draw(xlab.c_str(),  x0 + (w()-margin_right - margin_left)/2 -sw/2, y() + h() - 2);

		sw = fl_width(ylab.c_str());
#ifdef HAVE_FLTK_1_3
		fl_draw(ylab.c_str(), x()+4, y() + h()/2 + sw);
#else
		fl_draw(ylab.c_str(), x()+4, y() + h()/2);
#endif
	}
}

void Fl_Gnuplot::draw() {
	if (internal_function_plotter) {
		play_intern();
		return;
	}

	if (!loaded) load();
	play();
}

/** name: Fl_Gnuplot::set_2d_data_columns
 * This function initializes the column mode. Data sharing the same x-axis
 * (equal x-distances) can be supplied at once. When saving these data the file
 * will be organized in columns. This procedure needs less memory, since the
 * x-axis is stored only once. The function is written using an ellipse, so it
 * handles an argument list of variable length.
 *
 * @param xx: the data on the x-axis
 * @param n : the number of values in xx
 * @param nr_cols : the number of columns (how many different data arrays follow ?)
 * @return Nothing
 **/
void Fl_Gnuplot::set_2d_data_columns(const double *xx, int n, int nr_cols, ...) {
	clear_data(); // delete all data in the arrays

	Polar=0;
	ycols 			= nr_cols;
	currentIndex 	= nr_cols-1;

	memory_allocation_column_mode(ycols);

	xdata[0] =(double *)malloc(n*sizeof(double));
	if (!xdata[0]) {
		fprintf(stderr,"Error allocating xdata[0] array\n"); exit(0);
	}

	for (int i = 0; i < ycols; i++) {
//		memory_allocation_columns(i,n);
		ydata[i]=(double *)malloc(n*sizeof(double));
		if (!ydata[i]) {
			fprintf(stderr,"Error allocating ydata[] arrays\n"); exit(0);
		}
		ndata[i]  = n;
	}


	for(int i=0 ; i<n; i++) xdata[0][i]=xx[i];

	// We access the ellipses through a va_list, so let's declare one
	va_list list;

	// We initialize the va_list using va_start.  The first parameter is
	// the list to initialize.  The second parameter is the last non-ellipse
	// parameter.
	va_start(list, nr_cols);

	for (int j = 0; j < ycols; j++) {
		double *yy = va_arg(list, double*);
		for (int i = 0; i < n; i++) ydata[j][i]=yy[i];
	}
	// Cleanup the va_list when we're done.
	va_end(list);

	loaded=0;
}

/**
 * name: mylibs::Fl_Gnuplot::add_2d_data_columns()
 *
 * Adds a complete column to the plot.
 *
 * \Attention The the user can only submit the y-values here.
 * The x-values are defined in Fl_Gnuplot::set_2d_data_columns. The
 * number of y-values MUST match the number of x-values!
 *
 * @param nr_cols: The number of columns
 * @return Nothing
 *
 **/
void Fl_Gnuplot::add_2d_data_columns(int nr_cols, ...){
	// adds complete columns

	if ( ycols == 0 )
		throw myexception(EXCEPTION_ID+"Before calling "
							+ __FUNCTION__ + "() you have to call set_2d_data_columns() !");

	Polar=0;
	int old_ycols 	 = ycols;
	ycols 			+= nr_cols;
	currentIndex 	= ycols-1;

	int n = ndata[0];


	// x-axis needs not to be re-alloc'ed
	ydata  = (double **)realloc(ydata ,(ycols)*sizeof(double *));
	titles = (char **  )realloc(titles,(ycols)*sizeof(char*   ));
	with   = (With *   )realloc(with  ,(ycols)*sizeof(With    ));
	ndata  = (int *    )realloc(ndata ,(ycols)*sizeof(int     ));

	if (!xdata || !ydata || !ndata  || !titles || !with) {
		fprintf(stderr,"Error re-allocating xdata or ydata or ndata, titles arrays\n"); exit(0);
	}

	for (int i = old_ycols; i < ycols; i++) { // alloc memory for the new columns
//		memory_allocation_columns(i, n);
		ydata[i]=(double *)malloc(n*sizeof(double));
		if (!ydata[i]) {
			fprintf(stderr,"Error allocating ydata[] arrays\n"); exit(0);
		}
		titles[i]= NULL;
		ndata[i] = n;
		with[i]  = PLOT_WITH_POINTS;
	}


	// We access the ellipses through a va_list, so let's declare one
	va_list list;

	// We initialize the va_list using va_start.  The first parameter is
	// the list to initialize.  The second parameter is the last non-ellipse
	// parameter.
	va_start(list, nr_cols);

	for (int j = old_ycols; j < ycols; j++) {
		double *yy = va_arg(list, double*);
		for (int i = 0; i < n; i++) ydata[j][i]=yy[i];
	}
	// Cleanup the va_list when we're done.
	va_end(list);

	loaded=0;
}

/** Fl_Gnuplot::append_2d_data_columns()
 * This function appends one value to the data that is already stored
 * in the object.
 * Data is be organized in columns which share the same the xaxis
 * -- typically the time.
 *
 * In the case the column mode was not activated
 * yet, all data is freed and new arrays are created.
 * @param xx      : value on the x-axis
 * @param nr_cols : How many columns of data will follow?
 * @param ...     : As many double values as specified by nr_cols.
 */
void Fl_Gnuplot::append_2d_data_columns(const double xx, int nr_cols, ...) {
	Polar=0;
	// check if everything was initialized, and do it.
	if ((ycols == 0) or (ycols != nr_cols)) {
		clear_data(); // delete all data in the arrays
		ycols        = nr_cols;
		currentIndex = nr_cols-1;

		memory_allocation_column_mode(ycols);
	}

	int n = ndata[0]; n++;
	xdata[0] =(double *)realloc(xdata[0], n*sizeof(double));
	if (!xdata[0]) {
		fprintf(stderr,"Error allocating xdata[0] array\n"); exit(0);
	}

	for (int i = 0; i < ycols; i++) {
//		memory_allocation_columns(i,n);
		ydata[i]=(double *)realloc(ydata[i],n*sizeof(double));
		if (!ydata[i]) {
			fprintf(stderr,"Error allocating ydata[] arrays\n"); exit(0);
		}
		ndata[i]=n;
	}

	// add the time value of this dataset
	xdata[0][n-1]=xx;

	// We access the ellipse through a va_list, so let's declare one
	va_list list;

	// We initialize the va_list using va_start.  The first parameter is
	// the list to initialize.  The second parameter is the last non-ellipse
	// parameter.
	va_start(list, nr_cols);

	for (int j = 0; j < ycols; j++) {
		double yy = va_arg(list, double);
		ydata[j][n-1]=yy;
	}
	// Cleanup the va_list when we're done.
	va_end(list);

	loaded=0;
	return;
}

/**
 * Fl_Gnuplot::add_2d_data_columns()
 * This function appends one value to the data that is already stored
 * in the object.
 * Data is be organized in columns which share the same the xaxis
 * -- typically the time.
 *
 * \attention: New data values are organized within one array.
 *
 * In the case the column mode was not activated
 * yet, all data is freed and new arrays are created.
 * @param xx      : value on the x-axis
 * @param nr_cols : How many columns of data will follow?
 * @param ...     : As many double values as specified by nr_cols.
 *
 * @param xx      : value on the x-axis
 * @param nr_cols : How many columns of data will follow?
 * @param cc      : Array with as many double values as specified by nr_cols.
 */
void Fl_Gnuplot::append_2d_data_columnsfv(const double xx, int nr_cols, double *cc) {
	Polar=0;
	// check if everything was initialized, and do it.
	if ((ycols == 0) || (ycols != nr_cols)) {
		clear_data(); // delete all data in the arrays
		ycols        = nr_cols;
		currentIndex = nr_cols-1;
		memory_allocation_column_mode(ycols);
	}

	int n = ndata[0]; n++;
	xdata[0] =(double *)realloc(xdata[0], n*sizeof(double));
	if (!xdata[0]) {
		fprintf(stderr,"Error allocating xdata[0] array\n"); exit(0);
	}

	for (int i = 0; i < ycols; i++) {
//		memory_allocation_columns(i,n);
		ydata[i]=(double *)realloc(ydata[i],n*sizeof(double));
		if (!ydata[i]) {
			fprintf(stderr,"Error allocating ydata[] arrays\n"); exit(0);
		}
		ndata[i]=n;
	}

	// set the xdata
	xdata[0][n-1]=xx;
	// set the columns
	for (int j = 0; j < ycols; j++)	ydata[j][n-1]=cc[j];

	loaded=0;
	return;
}


/** name: Fl_Gnuplot::set_2d_titles_columns
 * In column mode there is also the posibility to set titles for each curve.
 * Since the function Fl_Gnuplot::set_2d_data_columns is written such, that it
 * takes a variable argument list, it cannot handle the titles, too. This is
 * done here.
 *
 * The titles are defined in the same order as you defined the data arrays.
 * You don't have to supply a title for every column. If the number of titles
 * is less then the number of columns the last titles we be left empty. If you
 * supply too much titles, then all remaining titles will be ignored.
 *
 * @param nr_titles: The number of titles that follow.
 * @return Nothing.
 **/
void Fl_Gnuplot::set_2d_titles_columns(int nr_titles, ...) { // , const char *title
	// We access the ellipses through a va_list, so let's declare one
	va_list list;

	// We initialize the va_list using va_start.  The first parameter is
	// the list to initialize.  The second parameter is the last non-ellipse
	// parameter.
	va_start(list, nr_titles);

	int max = (nr_titles > ycols) ? ycols : nr_titles;
	for (int i = 0; i < max; i++) {
		char *title = va_arg(list, char*);
		if (title and strlen(title) > 0) {
			if (!titles[i])
				titles[i]=(char *)malloc(MAXPATHLEN*sizeof(char));
			sprintf(titles[i],"%s",title);
		}
	}
	includeKey=1;
	// Cleanup the va_list when we're done.
	va_end(list);
}

void Fl_Gnuplot::set_2d_polar_data(	const double *xx, const double *yy,
                                    int n, const char *title) {
	set_2d_data(xx,yy,n,title);
	Polar = 1;
}

void Fl_Gnuplot::add_2d_polar_data(const double *xx, const double *yy,
                                   int n, const char *title) {
	add_2d_data(xx,yy,n,title);
	Polar = 1;
}


void Fl_Gnuplot::resize(int _x, int _y, int _w, int _h) {
	Fl_Box::resize(_x, _y, _w, _h);
	scaleX = 1/4096.0*w();
	scaleY = 1/4096.0*h();
}

/** name: Fl_Gnuplot::handle
 *	\brief The eventhandler for the class.
 *
 *	This method handles the events of this object. The list of handled events
 *	consists of:
 *	- Push, drag or release of the first mouse button (saving the coordinates).
 *	- Right clicks, which will produce a menu.
 *
 * \param event: Integer, describing the event that should be handled.
 * \return The integer 1, if the event was handled by this method, 0 otherwise.
 */
int Fl_Gnuplot::handle( int event ) {


//	switch (event){
//		case 9: // key down
//			char key = Fl::event_key();
////			cout << event << " " << key << endl;
//			switch (key){
//				case 'i':
//					internal_function_plotter = not internal_function_plotter;
//					return 1;
//					break;
//			}
//			break;
//	}


	if  (Fl::event_button() == 1 and internal_function_plotter) {
		switch (event) {
			case FL_PUSH:
				busy = true;
				mousex1 = Fl::event_x();
				mousey1 = Fl::event_y();
				return 1;
				break;
			case FL_DRAG:
				mousex2 = Fl::event_x();
				mousey2 = Fl::event_y();
				draw();
				return 1;
				break;
			case FL_RELEASE:
				mousex2 = Fl::event_x();
				mousey2 = Fl::event_y();
				cout << mousex1 << " " << mousey2 << " ";
				cout << mousex2 << " " << mousey2 << endl;

				char c[64];
				sprintf(c, "set xrange [%f:%f]",  (mousex1 - x())/scaleX,  ((mousex2-mousex1) - x())/scaleX);
//			sprintf(c, "set xrange [%d:%d]",  mousex1, (mousex2-mousex1));
				printf("%s\n",c);
				mousex1 = -1; mousey1 = -1;
//			command(c);
				draw();
				busy = false;
				return 1;
				break;
		}
	}

	// These lines of code, produce a menu after a right-click with the mouse.
	if (( event==FL_PUSH && Fl::event_button() == 3 )
	        or (event = FL_KEYDOWN && Fl::event_key() == FL_ALT+'m') ) {
		Fl_Menu_Item rclick_menu[] = {
			//{text				, shortcut_  , callback_    ,user_data_, flags, labeltype_, labelfont_, labelsize_, labelcolor_}
			{ "&Gnuplot command",  FL_ALT+'r', gnuplotcom_cb, (void*)1, 0, 0, 0, 12, 0x00000000 },
			{ "&X-axis",		   FL_ALT+'x', set_xaxis_cb,  (void*)2, 0, 0, 0, 12, 0x00000000 },
			{ "&Save", 			   FL_ALT+'s', save_cb, 	  (void*)3, 0, 0, 0, 12, 0x00000000 },
			{ "&set stride", 	   FL_ALT+'+', stride_cb, 	  (void*)4, 0, 0, 0, 12, 0x00000000 },
			{ "&toggle internal plotter", 	   FL_ALT+'i', internal_plotter_cb, (void*)5, 0, 0, 0, 12, 0x00000000 },
			{ 0, 0, NULL, NULL, 0, 0, 0, 1, 0x00000000 }// Menu ends with a label that is null.
		};
		const Fl_Menu_Item *m = rclick_menu->popup(Fl::event_x(), Fl::event_y(), 0, 0, 0);
		if ( m ) m->do_callback(this, (void*)m);
		return 1;
	}


	return 0;
}

void Fl_Gnuplot::save_2d_data_raw(const char *filename) {
	FILE *f = fopen(filename, "w");
	if (ycols == 1) {
		for(size_t i=0 ; i< (size_t) currentIndex +1; i++) {
			for(size_t j=0 ; j< (size_t) ndata[i] ; j++) {
				fprintf(f, "%10.10E %10.10E\n", xdata[i][j],ydata[i][j]);
			}
			fprintf(f, "\n\n");
		}
	} else { // column mode
		for(size_t j=0 ; j< (size_t) ndata[0] ; j++) {
			fprintf(f, "%10.10E", xdata[0][j]);
			for(size_t i=0 ; i< (size_t) ycols; i++) {
				fprintf(f, " %10.10E", ydata[i][j]);
			}
			fprintf(f, "\n");
		}
	}
	fclose(f);
}

void Fl_Gnuplot::save_2d_data(const char *filename) {
	char fn[512] = "\0";
	strncpy(fn, filename, 512);
	fl_filename_relative(fn,512, fn);

	if (ycols == 1) {
		FILE *f = fopen(filename, "w");
		fprintf(f,"# Plot this file using $ gnuplot '%s'\n", fn);
		fprintf(f,"# Before publication remind to set an appropriate title and axes labels.\n\n");
		fprintf(f, "set terminal postscript landscape enhanced  color 20 dl 2.5 lw 3.0\n\n");
		fprintf(f,"# for black and white just erase 'color'\n");
		fprintf(f, "#set terminal postscript landscape enhanced  20 dl 2.5 lw 3.0\n\n");

		// other terminals
		//fprintf(f,"set terminal svg size 800,600 enhanced\n");
		//fprintf(f,"set terminal png large size 800,600 xFFFFFF\n");

		fl_filename_setext(fn, ".pdf");
		fprintf(f,"set output '| ps2pdf - %s'\n", fn);

		if (Polar) fprintf(f,"\nset polar\n");

		fprintf(f,"set xlabel '%s'\n", xlab.c_str());
		fprintf(f,"set ylabel '%s'\n", ylab.c_str());
		fprintf(f,"set title '%s'\n",Title.c_str());

		if (GPcommand.size() != 0) {
			fprintf(f,"%s\n",GPcommand.c_str());
		}
		//! the trick using '-' as input allows to use the same file for data and
		//! commands
		if (titles[0])
			fprintf(f, "plot '-' title '%s' w %s", titles[0], plot_with(with[0]));
		else
			fprintf(f, "plot '-' notitle w %s", plot_with(with[0]));

		for (int i = 0; i < currentIndex; i++) {
			if (titles[i+1])
				fprintf(f, ", '-' title '%s' w %s", titles[i+1], plot_with(with[i+1]));
			else
				fprintf(f, ", '-' notitle w %s", plot_with(with[i+1]));
		}
		fprintf(f,"\n");

		for(size_t i=0 ; i< (size_t) currentIndex +1; i++) {
			for(size_t j=0 ; j< (size_t) ndata[i] ; j++) {
				fprintf(f, "%+10.10E %+10.10E\n", xdata[i][j],ydata[i][j]);
			}
			fprintf(f, "end\n");
		}
		fclose(f);


		int ok = 1;
		for(size_t i=1 ; i<= (size_t) currentIndex; i++) {
			if (ndata[i] != ndata[0]) {ok = 0; break;}
		}

		if (ok > 0) {
			char fn[512];
			strncpy(fn, filename, 512);
			strncat(fn, ".raw", 512);
			f = fopen(fn, "w");

			for(size_t j=0 ; j< (size_t) ndata[0] ; j++) {
				fprintf(f, "%+10.10E ", xdata[0][j]);
				for(size_t i=0 ; i< (size_t) currentIndex +1; i++) {
					fprintf(f, "%+10.10E ", ydata[i][j]);
				}
				fprintf(f, "\n");
			}
			fclose(f);
		}

		char command[512] = "\0";
		sprintf(command, "gnuplot '%s'\n", filename);
		system(command);
	} else { // column mode

		save_2d_data_raw(filename);
		char gfn[512] = "\0";
		strncpy(gfn, filename, 512);
		fl_filename_setext(gfn, ".gnuplot");
		FILE *f = fopen(gfn, "w");
		fprintf(f,"# Plot this file using $ gnuplot '%s'\n", fn);
		fprintf(f,"# Before publication remind to set an appropriate title and axes labels.\n\n");
		fprintf(f, "set terminal postscript landscape enhanced  color 20 dl 2.5 lw 3.0\n\n");
		fprintf(f,"# for black and white just erase 'color'\n");
		fprintf(f, "#set terminal postscript landscape enhanced  20 dl 2.5 lw 3.0\n\n");

		fl_filename_setext(fn, ".eps");
		fprintf(f,"set output '%s'\n", fn);

		if (Polar) fprintf(f,"\nset polar\n");

		fprintf(f,"set xlabel '%s'\n", xlab.c_str());
		fprintf(f,"set ylabel '%s'\n", ylab.c_str());
		fprintf(f,"set title '%s'\n",Title.c_str());

		if (GPcommand.size() != 0) {
			fprintf(f,"%s\n",GPcommand.c_str());
		}

		if (xaxis > 0 && xaxis <= currentIndex+1) {
			if (currentIndex == 1 && xaxis == 1) {
				fprintf(f,"set title '%s vs. %s'\n", titles[1], titles[xaxis-1]);
				fprintf(f,"set xlabel '%s'\n", titles[xaxis-1]);
				fprintf(f,"set ylabel '%s'\n",titles[1]);
			} else if (currentIndex == 1 && xaxis == 2) {
				fprintf(f,"set title '%s vs. %s'\n", titles[0], titles[xaxis-1]);
				fprintf(f,"set xlabel '%s'\n", titles[xaxis-1]);
				fprintf(f,"set ylabel '%s'\n",titles[0]);
			} else {
				fprintf(f,"set title 'data vs. %s'\n", titles[xaxis-1]);
				fprintf(f,"set xlabel '%s'\n", titles[xaxis-1]);
				fprintf(f,"set ylabel 'data'\n");
			}
		}

		fl_filename_relative(fn,512, filename);
		if (xaxis==0) {
			fprintf(f, "plot '%s' u 1:2 title '%s' w %s", fn, titles[0], plot_with(with[0]));
			for (int i = 1; i <= currentIndex; i++) {
				if (titles[i])
					fprintf(f, ", '' u 1:%d title '%s' w %s", i+2, titles[i],plot_with(with[i]));
				else
					fprintf(f, ", '' u 1:%d notitle w %s", i+2, plot_with(with[i]));
			}
			fprintf(f,"\n");
		} else { // xaxis is different
			int start = 1;
			if (xaxis != 1) {
				if (titles[2])
					fprintf(f, "plot '%s' u %d:2 title '%s' w %s", fn,xaxis+1, titles[0], plot_with(with[0]));
				else
					fprintf(f, "plot '%s' u %d:2 notitle w %s", fn,xaxis+1, plot_with(with[0]));
			} else {
				if (titles[3])
					fprintf(f, "plot '%s' u 2:3 title '%s' w %s", fn, titles[1], plot_with(with[1]));
				else
					fprintf(f, "plot '%s' u 2:3 notitle w %s", fn, plot_with(with[1]));
				start = 2;
			}
			for (size_t i = start; i <= (size_t) currentIndex; i++) {
				if (size_t(xaxis+1) == i+2) continue;

				if ( titles[i+2] )
					fprintf(f, ", '' u %d:%d title '%s' w %s", xaxis+1,(int) i+2, titles[i], plot_with(with[i]));
				else
					fprintf(f, ", '' u %d:%d notitle w %s", xaxis+1,(int) i+2, plot_with(with[i]));
			}
			fprintf(f,"\n");
		}
		fclose(f);

		char command[512] = "\0";
		sprintf(command, "gnuplot '%s'\n", gfn);
		int res = system(command);


		fl_filename_relative(fn,512, filename);
		fl_filename_setext(fn, ".eps");
		sprintf(command, "ps2pdf '%s' && rm %s\n", fn, fn);
		res = system(command);
	}


}
}; // end of namespace mylibs
