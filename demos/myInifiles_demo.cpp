//      initest.cpp
//
//      Copyright 2009 Stefan Fruhner <jesus@computer>
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
#include <vector>
#include <mylibs/myinifiles.hpp>

int main(int argc, char** argv)
{
	myIniFiles ini(argc, argv, true);
	vector<mystring> l1, l2;
	ini.register_param("help2",  "h");
	ini.register_param("help3", 0, "this help", false) ;

//	int help2 = ini.read("help2", 42);
//	ini.register_param("test" , NULL);

//	ini.register_param("test34", "t", "test34");
//	ini.register_param("l1", "m", "list1");
//	ini.register_param("with_underscore", "u", "Test if underscores work.");
//	ini.register_param("with", "v", "Test if underscores work.");
//	ini.register_param("l2", "n", "list2");

//	ini.register_param("test2", "c", "test with dep", true);
////	ini.register_param("test3", "d", "test with dep", true, "help2");
////	ini.register_param("test5", 0);
//	// You can set section values as follows
//	ini.set_active_section("Model");
//	ini.register_param("test",  0, "test with dep");
//	ini.register_param("test1", 0, "test with dep", true, "test");
//	ini.register_param("test2", 0, "test with dep");
//	ini.register_param("test3", 0, "test redaing sections");
//	ini.register_param("test5", 0);

//	// ... or using the following style
//	ini.set_active_section("Solver");
//	ini.register_param("test",  0, "test with dep");
//	ini.register_param("test1", 0, "test with dep");
//	ini.register_param("test2", 0, "test with dep");
	ini.unset_active_section();
//	ini.register_param("test4",  0, "test with dep");

	ini.register_param("test_b" , NULL, "test als string", "hallo");

	//! attention if you forget to call ini.check_params() then this is done at the first ini.read(...)
	ini.check();

	ini.exists("test_b");
	string test = ini.read("test_b", string("1245"));

//	ini.gen_ini_template("demo.i.template");


	//int test21 = ini.read("test", 42);

//	int test = ini.read("test34", 0);
//	//float ftest = ini.read("test", 0.5);

//	//int test2 = ini.read("test3", 0);
//	if ( ini.exists("[Model]") ) cout << "found section 'Model'" << endl;
//	ini.set_active_section("Model");

//	double t = ini.read("test", .9);

//	if ( ini.exists("test3") ) cout << "Model::test3 exists" << endl;
//	int test4 = ini.read("test3", 0);
//	ini.unset_active_section();

//	mystring res = ini.totext(vardesc(test));
//	cout << res << endl;

//	//ini.write("test34", 22);
//	ini.save();

//	ini.readStringListv("l1", l1);
//	for (size_t i = 0; i < l1.size(); i++){
//		cout << l1[i] << endl;
//	}

//	if (ini.exists("with_underscore")) cout << "with_underscore exists" << endl;
//	if (ini.exists("with")) cout << "with exists" << endl;
//	mystring wu = ini.read("with_underscore", mystring());
//	mystring wo = ini.read("with", mystring());

////	ini.writeStringListv("l1", l1);
//	if  ( ini.exists("{l2}") )
//		ini.readStringListv("l2", l2);
//	for (size_t i = 0; i < l2.size(); i++){
//		cout << l2[i] << endl;
//	}
//	ini.unset_active_section();
////	ini.write("test", 37);
//	ini.write("test2", 234);

//	ini.set_active_section("Model");
//	ini.write("test2", 237);
//	ini.write("test5", 237);
//	ini.unset_active_section();

////	l1.push_back(mystring("1"));
////	l1.push_back(mystring("2"));
////	l1.push_back(mystring("3"));

//	ini.writeVector("l1", l1);
	ini.save();



//	int test2 = ini.read("test2", 0);
//	cout << test2<< endl;
//	ini.save();

	cout << " -Extracting doubles from '{1, 2 , 3},crap{45.5,6,7,8}' " << endl;

	mystring numbers("{1, 2 , 3},crap{45.5,6,7,8}");
	vector<double> vi;
	numbers.extract(vi);

	cout << "   Found " << vi.size() << " items :" << endl;
	for (vector<double>::iterator it = vi.begin(); it != vi.end(); ++it){
		cout << "    " << *it << endl;
	}

	return 0;
}
