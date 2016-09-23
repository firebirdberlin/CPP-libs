//      xydata.cpp
//
//      Copyright 2012 Stefan Fruhner <stefan.fruhner@gmail.com>
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


#include "xydata.hpp"

using namespace std;

/** \fn XYData::XYData(double *x, double *y, size_t sz)
 * \brief Constructor, that takes x- and y-values and the size.
 *
 * \param x, y: Double pointers to arrays where the x-/y-data can be found,
 *				respectively. These are not copied, just the pointers
 * 				are stored.
 * \param sz: 	The size (number of elements) of the arrays.
 */
XYData::XYData(const double *x,const double *y, size_t sz)
	: xdata((double*)x), ydata((double*)y), len(sz), local_data(false) {

	set_search_range();
}

/** \fn XYData::XYData(const char*)
 * (Stefan Fruhner - 30.01.2012 13:54:49 CET)
 * Reads in xy data from a simple text file of the format:
\verbatim
# xvalues	 yvalues
0.	10.
1.	12.
...
\endverbatim
 * The function skips complete lines on conversion error, especially lines
 * starting with #.
 * Memory is allocated in blocks of 1000 elements to reduce memory allocation
 * operations.
 *
 * @param filename: C-string, that contains the name of the file (and the path)
 *				from which to read the data.
 **/
XYData::XYData(const char* filename)
	: xdata(NULL), ydata(NULL), len(0), local_data(true) {

	vector<double> yd(1000, 0.);
	vector<double> xd(1000, 0.);

	fstream in(filename);
	if (in.fail()) throw XYData::Exception_IO(EXCEPTION_ID+ +"'"+toString(filename)+"'"+" failed !");

	in.flags(std::ios_base::skipws);
	size_t sz = 0;
	char junk[1024];
	do{
		in >> dec >> xd[sz] >> yd[sz];
		if (in.fail()) {						// on failure skip the complete line
			in.clear();
			in.getline(junk, 1024);
			continue;
		}
//		cout << sz << " " <<  xd[sz] << " " << yd[sz] << endl;
		sz++;
		if (sz == xd.size()){
			xd.resize(1000 + xd.size(), 0.);
			yd.resize(1000 + yd.size(), 0.);
		}
	} while (not in.eof());

	len = sz;

	if (len == 0) throw XYData::Exception_IO(EXCEPTION_ID+ "\n\
No data read. Format should be : \n\
# xvalues	 yvalues\n\
0.	10.\n\
1.	12.\n");

	xd.resize(len);
	yd.resize(len);

	xdata = new double[len];
	memcpy(xdata, &xd[0], len *sizeof(double));
	xd.clear();

	ydata = new double[len];
	memcpy(ydata, &yd[0], len*sizeof(double));
	yd.clear();

	in.close();

	set_search_range(xdata[0], xdata[len-1]);	// init range
}

/** \fn XYData::~XYData()
 * \brief Destructor, if memory was allocated, it is here freed.
 */
XYData::~XYData() {
	clear();
}

void XYData::clear(){
	if (local_data){
		delete[] xdata; xdata = NULL;
		delete[] ydata; ydata = NULL;
	}
}

const XYData::Range XYData::xrange() const {
	return XYData::Range(xdata[0], xdata[len-1]);
}

/**
 * name: XYData::yrange()
 *
 * Determines the yrange of the complete data.
 *
 * @return const XYData::Range;
 *
 **/
const XYData::Range XYData::yrange() const {
	return XYData::Range(min(), max());
}

void XYData::set_search_range(const double lower, const double upper){
	search_range.min = lower;
	search_range.max = upper;
}

void XYData::set_search_range(const XYData::Range &range){
	search_range.min = range.min;
	search_range.max = range.max;
}

bool XYData::in_range(double x){
	return (x >= search_range.min and x <= search_range.max);
}

/** \fn double XYData::y(double)
 * (Stefan Fruhner - 02.02.2012 10:38:09 CET)
 * \brief Computes f(x) for a given x using linear interpolation.
 *
 * @param x: Location where to compute f(x).
 * @return The function value f(x) as double.
 **/
double XYData::y(double x) {
	for (size_t i = 0; i < len - 1; i++){
		if ( x >= xdata[i] and x < xdata[i+1])
			return ( ydata[i+1] - ydata[i] ) / ( xdata[i+1] - xdata[i] ) * (x-xdata[i]) + ydata[i];
	}
	throw Exception_Range(EXCEPTION_ID);
	return 666.;
}

/** \fn double XYData::operator[](double)
 * (Stefan Fruhner - 02.02.2012 10:38:09 CET)
 * \brief Returns f(x) for a given x index.
 *
 * @param x: x index of f(x)
 * @return a reference to the function value f(x).
 **/
double& XYData::operator[](size_t x) {
	if (x < len) return ydata[x];

	throw Exception_Range(EXCEPTION_ID);
	return ydata[0];
}


/** \fn double XYData::x(double)
 * \brief Returns the x-value for a given x index.
 *
 * @param x: x index of f(x)
 * @return Reference to the x value
 **/
double& XYData::x(size_t x_index) {
	if (x_index < len) return xdata[x_index];

	throw Exception_Range(EXCEPTION_ID);
	return xdata[0];
}


/** \fn set<double> XYData::x(double, Flags)
 * (Stefan Fruhner - 02.02.2012 10:38:09 CET)
 * Locates where the function f(x) equals a given value y using linear
 * interpolation.
 *
 * \note You can limit the xrange which is considered by calling
 *       the method set_search_range(a,b) to an appropriate interval.
 *
 * @param y 	: function value
 * @param flag	: Allowed values {YXData::none, XYData::rising, XYData::falling}.
 * 				  Only return a result if the slope {doesn't matter, rises,
 * 				  falls}.
 * @return A set of locations x where f(x) = y.
 **/
set<double> XYData::x(double y, Flags flag ){
	set<double> p;

	switch (flag){
	case XYData::rising:
		for (size_t i = 0; i < len - 1; i++){
			if (y >= ydata[i] and y < ydata[i+1]){
				double v = xdata[i] + (y - ydata[i]) * ( xdata[i+1] - xdata[i] ) / ( ydata[i+1] - ydata[i] );
				if (in_range(v)) p.insert(v);
			}
		}
		break;
	case XYData::falling:
		for (size_t i = 0; i < len - 1; i++){
			if (y <= ydata[i] and y > ydata[i+1]){
				double v = xdata[i] + (y - ydata[i]) * ( xdata[i+1] - xdata[i] ) / ( ydata[i+1] - ydata[i] );
				if (in_range(v)) p.insert(v);
			}
		}
		break;
	default: // both cases
		for (size_t i = 0; i < len - 1; i++){
			if (	(y <= ydata[i] and y > ydata[i+1])
				or 	(y >= ydata[i] and y < ydata[i+1])
				) {
				double v = xdata[i] + (y - ydata[i]) * ( xdata[i+1] - xdata[i] ) / ( ydata[i+1] - ydata[i] );
				if (in_range(v)) p.insert(v);
			}
		}
	}

	return p;
}

size_t XYData::xindex(double xvalue) const{
	for (size_t i = 0; i < len - 1; i++){
		if (xvalue >= xdata[i] and xvalue < xdata[i+1]) return i;
	}
	throw Exception_Range(EXCEPTION_ID);
	return 0;
}

/**
 * name: XYData::min()
 * Find the (global) minimum of the function.
 * @return minimum value
 **/
double XYData::min() const{
	return mylibs::algorithm::min(ydata, len);
}

/**
 * name: XYData::min()
 * Find the (global) minimum of the function.
 * @return minimum value
 **/
double XYData::min(const Range &xrange){
	size_t start = xindex(xrange.min);
	size_t end	 = xindex(xrange.max);
	if (not end > start){
		throw Exception_Range(EXCEPTION_ID);
	}

	return mylibs::algorithm::min(&(ydata[0])+start, end-start);
}

/**
 * name: XYData::max()
 * Find the (global) maximum of the function.
 * @return maximum value
 **/
double XYData::max() const{
	return mylibs::algorithm::max(ydata, len);
}

/**
 * name: XYData::max()
 * Find the (global) maximum of the function.
 * @return maximum value
 **/
double XYData::max(const XYData::Range &xrange) {

	size_t start = xindex(xrange.min);
	size_t end	 = xindex(xrange.max);
	if (not end > start){
		throw Exception_Range(EXCEPTION_ID);
	}

	return mylibs::algorithm::max(&(ydata[0])+start, end-start);
}

/**
 * name: XYData::min_element()
 * Find the (global) minimum of the function.
 * @return Index of the item with the minimum value
 **/
size_t XYData::min_element() const{
	return mylibs::algorithm::min_element(ydata, len);
}

/**
 * name: XYData::min_element()
 * Find the (global) minimum of the function.
 * @return minimum value
 **/
size_t XYData::min_element(const XYData::Range &xrange) {

	size_t start = xindex(xrange.min);
	size_t end	 = xindex(xrange.max);
	if (not end > start){
		throw Exception_Range(EXCEPTION_ID);
	}

	size_t mine = mylibs::algorithm::min_element(&(ydata[0])+start, end-start);
	return mine+start;
}

/**
 * name: XYData::max_element()
 * Find the (global) maximum of the function.
 * @return Index of the item with the maximum value
 **/
size_t XYData::max_element() const{
	return mylibs::algorithm::max_element(ydata, len);
}

/**
 * name: XYData::max_element()
 * Find the (global) maximum of the function.
 * @return maximum value
 **/
size_t XYData::max_element(const XYData::Range &xrange) {

	size_t start = xindex(xrange.min);
	size_t end	 = xindex(xrange.max);
	if (not end > start){
		throw Exception_Range(EXCEPTION_ID);
	}

	size_t maxe = mylibs::algorithm::max_element(&(ydata[0])+start, end-start);
	return maxe+start;
}


/**
 * name: XYData::extremum_element()
 * Find the (global) maximum of the function.
 * @return Index of the item with the maximum value
 **/
size_t XYData::extremum_element() const{
	size_t max = max_element();
	size_t min = min_element();

	if (fabs(ydata[max]) > fabs(ydata[min])) return max;
	return min;
}

/**
 * name: XYData::extremum_element()
 * Find the extremum of the function within a certain range.
 * @return Index of the extremum.
 **/
size_t XYData::extremum_element(const XYData::Range &xrange) {
	size_t max = max_element(xrange);
	size_t min = min_element(xrange);

	if (fabs(ydata[max]) > fabs(ydata[min])) return max;
	return min;
}


void XYData::moving_average(size_t window) const {
	return mylibs::algorithm::moving_average(ydata, len, window);
}

pair<size_t,double *> XYData::average_triggered(vector<size_t> triggers,
												const size_t offset_negative,
												const size_t offset_positive){
	if (triggers.size() < 2){
		cerr << EXCEPTION_ID << "At least two triggers needed" << endl;
		return pair<size_t, double*>(0,NULL);
	}

	if (offset_negative + offset_positive >= len){
		cerr << EXCEPTION_ID << "Offset is larger then the complete length of the data, please reduce it." << endl;
		return pair<size_t, double*>(0,NULL);
	}

	// step 1 : find the maximum and minimum distance
	size_t *t = &(triggers[0]);
	size_t mind = t[1] - t[0];
	for (size_t i = 2; i < triggers.size() ; i++){
		if ( t[i] <= t[i-1] ) {
			cerr << EXCEPTION_ID << "Triggers must be consecutive !!!" << endl;
			return pair<size_t, double*>(0,NULL);
		}
		mind = (t[i] - t[i-1]) < mind ? (t[i] - t[i-1]) : mind;
	}

	// allocate memory for the smalles distance between the triggers
	// this is the smallest length which can be averaged
	size_t items = mind+offset_negative+offset_positive;
	double *out = new double[items];
	// initialize
	#pragma omp parallel for
	for (size_t i = 0; i < items; i++) out[i] = 0.;

	// find the first trigger we can use
	size_t valid_start_trigger = 0;
	for (size_t i = 0; i < triggers.size() ; i++){
		if (t[i] < offset_negative) continue;
		valid_start_trigger = i;
		break;
	}

	// find the last trigger we can use
	size_t valid_end_trigger = triggers.size()-1;
	for (size_t i = triggers.size(); i > 0 ; i--){
		if (t[i-1] + offset_positive + mind >= len) continue;
		valid_end_trigger = i-1;
		break;
	}

	if (valid_end_trigger <= valid_start_trigger+1) {
		cerr << EXCEPTION_ID << "Offsets to large ? Trigger difference seems to be too small !!!" << endl;
		return pair<size_t, double*>(0,NULL);
	}

	// For each valid trigger ...
	for (size_t i = valid_start_trigger; i <= valid_end_trigger ; i++){
		size_t idx = t[i] - offset_negative;
		// ... add up the interval that belongs to it
		for (size_t j = 0; j < items; j++){ // attention positive offset is already included in items
			out[j] += ydata[idx + j];
		}
	}

	// compute the average value
	for (size_t i = 0; i < items; i++){
		out[i] /= (double) ( valid_end_trigger - valid_start_trigger + 1 );
	}

	return pair<size_t, double*>(items,out);
}


/** \fn double XYData::slope(double) const
 * (Stefan Fruhner - 02.02.2012 10:38:09 CET)
 * Compute the slope at position x by linear interpolation.
 * @param x: x as in f'(x)
 * @return Value of the slope.
 **/
double XYData::slope(double x) const {
	size_t i = xindex(x);
	return ( ( ydata[i+1] - ydata[i] ) / ( xdata[i+1] - xdata[i] ) );
}

XYData::Flags XYData::slope_type(double x) const {
	double s = slope(x);

	if (s > 0. ) return XYData::rising;
	if (s < 0. ) return XYData::falling;

	return XYData::none; // saddle point

}

/** \fn list<XYData::Range> XYData::find_step(XYData::Range)
 * (Stefan Fruhner - 02.02.2012 10:38:09 CET)
 * Assuming the dataset contains step functions find_step() tries to locate
 * them. Given a lower and an upper threshold value it returns the corresponding
 * x values of the thresholds, if the slope in both points has the same sign.
\verbatim
   ^
  y|----                -------------
   |    \    f(x)      /
   |     \            /
   |      \          /
   |       \        /
   |        --------
   |__________________________________
		|  |         |  |            x
	   max min      min max
\endverbatim
 *
 * @param yrange: XYData::Range containing threshold values defining the step.
 * @param flag  : Allowed values {YXData::none, XYData::rising, XYData::falling}.
 * 				  Only return a result if the slope {doesn't matter, rises,
 * 				  falls}.
 * @return 	A list of XYData::Range, where the first value corresponds to the
 * 			lower threshold and the second to the upper one.
 *
 **/
list<XYData::Range> XYData::find_step(XYData::Range yrange, XYData::Flags flag){

	list<XYData::Range> result;
	set_search_range(xdata[0], xdata[len-1]);	// reset the search range

	set<double> lb = this->x(yrange.min);		// find all points with f(x) = lower value

	double left 	= this->xmin();
	double right	= this->xmax();
	for (set<double>::iterator it = lb.begin(); it != lb.end(); ++it){ // iter through lower values
		XYData::Flags f = this->slope_type(*it);
		set<double>::iterator itn = it; itn++;	// next value
		set<double>::iterator itp = it; 	  	// previous value of lower boundary

		if (it  != lb.begin()) left = *(--itp); else left  = this->xmin();
		if (itn != lb.end())  right = *(itn);   else right = this->xmax();

		switch (f){
			case XYData::rising :	// upper val should be at higher x
				if (flag & ~XYData::rising) continue;  // if we don't search for positive slopes ... continue
				this->set_search_range(*it    , right);
				break;

			case XYData::falling:	// upper val should be at lower x
				if (flag & ~XYData::falling) continue; // if we don't search for negative slopes ... continue
				this->set_search_range(left,     *it);
				break;
			default: 				// oh oh ... not really considered this case
				this->set_search_range();
				break;				// unset range
		}

		set<double> ub = this->x(yrange.max, f);// find points with upper
		if (ub.size() > 0) {					// value that have the matching slope
			set<double>::iterator uit = ub.begin();
			switch (f){
				case XYData::none	:
				case XYData::rising : uit = ub.begin();	     break;	// upper val should be at higher x
				case XYData::falling: uit = ub.end(); uit--; break;	// upper val should be at lower x
			}


			result.push_back(XYData::Range(*it, *uit));	// add to the list of found steps

		}
	}
	set_search_range(xdata[0], xdata[len-1]);	// reset the search range
	return result;

}

/** \fn list<XYData::Range> XYData::find_peak(XYData::Range)
 * (Stefan Fruhner - 02.02.2012 10:38:09 CET)
 * Assuming the dataset contains peaks or valleys this method tries to
 * locate them. Therefore a given threshold value must be exceeded
 * consecutively with alternating slope.
 *
 * The parameter 'flag' can be used to to only find peaks or valleys.
 * It denotes the slope of the first point of the peak/valley.
 *
 * A flag equal to XYData::falling would describe the following situation:
\verbatim
   ^
  y|----                -------------
   |    \    f(x)      /
   | ----\------------/---- threshold
   |      \          /
   |       \        /
   |        --------
   |__________________________________
         |          |                 x ->
        min        max
\endverbatim
 * A flag equal to XYData::rising would describe the following situation:
\verbatim
   ^
  y|
   |        --------
   |       / f(x)   \
   | -----/----------\---- threshold
   |     /            \
   |    /              \
   |----                -------------
   |__________________________________
          |           |               x ->
         min         max
\endverbatim

 *
 * @param yrange: XYData::Range containing threshold values defining the step.
 * @param flag  : Allowed values {YXData::none, XYData::rising, XYData::falling}.
 * 				  Only return a result if the slope of the first point
 * 				  defining the peak {doesn't matter, rises,
 * 				  falls}.
 * @return 	A list of XYData::Range, where the first value corresponds to the
 * 			lower threshold and the second to the upper one.
 *
 **/
list<XYData::Range> XYData::find_peak(const double& threshold, XYData::Flags flag){

	list<XYData::Range> result;
	set_search_range(xdata[0], xdata[len-1]);	// reset the search range

	set<double> lb = this->x(threshold);		// find all points with f(x) = lower value

//	double right	= this->xmax(); // x value of right neighbour
	for (set<double>::iterator it = lb.begin(); it != lb.end(); ++it){ // iter through lower values

		XYData::Flags slope_this = this->slope_type(*it); // rising or falling ?
		if (flag & ~slope_this) continue; // skip this point, if the slope is not what we search for

		set<double>::iterator itn = it; itn++;	// next value
		if (itn == lb.end()) break; 			// reached the end of the list

//		right = *(itn);							// x value of next candidate
		XYData::Flags slope_next = this->slope_type(*itn); // next candidate : rising or falling ?

		switch (slope_this){
			case XYData::rising :	// next point should have negative slope
				if (slope_next != XYData::falling) continue;
				result.push_back(XYData::Range(*it, *itn));	// add to the list of found peaks
				break;

			case XYData::falling:	// next point should have a rising slope
				if (slope_next != XYData::rising) continue;
				result.push_back(XYData::Range(*it, *itn));	// add to the list of found peaks
				break;
			default: 				// oh oh ... not really considered this case
				this->set_search_range();
				throw myexception(EXCEPTION_ID + "This case should never happen (... saddle point ?).");
				break;				// unset range
		}
	}
	set_search_range(xdata[0], xdata[len-1]);	// reset the search range
	return result;
}

list<size_t> XYData::Coords2Index(const list<XYData::Range> &l){
	list<size_t> idx;
	for (list<XYData::Range>::const_iterator it = l.begin(); it != l.end(); it++){
		idx.push_back(xindex(it->min));
	}
	return idx;

}

// mean value of the ydata
double XYData::mean() const{
	double sum   = 0.;
	for (size_t i = 0; i < len; ++i) sum += ydata[i];

	return (sum/(double)len);
}

//Standard deviation of the ydata
double XYData::stddev() const{
	double sum   = 0.;
	double sumsq = 0.;

	for (size_t i = 0; i < len; ++i) {
		sum   += ydata[i];
		sumsq += ydata[i]*ydata[i];
	}

	return sqrt( ( sumsq - (sum*sum) /(double)len ) / (len-1));
}

double XYData::MeanSquaredError() const{
	return (stddev() / sqrt(len));
}

#ifdef HAVE_LIBGSL
/** \fn XYData::LinLSQ XYData::LinearLeastSquaresFit()
 * Computes a linear least squares fit of the function. The fit function can be
 * computed using fit(x) = c0 + c1 * x
 * Further information:
 * http://www.gnu.org/software/gsl/manual/html_node/Fitting-Overview.html
 *
 * @return The linear least squares fit as XYData::LinLSQ object.
 **/
XYData::LinLSQ XYData::LinearLeastSquaresFit(){
	XYData::LinLSQ lsf;


//	gsl_fit_linear (const double * x, const size_t xstride, const double * y, const size_t ystride, size_t n, double * c0, double * c1, double * cov00, double * cov01, double * cov11, double * sumsq)
	gsl_fit_linear(xdata, 1,ydata, 1, len, &lsf.c0, &lsf.c1, &lsf.cov00, &lsf.cov01, &lsf.cov11, &lsf.sumsq);

//	cout << "c0 == " << c0 << endl;
//	cout << "c1 == " << c1 << endl;
//	cout << "cov00  == " << cov00 << endl;
//	cout << "cov01  == " << cov01 << endl;
//	cout << "cov11  == " << cov11 << endl;

//	cout << "sigma1 = " << sqrt(cov00)<<endl;
//	cout << "sigma2 = " << sqrt(cov11)<<endl;

//	double r = cov01 /sqrt( cov00 * cov11);
//	cout << "r = " << r << endl;

//	double rsq = (cov01*cov01) / (cov00 * cov11);
//	cout << "rsq = " << rsq << endl;

	return lsf;
}

/** name: XYData::compute_spline()
 *
 * Computes a spline for a given PointCurve in 3D. One can select which fraction
 * of points of this curve should be used for interpolation.
 *
 * This function is also computes 1st od 2nd derivatives with respect to the
 * arc-length of the curve.
 *
 * @param fraction   : fraction of point which should be used for interpolation [0., 1.] (standard is 0.3)
 * @param resolution : resolution of the interpolated curve
 * @param deriv      : Which derivative should be computed ? (0: only spline,
 *                     1: spline of 1st derivative , 2: spline of 2nd derivative )
 * @param save_files : For debugging reasons the files curve.dat and spline.dat can be created to plot the curves.
 * @return A list of Points sorted by arc length (= PointCurve)
 **/
XYData::CSpline XYData::compute_spline(){
	CSpline cspline(xdata, ydata, len);
	cspline.compute();
	return cspline;
}
#endif // gsl present

/** name: XYData::detect_peaks()
 *
 * Detects peaks in noisy signal using a threshold value 'delta'.
 * Differences smaller the delta are not reported as peak.
 *
 * This code was modified from
 * https://github.com/xuphys/peakdetect/blob/master/peakdetect.c
 * Copyright 2011 Hong Xu. All rights reserved.
 *
 * @param max_peaks : vector<size_t> where to store the maximum peaks
 * @param min_peaks : vector<size_t> where to store the minimum peaks
 * @param delta     : Delta value in y for distinguishing peaks (default 0. (=off))
 * @param max_first : search for positive peaks first before detcting negative ones.
 **/
int XYData::detect_peaks(
		vector<size_t> &max_peaks,
		vector<size_t> &min_peaks,
		double delta, /* delta used for distinguishing peaks */
		bool max_first /* should we search emission peak first of absorption peak first? */
		)
{

	if (len == 0) { throw mystring("No data loaded !!!"); return 1;}
	bool is_detecting_max = max_first;
	max_peaks.clear();
	min_peaks.clear();

	size_t i 		= 0;
	size_t mx_pos 	= 0;
	size_t mn_pos 	= 0;
	double mx 		= ydata[0];
	double mn 		= ydata[0];

	for(i = 1; i < len; ++i){
		if(ydata[i] > mx){	mx_pos 	= i; mx = ydata[i]; }
		if(ydata[i] < mn){	mn_pos 	= i; mn = ydata[i];	}

		if(is_detecting_max && ydata[i] < mx - delta){
			max_peaks.push_back(mx_pos);

			is_detecting_max = false;

			i = mx_pos - 1;
			mn 	   = ydata[mx_pos];
			mn_pos = mx_pos;
		}
		else if( (not is_detecting_max) && ydata[i] > mn + delta) {
			min_peaks.push_back(mn_pos);

			is_detecting_max = true;

			i = mn_pos - 1;
			mx 	   = ydata[mn_pos];
			mx_pos = mn_pos;
		}
	}

	return 0;
}

void XYData::info(const char *label) const{
	if (label) mylibs::cmdline::section(label);
	cout << "[ ";
	if (len > 7){
		for (size_t i = 0; i < 3; i++)
			cout << xdata[i] << ", ";
		cout << ", ... , ";
		for (size_t i = len-4; i < len-1; i++)
			cout << xdata[i-1] << ", ";
	}
	else {
		for (size_t i = 0; i < len-1; i++)
			cout << xdata[i] << ", ";
	}
	cout << xdata[len-1] << " ]" << endl;

	cout << "[ ";
	if (len > 7){
		for (size_t i = 0; i < 3; i++)
			cout << ydata[i] << ", ";
		cout << ", ... , ";
		for (size_t i = len-4; i < len-1; i++)
			cout << ydata[i-1] << ", ";
	}
	else {
		for (size_t i = 0; i < len-1; i++)
			cout << ydata[i] << ", ";
	}
	cout << ydata[len-1] << " ]" << endl;

}
