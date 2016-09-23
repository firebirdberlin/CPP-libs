//		./Symbolic/Parser.hpp
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
#ifndef PARSER_HPP
#define PARSER_HPP

/** \file Symbolic/Parser.hpp
 *	\brief Declaration of Parser-class.
 *
 *	This file declares the Parser-class.
 */

#include "Bytecode.hpp"
#include "Lexer.hpp"
#include "Symbolic_Exception.hpp"
#include <iostream>
#include <limits>

namespace Symbolic {

/** \class Parser
 *	\brief Class for parsing a string to a function.
 *
 *	This class is designed for creating a executable function from a string.
 *
 * \section sec_usage usage
 *	Therefore the string has to be passed to the constructor, the creation of the
 *	bytecode is then done automatically.<br>
 *	The function can be used by calling the method Compute(), whith
 *	the variables of the function as arguments.<br>
 *
 *	\section sec_req_lim requirements and limits
 *	The function (in the string) can neither have any form nor used all other
 *	functions (meant are mathematic functions as sine, cosine, logarithm,..).<br>
 *
 *	The function that should be parsed can not depend on more then four variables.
 *	These have to be called x, y, z and t (note that x corresponds to the first
 *	argument of the Compute()-method, y to the second and so on).<br>
 *
 *	Up to now you can use the four basic arithmetic operations (plus +, minus -,
 *	times * and divide /), the modulo-operation (%) and exponentiation (^).
 *
 *	Mathematical functions that can be used are:
 *	- square root (sqrt)
 *	- exponential (exp)
 *	- natural logarythm (ln)
 *	- sine (sin) and arcsine (asin)
 *	- cosine (cos) and arccosine (acos)
 *	- tangens (tan) and arctan (atan)
 *	- hyperbolic sine (sinh) and area hyperbolic sine (asinh)
 *	- hyperbolic cosine (cosh) and area hyperbolic cosine (acosh)
 *	- hyperbolic tangens (tanh) and area hyperbolic tangens (atanh)
 *	- heavyside-function (H)
 *
 *	Besides of these predefined symbols you can define a set of Parameter.
 *	These have to be given to the constructor. This gives you the possibility to
 *	allow the writer of the function, to include some constants which are
 *	provided at runtime, like modelparameter or the size/dimension of the space.
 *
 *	The function parameters have to be enclosed in paranthesises ((), {} or []).<br>
 *	Whitespace should make no difference.
 */
/* \section sec_draw_tree_from_string Howto draw the tree that is created from the given string
 *	To draw a tree from a given function (as string) use the following
 *	algorithm:
 *	- 1. Divide the function into different parts seperated by the outmost
 *		(the ones that will be executed at last) + and - (there might be none).
 *	- 2. Divide the subgroups according to * and /.
 *	- 3. Now further subgroup according to ^.
 *	- 4. Now within each subgroup the leftmost constant/variable/function is the
 *		starting node.
 *	- 5. According to the type perform the substep
 *		- constant/variable, the operator to the right is a node at the top
 *			right.
 *		- function, the argument is a node at the bottom right. The subtree
 *			could be drawn using this algorhythm for the argument.
 *	- 6. For operations (+ - * / % ^) the node at the bottom right is the
 *		function/variable/constant at the right of the operator (for functions
 *		you have to draw a subtree at the bottom right for its argument).
 *	- 7. The next operator is at the top right of the last operator in this
 *		(sub)group.
 *	- 8. Repeat steps 6 and 7 until the (sub)group is done, then go the the next
 *		(sub)group within the same level.
 *	- 9. If all (sub)groups of a level are ready, steps 6 and 7 are used to
 *		connect the (sub)groups.
 * \todo Example
 */
/*
 *	\section sec_improvements Improvements not implemented yet
 *	There are many things which could be improved, but these are considered to
 *	be (more or less) important and usefull:
 *	- Adding functions that could be used.
 *	- Swaping of subtrees to enable further precalculations.
 *	- Predefined constants (like \f$ \pi \f$ and \f$ e \f$).
 */
class Parser {
	public:
		Parser(string math, Parameter const * const parameter = NULL) throw(ParserException, LexerException);
		Parser(Parser const & other);


		double Compute(	double x,
		                double y=numeric_limits<double>::max(),
		                double z=numeric_limits<double>::max(),
		                double t=numeric_limits<double>::max()
		              ) const throw(BytecodeException);

#ifdef DEBUG
		void   Info() const;
#endif
		Node		const & GetRoot()		const {return root;};
		Bytecode	const & GetBytecode()	const {return code;};
		Lexer		const & GetLexer()		const {return lexer;};


	private:
		Node root;//!< The function as tree
		Lexer lexer;//!< For extracting the mathematic symbols and operations of the string.
		Bytecode code;//!< Object for the generated code.

		void Expression(Node & n)	throw(ParserException, LexerException);
		void Term(Node & n)			throw(ParserException, LexerException);
		void Factor(Node & n)		throw(ParserException, LexerException);
		void Power(Node & n)		throw(ParserException, LexerException);
};

}

#endif // PARSER_HPP
