//		./examples/demo_FFT.cpp
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

#include <math.h>
#include <string>
#include <map>
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Light_Button.H>
#include <FL/Fl_Float_Input.H>
#include <FL/Fl_Int_Input.H>
#include <FL/Fl_Choice.H>

#include <mylibs/myinifiles.hpp>
#include <mylibs/Array.hpp>

#include <myFl/Fl_Gnuplot.hpp>
#include <myFl/Fl_Gnuplot_Window.hpp>
#include <myFl/Fl_Functions.hpp>

#include <myFl/ValueSlider.hpp>
#include <mylibs/FastFourierTransformation.hpp>
#include <mylibs/xydata.hpp>

using namespace std;
using namespace mylibs;

 // forward declarations
class MyWindow;
pair<size_t, double*> triggered_average(const float *d, const double* xt, const size_t dItems, vector<size_t> triggers);

// array storing the data
mylibs::Array<float> ndarray(0,0);
MyWindow *window = NULL;

//mystring filename = "rsaz2012s4.0105.seq";
mystring filename = "";
size_t nr_traces  = 76;

float average_frequency = 0.;

class MyWindow : public Fl_Double_Window {
	public:

		MyWindow(int w,int h,const char*l=0);
		virtual ~MyWindow();
		void update();
		void play(int f = 1);
		int handle(int e);

	private:
		static void BtnLoad_cb(Fl_Widget*, void*userdata);
		static void BtnRun_cb(Fl_Widget*, void*userdata);
		static void BtnPower2_cb(Fl_Widget*, void*userdata);
		static void freq_cb(Fl_Widget*, void*userdata) {
			MyWindow *s = (MyWindow*) userdata;
			s->play();
		}

	size_t update_time_axis();

	public:
		float GetSamplingRate(){
			float v = atof(InputSamplingRate.value());
			if (v <= 0.) return 1.;
			return v;
		}

		inline bool ZeroPaddingEnabled() {return (BtnZeroPadding.value() == 1.);}
		inline double FilterFreqLo() {return freq1.value();}
		inline double FilterFreqHi() {return freq2.value();}

		inline double OffsetLo() {return atof(InputLowerOffset.value());}
		inline double OffsetHi() {return atof(InputUpperOffset.value());}

		inline void filename(const char* f){_filename = string(f);}
		inline const mystring& filename() const {return _filename;}


private:
	// inputs
	Fl_Button                  BtnLoad;
	Fl_Button                   BtnRun;
	Fl_Gnuplot_Window        *graphwin;
	Fl_Int_Input     InputNumberTraces;
	Fl_Int_Input    InputNumberSamples;
	Fl_Float_Input   InputLowerOffset;
	Fl_Float_Input   InputUpperOffset;
	Fl_Choice              ChoiceTrace;	// Selection of a trace
	Fl_Float_Input   InputSamplingRate;
	Fl_Light_Button     BtnZeroPadding;	// enable/disable zero padding
public:
	Fl_Light_Button     BtnVisibleOnly;	// enable/disable zero padding
	ValueSlider                    pos;
	ValueSlider                    win;

private:
	Fl_Button                BtnPower2;
	Fl_Gnuplot                  graph1;
	Fl_Gnuplot                  graph2;
	Fl_Gnuplot                  graph3;
	Fl_Value_Slider              freq1;
	Fl_Value_Slider              freq2;

	// things needed for computation
	mystring                _filename;
	double*                  time_axis;
	vector<size_t>            triggers; // needed for triggered averages


};

MyWindow::~MyWindow(){
	delete[] time_axis;
}

MyWindow::MyWindow(int w,int h,const char*l) :
	Fl_Double_Window(w,h,l),
	BtnLoad				( fl_width("# samples\n") + 20 + 70,   3, 60,  20, "Load"),
	BtnRun				( w - 140,                   3, 120,  20, "Compute average"),
	BtnVisibleOnly		( w - 140,                  43, 120,  20, "only visible interval"),
	graphwin(NULL),
	InputNumberTraces	( fl_width("# samples/s\n") + 20,  3, 60,  20, "# traces"),
	InputNumberSamples	( fl_width("# samples/s\n") + 20, 23, 60,  20, "# samples"),
	InputLowerOffset	( w -200, 3, 60,  20, "Offset left [s]"),
	InputUpperOffset	( w -200, 23, 60,  20, "Offset right [s]"),
	ChoiceTrace			( w -300,   43, 60, 20, "Trace"),
	InputSamplingRate	( fl_width("# samples/s\n") + 20, 43, 60,  20, "# samples/s"),
	BtnZeroPadding		( w -240, 43, 100,  20, "Zero padding"),


	pos					(  20,  65, 560,  15),
	win					(  20,  80, 475,  15),
	BtnPower2           ( 500,  80,   80,  15, "adjust pwr(2)"),
	graph1				(  20,  97, 560, 200),
	graph2				(  20, 300, 560, 200),
	graph3				(  20, 535, 560, 200),
	freq1				(  20, 502, 560,  15),
	freq2				(  20, 517, 560,  15),
	time_axis(NULL),
	triggers(vector<size_t>()) {

	Fl::scheme("gtk+");
	BtnLoad.labelsize(10);
	BtnLoad.callback(BtnLoad_cb, this);
	BtnRun.labelsize(10);
	BtnRun.callback(BtnRun_cb, this);

	BtnZeroPadding.labelsize(10);
	BtnZeroPadding.callback(freq_cb, this);

	BtnVisibleOnly.labelsize(10);
	BtnVisibleOnly.callback(freq_cb, this);

	BtnPower2.labelsize(8);
	BtnPower2.callback(BtnPower2_cb, this);

	InputSamplingRate.labelsize(10);
	InputSamplingRate.value("1000");

	InputNumberSamples.labelsize(10);
	InputNumberSamples.type(FL_INPUT_READONLY);

	InputNumberTraces.labelsize(10);
	InputNumberTraces.value(toString(nr_traces,0).c_str());

	InputLowerOffset.labelsize(10);
	InputLowerOffset.value("0.1");

	InputUpperOffset.labelsize(10);
	InputUpperOffset.value("0.0");

	ChoiceTrace.callback(freq_cb, this);
	ChoiceTrace.tooltip("visible trace");
	ChoiceTrace.down_box(FL_BORDER_BOX);
	ChoiceTrace.labelsize(10);
	int nt = atoi(InputNumberTraces.value());
	for (size_t i = 0; i < nt ; i++){
		ChoiceTrace.add(toString(i).c_str());
	}


	pos.type(FL_HOR_SLIDER);
	pos.tooltip("Position of the visualization window");

//	pos.when(FL_WHEN_CHANGED);
	pos.when(FL_WHEN_RELEASE);
	pos.callback(freq_cb, this);

	win.type(FL_HOR_SLIDER);
	win.tooltip("Width of the visualization window.");
	win.when(FL_WHEN_RELEASE);
	win.callback(freq_cb, this);

	freq1.type(FL_HOR_SLIDER);
	freq2.type(FL_HOR_SLIDER);

	freq1.tooltip("Set the frequency of the signal 1");
	freq2.tooltip("Set the frequency of the signal 2");

	freq1.when(FL_LEAVE);
	freq2.when(FL_LEAVE);

	freq1.callback(freq_cb, this);
	freq2.callback(freq_cb, this);

	freq1.bounds(0, 100); // in Hz
	freq2.bounds(0, 100); // in Hz

	freq1.value(1.);
	freq2.value(100.);

	freq1.step(.1);
	freq2.step(.1);
	add_resizable(graph2);

	triggers.clear();

	update();
}

int MyWindow::handle(int e){

  switch (e){
	 case FL_FOCUS:
//	 		cout << "FOCUS" << endl;
			break;
	case FL_UNFOCUS:
//	 		cout << "UNFOCUS" << endl;
			break;
	 case FL_KEYDOWN:
		 break;
	 case FL_KEYUP:{
		int k = Fl::event_key();
		switch (k){
			case FL_Up:
//				cout << "UP" << endl;
				if (ChoiceTrace.value() > 0){
					ChoiceTrace.value(ChoiceTrace.value()-1);
					play();
				}
				return 1;
				break;
			case FL_Down:
//				cout << "DOWN" << endl;
				if (ChoiceTrace.value() < nr_traces-1){
					ChoiceTrace.value(ChoiceTrace.value()+1);
					play();
				}
				return 1;
				break;
		}

		break;
	 }
 }
 if (Fl_Double_Window::handle(e) == 1 ) return 1;

 return 0;
}

size_t MyWindow::update_time_axis(){
//	float *d = ndarray.GetData(ChoiceTrace.value()); // get the pointer to the beginning of the trace
	size_t dItems = ndarray.GetItems();
	int N	  = (win.value()	 >= dItems) ? dItems		: win.value();
	int start = (pos.value() + N >= dItems) ? dItems - N : pos.value();

	if (BtnVisibleOnly.value() == 1.){
//		d     += start; 	// set pointer to te desired starting value
		dItems = N;			// only visible items
	}
	delete[] time_axis; time_axis= NULL;
	time_axis = new double[dItems];

	float inverse_sampling_rate = 1./GetSamplingRate();
	for (size_t i = 0; i < dItems; i++)
		time_axis[i] = inverse_sampling_rate * (i+start);
	return dItems;
}

void MyWindow::BtnLoad_cb(Fl_Widget*, void*userdata) {
	MyWindow *s = (MyWindow*) userdata;
	string fn;

	if (not mylibs::Fl_ChooseFile(fn, "File to load (must be binary, headerless...)", fn)){
		return;
	}
	s->filename(fn.c_str());

	ndarray.clear();
	ndarray.LoadData_raw(fn.c_str(), -1, atoi(s->InputNumberTraces.value()));
	ndarray.Transpose();
	ndarray.Info();
	s->update();
	s->play();
}

void MyWindow::BtnPower2_cb(Fl_Widget*, void*userdata){
	MyWindow *s = (MyWindow*) userdata;
	size_t v = (size_t) s->win.value();

	size_t dl = FastFourierTransformation::next_smaller_power2(v);
	size_t du = FastFourierTransformation::next_higher_power2(v);
	s->win.value((v-dl < du-v) ? dl : du);

//	s->update(); // would reset the win.value()
	s->play();
}

void MyWindow::BtnRun_cb(Fl_Widget*, void*userdata) {
	MyWindow *s = (MyWindow*) userdata;
	delete s->graphwin; s->graphwin = NULL;

	float sampling_rate = s->GetSamplingRate();

	float *data = ndarray.GetData(0); // get the pointer to the beginning of the trace
	size_t I = ndarray.GetItems();
	int N     = (s->win.value()	    >= I) ? I     : s->win.value();
	int start = (s->pos.value() + N >= I) ? I - N : s->pos.value();
	if (s->BtnVisibleOnly.value() == 1.){
		data     += start; 	// set pointer to the desired starting value
	} else start=0;

	I = s->update_time_axis();

	pair<size_t, double*> res = triggered_average(data,s->time_axis, I , s->triggers);

	size_t &sz = res.first;
	if ( sz == 0 ) return;

	double  *d = res.second;

	float inverse_sampling_rate = 1./s->GetSamplingRate();
	double xt[sz];
	for (size_t i = 0; i < sz; i++) xt[i] = inverse_sampling_rate * i;

	Array<float> av(ndarray.GetNrVariables(), sz); // create the new array for data output
	float *o = av.GetData(0);

	for (int j = 0; j < sz; j++) o[j] = (float) d[j];

	s->graphwin = new Fl_Gnuplot_Window(s->x(), s->y()+s->h(), s->w(), 300);
	Fl_Gnuplot &g = s->graphwin->graph;
	g.title("Butterfly plot");
	g.xlabel("time / s");
	g.ylabel("magnitude / a.u.");
//	g.set_2d_data(s->time_axis, o, sz, "averaged signals");
	g.set_2d_data_columns(xt, sz, 1, d);

	stringstream ss;
	ss << "#"	<< setw(11) << "index" << " "											// index
	<< setw(12) << "t_{RPeak}" << " "
	<< setw(12) << "t_{TPeak}" << " " 				// x values
	<< setw(12) << "f(t_{RPeak})" << " "
	<< setw(12) << "f(t_{TPeak})" << " "		// y values
	<< setw(12) << " RT ratio" << " "	 				// RT ratio
	<< setw(12) << " RT lenght" << " "							// RT length
	<< endl;

	// ranges for QRS and T detection
	double min = (s->OffsetLo() > 0.05) ? s->OffsetLo() - 0.05 : 0.;
	XYData::Range range_QRS(min,s->OffsetLo() + 0.05); 					// range for the QRS - peak
	XYData::Range range_T(s->OffsetLo() + 0.1, s->OffsetLo() + 0.5);	// range for the T - peak


	{// detection of R and T peak
		XYData func(xt, d, sz);
		size_t RPeak = func.extremum_element(range_QRS);
		size_t TPeak = func.extremum_element(range_T);
		double &xRPeak = func.x(RPeak);
		double &xTPeak = func.x(TPeak);
		ss 	<< setw(12) << "0" << " "											// index
			<< setw(12) << xRPeak << " "
			<< setw(12) << xTPeak << " " 				// x values
			<< setw(12) << func[RPeak] << " "
			<< setw(12) << func[TPeak] << " "		// y values
			<< setw(12) << func[TPeak]/func[RPeak] << " "	 				// RT ratio
			<< setw(12) << xTPeak - xRPeak << " "							// RT length
			<< endl;

	}
	delete[] d;

	for (int i = 1; i < ndarray.GetNrVariables(); i++){
		data = ndarray.GetData(i) +start;
		pair<size_t, double*> res = triggered_average(data, s->time_axis, I, s->triggers);
		if (res.first == 0) continue; // no data loaded
		double  *d = res.second;
		if (sz != res.first) cerr << "Size is different, why ? (index " << i << ")" << endl;
		o = av.GetData(i); // get the appropriate data array
		for (int j = 0; j < sz; j++) o[j] = (float) d[j]; // fill the output array with data
		g.add_2d_data_columns(1, d);					  // add the data also to the graph
		{
			// detection of local maxima
			XYData func(xt, d, sz);


			size_t RPeak = func.extremum_element(range_QRS);
			size_t TPeak = func.extremum_element(range_T);

	//		cout << i << " : " << RPeak << " " << TPeak << endl;
	//		cout << i << " : " << func[RPeak] << " " << func[TPeak] << endl;
	//		cout << "mean value = " << func.mean() << endl;
	//		func.yrange().info();

			double &xRPeak = func.x(RPeak);
			double &xTPeak = func.x(TPeak);
			ss 	<< setw(12) << i << " "											// index
				<< setw(12) << xRPeak << " "
				<< setw(12) << xTPeak << " " 				// x values
				<< setw(12) << func[RPeak] << " "
				<< setw(12) << func[TPeak] << " "		// y values
				<< setw(12) << func[TPeak]/func[RPeak] << " "	 				// RT ratio
				<< setw(12) << xTPeak - xRPeak << " "							// RT length
				<< endl;
		}
//		double height = (func.max() - func.mean()) * 0.05;
//		vector<size_t> p;
//		vector<size_t> v;
//		func.detect_peaks(p, v, height);
//		if (p.size() < 4)
//		for (vector<size_t>::iterator it = p.begin(); it != p.end(); it++){
//			cout << "peak " << *it << " : " << func[*it] << endl;
//		}

//		list<XYData::Range> peaks = func.find_peak(height, XYData::rising);
//		for (list<XYData::Range>::iterator it = peaks.begin(); it != peaks.end(); it++){
//			it->info();
//		}

		delete[] d;									  // data was copied, so delete it.
	}

	// everything's done, so save
	mystring fn   = s->filename();

	mystring ext = fn.file_ext();
	fn = fn.file_strip_ext();

	fn += "_averaged."+ext;
	av.Transpose();// data should be organized such, that for each point in time the trace number is increasing ...
	av.SaveData_raw(fn.c_str());

	mystring desc(fn.file_strip_ext()); // filename for additional information
	desc += ".txt";

	ofstream of(desc.c_str());

	of << "## Additional information " << endl
	   << "# The file " << fn << " contains time averaged data of a set\n\
# of sensor traces. The averaging is done using triggers to identify\n\
# the period of the signal.\n\
#\n#\n";
	of << "# averaged frequency : " << average_frequency << " Hz" << endl
	   << "# period length      : " << 1./average_frequency << " s" << endl
	   << "# averaging interval : [" << s->pos.value_scaled() << ", "
	   << s->pos.value_scaled() + s->win.value_scaled() << "] (in s)" << endl
	   << "# interval length    : " << s->win.value_scaled()<< "s" << endl;

	   int av_window_size = (int)s->win.value();
	   if ( FastFourierTransformation::power_of_two(av_window_size) )
			of << "# ( which is a nerdy power of two - remember: we are using FFT here ! ) " << endl;

	of << endl;
	of << ss.str();
	of.close();

}

void MyWindow::update(){
	ChoiceTrace.tooltip("visible trace");
	ChoiceTrace.down_box(FL_BORDER_BOX);
	ChoiceTrace.labelsize(10);

	ChoiceTrace.clear();
	size_t nt = ndarray.GetNrVariables();
	for (size_t i = 0; i < nt ; i++){
		ChoiceTrace.add(toString(i).c_str());
	}
	ChoiceTrace.value(0);
	if (nr_traces == 76) ChoiceTrace.value(53); // for special files # dirty but I'm too lazy now.

	pos.bounds(0, ndarray.GetItems());
	pos.value(0);
	pos.step(1.);
	pos.factor(1./GetSamplingRate());
	pos.unit("s");

	win.bounds(GetSamplingRate()/2, ndarray.GetItems());
	win.value(GetSamplingRate());
	win.step(1.);

	win.factor(1./GetSamplingRate());
	win.unit("s");

	InputNumberSamples.value(toString(ndarray.GetItems(),0).c_str());
}

void MyWindow::play(int f) {
	// compute time axis
	float sampling_rate = GetSamplingRate();

	float *d = ndarray.GetData(ChoiceTrace.value()); // get the pointer to the beginning of the trace
	size_t I = ndarray.GetItems();
	int N	 = (win.value()	 >= I) ? I		: win.value();
	int start = (pos.value() + N >= I) ? I - N : pos.value();

	if (BtnVisibleOnly.value() == 1.)	d     += start; 	// set pointer to te desired starting value

	size_t dItems = update_time_axis();

	string title(string("trace ") + string(ChoiceTrace.text()));
	graph1.set_2d_data(time_axis,
		ndarray.GetData(ChoiceTrace.value())+start,N,title.c_str());
	graph1.xlabel("t / s");
	graph1.ylabel("f(x)");
	graph1.title(title.c_str());

	FastFourierTransformation fft(d, dItems, BtnZeroPadding.value()==1.);
	fft.Compute();
	fft.sampling_rate(sampling_rate);

// 	plot the power spectrum
	double       power[(int) dItems/2];
	double frequencies[(int) dItems/2];

	// plot the spectrum
	int k = fft.GetPowerSpectrum(frequencies, power);

	size_t maxIdx = fft.Frequency2Index(10);

	// usally low frequencies get obfuscated, so we increase the visible interval
	if (BtnZeroPadding.value() == 1.) maxIdx = fft.Frequency2Index(50);
	maxIdx = ( maxIdx < k ) ? maxIdx : k;

	cout << maxIdx << endl;
// find main peek between 0.5 an 2 Hz
	XYData pwr(frequencies, power, maxIdx);
	pwr.info("power spectrum");

	graph2.clear();
	graph2.xlabel("frequency / Hz");
	graph2.ylabel("magnitude");
	graph2.title("fourier transform");

	graph2.set_2d_data(frequencies,power,maxIdx,"power spectrum");

	try {

		cmdline::section("Main peek between 0.5 and 2 Hz");
		XYData::Range xr(0.5, 2);
		xr.info();
		size_t maxe = pwr.max_element(xr);
		cout << "  maximum value : " << pwr.max(xr) << endl;
		cout << "maximum element : " << maxe << endl;
		cout << "      frequency : " << frequencies[maxe] << " Hz" << endl;
	} catch (XYData::Exception_Range &e){
		cerr << e.what() << endl;
		// ignore this exception sind we don't use the value
	}

	{ // peak detection in the signal
		vector<size_t> emi_peaks;
		vector<size_t> absop_peaks;
		vector<float> emi_freq;
		vector<float> emi_vals;
		vector<float> abs_freq;
		vector<float> abs_vals;
		double delta = pwr.yrange().size() * 0.4;
		pwr.detect_peaks(emi_peaks, absop_peaks, delta);
		for (vector<size_t>::iterator it = emi_peaks.begin(); it != emi_peaks.end(); it++){
			emi_freq.push_back(frequencies[*it]);
			emi_vals.push_back(power[*it]);
		}
		for (vector<size_t>::iterator it = absop_peaks.begin(); it != absop_peaks.end(); it++){
			abs_freq.push_back(frequencies[*it]);
			abs_vals.push_back(power[*it]);
		}
		graph2.add_2d_data(emi_freq, emi_vals, "peaks", Fl_Gnuplot::PLOT_WITH_POINTS);
		graph2.add_2d_data(abs_freq, abs_vals, "valleys", Fl_Gnuplot::PLOT_WITH_POINTS);
	}

	//apply filter
	fft.BandPassFilter(freq1.value(), freq2.value());

	// compute inverse
	double inverse[dItems];
	for (size_t i = 0; i < dItems; i++) inverse[i] = 0.;

	k 	  = fft.ComputeInverse(inverse);
	N	  = (win.value()	 >= k) ? k	 : win.value();
	start = (pos.value() + N >= k) ? k - N : pos.value();

	graph3.xlabel("t / a.u.");
	graph3.ylabel("f(x)");
	graph3.title("Signal computed by inverse transformation with a bandpass filter applied.");

	// compute inverse fourier transform
	XYData func(time_axis, inverse, k);
	graph3.set_2d_data(time_axis, inverse, N);

	cmdline::section("Peek detection in the filtered signal");
	// we search for outstanding peaks
	double min = func.mean(); // use mean value as minimum of the step function
	cout <<" mean_value = " << min << endl;
	double max = func.max();

	XYData::Range yrange(
							min + 0.1 * (max - min), // 10 % above
							min + 0.7 * (max - min)  // 70 % above mean value
						);
	yrange.info();

	{ // peak detection : needed for triggers
		vector<size_t> valleys;
		vector<float> emi_freq;
		vector<float> emi_vals;
		vector<float> abs_freq;
		vector<float> abs_vals;

		func.detect_peaks(triggers, valleys, 0.8 * (max - min));
		cout << " emi_peaks : " << triggers.size() << endl;
		cout << " abs_peaks : " << valleys.size() << endl;

		for (vector<size_t>::iterator it = triggers.begin(); it != triggers.end(); it++){
			if (*it > N ) continue;
			emi_freq.push_back(time_axis[*it]);
			emi_vals.push_back(inverse[*it]);
		}

		for (vector<size_t>::iterator it = valleys.begin(); it != valleys.end(); it++){
			if ( *it > N ) continue;
			abs_freq.push_back(time_axis[*it]);
			abs_vals.push_back(inverse[*it]);
		}
		graph3.add_2d_data(emi_freq, emi_vals, "peaks", Fl_Gnuplot::PLOT_WITH_POINTS);
		graph3.add_2d_data(abs_freq, abs_vals, "valleys", Fl_Gnuplot::PLOT_WITH_POINTS);


		size_t ol = OffsetLo() * GetSamplingRate();
		size_t ou = OffsetHi() * GetSamplingRate();
		pair<size_t,double *> res = func.average_triggered(triggers, ol, ou);
		if (res.first > 0)
			graph1.add_2d_data(time_axis, res.second, res.first, "average");
	}

//	determine frequency of the filtered signal
	if ( triggers.size() > 1 ){
		cmdline::section("Frequency of the filtered signal");
		vector<size_t>::iterator it = triggers.begin();
		double last = time_axis[*it]; // time of the first trigger
		double mean = 0.;
		for (++it; it != triggers.end(); ++it){ // from the second trigger to the end
			mean += time_axis[*it] - last;
			last = time_axis[*it];
		}
		mean /= triggers.size()-1;
		average_frequency = 1./mean;
		cout << "Frequency = " <<  average_frequency << " Hz" << endl;
	}

	graph1.hide();
	graph2.hide();
	graph3.hide();
	graph1.show();
	graph2.show();
	graph3.show();
}

pair<size_t, double*> triggered_average(const float *d, const double *xt,  const size_t dItems, vector<size_t> triggers) {
	if (!window) return pair<size_t, double*>(0,NULL); // if window is not created return

	FastFourierTransformation fft(d, dItems, window->ZeroPaddingEnabled());
	fft.Compute();
	fft.sampling_rate(window->GetSamplingRate());

//  apply filter
	fft.BandPassFilter(window->FilterFreqLo(), window->FilterFreqHi());

//  compute inverse
	double inverse[dItems];
	for (size_t i = 0; i < dItems; i++) inverse[i] = 0.; // init
	int k 	  = fft.ComputeInverse(inverse);

//  apply triggered average
	XYData func(xt, inverse, k);

	size_t ol = window->OffsetLo() * window->GetSamplingRate();
	size_t ou = window->OffsetHi() * window->GetSamplingRate();
	return func.average_triggered(triggers, ol, ou);
}

int main(int argc, char **argv) {

	fl_font(FL_TIMES, 10);
	fl_color(FL_BLACK);

	if ( argc > 1 ){
		if (IO::file_exists(argv[1])){
			filename = mystring(argv[1]);
		}
	}

	if ( argc > 2 ){
		nr_traces = atoi(argv[2]);
	}

	cout << " filename : " << filename  << endl;
	cout << " # traces : " << nr_traces << endl;

	if (not IO::file_exists(filename.c_str())) cmdline::exit("No valid file to load !");

	ndarray.Info();
	ndarray.LoadData_raw(filename.c_str(), -1, nr_traces);
	ndarray.Transpose();
	ndarray.Info();

	MyWindow mainWin(600, 740, "Signal analyzer");
	mainWin.end();
	mainWin.update();
	window = &mainWin;
	mainWin.filename(filename.c_str());
	mainWin.play();

	mainWin.show(argc, argv);

	return Fl::run();
}
