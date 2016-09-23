
#include "units.hpp"

double mylibs::units::SI_Prefix_Factor(mystring unit){
	if (unit == "a.u.") return 1.;
	else
	if (unit.startswith("Y")) return 1.E24;
	else
	if (unit.startswith("Z")) return 1.E21;
	else
	if (unit.startswith("E")) return 1.E18;
	else
	if (unit.startswith("P")) return 1.E15;
	else
	if (unit.startswith("T")) return 1.E12;
	else
	if (unit.startswith("G")) return 1.E9;
	else
	if (unit.startswith("M")) return 1.E6;
	else
	if (unit.startswith("k")) return 1.E3;
	else
	if (unit.startswith("h")) return 1.E2;
	else
	if (unit.startswith("da")) return 1.E1;
//		else
//		if (unit.startswith("")) return 1.E0;
	else
	if (unit.startswith("d")) return 1.E-1;
	else
	if (unit.startswith("c")) return 1.E-2;
	else
	if (unit.startswith("m")) return 1.E-3;
	else
	if (unit.startswith("u")) return 1.E-6;
	else
	if (unit.startswith("n")) return 1.E-9;
	else
	if (unit.startswith("p")) return 1.E-12;
	else
	if (unit.startswith("f")) return 1.E-15;
	else
	if (unit.startswith("a")) return 1.E-18;
	else
	if (unit.startswith("z")) return 1.E-21;
	else
	if (unit.startswith("y")) return 1.E-24;

	return 1.;
}

/**
 * name: mylibs::units::SI_Prefix_Factor
 *
 * Computes the facor that is needed to convert the source unit to the
 * target unit, e.g. source_unit is cm and target_unit is mm. Then the
 * factor schould be 0.1.
 * @param source_unit: Unit to be converted from.
 * @param target_unit: Unit to be converted to.
 * @return 	Factor that has to be multiplied on allvalues to convert the
 * 			units correctly.
 *
 */
double mylibs::units::SI_Prefix_Factor(const mystring source_unit, const mystring target_unit){
	double S = mylibs::units::SI_Prefix_Factor(source_unit);
	double T = mylibs::units::SI_Prefix_Factor(target_unit);

	return T/S;
}

const char* mylibs::units::SI_Prefix_Name(mystring unit){
	if (unit == "a.u.") return "arbitrary unit";
	else
	if (unit.startswith("Y")) return "yotta";
	else
	if (unit.startswith("Z")) return "zetta";
	else
	if (unit.startswith("E")) return "exa";
	else
	if (unit.startswith("P")) return "peta";
	else
	if (unit.startswith("T")) return "tera";
	else
	if (unit.startswith("G")) return "giga";
	else
	if (unit.startswith("M")) return "mega";
	else
	if (unit.startswith("k")) return "kilo";
	else
	if (unit.startswith("h")) return "hecto";
	else
	if (unit.startswith("da")) return "deca";
//		else
//		if (unit.startswith("")) return "";
	else
	if (unit.startswith("d")) return "deci";
	else
	if (unit.startswith("c")) return "centi";
	else
	if (unit.startswith("m")) return "milli";
	else
	if (unit.startswith("u")) return "micro";
	else
	if (unit.startswith("n")) return "nano";
	else
	if (unit.startswith("p")) return "pico";
	else
	if (unit.startswith("f")) return "femto";
	else
	if (unit.startswith("a")) return "atto";
	else
	if (unit.startswith("z")) return "zepto";
	else
	if (unit.startswith("y")) return "yocto";

	return "";
}

mystring mylibs::units::SI_Prefix_Strip(const mystring unit){
	if (unit == "a.u.") return unit;
	else
	if (unit.startswith("Y")) return unit(1);
	else
	if (unit.startswith("Z")) return unit(1);
	else
	if (unit.startswith("E")) return unit(1);
	else
	if (unit.startswith("P")) return unit(1);
	else
	if (unit.startswith("T")) return unit(1);
	else
	if (unit.startswith("G")) return unit(1);
	else
	if (unit.startswith("M")) return unit(1);
	else
	if (unit.startswith("k")) return unit(1);
	else
	if (unit.startswith("h")) return unit(1);
	else
	if (unit.startswith("da")) return unit(2);
//		else
//		if (unit.startswith("")) return "";
	else
	if (unit.startswith("d")) return unit(1);
	else
	if (unit.startswith("c")) return unit(1);
	else
	if (unit.startswith("m")) return unit(1);
	else
	if (unit.startswith("u")) return unit(1);
	else
	if (unit.startswith("n")) return unit(1);
	else
	if (unit.startswith("p")) return unit(1);
	else
	if (unit.startswith("f")) return unit(1);
	else
	if (unit.startswith("a")) return unit(1);
	else
	if (unit.startswith("z")) return unit(1);
	else
	if (unit.startswith("y")) return unit(1);

	return unit;
}
