//      exists.cpp
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


#include <iostream>
#include <mylibs/mymath.hpp>
#include <mylibs/cmdline.hpp>
#include <mylibs/mystring.hpp>
#include <mylibs/cmdline.hpp>
#include <mylibs/myinifiles.hpp>
#include <mylibs/mymesh.hpp>
#include <mylibs/myio.hpp>
#include <Magick++.h>

using namespace myMath;
using namespace std;
using namespace mylibs;
using namespace mylibs::cmdline;
using namespace mylibs::IO;

#define INFO(name) info(name, #name)
void SetupTransformationMatrix( Matrix &M,
								const vector<double> &Position,
								const vector<double> &Orientation,
								const vector<double> &PixelSpacing){
	M(0,0) = Orientation[0] * PixelSpacing[0];
	M(1,0) = Orientation[1] * PixelSpacing[0];
	M(2,0) = Orientation[2] * PixelSpacing[0];
	M(3,0) = 0.;

	M(0,1) = Orientation[3] * PixelSpacing[1];
	M(1,1) = Orientation[4] * PixelSpacing[1];
	M(2,1) = Orientation[5] * PixelSpacing[1];
	M(3,1) = 0.;

	//! \todo PixelSpacing[2] cannot be read from a single images of an image stack

	/// in this case: PixelSpacing[2] == 1.!
	M(0,2) = (Orientation[1] * Orientation[5] - Orientation[2] * Orientation[4])  * PixelSpacing[2];
	M(1,2) = (Orientation[2] * Orientation[3] - Orientation[0] * Orientation[5])  * PixelSpacing[2];
	M(2,2) = (Orientation[0] * Orientation[4] - Orientation[1] * Orientation[3])  * PixelSpacing[2];
	M(3,2) = 0.;

	M(0,3) = Position[0];
	M(1,3) = Position[1];
	M(2,3) = Position[2];
	M(3,3) = 1.;
}

void SetupTransformationMatrix( Matrix &M,
								const vector<double> &Position,
								const vector<double> &Orientation){
	M(0,0) = Orientation[0];
	M(1,0) = Orientation[1];
	M(2,0) = Orientation[2];
	M(3,0) = 0.;

	M(0,1) = Orientation[3];
	M(1,1) = Orientation[4];
	M(2,1) = Orientation[5];
	M(3,1) = 0.;

	M(0,2) = (Orientation[1] * Orientation[5] - Orientation[2] * Orientation[4]);
	M(1,2) = (Orientation[2] * Orientation[3] - Orientation[0] * Orientation[5]);
	M(2,2) = (Orientation[0] * Orientation[4] - Orientation[1] * Orientation[3]);
	M(3,2) = 0.;

	M(0,3) = Position[0];
	M(1,3) = Position[1];
	M(2,3) = Position[2];
	M(3,3) = 1.;
}

int main(int argc, char **argv){

	myIniFiles ini(argc, argv);

//	ini.infotext(
	ini.register_param("image", "i", "Dicom image to load.");
	ini.register_param("vec"  , "v", "File name of a *.vec file containing vectors in a cartesian coordinate system.");


	mystring image = ini.read("image", string("test.dcm"));
	Magick::Image dcm(image.c_str());

	mystring VecFile = ini.read("vec", string(""));
	if (not VecFile.empty() and not mylibs::IO::file_exists(VecFile.c_str()))
		cmdline::exit(VecFile + " does not exist !");

	mystring tmp = dcm.attribute("dcm:PixelSpacing");
	vector<double> PixelSpacing = tmp.extract<double>();

	if (PixelSpacing.size() == 2){ // z-value is often hidden in SliceThickness
		tmp = dcm.attribute("dcm:SliceThickness");
		double t = 0.; tmp >> t;
		PixelSpacing.push_back(t);
	}

	tmp	 = dcm.attribute(string("dcm:ImagePosition(Patient)"));
	vector<double> Position = tmp.extract<double>();

	tmp = dcm.attribute(string("dcm:ImageOrientation(Patient)"));
	vector<double> Orientation = tmp.extract<double>();

	INFO(PixelSpacing);
	INFO(Orientation);
	INFO(Position);

	Matrix M(4,4);
//	SetupTransformationMatrix(M, Position, Orientation, PixelSpacing );
	SetupTransformationMatrix(M, Position, Orientation);
	M.info("Transf.");

	// We have two possiilities to transform back
	// (1) We stupidly invert the transformation matrix, this need divisions, which may impose numerical issues.
	//	Matrix R(M);
	//	R.invert();
	//	R.info("Reverse tranformation");

	// (2) For the rotation matrix holds R^1 = R^T, so we compute the transposed
	Matrix R = M.Minor(3,3);	// of the minor matrix (rotational part only)
	R.transpose();

	// We transform the position vector ...
	Vector Pos_1 = R * Position;
	Pos_1 *= -1;

	//	and build the reverse transformation matrix
	R.addCol(Pos_1);
	Vector r(4); r[3] = 1.;
	R.addRow(r);
	R.info("Reverse");

	Vector P1(4);
	P1[0] = 93. * PixelSpacing[0];
	P1[1] = 42. * PixelSpacing[1];
	P1[2] = 0.;
	P1[3] = 1.;

	Point P2(93. * PixelSpacing[0], 42. * PixelSpacing[1], 0.);

	P1.info("P1         ");
	Vector P0 = M * P1;
	P0.info("P0         ");
	Vector P0R = R * P0;
	P0R.info("P1_restored");

	return 0;
}

