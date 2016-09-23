#include <mylibs/myinifiles.hpp>
using namespace std;

int main(int argc, char** argv){

	myIniFiles ini(argc, argv, true);


	ini.register_param("param",  "p", "This is a demo parameter. This help message appears, when the user runs $./test -?");

	ini.check();


	int param = ini.read("param", 0); //< 0 is a default value, that is used, when the user supplied no value for "param"

	cout << param << endl;

	ini.save();

	return 0;
};
