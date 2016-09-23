//		./Symbolic/Parser.cpp
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
#ifndef PARSER_CPP
#define PARSER_CPP

/** \file Symbolic/Parser.cpp
 *	\brief Defines the methods of the Parser-class.
 */

#ifdef DEBUG
#include <iostream>
#endif

#include "Parser.hpp"

using namespace std;

namespace Symbolic {

/** \fn Parser::Parser(string, Parameter const * const)
 * \brief Constructor which takes the function as string.
 *
 *	This constructor initializes the members of the object. Also it creates the
 *	tree out of the function and passes them to the Bytecode-object which uses
 *	the tree to create the bytecode.<br>
 *
 *	If the tree-creation has finished, but the end of the string isn't reached,
 *	an exception is thrown.
 *
 * \param math: String which contains the function that should be parsed.
 * \param parameter: A pointer to a Parameter, which contains all the
 *				parameternames and their values, that could be in the string.<br>
 *				Default value is NULL, e.g. no parameters are used.
 */
Parser::Parser(string math, Parameter const * const parameter)
throw(ParserException, LexerException)
	:	root(), lexer(math, parameter), code() {
	lexer.Check();// Check for the first character which is of interest.
	Expression(root);// Create the tree from the string.

#ifdef DEBUG
	root.Info();
#endif

	// If the creation is finished but the string isn't at the end, is an error in the function, so throw an exception.
	if(lexer.GetSymbol() != SYM_END) throw ParserException(&(*(lexer.GetLastSym())), EXPECTED_EXPRESSION);

	// Create the bytecode from the tree.
	code.CreateCode(root);

#ifdef DEBUG
	code.Info();
#endif
}

/** \fn Parser::Parser(Parser const &)
 * \brief Copyconstructor.
 *
 *	This copyconstructor, creates a new Parser as a deep copy of an already
 *	existing Parser-object. Therefore the copy-contructors of the root members are
 *	used.<br>
 *	If DEBUG is defined, the Print-methods of the node and the bytecode are
 *	called.
 *
 * \param other: Reference to the parser that should be copied.
 */
Parser::Parser(Parser const & other)
	:	root(other.GetRoot()), lexer(other.GetLexer()), code(other.GetBytecode()) {
#ifdef DEBUG
	root.Info();
	code.Info();
#endif
}

/** name: Parser::Expression
 * \brief Checking parts separated by + or -.
 *
 *	This method is responsible for checking the parts of the function that are
 *	seperated by plus or minus.
 *	The subterms are checked by the Term-member.
 *
 * \param n: Reference to a node, where the tree is stored.
 */
void Parser::Expression(Node & n) throw(ParserException, LexerException) {
	Node t, t1, temp;
	Opcode op;

	Term(t);

	// As long as the last found symbol was + or - there are additional 'Expression's.
	while((lexer.GetSymbol() == SYM_PLUS) || (lexer.GetSymbol() == SYM_MINUS)) {
		op = lexer.GetOpcode();
		lexer.Check();
		Term(t1);

		temp.Set(op, t, t1);

		t.Copy(temp);
	}

	n.Copy(t);
}

/** name: Parser::Term
 * \brief Checking parts separated by *, / or %.
 *
 *	This method is responsible for checking the parts of the function that are
 *	seperated by *, / or % (times, divide, modulo).
 *	The subterms are checked by the Factor-member.
 *
 * \param n: Reference to a node, where the tree is stored.
 */
void Parser::Term(Node & n) throw(ParserException, LexerException) {
	Node t, t1, temp;
	Opcode op;

	Factor(t);

	// As long as the last symbol found was *, / or % there are additional 'Term's.
	while((lexer.GetSymbol() == SYM_TIMES) || (lexer.GetSymbol() == SYM_DIVIDES) || (lexer.GetSymbol() == SYM_MODULO)) {
		op = lexer.GetOpcode();
		lexer.Check();
		Factor(t1);
		temp.Set(op, t, t1);
		t.Copy(temp);
	}

	n.Copy(t);
}

/** name: Parser::Factor
 * \brief Checking exponents.
 *
 *	This method is responsible for checking the exponent-parts of the function.
 *	The subterms are checked by the Power-member.
 *
 * \param n: Reference to a node, where the tree is stored.
 */
void Parser::Factor(Node & n) throw(ParserException, LexerException) {
	Node t, t1;

	Power(t);

	// As long as the last symbol found was ^ there are additional 'Factor's.
	if(lexer.GetSymbol() == SYM_EXPONENTATION) {
		lexer.Check();
		Factor(t1);
		n.Set(OP_EXPONENTATION, t, t1);
	} else n.Copy(t);
}

/** name: Parser::Power
 * \brief Checking elementary parts.
 *
 *	This method is responsible for checking the 'elementary' parts of a
 *	function. As those are considered here variables, numbers, negative-
 *	operator, paranthesises and functions. Arguments of the latter one are
 *	considered to be 'Expression's.
 *
 * \param n: Reference to a node, where the tree is stored.
 */
void Parser::Power(Node & n) throw(ParserException, LexerException) {
	Node t;
	Node minus_one(OP_CONST, -1);

	Opcode op;

	switch(lexer.GetSymbol()) {

		case SYM_VARIABLE:
			t.Set(lexer.GetOpcode());
			lexer.Check();
			n.Copy(t);
			break;
		case SYM_FUNCTION:
			op= lexer.GetOpcode();
			lexer.Check();

			if(lexer.GetSymbol() != SYM_OPEN_PARENTHESIS) throw ParserException(&(*(lexer.GetLastSym())),EXPECTED_PARENTHESIS_AFTER_FUNCTIONNAME);
			else {lexer.Check();}

			Expression(t);

			if(lexer.GetSymbol() == SYM_CLOSE_PARENTHESIS) lexer.Check();
			else throw ParserException(&(*(lexer.GetLastSym())), EXPECTED_PARENTHESIS_AFTER_FUNCTIONPARAM);

			n.Set(op, t);
			break;
		case SYM_NUMBER:
			t.Set(OP_CONST, lexer.GetNumber());
			lexer.Check();
			n.Copy(t);
			break;
		case SYM_OPEN_PARENTHESIS:
			lexer.Check();
			Expression(t);

			if(lexer.GetSymbol() == SYM_CLOSE_PARENTHESIS) lexer.Check();
			else throw ParserException(&(*(lexer.GetLastSym())), EXPECTED_PARENTHESIS_AFTER_EXPRESSION);

			n.Copy(t);
			break;
		case SYM_MINUS:
			lexer.Check();
			Factor(t);
			n.Set(OP_TIMES, t, minus_one);
			break;
		default:
			throw ParserException(&(*(lexer.GetLastSym())), WRONG_SYMBOL);
			break;
	}
}

/** name: Parser::Compute
 * \brief Caclculation of functionvalues.
 *
 *	This function calculates the function-value for a given set of variables by
 *	using the Compute-method of the bytecode-object.
 *
 * \param x:
 * \param y:
 * \param z:
 * \param t: The set of variables for which to calculate the function value.
 * \return The value of the function, computed by the Compute-method of the
 *				bytecode-object.
 */
double Parser::Compute(double const x, double const y, double const z, double const t) const throw(BytecodeException) {
	return code.Compute(x, y, z, t);
}

#ifdef DEBUG
/** \fn void Parser::Info() const
 * \brief Get type of the function.
 *
 *	Checks from which variables (x,y,z,t) the function depends of and prints the
 *	result.
 */
void Parser::Info() const {
	string const math(lexer.GetExpression());
	string d("");

	if (code.Exists(OP_X)) 						d += "x";
	if (code.Exists(OP_Y) and d.size()) d+=","; d += "y";
	if (code.Exists(OP_Z) and d.size()) d+=","; d += "z";
	if (code.Exists(OP_T) and d.size()) d+=","; d += "t";

	cout << " Symbolic expression : f("<<d<<") = "<< math << endl;
	return;
};
#endif // DEBUG

}

#endif // PARSER_CPP
