//		./demos/lists_demo.cpp
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
 *      test_lists.cpp
 * 		Test program for lists.
 * 		g++ test_lists.cpp -o test_lists.cpp -Wall
 */

#include <iostream>
#include <mylibs/point.hpp>
#include <mylibs/lists.h>
#include <mylibs/myline.hpp>
#include <mylibs/cmdline.hpp>

#ifndef print
#define print(x) (cout << __FILE__ << ":" << __FUNCTION__ <<"() (" <<__LINE__ << ") : " << x << endl)
#endif

using namespace mylibs;

int main(int argc, char** argv){
	try {
		myList<int> test;
		myList<int> scnd;
		cmdline::subsection("Creating lists");

		for (int i = 0;  i < 10; i++)	test.append(i); //	 fill the lists
		for (int i = 10; i < 20; i++)	scnd.append(i);

		// print out the list
		test.prnt("List 1");
		scnd.prnt("List 2");

		int *ar = test.toArray();

		cmdline::subsection("Conversion to an array");
		for (int i = 0; i < 10; i++){
			cout << ar[i] << " ";
		}
		cout << endl;

		delete[] ar; ar=0;

		cmdline::subsection("Element access");
		// element access
		print( test[0]);
		print( test[-1]);
		print( test[-2]);
		print( test[-3]);


		print( test[0]);
		print( test[2]);
		print(test.next()); // get item 3
		print(test.next()); // get item 4
		print(test.next()); // get item 5
		print( test[8]);

		test.erase(-1);
		test.erase(-2);

		test.prnt();
		scnd.prnt();
		print("Extending test by second list");
		test.extend(scnd);
		test.prnt();
		scnd.prnt();

		print( test[20]); //<< here we create an exception
	}
	catch (string e){
		print(e);
	}
	cmdline::subsection("PointCurves");
	try{ //! PointCurves are inherited from lists.
		PointCurve curve;
		Point p(1.,1.,1.);
		Point p2(0.,0.,0.);

		for (int i = 0; i < 20; i++){
			curve.push_back(p2);
			p2 += p;
		}

		// here we demonstrate a problem using references
		// a reference can be assigned once using
		// Point &pt = p2;
		// On the other hand one cannot change the reference to point to
		// another item again. The value of p2 would be modified instead.
		{ // good code
			print ("curve 1");
			Point p3 = curve[0]; p3.prnt();
			p3 = curve.next();	 p3.prnt();
			p3 = curve.next();	 p3.prnt();
			p3 = curve.next();	 p3.prnt();
			p3 = curve.next();	 p3.prnt();

			print ("curve 2");
			p3 = curve[0];		 p3.prnt();
			p3 = curve.next();	 p3.prnt();
			p3 = curve.next();	 p3.prnt();
			p3 = curve.next();	 p3.prnt();
			p3 = curve.next();	 p3.prnt();
		}
		cout << endl;
		{// wrong code
			Point &p3 = curve[0]; 	p3.prnt(); // a reference to the first item is stored
			p3 = curve.next(); 		p3.prnt(); // We want to save a new reference
											   // but mistakenly the _value_ of curve[0]
											   // is changed.
			cout <<" curve[0] is now "; curve[0].prnt();
		}
		TELL(curve.arc_length());
		PointCurve::iterator it = curve.end();
		TELL(curve.arc_length(it));
		TELL(curve.arc_length(--it));
	}
	catch (string e){
		print(e);
	}

	return 0;
}
