//		./Fl_Gnuplot.hpp
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
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA.
*/

#ifndef _FL_GNUPLOT_HEADER_
#define _FL_GNUPLOT_HEADER_

#include <sys/param.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h> // variable argument lists
#include "FL/Fl.H"
#include "FL/Fl_Widget.H"
#include "FL/Fl_Box.H"
#include <FL/filename.H>
#include <FL/Fl_File_Chooser.H>
#include <sstream>
#include <mylibs/myio.hpp>
#include <mylibs/xydata.hpp>

using namespace std;

namespace mylibs {

typedef struct RANGE {
	float min;
	float max;
} Range;



/** \class Fl_Gnuplot
 * \brief Widget which provides a gnuplot interface.
 *
 *	This class is derived from Fl_Box and provides an interface for gnuplot.<br>
 *	It provides methods for setting/adding/saving/loading data, for parsing
 *	commands and getting /setting labels and titles.
 *	\note Full gnuplot functionality is not intended.
 *
 */
class Fl_Gnuplot : public Fl_Box {
public:
	//Flags
	enum With {	PLOT_WITH_LINES, PLOT_WITH_DOTS, PLOT_WITH_STEPS,
					PLOT_WITH_ERRORBARS, PLOT_WITH_XERRORBAR, PLOT_WITH_XYERRORLINES,
					PLOT_WITH_POINTS, PLOT_WITH_IMPULSES, PLOT_WITH_FSTEPS,
					PLOT_WITH_ERRORLINES, PLOT_WITH_XERRORLINES,
					PLOT_WITH_YERRORBARS, PLOT_WITH_LINESPOINTS, PLOT_WITH_LABELS,
					PLOT_WITH_HISTEPS, PLOT_WITH_FINANCEBARS, PLOT_WITH_XYERRORBARS,
					PLOT_WITH_YERRORLINES, PLOT_WITH_VECTORS};
	void touch() { loaded=0; }
	int handle(int);

	Fl_Gnuplot(const int x, const int y, const int w, const int h, const char * const l = 0);
	~Fl_Gnuplot();

private:
	void memory_allocation(size_t items);
	void memory_allocation_column_mode(size_t items);
	void memory_allocation_columns(int col, int n);
	void memory_allocation_at_index(int index, int n, const char *title, const With plotwith);
public:

//	void set_filename(const char *f) {
//		strcpy(filename, f); loaded = 0;
//	}

	void draw();
	void resize(int x, int y, int w, int h);

	bool check_data();

	template <class T1, class T2>
	void set_2d_data(const T1 *, const T2 *, int n, const char *label = 0, const With plotwith=PLOT_WITH_LINES);
	template <class T1, class T2>
	void add_2d_data(const T1 *, const T2 *, int n, const char *label = 0, const With plotwith=PLOT_WITH_LINES);

	template <class T1, class T2>
	void set_2d_data(const vector<T1> &xx, const vector<T2> &yy, const char *title, const With plotwith=PLOT_WITH_LINES);
	template <class T1, class T2>
	void add_2d_data(const vector<T1> &xx, const vector<T2> &yy, const char *title, const With plotwith=PLOT_WITH_LINES);

	void set_2d_polar_data(const double *, const double *, int n, const char *label = 0);
	void add_2d_polar_data(const double *, const double *, int n, const char *label = 0);

	void set_2d_data_columns(const double *xx, int n, int nr_cols, ...);
	void set_2d_titles_columns(int nr_titles, ...);

	void add_2d_data_columns(int nr_cols, ...);

	void append_2d_data_columns(const double xx, int nr_cols, ...);
	void append_2d_data_columnsfv(const double xx, int nr_cols, double *cc);

	void save_2d_data_raw(const char *filename);
	void save_2d_data(const char *filename);

	void command(const char *s ){ GPcommand=s; loaded=0;}
	const char* command() const { return GPcommand.c_str(); }

	void x_axis( char const * const s ){ xaxis=atoi(s);}
	void x_axis( const int v ){ xaxis=v;}
	int  x_axis() const { return xaxis; }

	void stride( char const * const s ){ v_stride = (atoi(s) >=1) ? atoi(s): 1;}
	void stride( const int v ){ v_stride=(v >=1) ? v: 1;}
	int  stride() const { return v_stride;}

	void xlabel(const string v){xlab = v;}
	void ylabel(const string v){ylab = v;}
	void title(const string v){Title = v;}

	string xlabel() const {return xlab;}
	string ylabel() const {return ylab;}
	string title()  const {return Title;}

	int  get_nrdata() const {return currentIndex+1; }
	void get_nrdata(int **nd){  *nd = ndata; }
	void get_ydata(double ***yd){ *yd = ydata; }
	void get_xdata(double ***xd){ *xd = xdata; }
	void get_titles(char ***t){ *t = titles; }

	void toggle_internal_plotter(){internal_function_plotter = not internal_function_plotter;}
	bool is_busy() const {return busy;}
	void clear(){clear_data();}

	static const char* plot_with(const With &w);

protected:
	double **xdata;
	double **ydata;
	int 	*ndata;
	With 	 *with;
	int ycols;  //!< determines if column mode is used
	int ncurves;
	char **titles;
	int includeKey;
	int Polar;
	int xaxis; //!< x-axis in column mode
	string GPcommand;
	string xlab;
	string ylab;
	string Title;
	int v_stride;
	int currentIndex;

// // // // // // variables for the internal function plotter
	bool internal_function_plotter;
	bool busy;
	int mousex1, mousey1;
	int mousex2, mousey2;
	float x0; //!< Store where the origin is (0,0)
	float y0;
//	done
//	char filename[MAXPATHLEN];
	int loaded;

	void play() const;
	void load();

	Range minmax(const double *data,const int &ndata);
	void draw_ticks(Range &xr, Range &yr);
	void draw_curve(const double *xdata,const double *ydata, int ndata,
					Range &rx, Range &ry);
	void play_intern();

	double scaleX, scaleY;

	struct OutputEntry {
	public:
		char k;		// type of entry
		int x, y;	// coordinates
		short type; // modifier, e.g. for different symbols
		char *t;	// string
		OutputEntry() : k('G'), x(0), y(0), type(0), t(0) {}
		OutputEntry &operator=(OutputEntry &e) {
			delete [] t;
			k = e.k; x = e.x; y = e.y;
			t = e.t;
			type = e.type;
			e.t = 0;
			return *this;
		}
		~OutputEntry() { delete [] t;}

	private:

		OutputEntry(const OutputEntry &e);
		OutputEntry &operator=(const OutputEntry &e);
	};

	OutputEntry *entries;
	int nEntries;
	int nMaxEntries;
	void clear_data();
	void clear_entries(int release=0);
	void add_entry(char *);

	virtual void build_command(char *);

private:
	Fl_Gnuplot();
};



template <class T1, class T2>
void Fl_Gnuplot::set_2d_data(const T1 *xx, const T2 *yy, int n, const char *title, const With plotwith) {
	clear_data();
	currentIndex = 0;
	Polar        = 0;
	ycols        = 1;

	// allocate memory
	memory_allocation(1);
	memory_allocation_at_index(currentIndex, n, title, plotwith);

	// allocate the data
	for(int i=0 ; i<ndata[currentIndex] ; i++) {
		xdata[currentIndex][i]=(double) xx[i];
		ydata[currentIndex][i]=(double) yy[i];
	}

	check_data();

	loaded=0;
}

template <class T1, class T2>
void Fl_Gnuplot::add_2d_data(	const T1 *xx, const T2 *yy, int n,
								const char *title, const With plotwith) {
	if (ycols != 1) {
		cerr << "Adding of data is forbidden in column mode" << endl;
		exit(-1);
	}
	int i;
	currentIndex++;

	xdata  = (double **)realloc(xdata ,(currentIndex+1)*sizeof(double *));
	ydata  = (double **)realloc(ydata ,(currentIndex+1)*sizeof(double *));
	ndata  = (int *)    realloc(ndata ,(currentIndex+1)*sizeof(int)     );
	with   = (With*)    realloc(with  ,(currentIndex+1)*sizeof(With)    );
	titles = (char **)  realloc(titles,(currentIndex+1)*sizeof(char *)  );
	if (!xdata || !ydata || !ndata || !titles || ! with) {
		fprintf(stderr,"Error re-allocating xdata or ydata or ndata or titles arrays\n"); exit(0);
	}

	memory_allocation_at_index(currentIndex, n, title, plotwith);

	// assignment of data
	for(i=0 ; i<ndata[currentIndex] ; i++) {
		xdata[currentIndex][i]= (double) xx[i];
		ydata[currentIndex][i]= (double) yy[i];
	}

	check_data();

	loaded=0;
}

template <class T1, class T2>
/**
 * name: mylibs::Fl_Gnuplot::set_2d_data()
 *
 * Wrapper method for setting 2D data, when the input is an stl vector.
 *
 * @param xx : x-values
 * @param yy : y-values
 * @param title :
 * @return
 *
 **/
void Fl_Gnuplot::set_2d_data(const vector<T1> &xx, const vector<T2> &yy,
                             const char *title, const With plotwith) {

	if ((xx.size() > 0) and (xx.size() == yy.size()))
		this->set_2d_data(&(xx[0]), &(yy[0]), xx.size(), title, plotwith);
	return;
}


template <class T1, class T2>
/**
 * name: mylibs::Fl_Gnuplot::add_2d_data()
 *
 * Wrapper method for adding 2D data, when the input is an stl vector.
 *
 * @param xx : x-values
 * @param yy : y-values
 * @param title :
 * @return
 *
 **/
void Fl_Gnuplot::add_2d_data(const vector<T1> &xx, const vector<T2> &yy,
                             const char *title, const With plotwith) {

	if ((xx.size() > 0) and (xx.size() == yy.size()))
		this->add_2d_data(&(xx[0]), &(yy[0]), xx.size(), title, plotwith);

	return;
}

} // end of namespace mylibs
#endif // _FL_GNUPLOT_HEADER_
