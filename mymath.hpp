
#include <string>
#include <math.h>
#include <stdio.h>
#include <iostream>
#include <vector>

#if defined(__GXX_EXPERIMENTAL_CXX0X__) || __cplusplus >= 201103L
#include <initializer_list>
#endif

#include "myexception.hpp"
#include "point.hpp"

using namespace std;
#ifndef __MYMATH_H
#define __MYMATH_H

namespace myMath {

double power(double value, int p);

int min(int a,int b);
int max(int a,int b);

int	abs(int a);
double abs(double value);

class Point2D{
public:
	double x;
	double y;
public:
	Point2D(){
		this->x = 0.;
		this->y = 0.;
	}

	Point2D(double x, double y){
		this->x = x;
		this->y = y;
	}
};

class Vector: public vector<double> {
	public:
		Vector();
		Vector(size_t n);
		Vector(size_t n, double *values);
		Vector(vector<double> &vec);
		Vector(const Point &p);
#if defined(__GXX_EXPERIMENTAL_CXX0X__) || __cplusplus >= 201103L
		Vector(initializer_list<double> values);
#endif
		~Vector();

		Vector  operator-(const Vector &other);
		Vector  operator*(const double &val);
		Vector& operator*=(const double &val);

		double abs();
		void info(string name = "")const;
};

class Matrix {
	public:

		Matrix();
		Matrix(size_t n, size_t m);
		Matrix(size_t n, size_t m, double *values);
		Matrix(Matrix &B);
		Matrix(const Matrix &B);
		void clear();
		void copy(const Matrix &B);
		void assign(const size_t n, const size_t m, const double value);
		void addRow(Vector row, size_t position=(size_t)-1);
		void addCol(Vector col, size_t position=(size_t)-1);
		void invert();
		Matrix& operator=(const Matrix &M);
		double& operator()(const size_t i, const size_t j);
		Vector operator*(const Vector &x) const;
		Matrix operator*(const Matrix &B) const;
		Matrix operator+(const Matrix &B) const;
		double trace();
		void transpose();
		Matrix Minor(size_t i, size_t j) const ;
		bool isNullMatrix();
//		Matrix operator+(const Matrix B) const;
		Vector & operator[]( const size_t i);
		size_t Rows()const {return rows;};
		size_t Cols()const {return cols;};
		void info(const string &name="none") const;

	private:
		vector< Vector > matrix;
		size_t rows;
		size_t cols;
};
} // end of namespace myMath
#endif //__MYMATH_
