//		test_Symbolic.cpp
//
//		Copyright 2011 Philipp Gast <avatar81@itp.tu-berlin.de>
//		Copyright 2011 Rico Buchholz <buchholz@itp.tu-berlin.de>
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

#include <iostream>
#include <iomanip>
#include <string>
#include <cmath>

#include "../Symbolic.hpp"

/** \file test_Symbolic.cpp
 *	\brief main-function for testing Symbolic.
 *
 *	This file contains the main-function which is used for testing the classes
 *	of the Symbolic namespace.
 */

using namespace std;

/** \fn int main(int , char ** )
 *	This is the main-function, which includes the code for testing the classes
 *	of the Symbolic namespace, e.g. the function parser Parser.
 *	The function consist of several blocks, each for testing of some aspect.
 *	Two examples: The first block gives a small demonstration code for how to
 *	use the Parser, while the second tests a specific constructor.
 *
 * \return Zero if all is ok, one if there was an exception which was thrown.
 */
int main(int /* argc */, char ** /* argv */) {

	{
		//	This is a basic demonstration of usage for the Symbolic::Parser.
		std::cout<<"-----------------------------\n";
		std::cout<<"Basic demonstration of usage.\n";
		double	x = 0.0;
		double	y = 5.0;
		double	res = 0.0;

		try {// If the function (see line below) doesn't match the requirements of the class, an exception is thrown, so we put the following code into a try block.
			string const fct("x%1+1/10*sin(2*x*4*atan(1))-y");// The function that should be parsed.
			Symbolic::Parser p(fct);// Create a Parser, the function is argument of the constructor.

			// Just to have the output aligned.
			cout.setf(ios::fixed, ios::floatfield);
			cout.setf(ios::showpoint);

			for(x = 0; x <= 2; x += 0.1) {// Variation of x-variable.
				// For each set of variables just call the Compute-method of the parser, the function value is returned.
				// Here we print for each x-value the values that where used to call the function and print the result.
				cout << setprecision(6) << "         " <<"f[" << x << ", "<<y<<"] = "<< p.Compute(x,y) <<"\n";
			}
		} catch(exception & e) {
			// Here we catch each excetion which is derived from the standard exception class, as is the case for the excetions thrown within the Symbolic namespace (to which belongs the parser).
			cout << "An exception was thrown during parsing or computation. Error message is:\n";
			cout << e.what();
			return 1;
		}
	}

	//{
	//std::cout<<"-----------------------------------------------\n";
	//std::cout<<"Test of Node-constructor's which take a opcode:\n";
	//Symbolic::Node nc(Symbolic::OP_CONST, 1.0);
	//Symbolic::Node ne(Symbolic::OP_EXP, nc);
	//Symbolic::Node np(Symbolic::OP_PLUS, nc, ne);
	//}

	//{
	//std::cout<<"-------------------------------------------------\n";
	//std::cout<<"Testing of copy-constructor of the parser & node:\n";
	//Symbolic::Parser p1("2.4*x+2+y^2");
	//Symbolic::Parser p2(p1);
	//Symbolic::Node n1(p2.GetRoot());
	//n1.Copy(n1);

	//std::cout<<"Length of the first tree:           "<<p1.GetRoot().SerializedLength()<<"\n";
	//std::cout<<"Length of the copied tree (parser): "<<p2.GetRoot().SerializedLength()<<"\n";
	//std::cout<<"Length of the copied tree (node):   "<<n1.SerializedLength()<<"\n";
	//}

	//{
	//std::cout<<"------------------------------\n";
	//std::cout<<"Testing of the compute-method.\n";
	//double	x = 0.0;
	//double	y = 5.0;
	//double	res = 0.0;

	//try {
	//string const fct("x%1+1/10*sin(2*x*4*atan(1))-y");
	//Symbolic::Parser p(fct);
	//double diff_sum(0.0);

//#ifdef DEBUG
	//p.GetRoot().Info();
	//p.GetBytecode().Info();
	//p.Info();
//#endif

	//cout.setf(ios::fixed, ios::floatfield);
	//cout.setf(ios::showpoint);
	////cout << "result={" << endl;
	//for(x = 0; x <= 2; x += 0.1) {
	//res = p.Compute(x,y);
	////cout << setprecision(6) << "         " <<"f[" << x << ","<<y<<"] = "<< res<<"  "
	////<<fmod(x,1) + 0.1*sin(8*x*atan(1))-y<<"  "
	////<<res - (fmod(x,1) + 0.1*sin(8*x*atan(1))-y);
	//diff_sum += abs(res - (fmod(x,1) + 0.1*sin(8*x*atan(1))-y));
	////if(fabs(x - 2) > 0.1) cout << ",\n";
	//}
	//if(diff_sum < 0.000001) cout<<"\t..success.\n";
	////cout << "\n       }\n";
	//} catch(exception & e) {
	//cout << "An exception was thrown during parsing or computation. Error message is:\n";
	//cout << e.what();
	//return 1;
	//}
	//}

	//{
	//std::cout<<"----------------------------------------\n";
	//std::cout<<"Check the ReduceTree-method of the Node.\n";
	//string fct1("1.0-2.0*3.0+1.5/2.4+3%4+2^2");
	//string fct2("ln(exp(sqrt(sin(cos(H(0.4))))))+0/x");
	//string fct3("(x+4)/(x+4)");
	//Symbolic::Parser p1(fct1.c_str());
	//Symbolic::Parser p2(fct2.c_str());
	//Symbolic::Parser p3(fct3.c_str());
	//Symbolic::Node n1(p1.GetRoot());
	//Symbolic::Node n2(p2.GetRoot());
	//Symbolic::Node n3(p3.GetRoot());
//#ifdef DEBUG
	//std::cout<<"First function-tree:\n";
	//n1.Info();
	//std::cout<<"Second function-tree:\n";
	//n2.Info();
//#endif
	//std::cout<<"Before reducing the tree, operation code is: "<<n1.GetOpcode()<<"\n";
	//std::cout<<"Length of the tree is: "<<n1.SerializedLength()<<"\n";
	//n1.ReduceTree();
	//n2.ReduceTree();
	//n3.ReduceTree();
//#ifdef DEBUG
	//std::cout<<"First function-tree, reduced:\n";
	//n1.Info();
	//std::cout<<"Second function-tree, reduced:\n";
	//n2.Info();
//#endif
	//std::cout<<"After reducing of the tree's, are there length:\n"
	//<<"\t"<<n1.SerializedLength()<<"\n"
	//<<"\t"<<n2.SerializedLength()<<"\n"
	//<<"\t"<<n3.SerializedLength()<<"\n";
	//if(Symbolic::OP_CONST == n1.GetOpcode()) {
	//std::cout<<"Operation code of the first root is OP_CONST and the value is : "<<n1.GetDirect()<<"\n";
	//}
	//if(Symbolic::OP_CONST == n2.GetOpcode()) {
	//std::cout<<"Operation code of the second root is OP_CONST and the value is: "<<n2.GetDirect()<<"\n";
	//}
	//if(Symbolic::OP_CONST == n3.GetOpcode() and 1.0 == n3.GetDirect()) {
	//std::cout<<"Reducing f(..)="<<fct3<<"\n\t..successfull.\n";
	//}
	//}

	//{
	//std::cout<<"-------------------------------------------------------\n";
	//std::cout<<"Check the LinearSolve(Opcode, Node)-method of the Node.\n";
	//string fct1("5*x+x+2");
	//string fct2("5*x+2");
	//string fct3("5*x+x/tan(x)");
	//string fct4("(z+z*2)*x+x*(1+4*y+y*t+t)");
	//Symbolic::Parser p1(fct1.c_str());
	//Symbolic::Parser p2(fct2.c_str());
	//Symbolic::Parser p3(fct3.c_str());
	//Symbolic::Parser p4(fct4.c_str());
	//Symbolic::Node n1(p1.GetRoot());
	//Symbolic::Node n2(p2.GetRoot());
	//Symbolic::Node n3(p3.GetRoot());
	//Symbolic::Node n4(p4.GetRoot());
	//Symbolic::Node factor1, factor2, factor3, factor4;
	//n1.LinearSolve(Symbolic::OP_X, factor1);
	//n2.LinearSolve(Symbolic::OP_X, factor2);
	//n3.LinearSolve(Symbolic::OP_X, factor3);
	//n4.LinearSolve(Symbolic::OP_Y, factor4);
//#ifdef DEBUG
	//std::cout<<"##### First function-tree:\n";
	//n1.Info();
	//std::cout<<"##### Factor1 is now:\n";
	//factor1.Info();
	//std::cout<<"##### Second function-tree:\n";
	//n2.Info();
	//std::cout<<"##### Factor2 is now:\n";
	//factor2.Info();
	//std::cout<<"##### Third function-tree:\n";
	//n3.Info();
	//std::cout<<"##### Factor3 is now:\n";
	//factor3.Info();
	//std::cout<<"##### Fourth function-tree:\n";
	//n4.Info();
	//std::cout<<"##### Factor4 is now:\n";
	//factor4.Info();
//#endif
	//}

	{
		std::cout<<"-------------------------------------------------\n";
		std::cout<<"Check the LinearSolve(Opcode)-method of the Node.\n";

		string const fct1("5*x+x+2");
		cout << fct1 << endl;
		string const fct2("(z-z*2)*x+x*(1+4*y+y*t+t)");
		cout << fct2 << endl;
		//string const fct2("z-z*2");
		Symbolic::Parser p1(fct1.c_str());
		Symbolic::Parser p2(fct2.c_str());
		Symbolic::Node n1(p1.GetRoot());
		Symbolic::Node n2(p2.GetRoot());
		Symbolic::Node & nr = n1;
		n1.LinearSolve(Symbolic::OP_X);
		n2.LinearSolve(Symbolic::OP_X);
#ifdef DEBUG
		std::cout<<"First function-tree:\n";
		n1.Info();
		std::cout<<"Second function-tree:\n";
		n2.Info();
#endif
	}

	{
		cout<<"---------------------------------\n";
		cout<<"Check the handling of parameters.\n";
		double const dim(2.0), size(14.0);
		double sum(0.0);
		string const fct("x^DIM/CX-0.5");
		Symbolic::Parameter paramlist;
		paramlist.insert(pair<string, double>("DIM", dim));
		paramlist.insert(pair<string, double>("CX", size));

		Symbolic::Parser p(fct, &paramlist);

		for(double x = 0.0; x < size; x += 0.5) {
			sum += (p.Compute(x) - double(powf(x, dim)/size-0.5))*(p.Compute(x) - double(powf(x, dim)/size-0.5));
		}
		if(sum < 0.000001) cout<<"... successfull.\n";
		else {
			cout<<"... failed!\n"
				<<"    sum of differences is: "<<sum<<"\n";
		}
	}

	return 0;
}

#undef DEBUG
