
#include "mymath.hpp"

using namespace myMath;

double myMath::power(double value, int p){
	double res=1.;
	for (int i=0; i < p; i++) res *= value;
	return res;
}

int myMath::min(int a,int b){
	return (a<b)?a:b;
}
int myMath::max(int a,int b){
	return (a<b)?b:a;
}
int	myMath::abs(int a){
	return  (a < 0) ? -a: a;
}
double myMath::abs(double value){
	if (value < 0.) return -value;
	else return value;
}


Vector::Vector(){

}

Vector::Vector(size_t n):vector<double>(n){
	assign(n, 0.);
}

Vector::Vector(size_t n, double *values){
	this->assign (values,values+n);
}

Vector::Vector(vector<double> &vec){
	this->assign(vec.begin(), vec.end());
}

Vector::~Vector(){
	clear();
}

#if defined(__GXX_EXPERIMENTAL_CXX0X__) || __cplusplus >= 201103L
Vector::Vector(initializer_list<double> values){
	assign(values.begin(), values.end());
}
#endif

Vector::Vector(const Point &p){
	assign(3,0.);
	this->operator[](0) = p.x;
	this->operator[](1) = p.y;
	this->operator[](2) = p.z;
}

Vector Vector::operator-(const Vector &other){
	if (size() != other.size()) throw(string("Sizes of both vectors don't match."));
	Vector res;
	for (size_t i = 0; i < this->size(); i++){
		res.push_back(this->operator[](i) - other[i]);
	}
	return res;
}

Vector Vector::operator*(const double &val){
	Vector res;
	for (Vector::iterator it = begin(); it != end(); it++){
		res.push_back(*it * val);
	}
	return res;
}

Vector& Vector::operator*=(const double &val){
	for (Vector::iterator it = begin(); it != end(); it++){
		*it *= val;
	}
	return *this;
}

double Vector::abs(){
	double sq_sum = 0.;
	for (size_t i = 0; i < this->size(); i++)
		sq_sum += this->operator[](i) * this->operator[](i);

	return sqrt(sq_sum);
}

void Vector::info(string name)const {

	if (name != "")	cout << name;
	for (size_t i = 0; i < name.size(); i++){
		if (isalnum(char(name[i]))){
			cout << " = ";
			break;
		}
	}

	if (name != "none"){
		size_t w = ( name.size() + 3 < 13 ) ? 13 :  name.size() + 3;
		for (size_t j = 0; j < w - name.size(); j++)cout << " ";
	}

	cout << "( ";
	for (Vector::const_iterator it = begin(); it != end(); it++){
		if (*it >= 0. ) cout << "+";
		cout << std::scientific <<  *it << " " ;
//		printf("%+.10E ", *it);
	}
	cout << ")" << endl;

}


Matrix::Matrix() : rows(0), cols(0){
	rows = 0;
	cols = 0;
	Vector row;
	row.assign(3, 0.);
	for (size_t i = 0; i < 3; i++) matrix.push_back(row);

}

Matrix::Matrix(size_t n, size_t m) : rows(n), cols(m){
	Vector line(m);
	for (size_t i = 0; i < n; i++)
		this->matrix.push_back(line);
}

Matrix::Matrix(size_t n, size_t m, double *values): rows(n), cols(m){
	for (size_t i = 0; i < rows; i++) {
		matrix.push_back(Vector(m, &values[i*rows]));
	}
}

Matrix::Matrix(Matrix &B){
	this->copy(B);
}

Matrix::Matrix(const Matrix &B){
	this->copy(B);
}

void Matrix::clear(){
	rows = 0;
	cols = 0;
	matrix.clear();
}

void Matrix::copy(const Matrix &B){

	clear();
	rows = B.rows;
	cols = B.cols;

	Vector line;
	line.assign(cols, 0.);
	for (size_t i = 0; i < rows; i++)
		this->matrix.push_back(line);

	for(size_t i=0; i< rows; i++){
		for(size_t j=0; j< cols; j++){
			matrix[i][j] = B.matrix[i][j];
		}
	}
}

void Matrix::assign(const size_t n, const size_t m, const double value){
	clear();
	rows = n;
	cols = m;

	Vector row;
	row.assign(n, value);
	for (size_t i = 0; i < m; i++) matrix.push_back(row);

	return;
}

void Matrix::addRow(Vector row, size_t position){
	if (rows == 0) cols = row.size();

	if (row.size() != cols ) throw myexception(EXCEPTION_ID + "Error: # of columns wrong");

	if (position < rows)
		matrix.insert(matrix.begin()+position, row);
	else
		matrix.push_back(row);
	rows++;
}

void Matrix::addCol(Vector col, size_t position){
	if (col.size() != rows ) throw myexception(EXCEPTION_ID + "Error: wrong # of rows!");


	if (position < cols) {
		for (size_t i = 0; i < rows; i++)
			matrix[i].insert(matrix[i].begin()+ position, col[i]);
	}
	else {
		for (size_t i = 0; i < rows; i++)
			matrix[i].push_back(col[i]);
	}
	cols++;
}

Matrix operator*(const double c, Matrix &M){
	Matrix R(M.Rows(), M.Cols());
	for (size_t i = 0; i < M.Rows(); i++){
		for (size_t j = 0; j < M.Cols(); j++){
			R[i][j] = c * M[i][j];
	}
	}
	return R;
}

Matrix& Matrix::operator=(const Matrix &M){
	this->copy(M);
	return *this;
}

double& Matrix::operator()(const size_t i, const size_t j){
	return matrix[i][j];
}

Vector Matrix::operator*(const Vector &x) const{  // Matrix * Vector
	Vector res;
	for(size_t i=0; i< rows; i++){
		double value = 0.;
		for(size_t j=0; j< cols; j++){
			value += matrix[i][j] * x[j];
		}
		res.push_back(value);
	}
	return res;
}

Matrix Matrix::operator*(const Matrix &B) const { // Matrix * Matrix
	if (this->cols != B.rows ) throw(string("A.cols != B.rows"));
	Matrix res(B.cols, this->rows);

	for(size_t i=0; i< res.rows; i++)
		for(size_t j=0; j< res.cols; j++){
			double val = 0.;
			for (size_t k = 0; k < res.rows; k++)
				val += matrix[i][k] * B.matrix[k][j];
			res[i][j] = val;
		}
	return res;
}

Matrix Matrix::operator+(const Matrix & B)const{ // Matrix + Matrix
	if (this->cols != B.cols ) throw(string("A.cols != B.cols"));
	if (this->rows != B.rows ) throw(string("A.rows != B.rows"));

	Matrix res(cols, rows);
	for(size_t i=0; i< res.rows; i++)
		for(size_t j=0; j< res.cols; j++){
			res[i][j] = matrix[i][j] + B.matrix[i][j];
		}
	return res;
}

Vector& Matrix::operator[](const size_t i){
	return matrix[i];
}

void Matrix::info(const string &name)const{
	if (name != "none") cout << name << " = ";
	if (matrix.size() == 0 ) { cout << "[ ]" << endl; return;}

	size_t w = ( name.size()+3 < 13 ) ? 13 :  name.size() + 3;

	for(size_t i=0; i < rows; i++) {
		if (i > 0)
			for (size_t j = 0; j < w+3; j++)cout << " ";
		else
			for (size_t j = 0; j < w - name.size(); j++)cout << " ";

		// print a line
		cout << "( ";
		for (Vector::const_iterator it = matrix[i].begin(); it != matrix[i].end(); it++){
			if (*it >= 0. ) cout << "+";
			cout << std::scientific <<  *it << " " ;
	//		printf("%+.10E ", *it);
		}
		cout << ")" << endl;
	}
}

bool Matrix::isNullMatrix(){
	for (size_t i = 0; i < cols; i++){
		for (size_t j = 0; j < rows; j++){
 			if (fabs(matrix[i][j]) > 1E-12) return false;
		}
	}
	return true;
}
/** \name: ::invert()
 * Inverts *square matrices* using the algorithm by Faddejew-Leverrier.
 *
 * http://de.wikipedia.org/wiki/Algorithmus_von_Faddejew-Leverrier
 *
 * Can also compute :
 *  c[0] , ..., c[n]  : coefficients of the charakteristic polynomial
 *  (-1)^n * c[0]     : determinant of A
 *  Ainv              : inverse of A (sofern c[0] <> 0)
 */
void Matrix::invert(){
	if (rows != cols) throw myexception(EXCEPTION_ID + "The algorithm by Faddejew-Leverrier works only for squared matrices.");

	Matrix I(rows, rows);
	for (size_t i = 0; i < rows; i++) I.matrix[i][i] = 1.;

	Matrix *B = new Matrix[rows+2];
	double *c = new double[rows+1];

	c[rows] = 1.;
	for (size_t i = 0; i < rows; i++){
		B[i].assign(rows, cols, 0.);
		c[i] = 1.;
	}

	size_t &n = rows;
	for (size_t k = 1; k <= n; k++){
		B[k]     =   *this * B[k-1] + c[n-k+1] * I;
		c[n-k] = - 1./k * ( *this * B[k] ).trace();
	}

	B[n+1] = *this * B[n] + c[0] * I;
	if ( not B[n+1].isNullMatrix() ){
		cerr << "Error: Algorithm does not converge !!" << endl;
		B[n+1].info(" 0 != B[n+1]");
		return;
	}

	// compute inverse
	if ( c[0] != 0. ){
		*this = -1./c[0] * B[n];
	}
	else{
		printf("The matrix was singular !");
	}

	delete[] B;
	delete[] c;
	return;



}

double Matrix::trace(){
	double t = 0.;
	for (size_t i = 0; i < rows; i++) t+= matrix[i][i];
	return t;
}

void Matrix::transpose(){
	double t = 0.;

	for (size_t i = 0; i < rows; i++){
		for (size_t j = i+1; j < cols; j++){
			t 			 = matrix[i][j];
			matrix[i][j] = matrix[j][i];
			matrix[j][i] = t;
		}

	}

}

/** name: Matrix::Minor(size_t, size_t)
 * Compose the minor matrix by removing the i'th line and the j'th row.
 * @param i: line to remove
 * @param j: row to remove
 * @return minor matrix
 */
Matrix Matrix::Minor(size_t i, size_t j) const {

	Matrix M(*this);

	// remove complete line
	M.matrix.erase(M.matrix.begin()+i);
	M.rows--;
	for (size_t k = 0; k < M.rows; k++){
		M.matrix[k].erase(M.matrix[k].begin() + j);
	}
	M.cols--;

	return M;
}

