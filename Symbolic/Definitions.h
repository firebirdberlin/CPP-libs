//		./Symbolic/Definitions.h
//
//		Copyright 2011 Fabian Paul <fab@itp.tu-berlin.de>
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
#ifndef DEFINITIONS_H
#define DEFINITIONS_H

/** \file Symbolic/Definitions.h
 * \brief Contains public enums, constants and typdefs of the Symbolic-library.
 *
 * This file contains the public enums of the Symbolic-library together with the
 * constants and typedefs.
 */

#include <map>
#include <string>

namespace Symbolic {

/** \enum Symbolic::Opcode
 *	\brief Operationcode
 *
 *	This enum defines the constants for the different operations of the nodes.
 */
enum Opcode {OP_PLUS /* 0 */, OP_MINUS /* 1 */, OP_TIMES /* 2 */,
             OP_DIVIDES /* 3 */, OP_MODULO /* 4 */,  OP_EXPONENTATION /* 5 */,
             OP_SQRT /* 6 */, OP_EXP /* 7 */, OP_LN /* 8 */, OP_SIN /* 9 */,
             OP_COS /* 10 */, OP_TAN /* 11 */, OP_SINH /* 12 */, OP_COSH /* 13 */,
             OP_TANH /* 14 */, OP_ASIN /* 15 */, OP_ACOS /* 16 */,
             OP_ATAN /* 17 */, OP_ASINH /* 18 */, OP_ACOSH /* 19 */,
             OP_ATANH /* 20 */, OP_HEAVISIDE /* 21 */, OP_CONST /* 22 */,
             OP_X /* 23 */, OP_Y /* 24 */, OP_Z /* 25 */, OP_T /* 26 */,
             OP_BAD /* 27 */, OP_END /* 28 */
            };

/** \enum Symbolic::Symbol
 *	\brief Symbolconstants for the Lexer.
 *
 *	These enum-constants are used by the Lexer-class, to store the meaning of
 *	the last symbol that was found.
 */
enum Symbol {SYM_PLUS /* 0 */, SYM_MINUS /* 1 */, SYM_TIMES /* 2 */,
             SYM_DIVIDES /* 3 */, SYM_MODULO /* 4 */,  SYM_EXPONENTATION /* 5 */,
             SYM_OPEN_PARENTHESIS /* 6 */, SYM_CLOSE_PARENTHESIS /* 7 */,
             SYM_SEMICOLON /* 8 */, SYM_FUNCTION /* 9 */, SYM_VARIABLE /* 10 */,
             SYM_NUMBER /* 11 */, SYM_BAD /* 12 */, SYM_END /* 13 */
            };

/* Some type definitions:
 */
struct Definition { Symbol sym; Opcode code; };
union Cell { Opcode opcode; double number; };

typedef std::pair<std::string, double> ParameterPair;
typedef std::map<std::string, double> Parameter;
typedef std::map<std::string, Definition> Literal;

/* constants for initialization.
 */
Definition const Def_var_x = {SYM_VARIABLE, OP_X};
Definition const Def_var_y = {SYM_VARIABLE, OP_Y};
Definition const Def_var_z = {SYM_VARIABLE, OP_Z};
Definition const Def_var_t = {SYM_VARIABLE, OP_T};
Definition const Def_func_sqrt = {SYM_FUNCTION, OP_SQRT};
Definition const Def_func_exp = {SYM_FUNCTION, OP_EXP};
Definition const Def_func_ln = {SYM_FUNCTION, OP_LN};
Definition const Def_func_sin = {SYM_FUNCTION, OP_SIN};
Definition const Def_func_cos = {SYM_FUNCTION, OP_COS};
Definition const Def_func_tan = {SYM_FUNCTION, OP_TAN};
Definition const Def_func_sinh = {SYM_FUNCTION, OP_SINH};
Definition const Def_func_cosh = {SYM_FUNCTION, OP_COSH};
Definition const Def_func_tanh = {SYM_FUNCTION, OP_TANH};
Definition const Def_func_asin = {SYM_FUNCTION, OP_ASIN};
Definition const Def_func_acos	= {SYM_FUNCTION, OP_ACOS};
Definition const Def_func_atan	= {SYM_FUNCTION, OP_ATAN};
Definition const Def_func_asinh	= {SYM_FUNCTION, OP_ASINH};
Definition const Def_func_acosh	= {SYM_FUNCTION, OP_ACOSH};
Definition const Def_func_atanh	= {SYM_FUNCTION, OP_ATANH};
Definition const Def_func_heavy	= {SYM_FUNCTION, OP_HEAVISIDE};

/** \attention The initialization with the Definition causes problems dependend
 *		on the used flags (up to not compiling).
 */
const Literal::value_type raw_literals[] = {
	Literal::value_type("x",	Def_var_x),
	Literal::value_type("y",	Def_var_y),
	Literal::value_type("z",	Def_var_z),
	Literal::value_type("t",	Def_var_t),
	Literal::value_type("sqrt",	Def_func_sqrt),
	Literal::value_type("exp",	Def_func_exp),
	Literal::value_type("ln",	Def_func_ln),
	Literal::value_type("sin",	Def_func_sin),
	Literal::value_type("cos",	Def_func_cos),
	Literal::value_type("tan",	Def_func_tan),
	Literal::value_type("sinh",	Def_func_sinh),
	Literal::value_type("cosh",	Def_func_cosh),
	Literal::value_type("tanh",	Def_func_tanh),
	Literal::value_type("asin",	Def_func_asin),
	Literal::value_type("acos",	Def_func_acos),
	Literal::value_type("atan",	Def_func_atan),
	Literal::value_type("asinh",Def_func_asinh),
	Literal::value_type("acosh",Def_func_acosh),
	Literal::value_type("atanh",Def_func_atanh),
	Literal::value_type("H",	Def_func_heavy)
};

}

#endif // DEFINITIONS_H
