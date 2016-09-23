//		./Symbolic/Bytecode.hpp
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
#ifndef BYTECODE_HPP
#define BYTECODE_HPP

/** \file Symbolic/Bytecode.hpp
 * \brief Declaration of Bytecode-class
 *
 *	This file declares the Bytecode-class.
 *	Thereby the declaration of one method is dependent on a define-constant.
 */

#include "Node.hpp"
#include "Symbolic_Exception.hpp"

namespace Symbolic {

/** \class Symbolic::Bytecode
 *	\brief Generates bytecode for a function and enables to compute values.
 *
 *	This class represents the bytecode of a function that has been parsed and
 *	expressed as binary tree.<br>
 *	From such a binary tree is then the bytecode produced, either in the
 *	constructor or in the CreateCode-method.
 *	Via the Compute-mehod it is then possible to calculate function values.
 */
class Bytecode {
	public:
		Bytecode();
		Bytecode(Node const & tree);
		Bytecode(Bytecode const & other);
		~Bytecode();

		void CreateCode(Node const   tree);

		double Compute(double const x, double const y, double const z, double const t) const throw(BytecodeException);
		bool Exists(Opcode const op) const throw(BytecodeException);

		double	const *	GetStack()		const {return stack;};
		size_t			GetStacksize()	const {return stacksize;};
		Cell	const *	GetCode()		const {return code;};
		size_t			GetCodesize()	const {return codesize;};


#ifdef DEBUG
		void Info() const;
#endif

	private:
		double *stack;//!< Used to store an array which holds the values for the computation of the function.
		Cell *code;//!< Stores an array for the operations of the function.

		size_t stacksize;//!< Stores the size of the stack array.
		size_t codesize;//!< Stores the size of the code array.
};

}

#endif // BYTECODE_HPP
