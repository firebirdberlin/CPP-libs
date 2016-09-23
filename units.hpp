
#ifndef __UNITS_HPP
#define __UNITS_HPP

#include "mystring.hpp"

namespace mylibs{
namespace units {
	double 	SI_Prefix_Factor(const mystring unit);
	double 	SI_Prefix_Factor(const mystring source_unit, const mystring target_unit);
	const char* SI_Prefix_Name(const mystring unit);
	mystring 	SI_Prefix_Strip(const mystring unit);
}
}

#endif
