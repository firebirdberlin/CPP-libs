//		./Symbolic/Symbolic_Exception.hpp
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
#ifndef SYMBOLIC_EXCEPTION_HPP
#define SYMBOLIC_EXCEPTION_HPP

/** \file Symbolic/Symbolic_Exception.hpp
 *	\brief Declaration of exception classes and enums.
 *
 *	This file declares the exception classes for the Lexer, the Bytecode and
 *	the Parser together with the corresponding enums.
 */

#include <string>

namespace Symbolic {

/** \enum Symbolic::Lexer_exceptions
 *	Exception-id's of the possible types of exceptions of the Lexer-class.
 */
enum Lexer_exceptions {UNKNOWN_OPERATOR_OR_SYMBOL, UNKNOWN_IDENTIFIER};

/** \enum Symbolic::Bytecode_exceptions
 *	Exception-id's of the possible types of exceptions of the Bytecode-class.
 */
enum Bytecode_exceptions {OPCODE_BAD, OPCODE_UNKNOWN};

/** \enum Symbolic::Parser_exceptions
 *	Exception-id's of the possible types of exceptions of the Parser-class.
 */
enum Parser_exceptions {EXPECTED_EXPRESSION, EXPECTED_PARENTHESIS_AFTER_FUNCTIONNAME,
                        EXPECTED_PARENTHESIS_AFTER_FUNCTIONPARAM,
                        EXPECTED_PARENTHESIS_AFTER_EXPRESSION,
                        WRONG_SYMBOL
                       };

/** \class Symbolic::LexerException
 * \brief Excpetions of the Lexer-object.
 *
 *	This class is derived from the std::exception-class and created for
 *	handling the exceptions of the Lexer-class.<br>
 *	It redefines the what-method, to give a possibility to inform the user
 *	what happened. Also it contains an id, to further specify the error
 *	which occours.
 */
class LexerException : public std::exception {
	public:
		LexerException(std::string const & str, Lexer_exceptions const id);
		LexerException(Lexer_exceptions const id);

		virtual ~LexerException() throw() {};

		Lexer_exceptions GetID() const {return ex_id;};
		std::string const & GetWhat() const {return s;};

		virtual const char* what() const throw() {return s.c_str();};

	protected:
		std::string s;//!< Stores a message for the user. Additional code may be given at the lines where the exception is thrown to the constructor.

		Lexer_exceptions ex_id;//!< An error-id, which further specify the exception.

		void SetString(Lexer_exceptions const id);
	private:
};

/** \class Symbolic::NodeException
 * \brief Excpetions of the Node-object.
 *
 *	This class is derived from the std::exception-class and created for handling
 *	the exceptions of the Node-class.<br>
 *	It redefines the what-method, to give a possibility to inform the user what
 *	happened. Also it contains an id, to further specify the error which occours.
 */
class NodeException : public std::exception {
	public:
		enum type {CANNOT_HANDLE_THIS_OPCODE, CANNOT_SOLVE_THIS_CONFIGURATION,
		           OPCODE_NOT_FOUND
		          };

		NodeException(std::string const & str, type const id);
		NodeException(type const id);

		virtual ~NodeException() throw() {};

		type GetID() const {return ex_id;};
		std::string const & GetWhat() const {return s;};

		virtual const char* what() const throw() {return s.c_str();};

	protected:
		std::string s;//!< Stores a message for the user. Additional code may be given at the lines where the exception is thrown to the constructor.

		type ex_id;//!< An error-id, which further specify the exception.

		void SetString();
	private:
};

/** \class Symbolic::BytecodeException
 * \brief Excpetions of the Bytecode-object.
 *
 *	This class is derived from the std::exception-class and created for handling
 *	the exceptions of the Bytecode-class.<br>
 *	It redefines the what-method, to give a possibility to inform the user what
 *	happened. Also it contains an id, to further specify the error which occours.
 */
class BytecodeException : public std::exception {
	public:
		BytecodeException(std::string const & str, Bytecode_exceptions const id);
		BytecodeException(Bytecode_exceptions const id);

		virtual ~BytecodeException() throw() {};

		Bytecode_exceptions GetID() const {return ex_id;};
		std::string const & GetWhat() const {return s;};

		virtual const char* what() const throw() {return s.c_str();};

	protected:
		std::string s;//!< Stores a message for the user. Additional code may be given at the lines where the exception is thrown to the constructor.

		Bytecode_exceptions ex_id;//!< An error-id, which further specify the exception.

		void SetString(Bytecode_exceptions id);
	private:
};

/** \class Symbolic::ParserException
 * \brief Excpetions of the Parser-object.
 *
 *	This class is derived from the std::exception-class and created for handling
 *	the exceptions of the Parser-class.<br>
 *	It redefines the what-method, to give a possibility to inform the user what
 *	happened. Also it contains an id, to further specify the error which occours.
 */
class ParserException : public std::exception {
	public:
		ParserException(std::string const & str, Parser_exceptions const id);
		ParserException(Parser_exceptions const id);

		virtual ~ParserException() throw() {};

		Parser_exceptions GetID() const {return ex_id;};
		std::string const & GetWhat() const {return s;};

		virtual const char* what() const throw() {return s.c_str();};

	protected:
		std::string s;//!< Stores a message for the user. Additional code may be given at the lines where the exception is thrown to the constructor.

		Parser_exceptions ex_id;//!< An error-id, which further specify the exception.

		void SetString(Parser_exceptions id);
	private:
};

}

#endif // SYMBOLIC_EXCEPTION_HPP
