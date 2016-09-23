#include <mylibs/cmdline.hpp>
#include <mylibs/myinifiles.hpp>

using namespace std;
using namespace mylibs;
/**\fn int main(int argc, char** argv)
 * \brief Testprogram for myIniFiles.
 *
 * This is a test program for the class myIniFiles.
 * It tests the all functionalities.
 *
 * \todo Check for the results.
 */
int main(int argc, char** argv){
	// Constructor.
	myIniFiles ini(argc, argv, true);

	// Other constants and variables that are needed.
	const string sectionname("section1");
	int const myints[4] = {0, 1, 2, 5};
	mystring const mystrings[5] = {mystring("Hello "), mystring("world, "), mystring("how "), mystring("are "), mystring("you?")};
	vector<int> const	int_list_to_write(myints, myints + sizeof(myints) / sizeof(int) );
	vector<int>			int_list_to_read(4);
	vector<mystring>	string_list_to_write(mystrings, mystrings + sizeof(mystrings) / sizeof(mystring) );
	std::string temp;
	vector<double> allowed_doubles(2, 1.0);
	vector<int> allowed_ints(2, 0);
	vector<string> allowed_strings(2, "test");

	{// Registration of parameters.
		cmdline::section("Registration of parameters");
		ini.register_param("test1", "a", "test parameter", true);
		ini.register_param("list1", "l", "test string list", true);
		ini.register_param("test2", "b", "test flag, depends upon test1", false, "test1");
		ini.register_param("list2", NULL, "test list", "hallo");

		ini.register_param<double>("double",	"d", "double parameter", 1.0,		allowed_doubles);
		ini.register_param<int>("int",			"i", "int parameter", 0,			allowed_ints);
		ini.register_param<string>("string",	"s", "string parameter", "default",	allowed_strings);

		// Setting a section.
		ini.set_active_section(sectionname.c_str());
		if(sectionname != ini.get_active_section()) {
			std::cout<<__LINE__<<": Error, section is set correctly.\n";
			return 1;
		}

		ini.register_param<double>("double", NULL, "double parameter", 0.0, -10.0, 11.0);
		ini.register_param<size_t>("size_t", NULL, "size_t parameter",   0,     0, 1000);
		ini.register_flag("flag", "g");

		// Unsetting a section.
		ini.unset_active_section();
		if(string("") != ini.get_active_section()) {
			std::cout<<__LINE__<<": Error, section is not unset.\n";
			return 1;
		}
	}

	// Generating of a template file.
	cmdline::section("Generate a template file.");
	ini.gen_ini_template("test_template.ini");

	{// Writing values and vectors.
		cmdline::section("Writing values and vectors");
		ini.write<double>("double", 3.141592);
		ini.writeStringListv("test1", string_list_to_write);
		ini.writeVector<int>("list2", int_list_to_write);
	}


	{// Read values and vectors.
		cmdline::section("Read values and vectors.");
		std::cout<<"test1: "<<ini.read("test1", mystring("t"))<<"\n";
		ini.readVector("list2", int_list_to_read);

		std::cout<<"double: "<<ini.readdouble("double")<<"\n";
		std::cout<<"int: "<<ini.readint("int", 1000)<<"\n";
		std::cout<<"string: "<<ini.readstring("string", string("goodbye"))<<"\n";
	}

	cmdline::section("Testing totext");
	std::cout<<ini.totext<int>("integer", 500, "class")<<"\n";

	{// Check for registration and existence of keys.
		cmdline::section("Check for registration and existence of keys.");
		ini.exists("list2");

		ini.set_active_section(sectionname.c_str());

		ini.exists("size_t");

		ini.param_registered("flag");
		ini.param_registered("test1");
		ini.param_registered("::test1");
		ini.param_registered(sectionname, "test1");

		ini.unset_active_section();

		ini.param_exists(string("double"));

		ini.param_registered("double");
		ini.param_registered("test1");
		ini.param_registered("::test1");
		ini.param_registered(sectionname + string("::flag"));
		ini.param_registered(sectionname, "test1");
		ini.param_registered(sectionname, "flag");
	}

	{// Check for type, if arguments needed, and getting of parameters.
		cmdline::section("Check for type, if arguments needed, and getting of parameters");
		ini.param_needs_arg("string");
		ini.param_needs_arg("test2");
		ini.param_needs_arg(sectionname, "flag");
		if(cmdline::parameter::keyvalue != ini.param_get_type("test1"))			return 1;
		if(cmdline::parameter::flag != ini.param_get_type(sectionname, "flag"))	return 1;

		ini.get_param("test1");
	}

	cmdline::section("Check parameters");
	ini.check();

	cmdline::section("Print for inifile in commandline");
	std::cout<<"Inifilename, given via commandline: "<<ini.check_inifile()<<"\n";

	//// Commented, because it aborts the execution of the program.
	//cmdline::section("Print help");
	//ini.print_help("test2");

	//// Commented, because it aborts the execution of the program.
	//cmdline::section("Free option chars");
	//ini.print_free_opt_chars();

	cmdline::section("Testing param_non_opt_args");
	std::cout<<"Non flags in commandline: "<<ini.param_non_opt_args()<<"\n";

	cmdline::section("Getting argument");
	ini.param_getarg(temp, "test1");
	int_list_to_read.clear();
	ini.param_getarg(int_list_to_read, "list2");

	cmdline::section("Testing save");
	ini.save("test_final.ini");

	cmdline::ok();
	cmdline::newline();
	return 0;
};
